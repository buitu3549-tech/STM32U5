#ifndef AHRS_H
#define AHRS_H

#include <stdbool.h>

/* Euler angles in degrees */
typedef struct {
    float roll;
    float pitch;
    float yaw;
} EulerAngles;

/* Sensor data input (SI units) */
typedef struct {
    float accel[3];   /* m/s^2 */
    float gyro[3];    /* rad/s */
    float mag[3];     /* uT (optional, only if AHRS_USE_MAG=1) */
    float dt;         /* seconds since last update */
} IMUData;

void ahrs_init(void);
void ahrs_set_gains(float kp, float ki);
bool ahrs_update(const IMUData *data);
EulerAngles ahrs_get_euler(void);
void ahrs_get_quaternion(float q[4]);
float ahrs_get_trunk_tilt(void);

#endif /* AHRS_H */
