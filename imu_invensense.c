
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <math.h>

#include "vector.h"
#include "imu.h"
#include "imu_invensense.h"

// config
#define ACC_RANGE 2
#define GYRO_RANGE 250

#if ACC_RANGE == 2
    #define ACC_CONF_VALUE 0
    #define ACC_SSF 16384.0f
#elif ACC_RANGE == 4
    #define ACC_CONF_VALUE (1 << 3)
    #define ACC_SSF 8192.0f
#elif ACC_RANGE == 8
    #define ACC_CONF_VALUE (2 << 3)
    #define ACC_SSF 4096.0f
#elif ACC_RANGE == 16
    #define ACC_CONF_VALUE (3 << 3)
    #define ACC_SSF 2048.0f
#else
    #error "invalid range"
#endif

#if GYRO_RANGE == 250
    #define GYRO_CONF_VALUE 0
    #define GYRO_SSF (131.0f * 180.0f / M_PI)
#elif GYRO_RANGE == 500
    #define GYRO_CONF_VALUE (1 << 3)
    #define GYRO_SSF (65.5f * 180.0f / M_PI)
#elif GYRO_RANGE == 1000
    #define GYRO_CONF_VALUE (2 << 3)
    #define GYRO_SSF (32.8f * 180.0f / M_PI)
#elif GYRO_RANGE == 2000
    #define GYRO_CONF_VALUE (3 << 3)
    #define GYRO_SSF (16.4f * 180.0f / M_PI)
#else
    #error "invalid range"
#endif

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
} _objt;

void* imu_invensense_init(int fd) {
    _objt* _obj = malloc(sizeof(_objt));
    _obj->fd = fd;

    i2c_smbus_write_byte_data(_obj->fd, POWERMAN1, POWERMAN1_DISABLE_TEMP);
    i2c_smbus_write_byte_data(_obj->fd, ACC_CONF, ACC_CONF_VALUE);
    i2c_smbus_write_byte_data(_obj->fd, GYRO_CONF, GYRO_CONF_VALUE);

    return _obj;
}

static int16_t read_int16(_objt* _obj, int address) {
    return i2c_smbus_read_byte_data(_obj->fd, address) << 8 |
        i2c_smbus_read_byte_data(_obj->fd, address + 1);
}

void imu_invensense_read(void* obj, imu_output* r) {
    _objt* _obj = (_objt*)obj;
    r->acc.x = - (double)read_int16(_obj, ACC_X) / ACC_SSF;
    r->acc.y = (double)read_int16(_obj, ACC_Y) / ACC_SSF;
    r->acc.z = (double)read_int16(_obj, ACC_Z) / ACC_SSF;
    r->gyro.x = (double)read_int16(_obj, GYRO_X) / GYRO_SSF;
    r->gyro.y = - (double)read_int16(_obj, GYRO_Y) / GYRO_SSF;
    r->gyro.z = (double)read_int16(_obj, GYRO_Z) / GYRO_SSF;
}
