#include "util.h"
#include "open_interface.h"

float degToRad(short deg){
    return (float) (deg * M_PI / 180.0);
}

short radToDeg(float rad){
    return (short) (rad * 180 / M_PI);
}

float fdegToRad(float deg){
    return (deg * M_PI / 180.0);
}
