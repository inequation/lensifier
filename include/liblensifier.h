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

enum LensifierRenderAPI;
{
	RA_OpenGL,
	RA_Direct3D
};

typedef struct
{
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

/** Initializes Lensifier for the given API. */
bool LensifierInit(LensifierRenderAPI API);

/** Shuts the current Lensifier instance down. */
void LensifierShutdown();

/**
 * Sets a new effect configuration. The memory may not be freed!
 * You can pass NULL to switch all effects off.
 */
void LensifierConfigure(LensifierConfig *Config);

/**
 * Renders all the effects. To be called by client after it sets all the
 * textures and render targets up.
 */
void LensifierRender();

#ifdef __cplusplus
}
#endif

#endif	// LIBLENSIFIER_H