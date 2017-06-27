#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

#include <llvm/Assembly/PrintModulePass.h> // createPrintModulePass

#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TypeBuilder.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/Host.h>

#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/MC/MCStreamer.h>

#include <clang/Frontend/CodeGenOptions.h>
#include <clang/AST/RecordLayout.h>
#include <clang/CodeGen/ModuleBuilder.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/MapUtility.h>
#include <digi/System/MemoryDevices.h>
#include <digi/System/Log.h>
#include <digi/Scene/SceneFile.h>
#include <digi/Engine/ParameterType.h>
#include <digi/EngineVM/Compiler.h>
#include <digi/EngineVM/GetStructs.h>

#include "generateSceneOpenGL.h"
#include "generateTextureOpenGL.h"
#include "writeForMC.h"


namespace digi {

namespace
{
	Pointer<CompileResult> compile(Compiler& compiler, const std::string& inputCode, clang::CodeGenerator* astConsumer,
		bool isWin32bit)
	{
		#ifndef NDEBUG
			Pointer<IODevice> file = File::create("input.cpp");
			file->write(getData(inputCode), inputCode.size());
			file->close();
		#endif

		// compile
		Pointer<CompileResult> compileResult = compiler.compileFromString(inputCode, astConsumer);
		if (compileResult == null)
			return null;
	
		// get compiled module (leaves ownership at astConsumer)
		llvm::Module* module = astConsumer->GetModule();
		if (module == NULL)
			return null;
		
		// set calling convention for opengl functions on windows 32 bit
		if (isWin32bit)
			setStdCall(module);
		
		// implement convert functions
		implementConvertFunctions(module);
	
		// create pass manager and add passes
		llvm::PassManager passManager;

		// optimization
		compileResult->addPasses(*module, passManager, true);
	
		// run all passes
		passManager.run(*module);

		return compileResult;
	}

	bool generateELF(llvm::TargetMachine* targetMachine, llvm::Module* module, std::string& elf)
	{
		/*
		// generate asm file
		#ifndef NDEBUG
		{			
			llvm::PassManager passManager;

			passManager.add(new llvm::TargetData(*targetMachine->getTargetData()));

			std::string error;
			llvm::raw_fd_ostream asmStream("output.asm", error);
			llvm::formatted_raw_ostream fAsmStream(asmStream);
			if (targetMachine->addPassesToEmitFile(passManager, fAsmStream, llvm::TargetMachine::CGFT_AssemblyFile, llvm::CodeGenOpt::Default)) // note: None leaves calls to _alloca in output
			{

			}

			passManager.run(*module);
		}		
		#endif
		*/
		
		// generate elf into string
		{
			llvm::PassManager passManager;

			passManager.add(new llvm::TargetData(*targetMachine->getTargetData()));

			llvm::raw_string_ostream elfStream(elf);
			llvm::formatted_raw_ostream fElfStream(elfStream);
			if (targetMachine->addPassesToEmitFile(passManager, fElfStream, llvm::TargetMachine::CGFT_ObjectFile, llvm::CodeGenOpt::Default))
			{
				// error
			}

			passManager.run(*module);
		}
		return !elf.empty();
	}

	struct ExternalRel
	{
		StringRef name;
		size_t offset;

		ExternalRel(StringRef name, size_t offset)
			: name(name), offset(offset) {}
	};

	struct InternalRel
	{
		enum Type
		{
			R_32, // 32 bit S + A
			R_64, // 64 bit S + A
		};
		
		size_t offset;
		Type type;
		
		InternalRel(size_t offset, Type type)
			: offset(offset), type(type) {}
	};

	// relocator for 32 bit x86
	struct RelocatorX86
	{
		enum RelType
		{
			R_X86_32    =  1, // 32 bit S + A
			R_X86_PC32  =  2, // 32 bit S + A - P
		};

		template <typename Rel>
		void rela2rel(const Rel& rel, uint8_t* code)
		{
			// convert rela to rel by writing the addend into the code
			
			// offset of relocation in code
			uint32_t offset = rel.r_offset;
			
			int type = rel.getType();
			switch (type)
			{
			case R_X86_32:
			case R_X86_PC32:
				{
					int32_t a = rel.getAddend32LE(code);
					writeLE<uint32_t>(a, &code[offset]);
				}
				break;
			default:
				dError("unsupported relocation type " << type);
			}
		}

