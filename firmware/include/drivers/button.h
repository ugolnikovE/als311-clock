#ifndef BUTTON_H
#define BUTTON_H
#include <stdint.h>

/* Gesture reported by the driver, consumed one at a time via btn_get_event */
typedef enum {
        BTN_EVENT_NONE,         /* no pending event */
        BTN_EVENT_CLICK,        /* pressed and released before long threshold */
        BTN_EVENT_LONG,         /* held past threshold, fires once */
        BTN_EVENT_REPEAT        /* still held, repeats at a fixed interval */
} btn_event_type_t;

/* Which button. Order MUST match btns[] in button.c: name == array index */
typedef enum {
        BTN_1,
        BTN_2,
        BTN_MODE
} btn_name_t;

typedef struct {
        btn_name_t       name;  /* source button */
        btn_event_type_t type;  /* what happened */
} btn_event_t;

/* Register the debounce/gesture callback on the systimer. Call once at startup */
void btn_init(void);

/* Pop the next pending event into e. Non-blocking, atomic.
 * Scans buttons in order and returns the first pending event.
 * Sets e->type to BTN_EVENT_NONE if no button has an event */
void btn_get_event(btn_event_t* e);

#endif // BUTTON_H