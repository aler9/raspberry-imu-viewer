
#include <string.h>
#include <math.h>

#include "vector.h"
#include "imu.h"
#include "gyro_bias.h"
#include "est.h"
#include "est_euler_gyrounalign.h"

typedef struct {
    vector gyro_bias;

    double prev_roll;
    double prev_pitch;
    double prev_yaw;

} _est_euler_gyrounalign;

_est_euler_gyrounalign _e_e_gyroup, *_e_e_gyrou = &_e_e_gyroup;

void est_euler_gyrounalign_init() {
    memset(_e_e_gyrou, 0, sizeof(_est_euler_gyrounalign));
    gyro_bias_init(&_e_e_gyrou->gyro_bias);
}

void est_euler_gyrounalign_do(const imu_output* io, double dt, estimator_output* eo) {
    vector tuned_gyro;
    vector_diff(&io->gyro, &_e_e_gyrou->gyro_bias, &tuned_gyro);

    _e_e_gyrou->prev_roll = _e_e_gyrou->prev_roll + tuned_gyro.x*dt;
    _e_e_gyrou->prev_pitch = _e_e_gyrou->prev_pitch + tuned_gyro.y*dt;
    _e_e_gyrou->prev_yaw = _e_e_gyrou->prev_yaw + tuned_gyro.z*dt;

    eo->roll = _e_e_gyrou->prev_roll;
    eo->pitch = _e_e_gyrou->prev_pitch;
    // eo->yaw = _e_e_gyrou->prev_yaw;
    // do not use yaw, such that the result can be compared with other estimators
    eo->yaw = 0;
}
