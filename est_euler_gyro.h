
#pragma once

void est_euler_gyro_init();
void est_euler_gyro_do(const imu_output* io, double dt, estimator_output* eo);
