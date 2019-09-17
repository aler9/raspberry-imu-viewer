
#pragma once

typedef void est_euler_acct;

error* est_euler_acc_init(est_euler_acct** pobj, imut* imu);
void est_euler_acc_do(est_euler_acct* obj, const imu_output* io, double dt, estimator_output* eo);
