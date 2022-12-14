//
// Created by user on 12/8/22.
//

#ifndef NEONTEST_RUN_TASK_H
#define NEONTEST_RUN_TASK_H

#include "BigRandomVectors.h"
#include "benchmark.h"
#include "task_0_sum.h"
#include "task_1.h"
#include "task_calc_pi.h"
#include "task_fir.h"
#include "task_max_abs.h"
#include "task_stereo_iir.h"
#include <vector>

void run_iir_filter() {
    auto check_result = []() {
        const int kSize = 1000;
        auto left = buildRandomVector<float>(kSize, 0, 1000);
        auto right = buildRandomVector<float>(kSize, 0, 1000);
        auto left_0 = left;
        auto right_0 = right;

        iirFilter(left.data(), right.data(), kSize);
        iirFilterNeon(left_0.data(), right_0.data(), kSize);

        return is_same_vector(left, left_0);
    };

    if (!check_result()) {
        LOGD("neon's result is not same with base's result\n");
        return;
    }

    LOGD("xxxxxxxxxxxxxxxxx task stereo iir filter xxxxxxxxxxxxxxxxxx\n");
    const int kSize = 1000000;
    auto left = buildRandomVector<float>(kSize, -1000, 1000);
    auto right = buildRandomVector<float>(kSize, -1000, 1000);
    auto left_0 = left;
    auto right_0 = right;
    const int kCallNum = 100;
    auto r = benchmark([&]() {
        iirFilter(left.data(), right.data(), kSize);
    },
                       kCallNum);
    LOGD("=========== baseline ==================\n");
    LOGD("total time: %d, average time: %d\n", r.first.count(), r.second.count());

    auto r_neon = benchmark([&]() {
        iirFilterNeon(left_0.data(), right_0.data(), kSize);
    },
                            kCallNum);
    LOGD("=========== neon ==================\n");
    LOGD("total time: %d, average time: %d\n", r_neon.first.count(), r_neon.second.count());
    LOGD("xxxxxxxxxxxxxxxxx task stereo iir value end xxxxxxxxxxxxxxxxxx\n");
}

void run_task_0() {
    auto check_result = []() {
        const int kSize = 8;
        auto numbers = buildRandomVector<float>(kSize, 0, 10000);

        auto base_result = sum(numbers.data(), numbers.size());
        auto neon_result = sum_neon(numbers.data(), numbers.size());

        return base_result == neon_result;
    };

    if (!check_result()) {
        LOGD("neon's result is not same with base's result\n");
        return;
    }


    LOGD("xxxxxxxxxxxxxxxxx task0 xxxxxxxxxxxxxxxxxx\n");
    const int kSize = 1000000;
    auto numbers = buildRandomVector<float>(kSize);

    const int kCallNum = 1000;
    auto r = benchmark<std::chrono::microseconds>([&numbers]() {
        sum(numbers.data(), numbers.size());
    },
                                                  kCallNum);
    LOGD("=========== baseline ==================\n");
    LOGD("total time: %d, average time: %d\n", r.first.count(), r.second.count());


    auto r_neon = benchmark<std::chrono::microseconds>([&numbers]() {
        sum_neon(numbers.data(), numbers.size());
    },
                                                       kCallNum);
    LOGD("=========== neon ==================\n");
    LOGD("total time: %d, average time: %d\n", r_neon.first.count(), r_neon.second.count());
    LOGD("xxxxxxxxxxxxxxxxx task0 end xxxxxxxxxxxxxxxxxx\n");
}

void run_task_1() {
    auto check_result = []() {
        const int kSize = 1000;
        auto left = buildRandomVector<float>(kSize, 0, 1000);
        auto right = buildRandomVector<float>(kSize, 0, 1000);
        auto left_volume = 0.3f;
        auto right_volume = 0.7f;

        auto c_result = std::vector<float>(left.size(), 0.0f);
        auto neon_result = std::vector<float>(left.size(), 0.0f);

        mix(left.data(), left_volume,
            right.data(), right_volume,
            c_result.data(), kSize);

        mix_neon(left.data(), left_volume,
                 right.data(), right_volume,
                 neon_result.data(), kSize);

        return is_same_vector(c_result, neon_result);
    };
    if (!check_result()) {
        LOGD("neon's result is not same with base's result\n");
        return;
    }

    LOGD("xxxxxxxxxxxxxxxxx task1 xxxxxxxxxxxxxxxxxx\n");
    const int kSize = 1000000;
    auto left = buildRandomVector<float>(kSize);
    auto right = buildRandomVector<float>(kSize);
    auto left_volume = 0.3f;
    auto right_volume = 0.7f;

    const int kCallNum = 100;
    auto r = benchmark([&]() {
        mix(left.data(), left_volume,
            right.data(), right_volume,
            left.data(), kSize);
    },
                       kCallNum);
    LOGD("=========== baseline ==================\n");
    LOGD("total time: %d, average time: %d\n", r.first.count(), r.second.count());


    auto r_neon = benchmark([&]() {
        mix_neon(left.data(), left_volume,
                 right.data(), right_volume,
                 left.data(), kSize);
    },
                            kCallNum);
    LOGD("=========== neon ==================\n");
    LOGD("total time: %d, average time: %d\n", r_neon.first.count(), r_neon.second.count());
    LOGD("xxxxxxxxxxxxxxxxx task1 end xxxxxxxxxxxxxxxxxx\n");
}

