
#pragma once

void est_euler_gyrounalign_init();
void est_euler_gyrounalign_do(const imu_output* io, double dt, estimator_output* eo);
