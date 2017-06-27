#include <boost/range.hpp>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/StringUtility.h>

#include "CodeWriterFunctions.h"
#include "NameGenerator.h"


namespace digi {

std::string NameGenerator::getCurrent() const
{
	char buf[8];
	char* end = convertIntToIdentifier(buf, this->count);
	return std::string(buf, end);
}

void NameGenerator::next(int n)
{
	#include "forbiddenVariables.h"
	while (n > 0)
	{
		++this->count;
		if (!binaryContains(boost::begin(forbiddenVariables), boost::end(forbiddenVariables), this->count))
			--n;
	};
}

} // namespace digi
