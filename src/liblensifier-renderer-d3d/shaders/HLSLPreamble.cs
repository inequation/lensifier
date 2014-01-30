STRINGIFY(
#if LENSIFIER_HLSL >= 40
\n#define Sampler1D Texture1D
\n#define Sampler2D Texture2D
\n#define Sampler3D Texture3D
//\n#define Sampler4D Texture4D
\nSamplerState LensifierSamplerBilinear
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};
vec4 Sample1D(Texture1D Texture, float Coord) {return Texture.Sample(LensifierSamplerBilinear, Coord);}
vec4 Sample2D(Texture2D Texture, vec2 Coord) {return Texture.Sample(LensifierSamplerBilinear, Coord);}
vec4 Sample3D(Texture3D Texture, vec3 Coord) {return Texture.Sample(LensifierSamplerBilinear, Coord);}
//vec4 Sample4D(Texture4D Sampler, vec4 Coord) {return Texture.Sample(LensifierSamplerBilinear, Coord);}
\n#else
\n#define Sampler1D sampler1D
\n#define Sampler2D sampler2D
\n#define Sampler3D sampler3D
//\n#define Sampler4D sampler4D
\n
vec4 Sample1D(sampler1D Sampler, float Coord) {return tex1D(Sampler, Coord);}
vec4 Sample2D(sampler2D Sampler, vec2 Coord) {return tex2D(Sampler, Coord);}
vec4 Sample3D(sampler3D Sampler, vec3 Coord) {return tex3D(Sampler, Coord);}
//vec4 Sample4D(sampler4D Sampler, vec4 Coord) {return tex4D(Sampler, Coord);}
\n#endif\n
float Saturate(float In) {return saturate(In);}
vec2 Saturate(vec2 In) {return saturate(In);}
vec3 Saturate(vec3 In) {return saturate(In);}
vec4 Saturate(vec4 In) {return saturate(In);}
float Blend(float A, float B, float Alpha) {return A * (1.0 - Alpha) + B * Alpha;}
vec2 Blend(vec2 A, vec2 B, float Alpha) {return A * (1.0 - Alpha) + B * Alpha;}
vec3 Blend(vec3 A, vec3 B, float Alpha) {return A * (1.0 - Alpha) + B * Alpha;}
vec4 Blend(vec4 A, vec4 B, float Alpha) {return A * (1.0 - Alpha) + B * Alpha;}
)