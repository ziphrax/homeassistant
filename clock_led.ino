//REF: https://github.com/jackw01/led-ring-clock/blob/master/led-ring-clock.ino
#include <math.h>
#include <MQTT.h>
#include <neopixel.h>

#define PIN D2
#define NUM_LEDS 76
#define PIXEL_TYPE WS2812B


STARTUP(WiFi.selectAntenna(ANT_EXTERNAL));

Adafruit_NeoPixel strip(NUM_LEDS, PIN, PIXEL_TYPE);

MQTT client("192.168.0.19", 1883, callback);

int hourGlowWidth = 2;
int secondGlowWidth = 1;
int minuteGlowWidth = 2;

void setup() {
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

    // connect to the server
    client.connect("photon client");
    
    // publish/subscribe
    if (client.isConnected()) {
        client.publish("home","clock connected");
        client.subscribe("home/clock");
    }
}

void loop() {
    
    if (client.isConnected()){
        client.loop();
    }
    
    drawClock();
}

void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    
    String red = "red";
    String green = "green";
    String blue = "blue";
    

    if (red.equalsIgnoreCase(p)){
        meteorRain(0xff,0x0,0x0,10, 64, true, 20);
    }
    else if (green.equalsIgnoreCase(p)){
        meteorRain(0x0,0xff,0x0,10, 64, true, 20);
    }
    else if (blue.equalsIgnoreCase(p)){
        meteorRain(0x0,0x00,0xff,10, 64, true, 20);
    }
    else{
        Strobe(0x33, 0x33, 0x33, 5, 500, 20);
    }
        
    delay(1000);
    
    Particle.process();
}

void drawClock(){
    int hour = hourPosition();
    int min = minPosition();
    int sec = secondPosition();
    
    setAll(0,0,0);
    
    for(float i = hour - hourGlowWidth; i<= hour+ hourGlowWidth; i++){
        setPixel(i, 0xff,0xff,0xff);    
    }
    
    for (float i = min - minuteGlowWidth; i <= min + minuteGlowWidth; i++) {
		setPixel(i, 0x00,0x00,0xff); 
	}
    
    for (float i = sec - secondGlowWidth; i <= sec + secondGlowWidth; i++) {
		setPixel(i, 0xff,0x00,0xff); 
	}
	
	showStrip();
	Particle.process();
}

float hourPosition() {
	int hour;
	int currentHour;
	currentHour = Time.hour();
	
	int currentMin;
	currentMin = Time.minute();
	
	if (currentHour > 12) hour = (currentHour - 12) * (NUM_LEDS / 12);
    else hour = currentHour * (NUM_LEDS / 12);
	return hour + mapFloat(currentMin, 0.0, 59.0, 0.0, (NUM_LEDS / 12.0) - 1.0);
	
}

float minPosition(){
    return mapFloat((float)Time.minute() + ((1.0 / 60.0) * (float)Time.second()), 0.0, 59.0, 0.0, (float)NUM_LEDS);
}


float secondPosition() {
	return mapFloat(Time.second(), 0.0, 60.0, 0.0, (float)NUM_LEDS);
}

float mapFloat(float x, float inMin, float inMax, float outMin, float outMax) {
	return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void Colours(int SpeedDelay){
    for (int color=0; color<255; color++) {
      for (int i=0; i<NUM_LEDS; i++) {
        strip.setPixelColor(i, Wheel(color));
       }
    strip.show();
    delay(SpeedDelay);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
  
  for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) {
    
    
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
    
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        setPixel(i-j, red, green, blue);
      } 
    }
   
    showStrip();
    delay(SpeedDelay);
  }
}

void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++) {
    setAll(red,green,blue);
    showStrip();
    delay(FlashDelay);
    setAll(0,0,0);
    showStrip();
    delay(FlashDelay);
  }
 
 delay(EndPause);
}

void fadeToBlack(int ledNo, byte fadeValue) {
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
    
    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
    
    strip.setPixelColor(ledNo, r,g,b);
}

void showStrip() {
   strip.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
