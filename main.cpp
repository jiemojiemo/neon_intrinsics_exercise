#include "benchmark.h"
#include "run_task.h"
#include <iostream>


int main() {
    run_task_calc_pi();
    run_task_0();
    run_task_1();
    run_task_fir();
    run_task_max_abs();
    run_iir_filter();
    return 0;
}
