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

#define _formula_main_decl float _formula_main(float input, float sampleRate, const float* knobs, const float* switches)
_formula_main_decl;
#define formula_main inline _formula_main_decl;

char** __debug_stack;
int* __debug_idx;
int __debug_stack_size;
int __debug_enabled;

#define __DEBUG_STACK_SIZE_REACHED_STR "Maximum debug stack size reached (beware of DEBUG() in loops!)"
#define __MAX_DEBUG_REACHED sprintf(__debug_stack[__debug_stack_size - 1], __DEBUG_STACK_SIZE_REACHED_STR); (*__debug_idx)++;
#define __PUSH_DEBUG(x) sprintf(__debug_stack[(*__debug_idx)++], "%s: %f (line %i)", #x, x, __LINE__);
#define DEBUG(x)  if (!__debug_enabled || !__debug_stack); else if (*__debug_idx >= __debug_stack_size - 1) { __MAX_DEBUG_REACHED } else __PUSH_DEBUG(x)

#define knob_1 knobs[0]
#define knob_2 knobs[1]
#define knob_3 knobs[2]
#define knob_4 knobs[3]
#define knob_5 knobs[4]
#define knob_6 knobs[5]
#define knob_7 knobs[6]
#define knob_8 knobs[7]
#define knob_9 knobs[8]
#define knob_10 knobs[9]
#define knob_11 knobs[10]
#define knob_12 knobs[11]
#define switch_1 switches[0]
#define switch_2 switches[2]
#define switch_3 switches[3]


float time = 0;

FORMULA_EXPORT void process_block_mono(float* in, int numSamples, float sampleRate,
                                       const float* knobs, const float* switches,
                                       float wet, float inGain, float outGain,
                                       int* debug_idx, char** debug_stack, int debug_stack_size) {
    __debug_idx = debug_idx; *__debug_idx = 0;
    __debug_stack = debug_stack;
    __debug_stack_size = debug_stack_size;
    __debug_enabled = 1;
    for (int s = 0; s < numSamples; s++) {
        time += 1 / sampleRate;
        double fixDenormal = (1.0 / 4294967295.0);
        in[s] = outGain*(wet*_formula_main(in[s]*inGain, sampleRate, knobs, switches) + (1-wet)*in[s]);
        __debug_enabled = 0;
    }
}

