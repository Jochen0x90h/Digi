#ifndef digi_Audio_AudioException_h
#define digi_Audio_AudioException_h

#include <exception>


namespace digi {

/// @addtogroup Data
/// @{

/// exception for audio devices
class AudioException : public std::exception
{
public:
		
	enum Reason
	{
		// not able to open audio device
		AUDIO_ERROR,
		
		// format is not supported
		FORMAT_NOT_SUPPORTED,
	};
	
	AudioException(Reason reason)
		: reason(reason) {}
		
	virtual const char* what() const throw();

	Reason getReason()
	{
		return this->reason;
	}
	
protected:
	
	Reason reason;
};

/// @}

} // namespace digi

#endif
