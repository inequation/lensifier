STRINGIFY(

void main()
{
	VertexInput VI;
	VI.Position = gl_Vertex;
	VI.TextureCoordinates = gl_MultiTexCoord0;
	VertexOutput VO = LensifierMain(VI);
	gl_Position = VO.Position;
	gl_TexCoord[0] = VO.TextureCoordinates;
}
)
