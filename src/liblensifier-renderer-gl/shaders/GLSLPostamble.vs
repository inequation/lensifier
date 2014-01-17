STRINGIFY(
void main()
{
	VertexInput _VI;
	_VI.Position = gl_Vertex;
	_VI.TexCoords = gl_MultiTexCoord0;
	_VI.Normal = gl_Normal;
	_VI.Colour = gl_Color;
	LensifierMain(_VI);
	gl_Position = _VI.Position;
	gl_TexCoord[0] = _VI.TexCoords;
	//gl_Normal = _VI.Normal;	// FIXME: varying per-pixel normal?
	gl_FrontColor = _VI.Colour;
}
)
