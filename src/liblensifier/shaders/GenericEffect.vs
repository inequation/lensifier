STRINGIFY(
VertexOutpu LensifierMain(VertexInput Input)
{
	VertexOutput VO;
	VO.Position = Input.Position;
	VO.TextureCoordinates = Input.TextureCoordinates;
	return VO;
}
)
