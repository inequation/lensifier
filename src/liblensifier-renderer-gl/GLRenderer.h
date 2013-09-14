/*
 * Lensifier library OpenGL renderer
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "../liblensifier/Renderer.h"

#ifndef LGL
	// OpenGL function prefix
	#define LGL(f)	gl##f
	
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

namespace Lensifier
{

class GLRenderer : public Renderer
{
public:
	GLRenderer();
	virtual ~GLRenderer();
	
	/** Notification issued by the library that the configuration has changed. */
	virtual void OnConfigChanged(LensifierConfig *OldConfig, LensifierConfig *NewConfig);

	/** Sets and binds the scene texture slots. */
	virtual void SetSceneTextureSlots(LUINT ColourTextureSlot, LUINT DepthTextureSlot);
	
	/** Renders the Depth of Field effect using current configuration. */
	virtual void RenderDOF();

private:
	GLuint	SceneColourLocation;
	GLuint	SceneDepthLocation;
	
	GLuint	SceneColourTexture;
	GLuint	SceneDepthTexture;
	
	GLuint	DOFProgram;
};

}

#endif	// GLRENDERER_H
