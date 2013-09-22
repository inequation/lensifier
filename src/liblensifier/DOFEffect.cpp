#include "DOFEffect.h"

namespace Lensifier
{

#define STRINGIFY(x)	#x

const char DOFEffectPixelShader[] =
	#include "shaders/DOFEffect.ps"
;

}
