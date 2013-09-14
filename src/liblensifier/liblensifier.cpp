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
//#include "../liblensifier-renderer-d3d/D3DRenderer.h"
#endif

Lensifier::Renderer	*GRenderer = nullptr;
LensifierConfig		*GConfig = nullptr;

extern "C"
{

/** Initializes Lensifier for the given API. */
bool LensifierInit(LensifierRenderAPI API)
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
		case RA_Direct3D:
#if LENSIFIER_USE_RENDERER_DIRECT3D
			if (!GRenderer)
				GRenderer = D3DRenderer::CreateInstance();
#else
			GRenderer = nullptr;
#endif
		default:
			GRenderer = nullptr;
	}
	
	if (!GRenderer)
		return false;
	
	
	
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
 * Sets a new effect configuration. The memory may not be freed!
 * You can pass nullptr to switch all effects off.
 */
void LensifierConfigure(LensifierConfig *Config)
{
	GConfig = Config;
}

/**
 * Renders all the effects. To be called by client after it sets all the
 * textures and render targets up.
 */
void LensifierRender()
{
	// early out on null config
	if (!GConfig)
		return;
	
	// render distance of focus
	
}

}