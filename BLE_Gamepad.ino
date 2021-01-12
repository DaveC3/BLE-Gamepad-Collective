/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 * 
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_32 
 * 
 * Possible DPAD/HAT switch position values are: 
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, 
 * DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * 
 * bleGamepad.setAxes takes the following int16_t parameters for the Left/Right Thumb X/Y, char for the Left/Right Triggers, and hat switch position as above: 
 * (Left Thumb X, Left Thumb Y, Right Thumb X, Right Thumb Y, Left Trigger, Right Trigger, Hat switch position);
 */
 
#include <Bounce2.h>      // https://github.com/thomasfredericks/Bounce2
#include <BleGamepad.h>   // https://github.com/lemmingDev/ESP32-BLE-Gamepad

//MODE SWITCH GOES THERE!
byte pins[] = {13,12,14,27,15,0,4,16,17};
int previousButton1State[sizeof(pins)]; 
int currentButton1State[sizeof(pins)]; 
BleGamepad bleGamepad("Collective", "DLC", 100);
const int potPin = 34;                // Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int numberOfPotSamples = 5;     // Number of pot samples to take (to smooth the values)
const int delayBetweenSamples = 4;    // Delay in milliseconds between pot samples
const int delayBetweenHIDReports = 5; // Additional delay in milliseconds between HID reports

//************************************************************************************************
void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleGamepad.begin();

  for (int i = 0; i < sizeof(pins); i++)
  {
    pinMode(pins[i], INPUT_PULLUP);
    previousButton1State[i]== HIGH;
  }
} //End setup
//****************************************************************************************************
void loop() 
{
  if(bleGamepad.isConnected()) 
  {
    for(int i = 0 ; i < sizeof(pins) ; i += 1)
    {
      currentButton1State[i] = digitalRead(pins[i]);
      if (currentButton1State[i] != previousButton1State[i]){
        if ( currentButton1State[i] != HIGH){   
          bleGamepad.press(pow(2, i));
        } // end if high
        else
        {
          bleGamepad.release(pow(2, i));    
         }// end else
        } // end if state
          previousButton1State[i] = currentButton1State[i];
      } // end for loop

    int potValues[numberOfPotSamples];  // Array to store pot readings

    // Populate readings
    for(int i = 0 ; i < numberOfPotSamples ; i++)
    {
      potValues[i] = analogRead(potPin);
      delay(delayBetweenSamples);
    }

    int potValue = 0;   // Variable to store calculated pot reading average

    // Iteraate through the readings to sum the values
    for(int i = 0 ; i < numberOfPotSamples ; i++)
    {
      potValue += potValues[i];
    }
    // Calculate the average
    potValue = potValue / numberOfPotSamples;
    // Map analog reading from 0 ~ 4095 to 32737 ~ -32737 for use as an axis reading
    int adjustedValue = map(potValue, 0, 4095, 32737, -32737);
    bleGamepad.setAxes(adjustedValue, 0, 0, 0, 0, 0, DPAD_CENTERED);
    delay(delayBetweenHIDReports);

   } //end if connected
  } //end loop
