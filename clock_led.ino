// This #include statement was automatically added by the Particle IDE.
#include <MQTT.h>

// This #include statement was automatically added by the Particle IDE.
#include <math.h>
#include <neopixel.h>

#define PIN D2
#define NUM_LEDS 150
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip(NUM_LEDS, PIN, PIXEL_TYPE);

MQTT client("192.168.1.20", 1883, callback);

void setup() {
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

    // connect to the server
    client.connect("photon client");
    
    // publish/subscribe
    if (client.isConnected()) {
        client.publish("test/rpi_1","photon connected");
        client.subscribe("test/rpi_1");
    }
}

void loop() {
    //meteorRain(0xff,0xff,0xff,10, 64, true, 20);
    if (client.isConnected())
        client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    
    

    if (!strcmp(p, "RED")){
        meteorRain(0xff,0x0,0x0,10, 64, true, 20);
    }
    else if (!strcmp(p, "GREEN")){
        meteorRain(0x0,0xff,0x0,10, 64, true, 20);
    }
    else if (!strcmp(p, "BLUE")){
        meteorRain(0x0,0x00,0xff,10, 64, true, 20);
    }
    else{
        meteorRain(0xff,0xff,0xff,10, 64, true, 20);
    }
        
    delay(1000);
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
