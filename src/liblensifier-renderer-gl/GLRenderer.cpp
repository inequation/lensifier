/*
 * Lensifier library OpenGL renderer
 * Copyright (C) 2013, Leszek Godlewski 
 */

#include "GLRenderer.h"
#include "../liblensifier/DOFEffect.h"

namespace Lensifier
{

DOFEffect<GLRenderer> *DOF;

GLRenderer::GLRenderer()
	: SceneColourTexture((GLuint)-1)
	, SceneDepthTexture((GLuint)-1)
{
}

GLRenderer::~GLRenderer()
{
}

/** Notification issued by the library that the configuration has changed. */
void GLRenderer::Setup(LUINT InScreenWidth, LUINT InScreenHeight,
		LUINT ColourTextureSlot, LUINT DepthTextureSlot)
{
	if (DOF)
	{
		DOF->SceneColour.Set(ColourTextureSlot);
		DOF->SceneDepth.Set(DepthTextureSlot);
		DOF->TexelSize.Set(Vector2(1.f / InScreenWidth, 1.f / InScreenHeight));
	}
}

/** Renders the Depth of Field effect using current configuration. */
void GLRenderer::RenderDOF()
{
	
}

}

