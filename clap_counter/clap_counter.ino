#include <RBD_Light.h> // Used for lights
/*
 * Arduino Clap/Noise Counter
 * Joshua Anderson
 * Description: Counts claps, or other noises.
 * Uses: Clap lights, clap door lock, etc...
 * Note: Unreliable at times, so not recommended for security.
 */
const int micIn = 7; // Input pin for mic module
const int ledPin = 12; // Output pin for the light
bool ledState = false; // The current state of the light
bool relayState = true; // The current state of the relay
const int relayPin = 8; // Output pin for the relay
const int spaced = 100; // Time required between claps, in ticks not ms
int clapCount = 0; // The clap count
int submitTime = 0; // Time since last clap, used for submitting after time
const int notifyPin = 13; // Output pin for notification light
RBD::Light light(ledPin); // Set up the light LED
RBD::Light notifyLed(notifyPin); // Set up the notify LED
void setup() {
  pinMode(micIn, INPUT); // Set up the mic
  pinMode(relayPin, OUTPUT); // Set up the relay
  Serial.begin(9600); // Start up Serial
  Serial.println("Initialized."); // Tell Serial it's on and ready
}

void loop() {
  notifyLed.update(); // Update the LEDs
  light.update();
  if(Serial.available()){
    clapEval(Serial.parseInt()); // Receive manual clap counts from Serial
  }
  if(digitalRead(micIn)==false){ // If there's a clap (false means on)
    submitTime=0; // Reset submit time
    clapCount++; // Add one clap to count
    notify(1); // Blink the notify LED
    Serial.print("Clap detected. Total so far: "); // Tell Serial the clap data
    Serial.println(clapCount);
    delay(spaced); // Wait to listen
  }
  if(submitTime>=32000 && clapCount>0) { // If ready to submit
    Serial.println(clapCount); // Tell Serial the clap count
    clapEval(clapCount); // Evaluate what to do
    clapCount=0; // Reset the count
  }
  submitTime++; // Increment time
}
void clapEval( int num ) { // Clap count evaluation - What to do...
  notify(num); // Tell user how many claps were there
  switch(num) { // Pick one based on clap count
      case 1:
        Serial.println("Lights"); // Tell Serial what we're doing
        if(light.isOn()){ // If the light is on
          light.off(); // Turn it off
        }else{ // Otherwise
          light.on(); // Turn it off
        }
      break;
      case 2:
        Serial.println("Other lights");
      break;
      case 3:
        Serial.println("Relay toggle");
        digitalWrite(relayPin, !relayState); // Toggle actual relay
        relayState = !relayState; // Toggle relay variable
        delay(70); // So it won't trigger a clap
      break;
      default:
        Serial.println("Uhh..."); // No entry for this many claps
      break;
    }
}
void notify(int count) { // Blink however many times
  notifyLed.blink(50,50,count);
}

