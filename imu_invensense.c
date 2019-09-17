
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>

#include "vector.h"
#include "imu.h"
#include "imu_invensense.h"

#define POWERMAN1 0x6B
#define POWERMAN1_DISABLE_TEMP (1 << 3)
#define ACC_CONF 0x1C
#define ACC_X 0x3B
#define GYRO_CONF 0x1B
#define GYRO_X 0x43

typedef struct {
    int fd;
    double gyro_ssf;
    double acc_ssf;
} _objt;

void* imu_invensense_init(int fd, invensense_acc_range acc_range,
    invensense_gyro_range gyro_range) {
    _objt* _obj = malloc(sizeof(_objt));
    _obj->fd = fd;

    uint8_t acc_conf = 0;

    switch(acc_range) {
    case INVENSENSE_ACC_2G:
        acc_conf = 0;
        _obj->acc_ssf = 16384.0f;
        break;

    case INVENSENSE_ACC_4G:
        acc_conf = (1 << 3);
        _obj->acc_ssf = 8192.0f;
        break;

    case INVENSENSE_ACC_8G:
        acc_conf = (2 << 3);
        _obj->acc_ssf = 4096.0f;
        break;

    case INVENSENSE_ACC_16G:
        acc_conf = (3 << 3);
        _obj->acc_ssf = 2048.0f;
        break;
    }

    uint8_t gyro_conf = 0;

    switch(gyro_range) {
    case INVENSENSE_GYRO_250:
        gyro_conf = 0;
        _obj->gyro_ssf = (131.0f * 180.0f / M_PI);
        break;

    case INVENSENSE_GYRO_500:
        gyro_conf = (1 << 3);
        _obj->gyro_ssf = (65.5f * 180.0f / M_PI);
        break;

    case INVENSENSE_GYRO_1000:
        gyro_conf = (2 << 3);
        _obj->gyro_ssf = (32.8f * 180.0f / M_PI);
        break;

    case INVENSENSE_GYRO_2000:
        gyro_conf = (3 << 3);
        _obj->gyro_ssf = (16.4f * 180.0f / M_PI);
        break;
    }

    uint8_t cmd[2];

    cmd[0] = POWERMAN1;
    cmd[1] = POWERMAN1_DISABLE_TEMP;
    write(_obj->fd, cmd, 2);

    cmd[0] = ACC_CONF;
    cmd[1] = acc_conf;
    write(_obj->fd, cmd, 2);

    cmd[0] = GYRO_CONF;
    cmd[1] = gyro_conf;
    write(_obj->fd, cmd, 2);

    return _obj;
}

static inline int16_t make_int16(uint8_t high, uint8_t low) {
    return high << 8 | low;
}

void imu_invensense_read(void* obj, imu_output* r) {
    _objt* _obj = (_objt*)obj;
    uint8_t cmd;
    uint8_t out[6];

    cmd = ACC_X;
    write(_obj->fd, &cmd, 1);
    read(_obj->fd, out, 6);

    r->acc.x = - (double)make_int16(out[0], out[1]) / _obj->acc_ssf;
    r->acc.y = (double)make_int16(out[2], out[3]) / _obj->acc_ssf;
    r->acc.z = (double)make_int16(out[4], out[5]) / _obj->acc_ssf;

    cmd = GYRO_X;
    write(_obj->fd, &cmd, 1);
    read(_obj->fd, out, 6);

    r->gyro.x = (double)make_int16(out[0], out[1]) / _obj->gyro_ssf;
    r->gyro.y = - (double)make_int16(out[2], out[3]) / _obj->gyro_ssf;
    r->gyro.z = (double)make_int16(out[4], out[5]) / _obj->gyro_ssf;
}
