/*
 * Lensifier public library interface
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef LIBLENSIFIER_H
#define LIBLENSIFIER_H

#ifdef __cplusplus
extern "C"
{
#else
// for the bool type
#include <stdbool.h>
#endif

typedef unsigned int	LUINT;

enum LensifierRenderAPI
{
	RA_OpenGL,
	RA_Direct3D9,
	RA_Direct3D10,
	RA_Direct3D11,
};

/**
 * Initializes Lensifier for the given API.
 * @param	RendererSpecificData	D3D device pointer; ignored in OpenGL
 */
bool LensifierInit(LensifierRenderAPI API, void *RendererSpecificData);

/** Shuts the current Lensifier instance down. */
void LensifierShutdown();

/**
 * Sets up the global library settings.
 * @param	ScreenWidth			screen X resolution
 * @param	ScreenHeight		screen Y resolution
 * @param	ColourTextureSlot	index of the texture slot to which scene colour is bound (sampler index in D3D, texture unit index in GL)
 * @param	DepthTextureSlot	index of the texture slot to which scene depth is bound (sampler index in D3D, texture unit index in GL)
 */
void LensifierSetup(LUINT ScreenWidth, LUINT ScreenHeight,
	LUINT ColourTextureSlot, LUINT DepthTextureSlot);

/** Renders all the configured effects. */
void LensifierRender();

#ifdef __cplusplus
}
#endif

#endif	// LIBLENSIFIER_H