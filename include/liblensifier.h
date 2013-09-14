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

typedef struct
{
	LUINT	ScreenWidth;
	LUINT	ScreenHeight;
	
	/** Whether to enable the Distance of Focus effect. */
	bool	EnableDOF;
	/** Focus distance in the [0..1] range. */
	float	FocusDistance;
	/** Minimum scale of focus blur in the [0..1] range. */
	float	MinFocusScale;
	/** Maximum scale of focus blur on the near side, in the [0..1] range. */
	float	MaxFocusScaleNear;
	/** Maximum scale of focus blur on the far side, in the [0..1] range. */
	float	MaxFocusScaleFar;
}
LensifierConfig;

/**
 * Initializes Lensifier for the given API.
 * @param	RendererSpecificData	D3D device pointer; ignored in OpenGL
 */
bool LensifierInit(LensifierRenderAPI API, void *RendererSpecificData);

/** Shuts the current Lensifier instance down. */
void LensifierShutdown();

/**
 * Sets a new effect configuration. The memory is owned by client application.
 * You can pass NULL to switch all effects off.
 */
void LensifierConfigure(LensifierConfig *Config);

/**
 * Renders all the configured effects.
 * @param	ColourTextureSlot	index of the texture slot to which scene colour is bound (sampler index in D3D, texture unit index in GL)
 * @param	DepthTextureSlot	index of the texture slot to which scene depth is bound (sampler index in D3D, texture unit index in GL)
 */
void LensifierRender(LUINT ColourTextureSlot, LUINT DepthTextureSlot);

#ifdef __cplusplus
}
#endif

#endif	// LIBLENSIFIER_H