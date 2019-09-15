
#include <math.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>

#include "vector.h"
#include "imu.h"

// config
#define DEVICE "/dev/i2c-1"
#define ADDRESS 0x68
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

#define WHO_AM_I 0x75
#define POWER1 0x6b
#define POWER2 0x6c
#define GYRO_CONF 0x1b
#define ACC_CONF 0x1c
#define BIT_RESET (1 << 7)
#define BIT_DISABLE_TEMP (1 << 3)
#define GYRO_X 0x43
#define GYRO_Y 0x45
#define GYRO_Z 0x47
#define ACC_X 0x3b
#define ACC_Y 0x3d
#define ACC_Z 0x3f

static struct {
    int fd;
} _obj;

static int16_t read_int16(int address) {
    return i2c_smbus_read_byte_data(_obj.fd, address) << 8 |
        i2c_smbus_read_byte_data(_obj.fd, address + 1);
}

void imu_init() {
    memset(&_obj, 0, sizeof(_obj));

    _obj.fd = open(DEVICE, O_RDWR);
    assert(_obj.fd >= 0);

    int res = ioctl(_obj.fd, I2C_SLAVE, ADDRESS);
    assert(res >= 0);

    uint8_t who_am_i = i2c_smbus_read_byte_data(_obj.fd, WHO_AM_I);
    assert(who_am_i == 0x68);

    i2c_smbus_write_byte_data(_obj.fd, POWER1, BIT_DISABLE_TEMP);
    i2c_smbus_write_byte_data(_obj.fd, ACC_CONF, ACC_CONF_VALUE);
    i2c_smbus_write_byte_data(_obj.fd, GYRO_CONF, GYRO_CONF_VALUE);
}

void imu_read(imu_output* r) {
    r->acc.x = - (double)read_int16(ACC_X) / ACC_SSF;
    r->acc.y = (double)read_int16(ACC_Y) / ACC_SSF;
    r->acc.z = (double)read_int16(ACC_Z) / ACC_SSF;
    r->gyro.x = (double)read_int16(GYRO_X) / GYRO_SSF;
    r->gyro.y = - (double)read_int16(GYRO_Y) / GYRO_SSF;
    r->gyro.z = (double)read_int16(GYRO_Z) / GYRO_SSF;
}
