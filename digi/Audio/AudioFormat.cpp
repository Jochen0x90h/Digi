#include "AudioFormat.h"


namespace digi {

int AudioFormat::getElementSize()
{
	static const int sizes[] = {1, 2, 4};
	return sizes[this->type];
}

} // namespace digi
