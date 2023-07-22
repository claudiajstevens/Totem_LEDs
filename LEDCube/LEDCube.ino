#include <FastLED.h>

#define LED_PIN               6
#define LEDS_PER_SEGMENT      7
#define LEDS_PER_WING_SEGMENT 9
#define SEGMENTS              12
#define WING_SEGMENTS         2
#define BRIGHTNESS            200
#define NUM_CUBE_LEDS         LEDS_PER_SEGMENT * SEGMENTS
#define NUM_WING_LEDS         LEDS_PER_WING_SEGMENT * WING_SEGMENTS
#define NUM_LEDS              NUM_CUBE_LEDS + NUM_WING_LEDS
#define NUM_LEDS_WITH_SAFETY  NUM_LEDS + 1

CRGB rawleds[NUM_LEDS];
CRGBSet leds(rawleds, NUM_LEDS);
CRGBSet cube1(leds(0, 83));
CRGBSet cube2(leds(0, 83));
CRGBSet cubeOutput(leds(0, 83));
CRGBSet wings1(leds(84, 101));
CRGBSet wings2(leds(84, 101));
CRGBSet wingsOutput(leds(84, 101));

struct CRGB * ledarray[] = {leds, cube1, cube2, cubeOutput, wings1, wings2, wingsOutput};

uint8_t blendAmount = 0;
uint8_t patternCounter = 0;
uint8_t source1Pattern = 1;
uint8_t source2Pattern = 0;
bool useSource1 = false;


void setup() {
  delay(2000); // power-up saftey delay
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(57600);
}

void loop() {
//   fill_solid(ledarray[1], 84, CHSV( 213, 255, 255)); //84 is number of elements in the array
//   FastLED.show();
//   delay(700);
//   fill_solid( ledarray[1], 84, CRGB::Black);
//   FastLED.show();    
//
    
    
    //fill_solid(ledarray[4] ,18, CHSV( 213, 255, 255)); //8 is number of elements in the array


//FastLED.show();
//   delay(700);
//   fill_solid( ledarray[2], 18, CRGB::Black);
//   FastLED.show();  
//   delay(700);
//
//   fill_solid(ledarray[0], 102, CHSV(213, 255, 255));
//   FastLED.show();
//   delay(700);
//   fill_solid(ledarray[0], 102, CRGB::Black);
//   FastLED.show();
//   delay(700);

  EVERY_N_MILLISECONDS(10){
    blend(ledarray[1], ledarray[2], ledarray[3], NUM_CUBE_LEDS, blendAmount);
    blend(ledarray[4], ledarray[5], ledarray[6], NUM_WING_LEDS, blendAmount);

        
    if (useSource1) {
      if (blendAmount < 255) blendAmount++;                   // Blend 'up' to source 2
    } else {
      if (blendAmount > 0) blendAmount--;                     // Blend 'down' to source 1
    }

  }



  EVERY_N_SECONDS(8){
    nextPattern();
  }


  wingPrettyNoise(ledarray[4]);
  wingRainbowComet(ledarray[5]);
  
  runPattern(source1Pattern, ledarray[1]);
  runPattern(source2Pattern, ledarray[2]);

  FastLED.show();

  

}

void nextPattern() {
  patternCounter = (patternCounter + 1) % 5;

  if (useSource1) source1Pattern = patternCounter;
  else source2Pattern = patternCounter;

  useSource1 = !useSource1;
}

void runPattern(uint8_t pattern, CRGB *LEDarray) {
  switch (pattern) {
    case 0:
      rainbowComet(LEDarray);
      break;
    case 1:
      prettyNoise(LEDarray);
      break;
    case 2:
      randomStar(LEDarray);
      break;
    case 3:
      fillRainbow(LEDarray);
      break;
    case 4:
      pixels(LEDarray);
      break;
  }
}


uint8_t xyz(uint8_t x, uint8_t y, uint8_t z) {

  /* The coordinates of the edges run from 0 - 15. Each edge only has 14 'real' pixels,
   * so there are only LEDs from 1 - 14. The vertices (corners) are missing.
   * If one of these vertices is requested, return a safe pixel that is not displayed.
   * We do the same for coordinates that are inside the cube i.e. not on an edge.
   */
  
  uint8_t lps = LEDS_PER_SEGMENT;
  //uint8_t safePx = NUM_LEDS;
  uint8_t safePx = NUM_LEDS + 1;

  if ((x == 0 || x == lps + 1) && (y == 0 || y == lps + 1) && (z == 0 || z == lps + 1)) return safePx;

  // z-direction edges
  if (x == 0        && y == 0)        return (8 * lps)  - z;      // Seg 7
  if (x == 0        && y == lps + 1)  return (12 * lps) - z;      // Seg 11
  if (x == lps + 1  && y == 0)        return (3 * lps)  + z - 1;  // Seg 3
  if (x == lps + 1  && y == lps + 1)  return (9 * lps)  + z - 1;  // Seg 9

  // y-direction edges
  if (x == 0        && z == 0)        return y - 1;               // Seg 0
  if (x == 0        && z == lps + 1)  return (7 * lps)  - y;      // Seg 6
  if (x == lps + 1  && z == 0)        return (3 * lps)  - y;      // Seg 2
  if (x == lps + 1  && z == lps + 1)  return (4 * lps)  + y - 1;  // Seg 4

  // x-direction edges
  if (y == 0        && z == 0)        return (8 * lps)  + x - 1;  // Seg 8
  if (y == 0        && z == lps + 1)  return (11 * lps) - x;      // Seg 10
  if (y == lps + 1  && z == 0)        return lps        + x - 1;  // Seg 1
  if (y == lps + 1  && z == lps + 1)  return (6 * lps)  - x;      // Seg 5

  // If none of the above, we have an invalid coordinate
  return safePx;
}

