#include <sstream>
#include <iomanip>

#include <digi/System/File.h>
#include <digi/Data/DataException.h>
#include <digi/Scene/SceneFile.h>
#include <digi/Checksum/CRC32.h>

#include "writeForMC.h"
#include "MCCache.h"


namespace digi {

// MCCache

MCCache::MCCache()
{
	// preallocate index for pick layer
	this->layerInfos["pick"] = LayerInfo(-1);
}

MCCache::~MCCache()
{
}

int MCCache::setLayer(StringRef layerName, StringRef outputCode)
{
	int index = int(this->layerInfos.size()) - 1;
	LayerInfo& layerInfo = this->layerInfos[layerName];
	if (layerInfo.index == -2)
	{
		layerInfo.index = index;
	}
	layerInfo.code = outputCode;
	return layerInfo.index;
}

Pointer<EngineFile> MCCache::load(const fs::path& path)
{
	// count number of passes and calc crc
	int numLayers = 0;
	uint32_t crc = 0;
	foreach (PassInfoPair& p, this->layerInfos)
	{
		if (!p.second.code.empty())
		{
			++numLayers;

			// name
			crc = calcCRC32(p.first.c_str(), p.first.length() + 1, crc);
			
			// index
			crc = calcCRC32((uint8_t*)&p.second.index, sizeof(int), crc);

			// code
			crc = calcCRC32(p.second.code.c_str(), p.second.code.length() + 1, crc);
		}
	}
		
	// build path of cache file
	std::stringstream s;
	s << '.' << std::setfill('0') << std::setw(8) << std::hex << crc << ".dmc";
	fs::path cachePath = path.parent_path() / (path.stem() + s.str());
	
	// check if .digi is newer than .dmc cache file or .dmc does not exist
	bool isCreated = false;
	boost::system::error_code errorCode;
	if (last_write_time(path) > last_write_time(cachePath, errorCode))
	{
		// recreate cache file
		isCreated = true;
		
		// read scene
		ObjectReader r(path);
		if (r.read<uint32_t>() != 0x49474944) // "DIGI"
			throw DataException(r.getDevice(), DataException::FORMAT_ERROR);
			
		Pointer<SceneFile> sceneFile = SceneFile::load(r);
		r.close();
		
		// first write into temp file
		cachePath += ".tmp";

		// create output file
		ObjectWriter ow(cachePath);

		// scene options
		SceneOptions sceneOptions;
		sceneOptions.api = GraphicsApi(GraphicsApi::GL, 330);
		sceneOptions.mapBuffer = true;
		
		// shader options
		if (numLayers > 0)
		{
			sceneOptions.shaderOptions.resize(numLayers);
			std::vector<ShaderOptions>::iterator it = sceneOptions.shaderOptions.begin();
			foreach (PassInfoPair& p, this->layerInfos)
			{
				if (!p.second.code.empty())
				{
					it->layerName = p.first;
					it->layerIndex = p.second.index;
					it->outputCode = p.second.code;
					++it;				
				}
			}
		}
		sceneOptions.shadersInCode = false;

		// write for machine code loader
		#ifdef _WIN32
			MCTarget mcTarget = sizeof(size_t) == 8 ? X64_WINDOWS : X86_WINDOWS;
		#else
			MCTarget mcTarget = sizeof(size_t) == 8 ? X64_GCC : X86_GCC;
		#endif
		writeForMC(sceneFile, ow, sceneOptions, mcTarget);
		
		ow.close();
	} 
	
	// load cached machine code file (load from .tmp if it has just been created)
	Pointer<File> file = File::open(cachePath, File::READ);
	Pointer<EngineFile> engineFile = new MCFile(file);
	file->close();
	
	if (isCreated)
	{
		// rename temp file
		fs::path cachePath2 = cachePath.parent_path() / cachePath.stem();
		fs::rename(cachePath, cachePath2);
	}	
	
	return engineFile;
}

} // namespace digi
