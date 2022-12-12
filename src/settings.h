#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    float theta1 = 0;
    float theta2 = 0;
    float theta3 = 0;

    float targetX = 0;
    float targetY = 0;
    float targetZ = 0;

    float segLength1 = 0;
    float segLength2 = 0;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
