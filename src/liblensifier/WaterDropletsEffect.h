#ifdef OP_PER_PARAM

OP_PER_PARAM(float, Timer, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, ResetTimer, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, RandomTime, std::numeric_limits<float>::quiet_NaN())

#else // OP_PER_PARAM

#ifndef WATERDROPLETSEFFECT_H
#define WATERDROPLETSEFFECT_H

#include "SinglePassEffect.h"

namespace Lensifier
{

extern const char WaterDropletsEffectPixelShader[];

template <class RendererClass>
class WaterDropletsEffect : public SinglePassEffect<RendererClass>
{
public:
	DECLARE_EFFECT_1T(SinglePassEffect, RendererClass)
	
	WaterDropletsEffect()
		: Super()
		#define OP_PER_PARAM(Type, Name, Default) , INIT_PARAM(Name, Default)	
		#include "WaterDropletsEffect.h"
		#undef OP_PER_PARAM
	{
		this->Program = RENDERER->CompileProgram(EffectGenericVertexShader, WaterDropletsEffectPixelShader);
		this->Register();
	}

protected:
	virtual void Register()
	{
		Super::Register();
		#define OP_PER_PARAM(Type, Name, Default) Name.Register(this->Program, #Name);
		#include "WaterDropletsEffect.h"
		#undef OP_PER_PARAM
	}
	
public:
	#define OP_PER_PARAM(Type, Name, Default)	\
		CachedShaderParam<RendererClass, Type> Name;
	#include "WaterDropletsEffect.h"
	#undef OP_PER_PARAM
};

}

#endif // WATERDROPLETSEFFECT_H
#endif // OP_PER_PARAM