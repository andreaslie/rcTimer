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
    position += lcd.print("TOPTIME: ");
    writeLap(alltimeBestRace, 0);
    lcd.setCursor(0,1);
    position += lcd.print("TOPLAP: ");
    writeLap(alltimeBestLap, 1);
}

/**
 * Function for writing lap time at specified column
 */
void writeLap(unsigned long lapTime, unsigned int column)
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
void writeRaceLapsOnDisplay(unsigned int column, unsigned int index)
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
void writeLapTime(unsigned int column, unsigned int lapCount, unsigned long lapTime)
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
 * Print initial info on display
 */
void writeInfoOnDisplay()
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
 * Turn the led on and off again
 */
void blinkLedLight()
{
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
}

/**
 * Prints an error message when laser is not correctly calibrated
 */
void writeLaserErrorMessage()
{
    lcd.setCursor(0,0);
    lcd.write("--LASER FAULTY--");
    lcd.setCursor(0,1);
    lcd.write("--RESET SYSTEM--");
}
