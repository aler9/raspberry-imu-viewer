
#include <stdio.h>

#include "vector.h"
#include "imu.h"
#include "gyro_bias.h"

#define GYRO_BIAS_SAMPLE_COUNT 500

void gyro_bias_init(vector* gyro_bias) {
    imu_output io;
    for(int i = 0; i < GYRO_BIAS_SAMPLE_COUNT; i++) {
        imu_read(&io);
        gyro_bias->x += io.gyro.x;
        gyro_bias->y += io.gyro.y;
        gyro_bias->z += io.gyro.z;
    }
    gyro_bias->x /= GYRO_BIAS_SAMPLE_COUNT;
    gyro_bias->y /= GYRO_BIAS_SAMPLE_COUNT;
    gyro_bias->z /= GYRO_BIAS_SAMPLE_COUNT;
    printf("gyro bias: %f, %f, %f\n", gyro_bias->x, gyro_bias->y, gyro_bias->z);
}
