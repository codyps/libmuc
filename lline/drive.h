#include <stdint.h>
#include "motor.h"

typedef struct drive_s {
  motor_t *left;
  motor_t *right;
} drive_t;

#define DRIVE_DEF(left_motor,right_motor)\
  {.left=&left_motor,.right=&right_motor}

void drive_setLR(drive_t *drive, int16_t l, int16_t r);
void drive_set(drive_t *drive, int16_t velocity, int16_t turn);
