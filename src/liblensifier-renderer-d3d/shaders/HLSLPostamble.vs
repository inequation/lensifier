STRINGIFY(

PixelInput main(in VertexInput _VI)
{
	LensifierMain(_VI);
	PixelInput _PI;
	_PI.Position = _VI.Position;
	_PI.TexCoords = _VI.TexCoords;
	_PI.VertexColour = _VI.Colour;
	return _PI;
}
)
