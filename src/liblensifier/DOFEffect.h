#ifdef OP_PER_PARAM

OP_PER_PARAM(float, FocalDepth, 0.f)
OP_PER_PARAM(float, FocalLength, 0.f)
OP_PER_PARAM(float, FStop, 0.f)
OP_PER_PARAM(float, ZNear, 0.f)
OP_PER_PARAM(float, ZFar, 0.f)
#ifndef NDEBUG
OP_PER_PARAM(bool, ShowFocus, false)
#endif

#else // OP_PER_PARAM

#ifndef DOFEFFECT_H
#define DOFEFFECT_H

#include "Effect.h"

namespace Lensifier
{

extern const char DOFEffectPixelShader[];

template <class RendererClass>
class DOFEffect : public Effect<RendererClass>
{
public:
	DECLARE_EFFECT(Effect<RendererClass>)
	
	DOFEffect()
		: Super()
		#define OP_PER_PARAM(Type, Name, Default) , INIT_PARAM(Name, Default)	
		#include "DOFEffect.h"
		#undef OP_PER_PARAM
	{
		this->Program = RENDERER->CompileProgram(EffectGenericVertexShader, DOFEffectPixelShader);
		this->Register();
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