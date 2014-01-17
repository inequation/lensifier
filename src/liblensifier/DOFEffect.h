#ifdef OP_PER_PARAM

OP_PER_PARAM(float, FocalDepth, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, FocalLength, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, FStop, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, ZNear, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, ZFar, std::numeric_limits<float>::quiet_NaN())
#ifndef NDEBUG
OP_PER_PARAM(bool, ShowFocus, false)
#endif

#else // OP_PER_PARAM

#ifndef DOFEFFECT_H
#define DOFEFFECT_H

#include "SinglePassEffect.h"

namespace Lensifier
{

extern const char DOFEffectPixelShader[];

template <class RendererClass>
class DOFEffect : public SinglePassEffect<RendererClass>
{
public:
	DECLARE_EFFECT_1T(SinglePassEffect, RendererClass)
	
	DOFEffect()
		: Super()
		#define OP_PER_PARAM(Type, Name, Default) , INIT_PARAM(Name, Default)	
		#include "DOFEffect.h"
		#undef OP_PER_PARAM
	{
		this->Program = RENDERER->CompileProgram(EffectGenericVertexShader, DOFEffectPixelShader);
		this->Register();
	}

protected:
	virtual void Register()
	{
		Super::Register();
		#define OP_PER_PARAM(Type, Name, Default) Name.Register(this->Program, #Name);
		#include "DOFEffect.h"
		#undef OP_PER_PARAM
	}
	
public:
	#define OP_PER_PARAM(Type, Name, Default)	\
		CachedShaderParam<RendererClass, Type> Name;
	#include "DOFEffect.h"
	#undef OP_PER_PARAM
};

}

#endif // DOFEFFECT_H
#endif // OP_PER_PARAM