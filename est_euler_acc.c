
#include <string.h>
#include <math.h>

#include "vector.h"
#include "imu.h"
#include "align_dcm.h"
#include "est.h"
#include "est_euler_acc.h"

#define ALPHA 0.4f

static struct {
    matrix align;

    double prev_roll;
    double prev_pitch;
} _obj;

void est_euler_acc_init() {
    memset(&_obj, 0, sizeof(_obj));
    align_dcm_init(&_obj.align);
}

void est_euler_acc_do(const imu_output* io, double dt, estimator_output* eo) {
    vector aligned_acc;
    matrix_multiply(&_obj.align, &io->acc, &aligned_acc);

    double cur_roll = atan2(aligned_acc.y, aligned_acc.z);
    double cur_pitch = -atan2(aligned_acc.x, sqrt(aligned_acc.y*aligned_acc.y + aligned_acc.z*aligned_acc.z));

    _obj.prev_roll = _obj.prev_roll*(1 - ALPHA) + ALPHA*cur_roll;
    _obj.prev_pitch = _obj.prev_pitch*(1 - ALPHA) + ALPHA*cur_pitch;

    eo->roll = _obj.prev_roll;
    eo->pitch = _obj.prev_pitch;
    eo->yaw = 0;
}
