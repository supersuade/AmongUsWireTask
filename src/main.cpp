#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

unsigned long time;
int checkTime = 500;
bool checking = false;

struct RGB {
  int r;
  int g;
  int b;
};

struct match {
  int wire;
  int port;
  RGB color;
};

RGB RED = {255,0,0};
RGB ORANGE = {255,128,0};
RGB YELLOW = {255,255,0};
RGB GREEN = {0,255,0};
RGB TEAL = {0,255,255};
RGB BLUE = {0,0,255};
RGB PURPLE = {255,0,200};

int wires[4] = {2,3,4,5};
int ports[4] = {6,7,8,9};
RGB colors[6] = {RED,YELLOW,GREEN,BLUE,PURPLE};
int color_i[6] = {0,1,2,3,4};
match matches[4];


Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, 12, NEO_GRB + NEO_KHZ800);

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

int wireToLED(int val){
  return val-2;
}
int portToLED(int val){
  return 7-(val-6);
}

void bubbleUnsort(int *list, int elem)
{
  for (int a=elem-1; a>0; a--)
  {
    int r = random(a+1);
    //int r = rand_range(a+1);
    if (r != a)
    {
      int temp = list[a];
      list[a] = list[r];
      list[r] = temp;
    }
  }
}

void printArray(int *list, int elem){
  for(int i=0; i<elem; i++){
    Serial.print(list[i]);
  }
}

void setNewPuzzle(){
  bubbleUnsort(wires,4);
  bubbleUnsort(ports,4);
  bubbleUnsort(color_i,5);

  for(int i=0; i<4; i++){
    matches[i].wire = wires[i];
    matches[i].port = ports[i];
    RGB color = colors[color_i[i]];
    matches[i].color = colors[color_i[i]];
    strip.setPixelColor(wireToLED(wires[i]),color.r/2,color.g/2,color.b/2);
    strip.setPixelColor(portToLED(ports[i]),color.r/2,color.g/2,color.b/2);
    Serial.print("\nWire:");Serial.print(wires[i]);Serial.print("; Port:");Serial.print(ports[i]);Serial.print("; Color:(");
    Serial.print(color.r);Serial.print(",");Serial.print(color.g);Serial.print(",");Serial.print(color.b);Serial.print(")");
  }
  strip.show();
  Serial.println();
}



bool checkMatches(){
  bool allMatch = true;
  for(int i=0; i<4; i++){
    digitalWrite(matches[i].wire,LOW);
    delay(10);
    if(digitalRead(matches[i].port)){
      allMatch = false;
      strip.setPixelColor(wireToLED(matches[i].wire),matches[i].color.r/2,matches[i].color.g/2,matches[i].color.b/2);
      strip.setPixelColor(portToLED(matches[i].port),matches[i].color.r/2,matches[i].color.g/2,matches[i].color.b/2);
      
    }else{
      strip.setPixelColor(wireToLED(matches[i].wire),matches[i].color.r,matches[i].color.g,matches[i].color.b);
      strip.setPixelColor(portToLED(matches[i].port),matches[i].color.r,matches[i].color.g,matches[i].color.b);
    }
    digitalWrite(matches[i].wire,HIGH);
  }
  strip.show();
  return allMatch;
}


void winSequence(){
  delay(500);
  for(int i=0; i<3; i++){
    colorWipe(strip.Color(255,   0,   0), 50); // Red
    colorWipe(strip.Color(  0, 255,   0), 50); // Green
    colorWipe(strip.Color(  0,   0, 255), 50); // Blue
  }
}

void setup() {

  Serial.begin(9600);
  Serial.println("Starting");
  randomSeed(analogRead(0));
  for(int i=0; i<4; i++){
    Serial.print("Setting pins ");Serial.print(wires[i]);Serial.println(" to output");
    pinMode(wires[i],OUTPUT);
    digitalWrite(wires[i],HIGH);
    pinMode(ports[i],INPUT_PULLUP);
  }
  
  strip.begin();
  strip.setBrightness(75);
  strip.show(); // Initialize all pixels to 'off'
  winSequence();
  setNewPuzzle();
}



void loop() {
  if(checking){
    if((millis()-time) < checkTime){
      if(!checkMatches()){
        checking = false;
      }
    }else{
      winSequence();
      setNewPuzzle();
      checking = false;
    }
  }
  else if(checkMatches()){
    checking = true;
    time = millis();
    
    
  }
}






