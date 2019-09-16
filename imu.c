
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

static struct {
    void* sensor;
    sensor_read_func sensor_read;
} _obj;

static int search_sensor(int fd, uint8_t address, uint8_t registry, uint8_t expected_val) {
    int res = ioctl(fd, I2C_SLAVE, address);
    assert(res >= 0);

    uint8_t val = i2c_smbus_read_byte_data(fd, registry);
    return val == expected_val;
}

void imu_init() {
    memset(&_obj, 0, sizeof(_obj));

    int fd = open("/dev/i2c-1", O_RDWR);
    assert(fd >= 0);

    if(search_sensor(fd, 0x68, 0x75, 0x68)) {
        printf("found mpu6000/mpu6050 with address 0x68\n");
        _obj.sensor = imu_invensense_init(fd);
        _obj.sensor_read = imu_invensense_read;

    } else if(search_sensor(fd, 0x69, 0x75, 0x68)) {
        printf("found mpu6000/mpu6050 with address 0x69\n");
        _obj.sensor = imu_invensense_init(fd);
        _obj.sensor_read = imu_invensense_read;

    } else if(search_sensor(fd, 0x68, 0x75, 0x11)) {
        printf("found icm20600 with address 0x68\n");
        _obj.sensor = imu_invensense_init(fd);
        _obj.sensor_read = imu_invensense_read;

    } else if(search_sensor(fd, 0x69, 0x75, 0x11)) {
        printf("found icm20600 with address 0x69\n");
        _obj.sensor = imu_invensense_init(fd);
        _obj.sensor_read = imu_invensense_read;

    } else {
        close(fd);
        printf("no IMU found.\n");
        exit(1);
    }
}

void imu_read(imu_output* r) {
    _obj.sensor_read(_obj.sensor, r);
}
