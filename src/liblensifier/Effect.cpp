#include "Effect.h"
#include "DOFEffect.h"
#include "DirtBloomEffect.h"
#include "TexturedDOFEffect.h"
#include "WaterDropletsEffect.h"

namespace Lensifier
{

#define STRINGIFY(x)	#x

const char EffectGenericVertexShader[] =
	#include "shaders/GenericEffect.vs"
;

const char SingleDirGaussianBlurPixelShader[] =
	"\n#define GAUSS_SIZE 16\n"
	#include "shaders/SingleDirGaussianBlur.ps"
;

const char DOFEffectPixelShader[] =
	"\n#define M_PI 3.14159265358979323846\n"
	#include "shaders/DOFEffect.ps"
;

const char DirtBloomEffectBrightPassPixelShader[] =
	#include "shaders/DirtBloomEffectBrightPass.ps"
;

const char DirtBloomEffectCompositePassPixelShader[] =
	#include "shaders/DirtBloomEffectCompositePass.ps"
;

const char TexturedDOFEffectVertexShader[] =
	#include "shaders/TexturedDOFEffect.vs"
;

const char TexturedDOFEffectPixelShader[] =
	#include "shaders/TexturedDOFEffect.ps"
;

const char WaterDropletsEffectPixelShader[] =
	#include "shaders/WaterDropletsEffect.ps"
;

}
