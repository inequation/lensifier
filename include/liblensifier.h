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

typedef enum
{
	RA_OpenGL,
	RA_Direct3D9,
	RA_Direct3D10,
	RA_Direct3D11,
} LensifierRenderAPI;

typedef enum
{
	RT_BackBuffer,			/// render to the regular scene back buffer
	RT_ScratchSpace = 0x0F,	/// render to a scratch space; resolution divisor exponent on 0xF0 bits
} LensifierRenderTarget;

typedef enum
{
	RQ_ReserveScratchSpace,	/// client must reserve a scratch space (FBO in OpenGL); resolution divisor exponent in RequestParam
	RQ_SetRenderTarget,		/// client must switch render target; RequestParam is a member of LensifierRenderTarget
	RQ_BlitFromScratchSpace,/// client must blit from scratch space of given resolution divisor exponent to current RT
} LensifierRequest;

typedef bool (*LensifierRequestCallback)(LensifierRequest, void *RequestParam);

/**
 * Initializes Lensifier for the given API.
 * @param	RendererSpecificData	D3D device pointer; ignored in OpenGL
 */
bool LensifierInit(LensifierRenderAPI API, LensifierRequestCallback Callback,
	void *RendererSpecificData);

/** Shuts the current Lensifier instance down. */
void LensifierShutdown(void);

/**
 * Sets up the global library settings.
 * @param	ScreenWidth			screen X resolution
 * @param	ScreenHeight		screen Y resolution
 * @param	ColourTextureSlot	index of the texture slot to which scene colour is bound (sampler index in D3D, texture unit index in GL)
 * @param	DepthTextureSlot	index of the texture slot to which scene depth is bound (sampler index in D3D, texture unit index in GL)
 */
void LensifierSetup(LUINT ScreenWidth, LUINT ScreenHeight,
	void *ColourTexture, void *DepthTexture);

void LensifierDOFSetEnabled(bool);
void LensifierDOFBeginSetup(void);
void LensifierDOFSetFocalDepth(float);
void LensifierDOFSetFocalLength(float);
void LensifierDOFSetFStop(float);
void LensifierDOFSetZNear(float);
void LensifierDOFSetZFar(float);
void LensifierDOFEndSetup(void);

void LensifierDirtBloomSetEnabled(bool);
void LensifierDirtBloomBeginSetup(LUINT Pass);
void LensifierDirtBloomSetBrightnessThreshold(float);
void LensifierDirtBloomSetHalfRes(void *);
void LensifierDirtBloomSetDirtTexture(void *);
void LensifierDirtBloomSetBrightnessGain(float);
void LensifierDirtBloomEndSetup(LUINT Pass);

void LensifierTexturedDOFSetEnabled(bool);
void LensifierTexturedDOFBeginSetup(void);
void LensifierTexturedDOFSetBokehTexture(void *);
void LensifierTexturedDOFSetFocusDistance(float);
void LensifierTexturedDOFSetFocusBreadth(float);
void LensifierTexturedDOFSetBlurFalloffExponent(float);
void LensifierTexturedDOFSetMinBlur(float);
void LensifierTexturedDOFSetMaxNearBlur(float);
void LensifierTexturedDOFSetMaxFarBlur(float);
void LensifierTexturedDOFSetZNear(float);
void LensifierTexturedDOFSetZFar(float);
void LensifierTexturedDOFEndSetup(void);

void LensifierWaterDropletsSetEnabled(bool);
void LensifierWaterDropletsBeginSetup(void);
void LensifierWaterDropletsSetTimer(float);
void LensifierWaterDropletsSetResetTimer(float);
void LensifierWaterDropletsSetRandomTime(float);
void LensifierWaterDropletsEndSetup(void);

/** Renders all the configured effects. */
void LensifierRender(void);

#ifdef __cplusplus
}
#endif

#endif	// LIBLENSIFIER_H