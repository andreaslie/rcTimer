/**
 * Function for sounding a new best Alltime Lap
 */
void soundAllTimeLapRecord()
{
    // currently no sound
    /*
    tone(soundPin, 1319, 250);
    delay(350);
    tone(soundPin, 1319, 250);
    delay(350);
    tone(soundPin, 1319, 250);
    */
}

/**
 * Function for sounding a lap record
 */
void soundLapRecord()
{
    // currently no sound
    /*
    tone(soundPin, 1319, 200);
    */
}

/**
 * Function for sounding a new AllTime Race Record
 */
void soundAllTimeRaceRecord()
{
    tone(soundPin,660,100);
    delay(175);tone(soundPin,660,100);
    delay(250);tone(soundPin,660,100);
    delay(250);tone(soundPin,510,100);
    delay(150);tone(soundPin,660,100);
    delay(250);tone(soundPin,770,100);
  
    /*    
    delay(375);tone(soundPin,380,100);
    delay(387);tone(soundPin,510,100);
    delay(325);tone(soundPin,380,100);    
    delay(300);tone(soundPin,320,100);
    delay(350);tone(soundPin,440,100);
    delay(230);tone(soundPin,480,80);
    delay(265);tone(soundPin,450,100);
    delay(175);tone(soundPin,430,100);
    delay(250);tone(soundPin,380,100);
    delay(180);tone(soundPin,660,80);
    delay(150);tone(soundPin,760,50);
    delay(175);tone(soundPin,860,100);
    delay(230);tone(soundPin,700,80);
    delay(125);tone(soundPin,760,50);
    
    delay(255);tone(soundPin,660,80);
    delay(230);tone(soundPin,520,80);
    delay(155);tone(soundPin,580,80);
    delay(155);tone(soundPin,480,80);
    delay(275);tone(soundPin,510,100);
    delay(375);tone(soundPin,380,100);
    delay(300);tone(soundPin,320,100);
    delay(350);tone(soundPin,440,100);
    delay(230);tone(soundPin,480,80);

    delay(265);tone(soundPin,450,100);
    delay(175);tone(soundPin,430,100);
    delay(150);tone(soundPin,380,100);
    delay(180);tone(soundPin,660,80);
    delay(150);tone(soundPin,760,50);
    delay(175);tone(soundPin,860,100);

    delay(230);tone(soundPin,700,80);
    delay(125);tone(soundPin,760,50);
    delay(255);tone(soundPin,660,80);
    delay(230);tone(soundPin,520,80);
    delay(155);tone(soundPin,580,80);
    delay(155);tone(soundPin,480,80);
    */
}

/**
 * Sound whenever a race is completed
 */
void soundRaceFinished()
{
    tone(soundPin, 1050, 100);
    delay(150);    
    tone(soundPin, 1050, 100);
    delay(150);
    tone(soundPin, 1050, 100);
    delay(150);
    tone(soundPin, 1050, 300);
}

/**
 * Sound whenever a lap is completed
 */
void soundLapCompleted()
{ 
    tone(soundPin, 1050, 100);
}

/**
 * Sound last lap
 */
void soundLastLap()
{
    tone(soundPin, 1050, 300);
}

/**
 * Sound beeps when race is about to start
 */
void readyForRace()
{
    tone(soundPin, 400, 300);
    delay(600);
    tone(soundPin, 400, 300);
    delay(600);
    tone(soundPin, 400, 300);
    delay(600);
    tone(soundPin, 800, 400);
}