//------------ Patterns below ------------//

void pixels(CRGB *LEDarray) {
  static uint8_t pos = 0;
  static uint8_t a = 0;
  static uint8_t b = 0;

  // Fill all pixels and blend them together
  for (int c = 0; c <= LEDS_PER_SEGMENT + 1; c++) {
    LEDarray[xyz(a,b,c)] = blend(LEDarray[xyz(a,b,c)], CRGB::Orange, 128);
    LEDarray[xyz(a,c,b)] = blend(LEDarray[xyz(a,c,b)], CRGB::Magenta, 128);
    LEDarray[xyz(c,a,b)] = blend(LEDarray[xyz(c,a,b)], CRGB::Blue, 128);
  }

  EVERY_N_MILLISECONDS(33) {    
    // Coordinates moving around a square
    if(pos < 15) a++;
    else if (pos <= (LEDS_PER_SEGMENT * 2) + 1) b++;
    else if (pos <= (LEDS_PER_SEGMENT * 3) + 2) a--;
    else b--;

    // Start again one we reach the end of the square
    pos = (pos + 1) % ((LEDS_PER_SEGMENT + 1) * 4);
  }

  fadeToBlackBy(LEDarray, NUM_CUBE_LEDS, 10);
  
}

void fillRainbow(CRGB *LEDarray) {
  static uint8_t pos = 0;
  
  uint8_t noise = inoise8(millis()/5);
  fill_rainbow(LEDarray, LEDS_PER_SEGMENT, noise, 10);

  // Copy to other segments
  for (int i = 0; i < SEGMENTS; i++) {
    memmove8(&LEDarray[LEDS_PER_SEGMENT * i], &LEDarray[0], LEDS_PER_SEGMENT * sizeof(CRGB));
  }

  // White dots moving up and down vertical pillars
  LEDarray[xyz(0, 0, LEDS_PER_SEGMENT - pos)] = CRGB::White;
  LEDarray[xyz(0, LEDS_PER_SEGMENT + 1, pos)] = CRGB::White;
  LEDarray[xyz(LEDS_PER_SEGMENT + 1, LEDS_PER_SEGMENT + 1, LEDS_PER_SEGMENT - pos)] = CRGB::White;
  LEDarray[xyz(LEDS_PER_SEGMENT + 1, 0, pos)] = CRGB::White;

  

  EVERY_N_MILLISECONDS(20) {
    pos = (pos + 1) % LEDS_PER_SEGMENT;
  }
}

void rainbowComet(CRGB *LEDarray) {
  static uint8_t easeOutVal = 0;
  static uint8_t easeInVal  = 0;

  // Make pattern appear over two segments
  uint8_t ledsPerSegment = LEDS_PER_SEGMENT * 2;
  uint8_t segments = SEGMENTS / 2;

  easeOutVal = ease8InOutQuad(easeInVal);
  easeInVal++;

  uint8_t pos = lerp8by8(0, ledsPerSegment, easeOutVal);
  uint8_t hue =  map(pos, 0, ledsPerSegment, 0, 230);
  
  LEDarray[pos] = CHSV(hue, 255, 255);
  fadeToBlackBy(LEDarray, ledsPerSegment, 20);

  // Copy to other segments
  for (int i = 0; i < segments; i++) {
    memmove8(&LEDarray[ledsPerSegment * i], &LEDarray[0], ledsPerSegment * sizeof(CRGB));
  }
}

void randomStar(CRGB *LEDarray) {
  EVERY_N_MILLISECONDS(75) {
    LEDarray[random16(0, NUM_CUBE_LEDS)] = CRGB::LightGrey;
  }


  for (int i = 0; i < NUM_CUBE_LEDS; i++) {
    
    // Brightness
    uint8_t bNoise = inoise8(i * 100, millis());
    bNoise = constrain(bNoise, 50, 200);
    bNoise = map(bNoise, 50, 200, 20, 80);

    // Hue
    uint8_t hNoise = inoise8(i * 20, millis() / 5);
    hNoise = constrain(hNoise, 50, 200);
    hNoise = map(hNoise, 50, 200, 160, 192);
    
    if (LEDarray[i].g == 0) {
      LEDarray[i] = CHSV(hNoise, 255, bNoise);
    }
  }
  
  fadeToBlackBy(LEDarray, NUM_CUBE_LEDS, 5);
}


void prettyNoise(CRGB *LEDarray) {
  fill_noise16 (LEDarray, NUM_CUBE_LEDS, 1, 0, 100, 1, 1, 50, millis() / 3, 5);
}

void wingRainbowComet(CRGB *LEDarray) {
  static uint8_t easeOutVal = 0;
  static uint8_t easeInVal  = 0;
  

  // Make pattern appear over two segments
  uint8_t ledsPerWingSegment = LEDS_PER_WING_SEGMENT;
  uint8_t wingSegments = WING_SEGMENTS;

  easeOutVal = ease8InOutQuad(easeInVal);
  easeInVal++;

  uint8_t pos = lerp8by8(0, ledsPerWingSegment, easeOutVal);
  uint8_t hue =  map(pos, 0, ledsPerWingSegment, 0, 230);
  
  LEDarray[pos] = CHSV(hue, 255, 255);
  fadeToBlackBy(LEDarray, ledsPerWingSegment, 20);

  // Copy to other segments
  for (int i = 0; i < wingSegments; i++) {
    memmove8(&LEDarray[ledsPerWingSegment * i], &LEDarray[0], ledsPerWingSegment * sizeof(CRGB));
  }
}


void wingPrettyNoise(CRGB *LEDarray) {
  fill_noise16 (LEDarray, NUM_WING_LEDS, 1, 0, 100, 1, 1, 50, millis() / 3, 5);
}
