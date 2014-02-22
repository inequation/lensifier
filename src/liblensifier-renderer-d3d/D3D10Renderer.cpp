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

#include <D3DCompiler.h>

namespace Lensifier
{

const D3D10Renderer::TextureHandle	D3D10Renderer::InvalidTextureHandle = NULL;

// NOTE: MUST match the declaration in HLSLPreamble.vs!!!
const D3D10_INPUT_ELEMENT_DESC	D3D10Renderer::ElementDescs[] =
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 28,	D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 44,	D3D10_INPUT_PER_VERTEX_DATA, 0},
};

using namespace D3D10Helpers;

#define STRINGIFY(x)	#x
const char D3D10Renderer::VertexShaderPreamble[] =
	#include "shaders/HLSLPreamble.cs"
	"\n"
	#include "shaders/HLSLPreamble.vs"
;
const size_t D3D10Renderer::VertexShaderPreambleLen = strlen(VertexShaderPreamble);

const char D3D10Renderer::VertexShaderPostamble[] =
	#include "shaders/HLSLPostamble.vs"
;
const size_t D3D10Renderer::VertexShaderPostambleLen = strlen(VertexShaderPostamble);

const char D3D10Renderer::PixelShaderPreamble[] =
	#include "shaders/HLSLPreamble.cs"
	"\n"
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
				ColourTexture, DepthTexture);								\
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
				ColourTexture, DepthTexture);								\
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
	: Device((ID3D10Device *)InDevice)
	, GenericVS(NULL)
	, GenericVSBlob(NULL)
	, GaussianBlurSceneColour(NULL)
	, GaussianBlurTexelSize(Vector2())
	, GaussianBlurHorizontal(false)
	, FSQuadVB(NULL)
{
	// initialize the full-screen quad data
	D3D10_BUFFER_DESC BufDesc;
	D3D10_SUBRESOURCE_DATA InitData;
	static const VertexInput Vertices[3] = 
	{
		{
			{-1.f,	 1.f,	 0.f,	 0.f},	// POSITION
			{ 0.f,	 0.f,	 0.f,	 0.f},	// TEXCOORD0
			{ 0.f,	 0.f,	 1.f},			// NORMAL
			{ 1.f,	 1.f,	 1.f,	 1.f},	// COLOR0
		},
		{
			{ 3.f,	 1.f,	 0.f,	 0.f},	// POSITION
			{ 2.f,	 0.f,	 0.f,	 0.f},	// TEXCOORD0
			{ 0.f,	 0.f,	 1.f},			// NORMAL
			{ 1.f,	 1.f,	 1.f,	 1.f},	// COLOR0
		},
		{
			{-1.f,	-3.f,	 0.f,	 0.f},	// POSITION
			{ 0.f,	 2.f,	 0.f,	 0.f},	// TEXCOORD0
			{ 0.f,	 0.f,	 1.f},			// NORMAL
			{ 1.f,	 1.f,	 1.f,	 1.f},	// COLOR0
		},
	};

	BufDesc.Usage					= D3D10_USAGE_IMMUTABLE;
    BufDesc.ByteWidth				= sizeof(Vertices);
    BufDesc.BindFlags				= D3D10_BIND_VERTEX_BUFFER;
    BufDesc.CPUAccessFlags			= 0;
    BufDesc.MiscFlags				= 0;
	InitData.pSysMem				= Vertices;

	HRESULT Result = Device->CreateBuffer(&BufDesc, &InitData, &FSQuadVB);

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

D3D10Renderer::~D3D10Renderer()
{
	if (FSQuadVB)
		FSQuadVB->Release();

	ReleaseProgram(GaussianBlur);

	if (GenericVS)
		GenericVS->Release();
	if (GenericVSBlob)
		GenericVSBlob->Release();
}

/** Notification issued by the library that the configuration has changed. */
void D3D10Renderer::Setup(LUINT InScreenWidth, LUINT InScreenHeight,
		void *InColourTexture, void *InDepthTexture)
{
	if (DOF)
	{
		DOF->SceneColour.Set((TextureHandle)InColourTexture);
		DOF->SceneDepth.Set((TextureHandle)InDepthTexture);
		DOF->ScreenSize.Set(Vector2((float)InScreenWidth, (float)InScreenHeight));
		DOF->TexelSize.Set(Vector2(1.f / InScreenWidth, 1.f / InScreenHeight));
	}
	if (DirtBloom)
	{
		DirtBloom->SceneColour.Set((TextureHandle)InColourTexture);
		DirtBloom->FullRes.Set((TextureHandle)InColourTexture);
	}
	if (TexturedDOF)
	{
		TexturedDOF->SceneColour.Set((TextureHandle)InColourTexture);
		TexturedDOF->SceneDepth.Set((TextureHandle)InDepthTexture);
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
		WaterDroplets->SceneColour.Set((TextureHandle)InColourTexture);
		WaterDroplets->ScreenSize.Set(Vector2((float)InScreenWidth, (float)InScreenHeight));
	}

	Renderer::Setup(InScreenWidth, InScreenHeight, InColourTexture, InDepthTexture);
}

ID3D10Blob *D3D10Renderer::CompileShader(const char *Source, const char *Profile)
{
	static const D3D10_SHADER_MACRO Macros[] = {
		{"LENSIFIER_GLSL",	"0"},
		{"LENSIFIER_HLSL",	"40"},	//	Shader Model 4.0

		// type aliases
		{"vec2",			"float2"},
		{"vec3",			"float3"},
		{"vec4",			"float4"},
		{"mat2",			"float2x2"},
		{"mat3",			"float3x3"},
		{"mat4",			"float4x4"},
		{"Sampler1D",		"Texture1D<float4>"},
		{"Sampler2D",		"Texture2D<float4>"},
		{"Sampler3D",		"Texture3D<float4>"},
		//{"Sampler4D",		"Texture4D<float4>"},

		// function aliases
		{"fract",			"frac"},
		{"mix",				"lerp"},
		{"dFdx",			"ddx"},
		{"dFdy",			"ddy"},

		{NULL,				NULL}
	};
	ID3D10Blob *Blob;
#ifndef NDEBUG
	ID3D10Blob *Errors;
#endif

	static const UINT Flags = D3D10_SHADER_OPTIMIZATION_LEVEL2
#ifndef NDEBUG
		| D3D10_SHADER_DEBUG | D3D10_SHADER_WARNINGS_ARE_ERRORS
#endif
		;

	bool IsVertexShader = Profile[0] == 'v';

	// concatenate sources
	const size_t CompleteLength = (IsVertexShader ? VertexShaderPreambleLen : PixelShaderPreambleLen)
		+ (IsVertexShader ? VertexShaderPostambleLen : PixelShaderPostambleLen)
		+ strlen(Source) + 3;
	char *CompleteSource = new char[CompleteLength];
	_snprintf_s(CompleteSource, CompleteLength + 2, _TRUNCATE, "%s\n%s\n%s",
		IsVertexShader ? VertexShaderPreamble : PixelShaderPreamble,
		Source,
		IsVertexShader ? VertexShaderPostamble : PixelShaderPostamble);

	HRESULT Result = D3DCompile(CompleteSource, strlen(CompleteSource), NULL, Macros, NULL, "main", Profile, Flags, 0, &Blob,
#ifndef NDEBUG
		&Errors
#else
		NULL
#endif
	);
	if (FAILED(Result))
	{
#ifndef NDEBUG
		if (Errors)
			printf("%s shader compilation failed (0x%x):\n%s\n", IsVertexShader ? "Vertex" : "Pixel", Result, (char *)Errors->GetBufferPointer());
		else
			printf("%s shader compilation failed (0x%x) with no errors!\n", IsVertexShader ? "Vertex" : "Pixel", Result);
#endif
		Errors->Release();
		return NULL;
	}

	return Blob;
}

void D3D10Renderer::ReflectShader(ID3D10Blob *Blob, Shader *OutShader)
{
	size_t LastConstantOffset = 0;
	size_t LastConstantSize = 0;
	// NOTE: deliberately using D3D11 interfaces here! D3D10ReflectShader and friends are deprecated
	ID3D11ShaderReflection *Reflection = NULL; 
	HRESULT Result = D3DReflect(Blob->GetBufferPointer(), Blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void **)&Reflection);
	if (SUCCEEDED(Result))
	{
		D3D11_SHADER_DESC Desc;
		if (SUCCEEDED(Reflection->GetDesc(&Desc)))
		{
			/*D3D11_SIGNATURE_PARAMETER_DESC InputDesc;
			for (UINT i = 0; i < Desc.InputParameters; ++i)
			{
				if (SUCCEEDED(Reflection->GetInputParameterDesc(i, &InputDesc)))
				{
					printf("%s: %d\n", InputDesc.SemanticName, InputDesc.SemanticIndex);
				}
			}*/

			ID3D11ShaderReflectionConstantBuffer *Buffer;
			for (UINT i = 0; i < Desc.ConstantBuffers; ++i)
			{
				Buffer = Reflection->GetConstantBufferByIndex(i);
				if (!Buffer)
					continue;
				D3D11_SHADER_BUFFER_DESC BufferDesc;
				if (SUCCEEDED(Buffer->GetDesc(&BufferDesc)))
				{
					ID3D11ShaderReflectionVariable *Var;
					for (UINT j = 0; j < BufferDesc.Variables; ++j)
					{
						Var = Buffer->GetVariableByIndex(j);
						if (!Var)
							continue;
						D3D11_SHADER_VARIABLE_DESC VarDesc;
						if (SUCCEEDED(Var->GetDesc(&VarDesc)))
						{
							//printf("Var: %s\n", VarDesc.Name);
							OutShader->ConstantMap[std::string(VarDesc.Name)] = VarDesc.StartOffset;
							if (LastConstantOffset < VarDesc.StartOffset)
							{
								LastConstantOffset = VarDesc.StartOffset;
								LastConstantSize = VarDesc.Size;
							}
						}
					}
				}
			}

			for (UINT i = 0; i < Desc.BoundResources; ++i)
			{
				D3D11_SHADER_INPUT_BIND_DESC BindDesc;
				if (SUCCEEDED(Reflection->GetResourceBindingDesc(i, &BindDesc)))
				{
					//printf("Bind: %s\n", BindDesc.Name);
					if (BindDesc.Type == D3D_SIT_TEXTURE)
						OutShader->TextureMap[std::string(BindDesc.Name)] = BindDesc.BindPoint;
				}
			}
		}
		Reflection->Release();
	}

	if (LastConstantSize > 0)
	{
		// round up to closest multiple of float4 (16 bytes)
		static const size_t RegisterSize		= 0x10;
		static const size_t RegisterSizeMask	= 0x0F;
		size_t ConstantBufferSize = (LastConstantOffset + LastConstantSize) & ~RegisterSizeMask | RegisterSize;

		D3D10_BUFFER_DESC Desc;
		Desc.ByteWidth = ConstantBufferSize;
		Desc.Usage = D3D10_USAGE_DYNAMIC;
		Desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		Desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		Desc.MiscFlags = 0;

		OutShader->ConstantBuffer = new Buffer;
		Result = Device->CreateBuffer(&Desc, NULL, &OutShader->ConstantBuffer->D3DBuffer);
	}
}

