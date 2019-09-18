
#pragma once

typedef struct {
    struct {
        double x;
        double y;
        double z;
    } acc;
    struct {
        double x;
        double y;
        double z;
    } gyro;
} imu_output;

typedef void imut;

error* imu_init(imut** pobj, int i2c_fd);
error* imu_read(imut* obj, imu_output* out);
