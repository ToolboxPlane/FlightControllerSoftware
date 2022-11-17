/**
 * @file math.c
 * @author paul
 * @date 03.05.19
 * @brief math @TODO
 */

#include "math.h"

static const float SIN_LUT[91] = {
        0.00000000F, 0.01745241F, 0.03489950F, 0.05233596F, 0.06975647F, 0.08715574F, 0.10452846F, 0.12186934F,
        0.13917310F, 0.15643447F, 0.17364818F, 0.19080900F, 0.20791169F, 0.22495105F, 0.24192190F, 0.25881905F,
        0.27563736F, 0.29237170F, 0.30901699F, 0.32556815F, 0.34202014F, 0.35836795F, 0.37460659F, 0.39073113F,
        0.40673664F, 0.42261826F, 0.43837115F, 0.45399050F, 0.46947156F, 0.48480962F, 0.50000000F, 0.51503807F,
        0.52991926F, 0.54463904F, 0.55919290F, 0.57357644F, 0.58778525F, 0.60181502F, 0.61566148F, 0.62932039F,
        0.64278761F, 0.65605903F, 0.66913061F, 0.68199836F, 0.69465837F, 0.70710678F, 0.71933980F, 0.73135370F,
        0.74314483F, 0.75470958F, 0.76604444F, 0.77714596F, 0.78801075F, 0.79863551F, 0.80901699F, 0.81915204F,
        0.82903757F, 0.83867057F, 0.84804810F, 0.85716730F, 0.86602540F, 0.87461971F, 0.88294759F, 0.89100652F,
        0.89879405F, 0.90630779F, 0.91354546F, 0.92050485F, 0.92718385F, 0.93358043F, 0.93969262F, 0.94551858F,
        0.95105652F, 0.95630476F, 0.96126170F, 0.96592583F, 0.97029573F, 0.97437006F, 0.97814760F, 0.98162718F,
        0.98480775F, 0.98768834F, 0.99026807F, 0.99254615F, 0.99452190F, 0.99619470F, 0.99756405F, 0.99862953F,
        0.99939083F, 0.99984770F, 1.00000000F};

float sinus(int16_t x) {
    while (x < 0) {
        x += 360;
    }
    while (x > 360) {
        x -= 360;
    }

    if (x < 90) {
        return SIN_LUT[x];
    } else if (x < 180) {
        return SIN_LUT[180 - x];
    } else if (x < 270) {
        return -SIN_LUT[x - 180];
    } else {
        return -SIN_LUT[360 - x];
    }
}

float cosinus(int16_t x) {
    return sinus(x + 90);
}