#include <Bridge.h>
#include <Temboo.h>
#include "led.h"
#include "TembooAccount.h" // contains Temboo account information, as described below
#include "string.h"
#include "TwitterParser.h"
#include "SearchTweet.h"

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 5;   // Maximum number of times the Choreo should be executed

//char c ;
void setup() {
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  digitalWrite(bluePin, HIGH);
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  
  // For debugging, wait until the serial console is connected.
  delay(4000);
  while (!Serial);
  Bridge.begin();
}

void loop()
{
  // check inputs
  if (emotionSmoothingFactor < 0.0f || emotionSmoothingFactor > 1.0f)  {
    Serial.println("invalid emotionSmoothingFactor");
  }
  if (moodSmoothingFactor < 0.0f ||  moodSmoothingFactor > 1.0f)  {
    Serial.println("invalid moodSmoothingFactor");
  }

  for (int i = 0; i < NUM_MOOD_TYPES; i++)  {
    m_worldTemperamentRatios[i] = temperamentRatios[i];
    m_worldMoodCounts[i] = INVALID_MOOD_VALUE;
    m_worldMoodRatios[i] = INVALID_MOOD_VALUE;
  }
  
   m_worldMood = NUM_MOOD_TYPES;

  // debug code - check sum of m_worldTemperamentRatios is 1.
  float sum = 0;
  for (int i = 0; i < NUM_MOOD_TYPES; i++)  {
    sum += m_worldTemperamentRatios[i];
  }
  
  if (sum > 1.0f + 1e-4f || sum < 1.0f - 1e-4f)  {
    Serial.println("unexpected m_worldTemperamentRatios sum");
  }
  
  digitalWrite(redPin, HIGH);   
  digitalWrite(greenPin, HIGH); 
  digitalWrite(bluePin, HIGH);  
  
  m_currentColorID = WHITE;
    
  for (int i = 0; i < NUM_MOOD_TYPES; i++)  {  
   
    Serial.println("Running Tweets - Run #" + String(numRuns++));
  
    reset();
    TembooChoreo TweetsChoreo;

    // Invoke the Temboo client
    TweetsChoreo.begin();
    
    /*Moved to settings.h in SD card */
    
   // Set Temboo account credentials
   // TweetsChoreo.setAccountName(TEMBOO_ACCOUNT);
   // TweetsChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
   // TweetsChoreo.setAppKey(TEMBOO_APP_KEY);
   
   // Set Choreo inputs
   // TweetsChoreo.addInput("AccessToken", "2288625830-JAgZw4XLvoaiNa84XFCu4Dm22WJCKvEosib29jp");
    
    TweetsChoreo.addInput("Query", searchStrings[i]);
   
   /*Moved to settings.h in SD card */
   
   // TweetsChoreo.addInput("AccessTokenSecret", "O9yq1j0MDSOlf9UWVKvig4B4kf2QYRh0NfFsCpUe2JlFc");
   // TweetsChoreo.addInput("ConsumerSecret", "AOvDTQjUKfQXZIF0aXMxSyihlEK9jAl31XJCHwvud5s");
   // TweetsChoreo.addInput("ConsumerKey", "NtBrvlmlrv5GXy7mIbLG0g");
    
   // Identify the Choreo to run
   // TweetsChoreo.setChoreo("/Library/Twitter/Search/Tweets");
   
     TweetsChoreo.setSettingsFileToRead("/mnt/sda1/settings.h");
  
    //Ram check
    //Serial.print("\n[memCheck]");
    // Serial.println(freeRam());
    
    // Run the Choreo; when results are available, print them to serial
    TweetsChoreo.run();
    
    while(TweetsChoreo.available()) {
      char c = TweetsChoreo.read();
      parse(c);
    }
 
    float tweetsPerMinute;
    tweetsPerMinute = GetTweetsPerMinute();
   
    // debug code
    Serial.println("");
    Serial.print(moodNames[i]);
    Serial.print(": tweets per min = ");
    Serial.println(tweetsPerMinute);

    RegisterTweets(i, tweetsPerMinute);
    
    TweetsChoreo.close();
  }
  
  MOOD_TYPE newMood = ComputeCurrentMood();
  MOOD_INTENSITY newMoodIntensity = ComputeCurrentMoodIntensity();
  
  Serial.print("The Mood of the World is ... ");
  Serial.print(moodIntensityNames[(int)newMoodIntensity]);
  Serial.print(" ");
  Serial.println(moodNames[(int)newMood]);
  Serial.println("Waiting...");
  setColor((int)newMood, (int)newMoodIntensity);
  delay(30000); // wait 30 seconds between Tweets calls
}

