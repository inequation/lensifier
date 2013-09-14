/*
 * Lensifier library OpenGL renderer
 * Copyright (C) 2013, Leszek Godlewski 
 */

#include "GLRenderer.h"

namespace Lensifier
{

GLRenderer::GLRenderer()
	: SceneColourLocation((GLuint)-1)
	, SceneDepthLocation((GLuint)-1)
	, SceneColourTexture((GLuint)-1)
	, SceneDepthTexture((GLuint)-1)
{
}

GLRenderer::~GLRenderer()
{
}

/** Notification issued by the library that the configuration has changed. */
void GLRenderer::OnConfigChanged(LensifierConfig *OldConfig, LensifierConfig *NewConfig)
{
	if (!OldConfig->EnableDOF && NewConfig->EnableDOF)
	{
		// DOF has just been turned on, allocate resources
		//if ()
	}
}

void GLRenderer::SetSceneTextureSlots(LUINT ColourTextureSlot, LUINT DepthTextureSlot)
{
	assert(SceneColourLocation != (GLuint)-1);
	assert(SceneDepthLocation != (GLuint)-1);
	
	if (SceneColourTexture != (GLuint)ColourTextureSlot)
	{
		SceneColourTexture = (GLuint)ColourTextureSlot;
		LGL(Uniform1iARB)(SceneColourLocation, SceneColourTexture);
	}
	if (SceneDepthTexture != (GLuint)DepthTextureSlot)
	{
		SceneDepthTexture = (GLuint)DepthTextureSlot;
		LGL(Uniform1iARB)(SceneDepthLocation, SceneDepthTexture);
	}
}

/** Renders the Depth of Field effect using current configuration. */
void GLRenderer::RenderDOF()
{
	
}

}

