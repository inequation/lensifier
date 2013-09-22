/*
 * Lensifier effect interface
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef EFFECT_H
#define EFFECT_H

#include "Renderer.h"
#include "Utils.h"

namespace Lensifier
{
	
extern Renderer		*GRenderer;
extern const char	EffectGenericVertexShader[];

template <class RendererClass>
class Effect
{
public:
	// convenience defines
	#define RENDERER			((RendererClass *)GRenderer)
	#define INIT_PARAM(n, v)	n(this->Program, v)
	#define DECLARE_EFFECT(s)														\
		typedef s												Super;				\
		typedef typename RendererClass::ProgramHandle			ProgramHandle;		\
		typedef typename RendererClass::ShaderParameterHandle	ShaderParamHandle;
	
	DECLARE_EFFECT(void)
	
	Effect()
		: Enabled(true)
		, Program(0)
		, INIT_PARAM(SceneColour, 0)
		, INIT_PARAM(SceneDepth, 0)
		, INIT_PARAM(ScreenSize, Vector2())
		, INIT_PARAM(TexelSize, Vector2())
	{}
	
	virtual ~Effect()
	{
		RENDERER->ReleaseProgram(Program);
	}
	
	virtual void SetEnabled(bool NewEnabled) {Enabled = NewEnabled;}
	inline bool GetEnabled() {return Enabled;}

protected:
	void Register()
	{
		SceneColour.Register(Program, "SceneColour");
		SceneDepth.Register(Program, "SceneDepth");
		TexelSize.Register(Program, "TexelSize");
	}
	
	bool						Enabled;
	
public:
	ProgramHandle								Program;
	CachedShaderParam<RendererClass, LUINT>		SceneColour;
	CachedShaderParam<RendererClass, LUINT>		SceneDepth;
	CachedShaderParam<RendererClass, Vector2>	ScreenSize;
	CachedShaderParam<RendererClass, Vector2>	TexelSize;
};

}

#endif	// EFFECT_H
