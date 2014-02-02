/*
 * Lensifier library OpenGL renderer
 * Copyright (C) 2013, Leszek Godlewski 
 */

#include "GLRenderer.h"
#include "../liblensifier/DOFEffect.h"
#include "../liblensifier/DirtBloomEffect.h"
#include "../liblensifier/TexturedDOFEffect.h"
#include "../liblensifier/WaterDropletsEffect.h"

#include <cstring>
#ifndef NDEBUG
	#include <cstdio>
#endif

namespace Lensifier
{

const GLRenderer::TextureHandle	GLRenderer::InvalidTextureHandle = (GLuint)-1;

const GLfloat GLRenderer::FSQuadVerts[] = {
	-1,	-1,
	-1,	 1,
	 1,	-1,
	 1,	 1,
};

const GLubyte GLRenderer::FSQuadIndices[] = {0, 1, 2, 3};

#define STRINGIFY(x)	#x
const char GLRenderer::VertexShaderPreamble[] =
	"\n#version 120\n"
	"\n#define LENSIFIER_GLSL 1\n"
	"\n#define LENSIFIER_HLSL 0\n"
	#include "shaders/GLSLPreamble.cs"
	#include "shaders/GLSLPreamble.vs"
;
const size_t GLRenderer::VertexShaderPreambleLen = strlen(VertexShaderPreamble);

const char GLRenderer::VertexShaderPostamble[] =
	#include "shaders/GLSLPostamble.vs"
;
const size_t GLRenderer::VertexShaderPostambleLen = strlen(VertexShaderPostamble);

const char GLRenderer::PixelShaderPreamble[] =
	"\n#version 120\n"
	"\n#define LENSIFIER_GLSL 1\n"
	"\n#define LENSIFIER_HLSL 0\n"
	#include "shaders/GLSLPreamble.cs"
	#include "shaders/GLSLPreamble.ps"
;
const size_t GLRenderer::PixelShaderPreambleLen = strlen(PixelShaderPreamble);

const char GLRenderer::PixelShaderPostamble[] =
	#include "shaders/GLSLPostamble.ps"
;
const size_t GLRenderer::PixelShaderPostambleLen = strlen(PixelShaderPostamble);

#define IMPLEMENT_BASE_EFFECT_INTERFACE(TheEffect)							\
	TheEffect ## Effect<GLRenderer> *TheEffect;

#define IMPLEMENT_SINGLE_PASS_EFFECT_INTERFACE(TheEffect)					\
	IMPLEMENT_BASE_EFFECT_INTERFACE(TheEffect)								\
																			\
	void GLRenderer::TheEffect ## BeginSetup()								\
	{																		\
		if (!TheEffect || !TheEffect->GetEnabled())							\
			return;															\
		LGL(UseProgram)(TheEffect->Program);								\
	}																		\
																			\
	void GLRenderer::TheEffect ## EndSetup()								\
	{																		\
		if (!TheEffect || !TheEffect->GetEnabled())							\
			return;															\
		LGL(UseProgram)(0);													\
	}																		\
																			\
	void GLRenderer::TheEffect ## SetEnabled(bool NewEnabled)				\
	{																		\
		if (TheEffect)														\
			TheEffect->SetEnabled(NewEnabled);								\
		else if (NewEnabled)												\
		{																	\
			TheEffect = new TheEffect ## Effect<GLRenderer>();				\
			TheEffect ## BeginSetup();										\
			Setup(ScreenWidth, ScreenHeight,								\
				ColourTexture, DepthTexture);								\
			TheEffect ## EndSetup();										\
			assert(TheEffect->GetEnabled());								\
		}																	\
	}

#define IMPLEMENT_MULTI_PASS_EFFECT_INTERFACE(TheEffect, NumPasses)			\
	IMPLEMENT_BASE_EFFECT_INTERFACE(TheEffect)								\
																			\
	void GLRenderer::TheEffect ## BeginSetup(LUINT Pass)					\
	{																		\
		if (!TheEffect || !TheEffect->GetEnabled())							\
			return;															\
		LGL(UseProgram)(TheEffect->Program[Pass]);							\
	}																		\
																			\
	void GLRenderer::TheEffect ## EndSetup(LUINT Pass)						\
	{																		\
		if (!TheEffect || !TheEffect->GetEnabled())							\
			return;															\
		LGL(UseProgram)(0);													\
	}																		\
																			\
	void GLRenderer::TheEffect ## SetEnabled(bool NewEnabled)				\
	{																		\
		if (TheEffect)														\
			TheEffect->SetEnabled(NewEnabled);								\
		else if (NewEnabled)												\
		{																	\
			TheEffect = new TheEffect ## Effect<GLRenderer>();				\
			TheEffect ## BeginSetup(0);										\
			Setup(ScreenWidth, ScreenHeight,								\
				ColourTexture, DepthTexture);								\
			TheEffect ## EndSetup(0);										\
			assert(TheEffect->GetEnabled());								\
		}																	\
	}

IMPLEMENT_SINGLE_PASS_EFFECT_INTERFACE(DOF)
#define OP_PER_PARAM(Type, Name, Default)									\
void GLRenderer::DOFSet ## Name(Type Value) {if (DOF) DOF->Name.Set(Value);}
#include "../liblensifier/DOFEffect.h"
#undef OP_PER_PARAM

IMPLEMENT_MULTI_PASS_EFFECT_INTERFACE(DirtBloom, 2)
#define OP_PER_PARAM(Pass, Type, Name, Default)									\
void GLRenderer::DirtBloomSet ## Name(Type Value) {if (DirtBloom) DirtBloom->Name.Set(Value);}
#include "../liblensifier/DirtBloomEffect.h"
#undef OP_PER_PARAM

IMPLEMENT_SINGLE_PASS_EFFECT_INTERFACE(TexturedDOF)
#define OP_PER_PARAM(Type, Name, Default)									\
void GLRenderer::TexturedDOFSet ## Name(Type Value) {if (TexturedDOF) TexturedDOF->Name.Set(Value);}
#include "../liblensifier/TexturedDOFEffect.h"
#undef OP_PER_PARAM

IMPLEMENT_SINGLE_PASS_EFFECT_INTERFACE(WaterDroplets)
#define OP_PER_PARAM(Type, Name, Default)									\
void GLRenderer::WaterDropletsSet ## Name(Type Value) {if (WaterDroplets) WaterDroplets->Name.Set(Value);}
#include "../liblensifier/WaterDropletsEffect.h"
#undef OP_PER_PARAM

GLRenderer::GLRenderer()
	: GaussianBlurSceneColour(0)
	, GaussianBlurTexelSize(Vector2())
	, GaussianBlurHorizontal(false)
{
	GaussianBlur = CompileProgram(EffectGenericVertexShader,
		SingleDirGaussianBlurPixelShader);
	// HACK!!! this is so that GRenderer is valid while Register() is called
	Renderer *PrevRenderer = GRenderer;
	GRenderer = this;
	GaussianBlurSceneColour.Register(GaussianBlur, "SceneColour");
	GaussianBlurTexelSize.Register(GaussianBlur, "TexelSize");
	GaussianBlurHorizontal.Register(GaussianBlur, "Horizontal");
	GRenderer = PrevRenderer;
}

GLRenderer::~GLRenderer()
{
	ReleaseProgram(GaussianBlur);
}

/** Notification issued by the library that the configuration has changed. */
void GLRenderer::Setup(LUINT InScreenWidth, LUINT InScreenHeight,
		void *InColourTexture, void *InDepthTexture)
{
	if (DOF)
	{
		DOF->SceneColour.Set(InColourTexture);
		DOF->SceneDepth.Set(InDepthTexture);
		DOF->ScreenSize.Set(Vector2((float)InScreenWidth, (float)InScreenHeight));
		DOF->TexelSize.Set(Vector2(1.f / InScreenWidth, 1.f / InScreenHeight));
	}
	if (DirtBloom)
	{
		DirtBloom->SceneColour.Set(InColourTexture);
		DirtBloom->FullRes.Set(InColourTexture);
	}
	if (TexturedDOF)
	{
		TexturedDOF->SceneColour.Set(InColourTexture);
		TexturedDOF->SceneDepth.Set(InDepthTexture);
		//TexturedDOF->TexelSize.Set(Vector2(1.f / InScreenWidth, 1.f / InScreenHeight));
		TexturedDOF->TexelSize.Set(Vector2(1.f / 256, 1.f / 256));
		if (InScreenWidth != ScreenWidth || InScreenHeight != ScreenHeight)
		{
			if (TexturedDOF->ParticleIndices != 0)
				LGL(DeleteBuffers)(1, &TexturedDOF->ParticleIndices);
			if (TexturedDOF->ParticleVertices != 0)
				LGL(DeleteBuffers)(1, &TexturedDOF->ParticleVertices);
		}
		//TexturedDOF->RegenerateGeometry(InScreenWidth, InScreenHeight);
		TexturedDOF->RegenerateGeometry(256, 256);
	}
	if (WaterDroplets)
	{
		WaterDroplets->SceneColour.Set(InColourTexture);
		WaterDroplets->ScreenSize.Set(Vector2((float)InScreenWidth, (float)InScreenHeight));
	}
	
	Renderer::Setup(InScreenWidth, InScreenHeight, InColourTexture, InDepthTexture);
}

/** Renders the configured effects. */
void GLRenderer::Render()
{
	if (DOF && DOF->GetEnabled())
	{
		SetRenderTarget(RT_BackBuffer);
		LGL(UseProgram)(DOF->Program);
		DrawFullScreenQuad();
	}
	if (DirtBloom && DirtBloom->GetEnabled())
	{
		// bright pass to half-res scratch space
		SetRenderTarget(RT_ScratchSpace, 1);
		LGL(UseProgram)(DirtBloom->Program[0]);
		DrawFullScreenQuad();
		// separable gaussian blur - horizontal
		LGL(UseProgram)(GaussianBlur);
		GaussianBlurSceneColour.Set(DirtBloom->HalfRes.Get());
		GaussianBlurTexelSize.Set(Vector2(3.f / 640.f, 3.f / 360.f));
		GaussianBlurHorizontal.Set(true);
		DrawFullScreenQuad();
		// separable gaussian blur - vertical
		GaussianBlurHorizontal.Set(false);
		DrawFullScreenQuad();
		// composite blur onto scene image
		SetRenderTarget(RT_BackBuffer);
		LGL(UseProgram)(DirtBloom->Program[1]);
		DrawFullScreenQuad();
	}
	if (TexturedDOF && TexturedDOF->GetEnabled())
	{
		SetRenderTarget(RT_ScratchSpace, 0);
		LGL(Clear)(GL_COLOR_BUFFER_BIT);
		LGL(UseProgram)(TexturedDOF->Program);
		LGL(Enable)(GL_BLEND);
		LGL(BlendFunc)(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#if TEXTURED_DOF_TRIANGLE_STRIP
		DrawBuffers(Renderer::PT_TriangleStrip,
			TexturedDOF->ParticleIndices, sizeof(LUINT), TexturedDOF->ParticleIndexCount,
			TexturedDOF->ParticleVertices, sizeof(TexturedDOFEffect<GLRenderer>::VertLayout),
			2, 2, offsetof(TexturedDOFEffect<GLRenderer>::VertLayout, Norm[0]));
#else
		DrawBuffers(Renderer::PT_Points,
			TexturedDOF->ParticleIndices, sizeof(LUINT), TexturedDOF->ParticleIndexCount,
			TexturedDOF->ParticleVertices, sizeof(TexturedDOFEffect<GLRenderer>::VertLayout), 2);
#endif
		LGL(Disable)(GL_BLEND);
		SetRenderTarget(RT_BackBuffer);
		BlitFromScratchSpace(0);
	}
	if (WaterDroplets && WaterDroplets->GetEnabled())
	{
		SetRenderTarget(RT_BackBuffer);
		LGL(UseProgram)(WaterDroplets->Program);
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
	GLuint PS = LGL(CreateShader)(GL_FRAGMENT_SHADER);
	
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
