/*
 * Lensifier library Direct3D 10 helpers
 * Copyright (C) 2014, Leszek Godlewski 
 */

// only allow inclusion from within D3D10Renderer.h
#ifndef D3D10RENDERER_H
	#error Do not include this file directly
#endif

#ifndef D3D10HELPERS_H
#define D3D10HELPERS_H

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
		~Shader() {Release(); delete ConstantBuffer;}
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
				Buf->Release();
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

}

#endif
