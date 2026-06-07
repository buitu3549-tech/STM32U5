#include "fall_detect.h"
#include "params.h"
#include <string.h>

/* --- threshold parameters (runtime tunable) --- */
static float freefall_thresh_g      = FREEFALL_THRESHOLD_G;
static float impact_thresh_g        = IMPACT_THRESHOLD_G;
static float tilt_alarm_angle_deg   = TILT_ALARM_ANGLE_DEG;
static float posture_tilt_thresh_deg = POSTURE_TILT_THRESHOLD_DEG;

static uint32_t freefall_dur_ms     = FREEFALL_DURATION_MS;
static uint32_t tilt_alarm_dur_ms   = TILT_ALARM_DURATION_MS;
static uint32_t post_impact_window_ms = POST_IMPACT_WINDOW_MS;

static uint8_t debounce_frames = DEBOUNCE_FRAMES;

/* --- state machine internals --- */
static DetectState state;

static uint32_t freefall_start_ms;
static uint32_t impact_timestamp_ms;
static uint32_t tilt_start_ms;

static uint8_t state_vote_count;    /* consecutive votes for next state */
static DetectState pending_state;   /* the state we might transition to */

/* Timestamps from the last sample */
static uint32_t now_ms;

/* --- helpers --- */
static void reset_timers(void) {
    freefall_start_ms = 0;
    impact_timestamp_ms = 0;
    tilt_start_ms = 0;
}

static void vote_for_state(DetectState next) {
    if (pending_state == next) {
        state_vote_count++;
    } else {
        pending_state = next;
        state_vote_count = 1;
    }
}

static bool vote_passed(void) {
    return state_vote_count >= debounce_frames;
}

static void set_state(DetectState s) {
    state = s;
    state_vote_count = 0;
    pending_state = state;
}

/* --- public --- */
void detect_init(void) {
    state = STATE_NORMAL;
    pending_state = STATE_NORMAL;
    state_vote_count = 0;
    reset_timers();
}

void detect_set_freefall_threshold(float thresh_g, uint32_t duration_ms) {
    freefall_thresh_g = thresh_g;
    freefall_dur_ms = duration_ms;
}

void detect_set_impact_threshold(float thresh_g) {
    impact_thresh_g = thresh_g;
}

void detect_set_tilt_threshold(float angle_deg, uint32_t duration_ms) {
    tilt_alarm_angle_deg = angle_deg;
    tilt_alarm_dur_ms = duration_ms;
}

void detect_set_post_impact_window(uint32_t window_ms, float tilt_threshold_deg) {
    post_impact_window_ms = window_ms;
    posture_tilt_thresh_deg = tilt_threshold_deg;
}

void detect_set_debounce_frames(uint8_t n) {
    debounce_frames = n;
}

DetectOutput detect_update(const DetectInput *input) {
    now_ms = input->timestamp_ms;
    float amag = input->accel_mag;           /* in g */
    float tilt = input->trunk_tilt_deg;      /* in degrees */

    DetectState next = state;

    switch (state) {

    case STATE_NORMAL:
        /* Free-fall pre-detection: |a| < freefall threshold */
        if (amag < freefall_thresh_g) {
            if (freefall_start_ms == 0) {
                freefall_start_ms = now_ms;
            }
            if ((now_ms - freefall_start_ms) >= freefall_dur_ms) {
                vote_for_state(STATE_FREEFALL);
                if (vote_passed()) next = STATE_FREEFALL;
            }
        } else {
            freefall_start_ms = 0;
            pending_state = STATE_NORMAL;
            state_vote_count = 0;
        }

        /* Immediate impact (bypass freefall if very high spike) */
        if (amag > impact_thresh_g) {
            vote_for_state(STATE_IMPACT);
            if (vote_passed()) next = STATE_IMPACT;
        }
        break;

    case STATE_FREEFALL:
        /* Impact after freefall: |a| > impact threshold */
        if (amag > impact_thresh_g) {
            vote_for_state(STATE_IMPACT);
            if (vote_passed()) next = STATE_IMPACT;
        }
        /* Timeout: return to normal if no impact arrives */
        if ((now_ms - freefall_start_ms) > (freefall_dur_ms + 2000)) {
            vote_for_state(STATE_NORMAL);
            if (vote_passed()) next = STATE_NORMAL;
        }
        break;

    case STATE_IMPACT:
        impact_timestamp_ms = now_ms;
        next = STATE_POST_IMPACT;  /* immediate transition, no debounce needed */
        break;

    case STATE_POST_IMPACT:
        /* Check posture within post-impact window */
        if ((now_ms - impact_timestamp_ms) <= post_impact_window_ms) {
            if (tilt > posture_tilt_thresh_deg) {
                vote_for_state(STATE_FALL_CONFIRMED);
                if (vote_passed()) next = STATE_FALL_CONFIRMED;
            }
        } else {
            /* Window expired without confirming fall -> normal */
            vote_for_state(STATE_NORMAL);
            pending_state = STATE_NORMAL;
            /* Immediate transition after window expires */
            next = STATE_NORMAL;
        }
        break;

    case STATE_FALL_CONFIRMED:
        /* Stay in fall state until explicit reset */
        next = STATE_FALL_CONFIRMED;
        break;
    }

    if (next != state && next != STATE_FALL_CONFIRMED) {
        set_state(next);
    } else if (next == STATE_FALL_CONFIRMED) {
        set_state(next);
    }

    /* Tilt alarm check (independent of fall detection, active only in normal state) */
    if (state == STATE_NORMAL && tilt > tilt_alarm_angle_deg) {
        if (tilt_start_ms == 0) {
            tilt_start_ms = now_ms;
        }
        if ((now_ms - tilt_start_ms) >= tilt_alarm_dur_ms) {
            /* Reset fall state timers — tilt warning takes priority over fall chain */
            return DETECT_TILT_WARNING;
        }
    } else if (tilt <= tilt_alarm_angle_deg) {
        tilt_start_ms = 0;
    }

    /* Output mapping */
    if (state == STATE_FALL_CONFIRMED) return DETECT_FALL_ALARM;
    return DETECT_NORMAL;
}

DetectState detect_get_state(void) {
    return state;
}

const char *detect_state_name(DetectState s) {
    switch (s) {
        case STATE_NORMAL:        return "NORMAL";
        case STATE_FREEFALL:      return "FREE_FALL";
        case STATE_IMPACT:        return "IMPACT";
        case STATE_POST_IMPACT:   return "POST_IMPACT";
        case STATE_FALL_CONFIRMED: return "FALL_CONFIRMED";
        default:                  return "UNKNOWN";
    }
}
