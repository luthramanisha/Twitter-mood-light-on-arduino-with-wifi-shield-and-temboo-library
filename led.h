#define redPin    (9)
#define greenPin (11)
#define bluePin   (13)
#define fadeDelay (15)

// blink durations
#define SLOW_FLASHES (6)
#define SLOW_FLASH_DURATION (500) // 0.5 seconds
#define FAST_FLASHES (12)
#define FAST_FLASH_DURATION (250) 

// simple colour structure
typedef struct
{
  int r;
  int g;
  int b;
} Color;

int m_currentColorID;

enum COLORID {
  PINK = 0,
  YELLOW,
  ORANGE,
  RED,
  GREEN,
  BLUE,
  WHITE,

  // debug
  BLACK,
  MAGENTA,
  NUM_COLORS,
};
// default colours

const Color Colors[] = {
  (Color){255, 128, 128}, // pink
  (Color){255, 255, 0},   // yellow
  (Color){255, 96, 0},    // orange
  (Color){255, 0,   0},   // red
  (Color){0,   255, 0},   // green
  (Color){0,   0,   255}, // blue
  (Color){255, 255, 255}, // white
  (Color){255, 0, 255},   // magenta
  (Color){0, 0, 0},       // black
};

void Flash(int numberOfFlashes, int duration)
{
  Color currentColor;
  currentColor.r = Colors[m_currentColorID].r;
  currentColor.g = Colors[m_currentColorID].g;
  currentColor.b = Colors[m_currentColorID].b;

  Color black;
  black.r = Colors[BLACK].r;
  black.g = Colors[BLACK].g;
  black.b = Colors[BLACK].b;

  while (numberOfFlashes > 0)  {
    // set colour to black (turn off)
    analogWrite(redPin,  black.r);   
    analogWrite(greenPin,  black.g); 
    analogWrite(bluePin,  black.b);   

    // hold at black for duration
    delay(duration);

    // set colour to current colour
    analogWrite(redPin,  currentColor.r);   
    analogWrite(greenPin,  currentColor.g); 
    analogWrite(bluePin,  currentColor.b);   

    // hold at this color for this many ms
    delay(duration);
    numberOfFlashes--;
  } // while (numberOfFlashes > 0)
}

/*
 The led is initially set to be currentColorID and over time will fade
 to desiredColorID with a time delay, fadeDelay, measured in ms, between 
 each step. No effort is made to scale the step size for each rgb 
 channel so each may not complete at the same time.
*/
void FadeTo(int desiredColorID)
{

  // check for valid colorID
  if (desiredColorID >= NUM_COLORS || desiredColorID < 0)   {
      //logger.log("invalid Color id")
      return;
    }
  
  // get a local copy of the colors
  Color currentColor;
  currentColor.r = Colors[m_currentColorID].r;
  currentColor.g = Colors[m_currentColorID].g;
  currentColor.b = Colors[m_currentColorID].b;

  Color desiredColor;
  desiredColor.r = Colors[desiredColorID].r;
  desiredColor.g = Colors[desiredColorID].g;
  desiredColor.b = Colors[desiredColorID].b;
   
  bool done = false;

  while (!done)  {
    // move each of r,g,b a step closer to the desiredColor value
    if (currentColor.r < desiredColor.r)   {
      currentColor.r++;
    }
    else if (currentColor.r > desiredColor.r)   {
      currentColor.r--;
    }
    if (currentColor.g < desiredColor.g)   {
      currentColor.g++;
    }
    else if (currentColor.g > desiredColor.g)   {
      currentColor.g--;
    }
    if (currentColor.b < desiredColor.b)   {
      currentColor.b++;
    }
    else if (currentColor.b > desiredColor.b)   {
      currentColor.b--;
    }
        
    // write the new rgb values to the correct pins
    analogWrite(redPin,  currentColor.r);   
    analogWrite(greenPin,  currentColor.g); 
    analogWrite(bluePin,  currentColor.b);  
      
    // hold at this color for this many ms
    delay(fadeDelay);

    // done when we have reach desiredColor  
    done = (currentColor.r == desiredColor.r && currentColor.g == desiredColor.g && currentColor.b == desiredColor.b);
  } // while (!done)
  m_currentColorID = desiredColorID;
}
 
void setColorSlowFlash(int ColorID)
{
  FadeTo(ColorID);
  Flash(SLOW_FLASHES, SLOW_FLASH_DURATION);
}

void setColorFastFlash(int ColorID)
{
  FadeTo(ColorID);
  Flash(FAST_FLASHES, FAST_FLASH_DURATION);
}


void setColor(int ColorID, int flash)
{
  switch (flash)  {
  case 1:
    setColorSlowFlash(ColorID);
    break;
  case 2:
    setColorFastFlash(ColorID);
    break;

  default: // 0 and out of range
    FadeTo(ColorID);
    break;
  }
}

