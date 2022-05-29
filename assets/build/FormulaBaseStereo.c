#include "libformula.h"

_formula_main_stereo_decl;

FORMULA_EXPORT void process_block_stereo(float** in, int numSamples, float sampleRate,
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
        Stereo output = _formula_main((Stereo) { .left = in[0][s]*inGain, .right = in[1][s]*inGain });
        in[0][s] = (output.left*wet + in[0][s]*(1-wet))*outGain;
        in[1][s] = (output.right*wet + in[0][s]*(1-wet))*outGain;
        __debug_enabled = 0;
    }
}

