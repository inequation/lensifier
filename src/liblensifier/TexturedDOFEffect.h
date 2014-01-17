#ifdef OP_PER_PARAM

OP_PER_PARAM(LUINT, BokehTexture, (LUINT)-1)
OP_PER_PARAM(float, FocusDistance, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, FocusBreadth, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, BlurFalloffExponent, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, MinBlur, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, MaxNearBlur, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, MaxFarBlur, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, ZNear, std::numeric_limits<float>::quiet_NaN())
OP_PER_PARAM(float, ZFar, std::numeric_limits<float>::quiet_NaN())

#else // OP_PER_PARAM

#ifndef TEXTUREDDOFEFFECT_H
#define TEXTUREDDOFEFFECT_H

#define TEXTURED_DOF_TRIANGLE_STRIP	1

#include "SinglePassEffect.h"

using namespace std;

namespace Lensifier
{

extern const char TexturedDOFEffectVertexShader[];
extern const char TexturedDOFEffectPixelShader[];

template <class RendererClass>
class TexturedDOFEffect : public SinglePassEffect<RendererClass>
{
public:
	DECLARE_EFFECT_1T(SinglePassEffect, RendererClass)
	
	TexturedDOFEffect()
		: Super()
		#define OP_PER_PARAM(Type, Name, Default) , INIT_PARAM(Name, Default)	
		#include "TexturedDOFEffect.h"
		#undef OP_PER_PARAM
		, ParticleIndices(0)
		, ParticleVertices(0)
	{
		this->Program = RENDERER->CompileProgram(TexturedDOFEffectVertexShader, TexturedDOFEffectPixelShader);
		this->Register();
		RENDERER->ReserveScratchSpace(0);
	}
	
	void RegenerateGeometry(LUINT ScreenWidth, LUINT ScreenHeight)
	{
#if TEXTURED_DOF_TRIANGLE_STRIP
		// regular triangle strips
		const size_t VertexCount = ScreenWidth * ScreenHeight * 4;
		ParticleIndexCount = ScreenWidth * ScreenHeight * 5;
		VertLayout *Verts = new VertLayout[VertexCount];
		LUINT *Indices = new LUINT[ParticleIndexCount];
		const Vector2 TexelSize(1.f / ScreenWidth, 1.f / ScreenHeight);
		
		for (size_t y = 0; y < ScreenHeight; ++y)
		{
			for (size_t x = 0; x < ScreenWidth; ++x)
			{
				for (size_t i = 0; i < 4; ++i)
				{
					const size_t Vert = (y * ScreenWidth + x) * 4 + i;
					Verts[Vert].Pos[0] = TexelSize.X * (x + (i > 1));
					Verts[Vert].Pos[1] = TexelSize.Y * (y + (i % 2));
					Verts[Vert].Norm[0] = (i > 1) ? 1.f : -1.f;
					Verts[Vert].Norm[1] = (i % 2) ? 1.f : -1.f;
				}
				
				for (size_t i = 0; i < 5; ++i)
				{
					// generate 4 indices for verts, then a degenerate triangle
					// by repeating the last vert of a particle
					const size_t Index = (y * ScreenWidth + x) * 5 + i;
					const size_t Vert = (y * ScreenWidth + x) * 4 + (i <= 3 ? i : 3);
					Indices[Index] = Vert;
				}
			}
		}
#else
		// points
		const size_t VertexCount = ScreenWidth * ScreenHeight;
		ParticleIndexCount = ScreenWidth * ScreenHeight;
		VertLayout *Verts = new VertLayout[VertexCount];
		LUINT *Indices = new LUINT[ParticleIndexCount];
		const Vector2 TexelSize(1.f / (ScreenWidth - 1), 1.f / (ScreenHeight - 1));
		
		for (size_t y = 0; y < ScreenHeight; ++y)
		{
			for (size_t x = 0; x < ScreenWidth; ++x)
			{
				for (size_t i = 0; i < 4; ++i)
				{
					const size_t Vert = (y * ScreenWidth + x) * 4 + i;
					Verts[Vert].Pos[0] = TexelSize.X * (x + (i > 1));
					Verts[Vert].Pos[1] = TexelSize.Y * (y + (i % 2));
					Verts[Vert].Norm[0] = (i > 1) ? 1.f : -1.f;
					Verts[Vert].Norm[1] = (i % 2) ? 1.f : -1.f;
				}
				
				for (size_t i = 0; i < 5; ++i)
				{
					// generate 4 indices for verts, then a degenerate triangle
					// by repeating the last vert of a particle
					const size_t Index = (y * ScreenWidth + x) * 5 + i;
					Indices[Index] = (y * ScreenWidth + x) * 4 + (i <= 3 ? i : 3);
				}
			}
		}
#endif
		
		ParticleIndices = RENDERER->UploadIndicesToBuffer(Indices, ParticleIndexCount, sizeof(*Indices));
		ParticleVertices = RENDERER->UploadVerticesToBuffer(Verts, VertexCount, sizeof(*Verts));
		
		delete [] Indices;
		delete [] Verts;
	}
	
	struct VertLayout
	{
		float Pos[2];
#if TEXTURED_DOF_TRIANGLE_STRIP
		float Norm[2];
#endif
	};

protected:
	virtual void Register()
	{
		Super::Register();
		#define OP_PER_PARAM(Type, Name, Default) Name.Register(this->Program, #Name);
		#include "TexturedDOFEffect.h"
		#undef OP_PER_PARAM
	}
	
public:
	#define OP_PER_PARAM(Type, Name, Default)	\
		CachedShaderParam<RendererClass, Type> Name;
	#include "TexturedDOFEffect.h"
	#undef OP_PER_PARAM
	IndexBufferHandle	ParticleIndices;
	VertexBufferHandle	ParticleVertices;
	size_t				ParticleIndexCount;
};

}

#endif // TEXTUREDDOFEFFECT_H
#endif // OP_PER_PARAM