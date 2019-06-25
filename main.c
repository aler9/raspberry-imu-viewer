
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#include "vector.h"
#include "imu.h"
#include "est.h"
#include "viewer.h"
#include "est_euler_acc.h"
#include "est_euler_gyro.h"
#include "est_euler_gyrounalign.h"
#include "est_euler_compl.h"
#include "est_dcm_compl.h"

inline uint32_t clock_usec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

int main() {
    // disable stdout buffering
    setbuf(stdout, NULL);

    imu_init();
    visualizer_init();
    est_euler_acc_init();
    est_euler_gyro_init();
    est_euler_gyrounalign_init();
    est_euler_compl_init();
    est_dcm_compl_init();

    imu_output io;
    uint32_t read_count = 0;
    uint32_t last_report = clock_usec();
    uint32_t prev = last_report;
    estimator_output eo;

    while(1) {
        imu_read(&io);
        read_count++;

        uint32_t now = clock_usec();
        double dt = (double)(now - prev) / 1000000;
        prev = now;

        visualizer_draw_start();

        est_euler_acc_do(&io, dt, &eo);
        visualizer_draw_estimate(0, &eo);

        est_euler_gyro_do(&io, dt, &eo);
        visualizer_draw_estimate(1, &eo);

        est_euler_gyrounalign_do(&io, dt, &eo);
        visualizer_draw_estimate(2, &eo);

        est_euler_compl_do(&io, dt, &eo);
        visualizer_draw_estimate(3, &eo);

        est_dcm_compl_do(&io, dt, &eo);
        visualizer_draw_estimate(4, &eo);

        visualizer_draw_end();

        if((now - last_report) >= 1000000) {
            printf("read per sec: %d\n", read_count);
            printf("gyro x,y,z: %f, %f, %f\n", io.gyro.x, io.gyro.y, io.gyro.z);
            printf("acc x,y,z: %f, %f, %f\n", io.acc.x, io.acc.y, io.acc.z);
            printf("\n");
            last_report = now;
            read_count = 0;
        }
    }

    return 0;
}
