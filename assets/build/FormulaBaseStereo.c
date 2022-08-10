#include "libformula.h"

_formula_main_stereo_decl;

FORMULA_EXPORT void process_block_stereo_1_1(float** in, int numSamples, float sampleRate,
                                         double bpm, double bar,
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
    __bpm = bpm;
    __bar = bar;
    for (int s = 0; s < numSamples; s++) {
        double fixDenormal = (1.0 / 4294967295.0);
        __time += 1 / sampleRate;
        Stereo output = _formula_main((Stereo) { .left = in[0][s]*inGain, .right = in[1][s]*inGain });
        in[0][s] = output.left*wet*outGain + in[0][s]*(1-wet);
        in[1][s] = output.right*wet*outGain + in[0][s]*(1-wet);
        __debug_enabled = 0;
    }
}

