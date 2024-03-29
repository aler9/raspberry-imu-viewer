
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "sensor-imu/imu.h"
#include "sensor-imu/orientation/align_dcm.h"
#include "sensor-imu/orientation/est.h"
#include "sensor-imu/orientation/est_dcm_compl.h"
#include "sensor-imu/orientation/est_euler_acc.h"
#include "sensor-imu/orientation/est_euler_compl.h"
#include "sensor-imu/orientation/est_euler_gyro.h"
#include "sensor-imu/orientation/est_euler_gyrounalign.h"
#include "sensor-imu/orientation/gyro_bias.h"
#include "sensor-imu/orientation/vector.h"

#include "viewer.h"

static uint32_t clock_usec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

static error *run() {
    // disable stdout buffering
    setbuf(stdout, NULL);

    imut *imu;
    error *err =
        imu_init(&imu, "/dev/i2c-1", IMU_ACC_RANGE_2G, IMU_GYRO_RANGE_250DPS);
    if (err != NULL) {
        return err;
    }

    matrix align_dcm;
    err = align_dcm_init(&align_dcm, imu);
    if (err != NULL) {
        imu_destroy(imu);
        return err;
    }

    vector gyro_bias;
    err = gyro_bias_init(&gyro_bias, imu);
    if (err != NULL) {
        imu_destroy(imu);
        return err;
    }

    printf("gyro bias: %f, %f, %f\n", gyro_bias.x, gyro_bias.y, gyro_bias.z);

    est_euler_acct *est_euler_acc;
    err = est_euler_acc_init(&est_euler_acc, &align_dcm,
                             EST_EULER_ACC_DEFAULT_ALPHA);
    if (err != NULL) {
        imu_destroy(imu);
        return err;
    }

    est_euler_gyrot *est_euler_gyro;
    err = est_euler_gyro_init(&est_euler_gyro, &align_dcm, &gyro_bias);
    if (err != NULL) {
        imu_destroy(imu);
        return err;
    }

    est_euler_gyrounalignt *est_euler_gyro_unalign;
    err = est_euler_gyrounalign_init(&est_euler_gyro_unalign, &gyro_bias);
    if (err != NULL) {
        imu_destroy(imu);
        return err;
    }

    est_euler_complt *est_euler_compl;
    err = est_euler_compl_init(&est_euler_compl, &align_dcm, &gyro_bias,
                               EST_EULER_COMPL_DEFAULT_ALPHA);
    if (err != NULL) {
        imu_destroy(imu);
        return err;
    }

    est_dcm_complt *est_dcm_compl;
    err = est_dcm_compl_init(&est_dcm_compl, &align_dcm, &gyro_bias,
                             EST_DCM_COMPL_DEFAULT_ALPHA);
    if (err != NULL) {
        imu_destroy(imu);
        return err;
    }

    viewert *viewer;
    err = viewer_init(&viewer);
    if (err != NULL) {
        imu_destroy(imu);
        return err;
    }

    imu_output io;
    uint32_t read_count = 0;
    uint32_t last_report = clock_usec();
    uint32_t prev = last_report;
    estimator_output eo;

    while (1) {
        err = imu_read(imu, &io);
        if (err != NULL) {
            imu_destroy(imu);
            return err;
        }
        read_count++;

        uint32_t now = clock_usec();
        double dt = (double)(now - prev) / 1000000;
        prev = now;

        viewer_draw_start(viewer);

        est_euler_acc_do(est_euler_acc, io.acc_array, &eo);
        viewer_draw_estimate(viewer, 0, &eo);

        est_euler_gyro_do(est_euler_gyro, io.gyro_array, dt, &eo);
        viewer_draw_estimate(viewer, 1, &eo);

        est_euler_gyrounalign_do(est_euler_gyro_unalign, io.gyro_array, dt,
                                 &eo);
        viewer_draw_estimate(viewer, 2, &eo);

        est_euler_compl_do(est_euler_compl, io.acc_array, io.gyro_array, dt,
                           &eo);
        viewer_draw_estimate(viewer, 3, &eo);

        est_dcm_compl_do(est_dcm_compl, io.acc_array, io.gyro_array, dt, &eo);
        viewer_draw_estimate(viewer, 4, &eo);

        viewer_draw_end(viewer);

        if ((now - last_report) >= 1000000) {
            printf("read per sec: %d\n", read_count);
            printf("gyro x,y,z: %f, %f, %f\n", io.gyro.x, io.gyro.y, io.gyro.z);
            printf("acc x,y,z: %f, %f, %f\n", io.acc.x, io.acc.y, io.acc.z);
            printf("\n");
            last_report = now;
            read_count = 0;
        }
    }

    return NULL;
}

int main() {
    error *err = run();
    if (err != NULL) {
        printf("ERR: %s\n", err);
        exit(1);
    }
    return 0;
}
