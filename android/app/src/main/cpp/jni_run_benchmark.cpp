//
// Created by user on 12/7/22.
//

#include "benchmark.h"
#include "run_task.h"
#include <jni.h>
#include <android/log.h>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;
extern "C"
JNIEXPORT jfloat JNICALL
Java_com_xinging_neontest_MainActivity_runBenchmark(JNIEnv *env, jobject thiz) {
    run_task_calc_pi();
    run_task_0();
    run_task_1();
    run_task_fir();
    run_task_max_abs();
    run_iir_filter();
    return 0.0f;
}


short *generateRamp(short startValue, short len) {
    short *ramp = new short[len];
    for (short i = 0; i < len; i++) {
        ramp[i] = startValue + i;
    }
    return ramp;
}

double msElapsedTime(std::chrono::system_clock::time_point start) {
    auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

std::chrono::system_clock::time_point now() {
    return std::chrono::system_clock::now();
}

int dotProduct(short *vector1, short *vector2, short len) {
    int result = 0;

    for (short i = 0; i < len; i++) {
        result += vector1[i] * vector2[i];
    }

    return result;
}

int dotProductNeon(short *vector1, short *vector2, short len) {
    const short transferSize = 4;
    short segments = len / transferSize;

    // 4-element vector of zeros
    int32x4_t partialSumsNeon = vdupq_n_s32(0);

    // Main loop (note that loop index goes through segments)
    for (short i = 0; i < segments; i++) {
        // Load vector elements to registers
        short offset = i * transferSize;
        int16x4_t vector1Neon = vld1_s16(vector1 + offset);
        int16x4_t vector2Neon = vld1_s16(vector2 + offset);

        // Multiply and accumulate: partialSumsNeon += vector1Neon * vector2Neon
        partialSumsNeon = vmlal_s16(partialSumsNeon, vector1Neon, vector2Neon);
    }

    // Store partial sums
    int partialSums[transferSize];
    vst1q_s32(partialSums, partialSumsNeon);

    // Sum up partial sums
    int result = 0;
    for (short i = 0; i < transferSize; i++) {
        result += partialSums[i];
    }

    return result;
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_xinging_neontest_MainActivity_getString(
        JNIEnv *env,
        jobject /* this */) {

// Ramp length and number of trials
    const int rampLength = 1024;
    const int trials = 10000;

// Generate two input vectors
// (0, 1, ..., rampLength - 1)
// (100, 101, ..., 100 + rampLength-1)
    auto ramp1 = generateRamp(0, rampLength);
    auto ramp2 = generateRamp(100, rampLength);

// Without NEON intrinsics
// Invoke dotProduct and measure performance
    int lastResult = 0;

    auto start = now();
    for (int i = 0; i < trials; i++) {
        lastResult = dotProduct(ramp1, ramp2, rampLength);
    }
    auto elapsedTime = msElapsedTime(start);

// With NEON intrinsics
// Invoke dotProductNeon and measure performance
    int lastResultNeon = 0;

    start = now();
    for (int i = 0; i < trials; i++) {
        lastResultNeon = dotProductNeon(ramp1, ramp2, rampLength);
    }
    auto elapsedTimeNeon = msElapsedTime(start);

// Clean up
    delete ramp1, ramp2;

// Display results
    std::string resultsString =
            "----==== NO NEON ====----\nResult: " + to_string(lastResult)
            + "\nElapsed time: " + to_string((int) elapsedTime) + " ms"
            + "\n\n----==== NEON ====----\n"
            + "Result: " + to_string(lastResultNeon)
            + "\nElapsed time: " + to_string((int) elapsedTimeNeon) + " ms";

    return env->NewStringUTF(resultsString.c_str());
}
