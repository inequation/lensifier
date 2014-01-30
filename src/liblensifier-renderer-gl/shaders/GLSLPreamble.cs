STRINGIFY(
#define Sampler1D sampler1D
\n#define Sampler2D sampler2D
\n#define Sampler3D sampler3D
//\n#define Sampler4D sampler4D
vec4 Sample1D(sampler1D Sampler, float Coord) {return texture1D(Sampler, Coord);}
vec4 Sample2D(sampler2D Sampler, vec2 Coord) {return texture2D(Sampler, Coord);}
vec4 Sample3D(sampler3D Sampler, vec3 Coord) {return texture3D(Sampler, Coord);}
//vec4 Sample4D(sampler4D Sampler, vec4 Coord) {return texture4D(Sampler, Coord);}
float Saturate(float In) {return clamp(In, 0.0, 1.0);}
vec2 Saturate(vec2 In) {return clamp(In, 0.0, 1.0);}
vec3 Saturate(vec3 In) {return clamp(In, 0.0, 1.0);}
vec4 Saturate(vec4 In) {return clamp(In, 0.0, 1.0);}
float Blend(float A, float B, float Alpha) {return mix(A, B, Alpha);}
vec2 Blend(vec2 A, vec2 B, float Alpha) {return mix(A, B, Alpha);}
vec3 Blend(vec3 A, vec3 B, float Alpha) {return mix(A, B, Alpha);}
vec4 Blend(vec4 A, vec4 B, float Alpha) {return mix(A, B, Alpha);}

)