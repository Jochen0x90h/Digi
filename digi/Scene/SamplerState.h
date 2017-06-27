#ifndef digi_Scene_SamplerState_h
#define digi_Scene_SamplerState_h

#include <digi/Math/All.h>

#include "ObjectReader.h"
#include "ObjectWriter.h"


namespace digi {

/// @addtogroup Scene
/// @{


/*
	sampler state
	
	only two magnification filter modes are provided. the minification mode can be set globally
	OpenGL (ES) 2.x:
		filter is magnificatin filter, the minification filter mode is specified with the textures
		(GL_LINEAR or GL_LINEAR_MIPMAP_LINEAR)
	DirectX:
		filter switches two sampler states, e.g. FILTER_NEAREST -> D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
		and FILTER_LINEAR -> D3D11_FILTER_ANISOTROPIC
*/
struct SamplerState : public Object
{
	public:
		
		SamplerState()
			: filter(FILTER_LINEAR),
			wrapS(WRAP_REPEAT), wrapT(WRAP_REPEAT), wrapR(WRAP_REPEAT) {}
	
		virtual ~SamplerState();
		
		enum FilterMode
		{
			FILTER_NEAREST,
			FILTER_LINEAR,
		};
			
		enum WrapMode
		{
			WRAP_REPEAT = 0,
			WRAP_CLAMP,
			WRAP_MIRROREDREPEAT = 4,
			WRAP_MIRROREDCLAMP,
		};
		
		FilterMode filter;

		WrapMode wrapS;
		WrapMode wrapT;
		WrapMode wrapR;
	

		static Pointer<SamplerState> load(ObjectReader& r);
		void save(ObjectWriter& w);
};


/// @}

} // namespace digi

#endif
