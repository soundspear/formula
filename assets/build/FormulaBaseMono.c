#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
#	define FORMULA_EXPORT __declspec(dllexport)
#else
#	define FORMULA_EXPORT __attribute__ ((visibility("default")))
#endif

#define _formula_main_decl float _formula_main(float input)
_formula_main_decl;
#define formula_main inline _formula_main_decl

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

FORMULA_EXPORT void process_block_mono(float* in, int numSamples, float sampleRate,
                                       const float* knobs, const float* switches,
                                       float wet, float inGain, float outGain,
                                       int* debug_idx, char** debug_stack, int debug_stack_size) {
    __debug_idx = debug_idx; *__debug_idx = 0;
    __debug_stack = debug_stack;
    __debug_stack_size = debug_stack_size;
    __debug_enabled = 1;
    __knobs = knobs;
    __switches = switches;
    __sample_rate = sampleRate;
    for (int s = 0; s < numSamples; s++) {
        __time += 1 / sampleRate;
        double fixDenormal = (1.0 / 4294967295.0);
        in[s] = outGain*(wet*_formula_main(in[s]*inGain) + (1-wet)*in[s]);
        __debug_enabled = 0;
    }
}

