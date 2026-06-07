#ifndef PARAMS_H
#define PARAMS_H

/* Sampling rate (Hz) */
#define AHRS_SAMPLE_RATE_HZ      100.0f

/* Mahony AHRS gains */
#define AHRS_KP                  0.5f    /* Proportional gain */
#define AHRS_KI                  0.01f   /* Integral gain (0 to disable) */

/* Gravity constant (m/s^2) */
#define GRAVITY_MSS              9.80665f

/* Free-fall detection */
#define FREEFALL_THRESHOLD_G     0.6f    /* |a| below this (in g) */
#define FREEFALL_DURATION_MS     100     /* Must persist this long */

/* Impact detection */
#define IMPACT_THRESHOLD_G       3.0f    /* |a| above this (in g) */

/* Post-impact posture check */
#define POST_IMPACT_WINDOW_MS    3000    /* Check tilt within N ms after impact */
#define POSTURE_TILT_THRESHOLD_DEG 60.0f /* Trunk tilt > this -> confirmed fall */

/* Tilt alarm */
#define TILT_ALARM_ANGLE_DEG     45.0f
#define TILT_ALARM_DURATION_MS   3000

/* Debounce: consecutive frames to confirm state transition */
#define DEBOUNCE_FRAMES          5

/* Gyroscope full scale (+/- dps), set to match your sensor config */
#define GYRO_FS_DPS              2000.0f

/* Use magnetometer for yaw correction (0 = disable, 1 = enable) */
#define AHRS_USE_MAG             0

#endif /* PARAMS_H */
