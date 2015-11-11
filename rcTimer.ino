/**
 * 
 */

#include <LiquidCrystal.h>

volatile unsigned long msStartTime        = 0;
volatile unsigned long msLastLapTime      = 0;
volatile unsigned long msLastStartTime    = 0;
volatile unsigned long msCurrentTime      = 0;
volatile unsigned long msBestLapTime      = 0;
volatile unsigned int lapCounterNo        = 0;
volatile unsigned int bestLapNo           = 0;

volatile unsigned long msLastButtonPush = 0;

unsigned long lapTimes[100];

const int lapFinishSensorPin = 2;
const int togglePin = 3;
const unsigned int interruptSleep = 200;
const unsigned int finishSleep = 5000;

volatile unsigned int totalRaceLaps = 3;
volatile int currentLapIndex = -1;

bool raceStarted = false;
bool raceFinishing = false;
bool raceEnded = false;

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);

void setup() {
    Serial.begin(9600);      // open the serial port at 9600 bps
    
    pinMode(lapFinishSensorPin, INPUT_PULLUP);
    pinMode(togglePin, INPUT_PULLUP);
    
    attachInterrupt(0, isrLapTimer, FALLING);
    attachInterrupt(1, togglePinActivated, FALLING);
    
    lcd.begin(16, 2);
    printInfoOnLcd();
  
    Serial.println("RC Timer v0.1");
}

void loop()
{
    if (raceFinishing)
    {
        volatile unsigned long now = millis();
        if ((msLastStartTime + finishSleep) <= now)
        {
            writeRaceTotals();
            raceFinishing = false;
            raceEnded = true;
        }
    }
    
    // update the lcd display
    if (lapCounterNo > 1 && !raceEnded)
    {
        writeLaptimeLcd(0, lapCounterNo-1, msLastLapTime);
        writeLaptimeLcd(1, bestLapNo, msBestLapTime);
    }
   
    // sleep a bit
    delay(200);
}

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
 * 
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

void printInfoOnLcd()
{
    lcd.setCursor(0,0);
    lcd.write("RC Timer v0.1");
    lcd.setCursor(0,1);
    int position = 0;
    position += lcd.print("LAPS: ");
    lcd.setCursor(position, 1);
    lcd.print(totalRaceLaps);
}

void togglePinActivated()
{
    volatile unsigned long now = millis();
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

            if (currentLapIndex > totalRaceLaps) // allow cycling of laps
            {
                currentLapIndex = -1;
                writeRaceTotals();
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
 * 
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
        lcd.print("RACE ON");
        
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
            }
        }
    }
}

