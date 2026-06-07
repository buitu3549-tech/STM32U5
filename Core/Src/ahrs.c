#include "ahrs.h"
#include "params.h"
#include <math.h>

/* Mahony filter state */
static float q[4];        /* quaternion [w, x, y, z] */
static float integral_fb[3]; /* integral error [x, y, z] */
static float kp, ki;

/* Reference vectors */
static const float grav_ref[3]  = {0.0f, 0.0f, 1.0f};   /* Up in NED */
static const float mag_ref[3]   = {1.0f, 0.0f, 0.0f};   /* North in NED */

static float inv_sqrt(float x) {
    return 1.0f / sqrtf(x);
}

void ahrs_init(void) {
    /* Initial quaternion: identity (no rotation) */
    q[0] = 1.0f;
    q[1] = 0.0f;
    q[2] = 0.0f;
    q[3] = 0.0f;

    integral_fb[0] = 0.0f;
    integral_fb[1] = 0.0f;
    integral_fb[2] = 0.0f;

    kp = AHRS_KP;
    ki = AHRS_KI;
}

void ahrs_set_gains(float _kp, float _ki) {
    kp = _kp;
    ki = _ki;
}

/* Normalize a 3-element vector in place. Returns true on success. */
static bool normalize3(float v[3]) {
    float norm = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (norm < 1e-10f) return false;
    float inv = 1.0f / norm;
    v[0] *= inv;
    v[1] *= inv;
    v[2] *= inv;
    return true;
}

/* Quaternion multiplication: out = a * b */
static void quat_mul(const float a[4], const float b[4], float out[4]) {
    out[0] = a[0]*b[0] - a[1]*b[1] - a[2]*b[2] - a[3]*b[3];
    out[1] = a[0]*b[1] + a[1]*b[0] + a[2]*b[3] - a[3]*b[2];
    out[2] = a[0]*b[2] - a[1]*b[3] + a[2]*b[0] + a[3]*b[1];
    out[3] = a[0]*b[3] + a[1]*b[2] - a[2]*b[1] + a[3]*b[0];
}

static void quat_unity(float q_out[4]) {
    q_out[0] = 1.0f; q_out[1] = 0.0f; q_out[2] = 0.0f; q_out[3] = 0.0f;
}

/* Normalize quaternion */
static void quat_normalize(float q_inout[4]) {
    float n = sqrtf(q_inout[0]*q_inout[0] + q_inout[1]*q_inout[1]
                  + q_inout[2]*q_inout[2] + q_inout[3]*q_inout[3]);
    if (n < 1e-10f) { quat_unity(q_inout); return; }
    float inv = 1.0f / n;
    q_inout[0] *= inv; q_inout[1] *= inv;
    q_inout[2] *= inv; q_inout[3] *= inv;
}

/* Rotate vector v by quaternion q (body-to-world) */
static void quat_rotate(const float q[4], const float v[3], float out[3]) {
    /* out = v + 2 * cross(q.xyz, cross(q.xyz, v) + q.w * v) */
    float qv[3] = {q[1], q[2], q[3]};
    float t[3];
    /* cross(q.xyz, v) */
    t[0] = qv[1]*v[2] - qv[2]*v[1];
    t[1] = qv[2]*v[0] - qv[0]*v[2];
    t[2] = qv[0]*v[1] - qv[1]*v[0];
    /* t += q.w * v */
    t[0] += q[0]*v[0]; t[1] += q[0]*v[1]; t[2] += q[0]*v[2];
    /* cross(q.xyz, t) */
    float u[3];
    u[0] = qv[1]*t[2] - qv[2]*t[1];
    u[1] = qv[2]*t[0] - qv[0]*t[2];
    u[2] = qv[0]*t[1] - qv[1]*t[0];
    out[0] = v[0] + 2.0f*u[0];
    out[1] = v[1] + 2.0f*u[1];
    out[2] = v[2] + 2.0f*u[2];
}