		template <typename Rel>
		int fixupSection(const Rel& rel, uint8_t* code, size_t codeOffset, size_t sectionOffset)
		{
			// code segment and data segments are put into the same buffer. this modifies relocations accordingly
			
			// offset of relocation in code
			uint32_t offset = rel.r_offset;

			int type = rel.getType();
			switch (type)
			{
			case R_X86_32:
				{
					// add offset of data section inside buffer to addend (i.e. access to data)
					int32_t s = int32_t(sectionOffset);
					int32_t a = rel.getAddend32LE(code);
					writeLE<uint32_t>(s + a, &code[offset]);
				}
				
				// relocation needs to be stored and done at loading time
				return InternalRel::R_32;
			case R_X86_PC32:
				{
					// as code and data are in the same buffer this relocation can be done here
					int32_t s = int32_t(sectionOffset);
					int32_t a = rel.getAddend32LE(code);
					int32_t p = int32_t(codeOffset + offset);
					writeLE<uint32_t>(s + a - p, &code[offset]);
				}
				break;
			default:
				dError("unsupported relocation type " << type);
			}
			return -1;
		}
	};

	// relocator for 64 bit x86
	struct RelocatorX64
	{
		enum RelType
		{
			R_X64_32   = 10, // 32 bit S + A
			R_X64_32S  = 11, // 32 bit S + A
			R_X64_PC32 =  2, // 32 bit S + A - P
			R_X64_64   =  1, // 64 bit S + A
			R_X64_PC64 = 24, // 64 bit S + A - P
		};

		template <typename Rel>
		void rela2rel(const Rel& rel, uint8_t* code)
		{
			// convert rela to rel by writing the addend into the code
			
			// offset of relocation in code
			uint64_t offset = rel.r_offset;
			
			int type = rel.getType();
			switch (type)
			{
			case R_X64_32:
			case R_X64_32S:
			case R_X64_PC32:
				{
					int32_t a = rel.getAddend32LE(code);
					writeLE<uint32_t>(a, &code[offset]);
				}
				break;
			case R_X64_64:
			case R_X64_PC64:
				{
					int64_t a = rel.getAddend64LE(code);
					writeLE<uint64_t>(a, &code[offset]);
				}
				break;
			default:
				dError("unsupported relocation type " << type);
			}
		}

		template <typename Rel>
		int fixupSection(const Rel& rel, uint8_t* code, size_t codeOffset, size_t sectionOffset)
		{
			// code segment and data segments are put into the same buffer. this modifies relocations accordingly.
			// sectionOffset is the offset of the accessed section relative to the code
						
			// offset of relocation in code
			uint64_t offset = rel.r_offset;
			
			int type = rel.getType();
			switch (rel.getType())
			{
			case R_X64_32:
			case R_X64_32S:
				{
					// add offset of data section inside buffer to addend (i.e. access to data)
					int32_t s = int32_t(sectionOffset);
					int32_t a = rel.getAddend32LE(code);
					writeLE<uint32_t>(s + a, &code[offset]);
				}

				// relocation needs to be stored and done at loading time
				return InternalRel::R_32;
			case R_X64_PC32:
				{
					// as code and data are in the same buffer this relocation can be done here
					int32_t s = int32_t(sectionOffset);
					int32_t a = rel.getAddend32LE(code);
					int32_t p = int32_t(codeOffset + offset);
					writeLE<uint32_t>(s + a - p, &code[offset]);
				}
				break;
			case R_X64_64:
				{
					// add offset of data section inside buffer to addend (i.e. access to data)
					int64_t s = int64_t(sectionOffset);
					int64_t a = rel.getAddend64LE(code);
					writeLE<uint64_t>(s + a, &code[offset]);
				}

				// relocation needs to be stored and done at loading time
				return InternalRel::R_64;
			case R_X64_PC64:
				{
					// as code and data are in the same buffer this relocation can be done here
					int64_t s = int64_t(sectionOffset);
					int64_t a = rel.getAddend64LE(code);
					int64_t p = int64_t(codeOffset + offset);
					writeLE<uint64_t>(s + a - p, &code[offset]);
				}
				break;
			default:
				dError("unsupported relocation type " << type);
			}
			return -1;
		}
	};

	template <typename Selector, typename A, typename B>
	struct Select
	{
		typedef A Type;
	};

	template <typename A, typename B>
	struct Select<int64_t, A, B>
	{
		typedef B Type;
	};

	template <typename SType, typename UType>
	struct Elf
	{
		typedef UType Addr;
		typedef uint16_t Half;
		typedef UType Off;
		typedef int32_t Sword;
		typedef uint32_t Word;	
		typedef SType Sxword;
		typedef UType Xword;	

		enum Type
		{
			ET_NONE = 0,
			ET_REL  = 1,
			ET_EXEC = 2,
			ET_DYN  = 3,
			ET_CORE = 4
		};

		enum Machine
		{
			EM_NONE = 0,
			EM_386  = 3,
			//EM_x64  = 
		};

		enum Constants
		{
			EI_NIDENT = 16
		};

