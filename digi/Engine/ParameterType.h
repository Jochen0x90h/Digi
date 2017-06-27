#ifndef digi_Engine_ParameterType_h
#define digi_Engine_ParameterType_h


namespace digi {

/// @addtogroup Engine
/// @{

/// scene node types
enum NodeType
{
	N_INVALID = -1,
	N_TRANSFORM,
	N_LOCATOR,
	N_CAMERA,
	N_AMBIENT_LIGHT,
	N_DIRECTIONAL_LIGHT,
	N_POINT_LIGHT,
	N_SPOT_LIGHT,
	N_AREA_LIGHT,
	N_VOLUME_LIGHT,
};

/// scene attribute types (for arrays of scene attributes the array length
/// is in bit 8 to 31 of a type).
enum AttributeType
{
	P_INVALID = -1,
	P_BOOL,
	P_BOOL2,
	P_BOOL3,
	P_BOOL4,
	P_INT,
	P_INT2,
	P_INT3,
	P_INT4,
	P_FLOAT,
	P_FLOAT2,
	P_FLOAT3,
	P_FLOAT4,
	P_FLOAT4X4,

	P_PROJECTION,

	P_TEXTURE_2D,
	P_TEXTURE_3D,
	P_TEXTURE_CUBE,
	
	P_SYMBOL_MAP,
	P_STRING
};

/// @}

} // namespace digi

#endif
