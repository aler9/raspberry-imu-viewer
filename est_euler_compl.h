
#pragma once

typedef void est_euler_complt;

error* est_euler_compl_init(est_euler_complt** pobj, imut* imu);
void est_euler_compl_do(est_euler_complt* obj, const imu_output* io, double dt,
    estimator_output* eo);
