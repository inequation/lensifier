/*
 * Lensifier library OpenGL renderer
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "../liblensifier/Renderer.h"
#include "../liblensifier/Utils.h"

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
	// these definitions are required by the templated Effect class
	typedef GLuint	ProgramHandle;
	typedef GLint	ShaderParameterHandle;
	
	GLRenderer();
	virtual ~GLRenderer();
	
	/** Notification issued by the library that the configuration has changed. */
	virtual void Setup(LUINT InScreenWidth, LUINT InScreenHeight,
		LUINT ColourTextureSlot, LUINT DepthTextureSlot);
	
	/** Renders the Depth of Field effect using current configuration. */
	virtual void RenderDOF();
	
	/**
	 * Adds in the API-specific shader sugar, then compiles and links the shader
	 * sources.
	 * @return	GLSL program name
	 */
	GLuint CompileProgram(const char *VertexShaderSource, const char *PixelShaderSource);
	
	/** Finds the location of the given uniform variable in the given program. */
	inline GLint GetShaderParameter(GLuint Program, const char *ParamName)
	{
		return -1;
	}
	
	inline void SetShaderParameterValue(GLint Param, const bool Value)
	{LGL(Uniform1i)(Param, (GLint)Value);}	
	inline void SetShaderParameterValue(GLint Param, const GLuint Value)
	{LGL(Uniform1i)(Param, Value);}	
	inline void SetShaderParameterValue(GLint Param, const float Value)
	{LGL(Uniform1f)(Param, Value);}	
	inline void SetShaderParameterValue(GLint Param, const Vector2& Value)
	{LGL(Uniform2fv)(Param, Value.V);}	
	inline void SetShaderParameterValue(GLint Param, const Vector3& Value)
	{LGL(Uniform3fv)(Param, Value.V);}	
	inline void SetShaderParameterValue(GLint Param, const Vector4& Value)
	{LGL(Uniform4fv)(Param, Value.V);}
	
	/** Releases the given shader parameter. No-op in OpenGL. */
	void ReleaseShaderParameter(GLint Param) {}
	
	/** Releases the given program. */
	void ReleaseProgram(GLuint Program);

private:
	LUINT	ScreenWidth, ScreenHeight;
	
	GLuint	SceneColourTexture, SceneDepthTexture;
	
	GLuint	DOFProgram;
};

}

#endif	// GLRENDERER_H
