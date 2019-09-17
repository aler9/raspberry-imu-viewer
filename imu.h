
#pragma once

typedef void imut;

error* imu_init(imut** pobj, int i2c_fd);
void imu_read(imut* obj, imu_output* r);
