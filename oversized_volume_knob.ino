// Read the full tutorial at prusaprinters.org
// Karl Martin, 2019: Added functionality for adding some lights to the knob

#include <Adafruit_NeoPixel.h>  //  library for controlling LED strip

#include <ClickEncoder.h>  
#include <TimerOne.h>
#include <HID-Project.h>

#define ENCODER_CLK A0 // Change A0 to, for example, A5 if you want to use analog pin 5 instead
#define ENCODER_DT A1
#define ENCODER_SW A2

#define NUMPIXELS    6   // Number of pixels you are able to fit into the button 
#define PIN          2   // The led strip is controlled through this digital pin

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);  // set up the strip

ClickEncoder *encoder; // variable representing the rotary encoder
int16_t last, value; // variables for current and last rotation value
int sleepCounter = 0;
int sleepDelay = 1000; // number of iterations before showing off the cycle effect

void timerIsr() {
  encoder->service();
}

void setup() {
  Serial.begin(9600); // Opens the serial connection used for communication with the PC. 
  Consumer.begin(); // Initializes the media keyboard
  encoder = new ClickEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW); // Initializes the rotary encoder with the mentioned pins

  Timer1.initialize(1000); // Initializes the timer, which the rotary encoder uses to detect rotation
  Timer1.attachInterrupt(timerIsr); 
  last = -1;
  strip.begin();

} 

void loop() {  
  // If there is no action detected for a good while, show off the rainbow cycle
  if (sleepCounter >= sleepDelay)
  {
    sleepCounter = 0;
    rainbowCycle(2);
  }
  
  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  value += encoder->getValue();

  // This part of the code is responsible for the actions when you rotate the encoder
  if (value != last) { // New value is different than the last one, that means to encoder was rotated
    if(last<value) {// Detecting the direction of rotation
      Consumer.write(MEDIA_VOLUME_UP); // Replace this line to have a different function when rotating counter-clockwise
      //colorWipeRight(strip.Color(0, 255, 0), 2);
      colorWipeRight(strip.Color(128, 0, 0), 50);
      sleepCounter = 0;
    } else { 
      Consumer.write(MEDIA_VOLUME_DOWN); // Replace this line to have a different function when rotating clockwise
      //colorWipeLeft(strip.Color(128, 128, 128), 2);
      colorWipeLeft(strip.Color(0, 128, 0), 50);
      sleepCounter = 0;
    }
    
    last = value; // Refreshing the "last" varible for the next loop with the current value
    Serial.print("Encoder Value: "); // Text output of the rotation value used manily for debugging (open Tools - Serial Monitor to see it)
    Serial.println(value);
  }

  // This next part handles the rotary encoder BUTTON
  ClickEncoder::Button b = encoder->getButton(); // Asking the button for it's current state
  if (b != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
    //Serial.print("Button: "); 
    //#define VERBOSECASE(label) case label: Serial.println(#label); break;
    switch (b) {
      case ClickEncoder::Clicked: // Button was clicked once
        Consumer.write(MEDIA_PLAY_PAUSE); // Replace this line to have a different function when clicking button once
        sleepCounter = 0;
      break;      
      
      case ClickEncoder::DoubleClicked: // Button was double clicked
        rainbowCycle(10);
        //Consumer.write(MEDIA_NEXT); // Replace this line to have a different function when double-clicking
        sleepCounter = 0;
      break;      
    }
  }
  sleepCounter++;
  delay(10); // Wait 10 milliseconds, we definitely don't need to detect the rotary encoder any faster than that
  // The end of the loop() function, it will start again from the beggining (the beginging of the loop function, not the whole document)
}

// Fill the dots one after the other with a color
void colorWipeLeft(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
// Fill the dots one after the other with a color
void colorWipeRight(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(NUMPIXELS-i-1, c);
      strip.show();
      delay(wait);
  }
}

// Show colours of the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
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


/*
    This is just a long comment
    Here are some fun functions you can use to replace the default behaviour 
    Consumer.write(CONSUMER_BRIGHTNESS_UP);
    Consumer.write(CONSUMER_BRIGHTNESS_DOWN);
    Consumer.write(CONSUMER_BROWSER_HOME);
    Consumer.write(CONSUMER_SCREENSAVER);
    Consumer.write(HID_CONSUMER_AL_CALCULATOR); //launch calculator :)
    Consumer.write(HID_CONSUMER_AC_ZOOM_IN);
    Consumer.write(HID_CONSUMER_AC_SCROLL_UP);
    CONSUMER_SLEEP = 0x32,

    FULL LIST CAN BE FOUND HERE:
    https://github.com/NicoHood/HID/blob/master/src/HID-APIs/ConsumerAPI.h
*/
        
