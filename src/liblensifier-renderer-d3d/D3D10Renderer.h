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

// for constant buffer mappings
#include <map>
#include <string>

// helper classes reside in a separate file
#include "D3D10Helpers.h"

namespace Lensifier
{

class D3D10Renderer : public Renderer
{
public:
	// these definitions are required by the templated Effect class
	typedef D3D10Helpers::Program			*ProgramHandle;
	typedef D3D10Helpers::ShaderParam		*ShaderParameterHandle;
	typedef ID3D10Buffer					*IndexBufferHandle;
	typedef ID3D10Buffer					*VertexBufferHandle;
	typedef ID3D10ShaderResourceView		*TextureHandle;

	static const TextureHandle	InvalidTextureHandle;
	
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
	{
		D3D10Helpers::Shader::OffsetMap::const_iterator It;
		
		It = Program->VS->ConstantMap.find(std::string(ParamName));
		if (It != Program->VS->ConstantMap.end())
			return new D3D10Helpers::ShaderParam(Program->VS->ConstantBuffer, It->second);

		It = Program->VS->TextureMap.find(std::string(ParamName));
		if (It != Program->VS->TextureMap.end())
			return new D3D10Helpers::ShaderParam(D3D10Helpers::ShaderParam::VERTEX, It->second);

		It = Program->PS->ConstantMap.find(std::string(ParamName));
		if (It != Program->PS->ConstantMap.end())
			return new D3D10Helpers::ShaderParam(Program->PS->ConstantBuffer, It->second);

		It = Program->PS->TextureMap.find(std::string(ParamName));
		if (It != Program->PS->TextureMap.end())
			return new D3D10Helpers::ShaderParam(D3D10Helpers::ShaderParam::PIXEL, It->second);
		
		return new D3D10Helpers::ShaderParam(NULL, (size_t)-1);
	}
	
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const bool Value)
	{if (Param->Buf) *(UINT *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = Value ? (UINT)-1 : 0;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const LUINT Value)
	{if (Param->Buf) *(UINT *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = (UINT)Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const float Value)
	{if (Param->Buf) *(float *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const Vector2& Value)
	{if (Param->Buf) *(Vector2 *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const Vector3& Value)
	{if (Param->Buf) *(Vector3 *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const Vector4& Value)
	{if (Param->Buf) *(Vector4 *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const TextureHandle Value)
	{
		switch (Param->Stage)
		{
			case D3D10Helpers::ShaderParam::VERTEX:	Device->VSSetShaderResources(Param->Offset, 1, &Value);
			case D3D10Helpers::ShaderParam::PIXEL:	Device->PSSetShaderResources(Param->Offset, 1, &Value);
		}
	}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const void *Value)
	{SetShaderParameterValue(Param, (TextureHandle)Value);}
	
	/** Releases the given shader parameter. */
	void ReleaseShaderParameter(ShaderParameterHandle Param)
	{if (Param != (ShaderParameterHandle)-1) delete Param;}
	
	/** Releases the given program. */
	inline void ReleaseProgram(ProgramHandle Program)
	{delete Program;}

	ID3D10Buffer *UploadVerticesToBuffer(void *Data, size_t Count, size_t ElementSize);
	ID3D10Buffer *UploadIndicesToBuffer(void *Data, size_t Count, size_t ElementSize);

	/** Notification issued by the library that the configuration has changed. */
	virtual void Setup(LUINT InScreenWidth, LUINT InScreenHeight,
		void *ColourTexture, void *DepthTexture);

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
	// NOTE: MUST match the declaration in HLSLPreamble.vs!!!
	struct VertexInput
	{
		float Position[4];
		float TexCoords[4];
		float Normal[3];
		float Colour[4];
	};

	ID3D10Blob *CompileShader(const char *Source, const char *Profile);
	/** Returns the number of input parameters. */
	void ReflectShader(ID3D10Blob *Blob, D3D10Helpers::Shader *OutShader);

	ID3D10Device				*Device;
	D3D10Helpers::Shader		*GenericVS;
	ID3D10Blob					*GenericVSBlob;

	inline void DrawFullScreenQuad(const ProgramHandle Program)
	{
		Program->Bind(Device);
		if (Program->VS->ConstantBuffer)
		{
			Program->VS->ConstantBuffer->Commit();
			ID3D10Buffer *Buf = Program->VS->ConstantBuffer->GetBuffer();
			Device->VSSetConstantBuffers(0, 1, &Buf);
		}
		if (Program->PS->ConstantBuffer)
		{
			Program->PS->ConstantBuffer->Commit();
			ID3D10Buffer *Buf = Program->PS->ConstantBuffer->GetBuffer();
			Device->PSSetConstantBuffers(0, 1, &Buf);
		}
		static const UINT Stride = sizeof(VertexInput);
		static const UINT Offset = 0;
		Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Device->IASetVertexBuffers(0, 1, &FSQuadVB, &Stride, &Offset);
		Device->Draw(3, 0);
	}

	ID3D10Buffer				*FSQuadVB;
	static const D3D10_INPUT_ELEMENT_DESC	ElementDescs[];
	static const char			VertexShaderPreamble[];
	static const size_t			VertexShaderPreambleLen;
	static const char			VertexShaderPostamble[];
	static const size_t			VertexShaderPostambleLen;
	static const char			PixelShaderPreamble[];
	static const size_t			PixelShaderPreambleLen;
	static const char			PixelShaderPostamble[];
	static const size_t			PixelShaderPostambleLen;

	D3D10Helpers::Program		*GaussianBlur;
	CachedShaderParam<D3D10Renderer, void *> GaussianBlurSceneColour;
	CachedShaderParam<D3D10Renderer, Vector2> GaussianBlurTexelSize;
	CachedShaderParam<D3D10Renderer, bool> GaussianBlurHorizontal;
};

}

#endif	// D3D10RENDERER_H
