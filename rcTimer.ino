/**
  *  TODO: 
  *  remove blink delay
  *  add different blink signals
  *
  */

#include <LiquidCrystal.h>

const char * versionNumber = "v0.3";

// the best lap & race times since reset of device
volatile unsigned long alltimeBestLap     = 0;
volatile unsigned long alltimeBestRace    = 0;

// the current race variables
volatile unsigned long msStartTime        = 0;
volatile unsigned long msLastLapTime      = 0;
volatile unsigned long msLastStartTime    = 0;
volatile unsigned long msCurrentTime      = 0;
volatile unsigned long msBestLapTime      = 0;
volatile unsigned int lapCounterNo        = 0;
volatile unsigned int bestLapNo           = 0;

// button variables
volatile unsigned long msLastButtonPush = 0;

// buffer for laptimes in current race
unsigned long lapTimes[100];

// pins
const int lapFinishSensorPin  = 2;
const int buttonPin           = 3;
const int ledPin              = 5;
const int resetPin            = 13;
const int soundPin            = 6;

// sleep constants
const unsigned int interruptSleep = 200;
const unsigned int finishSleep    = 5000;

// race lap counters
volatile int totalRaceLaps    = 3;
volatile int currentLapIndex  = -3;

// race states
bool raceStarted    = false;
bool raceFinishing  = false;
bool raceEnded      = false;

// sound states
bool allTimeLap     = false;
bool allTimeRace    = false;
bool bestLap        = false;
bool completedRace  = false;
bool completedLap   = false; // don't indicate on start

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);

/**
 * Init function
 */
void setup()
{
    Serial.begin(9600);      // open the serial port at 9600 bps

    // activate pins
    pinMode(lapFinishSensorPin, INPUT_PULLUP);
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(resetPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);

    // attach interrupts
    attachInterrupt(0, isrLapTimer, FALLING);
    attachInterrupt(1, buttonPinActivated, RISING);

    // start display
    lcd.begin(16, 2);

    resetSystem();
}

/**
 * Function for resetting the system
 */
void resetSystem()
{
    lcd.clear();
    
    raceStarted    = false;
    raceFinishing  = false;
    raceEnded      = false;
    
    msStartTime       = 0;
    msLastLapTime     = 0;
    msLastStartTime   = 0;
    msCurrentTime     = 0;
    msBestLapTime     = 0;
    lapCounterNo      = 0;
    bestLapNo         = 0;
    totalRaceLaps     = 3;
    currentLapIndex   = -3;
    
    writeInfoOnDisplay();

    allTimeLap     = false;
    allTimeRace    = false;
    bestLap        = false;
    completedRace  = false;
    completedLap   = false; // don't indicate on start

    delay(500);

    readyForRace();
}

/**
 * The System Loop
 */
void loop()
{ 
    // sleep a bit
    delay(200);
    
    volatile unsigned long now = millis();
    
    // update the lcd display
    if (raceFinishing)
    {
        if ((msLastStartTime + finishSleep) <= now)
        {
            writeRaceTotals();
            raceFinishing = false;
            raceEnded = true;
        }
    }
    
    
    if (lapCounterNo > 1 && !raceEnded)
    {    
        writeLapTime(0, lapCounterNo-1, msLastLapTime);

        if (!raceFinishing)
            writeLapTime(1, bestLapNo, msBestLapTime);
        else
        {
            lcd.setCursor(0,1);
            lcd.print("RACE END");
        }
    }

    // play sounds accordingly
    if  (allTimeRace)
    {
        soundAllTimeRaceRecord();
        allTimeRace = false;
    }
    else if (allTimeLap)
    {
        soundAllTimeLapRecord();
        allTimeLap = false;
    }
    else if (completedRace)
    {
        soundRaceFinished();
        completedRace = false;
    }
    else if (bestLap)
    {
        soundLapRecord();
        bestLap = false;
    }
    else if (completedLap)
    {
        soundLapCompleted();
        completedLap = false;
    }
    
    if (!digitalRead(resetPin))
        resetSystem();
}

/**
 * Helper function for determining alltime lap record
 */
bool checkForLapRecord()
{
    // is it a new lap record?
    if (alltimeBestLap == 0)
    {
        alltimeBestLap = msLastLapTime;
        return true;
    }
    else if (alltimeBestLap > msLastLapTime)
    {
        alltimeBestLap = msLastLapTime;
        return true;
    }

    return false;
}

/**
 * Helper function for determining alltime race record
 */
bool checkForRaceRecord()
{
    if (alltimeBestRace == 0)
    {
        alltimeBestRace = msCurrentTime;
        return false; // dont indicate since this is the first record ever
    }
    else if (alltimeBestRace > msCurrentTime)
    {
        alltimeBestRace = msCurrentTime;
        return true;
    }

    return false;
}

/**
 * Interrupt service routine for Pushbutton
 */
void buttonPinActivated()
{
    volatile unsigned long now = millis();

    if ((msLastButtonPush + interruptSleep) <= now)
    {       
        msLastButtonPush = now;
      
        if (!raceStarted)
        {
            lcd.clear();
            ++totalRaceLaps;
            writeInfoOnDisplay();
        }
        else if (raceEnded)
        {
            raceFinishing = false;
            currentLapIndex += 2;
            lcd.clear();

            if (currentLapIndex > totalRaceLaps) // allow cycling of laps
            {
                currentLapIndex = -3;
                writeRaceTotals();
            }
            else if(currentLapIndex < 0)
            {
                writeAllTimeTotals();
            }
            else
            {
                writeRaceLapsOnDisplay(0, currentLapIndex-1);

                // only show if available lap
                if (currentLapIndex < totalRaceLaps)
                    writeRaceLapsOnDisplay(1, currentLapIndex);
            }
        }
    }
}

/**
 * Interrupt service routine for Lap Timer
 */
void isrLapTimer()
{
    volatile unsigned long now = millis();

    allTimeLap     = false;
    allTimeRace    = false;
    bestLap        = false;
    completedRace  = false;
    completedLap   = false; // don't indicate on start

    // start of laps?
    if (!raceStarted)
    {
        // use 'now' as starttime basis
        msStartTime     = now;
        msLastStartTime = now;
        
        // update last measurement made
        lcd.clear();

        raceStarted = true;

        lcd.setCursor(0,0);
        lcd.print("STARTED");
        
        // begin next lap
        lapCounterNo++;
    }
    else if ((msLastStartTime + interruptSleep) <= now)
    {
        if (!raceEnded && !raceFinishing)
        {       
            // update last measurement made
            msLastLapTime = now - msLastStartTime;
            msLastStartTime = now;

            // store current lap time
            lapTimes[lapCounterNo-1] = msLastLapTime;

            // best lap?
            if (bestLapNo == 0 || msBestLapTime > msLastLapTime)
            {
                allTimeLap = checkForLapRecord();
                
                if (bestLapNo != 0 && !allTimeLap)
                    bestLap = true;
              
                msBestLapTime = msLastLapTime;
                bestLapNo = lapCounterNo;
            }
       
            // begin next lap
            lapCounterNo++;
  
            if (lapCounterNo > totalRaceLaps)
            {
                raceFinishing = true;
                raceEnded     = false;

                // calculate race time
                msCurrentTime = now - msStartTime;

                completedRace = true;
                allTimeRace = checkForRaceRecord();
            }
        }

        if (!raceFinishing)
            completedLap = true;
            
        blinkLedLight();
    }
}

