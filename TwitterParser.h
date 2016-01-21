#include "TembooAccount.h"
#define TWITTER_TRIGGER     ("created_at\":\"")

#define TWITTER_BUFFER_SIZE (14)
#define TWEETS_PER_PAGE (20)
 
bool ok=false; 
char m_buffer[TWITTER_BUFFER_SIZE];
bool m_foundTrigger = false;
int m_counter = 0;

struct Time
  {
    int m_hour;
    int m_minute;
    int m_second;
  };
  
Time m_time1;
Time m_time2;


void reset() {

  for (int i = 0; i < TWITTER_BUFFER_SIZE; i++)
  {
    m_buffer[i] = '\0';
  }

  m_foundTrigger = false;

  m_time1.m_hour = 0;
  m_time1.m_minute = 0;
  m_time1.m_minute = 0;

  m_time2.m_hour = 0;
  m_time2.m_minute = 0;
  m_time2.m_minute = 0;
  m_counter = 0;

}

bool isNumber(char a)
{
  if ((int)a >= (int)'0' && (int)a <= (int)'9')
  {
    return true;
  }
  return false;
}

bool isTimeFormat(const char* mbuffer)
{
  // test for time format, XX:XX:XX
  if (
    isNumber(mbuffer[0]) &&
    isNumber(mbuffer[1]) &&
    mbuffer[2] == ':' &&
    isNumber(mbuffer[3]) &&
    isNumber(mbuffer[4]) &&
    mbuffer[5] == ':' &&
    isNumber(mbuffer[6]) &&
    isNumber(mbuffer[7])
  )
  {
    return true;
  }

  return false;
}

void setTimeFromBuffer(Time& time, const char* mbuffer)
{

  time.m_hour   = atoi(&mbuffer[0]);
  time.m_minute = atoi(&mbuffer[3]);
  time.m_second = atoi(&mbuffer[6]);
  //Serial.print("Settimefrombuffer :: Time:");
  //Serial.println(time.m_hour);
  //Serial.print(":");
  //Serial.print(time.m_minute);
  //Serial.print(":");
  //Serial.println(time.m_second);
}

bool parse(char a)
{
  //Serial.print("parse() called");
  // move everything in the buffer down a slot
  for (int i = 0; i < TWITTER_BUFFER_SIZE - 2; i++)
  {
    m_buffer[i] = m_buffer[i + 1];
  }

  // and add the new character
  m_buffer[TWITTER_BUFFER_SIZE - 2] = a;
  m_buffer[TWITTER_BUFFER_SIZE - 1] = '\0';

  if (!m_foundTrigger)
  {
    // keep looking for the 'created_at":"' in the twitter stream
    if (strstr(m_buffer, TWITTER_TRIGGER))
    {
      //Serial.print(m_buffer);
      m_foundTrigger = true;

#ifdef DEBUG
      Serial.print("found trigger");
#endif // #ifdef DEBUG
      Serial.print("");
      m_counter++;
      //tweets = counter;
      //Serial.print("m_counter:");
      //Serial.println(m_counter);
    }
  }
  else
  {
    // found 'created_at":"'
    // look for tweet time, XX:XX:XX
    if (isTimeFormat(m_buffer))
    {
#ifdef DEBUG
      Serial.print("found time");
#endif // #ifdef DEBUG

      // now found time of tweet, XX:XX:XX
      m_foundTrigger = false;

      if (m_counter == 1)
      {
    //    Serial.println ("Time 1 **");
        setTimeFromBuffer(m_time1, m_buffer);
      }
      else
      {
      //  Serial.println ("Time 2 ***");
        setTimeFromBuffer(m_time2, m_buffer);
        // Serial.println("m_tweetsPerRun:");
        // Serial.println(m_tweetsPerRun);
         if (m_counter >= TWEETS_PER_PAGE - 1)

        {
          //  Serial.print(m_tweetsPerRun);
         // Serial.println("TRUE");
          return true;
        }
      }
    }
  }
  return false;
}


long getTotalMinutesBetweenTimes(const Time& t1, const Time& t2)
{
  long time1 = 60 * 60 * t1.m_hour + 60 * t1.m_minute + t1.m_second;
  const long time2 = 60 * 60 * t2.m_hour + 60 * t2.m_minute + t2.m_second;
 // Serial.print("gettotalminitesbwtimes :: Time 1:");
 // Serial.println((int)time1);
 // Serial.print("Time 2:");
 // Serial.println((int)time2);

  if (time1 < time2)
  {
    // assume that t2 is yesterday.
    // add on 1 day to time1 to keep them both measured from 00:00 to yesterday
    time1 += 24 * 60 * 60;

#ifdef DEBUG
    Serial.print("DEBUG assuming tweet was from yesterday");
#endif // #ifdef DEBUG
  }

  long seconds = time1 - time2;
  //Serial.print("Minutes :" );
  //Serial.println( (int)seconds / 60);
  return seconds / 60;
}

float GetTweetsPerMinute()
{
  long minutes = getTotalMinutesBetweenTimes(m_time1, m_time2);

  //Serial.print("GetTweets Per minute:: minutes:");
  //Serial.println((int)minutes);
  if (minutes < 1)
  {

#ifdef DEBUG
    Serial.print("unexpected number of minutes");
#endif // #ifdef DEBUG

    minutes = 1;
  }
  //  Serial.print("m_counter / minutes \n");
  //Serial.print("m_counter:");
  //Serial.println(m_counter);
  const float tps = (float)m_counter / (float)minutes;

  return tps;
}

