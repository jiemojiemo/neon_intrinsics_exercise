//
// Created by user on 12/6/22.
//

#ifndef NEON_INTRINSICS_EXERCISE_TASK_1_H
#define NEON_INTRINSICS_EXERCISE_TASK_1_H
#include "benchmark.h"
#include <arm_neon.h>
void mix(float *left, float left_volume,
         float *right, float right_volume,
         float *output, size_t size) {
    for (int i = 0; i < size; ++i) {
        output[i] = left[i] * left_volume + right[i] * right_volume;
    }
}

void mix_expand(float *left, float left_volume,
                float *right, float right_volume,
                float *output, size_t size) {
    int i = 0;
    for (; i < size; i += 4) {
        output[i] =     left[i] * left_volume + right[i] * right_volume;
        output[i + 1] = left[i + 1] * left_volume + right[i + 1] * right_volume;
        output[i + 2] = left[i + 2] * left_volume + right[i + 2] * right_volume;
        output[i + 3] = left[i + 3] * left_volume + right[i + 3] * right_volume;
    }

    for (; i < size; ++i) {
        output[i] = left[i] * left_volume + right[i] * right_volume;
    }
}

void mix_neon(float *left, float left_volume,
              float *right, float right_volume,
              float *output, size_t size) {
    int i = 0;
    for (; i < size; i += 4) {
        float32x4_t left_chunk = vld1q_f32(left + i);
        float32x4_t right_chunk = vld1q_f32(right + i);

        left_chunk = vmulq_n_f32(left_chunk, left_volume);
        right_chunk = vmulq_n_f32(right_chunk, right_volume);

        float32x4_t output_chunk = vaddq_f32(left_chunk, right_chunk);
        vst1q_f32(output + i, output_chunk);
    }

    for (; i < size; ++i) {
        output[i] = left[i] * left_volume + right[i] * right_volume;
    }
}

#endif//NEON_INTRINSICS_EXERCISE_TASK_1_H
