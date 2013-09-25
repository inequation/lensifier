STRINGIFY(
VertexOutput LensifierMain(VertexInput Input)
{
	// draws a full-screen quad
	VertexOutput VO;
	VO.Position.xy = Input.Position.xy;
	VO.Position.zw = vec2(0.0, 1.0);
	const vec4 HalfVec = vec4(0.5);
	VO.TexCoords = Input.Position * HalfVec + HalfVec;
	return VO;
}
)