		struct Header
		{
			unsigned char ident[EI_NIDENT];
			Half e_type; // Type
			Half e_machine; // Machine
			Word e_version;
			Addr e_entry;
			Off e_phoff;
			Off e_shoff;
			Word e_flags;
			Half e_ehsize;
			Half e_phentsize;
			Half e_phnum;
			Half e_shentsize;
			Half e_shnum;
			Half e_shstrndx;
		};

		enum SectionType
		{
			SHT_NULL     = 0,
			SHT_PROGBITS = 1,
			SHT_SYMTAB   = 2,
			SHT_STRTAB   = 3,
			SHT_RELA     = 4,
			SHT_HASH     = 5,
			SHT_DYNAMIC  = 6,
			SHT_NOTE     = 7,
			SHT_NOBITS   = 8,
			SHT_REL      = 9,
			SHT_SHLIB    = 10,
			SHT_DYNSYM   = 11
		};
		
		// special section indexes
		enum SectionIndex
		{
			SHN_UNDEF      = 0,
			SHN_LORESERVE  = 0xff00,
			SHN_LOPROC     = 0xff00,
			SHN_HIPROC     = 0xff1f,
			SHN_ABS        = 0xfff1,
			SHN_COMMON     = 0xfff2,
			SHN_HIRESERVE  = 0xffff
		};

		// section flags
		enum SectionFlags
		{
			SHF_WRITE     = 0x1,
			SHF_ALLOC     = 0x2,
			SHF_EXECINSTR = 0x4,
			SHF_MASKPROC  = 0xf0000000		
		};

		struct Section
		{
			Word sh_name;
			Word sh_type;
			Xword sh_flags;
			Addr sh_addr;
			Off sh_offset;
			Xword sh_size;
			Word sh_link;
			Word sh_info;
			Xword sh_addralign;
			Xword sh_entsize;
		};

		enum SymbolBinding
		{
			STB_LOCAL  = 0,
			STB_GLOBAL = 1,
			STB_WEAK   = 2,
			STB_LOPROC = 13,
			STB_HIPROC = 15
		};

		enum SymbolType
		{
			STT_NOTYPE   = 0,
			STT_OBJECT   = 1,
			STT_FUNC     = 2,
			STT_SECTION  = 3,
			STT_FILE     = 4,
			STT_LOOS     = 10,
			STT_HIOS     = 12,
			STT_LOPROC   = 13,
			STT_HIPROC   = 15
		};

		struct Symbol32
		{
			Word st_name;
			Addr st_value;
			Word st_size;
			unsigned char st_info;
			unsigned char st_other;
			Half st_shndx;

			SymbolBinding getBinding() const {return SymbolBinding(st_info >> 4);}
			SymbolType getType() const {return SymbolType(st_info & 0xf);}
		};
		struct Symbol64
		{
			Word st_name;
			unsigned char st_info;
			unsigned char st_other;
			Half st_shndx;
			Addr st_value;
			Xword st_size;

			SymbolBinding getBinding() const {return SymbolBinding(st_info >> 4);}
			SymbolType getType() const {return SymbolType(st_info & 0xf);}
		};
		typedef typename Select<SType, Symbol32, Symbol64>::Type Symbol;

		struct Rel
		{
			Addr r_offset;
			Xword r_info;

			int getSymbolIndex() const {return r_info >> (sizeof(Xword) == 8 ? 32 : 8);}
			int getType() const {return r_info & (sizeof(Xword) == 8 ? 0xffffffff : 0xff);}
			
			int32_t getAddend32LE(uint8_t* code) const {return readLE<uint32_t>(&code[r_offset]);}
			int64_t getAddend64LE(uint8_t* code) const {return readLE<uint64_t>(&code[r_offset]);}
		};

		struct Rela : public Rel
		{
			Sxword r_addend;

			int32_t getAddend32LE(uint8_t* code) const {return int32_t(r_addend);}
			int64_t getAddend64LE(uint8_t* code) const {return int64_t(r_addend);}
		};


