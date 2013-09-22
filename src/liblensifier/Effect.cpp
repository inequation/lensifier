#include "Effect.h"

namespace Lensifier
{

#define STRINGIFY(x)	#x

const char EffectGenericVertexShader[] =
	#include "shaders/GenericEffect.vs"
;

}
