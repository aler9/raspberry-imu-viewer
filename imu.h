
#pragma once

typedef struct {
    vector acc;
    vector gyro;
} imu_output;

void imu_init();
void imu_read(imu_output* r);
