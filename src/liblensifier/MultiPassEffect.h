#ifdef OP_PER_PARAM

#else // OP_PER_PARAM

#ifndef MULTIPASSEFFECT_H
#define MULTIPASSEFFECT_H

#include "Effect.h"

namespace Lensifier
{

template <class RendererClass, LUINT NumPasses>
class MultiPassEffect : public Effect<RendererClass>
{
public:
	DECLARE_EFFECT_1T(Effect, RendererClass)
	
protected:
	virtual void Register()
	{
		for (LUINT i = 0; i < NumPasses; ++i)
		{
			#define OP_PER_PARAM(Pass, Type, Name, Default) if (i == Pass) Name.Register(this->Program[i], #Name);
			#include "MultiPassEffect.h"
			#undef OP_PER_PARAM
		}
	}
	
public:
	ProgramHandle								Program[NumPasses];
	#define OP_PER_PARAM(Type, Name, Default)	\
		CachedShaderParam<RendererClass, Type> Name;
	#include "MultiPassEffect.h"
	#undef OP_PER_PARAM
};

}

#endif // MULTIPASSEFFECT_H
#endif // OP_PER_PARAM