#include "remote.h"

#include <Drivers/sbus.h>

/*
 * (x-172)/(1811-172)*1000
 *
 * 1000/(1811-172) = 1000/1639 \approx 1000/1640 = 25/41
 */
#define NORMALIZE_TARANIS(x) ((uint16_t) (((x) -172) * 25 / 41))

void remote_init(void) {
    sbus_init();
}

remote_data_t remote_get_data(void) {
    sbus_data_t sbus_data = sbus_get_latest_data();
    remote_data_t remote_data = {.throttle_mixed = NORMALIZE_TARANIS(sbus_data.channel[0]),
                                 .elevon_left_mixed = NORMALIZE_TARANIS(sbus_data.channel[1]),
                                 .elevon_right_mixed = NORMALIZE_TARANIS(sbus_data.channel[2]),
                                 .throttle_raw = NORMALIZE_TARANIS(sbus_data.channel[3]),
                                 .pitch_raw = NORMALIZE_TARANIS(sbus_data.channel[4]),
                                 .roll_raw = NORMALIZE_TARANIS(sbus_data.channel[5]),
                                 .is_armed = NORMALIZE_TARANIS(sbus_data.channel[6]) < 400,
                                 .override_active = NORMALIZE_TARANIS((sbus_data.channel[7])) > 600,
                                 .remote_ok = (!sbus_data.failsave && !sbus_data.frame_lost)};
    return remote_data;
}

bool remote_data_available(void) {
    return sbus_data_available();
}