/*
 * Lensifier library Direct3D 10 renderer
 * Copyright (C) 2014, Leszek Godlewski 
 */

#include "D3D10Renderer.h"
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

const float D3D10Renderer::FSQuadVerts[] = {
	-1,	-1,
	-1,	 1,
	 1,	-1,
	 1,	 1,
};

const BYTE D3D10Renderer::FSQuadIndices[] = {0, 1, 2, 3};

#define STRINGIFY(x)	#x
const char D3D10Renderer::VertexShaderPreamble[] =
	"\n#define LENSIFIER_HLSL 1\n"
	#include "shaders/HLSLPreamble.cs"
	#include "shaders/HLSLPreamble.vs"
;
const size_t D3D10Renderer::VertexShaderPreambleLen = strlen(VertexShaderPreamble);

const char D3D10Renderer::VertexShaderPostamble[] =
	#include "shaders/HLSLPostamble.vs"
;
const size_t D3D10Renderer::VertexShaderPostambleLen = strlen(VertexShaderPostamble);

const char D3D10Renderer::PixelShaderPreamble[] =
	"\n#version 120\n"
	"\n#define LENSIFIER_HLSL 1\n"
	#include "shaders/HLSLPreamble.cs"
	#include "shaders/HLSLPreamble.ps"
;
const size_t D3D10Renderer::PixelShaderPreambleLen = strlen(PixelShaderPreamble);

const char D3D10Renderer::PixelShaderPostamble[] =
	#include "shaders/HLSLPostamble.ps"
;
const size_t D3D10Renderer::PixelShaderPostambleLen = strlen(PixelShaderPostamble);

#define IMPLEMENT_BASE_EFFECT_INTERFACE(TheEffect)							\
	TheEffect ## Effect<D3D10Renderer> *TheEffect;

#define IMPLEMENT_SINGLE_PASS_EFFECT_INTERFACE(TheEffect)					\
	IMPLEMENT_BASE_EFFECT_INTERFACE(TheEffect)								\
																			\
	void D3D10Renderer::TheEffect ## BeginSetup()							\
	{																		\
		if (!TheEffect || !TheEffect->GetEnabled())							\
			return;															\
	}																		\
																			\
	void D3D10Renderer::TheEffect ## EndSetup()								\
	{																		\
		if (!TheEffect || !TheEffect->GetEnabled())							\
			return;															\
	}																		\
																			\
	void D3D10Renderer::TheEffect ## SetEnabled(bool NewEnabled)			\
	{																		\
		if (TheEffect)														\
			TheEffect->SetEnabled(NewEnabled);								\
		else if (NewEnabled)												\
		{																	\
			TheEffect = new TheEffect ## Effect<D3D10Renderer>();			\
			TheEffect ## BeginSetup();										\
			Setup(ScreenWidth, ScreenHeight,								\
				ColourTextureSlot, DepthTextureSlot);						\
			TheEffect ## EndSetup();										\
			assert(TheEffect->GetEnabled());								\
		}																	\
	}

#define IMPLEMENT_MULTI_PASS_EFFECT_INTERFACE(TheEffect, NumPasses)			\
	IMPLEMENT_BASE_EFFECT_INTERFACE(TheEffect)								\
																			\
	void D3D10Renderer::TheEffect ## BeginSetup(LUINT Pass)					\
	{																		\
		if (!TheEffect || !TheEffect->GetEnabled())							\
			return;															\
	}																		\
																			\
	void D3D10Renderer::TheEffect ## EndSetup(LUINT Pass)					\
	{																		\
		if (!TheEffect || !TheEffect->GetEnabled())							\
			return;															\
	}																		\
																			\
	void D3D10Renderer::TheEffect ## SetEnabled(bool NewEnabled)			\
	{																		\
		if (TheEffect)														\
			TheEffect->SetEnabled(NewEnabled);								\
		else if (NewEnabled)												\
		{																	\
			TheEffect = new TheEffect ## Effect<D3D10Renderer>();			\
			TheEffect ## BeginSetup(0);										\
			Setup(ScreenWidth, ScreenHeight,								\
				ColourTextureSlot, DepthTextureSlot);						\
			TheEffect ## EndSetup(0);										\
			assert(TheEffect->GetEnabled());								\
		}																	\
	}

IMPLEMENT_SINGLE_PASS_EFFECT_INTERFACE(DOF)
#define OP_PER_PARAM(Type, Name, Default)									\
void D3D10Renderer::DOFSet ## Name(Type Value) {if (DOF) DOF->Name.Set(Value);}
#include "../liblensifier/DOFEffect.h"
#undef OP_PER_PARAM

IMPLEMENT_MULTI_PASS_EFFECT_INTERFACE(DirtBloom, 2)
#define OP_PER_PARAM(Pass, Type, Name, Default)								\
void D3D10Renderer::DirtBloomSet ## Name(Type Value) {if (DirtBloom) DirtBloom->Name.Set(Value);}
#include "../liblensifier/DirtBloomEffect.h"
#undef OP_PER_PARAM

IMPLEMENT_SINGLE_PASS_EFFECT_INTERFACE(TexturedDOF)
#define OP_PER_PARAM(Type, Name, Default)									\
void D3D10Renderer::TexturedDOFSet ## Name(Type Value) {if (TexturedDOF) TexturedDOF->Name.Set(Value);}
#include "../liblensifier/TexturedDOFEffect.h"
#undef OP_PER_PARAM

