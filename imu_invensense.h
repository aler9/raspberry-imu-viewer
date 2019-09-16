
#pragma once

void* imu_invensense_init(int fd);
void imu_invensense_read(void* obj, imu_output* r);
