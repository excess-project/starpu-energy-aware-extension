#ifndef __UTILS_H_
#define __UTILS_H_

#include <stddef.h>
#define CPU_ENERGY 1
#define GPU_ENERGY 2
#define ALL_ENERGY 3

int mf_starpu_task_training(struct starpu_task *task, unsigned nimpl);
void mf_starpu_show_expectation(struct starpu_task *task, unsigned nimpl);
void mf_starpu_metrics_feed(struct starpu_task *task, unsigned nimpl, double energy);
float get_mf_power_data(long double start_time, long double end_time, char *Metric);
double get_gpu_energy(long double start_time, long double end_time);
double get_cpu_energy(long double start_time, long double end_time);
double mf_starpu_get_energy(long double start_t, long double end_t, unsigned ENERGY_TYPE);
int mf_starpu_get_execution_id(int argc, char **argv, char *exe_id);
int mf_starpu_get_train_loops(int argc, char **argv);
int mf_starpu_init(struct starpu_conf *conf, char *execution_id);
long double mf_starpu_time(void);

#endif