#include "DOFEffect.h"

namespace Lensifier
{

#define STRINGIFY(x)	#x

const char DOFEffectPixelShader[] =
	"\n#define M_PI 3.14159265358979323846\n"
	#include "shaders/DOFEffect.ps"
;

}