D3D10Renderer::ProgramHandle D3D10Renderer::CompileProgram(const char *VertexShaderSource, const char *PixelShaderSource)
{
	Shader *VS = NULL, *PS = NULL;
	HRESULT Result;

	ID3D10Blob *VSBlob = NULL, *PSBlob = NULL;

	// HACK: prevent recompiling the generic effect over and over again
	extern const char EffectGenericVertexShader[];
	if (VertexShaderSource == EffectGenericVertexShader && GenericVS)
	{
		VS = GenericVS;
		VSBlob = GenericVSBlob;
		VSBlob->AddRef();
	}
	else
	{
		VSBlob = CompileShader(VertexShaderSource, "vs_4_0");
		if (!VSBlob)
		{
			if (VSBlob)
				VSBlob->Release();
			return NULL;
		}

		ID3D10VertexShader *NativeVS = NULL;
		HRESULT Result = Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &NativeVS);
		if (FAILED(Result) || !NativeVS)
		{
#ifndef NDEBUG
			printf("Failed to create VS from blob (0x%x)!\n", Result);
#endif
			VSBlob->Release();
			if (NativeVS)
				VS->Release();
		}

		VS = new Shader(NativeVS);
		ReflectShader(VSBlob, VS);
		NativeVS->Release();	// the Shader object adds a ref

		// cache 
		if (VertexShaderSource == EffectGenericVertexShader)
		{
			GenericVS = VS;
			GenericVS->AddRef();
			GenericVSBlob = VSBlob;
			VSBlob->AddRef();
		}
	}

	PSBlob = CompileShader(PixelShaderSource, "ps_4_0");
	if (!PSBlob)
	{
		VS->Release();
		VSBlob->Release();
		if (PSBlob)
			PSBlob->Release();
		return NULL;
	}

	ID3D10PixelShader *NativePS = NULL;
	Result = Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), &NativePS);
	if (FAILED(Result) || !NativePS)
	{
#ifndef NDEBUG
		printf("Failed to create PS from blob (0x%x)!\n", Result);
#endif
		VS->Release();
		PSBlob->Release();
		if (NativePS)
			PS->Release();
	}

	PS = new Shader(NativePS);
	ReflectShader(PSBlob, PS);
	NativePS->Release();	// the Shader object adds a ref

	ID3D10InputLayout *IL = NULL;
	Result = Device->CreateInputLayout(ElementDescs, sizeof(ElementDescs) / sizeof(ElementDescs[0]), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &IL);

	// we no longer need this
	PSBlob->Release();
	VSBlob->Release();

	Program *RetVal = new Program(VS, PS, IL);
	// the Program object adds refs
	PS->Release();
	VS->Release();
	return RetVal;
}

