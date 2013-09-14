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
	virtual void OnConfigChanged(LensifierConfig *OldConfig, LensifierConfig *NewConfig) = 0;

	/** Sets and binds the scene texture slots. */
	virtual void SetSceneTextureSlots(LUINT ColourTextureSlot, LUINT DepthTextureSlot) = 0;
	
	/** Renders the Depth of Field effect using current configuration. */
	virtual void RenderDOF() = 0;
};

}

#endif	// RENDERER_H