void RegisterTweets(int moodID, float tweetsPerMinute)
{
   // check input is valid
  if (moodID < 0 || moodID >= NUM_MOOD_TYPES)  {
    Serial.println("invalid moodID");
    return;
  }
  if (tweetsPerMinute < 0) {
    Serial.println("unexpected tweetsPerMinute");
  }

  // I'm using the tweetsPerMinute to be equivalent to the current emotion.
  // to get the mood, average these potentially noisy and volatile emotions using an exponential moving average
  if (m_worldMoodCounts[moodID] == INVALID_MOOD_VALUE)  {
    // first time through
    m_worldMoodCounts[moodID] = tweetsPerMinute;
  }
  else  {
    const float a = emotionSmoothingFactor;
    m_worldMoodCounts[moodID] = (m_worldMoodCounts[moodID] * (1.0f - a)) + (tweetsPerMinute * a);
  }
}

MOOD_INTENSITY ComputeCurrentMoodIntensity() 
{ 
  // check input is valid
  if (m_worldMood < 0 || m_worldMood >= NUM_MOOD_TYPES) {
    Serial.println("invalid world mood");
    return MILD;
  }
  
  if (m_worldTemperamentRatios[m_worldMood] < 1e-4f) {
    Serial.print("unexpected m_worldTemperamentRatios");
    return EXTREME;
  }

 // get the mood ratio as a percent of the temperament ratio.
 // this will show the mood ratio as a divergence from the norm, and so is a good measure of mood intensity.
 const float percent = m_worldMoodRatios[m_worldMood] / m_worldTemperamentRatios[m_worldMood];
  if (percent > extremeMoodThreshold) {
    return EXTREME;
  }
  else if (percent > moderateMoodThreshold) {
    return CONSIDERABLE;
  }
  else  {
   return MILD;
  }
}

MOOD_TYPE ComputeCurrentMood() {
  // find the current ratios
  float sum = 0;
  for (int i = 0; i < NUM_MOOD_TYPES; i++) {
    sum += m_worldMoodCounts[i];
  }
  if (sum < 1e-4f)  {
    Serial.println("unexpected total m_worldMoodCounts");
    return m_worldMood;
  }

  for (int i = 0; i < NUM_MOOD_TYPES; i++)  {
    m_worldMoodRatios[i] = m_worldMoodCounts[i] / sum;
  }

  // find the ratio that has increased by the most, as a proportion of its moving average.
  // So that, for example, an increase from 5% to 10% is more significant than an increase from 50% to 55%.

  float maxIncrease = -1.0f;
  for (int i = 0; i < NUM_MOOD_TYPES; i++)  {
    float difference = m_worldMoodRatios[i] - m_worldTemperamentRatios[i];
    if (m_worldTemperamentRatios[i] < 1e-4f)   {
      Serial.print("unexpected m_worldTemperamentRatios");
      continue;
    }

    difference /= m_worldTemperamentRatios[i];
    if (difference > maxIncrease)   {
      maxIncrease = difference;
      m_worldMood = (MOOD_TYPE)i; // this is now the most dominant mood of the world!
    }
  }

  // update the world temperament, as an exponential moving average of the mood.
  // this allows the baseline ratios, i.e. world temperament, to change slowly over time.
  // this means, in affect, that the 2nd derivative of the world mood wrt time is part of the current mood calcuation.
  // and so, after a major anger-inducing event, we can see when people start to become less angry.
  sum = 0;

  for (int i = 0; i < NUM_MOOD_TYPES; i++)  {
    if (m_worldTemperamentRatios[i]  <= 0)   {
      Serial.println("m_worldTemperamentRatios should be initialised at construction");
      m_worldTemperamentRatios[i] = m_worldMoodRatios[i];
    }
    else   {
      const float a = moodSmoothingFactor;
      m_worldTemperamentRatios[i] = (m_worldTemperamentRatios[i] * (1.0f - a)) + (m_worldMoodRatios[i] * a);
    }
    sum += m_worldTemperamentRatios[i];
  }
  if (sum < 1e-4f)  {
    Serial.println("unexpected total m_worldTemperamentRatios total");
    return m_worldMood;
  }

  // and finally, renormalise, to keep the sum of the moving average ratios as 1.0f
  for (int i = 0; i < NUM_MOOD_TYPES; i++)  {
    m_worldTemperamentRatios[i] *= 1.0f / sum;
    Serial.print("temperament ratio: ");
    Serial.println(m_worldTemperamentRatios[i]);
  }

  // debug code - check sum is 1.
  sum = 0;
  for (int i = 0; i < NUM_MOOD_TYPES; i++)  {
    sum += m_worldTemperamentRatios[i];
  }

  if (sum > 1.0f + 1e-4f || sum < 1.0f - 1e-4f)  {
    Serial.println("unexpected renormalise result");
  }
  return m_worldMood;
}

//Ram check commented
/*
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
*/