		template <typename Relocator>
		static void writeMachineCode(const std::string& elf, ObjectWriter& ow, Relocator& relocator)
		{
			// get header
			const Header& header = (const Header&)elf[0];

			// get sections
			const Section* sections = (const Section*)&elf[header.e_shoff];

			// section header string table
			const char* headerStringTable = &elf[sections[header.e_shstrndx].sh_offset];
			
			// all sections (data and code) are written into one buffer. this map maps section id to offset in buffer
			std::map<int, size_t> sectionOffsets; 
			int symbolTableIndex = -1;

			// iterate over all sections and assign section offsets and get symbol table
			size_t dataSize = 0;
			for (int i = 0; i < header.e_shnum; ++i)
			{
				const Section& section = sections[i];
				StringRef sectionName(&headerStringTable[section.sh_name]);

				if (section.sh_type == SHT_PROGBITS && section.sh_size > 0 && (section.sh_flags & SHF_WRITE) == 0)
				{
					// code or constant data: set offset for this section
					sectionOffsets[i] = dataSize;
					
					// round up to 16 bytes
					dataSize += (section.sh_size + 15) & ~15;
				}
				else if (section.sh_type == SHT_SYMTAB)
				{
					// found symbol table (used to get names of external functions that we have to link to)
					symbolTableIndex = i;
				}
			}
		
			if (symbolTableIndex != -1)
			{
				std::vector<ExternalRel> externalRels;
				std::map<StringRef, uint> externalNames;
				std::vector<InternalRel> internalRels;

				// get relocations from all relocation sections
				for (int i = 0; i < header.e_shnum; ++i)
				{
					const Section& relocationSection = sections[i];

					// check if section is a relocation section and refers to one of the sections (typically a code section)
					boost::optional<size_t> codeOffset = getValue(sectionOffsets, relocationSection.sh_info);				 
					if ((relocationSection.sh_type == SHT_REL || relocationSection.sh_type == SHT_RELA) && codeOffset != null)
					{
						// code section
						const Section& codeSection = sections[relocationSection.sh_info];
						uint8_t* code = (uint8_t*)elf.data() + codeSection.sh_offset;

						// associated symbol table of relocation table
						const Section& symbolSection = sections[relocationSection.sh_link];
			
						// associated string table of symbol table
						const Section& stringSection = sections[symbolSection.sh_link];
						const char* symbolStringTable = &elf[stringSection.sh_offset];

						// symbols
						const Symbol* symbols = (const Symbol*)&elf[symbolSection.sh_offset];

						if (relocationSection.sh_type == SHT_REL)
						{
							// relocations of type rel: addend in code
							int numRels = relocationSection.sh_size / sizeof(Rel);
							const Rel* elfRels = (const Rel*)&elf[relocationSection.sh_offset];
									
							// iterate over relocations
							for (int relIndex = 0; relIndex < numRels; ++relIndex)
							{
								const Rel& rel = elfRels[relIndex];

								// get symbol
								int symbolIndex = rel.getSymbolIndex();
								const Symbol& symbol = symbols[symbolIndex];
								SymbolBinding binding = symbol.getBinding();
								//SymbolType type = symbol.getType();
					
								boost::optional<size_t> sectionOffset = getValue(sectionOffsets, symbol.st_shndx);				 
								if (binding == STB_GLOBAL && symbol.st_shndx == SHN_UNDEF)
								{
									// external relocation
									
									// get name of external symbol (e.g. "malloc")
									StringRef name = &symbolStringTable[symbol.st_name];
									//dNotify("ext rel " << name << " t " << rel.getType() << " o " << rel.r_offset);

									// create external relocation with offset where to patch the code
									externalRels += ExternalRel(name, *codeOffset + rel.r_offset);
									externalNames[name];
								}
								else if (binding != STB_GLOBAL && sectionOffset != null)
								{
									// section relocation (binding == STB_LOCAL, type == STT_SECTION)
									// or function relocation (binding == STB_WEAK, type == STT_FUNC).

									// function relocations can be applied immediately if the call to the function is
									// pc-relative since all code and data sections are written into one buffer
									int type = relocator.fixupSection(rel, code, *codeOffset, *sectionOffset);
									if (type != -1)
									{
										// create internal relocation with offset where to patch the code
										internalRels += InternalRel(Off(*codeOffset) + rel.r_offset, InternalRel::Type(type));
									}
								}
							}
						}
						else
						{
							// relocations of type rela: addend in 'Rela' field of relocation
							int numRels = relocationSection.sh_size / sizeof(Rela);
							const Rela* elfRels = (const Rela*)&elf[relocationSection.sh_offset];
									
							// iterate over relocations
							for (int relIndex = 0; relIndex < numRels; ++relIndex)
							{
								const Rela& rel = elfRels[relIndex];

								// get symbol
								int symbolIndex = rel.getSymbolIndex();
								const Symbol& symbol = symbols[symbolIndex];
								SymbolBinding binding = symbol.getBinding();
								//SymbolType type = symbol.getType();
					
								boost::optional<size_t> sectionOffset = getValue(sectionOffsets, symbol.st_shndx);
								if (binding == STB_GLOBAL && symbol.st_shndx == SHN_UNDEF)
								{
									// external relocation
									
									// convert rela to rel by patching offset into code
									relocator.rela2rel(rel, code);

									// get name of external symbol (e.g. "malloc")
									StringRef name = &symbolStringTable[symbol.st_name];
									//dNotify("ext rela " << name << " t " << rel.getType() << " o " << rel.r_offset);

									// create external relocation with offset where to patch the code
									externalRels += ExternalRel(name, *codeOffset + rel.r_offset);
									externalNames[name];
								}
								else if (binding != STB_GLOBAL && sectionOffset != null)
								{
									// section relocation (binding == STB_LOCAL, type == STT_SECTION)
									// or function relocation (binding == STB_WEAK, type == STT_FUNC).

									// function relocations can be applied immediately if the call to the function is
									// pc-relative since all code and data sections are written into one buffer
									int type = relocator.fixupSection(rel, code, *codeOffset, *sectionOffset);
									if (type != -1)
									{
										// create internal relocation with offset where to patch the code
										internalRels += InternalRel(Off(*codeOffset) + rel.r_offset, InternalRel::Type(type));
									}
								}
							}
						}
					}
				}
			
				// store code sections into file
				{
					ow & dataSize;
					typedef std::pair<const int, size_t> OffsetPair;
					foreach (OffsetPair& p, sectionOffsets)
					{
						const Section& section = sections[p.first];
						ow.writeData(elf.data() + section.sh_offset, section.sh_size);
						ow.skip(-section.sh_size & 15);				
					}
				}
						
				// write public symbols (e.g. "initGlobal", "update", "render" etc.)
				{
					const Section& symbolSection = sections[symbolTableIndex];
		
					// associated string table of symbol table
					const Section& stringSection = sections[symbolSection.sh_link];
					const char* symbolStringTable = &elf[stringSection.sh_offset];
				
					int numSymbols = symbolSection.sh_size / sizeof(Symbol);
					const Symbol* symbols = (const Symbol*)&elf[symbolSection.sh_offset];

					uint numPublics = 0;
					for (int symbolIndex = 0; symbolIndex < numSymbols; ++symbolIndex)
					{
						const Symbol& symbol = symbols[symbolIndex];
				
						// check if symbol is a global in one of our code/data sections (e.g. "update")
						if (symbol.getBinding() == STB_GLOBAL && contains(sectionOffsets, symbol.st_shndx))
						{
							++numPublics;
						}							
					}

					// write public symbols
					ow & numPublics;
					for (int symbolIndex = 0; symbolIndex < numSymbols; ++symbolIndex)
					{
						const Symbol& symbol = symbols[symbolIndex];
				
						// check if symbol is a global in one of our code/data sections (e.g. "update")
						boost::optional<size_t> sectionOffset = getValue(sectionOffsets, symbol.st_shndx);				 
						if (symbol.getBinding() == STB_GLOBAL && sectionOffset != null)
						{
							StringRef name = &symbolStringTable[symbol.st_name];
							size_t offset = *sectionOffset + symbol.st_value;

							// write name of global
							ow & name;

							// write offset
							ow & offset;
						}
					}
				}

/*
	// debug output
	const char* bindings[] =
	{
		"STB_LOCAL",
		"STB_GLOBAL",
		"STB_WEAK",
	};
	if (binding < boost::size(bindings))
		w << bindings[binding];
	else
		w << int(binding);
	w << ' ';

	const char* types[] =
	{
		"STT_NOTYPE",
		"STT_OBJECT",
		"STT_FUNC",
		"STT_SECTION",
		"STT_FILE",
	};
	if (type < boost::size(types))
		w << types[type];
	else
		w << int(type);
	w << ' ';

	w << symbol.st_shndx << ' ';

	w << '\n';
	
	w.close();
*/
				// write list of names of external symbols (e.g. "malloc")
				uint index = 0;
				typedef std::pair<const StringRef, uint> NamePair;						
				ow & externalNames.size();
				foreach (NamePair& p, externalNames)
				{
					ow & p.first;

					// assign index to name
					p.second = index++;
				}

				// write relocations for extrnal symbols
				ow & externalRels.size();
				foreach (const ExternalRel& externalRel, externalRels)
				{
					// write index of name of relocation (e.g. reference to an external function, e.g. "malloc")
					ow & externalNames[externalRel.name];
					
					// write offset of location to patch in machine code (call of external function)
					ow & externalRel.offset;
				}

				// write internal relocations (e.g. access to data)
				ow & internalRels.size();
				foreach (const InternalRel& internalRel, internalRels)
				{
					// write offset of location to patch in machine code (call instruction)
					ow & internalRel.offset;
					//ow & wrapUInt(internalRel.type);
				}
			}
		}
	};

