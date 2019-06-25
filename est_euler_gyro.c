
#include <string.h>
#include <math.h>

#include "vector.h"
#include "imu.h"
#include "align_dcm.h"
#include "gyro_bias.h"
#include "est.h"
#include "est_euler_gyro.h"

typedef struct {
    vector gyro_bias;
    matrix align;

    double prev_roll;
    double prev_pitch;
    double prev_yaw;

} _est_euler_gyro;

_est_euler_gyro _e_e_gyrop, *_e_e_gyro = &_e_e_gyrop;

void est_euler_gyro_init() {
    memset(_e_e_gyro, 0, sizeof(_est_euler_gyro));
    gyro_bias_init(&_e_e_gyro->gyro_bias);
    align_dcm_init(&_e_e_gyro->align);
}

void est_euler_gyro_do(const imu_output* io, double dt, estimator_output* eo) {
    vector tuned_gyro;
    vector_diff(&io->gyro, &_e_e_gyro->gyro_bias, &tuned_gyro);

    vector aligned_gyro;
    matrix_multiply(&_e_e_gyro->align, &tuned_gyro, &aligned_gyro);

    _e_e_gyro->prev_roll = _e_e_gyro->prev_roll + aligned_gyro.x*dt;
    _e_e_gyro->prev_pitch = _e_e_gyro->prev_pitch + aligned_gyro.y*dt;
    _e_e_gyro->prev_yaw = _e_e_gyro->prev_yaw + aligned_gyro.z*dt;

    eo->roll = _e_e_gyro->prev_roll;
    eo->pitch = _e_e_gyro->prev_pitch;
    //eo->yaw = _e_e_gyro->prev_yaw;
    // do not use yaw, such that the result can be compared with other estimators
    eo->yaw = 0;
}
