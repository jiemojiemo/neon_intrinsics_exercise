//
// Created by user on 12/12/22.
//

#ifndef NEON_INTRINSICS_EXERCISE_TASK_MAX_ABS_H
#define NEON_INTRINSICS_EXERCISE_TASK_MAX_ABS_H
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <algorithm>
#ifdef __aarch64__
#include <arm_neon.h>
#endif

#define WEBRTC_SPL_WORD32_MAX (int32_t)0x7fffffff
#define WEBRTC_SPL_WORD32_MIN (int32_t)0x80000000
#define WEBRTC_SPL_MIN(A, B) (A < B ? A : B)  // Get min value
#define WEBRTC_SPL_MAX(A, B) (A > B ? A : B)  // Get max value

int32_t MaxAbsValueW32(const int32_t* vector, size_t length)
{
    int32_t max_abs_value = vector[0];
    for(int i = 0; i < length; ++i){
        auto abs_value = abs(vector[i]);
        if(abs_value > max_abs_value)
            max_abs_value = abs_value;
    }
    return max_abs_value;
}

int32_t WebRtcSpl_MaxAbsValueW32Neon(const int32_t* vector, size_t length) {
    // Use uint32_t for the local variables, to accommodate the return value
    // of abs(0x80000000), which is 0x80000000.

    uint32_t absolute = 0, maximum = 0;
    size_t i = 0;
    size_t residual = length & 0x7;

    const int32_t* p_start = vector;
    uint32x4_t max32x4_0 = vdupq_n_u32(0);
    uint32x4_t max32x4_1 = vdupq_n_u32(0);

    // First part, unroll the loop 8 times.
    for (i = 0; i < length - residual; i += 8) {
        int32x4_t in32x4_0 = vld1q_s32(p_start);
        p_start += 4;
        int32x4_t in32x4_1 = vld1q_s32(p_start);
        p_start += 4;
        in32x4_0 = vabsq_s32(in32x4_0);
        in32x4_1 = vabsq_s32(in32x4_1);
        // vabs doesn't change the value of 0x80000000.
        // Use u32 so we don't lose the value 0x80000000.
        max32x4_0 = vmaxq_u32(max32x4_0, vreinterpretq_u32_s32(in32x4_0));
        max32x4_1 = vmaxq_u32(max32x4_1, vreinterpretq_u32_s32(in32x4_1));
    }

    uint32x4_t max32x4 = vmaxq_u32(max32x4_0, max32x4_1);
#if defined(__arm64)
    maximum = vmaxvq_u32(max32x4);
#else
    uint32x2_t max32x2 = vmax_u32(vget_low_u32(max32x4), vget_high_u32(max32x4));
    max32x2 = vpmax_u32(max32x2, max32x2);

    maximum = vget_lane_u32(max32x2, 0);
#endif

    // Second part, do the remaining iterations (if any).
    for (i = residual; i > 0; i--) {
        absolute = abs((int)(*p_start));
        if (absolute > maximum) {
            maximum = absolute;
        }
        p_start++;
    }

    // Guard against the case for 0x80000000.
    maximum = WEBRTC_SPL_MIN(maximum, WEBRTC_SPL_WORD32_MAX);

    return (int32_t)maximum;
}


#endif//NEON_INTRINSICS_EXERCISE_TASK_MAX_ABS_H