IMPLEMENT_SINGLE_PASS_EFFECT_INTERFACE(WaterDroplets)
#define OP_PER_PARAM(Type, Name, Default)									\
void D3D10Renderer::WaterDropletsSet ## Name(Type Value) {if (WaterDroplets) WaterDroplets->Name.Set(Value);}
#include "../liblensifier/WaterDropletsEffect.h"
#undef OP_PER_PARAM

D3D10Renderer::D3D10Renderer(void *InDevice)
{
	/*GaussianBlur = CompileProgram(EffectGenericVertexShader,
		SingleDirGaussianBlurPixelShader);*/
	// HACK!!! this is so that GRenderer is valid while Register() is called
	Renderer *PrevRenderer = GRenderer;
	GRenderer = this;
	/*GaussianBlurSceneColour.Register(GaussianBlur, "SceneColour");
	GaussianBlurTexelSize.Register(GaussianBlur, "TexelSize");
	GaussianBlurHorizontal.Register(GaussianBlur, "Horizontal");*/
	GRenderer = PrevRenderer;
}

D3D10Renderer::~D3D10Renderer()
{
	//ReleaseProgram(GaussianBlur);
}

/** Notification issued by the library that the configuration has changed. */
void D3D10Renderer::Setup(LUINT InScreenWidth, LUINT InScreenHeight,
		LUINT InColourTextureSlot, LUINT InDepthTextureSlot)
{
#if 0
	if (DOF)
	{
		DOF->SceneColour.Set(InColourTextureSlot);
		DOF->SceneDepth.Set(InDepthTextureSlot);
		DOF->ScreenSize.Set(Vector2(InScreenWidth, InScreenHeight));
		DOF->TexelSize.Set(Vector2(1.f / InScreenWidth, 1.f / InScreenHeight));
	}
	if (DirtBloom)
	{
		DirtBloom->SceneColour.Set(InColourTextureSlot);
		DirtBloom->FullRes.Set(InColourTextureSlot);
	}
	if (TexturedDOF)
	{
		TexturedDOF->SceneColour.Set(InColourTextureSlot);
		TexturedDOF->SceneDepth.Set(InDepthTextureSlot);
		//TexturedDOF->TexelSize.Set(Vector2(1.f / InScreenWidth, 1.f / InScreenHeight));
		TexturedDOF->TexelSize.Set(Vector2(1.f / 256, 1.f / 256));
		if (InScreenWidth != ScreenWidth || InScreenHeight != ScreenHeight)
		{
			/*if (TexturedDOF->ParticleIndices != 0)
				LGL(DeleteBuffers)(1, &TexturedDOF->ParticleIndices);
			if (TexturedDOF->ParticleVertices != 0)
				LGL(DeleteBuffers)(1, &TexturedDOF->ParticleVertices);*/
		}
		//TexturedDOF->RegenerateGeometry(InScreenWidth, InScreenHeight);
		TexturedDOF->RegenerateGeometry(256, 256);
	}
	if (WaterDroplets)
	{
		WaterDroplets->SceneColour.Set(InColourTextureSlot);
		WaterDroplets->ScreenSize.Set(Vector2(InScreenWidth, InScreenHeight));
	}
#endif

	Renderer::Setup(InScreenWidth, InScreenHeight, InColourTextureSlot, InDepthTextureSlot);
}

/** Renders the configured effects. */
void D3D10Renderer::Render()
{
#if 0
	if (DOF && DOF->GetEnabled())
	{
		SetRenderTarget(RT_BackBuffer);
		DrawFullScreenQuad();
	}
	if (DirtBloom && DirtBloom->GetEnabled())
	{
		// bright pass to half-res scratch space
		SetRenderTarget(RT_ScratchSpace, 1);
		DrawFullScreenQuad();
		// separable gaussian blur - horizontal
		GaussianBlurSceneColour.Set(DirtBloom->HalfRes.Get());
		GaussianBlurTexelSize.Set(Vector2(3.f / 640.f, 3.f / 360.f));
		GaussianBlurHorizontal.Set(true);
		DrawFullScreenQuad();
		// separable gaussian blur - vertical
		GaussianBlurHorizontal.Set(false);
		DrawFullScreenQuad();
		// composite blur onto scene image
		SetRenderTarget(RT_BackBuffer);
		DrawFullScreenQuad();
	}
	if (TexturedDOF && TexturedDOF->GetEnabled())
	{
		SetRenderTarget(RT_ScratchSpace, 0);
#if TEXTURED_DOF_TRIANGLE_STRIP
		DrawBuffers(Renderer::PT_TriangleStrip,
			TexturedDOF->ParticleIndices, sizeof(LUINT), TexturedDOF->ParticleIndexCount,
			TexturedDOF->ParticleVertices, sizeof(TexturedDOFEffect<D3D10Renderer>::VertLayout),
			2, 2, offsetof(TexturedDOFEffect<D3D10Renderer>::VertLayout, Norm[0]));
#else
		DrawBuffers(Renderer::PT_Points,
			TexturedDOF->ParticleIndices, sizeof(LUINT), TexturedDOF->ParticleIndexCount,
			TexturedDOF->ParticleVertices, sizeof(TexturedDOFEffect<D3D10Renderer>::VertLayout), 2);
#endif
		SetRenderTarget(RT_BackBuffer);
		BlitFromScratchSpace(0);
	}
	if (WaterDroplets && WaterDroplets->GetEnabled())
	{
		SetRenderTarget(RT_BackBuffer);
		DrawFullScreenQuad();
	}
#endif
}

}
