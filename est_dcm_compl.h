
#pragma once

void est_dcm_compl_init();
void est_dcm_compl_do(const imu_output* io, double dt, estimator_output* eo);
