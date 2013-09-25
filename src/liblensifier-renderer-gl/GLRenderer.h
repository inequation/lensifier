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
	
	#define GL_GLEXT_PROTOTYPES
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
		
	/**
	 * Adds in the API-specific shader sugar, then compiles and links the shader
	 * sources.
	 * @return	GLSL program name
	 */
	GLuint CompileProgram(const char *VertexShaderSource, const char *PixelShaderSource);
	
	/** Finds the location of the given uniform variable in the given program. */
	inline GLint GetShaderParameter(GLuint Program, const char *ParamName)
	{
		return LGL(GetUniformLocation)(Program, ParamName);
	}
	
	inline void SetShaderParameterValue(GLint Param, const bool Value)
	{LGL(Uniform1i)(Param, (GLint)Value);}	
	inline void SetShaderParameterValue(GLint Param, const GLuint Value)
	{LGL(Uniform1i)(Param, Value);}	
	inline void SetShaderParameterValue(GLint Param, const float Value)
	{LGL(Uniform1f)(Param, Value);}	
	inline void SetShaderParameterValue(GLint Param, const Vector2& Value)
	{LGL(Uniform2fv)(Param, 1, Value.V);}	
	inline void SetShaderParameterValue(GLint Param, const Vector3& Value)
	{LGL(Uniform3fv)(Param, 1, Value.V);}	
	inline void SetShaderParameterValue(GLint Param, const Vector4& Value)
	{LGL(Uniform4fv)(Param, 1, Value.V);}
	
	/** Releases the given shader parameter. No-op in OpenGL. */
	void ReleaseShaderParameter(GLint Param) {}
	
	/** Releases the given program. */
	inline void ReleaseProgram(GLuint Program) {LGL(DeleteProgram)(Program);}
	
	virtual void DOFBeginSetup();
	virtual void DOFEndSetup();
	virtual void DOFSetEnabled(bool);
	#define OP_PER_PARAM(Type, Name, Default) virtual void DOFSet ## Name(Type);
	#include "../liblensifier/DOFEffect.h"
	#undef OP_PER_PARAM
	
	/** Renders the configured effects. */
	virtual void Render();

private:
	inline void DrawFullScreenQuad()
	{
#if 0
		LGL(VertexPointer)(3, GL_FLOAT, 0, FSQuadVerts);
		LGL(DrawElements)(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, FSQuadIndices);
#elif 0
		LGL(Begin)(GL_QUADS);
		LGL(Vertex3f)(-1.0f, 1.0f, 0.0f);
		LGL(Vertex3f)( 1.0f, 1.0f, 0.0f);
		LGL(Vertex3f)( 1.0f,-1.0f, 0.0f);
		LGL(Vertex3f)(-1.0f,-1.0f, 0.0f);
		LGL(End)();
#else
		LGL(Begin)(GL_QUADS);
		LGL(Vertex2f)(-1.0f, 1.0f);
		LGL(Vertex2f)( 1.0f, 1.0f);
		LGL(Vertex2f)( 1.0f,-1.0f);
		LGL(Vertex2f)(-1.0f,-1.0f);
		LGL(End)();
#endif
	}
	
	static const GLfloat FSQuadVerts[];
	static const GLubyte FSQuadIndices[];
	static const char VertexShaderPreamble[];
	static const size_t VertexShaderPreambleLen;
	static const char VertexShaderPostamble[];
	static const size_t VertexShaderPostambleLen;
	static const char PixelShaderPreamble[];
	static const size_t PixelShaderPreambleLen;
	static const char PixelShaderPostamble[];
	static const size_t PixelShaderPostambleLen;
};

}

#endif	// GLRENDERER_H
