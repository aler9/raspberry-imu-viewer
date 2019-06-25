
#pragma once

void est_euler_acc_init();
void est_euler_acc_do(const imu_output* io, double dt, estimator_output* eo);
