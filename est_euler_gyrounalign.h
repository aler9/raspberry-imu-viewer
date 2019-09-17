
#pragma once

typedef void est_euler_gyrounalignt;

error* est_euler_gyrounalign_init(est_euler_gyrounalignt** pobj, imut* imu);
void est_euler_gyrounalign_do(est_euler_gyrounalignt* obj, const imu_output* io,
    double dt, estimator_output* eo);
