
// these numbers can be tweaked to get the system to be more or less reactive
// to be more or less susceptible to noise or short term emotional blips, like sport results
// or bigger events, like world disasters
#define emotionSmoothingFactor (0.1f)
#define moodSmoothingFactor (0.05f)
#define moderateMoodThreshold (2.0f)
#define extremeMoodThreshold (4.0f)
#define INVALID_MOOD_VALUE (-1)

// save battery, put the wifly to sleep for this long between searches (in ms)
#define SLEEP_TIME_BETWEEN_SEARCHES (1000 * 5)
 
enum MOOD_TYPE {
  LOVE = 0,
  JOY,
  SURPRISE,
  ANGER,
  ENVY,
  SADNESS,
  FEAR,
  NUM_MOOD_TYPES,
};

enum MOOD_INTENSITY {
  MILD = 0,
  CONSIDERABLE,
  EXTREME,
  NUM_MOOD_INTENSITY,
};

const char* moodNames[NUM_MOOD_TYPES] = {
 "love",
 "joy",
 "surprise",
 "anger",
 "envy",
 "sadness",
 "fear",
};
 
const char* moodIntensityNames[NUM_MOOD_INTENSITY] = {
 "mild",
 "considerable",
 "extreme",
};

// the long term ratios between tweets with emotional content
// as discovered by using the below search terms over a period of time.
float temperamentRatios[NUM_MOOD_TYPES] = {
 0.13f,
 0.15f,
 0.20f,
 0.14f,
 0.16f,
 0.12f,
 0.10f,
};
 
char string_0[] = "i love you OR i love her OR i love him OR all my love OR i'm in love OR i really love";
char string_1[] = "happiest OR so happy OR so excited OR i'm happy OR woot OR w00t";
char string_2[] = "wow OR O_o OR can't believe OR wtf OR unbelievable";
char string_3[] = "i hate OR really angry OR iam mad OR really hate OR so angry";
char string_4[] = "i wish i OR i'm envious OR i'm jealous OR i want to be OR why can't i";
char string_5[] = "i'm so sad OR i'm heartbroken OR i'm so upset OR i'm depressed OR i can't stop crying";
char string_6[] = "i'm so scared OR i'm really scared OR i'm terrified OR i'm really afraid OR so scared i";
//char string_5[] = "i'm so sad";
//char string_6[] = "i'm so scared";

const char *searchStrings[] =        
{  
 string_0,
 string_1,
 string_2,
 string_3,
 string_4,
 string_5,
 string_6,
};

char searchString[160];
float m_worldTemperamentRatios[NUM_MOOD_TYPES];
float m_worldMoodCounts[NUM_MOOD_TYPES];
float m_worldMoodRatios[NUM_MOOD_TYPES];
MOOD_TYPE  m_worldMood;
