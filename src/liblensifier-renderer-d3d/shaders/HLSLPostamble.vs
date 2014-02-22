STRINGIFY(

PixelInput main(in VertexInput _VI)
{
	LensifierMain(_VI);
	PixelInput _PI;
	_PI.Position = _VI.Position;
	// flip V to convert from OpenGL convention
	_PI.TexCoords = _VI.TexCoords * vec4(1.0, -1.0, 1.0, 1.0) + vec4(0.0, 1.0, 0.0, 0.0);
	_PI.VertexColour = _VI.Colour;
	return _PI;
}
)
