#ifndef FALL_DETECT_H
#define FALL_DETECT_H

#include <stdint.h>

typedef enum {
    DETECT_NORMAL = 0,
    DETECT_TILT_WARNING,
    DETECT_FALL_ALARM
} DetectOutput;

typedef enum {
    STATE_NORMAL = 0,
    STATE_FREEFALL,
    STATE_IMPACT,
    STATE_POST_IMPACT,
    STATE_FALL_CONFIRMED
} DetectState;

typedef struct {
    float accel_mag;       /* Current |a| in g */
    float trunk_tilt_deg;  /* Combined roll+pitch tilt degrees */
    uint32_t timestamp_ms; /* Sample timestamp */
} DetectInput;

/* Initialise state machine */
void detect_init(void);

/* Override threshold parameters at runtime */
void detect_set_freefall_threshold(float thresh_g, uint32_t duration_ms);
void detect_set_impact_threshold(float thresh_g);
void detect_set_tilt_threshold(float angle_deg, uint32_t duration_ms);
void detect_set_post_impact_window(uint32_t window_ms, float tilt_threshold_deg);
void detect_set_debounce_frames(uint8_t n);

/* Feed one sample; returns current detection output */
DetectOutput detect_update(const DetectInput *input);

/* Query internal state (for debug) */
DetectState detect_get_state(void);
const char *detect_state_name(DetectState s);

#endif /* FALL_DETECT_H */
