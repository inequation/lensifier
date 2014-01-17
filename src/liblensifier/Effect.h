/*
 * Lensifier effect interface
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef EFFECT_H
#define EFFECT_H

#include <limits>	// for NaN and friends

#include "Renderer.h"
#include "Utils.h"

namespace Lensifier
{
	
extern Renderer		*GRenderer;
extern const char	EffectGenericVertexShader[];
extern const char	SingleDirGaussianBlurPixelShader[];

template <class RendererClass>
class Effect
{
public:
	// convenience defines
	#define RENDERER					((RendererClass *)GRenderer)
	#define INIT_PARAM(n, v)			n(v)
	#define DECLARE_EFFECT_0T(BaseClass)											\
		typedef BaseClass										Super;				\
		typedef typename RendererClass::ProgramHandle			ProgramHandle;		\
		typedef typename RendererClass::ShaderParameterHandle	ShaderParamHandle;	\
		typedef typename RendererClass::IndexBufferHandle		IndexBufferHandle;	\
		typedef typename RendererClass::VertexBufferHandle		VertexBufferHandle;
	#define DECLARE_EFFECT_1T(BaseClass, TemplateParam1)							\
		typedef BaseClass<TemplateParam1>						Super;				\
		typedef typename RendererClass::ProgramHandle			ProgramHandle;		\
		typedef typename RendererClass::ShaderParameterHandle	ShaderParamHandle;	\
		typedef typename RendererClass::IndexBufferHandle		IndexBufferHandle;	\
		typedef typename RendererClass::VertexBufferHandle		VertexBufferHandle;
	#define DECLARE_EFFECT_2T(BaseClass, TemplateParam1, TemplateParam2)			\
		typedef BaseClass<TemplateParam1, TemplateParam2>		Super;				\
		typedef typename RendererClass::ProgramHandle			ProgramHandle;		\
		typedef typename RendererClass::ShaderParameterHandle	ShaderParamHandle;	\
		typedef typename RendererClass::IndexBufferHandle		IndexBufferHandle;	\
		typedef typename RendererClass::VertexBufferHandle		VertexBufferHandle;
	
	DECLARE_EFFECT_0T(void)
	
	Effect() : Enabled(true) {}
	
	virtual ~Effect() {};
	
	virtual void SetEnabled(bool NewEnabled) {Enabled = NewEnabled;}
	inline bool GetEnabled() {return Enabled;}

protected:
	virtual void Register() = 0;
	
	bool						Enabled;
};

}

#endif	// EFFECT_H
