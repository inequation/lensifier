#ifdef OP_PER_PARAM

// params for pass #0 (bright pass filter)
OP_PER_PARAM(0, LUINT, SceneColour, (LUINT)-1)
OP_PER_PARAM(0, float, BrightnessThreshold, std::numeric_limits<float>::quiet_NaN())

// params for pass #1 (compositing)
OP_PER_PARAM(1, LUINT, FullRes, (LUINT)-1)
OP_PER_PARAM(1, LUINT, HalfRes, (LUINT)-1)
OP_PER_PARAM(1, LUINT, DirtTexture, (LUINT)-1)
OP_PER_PARAM(1, float, BrightnessGain, std::numeric_limits<float>::quiet_NaN())

#else // OP_PER_PARAM

#ifndef DIRTBLOOMEFFECT_H
#define DIRTBLOOMEFFECT_H

#include "MultiPassEffect.h"

namespace Lensifier
{

extern const char DirtBloomEffectBrightPassPixelShader[];
extern const char DirtBloomEffectCompositePassPixelShader[];

template <class RendererClass>
class DirtBloomEffect : public MultiPassEffect<RendererClass, 2>
{
public:
	DECLARE_EFFECT_2T(MultiPassEffect, RendererClass, 2)
	
	DirtBloomEffect()
		: Super()
		#define OP_PER_PARAM(Pass, Type, Name, Default) , INIT_PARAM(Name, Default)
		#include "DirtBloomEffect.h"
		#undef OP_PER_PARAM
	{
		this->Program[0] = RENDERER->CompileProgram(EffectGenericVertexShader, DirtBloomEffectBrightPassPixelShader);
		this->Program[1] = RENDERER->CompileProgram(EffectGenericVertexShader, DirtBloomEffectCompositePassPixelShader);
		this->Register();
		RENDERER->ReserveScratchSpace(1);
	}

protected:
	virtual void Register()
	{
		for (LUINT i = 0; i < 2; ++i)
		{
			#define OP_PER_PARAM(Pass, Type, Name, Default) if (i == Pass) Name.Register(this->Program[i], #Name);
			#include "DirtBloomEffect.h"
			#undef OP_PER_PARAM
		}
	}
	
public:
	#define OP_PER_PARAM(Pass, Type, Name, Default)	\
		CachedShaderParam<RendererClass, Type> Name;
	#include "DirtBloomEffect.h"
	#undef OP_PER_PARAM
};

}

#endif // DIRTBLOOMEFFECT_H
#endif // OP_PER_PARAM