#include "libformula.h"

_formula_main_decl;

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
        double fixDenormal = (1.0 / 4294967295.0);
        __time += 1.0L / (long double)(sampleRate);
        in[s] = outGain*(wet*_formula_main(in[s]*inGain) + (1-wet)*in[s]);
        __debug_enabled = 0;
    }
}

