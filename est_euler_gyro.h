
#pragma once

typedef void est_euler_gyrot;

error* est_euler_gyro_init(est_euler_gyrot** pobj, imut* imu);
void est_euler_gyro_do(est_euler_gyrot* obj, const imu_output* io, double dt,
    estimator_output* eo);