	struct TypeOffset
	{
		const clang::Type* type;
		size_t offset;
		
		TypeOffset()
			: type(NULL), offset() {}
		TypeOffset(const clang::Type* type, size_t offset)
			: type(type), offset(offset) {}
	};

	TypeOffset getRecordTypeOffset(clang::ASTContext* astContext, clang::RecordDecl* recordDecl, const std::string& name)
	{
		clang::RecordDecl::field_iterator it = recordDecl->field_begin();
		clang::RecordDecl::field_iterator end = recordDecl->field_end();
		int fieldIndex = 0;
		for (; it != end; ++it, ++fieldIndex)
		{
			clang::FieldDecl* fieldDecl = *it;
			if (fieldDecl->getName() == name)
			{
				const clang::Type* type = fieldDecl->getType().getTypePtr();
				size_t offset = size_t(astContext->getASTRecordLayout(recordDecl).getFieldOffset(fieldIndex) / 8);
				return TypeOffset(type, offset);
			}
		}
		return TypeOffset();
	}

	size_t getTypeSize(clang::ASTContext* astContext, const clang::Type* type)
	{
		return size_t(astContext->getTypeSize(type)) / 8;
	}

	TypeOffset getTypeOffset(clang::ASTContext* astContext, clang::RecordDecl* recordDecl, const std::string& p)
	{
		// get offset of path in struct. example: path = "foo.bar[3].x"
		
		int length = int(p.length());

		// first path element (e.g. "foo")
		int pos = 0;
		do
			++pos;
		while (pos < length && p[pos] != '.' && p[pos] != '[');

		std::string element = p.substr(0, pos);
		TypeOffset typeOffset = getRecordTypeOffset(astContext, recordDecl, element);
		if (typeOffset.type == NULL)
			return TypeOffset();

		// following path elements (e.g. ".bar[3].x")
		while (pos < length)
		{	
			// get next path element
			int start = pos;
			do
				++pos;
			while (pos < length && p[pos] != '.' && p[pos] != '[');
			
			if (p[start] == '.')
			{
				// member selection on struct or vector (e.g. ".bar" or ".x")
				std::string element = p.substr(start + 1, pos - start - 1);

				if (const clang::RecordType* recordType = typeOffset.type->getAs<clang::RecordType>())
				{
					// struct
					clang::RecordDecl* recordDecl = recordType->getDecl();
					TypeOffset typeOffset2 = getRecordTypeOffset(astContext, recordDecl, element);
					if (typeOffset2.type == NULL)
						return TypeOffset();
					
					typeOffset.type = typeOffset2.type;
					typeOffset.offset += typeOffset2.offset;				
				}
				else if (const clang::VectorType* vectorType = typeOffset.type->getAs<clang::VectorType>())
				{
					// vector
					int index = 0;
					if (element == "x")
						index = 0;
					else if (element == "y")
						index = 1;
					else if (element == "z")
						index = 2;
					else if (element == "w")
						index = 3;
					else
					{
						// error: member not found
						return TypeOffset();
					}
					
					const clang::Type* elementType = vectorType->getElementType().getTypePtr();
					size_t size = getTypeSize(astContext, elementType);
					
					typeOffset.type = elementType;
					typeOffset.offset += index * size;
				}
				else
				{
					// error: not a struct or vector
					return TypeOffset();
				}
			}
			else
			{
				// array index (e.g. "[3]")
				int index = toInt(p.substr(start + 1, pos - start - 2));
			
				if (const clang::Type* elementType = typeOffset.type->getArrayElementTypeNoTypeQual())
				{
					size_t size = getTypeSize(astContext, elementType);
					typeOffset.type = elementType;
					typeOffset.offset += index * size;
				}
				else
				{
					// error: not an array
					return TypeOffset();
				}
			}
		}

		return typeOffset;
	}
} // anonymous namespace


void writeForMC(Pointer<SceneFile> sceneFile, ObjectWriter& ow, const SceneOptions& options, MCTarget mcTarget)
{
	// create converters
	Pointer<ConverterContext> converterContext = new ConverterContext();
	Pointer<ImageConverter> imageConverter = new ImageConverter(converterContext);
	Pointer<BufferConverter> bufferConverter = new BufferConverter(converterContext);

	// llvm context
	llvm::LLVMContext context;
	Compiler compiler(Compiler::VM_OPENGL);

	// target
	std::string triple;
	//std::string triple = llvm::sys::getHostTriple();
	bool is64bit = false;
	switch (mcTarget)
	{
	case X86_GCC:
		triple = "i686-pc-linux";
		break;
	case X64_GCC:
		triple = "x86_64-pc-linux";
		is64bit = true;
		break;
	case X86_WINDOWS:
		triple = "i686-pc-digiwin";
		break;
	case X64_WINDOWS:
		triple = "x86_64-pc-digiwin";
		is64bit = true;
		break;
	}
	std::string error;
	//clang::TargetOptions targetOptions;
	const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple, error);
	llvm::TargetMachine* targetMachine = target->createTargetMachine(triple, std::string(),
		std::string(), llvm::Reloc::Default, sizeof(size_t) == 4 ? llvm::CodeModel::Default : llvm::CodeModel::Large);


