
#pragma once

typedef void imut;

error* imu_init(imut** pobj);
void imu_read(imut* obj, imu_output* r);
