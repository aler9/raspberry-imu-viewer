
# raspberry-imu-viewer

![](front.jpg)

[![Lint](https://github.com/aler9/raspberry-imu-viewer/workflows/lint/badge.svg)](https://github.com/aler9/raspberry-imu-viewer/actions?query=workflow:lint)
[![Test](https://github.com/aler9/raspberry-imu-viewer/workflows/test/badge.svg)](https://github.com/aler9/raspberry-imu-viewer/actions?query=workflow:test)

This tool allows to view in 3D the output of various orientation estimation algorithms, fed by a IMU sensor, connected to a Raspberry Pi. Supported sensors are:

* MPU6000 (I2C mode)
* MPU6050 (I2C mode)
* MPU6500 (I2C mode)
* ICM20600 (I2C mode)
* ICM20601 (I2C mode)
* ICM20602 (I2C mode)

This tool is intended to:

* provide a zero-dependencies, ready-to-use tool to test IMUs
* provide a starting point to anyone wishing to explore the field of orientation estimation

## Build & launch

1. On the Raspberry Pi, enable I2C in fast-speed mode: edit `/boot/config.txt` and add:

   ```
   dtparam=i2c_arm=on
   dtparam=i2c_arm_baudrate=400000
   ```

   then edit `/etc/modules` and add

   ```
   i2c-dev
   ```

   then reboot the system.

2. Install the dependencies:

   ```
   sudo apt install -y git make gcc libc6-dev libi2c-dev libraspberrypi-dev
   ```

3. Clone this repository:

   ```
   git clone https://github.com/aler9/raspberry-imu-viewer
   cd raspberry-imu-viewer
   ```

4. Build:

   ```
   make
   ```

5. Launch:

   ```
   ./raspberry-imu-viewer
   ```

## Links

* https://github.com/aler9/sensor-imu (IMU library used by this project)
