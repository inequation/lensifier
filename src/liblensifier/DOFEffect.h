#ifndef DOFEFFECT_H
#define DOFEFFECT_H

#include "Effect.h"

namespace Lensifier
{

template <class RendererClass>
class DOFEffect : public Effect<RendererClass>
{
public:
	DECLARE_EFFECT(Effect<RendererClass>)
	
	DOFEffect()
		: Super()
		, INIT_PARAM(FocalDepth, 0.f)
		, INIT_PARAM(FocalLength, 0.f)
		, INIT_PARAM(FStop, 0.f)
#ifndef NDEBUG
		, INIT_PARAM(ShowFocus, false)
#endif
	{}
	
	virtual ~DOFEffect() {}
	
protected:
	virtual const char *GetVertexShaderSource()
	{
		extern const char *DOFEffectVertexShader;
		return DOFEffectVertexShader;
	}
	virtual const char *GetPixelShaderSource()
	{
		extern const char *DOFEffectPixelShader;
		return DOFEffectPixelShader;
	}
	
public:
	CachedShaderParam<float>	FocalDepth;
	CachedShaderParam<float>	FocalLength;
	CachedShaderParam<float>	FStop;
#ifndef NDEBUG
	CachedShaderParam<bool>	ShowFocus;
#endif
};

}

#endif // DOFEFFECT_H