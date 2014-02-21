STRINGIFY(
void LensifierMain(inout VertexInput Input)
{
	// draws a full-screen quad
	//Input.Position.xy = Input.Position.xy;
	Input.Position.zw = vec2(0.0, 1.0);
\n#if LENSIFIER_GLSL\n
	// D3D10 tex coords are already fine
	const vec4 HalfVec = vec4(0.5, 0.5, 0.5, 0.5);
	Input.TexCoords = Input.Position * HalfVec + HalfVec;
\n#endif\n
}
)
