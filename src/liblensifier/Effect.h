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

template <class RendererClass>
class Effect
{
public:
	// convenience defines
	#define RENDERER			((RendererClass *)GRenderer)
	#define INIT_PARAM(n, v)	n(this->Program, #n, v)
	#define DECLARE_EFFECT(s)														\
		typedef s												Super;				\
		typedef typename RendererClass::ProgramHandle			ProgramHandle;		\
		typedef typename RendererClass::ShaderParameterHandle	ShaderParamHandle;	\
		template <typename T>														\
		class CachedShaderParam : public Lensifier::CachedShaderParam<RendererClass, T> {};
	
	DECLARE_EFFECT(void)
	
	Effect()
		: Program(RENDERER->CompileProgram(
			GetVertexShaderSource(), GetPixelShaderSource()))
		, INIT_PARAM(SceneColour, 0)
		, INIT_PARAM(SceneDepth, 0)
		, INIT_PARAM(TexelSize, Vector2())
	{}
	
	virtual ~Effect()
	{
		RENDERER->ReleaseProgram(Program);
	}

protected:
	virtual const char *GetVertexShaderSource() = 0;
	virtual const char *GetPixelShaderSource() = 0;
	
public:
	ProgramHandle				Program;
	CachedShaderParam<LUINT>	SceneColour;
	CachedShaderParam<LUINT>	SceneDepth;
	CachedShaderParam<Vector2>	TexelSize;
};

}

#endif	// EFFECT_H
