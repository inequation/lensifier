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
		size_t							DataLength;
		ID3D10Buffer					*D3DBuffer;

		// private destructor so that can't be deleted from outside
		~Buffer() {delete [] Data;}

	public:
		Buffer() : Data(NULL), D3DBuffer(NULL) {}
		inline void *GetData() {return Data;}
		inline ID3D10Buffer *GetBuffer() {return D3DBuffer;}
		inline void Set(ID3D10Buffer *Buf, D3D10_BUFFER_DESC *Desc)
		{
			D3DBuffer = Buf;
			if (Data)
				delete Data;
			DataLength = Desc->ByteWidth;
			Data = new BYTE[DataLength];
		}
		inline void Commit()
		{
			if (Data)
			{
				void *BufData = NULL;
				HRESULT Result = D3DBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &BufData);
				if (SUCCEEDED(Result))
				{
					memcpy(BufData, Data, DataLength);
					D3DBuffer->Unmap();
				}
			}
		}
		ULONG AddRef() {return D3DBuffer->AddRef();}
		ULONG Release()
		{
			ULONG Count = D3DBuffer ? D3DBuffer->Release() : 0;
			if (Count == 0)
				delete this;
			return Count;
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
	private:
		~Shader() {if (ConstantBuffer) ConstantBuffer->Release();}
	public:
		ULONG AddRef() {return DeviceChild->AddRef();}
		ULONG Release()
		{
			ULONG Count = DeviceChild->Release();
			if (Count == 0)
				delete this;
			return Count;
		}
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
			Device->GSSetShader(NULL);
			Device->PSSetShader(PS->Pixel);
			Device->IASetInputLayout(IL);
		}
	};
}

}

#endif
