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

namespace Lensifier
{

namespace D3D10Helpers
{
	// encapsulates ID3D10Buffer with lazy intialization etc.
	class Buffer
	{
		void							*Data;

	public:
		ID3D10Buffer					*D3DBuffer;

		Buffer() : Data(NULL), D3DBuffer(NULL) {}
		~Buffer() {Release();}
		inline void *GetData()
		{
			if (!Data)
				D3DBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &Data);
			return Data;
		}
		inline void Commit()
		{
			if (Data)
			{
				D3DBuffer->Unmap();
				Data = NULL;
			}
		}
		ULONG AddRef() {return D3DBuffer->AddRef();}
		ULONG Release()
		{
			if (!D3DBuffer)
				return 0;
			if (Data) D3DBuffer->Unmap();
			ID3D10Buffer *Buf = D3DBuffer;
			D3DBuffer = NULL;
			return Buf->Release();
		}
	};

	// encapsulates a shader and its reflection data
	struct Shader
	{
		union
		{
			ID3D10VertexShader			*Vertex;
			ID3D10PixelShader			*Pixel;
			ID3D10DeviceChild			*DeviceChild;
		};
		Buffer							*ConstantBuffer;
		typedef std::map<std::string, size_t> OffsetMap;
		OffsetMap						ConstantMap;
		OffsetMap						TextureMap;

		Shader(ID3D10DeviceChild *InShader) : DeviceChild(InShader), ConstantBuffer(NULL) {AddRef();}
		~Shader() {Release(); if (ConstantBuffer) ConstantBuffer->Release();}
		ULONG AddRef() {return DeviceChild->AddRef();}
		ULONG Release() {return DeviceChild->Release();}
	};

	// encapsulates a constant in a buffer or a resource view
	struct ShaderParam
	{
		// if Offset != (size_t)-1 and Buffer == NULL, it's a texture
		enum ResourceStage
		{NONE = NULL, VERTEX, PIXEL, LAST_STAGE};

		union
		{
			Buffer						*Buf;
			ResourceStage				Stage;
		};
		size_t							Offset;

		ShaderParam(Buffer *InBuffer, size_t InOffset)
			: Buf(InBuffer), Offset(InOffset)
		{if (Buf) Buf->AddRef();}
		ShaderParam(ResourceStage InStage, size_t InOffset) : Stage(InStage), Offset(InOffset) {}
		~ShaderParam()
		{
			if (Stage >= LAST_STAGE)
			{
				// Buf will release the underlying D3D buffer
				delete Buf;
			}
		}
	};

	// encapsulates a vertex/pixel shader combo (i.e. a pipeline)
	struct Program
	{
		Shader							*VS, *PS;
		ID3D10InputLayout				*IL;

		Program(Shader *InVS, Shader *InPS, ID3D10InputLayout *InIL) : VS(InVS), PS(InPS), IL(InIL) {VS->AddRef(); PS->AddRef(); IL->AddRef();}
		~Program() {VS->Release(); PS->Release(); IL->Release();}

		inline void Bind(ID3D10Device *Device)
		{
			Device->VSSetShader(VS->Vertex);
			if (VS->ConstantBuffer)
			{
				VS->ConstantBuffer->Commit();
				Device->VSSetConstantBuffers(0, 1, &VS->ConstantBuffer->D3DBuffer);
			}
			Device->GSSetShader(NULL);
			Device->PSSetShader(PS->Pixel);
			if (PS->ConstantBuffer)
			{
				PS->ConstantBuffer->Commit();
				Device->PSSetConstantBuffers(0, 1, &PS->ConstantBuffer->D3DBuffer);
			}
			Device->IASetInputLayout(IL);
		}
	};
}

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
	{*(UINT *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = (UINT)Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const LUINT Value)
	{*(UINT *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = (UINT)Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const float Value)
	{*(float *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const Vector2& Value)
	{*(Vector2 *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const Vector3& Value)
	{*(Vector3 *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = Value;}
	inline void SetShaderParameterValue(ShaderParameterHandle Param, const Vector4& Value)
	{*(Vector4 *)&(((BYTE *)Param->Buf->GetData())[Param->Offset]) = Value;}
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
	{delete Param;}
	
	/** Releases the given program. */
	inline void ReleaseProgram(ProgramHandle Program)
	{delete Program;}

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

	inline void DrawFullScreenQuad()
	{
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
