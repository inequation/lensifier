/*
 * Lensifier library renderer abstraction interface
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef RENDERER_H

#include "../../include/liblensifier.h"

namespace Lensifier
{

class Renderer
{
public:
	/** Virtual destructor. */
	virtual ~Renderer() {}

	virtual bool IsRenderAPI(RenderAPI API) = 0;
}

}

#endif	// RENDERER_H
