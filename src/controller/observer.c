#include "model/model.h"
#include "watcher.h"
#include "observer.h"
#include "bsp/relay.h"
#include "bsp/pwm.h"
#include "services/timestamp.h"


static void update_relays(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg);
static void update_pwm(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg);
static void refresh_relays(uint16_t map);


static watcher_t watcher = {0};
static struct {
    uint8_t  pwm_percentage;
    uint16_t relays;
} state = {0};


void observer_init(mut_model_t *model) {
    WATCHER_INIT_STD(&watcher, model);

    WATCHER_ADD_ENTRY(&watcher, &state.pwm_percentage, update_pwm, NULL);
    WATCHER_ADD_ENTRY(&watcher, &state.relays, update_relays, NULL);
}


void observer_manage(model_t *model) {
    state.pwm_percentage = model_get_pwm_percentage(model);
    state.relays         = model_get_relay_map(model);

    watcher_watch(&watcher, timestamp_get());
}


static void update_relays(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg) {
    (void)old_value;
    (void)new_value;
    (void)size;
    (void)user_ptr;
    (void)arg;

    refresh_relays(state.relays);
}


static void update_pwm(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg) {
    (void)old_value;
    (void)new_value;
    (void)size;
    (void)user_ptr;
    (void)arg;

    bsp_pwm_update((uint16_t)state.pwm_percentage * 10);
}


static void refresh_relays(uint16_t map) {
    for (bsp_relay_t relay = BSP_RELAY_1; relay < BSP_RELAY_NUM; relay++) {
        bsp_relay_update(relay, (map & (1 << relay)) > 0);
    }
}