	// code generator options
	clang::CodeGenOptions codeGenOptions;
	codeGenOptions.DisableLLVMOpts = 1;
	codeGenOptions.OptimizationLevel = 0;
	//codeGenOptions.OptimizationLevel = 2;
	codeGenOptions.Inlining = clang::CodeGenOptions::NormalInlining;
	
	// write textures
	uint numTextures = uint(sceneFile->textures.size());
	ow & numTextures;
	std::map<std::string, int> textureIndices;
	foreach (Pointer<Texture> texture, sceneFile->textures)
	{
		const std::string& name = texture->name;

		// memoize index of texture in textureInfos array
		textureIndices[name] = int(textureIndices.size());
				
		// generate code texture
		uint type = P_TEXTURE_2D;
		std::vector<uint8_t> data;
		std::string code;
		{
			CodeWriter w(new StringRefDevice(code));
			DataWriter d(new ContainerDevice<std::vector<uint8_t>&>(data), false); //! little endian
			
			w << "extern \"C\" {\n";
			if (Pointer<TextureImage> textureImage = dynamicCast<TextureImage>(texture))
			{
				generateTextureOpenGL(imageConverter, textureImage->image, w, d, texture->type, options);
			}
			else if (Pointer<TextureSequence> textureSequence = dynamicCast<TextureSequence>(texture))
			{
				uint length = textureSequence->imageSequence.rbegin()->first + 1;
				type |= length << 8;
				
				generateTextureSequenceOpenGL(imageConverter, textureSequence->imageSequence, w, d, texture->type,
					options);
			}			
			w << "}\n";
			w.close();
			d.close();
		}
		
		// compile texture
		llvm::OwningPtr<GetStructs> astConsumer(new GetStructs(clang::CreateLLVMCodeGen(
			*compiler.diagnosticsEngine,
			name, // module name
			codeGenOptions,
			context)));

		Pointer<CompileResult> result = compile(compiler, code, astConsumer.get(), mcTarget == X86_WINDOWS);

		// single-pass loop to enable break on error
		bool ok = false;
		do
		{
			if (result == null)
			{
				dError("texture '" << name << "' failed to compile");
				break;
			}

			// get module (leave ownership at astConsumer)
			llvm::Module* module = astConsumer->GetModule();
			if (module == NULL)
				break;

			// check if struct Global was found
			if (astConsumer->globalDecl == NULL)
				break;

			// generate elf
			std::string elf;
			if (!generateELF(targetMachine, module, elf))
			{
				dError("failed to generate machine code for texture '" << name << "'");
				break;
			}

			ok = true;

			// write name
			ow & name;

			// write type
			ow & type;

			// write data
			ow & data;

			// write code
			if (!is64bit)
			{
				RelocatorX86 relocator;
				Elf<int32_t, uint32_t>::writeMachineCode(elf, ow, relocator);
			}
			else
			{
				RelocatorX64 relocator;
				Elf<int64_t, uint64_t>::writeMachineCode(elf, ow, relocator);
			}

			// write size of global
			size_t globalSize = result->getSize(astConsumer->globalDecl);
			ow & globalSize;
		} while (false);

		// if something failed write a zero-length name
		if (!ok)
			ow & 0;
	}

