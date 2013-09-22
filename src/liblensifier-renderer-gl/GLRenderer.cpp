/*
 * Lensifier library OpenGL renderer
 * Copyright (C) 2013, Leszek Godlewski 
 */

#include "GLRenderer.h"
#include "../liblensifier/DOFEffect.h"
#include <cstring>
#ifndef NDEBUG
	#include <cstdio>
#endif

namespace Lensifier
{

const GLubyte GLRenderer::FSQuadVerts[] = {
	0,		0,
	0,		255,
	255,	0,
	255,	255,
};

const GLubyte GLRenderer::FSQuadIndices[] = {0, 1, 2, 3};

#define STRINGIFY(x)	#x
const char GLRenderer::VertexShaderPreamble[] =
	#include "shaders/GLSLPreamble.vs"
;
const size_t GLRenderer::VertexShaderPreambleLen = strlen(VertexShaderPreamble);
const char GLRenderer::VertexShaderPostamble[] =
	#include "shaders/GLSLPostamble.vs"
;
const size_t GLRenderer::VertexShaderPostambleLen = strlen(VertexShaderPostamble);
const char GLRenderer::PixelShaderPreamble[] =
	#include "shaders/GLSLPreamble.ps"
;
const size_t GLRenderer::PixelShaderPreambleLen = strlen(PixelShaderPreamble);
const char GLRenderer::PixelShaderPostamble[] =
	#include "shaders/GLSLPostamble.ps"
;
const size_t GLRenderer::PixelShaderPostambleLen = strlen(PixelShaderPostamble);

DOFEffect<GLRenderer> *DOF;

GLRenderer::GLRenderer()
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
		DOF->ScreenSize.Set(Vector2(InScreenWidth, InScreenHeight));
		DOF->TexelSize.Set(Vector2(1.f / InScreenWidth, 1.f / InScreenHeight));
	}
}

void GLRenderer::DOFSetEnabled(bool NewEnabled)
{
	if (DOF)
		DOF->SetEnabled(NewEnabled);
	else if (NewEnabled)
	{
		DOF = new DOFEffect<GLRenderer>();
		assert(DOF->GetEnabled());
	}
}

#define OP_PER_PARAM(Type, Name, Default)									\
void GLRenderer::DOFSet ## Name(Type Value) {if (DOF) DOF->Name.Set(Value);}
#include "../liblensifier/DOFEffect.h"
#undef OP_PER_PARAM

/** Renders the configured effects. */
void GLRenderer::Render()
{
	if (DOF && DOF->GetEnabled())
	{
		LGL(UseProgram)(DOF->Program);
		DrawFullScreenQuad();
	}
}

GLuint GLRenderer::CompileProgram(const char *VertexShaderSource, const char *PixelShaderSource)
{
	const char *VertexSources[] = {
		VertexShaderPreamble,
		VertexShaderSource,
		VertexShaderPostamble
	};
	
	const GLint VertexSourceLengths[] = {
		(GLint)VertexShaderPreambleLen,
		(GLint)strlen(VertexShaderSource),
		(GLint)VertexShaderPostambleLen
	};
	
	const char *PixelSources[] = {
		PixelShaderPreamble,
		PixelShaderSource,
		PixelShaderPostamble
	};
	
	const GLint PixelSourceLengths[] = {
		(GLint)PixelShaderPreambleLen,
		(GLint)strlen(PixelShaderSource),
		(GLint)PixelShaderPostambleLen
	};
	
	GLuint VS = LGL(CreateShader)(GL_VERTEX_SHADER);
	GLuint PS = LGL(CreateShader)(GL_VERTEX_SHADER);
	
	LGL(ShaderSource)(VS, 3, VertexSources, VertexSourceLengths);
	LGL(ShaderSource)(PS, 3, PixelSources, PixelSourceLengths);
	
	LGL(CompileShader)(VS);
	LGL(CompileShader)(PS);
	
#ifndef NDEBUG
	GLint Status, LogLen;
	LGL(GetShaderiv)(VS, GL_COMPILE_STATUS, &Status);
	if (Status == GL_FALSE)
	{
		LGL(GetShaderiv)(VS, GL_INFO_LOG_LENGTH, &LogLen);
		char *Log = new char[LogLen + 1];		
		LGL(GetShaderInfoLog)(VS, LogLen, nullptr, Log);
		printf("Shader info log:\n%s\n", Log);
		assert(0);
	}
	LGL(GetShaderiv)(PS, GL_COMPILE_STATUS, &Status);
	if (Status == GL_FALSE)
	{
		LGL(GetShaderiv)(PS, GL_INFO_LOG_LENGTH, &LogLen);
		char *Log = new char[LogLen + 1];
		LGL(GetShaderInfoLog)(PS, LogLen, nullptr, Log);
		printf("Shader info log:\n%s\n", Log);
		assert(0);
	}
#endif

	GLuint Program = LGL(CreateProgram)();
	LGL(AttachShader)(Program, VS);
	LGL(AttachShader)(Program, PS);
	LGL(LinkProgram)(Program);

#ifndef NDEBUG
	LGL(GetProgramiv)(Program, GL_LINK_STATUS, &Status);
	if (Status == GL_FALSE)
	{
		LGL(GetProgramiv)(Program, GL_INFO_LOG_LENGTH, &LogLen);
		char *Log = new char[LogLen + 1];
		LGL(GetProgramInfoLog)(Program, LogLen, nullptr, Log);
		printf("Program info log:\n%s\n", Log);
		assert(0);
	}
#endif
	
	return Program;
}

}
