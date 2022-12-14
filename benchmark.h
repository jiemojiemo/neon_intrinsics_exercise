//
// Created by user on 12/5/22.
//

#ifndef NEON_INTRINSICS_EXERCISE_BENCHMARK_H
#define NEON_INTRINSICS_EXERCISE_BENCHMARK_H
#include <functional>
#include <vector>
#include <algorithm>
#include <ctime>
#include <random>

#ifdef __ANDROID__
#include <android/log.h>

#define TAG "neon_test"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)

#else
#define LOGD(...) printf(__VA_ARGS__)
#endif

template <typename duration_type = std::chrono::microseconds>
auto benchmark(std::function<void()> fun,
               int call_count = 1
               )
{
    using namespace std::chrono;
    duration_type total_time{0};

    for(auto i = 0; i < call_count; ++i)
    {
        const auto start = high_resolution_clock::now();
        fun();
        const auto end = high_resolution_clock::now();
        total_time += duration_cast<duration_type>(end - start);
    }
    auto average_time = total_time / call_count;
    return std::pair{total_time, average_time};
}

template <typename T>
std::vector<T> buildRandomVector(size_t size, T min_val = 0, T max_val=std::numeric_limits<T>::max())
{
    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    std::uniform_int_distribution<T> dist{min_val, max_val};

    auto gen = [&dist, &mersenne_engine](){
        return dist(mersenne_engine);
    };

    std::vector<T> v(size);
    std::generate(v.begin(), v.end(), gen);
    return v;
}

template <typename T>
bool is_same_vector(const std::vector<T>& lfh, const std::vector<T>& rhs)
{
    if(lfh.size() != rhs.size()){
        return false;
    }

    for(int i = 0; i < lfh.size(); ++i)
    {
        if(fabs(lfh[i] - rhs[i]) > 1e-2){
            return false;
        }
    }

    return true;
}

#endif//NEON_INTRINSICS_EXERCISE_BENCHMARK_H
