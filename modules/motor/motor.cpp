//=====[Libraries]=============================================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "motor.h"

//=====[Declaration of private defines]========================================

#define DUTY_MIN 0.020
#define DUTY_MAX 0.115
#define PERIOD 0.02
#define DELAY_TIME_MS 5000

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

PwmOut servo(PF_9);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void motorInit()
{
    servo.period(PERIOD);
    servo.write(DUTY_MIN);
}

void motorUpdate()
{
    motorOpen();
    delay(DELAY_TIME_MS);
    motorClose();
}

//=====[Implementations of private functions]==================================

static void motorOpen()
{
    servo.write(DUTY_MAX);
}

static void motorClose()
{
    servo.write(DUTY_MIN);
}
