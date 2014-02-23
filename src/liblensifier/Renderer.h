/*
 * Lensifier library renderer abstraction interface
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <cassert>

#include "../../include/liblensifier.h"
#include "Utils.h"

namespace Lensifier
{

extern class Renderer			*GRenderer;
extern LensifierRequestCallback	GCallback;

class Renderer
{
public:
	typedef enum
	{
		PT_Points,
		PT_Triangles,
		PT_TriangleStrip,
		PT_TriangleFan,
	}
	PrimitiveType;
	
	/** Virtual destructor. */
	virtual ~Renderer() {}
	
	/** Notification issued by the library that the configuration has changed. */
	virtual void Setup(LUINT InScreenWidth, LUINT InScreenHeight,
		void *InColourTexture, void *InDepthTexture)
	{
		ScreenWidth = InScreenWidth;
		ScreenHeight = InScreenHeight;
		ColourTexture = InColourTexture;
		DepthTexture = InDepthTexture;
	}
	
	/** Renders the configured effects. */
	virtual void Render() = 0;
	
	virtual void DOFBeginSetup() = 0;
	virtual void DOFEndSetup() = 0;
	virtual void DOFSetEnabled(bool) = 0;
	#define OP_PER_PARAM(Type, Name, Default) virtual void DOFSet ## Name(Type) = 0;
	#include "DOFEffect.h"
	#undef OP_PER_PARAM
	
	virtual void DirtBloomBeginSetup(LUINT Pass) = 0;
	virtual void DirtBloomEndSetup(LUINT Pass) = 0;
	virtual void DirtBloomSetEnabled(bool) = 0;
	#define OP_PER_PARAM(Pass, Type, Name, Default) virtual void DirtBloomSet ## Name(Type) = 0;
	#include "../liblensifier/DirtBloomEffect.h"
	#undef OP_PER_PARAM
	
	virtual void TexturedDOFBeginSetup() = 0;
	virtual void TexturedDOFEndSetup() = 0;
	virtual void TexturedDOFSetEnabled(bool) = 0;
	#define OP_PER_PARAM(Type, Name, Default) virtual void TexturedDOFSet ## Name(Type) = 0;
	#include "../liblensifier/TexturedDOFEffect.h"
	#undef OP_PER_PARAM
	
	virtual void WaterDropletsBeginSetup() = 0;
	virtual void WaterDropletsEndSetup() = 0;
	virtual void WaterDropletsSetEnabled(bool) = 0;
	#define OP_PER_PARAM(Type, Name, Default) virtual void WaterDropletsSet ## Name(Type) = 0;
	#include "WaterDropletsEffect.h"
	#undef OP_PER_PARAM
	
	inline void ReserveScratchSpace(LUINT ResolutionDivisorExponent)
	{
		assert(GCallback);
		GCallback(RQ_ReserveScratchSpace,
			reinterpret_cast<void *>(ResolutionDivisorExponent));
	}
	
	inline void SetRenderTarget(LensifierRenderTarget Target,
		LUINT ResolutionDivisorExponent = 0)
	{
		assert(GCallback);
		GCallback(RQ_SetRenderTarget,
			reinterpret_cast<void *>(Target | ((ResolutionDivisorExponent << 4) & 0xF0)));
	}
	
	inline void BlitFromScratchSpace(LUINT ResolutionDivisorExponent)
	{
		assert(GCallback);
		GCallback(RQ_BlitFromScratchSpace,
			reinterpret_cast<void *>(ResolutionDivisorExponent));
	}
	
protected:
	LUINT ScreenWidth, ScreenHeight;
	void *ColourTexture, *DepthTexture;
};

template <class RendererClass, typename T>
struct CachedShaderParam
{
	public:
		CachedShaderParam(const T& InitVal)
			: Handle((RendererClass::ShaderParameterHandle)-1)
			, CachedValue(InitVal)
		{}
		
		~CachedShaderParam()
		{
			((RendererClass *)GRenderer)->ReleaseShaderParameter(Handle);
		}
		
		void Register(typename RendererClass::ProgramHandle Program,
			const char *Name)
		{
			Handle = ((RendererClass *)GRenderer)->GetShaderParameter(Program, Name);
		}
		
		inline void Set(const T& Val, const bool Force = false)
		{
			if (Force || CachedValue != Val)
			{
				CachedValue = Val;
				((RendererClass *)GRenderer)->SetShaderParameterValue(Handle, Val);
			}
		}

		inline void Refresh()
		{
			((RendererClass *)GRenderer)->SetShaderParameterValue(Handle, CachedValue);
		}
		
		inline T Get() const { return CachedValue; }
	
	private:
		typename RendererClass::ShaderParameterHandle	Handle;
		T												CachedValue;
};

}

#endif	// RENDERER_H
