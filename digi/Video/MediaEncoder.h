#ifndef digi_Video_MediaEncoder_h
#define digi_Video_MediaEncoder_h

#include <digi/Base/Platform.h>
#include <digi/Utility/Object.h>


namespace digi {

/// @addtogroup Video
/// @{

/// base class for media (audio, video, subtitles) encoder
class MediaEncoder : public Object
{
public:
	struct Packet
	{
		enum Type
		{
			NORMAL,
			
			// key frame (only for video track, all others should set to false)
			KEY,
			
			// end packet. forces the container encoder to stop.
			END
		};
	
		// time in seconds
		double time;
		
		// type of frame
		Type type;
		
		// data of packet (owned by MediaEncoder)
		const void* data;
		
		// size of data
		size_t size;
		
		Packet()
			: time(), type(NORMAL), data(), size() {}
	};

	virtual ~MediaEncoder();

	/// close encoder and input devices (e.g. file)
	virtual void close() = 0;

	/// encode packet and set fields of packet structure. packet.data must be owned by the media encoder
	/// and stay valid until the next call to encode()
	virtual void encode(Packet& packet) = 0;
	};

/// @}

} // namespace digi

#endif
