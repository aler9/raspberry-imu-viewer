
#include <string.h>
#include <math.h>

#include "vector.h"
#include "imu.h"
#include "align_dcm.h"
#include "est.h"
#include "est_euler_acc.h"

#define ACC_SMOOTHING_ALPHA 0.4f

typedef struct {
    matrix align;

    double prev_roll;
    double prev_pitch;

} _est_euler_acc;

_est_euler_acc _e_e_accp, *_e_e_acc = &_e_e_accp;

void est_euler_acc_init() {
    memset(_e_e_acc, 0, sizeof(_est_euler_acc));
    align_dcm_init(&_e_e_acc->align);
}

void est_euler_acc_do(const imu_output* io, double dt, estimator_output* eo) {
    vector aligned_acc;
    matrix_multiply(&_e_e_acc->align, &io->acc, &aligned_acc);

    double cur_roll = atan2(aligned_acc.y, aligned_acc.z);
    double cur_pitch = -atan2(aligned_acc.x, sqrt(aligned_acc.y*aligned_acc.y + aligned_acc.z*aligned_acc.z));

    _e_e_acc->prev_roll = _e_e_acc->prev_roll*(1 - ACC_SMOOTHING_ALPHA) + ACC_SMOOTHING_ALPHA*cur_roll;
    _e_e_acc->prev_pitch = _e_e_acc->prev_pitch*(1 - ACC_SMOOTHING_ALPHA) + ACC_SMOOTHING_ALPHA*cur_pitch;

    eo->roll = _e_e_acc->prev_roll;
    eo->pitch = _e_e_acc->prev_pitch;
    eo->yaw = 0;
}
