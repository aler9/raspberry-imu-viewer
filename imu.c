
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
#define IMU_ADDRESS 0x68
#define IMU_ACC_RANGE 2
#define IMU_GYRO_RANGE 250

#if IMU_ACC_RANGE == 2
    #define IMU_ACC_CONFIG_VALUE 0
    #define IMU_ACC_SSF 16384.0f
#elif IMU_ACC_RANGE == 4
    #define IMU_ACC_CONFIG_VALUE (1 << 3)
    #define IMU_ACC_SSF 8192.0f
#elif IMU_ACC_RANGE == 8
    #define IMU_ACC_CONFIG_VALUE (2 << 3)
    #define IMU_ACC_SSF 4096.0f
#elif IMU_ACC_RANGE == 16
    #define IMU_ACC_CONFIG_VALUE (3 << 3)
    #define IMU_ACC_SSF 2048.0f
#else
    #error "invalid range"
#endif

#if IMU_GYRO_RANGE == 250
    #define IMU_GYRO_CONFIG_VALUE 0
    #define IMU_GYRO_SSF (131.0f * 180.0f / M_PI)
#elif IMU_GYRO_RANGE == 500
    #define IMU_GYRO_CONFIG_VALUE (1 << 3)
    #define IMU_GYRO_SSF (65.5f * 180.0f / M_PI)
#elif IMU_GYRO_RANGE == 1000
    #define IMU_GYRO_CONFIG_VALUE (2 << 3)
    #define IMU_GYRO_SSF (32.8f * 180.0f / M_PI)
#elif IMU_GYRO_RANGE == 2000
    #define IMU_GYRO_CONFIG_VALUE (3 << 3)
    #define IMU_GYRO_SSF (16.4f * 180.0f / M_PI)
#else
    #error "invalid range"
#endif

#define IMU_WHO_AM_I 0x75
#define IMU_POWER1 0x6b
#define IMU_POWER2 0x6c
#define IMU_GYRO_CONFIG 0x1b
#define IMU_ACC_CONFIG 0x1c
#define IMU_BIT_RESET (1 << 7)
#define IMU_BIT_DISABLE_TEMP (1 << 3)
#define IMU_TEMP 0x41
#define IMU_GYROX 0x43
#define IMU_GYROY 0x45
#define IMU_GYROZ 0x47
#define IMU_ACCX 0x3b
#define IMU_ACCY 0x3d
#define IMU_ACCZ 0x3f

typedef struct {
    int fd;
} _imu;

static _imu _obj;

static int16_t _imu_read_int16(int address) {
    return i2c_smbus_read_byte_data(_obj.fd, address) << 8 |
        i2c_smbus_read_byte_data(_obj.fd, address + 1);
}

void imu_init() {
    memset(&_obj, 0, sizeof(_imu));

    _obj.fd = open("/dev/i2c-1", O_RDWR);
    assert(_obj.fd >= 0);

    int res = ioctl(_obj.fd, I2C_SLAVE, IMU_ADDRESS);
    assert(res >= 0);

    uint8_t who_am_i = i2c_smbus_read_byte_data(_obj.fd, IMU_WHO_AM_I);
    assert(who_am_i == 0x68);

    i2c_smbus_write_byte_data(_obj.fd, IMU_POWER1, IMU_BIT_DISABLE_TEMP);
    i2c_smbus_write_byte_data(_obj.fd, IMU_ACC_CONFIG, IMU_ACC_CONFIG_VALUE);
    i2c_smbus_write_byte_data(_obj.fd, IMU_GYRO_CONFIG, IMU_GYRO_CONFIG_VALUE);
}

void imu_read(imu_output* r) {
    r->acc.x = - (double)_imu_read_int16(IMU_ACCX) / IMU_ACC_SSF;
    r->acc.y = (double)_imu_read_int16(IMU_ACCY) / IMU_ACC_SSF;
    r->acc.z = (double)_imu_read_int16(IMU_ACCZ) / IMU_ACC_SSF;
    r->gyro.x = (double)_imu_read_int16(IMU_GYROX) / IMU_GYRO_SSF;
    r->gyro.y = - (double)_imu_read_int16(IMU_GYROY) / IMU_GYRO_SSF;
    r->gyro.z = (double)_imu_read_int16(IMU_GYROZ) / IMU_GYRO_SSF;
}
