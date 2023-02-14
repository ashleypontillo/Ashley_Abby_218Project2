//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"

#include "code.h"
#include "siren.h"
#include "smart_home_system.h"
#include "fire_alarm.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"
#include "display.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_MS 1000

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static bool incorrectCodeState = OFF;
static bool systemBlockedState = OFF;

static bool codeComplete = false;
static int numberOfCodeChars = 0;

static int numberTries = 0;

//=====[Declarations (prototypes) of private functions]========================

static void userInterfaceMatrixKeypadUpdate();
static void incorrectCodeIndicatorUpdate();
static void systemBlockedIndicatorUpdate();

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();
static void userInterfaceDisplayPasscode();
static void userInterfaceDisplayIncorrect();
static void userInterfaceDisplayCorrect();
static void userInterfaceDisplayDisabled();


//=====[Implementations of public functions]===================================

void userInterfaceInit()
{
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
    matrixKeypadInit( SYSTEM_TIME_INCREMENT_MS );
    userInterfaceDisplayInit();
}

void userInterfaceUpdate()
{
    userInterfaceMatrixKeypadUpdate();
    incorrectCodeIndicatorUpdate();
    systemBlockedIndicatorUpdate();
    userInterfaceDisplayUpdate();
}

bool incorrectCodeStateRead()
{
    return incorrectCodeState;
}

void incorrectCodeStateWrite( bool state )
{
    incorrectCodeState = state;
}

bool systemBlockedStateRead()
{
    return systemBlockedState;
}

void systemBlockedStateWrite( bool state )
{
    systemBlockedState = state;
}

bool userInterfaceCodeCompleteRead()
{
    return codeComplete;
}

void userInterfaceCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================

static void userInterfaceMatrixKeypadUpdate()
{
    static int numberOfHashKeyReleased = 0;
    char keyReleased = matrixKeypadUpdate();

    if( keyReleased != '\0' ) {

        if( sirenStateRead() && !systemBlockedStateRead() ) {
            if( !incorrectCodeStateRead() ) {
                codeSequenceFromUserInterface[numberOfCodeChars] = keyReleased;
                numberOfCodeChars++;
                if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
                    codeComplete = true;
                    numberOfCodeChars = 0;
                }
            } else {
                if( keyReleased == '#' ) {
                    numberOfHashKeyReleased++;
                    if( numberOfHashKeyReleased >= 2 ) {
                        numberOfHashKeyReleased = 0;
                        numberOfCodeChars = 0;
                        codeComplete = false;
                        incorrectCodeState = OFF;
                    }
                }
            }
        }
    }
}


static void userInterfaceDisplayInit()
{
    displayInit();
     
    displayCharPositionWrite ( 0,0 );
    displayStringWrite( "Enter Passcode" );

}

static void userInterfaceDisplayUpdate()
{
    static int accumulatedDisplayTime = 0; 
    
    if( accumulatedDisplayTime >=
        DISPLAY_REFRESH_TIME_MS ) {
            if (userInterfaceCodeCompleteRead()){
                if (incorrectCodeStateRead()){
                    numberTries ++;
                    userInterfaceDisplayIncorrect();
                } else if(numberTries > 3){
                    userInterfaceDisplayDisabled();
                }else {
                    userInterfaceDisplayCorrect();
                } 
            } else {
                userInterfaceDisplayPasscode();
            }
    } else {
        accumulatedDisplayTime =
            accumulatedDisplayTime + SYSTEM_TIME_INCREMENT_MS;        
    } 
}

static void userInterfaceDisplayPasscode()
{
    char passcodeString[4] = "";
    char pressedKey = matrixKeypadUpdate();

    for (int i = 0; i < 4; i++){
        displayCharPositionWrite(i, 1);
        char pressedKey = matrixKeypadUpdate();
        sprintf(passcodeString, "%c", pressedKey);
        displayStringWrite( passcodeString );
        }   
}


static void userInterfaceDisplayIncorrect()
{
    char numberTriesString[1] = "";

    displayCharPositionWrite ( 0, 0 );
    displayStringWrite( "Incorrect Code" );


    displayCharPositionWrite(0, 1);
    displayStringWrite( "Tries: ");


    sprintf(numberTriesString, "%c", numberTries);
    displayCharPositionWrite ( 7, 1 );
    displayStringWrite( numberTriesString );
}


static void userInterfaceDisplayCorrect()
{
    displayCharPositionWrite ( 0, 0 );
    displayStringWrite( "Correct Code" );


    displayCharPositionWrite ( 0, 1 );
    displayStringWrite( "Gate Unlocked" );
}


static void userInterfaceDisplayDisabled()
{
    displayCharPositionWrite ( 0, 0 );
    displayStringWrite( "Gate is Disabled" );


    displayCharPositionWrite ( 0, 1 );
    displayStringWrite( "Reset System" );
}


static void incorrectCodeIndicatorUpdate()
{
    incorrectCodeLed = incorrectCodeStateRead();
}

static void systemBlockedIndicatorUpdate()
{
    systemBlockedLed = systemBlockedState;
}
