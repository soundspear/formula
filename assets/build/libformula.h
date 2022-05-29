#ifndef FORMULA_ASSETS_LIBFORMULA_INCLUDED
#define FORMULA_ASSETS_LIBFORMULA_INCLUDED

/**
 * Standard includes and compilers cross-compatibility
 */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

/**
 * TinyCC does not support last C standards and floating point
 * overloads from math.h
 */
#ifdef __TINYC__
    float acosf(float _X) { return ((float)acos((double)_X)); }
    float asinf(float _X) { return ((float)asin((double)_X)); }
    float atanf(float _X) { return ((float)atan((double)_X)); }
    float atan2f(float _X,float _Y) { return ((float)atan2((double)_X,(double)_Y)); }
    float ceilf(float _X) { return ((float)ceil((double)_X)); }
    float cosf(float _X) { return ((float)cos((double)_X)); }
    float coshf(float _X) { return ((float)cosh((double)_X)); }
    float expf(float _X) { return ((float)exp((double)_X)); }
    float floorf(float _X) { return ((float)floor((double)_X)); }
    float fmodf(float _X,float _Y) { return ((float)fmod((double)_X,(double)_Y)); }
    float logf(float _X) { return ((float)log((double)_X)); }
    float log10f(float _X) { return ((float)log10((double)_X)); }
    float modff(float _X,float *_Y) {
        double _Di,_Df = modf((double)_X,&_Di);
        *_Y = (float)_Di;
        return ((float)_Df);
    }
    float powf(float _X,float _Y) { return ((float)pow((double)_X,(double)_Y)); }
    float sinf(float _X) { return ((float)sin((double)_X)); }
    float sinhf(float _X) { return ((float)sinh((double)_X)); }
    float sqrtf(float _X) { return ((float)sqrt((double)_X)); }
    float tanf(float _X) { return ((float)tan((double)_X)); }
    float tanhf(float _X) { return ((float)tanh((double)_X)); }
#endif

/**
 * Formula symbols
 */
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
#	define FORMULA_EXPORT __declspec(dllexport)
#else
#	define FORMULA_EXPORT __attribute__ ((visibility("default")))
#endif

typedef struct Stereo {
    float left;
    float right;
} Stereo;

#define _formula_main_decl float _formula_main(float input)
#define formula_main inline _formula_main_decl

#define _formula_main_stereo_decl Stereo _formula_main(Stereo input)
#define formula_main_stereo inline _formula_main_stereo_decl

/**
 * Global variables and macros
 */
char** __debug_stack;
int* __debug_idx;
int __debug_stack_size;
int __debug_enabled;

#define __DEBUG_STACK_SIZE_REACHED_STR "Maximum debug stack size reached (beware of DEBUG() in loops!)"
#define __MAX_DEBUG_REACHED sprintf(__debug_stack[__debug_stack_size - 1], __DEBUG_STACK_SIZE_REACHED_STR); (*__debug_idx)++;
#define __PUSH_DEBUG(x) sprintf(__debug_stack[(*__debug_idx)++], "%s: %f (line %i)", #x, x, __LINE__);
#define DEBUG(x)  if (!__debug_enabled || !__debug_stack); else if (*__debug_idx >= __debug_stack_size - 1) { __MAX_DEBUG_REACHED } else __PUSH_DEBUG(x)

const float* __knobs;
const float* __switches;
float __time = 0;
float __sample_rate;

#define KNOB_1 __knobs[0]
#define KNOB_2 __knobs[1]
#define KNOB_3 __knobs[2]
#define KNOB_4 __knobs[3]
#define KNOB_5 __knobs[4]
#define KNOB_6 __knobs[5]
#define KNOB_7 __knobs[6]
#define KNOB_8 __knobs[7]
#define KNOB_9 __knobs[8]
#define KNOB_10 __knobs[9]
#define KNOB_11 __knobs[10]
#define KNOB_12 __knobs[11]
#define SWITCH_1 __switches[0]
#define SWITCH_2 __switches[2]
#define SWITCH_3 __switches[3]

#define TIME __time
#define SAMPLE_RATE __sample_rate

#endif //FORMULA_ASSETS_LIBFORMULA_INCLUDED