bool ahrs_update(const IMUData *data) {
    float dt = data->dt;
    if (dt <= 0.0f) return false;

    /* Normalize accelerometer measurement */
    float acc[3] = {data->accel[0], data->accel[1], data->accel[2]};
    float ax = acc[0], ay = acc[1], az = acc[2];
    if (!normalize3(acc)) return false;

    /* Rotate gravity reference into body frame using current quaternion */
    float grav_body[3];
    float q_conj[4] = {q[0], -q[1], -q[2], -q[3]};
    quat_rotate(q_conj, grav_ref, grav_body);

    /* Cross product: measured accel x predicted gravity (error in body frame) */
    float error[3];
    error[0] = acc[1]*grav_body[2] - acc[2]*grav_body[1];
    error[1] = acc[2]*grav_body[0] - acc[0]*grav_body[2];
    error[2] = acc[0]*grav_body[1] - acc[1]*grav_body[0];

    /* Integrate error (in body frame) */
    if (ki > 0.0f) {
        integral_fb[0] += error[0] * ki * dt;
        integral_fb[1] += error[1] * ki * dt;
        integral_fb[2] += error[2] * ki * dt;
    }

    /* Gyroscope with correction: g = gyro + kp*error + integral_error */
    float gx = data->gyro[0] + kp * error[0] + integral_fb[0];
    float gy = data->gyro[1] + kp * error[1] + integral_fb[1];
    float gz = data->gyro[2] + kp * error[2] + integral_fb[2];

#if AHRS_USE_MAG
    /* Magnetometer correction for yaw drift */
    float mag[3] = {data->mag[0], data->mag[1], data->mag[2]};
    if (normalize3(mag)) {
        /* Rotate mag into world frame */
        float mag_world[3];
        quat_rotate(q, mag, mag_world);
        /* Project to horizontal plane */
        float bx = sqrtf(mag_world[0]*mag_world[0] + mag_world[1]*mag_world[1]);
        float bz = mag_world[2];
        float mag_ref_hat[3] = {bx, 0.0f, bz};
        if (normalize3(mag_ref_hat)) {
            /* Rotate back to body frame */
            float mag_ref_body[3];
            quat_rotate(q_conj, mag_ref_hat, mag_ref_body);
            float m_error[3];
            m_error[0] = mag[1]*mag_ref_body[2] - mag[2]*mag_ref_body[1];
            m_error[1] = mag[2]*mag_ref_body[0] - mag[0]*mag_ref_body[2];
            m_error[2] = mag[0]*mag_ref_body[1] - mag[1]*mag_ref_body[0];
            gx += kp * m_error[0];
            gy += kp * m_error[1];
            gz += kp * m_error[2];
        }
    }
#endif

    /* Quaternion derivative: dq = 0.5 * q * omega_quat */
    float dq[4];
    dq[0] = 0.5f * (-q[1]*gx - q[2]*gy - q[3]*gz);
    dq[1] = 0.5f * ( q[0]*gx + q[2]*gz - q[3]*gy);
    dq[2] = 0.5f * ( q[0]*gy - q[1]*gz + q[3]*gx);
    dq[3] = 0.5f * ( q[0]*gz + q[1]*gy - q[2]*gx);

    q[0] += dq[0] * dt;
    q[1] += dq[1] * dt;
    q[2] += dq[2] * dt;
    q[3] += dq[3] * dt;

    quat_normalize(q);
    return true;
}

EulerAngles ahrs_get_euler(void) {
    EulerAngles e;
    float q0 = q[0], q1 = q[1], q2 = q[2], q3 = q[3];

    /* Roll (rotation around x-axis) */
    float sinr = 2.0f * (q0*q1 + q2*q3);
    float cosr = 1.0f - 2.0f * (q1*q1 + q2*q2);
    e.roll = atan2f(sinr, cosr);

    /* Pitch (rotation around y-axis) */
    float sinp = 2.0f * (q0*q2 - q3*q1);
    if (sinp > 1.0f)  sinp = 1.0f;
    if (sinp < -1.0f) sinp = -1.0f;
    e.pitch = asinf(sinp);

    /* Yaw (rotation around z-axis) */
    float siny = 2.0f * (q0*q3 + q1*q2);
    float cosy = 1.0f - 2.0f * (q2*q2 + q3*q3);
    e.yaw = atan2f(siny, cosy);

    /* Convert to degrees */
    e.roll  *= 57.295779513f;
    e.pitch *= 57.295779513f;
    e.yaw   *= 57.295779513f;

    return e;
}

void ahrs_get_quaternion(float q_out[4]) {
    q_out[0] = q[0];
    q_out[1] = q[1];
    q_out[2] = q[2];
    q_out[3] = q[3];
}

float ahrs_get_trunk_tilt(void) {
    EulerAngles e = ahrs_get_euler();
    /* Combined tilt: angle between body z-axis and world z-axis */
    float tilt = sqrtf(e.roll * e.roll + e.pitch * e.pitch);
    return tilt;
}
