#include "motor.h"

void drive_setLR(drive_t *drive, int16_t l, int16_t r) {
  motor_set(drive->left,l);
  motor_set(drive->right,r);
}

void drive_set(drive_t *drive, int16_t velocity, int16_t turn) {
  int16_t l,r;
  
  l = velocity + turn;
  r = velocity - turn;

  int16_t largest = max(abs(l),abs(r));
  if (largest > MOTOR_SPEED_MAX) {
    // a*(c/d) = (a*c)/d
    l = ((int32_t)l * MOTOR_SPEED_MAX) / largest;
    r = ((int32_t)r * MOTOR_SPEED_MAX) / largest;
  }
  
  setLR(l,r);
}
