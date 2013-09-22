/*
 * Lensifier library renderer abstraction interface
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <cassert>

#include "../../include/liblensifier.h"

namespace Lensifier
{

class Renderer
{
public:
	/** Virtual destructor. */
	virtual ~Renderer() {}
	
	/** Notification issued by the library that the configuration has changed. */
	virtual void Setup(LUINT ScreenWidth, LUINT ScreenHeight,
		LUINT ColourTextureSlot, LUINT DepthTextureSlot) = 0;
	
	/** Renders the configured effects. */
	virtual void Render() = 0;
	
	virtual void DOFSetEnabled(bool) = 0;
	#define OP_PER_PARAM(Type, Name, Default) virtual void DOFSet ## Name(Type) = 0;
	#include "DOFEffect.h"
	#undef OP_PER_PARAM
};

extern Renderer		*GRenderer;

template <class RendererClass, typename T>
struct CachedShaderParam
{
	public:
		CachedShaderParam(const typename RendererClass::ProgramHandle& InProgram,
			const T& InitVal)
			: Handle(-1)
			, CachedValue(InitVal)
		{}
		
		~CachedShaderParam()
		{
			((RendererClass *)GRenderer)->ReleaseShaderParameter(Handle);
		}
		
		void Register(typename RendererClass::ProgramHandle Program,
			const char *Name)
		{
			((RendererClass *)GRenderer)->GetShaderParameter(Program, Name);
		}
		
		void Set(const T& Val)
		{
			if (CachedValue != Val)
			{
				CachedValue = Val;
				((RendererClass *)GRenderer)->SetShaderParameterValue(Handle, Val);					
			}
		}
	
	private:
		typename RendererClass::ShaderParameterHandle	Handle;
		T												CachedValue;
};

}

#endif	// RENDERER_H