	// write scenes
	uint numScenes = uint(sceneFile->scenes.size());
	ow & numScenes;
	foreach (Pointer<Scene> scene, sceneFile->scenes)
	{
		const std::string& name = scene->name;

		// generate code of scene
		std::vector<uint8_t> data;
		std::string code;
		{
			CodeWriter w(new StringRefDevice(code));
			DataWriter d(new ContainerDevice<std::vector<uint8_t>&>(data), false); //! little endian

			w << "extern \"C\" {\n";
			SceneStatistics stats;
			generateSceneOpenGL(bufferConverter, scene, w, d, options, stats);
			w << "}\n";

			w.close();
			d.close();
		}
		
		// compile scene
		llvm::OwningPtr<GetStructs> astConsumer(new GetStructs(clang::CreateLLVMCodeGen(
			*compiler.diagnosticsEngine,
			name, // module name
			codeGenOptions,
			context)));

		Pointer<CompileResult> result = compile(compiler, code, astConsumer.get(), mcTarget == X86_WINDOWS);

		// single-pass loop to enable break on error
		bool ok = false;
		do
		{
			if (result == null)
			{
				dError("scene '" << name << "' failed to compile");
				break;
			}
			
			// get module (leave ownership at astConsumer)
			llvm::Module* module = astConsumer->GetModule();
			if (module == NULL)
				break;

			// check if structs Global and Instance were found
			if (astConsumer->globalDecl == NULL || astConsumer->instanceDecl == NULL)
				break;

			// get type and offset of state in struct Instance
			TypeOffset stateTypeOffset = getRecordTypeOffset(result->astContext.get(), astConsumer->instanceDecl, "state");
			if (stateTypeOffset.type == NULL)
				break;

			// state must be a struct
			const clang::RecordType* stateRecordType = stateTypeOffset.type->getAs<clang::RecordType>();
			if (stateRecordType == NULL)
				break;
			clang::RecordDecl* stateDecl = stateRecordType->getDecl();

			// generate elf
			std::string elf;
			if (!generateELF(targetMachine, module, elf))
			{
				dError("failed to generate machine code for scene '" << name << "'");
				break;
			}

			ok = true;

			// write name
			ow & name;

			// write data
			ow & data;

			// write code
			if (!is64bit)
			{
				RelocatorX86 relocator;
				Elf<int32_t, uint32_t>::writeMachineCode(elf, ow, relocator);
			}
			else
			{
				RelocatorX64 relocator;
				Elf<int64_t, uint64_t>::writeMachineCode(elf, ow, relocator);
			}

			// write size of global and instance
			size_t globalSize = result->getSize(astConsumer->globalDecl);
			size_t instanceSize = result->getSize(astConsumer->instanceDecl);
			ow & globalSize;
			ow & instanceSize;

			// sort attributes and attribute sets because render interface on target uses binary search
			scene->sortAttributes();

			// node infos
			{
				size_t numNodes = scene->nodes.size();
				ow & numNodes;
				foreach (const Scene::Node& node, scene->nodes)
				{
					// name
					ow & node.name;
				
					// type
					ow & node.type;
				}			
			}
			
			// attribute infos
			{
				size_t numAttributes = scene->attributes.size();
				ow & numAttributes;
				foreach (const Scene::Attribute& attribute, scene->attributes)
				{
					// name
					ow & attribute.name;
				
					// type
					ow & attribute.type;

					// convert path to offset
					TypeOffset typeOffset = getTypeOffset(result->astContext.get(), stateDecl, attribute.path);
					size_t offset = stateTypeOffset.offset + typeOffset.offset;
					ow & offset;
				
					// semantic
					ow & attribute.semantic;
				}
			}		

			// texture bindings
			{
				// count number of valid texture bindings
				size_t numTextureBindings = 0;
				foreach (const Scene::TextureBinding& textureBinding, scene->textureBindings)
				{
					// find texture by name
					if (contains(textureIndices, textureBinding.textureName))
						++numTextureBindings;
				}
						
				ow & numTextureBindings;
				foreach (const Scene::TextureBinding& textureBinding, scene->textureBindings)
				{
					// find texture by name
					int textureIndex = getValue(textureIndices, textureBinding.textureName, -1);
					if (textureIndex != -1)
					{			
						// texture index
						ow & uint(textureIndex);

						// type
						ow & textureBinding.type;

						// convert path to offset
						TypeOffset typeOffset = getTypeOffset(result->astContext.get(), stateDecl, textureBinding.path);
						size_t offset = stateTypeOffset.offset + typeOffset.offset;
						ow & offset;
					}
				}				
			}
		
			// attribute sets
			{
				size_t numParameterSets = scene->attributeSets.size();
				ow & numParameterSets;
				foreach (const Scene::AttributeSet& attributeSet, scene->attributeSets)
				{
					ow & attributeSet.name;
					
					// convert path to offset
					TypeOffset typeOffset = getTypeOffset(result->astContext.get(), stateDecl, attributeSet.path);
					size_t offset = stateTypeOffset.offset + typeOffset.offset;
					ow & offset;

					ow & attributeSet.numTracks;
					ow & attributeSet.clipIndex;
					ow & attributeSet.numClips;
				}	
			}
		
			// clips
			{
				size_t numClips = scene->clips.size();
				ow & numClips;
				foreach (const Scene::Clip& clip, scene->clips)
				{
					ow & clip.name;
					ow & clip.index;
					ow & clip.length;
				}
			}

			// collision objects
			{
				// get and sort pick objects
				size_t numPickObjects = scene->instances.size();
				typedef std::pair<std::string, int> PickInfo;
				std::vector<PickInfo> pickInfos;
				foreach (const NamedInstance& instance, scene->instances)
				{
					int index = int(pickInfos.size());
					pickInfos += PickInfo(instance.name, index);
				}
				sort(pickInfos);
			
				// write collision objects
				ow & numPickObjects;
				foreach (const PickInfo& pickInfo, pickInfos)
				{
					// name
					ow & pickInfo.first;

					// convert path to offset
					TypeOffset typeOffset = getTypeOffset(result->astContext.get(), astConsumer->instanceDecl,
						arg("ids[%0]", pickInfo.second));
					size_t offset = typeOffset.offset;
					ow & offset;
				}
			}
		} while (false);

		// if something failed write a zero-length name
		if (!ok)
			ow & 0;
	}
}	

} // namespace digi
