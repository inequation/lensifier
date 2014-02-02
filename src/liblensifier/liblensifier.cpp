/*
 * Lensifier library
 * Copyright (C) 2013, Leszek Godlewski 
 */

#include "../../include/liblensifier.h"
#include "Renderer.h"

#ifndef LENSIFIER_USE_RENDERER_OPENGL
	#define LENSIFIER_USE_RENDERER_OPENGL		1
#endif

#ifndef LENSIFIER_USE_RENDERER_DIRECT3D
	#if defined(_WIN32) || defined(WIN32)
		#define LENSIFIER_USE_RENDERER_DIRECT3D	1
	#else
		#define LENSIFIER_USE_RENDERER_DIRECT3D	0
	#endif
#endif

#if LENSIFIER_USE_RENDERER_OPENGL
#include "../liblensifier-renderer-gl/GLRenderer.h"
#endif
#if LENSIFIER_USE_RENDERER_DIRECT3D
#include "../liblensifier-renderer-d3d/D3D10Renderer.h"
#endif

using namespace Lensifier;

Renderer					*Lensifier::GRenderer = nullptr;
LensifierRequestCallback	Lensifier::GCallback = nullptr;

extern "C"
{

/** Initializes Lensifier for the given API. */
bool LensifierInit(LensifierRenderAPI API, LensifierRequestCallback Callback,
	void *RendererSpecificData)
{
	if (GRenderer)
		return false;

	switch (API)
	{
		case RA_OpenGL:
#if LENSIFIER_USE_RENDERER_OPENGL
			if (!GRenderer)
				GRenderer = new GLRenderer();
#else
			GRenderer = nullptr;
#endif
			break;
		case RA_Direct3D10:
#if LENSIFIER_USE_RENDERER_DIRECT3D
			if (!GRenderer)
				GRenderer = new D3D10Renderer(RendererSpecificData);
#else
			GRenderer = nullptr;
#endif
			break;
		case RA_Direct3D9:
		case RA_Direct3D11:
			// FIXME: no 9 and 11 support yet
			GRenderer = nullptr;
			break;
		default:
			GRenderer = nullptr;
	}
	
	if (!GRenderer)
		return false;
	
	GCallback = Callback;
	
	return true;
}

/** Shuts the current Lensifier instance down. */
void LensifierShutdown(void)
{
	if (GRenderer)
	{
		delete GRenderer;
		GRenderer = nullptr;
	}
	
	GCallback = nullptr;
}

/**
 * Sets up the global library settings.
 * @param	ScreenWidth			screen X resolution
 * @param	ScreenHeight		screen Y resolution
 * @param	ColourTextureSlot	index of the texture slot to which scene colour is bound (sampler index in D3D, texture unit index in GL)
 * @param	DepthTextureSlot	index of the texture slot to which scene depth is bound (sampler index in D3D, texture unit index in GL)
 */
void LensifierSetup(LUINT ScreenWidth, LUINT ScreenHeight,
	void *ColourTextureSlot, void *DepthTextureSlot)
{
	// early out on null renderer
	if (!GRenderer)
		return;
	
	GRenderer->Setup(ScreenWidth, ScreenHeight,
		ColourTextureSlot, DepthTextureSlot);
}

#define DECLARE_RENDERER_SINGLE_PASS_BINDING_FORWARDER(Effect)				\
	void Lensifier ## Effect ## BeginSetup(void)							\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->Effect ## BeginSetup();									\
	}																		\
	void Lensifier ## Effect ## EndSetup(void)								\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->Effect ## EndSetup();									\
	}
#define DECLARE_RENDERER_MULTI_PASS_BINDING_FORWARDER(Effect)				\
	void Lensifier ## Effect ## BeginSetup(LUINT Pass)						\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->Effect ## BeginSetup(Pass);								\
	}																		\
	void Lensifier ## Effect ## EndSetup(LUINT Pass)						\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->Effect ## EndSetup(Pass);								\
	}

#define DECLARE_RENDERER_PROPERTY_FORWARDER(Effect, Type, Parameter)		\
	void Lensifier ## Effect ## Set ## Parameter(Type New ## Parameter)		\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->Effect ## Set ## Parameter(New ## Parameter);			\
	}

DECLARE_RENDERER_SINGLE_PASS_BINDING_FORWARDER(DOF)
DECLARE_RENDERER_PROPERTY_FORWARDER(DOF, bool, Enabled)
#define OP_PER_PARAM(Type, Parameter, Default)								\
	void LensifierDOFSet ## Parameter(Type New ## Parameter)				\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->DOFSet ## Parameter(New ## Parameter);					\
	}
#include "DOFEffect.h"
#undef OP_PER_PARAM

DECLARE_RENDERER_MULTI_PASS_BINDING_FORWARDER(DirtBloom)
DECLARE_RENDERER_PROPERTY_FORWARDER(DirtBloom, bool, Enabled)
#define OP_PER_PARAM(Pass, Type, Parameter, Default)						\
	void LensifierDirtBloomSet ## Parameter(Type New ## Parameter)			\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->DirtBloomSet ## Parameter(New ## Parameter);				\
	}
#include "DirtBloomEffect.h"
#undef OP_PER_PARAM

DECLARE_RENDERER_SINGLE_PASS_BINDING_FORWARDER(TexturedDOF)
DECLARE_RENDERER_PROPERTY_FORWARDER(TexturedDOF, bool, Enabled)
#define OP_PER_PARAM(Type, Parameter, Default)								\
	void LensifierTexturedDOFSet ## Parameter(Type New ## Parameter)		\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->TexturedDOFSet ## Parameter(New ## Parameter);			\
	}
#include "TexturedDOFEffect.h"
#undef OP_PER_PARAM

DECLARE_RENDERER_SINGLE_PASS_BINDING_FORWARDER(WaterDroplets)
DECLARE_RENDERER_PROPERTY_FORWARDER(WaterDroplets, bool, Enabled)
#define OP_PER_PARAM(Type, Parameter, Default)								\
	void LensifierWaterDropletsSet ## Parameter(Type New ## Parameter)		\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->WaterDropletsSet ## Parameter(New ## Parameter);			\
	}
#include "WaterDropletsEffect.h"
#undef OP_PER_PARAM

/** Renders all the configured effects. */
void LensifierRender(void)
{
	// early out on null renderer
	if (!GRenderer)
		return;
	
	GRenderer->Render();
}

}