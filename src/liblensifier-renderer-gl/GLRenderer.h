/*
 * Lensifier library renderer abstraction interface
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "../liblensifier/Renderer.h"
#include <GL/gl.h>
#include <GL/glext.h>

namespace Lensifier
{

class GLRenderer : public Renderer
{
public:
	GLRenderer();
	virtual ~GLRenderer();

	virtual bool IsRenderAPI(RenderAPI API) { return API == RA_OpenGL; }
};

}

#endif	// GLRENDERER_H
