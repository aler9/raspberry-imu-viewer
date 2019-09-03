
#include <string.h>
#include <math.h>

#include "vector.h"
#include "imu.h"
#include "align_dcm.h"
#include "gyro_bias.h"
#include "est.h"
#include "est_dcm_compl.h"

#define DCM_COMPL_ALPHA 0.1f

typedef struct {
    vector gyro_bias;
    matrix align;

    vector prev_K;

} _est_dcm_compl;

static _est_dcm_compl _obj;

void est_dcm_compl_init() {
    memset(&_obj, 0, sizeof(_est_dcm_compl));
    gyro_bias_init(&_obj.gyro_bias);
    align_dcm_init(&_obj.align);

    // initial K points downwards
    _obj.prev_K.x = 0;
    _obj.prev_K.y = 0;
    _obj.prev_K.z = 1;
}

void est_dcm_compl_do(const imu_output* io, double dt, estimator_output* eo) {
    vector aligned_acc;
    matrix_multiply(&_obj.align, &io->acc, &aligned_acc);

    vector tuned_gyro;
    vector_diff(&io->gyro, &_obj.gyro_bias, &tuned_gyro);

    vector aligned_gyro;
    matrix_multiply(&_obj.align, &tuned_gyro, &aligned_gyro);

    // get K from accelerometers
    vector acc_K;
    vector_copy(&aligned_acc, &acc_K);

    // get K from gyroscopes
    vector gyro_dK;
    vector gyro_K;
    vector_cross(&_obj.prev_K, &aligned_gyro, &gyro_dK);
    vector_copy(&_obj.prev_K, &gyro_K);
    gyro_K.x += dt * gyro_dK.x;
    gyro_K.y += dt * gyro_dK.y;
    gyro_K.z += dt * gyro_dK.z;

    // filter
    _obj.prev_K.x = DCM_COMPL_ALPHA*acc_K.x + (1 - DCM_COMPL_ALPHA)*gyro_K.x;
    _obj.prev_K.y = DCM_COMPL_ALPHA*acc_K.y + (1 - DCM_COMPL_ALPHA)*gyro_K.y;
    _obj.prev_K.z = DCM_COMPL_ALPHA*acc_K.z + (1 - DCM_COMPL_ALPHA)*gyro_K.z;

    // normalize
    vector_normalize(&_obj.prev_K);

    eo->roll = atan2(_obj.prev_K.y, _obj.prev_K.z);
    eo->pitch = -asin(_obj.prev_K.x);
    eo->yaw = 0;
}
