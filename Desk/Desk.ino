#include "config.h"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>

// Pin definitions of NodeMCU's builtin LEDs
const int pin_led_red = D0; // red
const int pin_led_blu = D4; // blue

BlynkTimer timer;

// these are set via Blynk virtual pins
volatile int num_vibrations = 0;
int vibrate_half_period_length_ms = 100;
volatile bool output_state = 0;

void vibrate()
{
  if(!num_vibrations) // vibration periods left?
    return;

  // change up/down
  output_state = !output_state;

  // Send it out
  digitalWrite(pin_led_red, LOW); // red led on

  BLYNK_LOG("num_vibrations = %d", num_vibrations);

  // decrement vibration number and schedule
  if(--num_vibrations)
  {
    // there is vibrations left, schedule next direction change
    timer.setTimeout(vibrate_half_period_length_ms, vibrate);
  } else {
    // no vibration left, stop after this half period
    timer.setTimeout(vibrate_half_period_length_ms, turn_off);
  }
}

void turn_off()
{
  digitalWrite(pin_led_red, HIGH); // red led off
}

// Write a value to virtual pin V1 to make Desk motor
// change directions <value> times.
// The vibration duration in ms is value(V1) * value(V2).
BLYNK_WRITE(V1)
{
  num_vibrations = param.asInt();
  vibrate();
}

// Value on virtual pin V2 is the half period of direction changes,
// i.e. vibration frequency in Hz is 500/<value>. Unit is ms.
BLYNK_WRITE(V2)
{
  vibrate_half_period_length_ms = param.asInt();
  BLYNK_LOG("vibrate_half_period_length_ms = %d", vibrate_half_period_length_ms);
}

void setup()
{
  pinMode(pin_led_red, OUTPUT);
  pinMode(pin_led_blu, OUTPUT);

  turn_off();
  
  Serial.begin(115200);

  Blynk.begin(CONFIG_AUTH, CONFIG_SSID, CONFIG_PASS);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  digitalWrite(pin_led_blu, !Blynk.connected()); // blue led shows connection state (on is low)
}

