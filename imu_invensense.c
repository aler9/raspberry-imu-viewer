
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <stdint.h>

#include "vector.h"
#include "imu.h"
#include "imu_invensense.h"

#define POWERMAN1 0x6B
#define POWERMAN1_DISABLE_TEMP (1 << 3)
#define ACC_CONF 0x1C
#define ACC_X 0x3B
#define ACC_Y 0x3D
#define ACC_Z 0x3F
#define GYRO_CONF 0x1B
#define GYRO_X 0x43
#define GYRO_Y 0x45
#define GYRO_Z 0x47

typedef struct {
    int fd;
    double gyro_ssf;
    double acc_ssf;
} _objt;

void* imu_invensense_init(int fd, invensense_acc_range acc_range,
    invensense_gyro_range gyro_range) {
    _objt* _obj = malloc(sizeof(_objt));
    _obj->fd = fd;

    uint8_t acc_conf;

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

    uint8_t gyro_conf;

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

    i2c_smbus_write_byte_data(_obj->fd, POWERMAN1, POWERMAN1_DISABLE_TEMP);
    i2c_smbus_write_byte_data(_obj->fd, ACC_CONF, acc_conf);
    i2c_smbus_write_byte_data(_obj->fd, GYRO_CONF, gyro_conf);

    return _obj;
}

static int16_t read_int16(_objt* _obj, int address) {
    return i2c_smbus_read_byte_data(_obj->fd, address) << 8 |
        i2c_smbus_read_byte_data(_obj->fd, address + 1);
}

void imu_invensense_read(void* obj, imu_output* r) {
    _objt* _obj = (_objt*)obj;
    r->acc.x = - (double)read_int16(_obj, ACC_X) / _obj->acc_ssf;
    r->acc.y = (double)read_int16(_obj, ACC_Y) / _obj->acc_ssf;
    r->acc.z = (double)read_int16(_obj, ACC_Z) / _obj->acc_ssf;
    r->gyro.x = (double)read_int16(_obj, GYRO_X) / _obj->gyro_ssf;
    r->gyro.y = - (double)read_int16(_obj, GYRO_Y) / _obj->gyro_ssf;
    r->gyro.z = (double)read_int16(_obj, GYRO_Z) / _obj->gyro_ssf;
}
