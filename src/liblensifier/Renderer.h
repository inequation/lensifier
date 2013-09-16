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
	
	/** Renders the Depth of Field effect using current configuration. */
	virtual void RenderDOF() = 0;
};

extern Renderer		*GRenderer;

template <class RendererClass, typename T>
struct CachedShaderParam
{
	public:
		CachedShaderParam(const typename RendererClass::ProgramHandle& InProgram,
			const char *Name, const T& InitVal)
			: Handle(((RendererClass *)GRenderer)->GetShaderParameter(InProgram, Name))
			, CachedValue(InitVal)
		{}
		
		~CachedShaderParam()
		{
			((RendererClass *)GRenderer)->ReleaseShaderParameter(Handle);
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
