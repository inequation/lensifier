"#if LENSIFIER_HLSL >= 40\n"
STRINGIFY(
SamplerState LensifierSamplerBilinear
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};
vec4 Sample1D(Texture1D Texture, float Coord) {return Texture.Sample(LensifierSamplerBilinear, Coord);}
vec4 Sample2D(Texture2D Texture, vec2 Coord)
{
	// flip V to convert from OpenGL convention
	Coord = Coord * vec2(1.0, -1.0) + vec2(0.0, 1.0);
	return Texture.Sample(LensifierSamplerBilinear, Coord);
}
vec4 Sample2DMS(Texture2DMS<float4, LENSIFIER_MS_SAMPLES> Texture, int2 Coord)
{
	// flip V to convert from OpenGL convention
	Coord = Coord * vec2(1.0, -1.0) + vec2(0.0, 1.0);
	return Texture.Load(Coord, LENSIFIER_MS_SAMPLES / 2);
}
vec4 Sample3D(Texture3D Texture, vec3 Coord)
{
	// flip V to convert from OpenGL convention
	Coord = Coord * vec3(1.0, -1.0, 1.0) + vec3(0.0, 1.0, 0.0);
	return Texture.Sample(LensifierSamplerBilinear, Coord);
}
/*vec4 Sample4D(Texture4D Sampler, vec4 Coord)
{
	// flip V to convert from OpenGL convention
	Coord = Coord * vec4(1.0, -1.0, 1.0, 1.0) + vec4(0.0, 1.0, 0.0, 0.0);
	return Texture.Sample(LensifierSamplerBilinear, Coord);
}*/
)
"\n#else\n"
STRINGIFY(
vec4 Sample1D(sampler1D Sampler, float Coord) {return tex1D(Sampler, Coord);}
vec4 Sample2D(sampler2D Sampler, vec2 Coord)
{
	// flip V to convert from OpenGL convention
	Coord = Coord * vec2(1.0, -1.0) + vec2(0.0, 1.0);
	return tex2D(Sampler, Coord);
}
vec4 Sample2DSM(sampler2DMS Sampler, vec2 Coord)
{
	// flip V to convert from OpenGL convention
	Coord = Coord * vec2(1.0, -1.0) + vec2(0.0, 1.0);
	return tex2D(Sampler, Coord);
}
vec4 Sample3D(sampler3D Sampler, vec3 Coord)
{
	// flip V to convert from OpenGL convention
	Coord = Coord * vec3(1.0, -1.0, 1.0) + vec3(0.0, 1.0, 0.0);
	return tex3D(Sampler, Coord);
}
/*vec4 Sample4D(sampler4D Sampler, vec4 Coord)
{
	// flip V to convert from OpenGL convention
	Coord = Coord * vec4(1.0, -1.0, 1.0, 1.0) + vec4(0.0, 1.0, 0.0, 0.0);
	return tex4D(Sampler, Coord);
}*/
)
"\n#endif\n"
STRINGIFY(
float Saturate(float In) {return saturate(In);}
vec2 Saturate(vec2 In) {return saturate(In);}
vec3 Saturate(vec3 In) {return saturate(In);}
vec4 Saturate(vec4 In) {return saturate(In);}
float Blend(float A, float B, float Alpha) {return lerp(A, B, Alpha);}
vec2 Blend(vec2 A, vec2 B, float Alpha) {return lerp(A, B, Alpha);}
vec3 Blend(vec3 A, vec3 B, float Alpha) {return lerp(A, B, Alpha);}
vec4 Blend(vec4 A, vec4 B, float Alpha) {return lerp(A, B, Alpha);}

struct PixelInput
{
	vec4 Position       : SV_POSITION;
	vec4 TexCoords		: TEXCOORD0;
	vec4 VertexColour	: COLOR0;
};
)