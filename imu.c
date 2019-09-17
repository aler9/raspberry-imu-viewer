
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "vector.h"
#include "imu.h"
#include "imu_invensense.h"

typedef void (*sensor_read_func)(void*, imu_output*);

typedef const char imu_err;

static struct {
    void* sensor;
    sensor_read_func sensor_read;
} _obj;

static imu_err* find_sensor() {
    int fd = open("/dev/i2c-1", O_RDWR);
    if(fd < 0) {
        return "unable to open device /dev/i2c-1";
    }

    for(uint8_t address = 0x68; address <= 0x69; address++) {
        int res = ioctl(fd, I2C_SLAVE, address);
        if(res != 0) {
            continue;
        }

        uint8_t cmd = 0x75;
        write(fd, &cmd, 1);
        uint8_t who_am_i;
        read(fd, &who_am_i, 1);

        switch(who_am_i) {
        case 0x68:
            printf("found MPU6000/MPU6050 with address 0x%.2x\n", address);
            _obj.sensor = imu_invensense_init(fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj.sensor_read = imu_invensense_read;
            return NULL;

        case 0x70:
            printf("found MPU6500 with address 0x%.2x\n", address);
            _obj.sensor = imu_invensense_init(fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj.sensor_read = imu_invensense_read;
            return NULL;

        case 0x11:
            printf("found ICM20600 with address 0x%.2x\n", address);
            _obj.sensor = imu_invensense_init(fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj.sensor_read = imu_invensense_read;
            return NULL;

        case 0xAC:
            printf("found ICM20601 with address 0x%.2x\n", address);
            _obj.sensor = imu_invensense_init(fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj.sensor_read = imu_invensense_read;
            return NULL;

        case 0x12:
            printf("found ICM20602 with address 0x%.2x\n", address);
            _obj.sensor = imu_invensense_init(fd, INVENSENSE_ACC_2G, INVENSENSE_GYRO_250);
            _obj.sensor_read = imu_invensense_read;
            return NULL;
        }
    }

    close(fd);
    return "no IMU sensor found";
}

void imu_init() {
    memset(&_obj, 0, sizeof(_obj));

    imu_err* err = find_sensor();
    if(err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }
}

void imu_read(imu_output* r) {
    _obj.sensor_read(_obj.sensor, r);
}