/** Renders the configured effects. */
void D3D10Renderer::Render()
{
	if (DOF && DOF->GetEnabled())
	{
		SetRenderTarget(RT_BackBuffer);
		DOF->Program->Bind(Device);
		DrawFullScreenQuad();
	}
	if (DirtBloom && DirtBloom->GetEnabled())
	{
		// bright pass to half-res scratch space
		SetRenderTarget(RT_ScratchSpace, 1);
		DirtBloom->Program[0]->Bind(Device);
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
		DirtBloom->Program[1]->Bind(Device);
		DrawFullScreenQuad();
	}
	if (TexturedDOF && TexturedDOF->GetEnabled())
	{
		SetRenderTarget(RT_ScratchSpace, 0);
		//LGL(Clear)(GL_COLOR_BUFFER_BIT);
		TexturedDOF->Program->Bind(Device);
/*#if TEXTURED_DOF_TRIANGLE_STRIP
		DrawBuffers(Renderer::PT_TriangleStrip,
			TexturedDOF->ParticleIndices, sizeof(LUINT), TexturedDOF->ParticleIndexCount,
			TexturedDOF->ParticleVertices, sizeof(TexturedDOFEffect<D3D10Renderer>::VertLayout),
			2, 2, offsetof(TexturedDOFEffect<D3D10Renderer>::VertLayout, Norm[0]));
#else
		DrawBuffers(Renderer::PT_Points,
			TexturedDOF->ParticleIndices, sizeof(LUINT), TexturedDOF->ParticleIndexCount,
			TexturedDOF->ParticleVertices, sizeof(TexturedDOFEffect<D3D10Renderer>::VertLayout), 2);
#endif*/
		SetRenderTarget(RT_BackBuffer);
		BlitFromScratchSpace(0);
	}
	if (WaterDroplets && WaterDroplets->GetEnabled())
	{
		SetRenderTarget(RT_BackBuffer);
		WaterDroplets->Program->Bind(Device);
		DrawFullScreenQuad();
	}
}

ID3D10Buffer *D3D10Renderer::UploadVerticesToBuffer(void *Data, size_t Count, size_t ElementSize)
{
	ID3D10Buffer *Buf = NULL;
	D3D10_BUFFER_DESC Desc;
	D3D10_SUBRESOURCE_DATA InitData;
	Desc.ByteWidth = Count * ElementSize;
	Desc.Usage = D3D10_USAGE_IMMUTABLE;
	Desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	InitData.pSysMem = Data;
	HRESULT Result = Device->CreateBuffer(&Desc, &InitData, &Buf);
	return Buf;
}
	
ID3D10Buffer *D3D10Renderer::UploadIndicesToBuffer(void *Data, size_t Count, size_t ElementSize)
{
	ID3D10Buffer *Buf = NULL;
	D3D10_BUFFER_DESC Desc;
	D3D10_SUBRESOURCE_DATA InitData;
	Desc.ByteWidth = Count * ElementSize;
	Desc.Usage = D3D10_USAGE_IMMUTABLE;
	Desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	InitData.pSysMem = Data;
	HRESULT Result = Device->CreateBuffer(&Desc, &InitData, &Buf);
	return Buf;
}

}
