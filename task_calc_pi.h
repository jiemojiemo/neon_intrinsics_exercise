//
// Created by user on 12/14/22.
//

#ifndef NEON_INTRINSICS_EXERCISE_TASK_CALC_PI_H
#define NEON_INTRINSICS_EXERCISE_TASK_CALC_PI_H
#include <arm_neon.h>
constexpr static long num_steps = 100000;

float calc_pi()
{
    float step = 1.0f/num_steps;
    float x{0.0f};
    float sum{0.0f};
    for(int i = 0; i < num_steps; ++i)
    {
        x = (i + 0.5f) * step;
        sum += 4.0f / (1.0f + x * x);
    }
    return step * sum;
}

float calc_pi_expand()
{
    float step = 1.0f/num_steps;
    float x{0.0f};
    float sum{0.0f};
    int i = 0;
    for(; i < num_steps; i+=4)
    {
        auto x0 = (i + 0.5f) * step;
        auto x1 = (i + 1 + 0.5f) * step;
        auto x2 = (i + 2 + 0.5f) * step;
        auto x3 = (i + 3 + 0.5f) * step;

        sum += 4.0f / (1.0f + x0 * x0);
        sum += 4.0f / (1.0f + x1 * x1);
        sum += 4.0f / (1.0f + x2 * x2);
        sum += 4.0f / (1.0f + x3 * x3);
    }

    for(;i<num_steps;++i)
    {
        auto x0 = (i + 0.5f) * step;
        sum += 4.0f / (1.0f + x0 * x0);
    }

    return step * sum;
}

float calc_pi_neon()
{
    const float step = 1.0f/num_steps;
    float x{0.0f};
    int i = 0;
    float32x4_t ones = vdupq_n_f32(1.0f);
    float32x4_t fours = vdupq_n_f32(4.0f);
    float32x4_t sum = vdupq_n_f32(0.0f);
    for(; i < num_steps; i+=4)
    {
        float32x4_t x_chunk{i+0.5f, i+1.5f, i+2.5f, i+3.5f};
        x_chunk = vmulq_n_f32(x_chunk, step);
        x_chunk = vmulq_f32(x_chunk, x_chunk);
        x_chunk = vaddq_f32(ones, x_chunk);

        float32x4_t temp = vdivq_f32(fours, x_chunk);
        sum = vaddq_f32(temp, sum);
    }

    x = vaddvq_f32(sum);

    for(;i<num_steps;++i)
    {
        auto x0 = (i + 0.5f) * step;
        x += 4.0f / (1.0f + x0 * x0);
    }

    return step * x;
}



#endif//NEON_INTRINSICS_EXERCISE_TASK_CALC_PI_H
