#include <digi/System/Log.h>

#include "TreeNode.h"


namespace digi {


std::string makeTargetPath(const std::string& path)
{
	if (path.empty())
		return std::string();

	std::string p;
	size_t len = path.length();

	for (size_t i = 0; i < len; ++i)
	{
		char ch = path[i];
		if (ch == '.')
			p += "._";
		else
			p += ch;
	}
	return p;
}


} // namespace digi
