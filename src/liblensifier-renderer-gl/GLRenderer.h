/*
 * Lensifier library OpenGL renderer
 * Copyright (C) 2013, Leszek Godlewski 
 */

#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "../liblensifier/Renderer.h"
#include "../liblensifier/Utils.h"

#if defined(_WIN32) || defined(WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN 1
		#define __UNDEF_WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>
	#ifdef __UNDEF_WIN32_LEAN_AND_MEAN
		#undef WIN32_LEAN_AND_MEAN
		#undef __UNDEF_WIN32_LEAN_AND_MEAN
	#endif
#endif

#ifndef LGL
	// OpenGL function prefix
	#define LGL(f)	gl##f
	
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

#include <map>

namespace Lensifier
{

class GLRenderer : public Renderer
{
public:
	// these definitions are required by the templated Effect class
	typedef GLuint	ProgramHandle;
	typedef GLint	ShaderParameterHandle;
	typedef GLuint	IndexBufferHandle;
	typedef GLuint	VertexBufferHandle;
	typedef GLuint	TextureHandle;

	static const TextureHandle	InvalidTextureHandle;
	
	GLRenderer();
	virtual ~GLRenderer();
	
	/** Notification issued by the library that the configuration has changed. */
	virtual void Setup(LUINT InScreenWidth, LUINT InScreenHeight,
		void *ColourTexture, void *DepthTexture);
		
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
	inline void SetShaderParameterValue(GLint Param, const void *Value)
	{LGL(Uniform1i)(Param, (GLuint)Value);}	
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
	inline void ReleaseProgram(GLuint Program)
	{
		GLuint Shaders[3];
		GLsizei ShaderCount;
		LGL(GetAttachedShaders)(Program, sizeof(Shaders) / sizeof(Shaders[0]),
			&ShaderCount, Shaders);
		for (GLsizei i = 0; i < ShaderCount; ++i)
			LGL(DetachShader)(Program, Shaders[i]);
		LGL(DeleteProgram)(Program);
	}
	
	inline GLuint UploadIndicesToBuffer(void *Data, size_t Count, size_t ElementSize)
	{
		return UploadToBuffer(GL_ELEMENT_ARRAY_BUFFER, Data, Count, ElementSize);
	}
	
	inline GLuint UploadVerticesToBuffer(void *Data, size_t Count, size_t ElementSize)
	{
		return UploadToBuffer(GL_ARRAY_BUFFER, Data, Count, ElementSize);
	}
	
	inline void DrawBuffers(Renderer::PrimitiveType Primitive,
		GLuint IndexBufferHandle, size_t IndexStride, size_t IndexCount,
		GLuint VertexBufferHandle, size_t VertexStride, size_t PositionSize,
		size_t TexCoordsSize = 0, size_t TexCoordsOffset = 0,
		size_t NormalSize = 0, size_t NormalOffset = 0)
	{
		GLenum PrimType, IndexType;
		
		switch (Primitive)
		{
			case PT_Points:			PrimType = GL_POINTS;			break;
			case PT_Triangles:		PrimType = GL_TRIANGLES;		break;
			case PT_TriangleStrip:	PrimType = GL_TRIANGLE_STRIP;	break;
			case PT_TriangleFan:	PrimType = GL_TRIANGLE_FAN;		break;
			default:				PrimType = GL_TRIANGLES;		break;
		}
		switch (IndexStride)
		{
			case 1:		IndexType = GL_UNSIGNED_BYTE;	break;
			case 2:		IndexType = GL_UNSIGNED_SHORT;	break;
			case 4:		IndexType = GL_UNSIGNED_INT;	break;
			default:	IndexType = GL_UNSIGNED_BYTE;	break;
		}
		
		/*LGL(EnableVertexAttribArray)(0);
		LGL(EnableVertexAttribArray)(1);*/
		
		LGL(BindBuffer)(GL_ARRAY_BUFFER, VertexBufferHandle);
		LGL(BindBuffer)(GL_ELEMENT_ARRAY_BUFFER, IndexBufferHandle);
		
		// bind positions
		LGL(EnableClientState)(GL_VERTEX_ARRAY);
		//LGL(VertexAttribPointer)(0, PositionSize, GL_FLOAT, GL_FALSE, VertexStride, NULL);
		LGL(VertexPointer)(PositionSize, GL_FLOAT, VertexStride, NULL);
		// bind tex coords
		if (TexCoordsSize > 0)
		{
			LGL(EnableClientState)(GL_TEXTURE_COORD_ARRAY);
			//LGL(VertexAttribPointer)(1, NormalSize, GL_FLOAT, GL_FALSE, VertexStride, (GLvoid *)NormalOffset);
			LGL(TexCoordPointer)(TexCoordsSize, GL_FLOAT, VertexStride, (GLvoid *)TexCoordsOffset);
		}
		// bind normals
		if (NormalSize > 0)
		{
			assert(NormalSize == 3);
			LGL(EnableClientState)(GL_NORMAL_ARRAY);
			//LGL(VertexAttribPointer)(2, NormalSize, GL_FLOAT, GL_FALSE, VertexStride, (GLvoid *)NormalOffset);
			LGL(NormalPointer)(GL_FLOAT, VertexStride, (GLvoid *)NormalOffset);
		}
		
		LGL(DrawElements)(PrimType, IndexCount, IndexType, NULL);
		
		LGL(BindBuffer)(GL_ARRAY_BUFFER, 0);
		LGL(BindBuffer)(GL_ELEMENT_ARRAY_BUFFER, 0);
		/*LGL(DisableVertexAttribArray)(1);
		LGL(DisableVertexAttribArray)(0);*/
		if (NormalSize > 0)
			LGL(DisableClientState)(GL_NORMAL_ARRAY);
		if (TexCoordsSize > 0)
			LGL(DisableClientState)(GL_TEXTURE_COORD_ARRAY);
		LGL(DisableClientState)(GL_VERTEX_ARRAY);		
	}
	
