
#pragma once

typedef void est_euler_gyrounalignt;

error* est_euler_gyrounalign_init(est_euler_gyrounalignt** pobj, imu_autot* imu);
void est_euler_gyrounalign_do(est_euler_gyrounalignt* obj,
    const vector* gyro_out, double dt, estimator_output* eo);
