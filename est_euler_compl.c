
#include <string.h>
#include <math.h>

#include "vector.h"
#include "imu.h"
#include "align_dcm.h"
#include "gyro_bias.h"
#include "est.h"
#include "est_euler_compl.h"

#define EULER_COMPL_ALPHA 0.1f

typedef struct {
    vector gyro_bias;
    matrix align;

    double prev_roll;
    double prev_pitch;

} _est_euler_compl;

static _est_euler_compl _obj;

void est_euler_compl_init() {
    memset(&_obj, 0, sizeof(_est_euler_compl));
    gyro_bias_init(&_obj.gyro_bias);
    align_dcm_init(&_obj.align);
}

void est_euler_compl_do(const imu_output* io, double dt, estimator_output* eo) {
    vector aligned_acc;
    matrix_multiply(&_obj.align, &io->acc, &aligned_acc);

    vector tuned_gyro;
    vector_diff(&io->gyro, &_obj.gyro_bias, &tuned_gyro);

    vector aligned_gyro;
    matrix_multiply(&_obj.align, &tuned_gyro, &aligned_gyro);

    double acc_roll = atan2(aligned_acc.y, aligned_acc.z);
    double acc_pitch = -atan2(aligned_acc.x, sqrt(aligned_acc.y*aligned_acc.y + aligned_acc.z*aligned_acc.z));

    _obj.prev_roll = acc_roll*EULER_COMPL_ALPHA + (1 - EULER_COMPL_ALPHA)*(_obj.prev_roll + aligned_gyro.x * dt);
    _obj.prev_pitch = acc_pitch*EULER_COMPL_ALPHA + (1 - EULER_COMPL_ALPHA)*(_obj.prev_pitch + aligned_gyro.y * dt);

    eo->roll = _obj.prev_roll;
    eo->pitch = _obj.prev_pitch;
    eo->yaw = 0;
}
