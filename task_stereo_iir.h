//
// Created by user on 12/12/22.
//

#ifndef NEON_INTRINSICS_EXERCISE_TASK_STEREO_IIR_H
#define NEON_INTRINSICS_EXERCISE_TASK_STEREO_IIR_H

#ifdef __aarch64__
#include <arm_neon.h>
#include "xsimd/xsimd.hpp"
#endif

#include <array>

enum filter_coeff { a0,
                    a1,
                    a2,
                    b1,
                    b2,
                    c0,
                    d0,
                    num_coeffs };
enum state_registers { z0,
                       z1,
                       z2,
                       z3,
                       num_states };
using FilterCoeffs = std::array<float, num_coeffs>;

template <typename T>
class BiquadImpl {
public:
    T processSample(T in) {
        return processSampleWithDirectForm(in);
    }

    T processSampleWithDirectForm(T in) {
        T out = coeff_array[a0] * in + coeff_array[a1] * state_array[z0] +
                coeff_array[a2] * state_array[z1] -
                coeff_array[b1] * state_array[z2] -
                coeff_array[b2] * state_array[z3];

        state_array[z1] = state_array[z0];
        state_array[z0] = in;

        state_array[z3] = state_array[z2];
        state_array[z2] = out;

        return out;
    }

    T processSampleWithCanonicalForm(T in) {
        T wn = in - coeff_array[b1] * state_array[z0] -
               coeff_array[b2] * state_array[z1];
        T out = coeff_array[a0] * wn + coeff_array[a1] * state_array[z0] +
                coeff_array[a2] * state_array[z1];

        state_array[z1] = state_array[z0];
        state_array[z0] = wn;

        return out;
    }

    T processSampleWithTransposedDirectForm(T in) {
        T wn = in + state_array[z0];
        T out = coeff_array[a0] * wn + state_array[z2];

        state_array[z0] = -coeff_array[b1] + state_array[z1];
        state_array[z1] = -coeff_array[b2] * wn;

        state_array[z2] = coeff_array[a1] * wn + state_array[z3];
        state_array[z3] = coeff_array[a2] * wn;

        return out;
    }

    T processSampleWithTransposedCanonicalForm(T in) {
        T out = coeff_array[a0] * in + state_array[z0];

        state_array[z0] =
                coeff_array[a1] * in - coeff_array[b1] * out + state_array[z1];
        state_array[z1] = coeff_array[a2] * in - coeff_array[b2] * out;

        return out;
    }

    T operator()(T in) {
        return processSampleWithTransposedCanonicalForm(in);
    }

    void reset() {
        std::fill(coeff_array.begin(), coeff_array.end(), 0.0f);
        std::fill(state_array.begin(), state_array.end(), 0.0f);
    }

    void updateCoeffs(const FilterCoeffs &coeffs) {
        std::copy(coeffs.begin(), coeffs.end(), coeff_array.begin());
    }

    std::array<T, num_coeffs> coeff_array{{
            0.0f,
    }};
    std::array<T, num_states> state_array{{
            0.0f,
    }};
};

static FilterCoeffs test_coeffs{0.0667, 0.0667, 0, -0.8667, 0, 0};

void iirFilter(float* left_data,
               float* right_data,
               int size
               )
{
    BiquadImpl<float> left_biquad;
    BiquadImpl<float> right_biquad;
    left_biquad.updateCoeffs(test_coeffs);
    right_biquad.updateCoeffs(test_coeffs);

    for(int i = 0; i < size; ++i)
    {
        left_data[i] =  left_biquad.processSample(left_data[i]);
        right_data[i] =  right_biquad.processSample(right_data[i]);
    }
}

void iirFilterNeon(float* left_data,
               float* right_data,
               int size)
{
    using batch = xsimd::batch<float>;

    BiquadImpl<batch> biquad;
    biquad.updateCoeffs(test_coeffs);

    for(int i = 0; i < size; ++i)
    {
        batch chunk{left_data[i], right_data[i], 0, 0};
        auto r = biquad.processSample(chunk);
        left_data[i] = r.get(0);
        right_data[i] = r.get(1);
    }
}

#endif//NEON_INTRINSICS_EXERCISE_TASK_STEREO_IIR_H
