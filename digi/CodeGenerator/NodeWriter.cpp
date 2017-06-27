#include <digi/Utility/Ascii.h>

#include "TreeNode.h"
#include "NodeWriter.h"


namespace digi {

// NodeWriter

NodeWriter::~NodeWriter()
{
}

void NodeWriter::writeLineInternal()
{	
	// add comment if node was pushed onto node stack
	if (!this->paths.empty())
	{
		PathState& path = this->paths.back();
		if (!path.written)
		{
			path.written = true;
			
			// empty line
			std::string line;
			this->finishLine(line);
			this->dev->write(line.data(), line.length());
			
			// comment
			line = "// '" + path.path.getPath() + "' (" + path.path.node->getNodeType() + ")";
			this->finishLine(line);
			this->dev->write(line.data(), line.length());
		}
	}
	
	// substitute placeholders
	std::string line;
	line.swap(this->line);
	NodeWriter& w = *this;
	
	size_t len = line.length();
	const char* str = line.c_str();
	size_t e = 0;
	while (e < len)
	{
		// search for newline, placeholder or string
		size_t s = e;
		while (e < len && str[e] != '$' && str[e] != '"')
			++e;
		
		// output line to end, placeholder or string 
		w.writeString(str + s, e - s);
		if (e == len)
			break;
	
		s = e;
		if (str[s] == '$')
		{
			// '$' found: substitute placeholder
			if (s + 1 == len)
				break;
			
			// the char following '$' is the command
			char command = str[s + 1];

			// get the identifier after placeholder (e.g. "$.result.x" -> "result.x")
			e = s + 2;
			bool inIndex = false;
			while (e < len)
			{
				char ch = str[e];
				if (ch == '[')
				{
					// begin array index
					
					// currently only literal allowed (e.g. "[5]")
					if (e + 1 < len && Ascii::isDecimal(str[e + 1]))
					{
						e += 2;
						inIndex = true;
					}
					else
						break;
				}
				else if (ch == ']' && inIndex)
				{
					// end array index
					++e;
					inIndex = false;
				}
				else if (Ascii::isIdentifier(ch) || ch == '.')
				{
					++e;
				}
				else
					break;
			}

			std::string value = substring(str, s + 2, e);

			// substitute (command, value)
			{
				if (this->paths.empty())
				{
					w << "?null" << command << value;
				}
				else
				{
					Path& path = this->paths.back().path;
					if (command == '.')
					{
						// $. is attribute of current node
						path.node->writeVariable(w, path.path + makePath(value));
					}
					else if (command == '@')
					{
						// $@ is type of attribute of current node
						w << path.node->getType(path.path + makePath(value));
					}
					else if (command == ':')
					{
						// $: is node target path of current node
						std::string name = makeTargetPath(path.getPath(1)).substr(1);
						w << name << value;
					}
					else if (command == '&')
					{
						// $& is node path (only useful for comments)
						std::string name = path.getPath(1);
						w << name << value;
					}
					//else if (command == '%')
					//{
					//	// $% is type of node (only useful for comments)
					//	w << path.node->getNodeType();
					//}
					else
					{
						// local variable (e.g. $x)
						w << '_' << command << value;
					}
				}
			}
		}
		else if (str[s] == '"')
		{
			// '"' found: skip string
			char lastChar = 0;
			e = s + 1;
			
			while (e < len && (lastChar == '\\' || str[e] != '"'))
			{
				lastChar = str[e];
				++e;
			}
			if (e < len)
				++e;
			w.writeString(str + s, e - s);
		}
	}

	// add indent and new line to current line
	this->finishLine(this->line);
	
	// write current line	
	this->dev->write(this->line.data(), this->line.length());
}

} // namespace digi