void run_task_fir() {
    const auto &signal = random2;
    const auto &filter = random1;
    int inputLength = signal.size();
    int filterSize = filter.size();

    int inputPaddingSize = inputLength + (filterSize - 1);
    std::vector<float> padding(inputPaddingSize, 0.0f);
    std::copy_n(signal.begin(), signal.size(), padding.begin());

    FilterInput input{};
    input.x = padding.data();
    input.inputLength = inputLength;
    input.c = filter.data();
    input.filterLength = filterSize;

    std::vector<float> base_result(signal.size(), 0.0f);
    std::vector<float> inner_loop(signal.size(), 0.0f);
    std::vector<float> out_loop(signal.size(), 0.0f);
    std::vector<float> innerout_loop(signal.size(), 0.0f);
    input.outputLength = base_result.size();

    auto check_result = [&]() {
        input.y = base_result.data();
        applyFirFilterSingle(input);

        input.y = inner_loop.data();
        applyFirFilterInnerLoopVectorizationARM(input);

        input.y = out_loop.data();
        applyFirFilterOuterLoopVectorizationARM(input);

        return is_same_vector(base_result, inner_loop) && is_same_vector(base_result, out_loop);
    };
    if (!check_result()) {
        LOGD("neon's result is not same with base's result\n");
        return;
    }

    LOGD("xxxxxxxxxxxxxxxxx task fir xxxxxxxxxxxxxxxxxx\n");

    const int kCallNum = 100;

    input.y = base_result.data();
    auto r = benchmark([&]() {
        applyFirFilterSingle(input);
    },
                       kCallNum);
    LOGD("=========== baseline ==================\n");
    LOGD("total time: %d, average time: %d\n", r.first.count(), r.second.count());


    input.y = inner_loop.data();
    auto vil_r = benchmark([&]() {
        applyFirFilterInnerLoopVectorizationARM(input);
    },
                           kCallNum);
    LOGD("=========== neon VIL ==================\n");
    LOGD("total time: %d, average time: %d\n", vil_r.first.count(), vil_r.second.count());


    input.y = inner_loop.data();
    auto vol_r = benchmark([&]() {
        applyFirFilterOuterLoopVectorizationARM(input);
    },
                           kCallNum);
    LOGD("=========== neon VOL ==================\n");
    LOGD("total time: %d, average time: %d\n", vol_r.first.count(), vol_r.second.count());

    input.y = inner_loop.data();
    auto viol_r = benchmark([&]() {
        applyFirFilterOuterInnerLoopVectorizationARM(input);
    },
                            kCallNum);
    LOGD("=========== neon VIOL ==================\n");
    LOGD("total time: %d, average time: %d\n", viol_r.first.count(), viol_r.second.count());

    LOGD("xxxxxxxxxxxxxxxxx task fir end xxxxxxxxxxxxxxxxxx\n");
}

void run_task_max_abs() {
    auto check_result = []() {
        auto random_number = buildRandomVector<int32_t>(1000, std::numeric_limits<int32_t>::min());
        auto base_r = MaxAbsValueW32(random_number.data(), random_number.size());
        auto neon_r = WebRtcSpl_MaxAbsValueW32Neon(random_number.data(), random_number.size());
        return base_r == neon_r;
    };
    if (!check_result()) {
        LOGD("neon's result is not same with base's result\n");
        return;
    }

    LOGD("xxxxxxxxxxxxxxxxx task max abs value xxxxxxxxxxxxxxxxxx\n");
    const int kSize = 1000000;
    auto random_number = buildRandomVector<int32_t>(kSize);
    const int kCallNum = 1000;
    auto r = benchmark<std::chrono::nanoseconds>([&]() {
        MaxAbsValueW32(random_number.data(), random_number.size());
    },
                                                 kCallNum);
    LOGD("=========== baseline ==================\n");
    LOGD("total time: %d, average time: %d\n", r.first.count(), r.second.count());

    auto r_neon = benchmark<std::chrono::nanoseconds>([&]() {
        WebRtcSpl_MaxAbsValueW32Neon(random_number.data(), random_number.size());
    },
                                                      kCallNum);
    LOGD("=========== neon ==================\n");
    LOGD("total time: %d, average time: %d\n", r_neon.first.count(), r_neon.second.count());
    LOGD("xxxxxxxxxxxxxxxxx task max abs value end xxxxxxxxxxxxxxxxxx\n");
}

void run_task_calc_pi() {
    auto check_result = []() {
        auto pi0 = calc_pi();
        auto pi1 = calc_pi_neon();
        return fabs(pi0 - pi1) < 1e-4;
    };
    if (!check_result()) {
        LOGD("neon's result is not same with base's result\n");
        return;
    }

    LOGD("xxxxxxxxxxxxxxxxx task calculate pi xxxxxxxxxxxxxxxxxx\n");
    const int kCallNum = 1000;
    auto r = benchmark<std::chrono::nanoseconds>([&]() {
        calc_pi();
    },
                                                 kCallNum);
    LOGD("=========== baseline ==================\n");
    LOGD("total time: %d, average time: %d\n", r.first.count(), r.second.count());

    auto r_neon = benchmark<std::chrono::nanoseconds>([&]() {
        calc_pi_neon();
    },
                                                      kCallNum);
    LOGD("=========== neon ==================\n");
    LOGD("total time: %d, average time: %d\n", r_neon.first.count(), r_neon.second.count());
    LOGD("xxxxxxxxxxxxxxxxx task calculate pi end xxxxxxxxxxxxxxxxxx\n");
}

#endif//NEONTEST_RUN_TASK_H
