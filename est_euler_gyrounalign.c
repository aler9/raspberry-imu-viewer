
#include <string.h>
#include <math.h>

#include "vector.h"
#include "imu.h"
#include "gyro_bias.h"
#include "est.h"
#include "est_euler_gyrounalign.h"

static struct {
    vector gyro_bias;

    double prev_roll;
    double prev_pitch;
    double prev_yaw;
} _obj;

void est_euler_gyrounalign_init() {
    memset(&_obj, 0, sizeof(_obj));
    gyro_bias_init(&_obj.gyro_bias);
}

void est_euler_gyrounalign_do(const imu_output* io, double dt, estimator_output* eo) {
    vector tuned_gyro;
    vector_diff(&io->gyro, &_obj.gyro_bias, &tuned_gyro);

    _obj.prev_roll = _obj.prev_roll + tuned_gyro.x*dt;
    _obj.prev_pitch = _obj.prev_pitch + tuned_gyro.y*dt;
    _obj.prev_yaw = _obj.prev_yaw + tuned_gyro.z*dt;

    eo->roll = _obj.prev_roll;
    eo->pitch = _obj.prev_pitch;
    // eo->yaw = _obj.prev_yaw;
    // do not use yaw, such that the result can be compared with other estimators
    eo->yaw = 0;
}
