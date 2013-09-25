/*
 * Lensifier library
 * Copyright (C) 2013, Leszek Godlewski 
 */

#include "../../include/liblensifier.h"
#include "Renderer.h"

#define LENSIFIER_USE_RENDERER_OPENGL	1
#define LENSIFIER_USE_RENDERER_DIRECT3D	0

#if LENSIFIER_USE_RENDERER_OPENGL
#include "../liblensifier-renderer-gl/GLRenderer.h"
#endif
#if LENSIFIER_USE_RENDERER_DIRECT3D
#error Unimplemented!
#include "../liblensifier-renderer-d3d/D3DRenderer.h"
#endif

using namespace Lensifier;

Renderer		*Lensifier::GRenderer = nullptr;

extern "C"
{

/** Initializes Lensifier for the given API. */
bool LensifierInit(LensifierRenderAPI API, void *RendererSpecificData)
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
		case RA_Direct3D9:
		case RA_Direct3D10:
		case RA_Direct3D11:
#if LENSIFIER_USE_RENDERER_DIRECT3D
			if (!GRenderer)
				GRenderer = D3DRenderer::CreateInstance(API, RendererSpecificData);
#else
			GRenderer = nullptr;
#endif
			break;
		default:
			GRenderer = nullptr;
	}
	
	if (!GRenderer)
		return false;
	
	// ???
	
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
}

/**
 * Sets up the global library settings.
 * @param	ScreenWidth			screen X resolution
 * @param	ScreenHeight		screen Y resolution
 * @param	ColourTextureSlot	index of the texture slot to which scene colour is bound (sampler index in D3D, texture unit index in GL)
 * @param	DepthTextureSlot	index of the texture slot to which scene depth is bound (sampler index in D3D, texture unit index in GL)
 */
void LensifierSetup(LUINT ScreenWidth, LUINT ScreenHeight,
	LUINT ColourTextureSlot, LUINT DepthTextureSlot)
{
	// early out on null renderer
	if (!GRenderer)
		return;
	
	GRenderer->Setup(ScreenWidth, ScreenHeight,
		ColourTextureSlot, DepthTextureSlot);
}

#define DECLARE_RENDERER_BINDING_FORWARDER(Effect)							\
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

DECLARE_RENDERER_BINDING_FORWARDER(DOF)

#define DECLARE_RENDERER_PROPERTY_FORWARDER(Effect, Type, Parameter)		\
	void Lensifier ## Effect ## Set ## Parameter(Type New ## Parameter)		\
	{																		\
		if (!GRenderer)														\
			return;															\
		GRenderer->Effect ## Set ## Parameter(New ## Parameter);			\
	}

DECLARE_RENDERER_PROPERTY_FORWARDER(DOF, bool, Enabled)
DECLARE_RENDERER_PROPERTY_FORWARDER(DOF, float, FocalDepth)
DECLARE_RENDERER_PROPERTY_FORWARDER(DOF, float, FocalLength)
DECLARE_RENDERER_PROPERTY_FORWARDER(DOF, float, FStop)
DECLARE_RENDERER_PROPERTY_FORWARDER(DOF, float, ZNear)
DECLARE_RENDERER_PROPERTY_FORWARDER(DOF, float, ZFar)

/** Renders all the configured effects. */
void LensifierRender(void)
{
	// early out on null renderer
	if (!GRenderer)
		return;
	
	GRenderer->Render();
}

}