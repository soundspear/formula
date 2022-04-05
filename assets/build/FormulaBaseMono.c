#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#   define M_PI 3.14159265358979323846
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
#	define FORMULA_EXPORT __declspec(dllexport)
#else
#	define FORMULA_EXPORT __attribute__ ((visibility("default")))
#endif
#define formula_main inline float _formula_main(float input, float sampleRate, const float* knobs, const float* switches)
formula_main;

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

FORMULA_EXPORT void process_block_mono(float* in, int numSamples, float sampleRate, const float* knobs, const float* switches,
                                       float wet, float inGain, float outGain) {
    for (int s = 0; s < numSamples; s++) {
        time += 1 / sampleRate;
        in[s] = outGain*(wet*_formula_main(in[s]*inGain, sampleRate, knobs, switches) + (1-wet)*in[s]);
    }
}

