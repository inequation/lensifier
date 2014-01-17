STRINGIFY(
uniform sampler2D SceneColour;
uniform sampler2D SceneDepth;
uniform vec2 TexelSize;
uniform float FocusDistance;
uniform float FocusBreadth;
uniform float BlurFalloffExponent;
uniform float MinBlur;
uniform float MaxNearBlur;
uniform float MaxFarBlur;
uniform float ZNear;
uniform float ZFar;

float Linearize(float Depth)
{
	return -ZFar * ZNear / (Depth * (ZFar - ZNear) - ZFar);
}

void LensifierMain(inout VertexInput Input)
{
	// back these up because we'll be changing them promptly
	vec2 SceneCoords = Input.Position.xy;
	vec2 Norm = Input.TexCoords.xy;
	
	// scale and bias from [0, 1] to [-1, 1]
	Input.Position.xy = Input.Position.xy * vec2(2.0) - vec2(1.0);
	Input.Position.zw = vec2(0.0, 1.0);
	// and the tex coords from [-1, 1] to [0, 1] :)
	Input.TexCoords.xy = Input.TexCoords.xy * vec2(0.5) + vec2(0.5);
	
	// colour the vertex to avoid further scene colour sampling in the PS
	Input.Colour = Sample2D(SceneColour, SceneCoords);
	Input.Colour.a = 1.0;
	
	float Depth = Linearize(Sample2D(SceneDepth, SceneCoords).x);
	float DistanceFromPlane = Depth - FocusDistance;
	
	// early out if in focus
	if (abs(DistanceFromPlane) <= FocusBreadth)
	{
		Input.TexCoords.z = MinBlur;
#if TEXTURED_DOF_TRIANGLE_STRIP
		Input.Position.xy += Norm * TexelSize * MinBlur;
#else
		Input.PointSize = MinBlur;
#endif
		return;
	}
	
	if (DistanceFromPlane < 0.0)
	{
		// near
		DistanceFromPlane = abs(DistanceFromPlane + FocusBreadth);
		Input.TexCoords.z = MaxNearBlur;
	}
	else
	{
		// far
		DistanceFromPlane -= FocusBreadth;
		Input.TexCoords.z = MaxFarBlur;
	}
#if TEXTURED_DOF_TRIANGLE_STRIP
	Input.Position.xy += Norm * TexelSize *
#else
	Input.PointSize =
#endif
		clamp(pow(DistanceFromPlane, BlurFalloffExponent), MinBlur, Input.TexCoords.z);
		//MaxBlur;
}
)
