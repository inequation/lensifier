/*
 * Lensifier library Direct3D 10 renderer
 * Copyright (C) 2014, Leszek Godlewski 
 */

#ifndef D3D10RENDERER_H
#define D3D10RENDERER_H

#include "../liblensifier/Renderer.h"
#include "../liblensifier/Utils.h"

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1
	#define __UNDEF_WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#ifdef __UNDEF_WIN32_LEAN_AND_MEAN
	#undef WIN32_LEAN_AND_MEAN
	#undef __UNDEF_WIN32_LEAN_AND_MEAN
#endif

#include <d3d10.h>

namespace Lensifier
{

class D3D10Renderer : public Renderer
{
public:
	struct D3D10Program
	{
		ID3D10VertexShader	*VS;
		ID3D10PixelShader	*PS;

		D3D10Program(ID3D10VertexShader *InVS, ID3D10PixelShader *InPS) : VS(InVS), PS(InPS) {}
	};

	// these definitions are required by the templated Effect class
	typedef D3D10Program	*ProgramHandle;
	typedef size_t			ShaderParameterHandle;
	typedef ID3D10Buffer	*IndexBufferHandle;
	typedef ID3D10Buffer	*VertexBufferHandle;
	
	D3D10Renderer(void *InDevice);
	virtual ~D3D10Renderer();

	/**
	 * Adds in the API-specific shader sugar, then compiles and links the shader
	 * sources.
	 * @return	GLSL program name
	 */
	ProgramHandle CompileProgram(const char *VertexShaderSource, const char *PixelShaderSource);

	/** Finds the location of the given uniform variable in the given program. */
	inline ShaderParameterHandle GetShaderParameter(ProgramHandle Program, const char *ParamName)
	{return -1;}
	
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const bool Value)
	{}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const LUINT Value)
	{}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const ID3D10Texture2D *Value)
	{}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const float Value)
	{}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const Vector2& Value)
	{}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const Vector3& Value)
	{}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const Vector4& Value)
	{}
	
	/** Releases the given shader parameter. No-op in OpenGL. */
	void ReleaseShaderParameter(ShaderParameterHandle Param) {}
	
	/** Releases the given program. */
	inline void ReleaseProgram(D3D10Program *Program)
	{
		Program->VS->Release();
		Program->PS->Release();
		delete Program;
	}

	inline ID3D10Buffer	*UploadIndicesToBuffer(void *Data, size_t Count, size_t ElementSize)
	{
		return NULL;
	}
	
	inline ID3D10Buffer	*UploadVerticesToBuffer(void *Data, size_t Count, size_t ElementSize)
	{
		return NULL;
	}

	/** Notification issued by the library that the configuration has changed. */
	virtual void Setup(LUINT InScreenWidth, LUINT InScreenHeight,
		LUINT ColourTextureSlot, LUINT DepthTextureSlot);

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
	ID3D10Device		*Device;
	ID3D10VertexShader	*GenericVS;

	inline void DrawFullScreenQuad()
	{
		// TODO
	}

	static const float FSQuadVerts[];
	static const unsigned char FSQuadIndices[];
	static const char VertexShaderPreamble[];
	static const size_t VertexShaderPreambleLen;
	static const char VertexShaderPostamble[];
	static const size_t VertexShaderPostambleLen;
	static const char PixelShaderPreamble[];
	static const size_t PixelShaderPreambleLen;
	static const char PixelShaderPostamble[];
	static const size_t PixelShaderPostambleLen;

	D3D10Program *GaussianBlur;
	CachedShaderParam<D3D10Renderer, ID3D10Texture2D *> GaussianBlurSceneColour;
	CachedShaderParam<D3D10Renderer, Vector2> GaussianBlurTexelSize;
	CachedShaderParam<D3D10Renderer, bool> GaussianBlurHorizontal;
};

}

#endif	// D3D10RENDERER_H
