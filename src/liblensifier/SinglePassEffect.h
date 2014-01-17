#ifdef OP_PER_PARAM

OP_PER_PARAM(LUINT, SceneColour, (LUINT)-1)
OP_PER_PARAM(LUINT, SceneDepth, (LUINT)-1)
OP_PER_PARAM(Vector2, ScreenSize, Vector2(std::numeric_limits<float>::quiet_NaN()))
OP_PER_PARAM(Vector2, TexelSize, Vector2(std::numeric_limits<float>::quiet_NaN()))

#else // OP_PER_PARAM

#ifndef SINGLEPASSEFFECT_H
#define SINGLEPASSEFFECT_H

#include "Effect.h"

namespace Lensifier
{

template <class RendererClass>
class SinglePassEffect : public Effect<RendererClass>
{
public:
	DECLARE_EFFECT_1T(Effect, RendererClass)
	
	SinglePassEffect()
		: Program(0)
		#define OP_PER_PARAM(Type, Name, Default) , INIT_PARAM(Name, Default)	
		#include "SinglePassEffect.h"
		#undef OP_PER_PARAM
	{}
	virtual ~SinglePassEffect() { RENDERER->ReleaseProgram(Program); }
	
protected:
	virtual void Register()
	{
		#define OP_PER_PARAM(Type, Name, Default) Name.Register(this->Program, #Name);
		#include "SinglePassEffect.h"
		#undef OP_PER_PARAM
	}
	
public:
	ProgramHandle								Program;
	#define OP_PER_PARAM(Type, Name, Default)	\
		CachedShaderParam<RendererClass, Type> Name;
	#include "SinglePassEffect.h"
	#undef OP_PER_PARAM
};

}

#endif // SINGLEPASSEFFECT_H
#endif // OP_PER_PARAM