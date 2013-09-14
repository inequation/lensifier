/*
 * Lensifier library renderer abstraction interface
 * Copyright (C) 2013, Leszek Godlewski 
 */

#include "GLRenderer.h"

namespace Lensifier
{

/** Concrete singleton accessor. */
Renderer *Renderer::GetInstance()
{
	static GLRenderer *Instance = NULL;
	if (!Instance)
		Instance = new GLRenderer();
	return Instance;
}

/** Requests singleton destruction. */
void Renderer::DestroyInstance()
{
	GLRenderer *Instance = (GLRenderer *)GetInstance();
	if (Instance)
		delete Instance;
}

GLRenderer::GLRenderer()
{
}

GLRenderer::~GLRenderer()
{
}


}

