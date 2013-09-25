STRINGIFY(

void main()
{
	VertexInput _VI;
	_VI.Position = gl_Vertex;
	_VI.TexCoords = gl_MultiTexCoord0;
	VertexOutput _VO = LensifierMain(_VI);
	gl_Position = _VO.Position;
	gl_TexCoord[0] = _VO.TexCoords;
}
)
