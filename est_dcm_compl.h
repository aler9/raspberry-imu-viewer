
#pragma once

typedef void est_dcm_complt;

error* est_dcm_compl_init(est_dcm_complt** pobj, imut* imu);
void est_dcm_compl_do(est_dcm_complt* obj, const imu_output* io, double dt,
    estimator_output* eo);
