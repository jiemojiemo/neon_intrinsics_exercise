//
// Created by user on 12/12/22.
//

#ifndef NEON_INTRINSICS_EXERCISE_TASK_FIR_H
#define NEON_INTRINSICS_EXERCISE_TASK_FIR_H
#include <array>
#ifdef __aarch64__
#include <arm_neon.h>
#endif
struct FilterInput {
    // assume that these fields are correctly initialized
    const float* x;       // input signal with (N_h-1) zeros appended
    size_t inputLength;   // N_x
    const float* c;       // reversed filter coefficients
    size_t filterLength;  // N_h
    float* y;             // output (filtered) signal;
                        // pointer to preallocated, uninitialized memory
    size_t outputLength;  // should be N_x in our context
};

float* applyFirFilterSingle(FilterInput& input) {
    const auto* x = input.x;
    const auto* c = input.c;
    auto* y = input.y;

    for (auto i = 0u; i < input.outputLength; ++i) {
        y[i] = 0.f;
        for (auto j = 0u; j < input.filterLength; ++j) {
            y[i] += x[i + j] * c[j];
        }
    }
    return y;
}

#ifdef __aarch64__

float* applyFirFilterInnerLoopVectorizationARM(FilterInput& input) {
    const auto* x = input.x;
    const auto* c = input.c;
    auto* y = input.y;

    for (auto i = 0u; i < input.outputLength; ++i) {
        y[i] = 0.f;
        float32x4_t outChunk = vdupq_n_f32(0.0f);
        for (auto j = 0u; j < input.filterLength; j += 4) {
            float32x4_t xChunk = vld1q_f32(x + i + j);
            float32x4_t cChunk = vld1q_f32(c + j);
            float32x4_t temp = vmulq_f32(xChunk, cChunk);
            outChunk = vaddq_f32(outChunk, temp);
        }
        y[i] = vaddvq_f32(outChunk);
    }
    return y;
}

float* applyFirFilterOuterLoopVectorizationARM(FilterInput& input) {
    const auto* x = input.x;
    const auto* c = input.c;
    auto* y = input.y;

    // Note the increment by 4
    for (auto i = 0u; i < input.outputLength; i += 4) {
        float32x4_t yChunk{0.0f, 0.0f, 0.0f, 0.0f};
        for (auto j = 0u; j < input.filterLength; ++j) {
            float32x4_t xChunk = vld1q_f32(x + i + j);
            float32x4_t temp = vmulq_n_f32(xChunk, c[j]);
            yChunk = vaddq_f32(yChunk, temp);
        }
        // store to memory
        vst1q_f32(y + i, yChunk);
    }
    return y;
}

float* applyFirFilterOuterInnerLoopVectorizationARM(FilterInput& input)
{
    const auto* x = input.x;
    const auto* c = input.c;
    auto* y = input.y;

    const int K = 4;
    std::array<float32x4_t, K> outChunk{};

    for (auto i = 0u; i < input.outputLength; i += K) {
        for(auto k = 0; k < K; ++k){
            outChunk[k] = vdupq_n_f32(0.0f);
        }

        for (auto j = 0u; j < input.filterLength; j += 4) {
            float32x4_t cChunk = vld1q_f32(c + j);

            for(auto k = 0; k < K; ++k)
            {
                float32x4_t xChunk = vld1q_f32(x + i + j +k);
                float32x4_t temp = vmulq_f32(cChunk, xChunk);
                outChunk[k] = vaddq_f32(temp, outChunk[k]);
            }

        }

        for(auto k = 0; k < K; ++k){
            y[i + k] = vaddvq_f32(outChunk[k]);
        }
    }

    return input.y;
}
#endif

#endif//NEON_INTRINSICS_EXERCISE_TASK_FIR_H