	virtual void DOFBeginSetup();
	virtual void DOFEndSetup();
	virtual void DOFSetEnabled(bool);
	#define OP_PER_PARAM(Type, Name, Default) virtual void DOFSet ## Name(Type);
	#include "../liblensifier/DOFEffect.h"
	#undef OP_PER_PARAM
	
	virtual void DirtBloomBeginSetup(LUINT Pass);
	virtual void DirtBloomEndSetup(LUINT Pass);
	virtual void DirtBloomSetEnabled(bool);
	#define OP_PER_PARAM(Pass, Type, Name, Default) virtual void DirtBloomSet ## Name(Type);
	#include "../liblensifier/DirtBloomEffect.h"
	#undef OP_PER_PARAM
	
	virtual void TexturedDOFBeginSetup();
	virtual void TexturedDOFEndSetup();
	virtual void TexturedDOFSetEnabled(bool);
	#define OP_PER_PARAM(Type, Name, Default) virtual void TexturedDOFSet ## Name(Type);
	#include "../liblensifier/TexturedDOFEffect.h"
	#undef OP_PER_PARAM
	
	virtual void WaterDropletsBeginSetup();
	virtual void WaterDropletsEndSetup();
	virtual void WaterDropletsSetEnabled(bool);
	#define OP_PER_PARAM(Type, Name, Default) virtual void WaterDropletsSet ## Name(Type);
	#include "../liblensifier/WaterDropletsEffect.h"
	#undef OP_PER_PARAM
	
	/** Renders the configured effects. */
	virtual void Render();

private:
	inline void DrawFullScreenQuad()
	{
#if 1
		LGL(EnableClientState)(GL_VERTEX_ARRAY);
		LGL(VertexPointer)(2, GL_FLOAT, 0, FSQuadVerts);
		LGL(DrawElements)(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, FSQuadIndices);
		LGL(DisableClientState)(GL_VERTEX_ARRAY);
#else
		LGL(Begin)(GL_QUADS);
		LGL(Vertex2f)(-1.0f, 1.0f);
		LGL(Vertex2f)( 1.0f, 1.0f);
		LGL(Vertex2f)( 1.0f,-1.0f);
		LGL(Vertex2f)(-1.0f,-1.0f);
		LGL(End)();
#endif
	}
	
	inline GLuint UploadToBuffer(GLenum BufferType, void *Data, size_t Count, size_t ElementSize)
	{
		LUINT Buffer;
		LGL(GenBuffers)(1, &Buffer);
		LGL(BindBuffer)(BufferType, Buffer);
		LGL(BufferData)(BufferType, Count * ElementSize, Data, GL_STATIC_DRAW);
		LGL(BindBuffer)(BufferType, 0);
		return Buffer;
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
	
	GLuint GaussianBlur;
	CachedShaderParam<GLRenderer, void *> GaussianBlurSceneColour;
	CachedShaderParam<GLRenderer, Vector2> GaussianBlurTexelSize;
	CachedShaderParam<GLRenderer, bool> GaussianBlurHorizontal;
};

}

#endif	// GLRENDERER_H
