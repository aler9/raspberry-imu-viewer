
#include "vector.h"
#include "imu.h"
#include "align_dcm.h"

#define SAMPLE_COUNT 500

void align_dcm_init(matrix* align) {
    // sample accelerometers and get K
    vector vK;
    imu_output io;
    for(int i = 0; i < SAMPLE_COUNT; i++) {
        imu_read(&io);
        vK.x += io.acc.x;
        vK.y += io.acc.y;
        vK.z += io.acc.z;
    }
    vK.x /= SAMPLE_COUNT;
    vK.y /= SAMPLE_COUNT;
    vK.z /= SAMPLE_COUNT;

    // normalize K
    vector_normalize(&vK);

    // assume IMU is pointing north
    vector vI;
    vI.x = 1;
    vI.y = 0;
    vI.z = 0;

    // get J
    vector vJ;
    vector_cross(&vK, &vI, &vJ);

    // get again I
    vector_cross(&vJ, &vK, &vI);

    // normalize I
    vector_normalize(&vI);

    // get again J
    vector_cross(&vK, &vI, &vJ);

    matrix_fill_rows(&vI, &vJ, &vK, align);
}
