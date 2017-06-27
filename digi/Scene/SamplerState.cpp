#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>

#include "SamplerState.h"


namespace digi {


SamplerState::~SamplerState()
{
}

// serialize

template <typename Serializer>
void serialize(Serializer& s, SamplerState& samplerState, uint version)
{
	s & wrapUInt(samplerState.filter);
	s & wrapUInt(samplerState.wrapR) & wrapUInt(samplerState.wrapS) & wrapUInt(samplerState.wrapT);
	//s & samplerState.borderColor;
}

Pointer<SamplerState> SamplerState::load(ObjectReader& r)
{
	Pointer<SamplerState> samplerState = new SamplerState();
	serialize(r, *samplerState, readVarSize<uint>(r));
	return samplerState;
}

void SamplerState::save(ObjectWriter& w)
{
	uint version = 1;
	w & version;
	serialize(w, *this, version);
}


} // namespace digi
