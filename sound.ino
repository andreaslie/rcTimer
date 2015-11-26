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
     tone(soundPin, 1000, 200);
     delay(50);
     tone(soundPin, 1000, 200);
     delay(50);
     tone(soundPin, 1000, 200);
}

/**
 * Function for sounding a new AllTime Race Record
 */
void soundAllTimeRaceRecord()
{
     tone(soundPin, 1000, 200);
     delay(50);
     tone(soundPin, 1000, 200);
     delay(50);
     tone(soundPin, 1000, 200);
}

/*
 *
 */
void soundRaceFinished()
{
//    tone(soundPin, 400, 500);
    
   // tone(soundPin, 800, 400);
}

/**
 *
 */
void soundLapCompleted()
{
   //  tone(soundPin, 600, 400);
}
