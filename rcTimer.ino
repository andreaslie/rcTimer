
#include <LiquidCrystal.h>

const char * versionNumber = "v0.2";

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
volatile unsigned long msLastButtonHold = 0;
volatile bool isButtonUp                = true;
const unsigned int resetCriteria        = 3000;

// buffer for laptimes in current race
unsigned long lapTimes[100];

// pins
const int lapFinishSensorPin  = 2;
const int togglePin           = 3;
const int ledPin              = 5;

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

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);

/**
 * Init function
 */
void setup()
{
    Serial.begin(9600);      // open the serial port at 9600 bps

    // activate pins
    pinMode(lapFinishSensorPin, INPUT_PULLUP);
    pinMode(togglePin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);

    // attach interrupts
    attachInterrupt(0, isrLapTimer, FALLING);
    attachInterrupt(1, togglePinActivated, RISING);

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
    isButtonUp     = false;
    
    msStartTime       = 0;
    msLastLapTime     = 0;
    msLastStartTime   = 0;
    msCurrentTime     = 0;
    msBestLapTime     = 0;
    lapCounterNo      = 0;
    bestLapNo         = 0;
    totalRaceLaps     = 3;
    currentLapIndex   = -3;
    
    printInfoOnLcd();  
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
        writeLaptimeLcd(0, lapCounterNo-1, msLastLapTime);

        if (!raceFinishing)
            writeLaptimeLcd(1, bestLapNo, msBestLapTime);
        else
        {
            lcd.setCursor(0,1);
            lcd.print("RACE ENDED      ");
        }
    }
    
    // check for reset criteria
    if (isButtonUp)
    {
        if ((msLastButtonHold + resetCriteria) <= now)
        {
            Serial.println("RESET");
            msLastButtonHold = now;
            msLastButtonPush = now;
            resetSystem();
        }
    }
}

/**
 * Function for writing the Race Total Time and Best Lap
 */
void writeRaceTotals()
{
    lcd.clear();
    lcd.setCursor(0,0);
    int position = 0;
    position += lcd.print("TIME: ");
    writeLap(msCurrentTime, 0);
    lcd.setCursor(0,1);
    position += lcd.print("BEST LAP: ");
    writeLap(msBestLapTime, 1);
}

/**
 * Function for writing the AllTimeTotals (since last reset..)
 */
void writeAllTimeTotals()
{
    lcd.clear();
    lcd.setCursor(0,0);
    int position = 0;
    position += lcd.print("αTIME: ");
    writeLap(alltimeBestRace, 0);
    lcd.setCursor(0,1);
    position += lcd.print("αLAP: ");
    writeLap(alltimeBestLap, 1);
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
        soundAllTimeLapRecord();
        return true;
    }

    return false;
}

/**
 * Helper function for determining alltime race record
 */
void checkForRaceRecord()
{
    if (alltimeBestRace = 0)
        alltimeBestRace = msCurrentTime;
    else if (alltimeBestRace > msCurrentTime)
    {
        alltimeBestRace = msCurrentTime;
        soundAllTimeRaceRecord();
    }
}

/**
 * Function for sounding a new best Alltime Lap
 */
void soundAllTimeLapRecord()
{
 
}

/**
 * Function for sounding a lap record
 */
void soundLapRecord()
{
  
}

/**
 * Function for sounding a new AllTime Race Record
 */
void soundAllTimeRaceRecord()
{
  
}

/**
 * Function for writing lap time at specified column
 */
void writeLap(unsigned int lapTime, unsigned int column)
{
    int position = 10; // always start time at this position
    lcd.setCursor(position, column);

    unsigned int seconds = (int)(lapTime / 1000);
    unsigned int milliseconds = lapTime - (seconds * 1000);

    if (seconds < 10)
    {
        position += lcd.print(" ");
        lcd.setCursor(position, column);
    }

    position += lcd.print(seconds);
    
    lcd.setCursor(position, column);
    position += lcd.print(".");   
    lcd.setCursor(position, column);

    if (milliseconds < 10)
    {
        position += lcd.print("00");
        lcd.setCursor(position, column);
    }
    else if(milliseconds < 100)
    {
        position += lcd.print("0");
        lcd.setCursor(position, column);
    }
    
    position += lcd.print(milliseconds);
}

/**
 * 
 */
void writeRaceLapOnLcd(unsigned int column, unsigned int index)
{
    int position = 0;
    lcd.setCursor(position, column);
    position += lcd.print("#");
    lcd.setCursor(position, column);
    position += lcd.print(index+1);
    lcd.setCursor(position, column);
    position += lcd.print(" ");

    writeLap(lapTimes[index], column);
}

/**
 * Update the Current & Best laptime on the display
 */
void writeLaptimeLcd(unsigned int column, unsigned int lapCount, unsigned long lapTime)
{
    volatile int position = 0;

    lcd.setCursor(position, column);
    
    if (column == 0)
        position += lcd.print("LAST #");
    else
        position += lcd.print("BEST #");
    
    lcd.setCursor(position, column);
    position += lcd.print(lapCount);
    
    writeLap(lapTime, column);
}

/**
 * 
 */
void printInfoOnLcd()
{
    lcd.setCursor(0,0);
    lcd.write("RC Timer ");
    lcd.setCursor(9,0);
    lcd.write(versionNumber);
    lcd.setCursor(0,1);
    int position = 0;
    position += lcd.print("LAPS: ");
    lcd.setCursor(position, 1);
    lcd.print(totalRaceLaps);
}

/**
 * Interrupt service routine for Pushbutton
 */
void togglePinActivated()
{
    volatile unsigned long now = millis();

    // fix the reset bouncing

   // isButtonUp = digitalRead(togglePin);

    if (isButtonUp)
        msLastButtonHold = now;

    if ((msLastButtonPush + interruptSleep) <= now)
    {    
        msLastButtonPush = now;
      
        if (!raceStarted)
        {
            lcd.clear();
            ++totalRaceLaps;
            printInfoOnLcd();
        }
        else if (raceEnded)
        {
            raceFinishing = false;
            currentLapIndex += 2;
            lcd.clear();

            Serial.println(currentLapIndex);

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
                writeRaceLapOnLcd(0, currentLapIndex-1);

                // only show if available lap
                if (currentLapIndex < totalRaceLaps)
                    writeRaceLapOnLcd(1, currentLapIndex);
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
        lcd.print("RACE STARTED");
        
        // begin next lap
        lapCounterNo++;

        blinkLedLight();
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
                bool wasAlltimeLapRecord = checkForLapRecord();
                
                if (bestLapNo != 0 && !wasAlltimeLapRecord)
                    soundLapRecord();
              
                msBestLapTime = msLastLapTime;
                bestLapNo = lapCounterNo;
            }
       
            // begin next lap
            lapCounterNo++;
  
            if (lapCounterNo > totalRaceLaps)
            {
                raceFinishing = true; // allow
                raceEnded = false;

                // calculate race time
                msCurrentTime = now - msStartTime;

                checkForRaceRecord();
            }
        }

        blinkLedLight();
    }
}

/**
 * Turn the led on and off again
 */
void blinkLedLight()
{
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
}

