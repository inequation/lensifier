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

Renderer		*GRenderer = nullptr;
LensifierConfig	*GConfig = nullptr;

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
		case RA_Direct3D9:
		case RA_Direct3D10:
		case RA_Direct3D11:
#if LENSIFIER_USE_RENDERER_DIRECT3D
			if (!GRenderer)
				GRenderer = D3DRenderer::CreateInstance(API, RendererSpecificData);
#else
			GRenderer = nullptr;
#endif
		default:
			GRenderer = nullptr;
	}
	
	if (!GRenderer)
		return false;
	
	// ???
	
	return true;
}

/** Shuts the current Lensifier instance down. */
void LensifierShutdown()
{
	if (GRenderer)
	{
		delete GRenderer;
		GRenderer = nullptr;
	}
}

/**
 * Sets a new effect configuration. The memory is owned by client application.
 * May block for a while to initialize effects  You can pass nullptr to switch all effects off.
 */
void LensifierConfigure(LensifierConfig *Config)
{
	// notify the re
	if (GRenderer)
		GRenderer->OnConfigChanged(GConfig, Config);

	GConfig = Config;
}

/**
 * Renders all the configured effects.
 * @param	ColourTextureSlot	index of the texture slot to which scene colour is bound (sampler index in D3D, active texture index in GL)
 * @param	DepthTextureSlot	index of the texture slot to which scene depth is bound (sampler index in D3D, active texture index in GL)
 */
void LensifierRender(LUINT ColourTextureSlot, LUINT DepthTextureSlot)
{
	// early out on null config
	if (!GConfig)
		return;
	
	GRenderer->SetSceneTextureSlots(ColourTextureSlot, DepthTextureSlot);
	
	if (GConfig->EnableDOF)
		GRenderer->RenderDOF();
}

}