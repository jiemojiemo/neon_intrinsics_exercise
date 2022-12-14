//
// Created by user on 12/6/22.
//

#ifndef NEON_INTRINSICS_EXERCISE_TASK_1_SUM_H
#define NEON_INTRINSICS_EXERCISE_TASK_1_SUM_H
#include <arm_neon.h>
#include <algorithm>
#include <numeric>
float sum(float* array, size_t size)
{
    float s = 0.0f;
    for(int i = 0; i < size; ++i){
        s += array[i];
    }
    return s;
}

float sum_expand(float* array, size_t size)
{
    float s = 0.0f;
    int i = 0;
    for(; i < size; i += 4){
        s += array[i];
        s += array[i + 1];
        s += array[i + 2];
        s += array[i + 3];
    }

    for(; i < size; ++i) {
        s += array[i];
    }
    return s;
}

float sum_neon(float* array, size_t size)
{
    int i = 0;
    float32x4_t out_chunk{0.0f,0.0f,0.0f,0.0f};
    for(; i < size; i+=4){
        float32x4_t chunk = vld1q_f32(array + i);
        out_chunk = vaddq_f32(out_chunk, chunk);
    }

    float x = out_chunk[0] + out_chunk[1] + out_chunk[2] + out_chunk[3];
    for(;i < size; ++i){
        x += array[i];
    }

    return x;
}

#endif//NEON_INTRINSICS_EXERCISE_TASK_1_SUM_H
