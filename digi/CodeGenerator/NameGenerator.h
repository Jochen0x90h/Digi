#ifndef digi_CodeGenerator_NameGenerator_h
#define digi_CodeGenerator_NameGenerator_h

#include <string>


namespace digi {

/// @addtogroup CodeGenerator
/// @{

class NameGenerator
{
public:
	
	NameGenerator()
		: count(0) {}
	
	// get current name
	std::string getCurrent() const;

	// generate next n-th name
	void next(int n = 1);

	// get current name and go to next
	std::string get() {std::string r = this->getCurrent(); this->next(); return r;}

	void reset() {this->count = 0;}
	
protected:
	
	int count;
};

/// @}

} // namespace digi

#endif
