/*
Program Briefing :
----------------
This program is made for Arduino Uno with L293D Motor shield, Ultrasonic sensor and a DC Pump designed with the purpose to serve as an automatic soap dispensor.
With the intent to save some power, Arduino Uno periodically goes to sleep for 1 second after which it checks for any objects using it's ultrasonic sensor.
If there is something, It turns on the pump for half a second and then turns it off.

Configuration :
-------------
  Ultrasonic :
    1) TRIG - 10
    2) ECHO - 9
  
  Motor Shield port :
    1) Port - 1 also known as M1

Program flow :
------------

1) Create and initialize Ultrasonic and Motor Shield objects.
2) Intialize and Configure the WatchDog Timer for an interrupt every 1 second.
3) Go to sleep.
4) At Watchdog interrupt, Wake up and get reading from ultrasonic sensor.
5) If the reading is smaller than the defined THRESHOLD then start the pump for 0.5 seconds.
6) Repeat Steps 3, 4, 5.
*/

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <Ultrasonic.h>
#include <AFMotor.h>

#define TURN_OFF_BUILTIN_LED(void) pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, LOW);
#define THRESHOLD 8

// Creating Control objects. (Ultrasonic ---- sensor, DC Pump ---- pump)
Ultrasonic sensor(10, 9);
AF_DCMotor pump(1);
void setup() {
  ADCSRA &= ~(1 << ADEN); // Disabling the ADCs since we are not using them.
  TURN_OFF_BUILTIN_LED();

  // Configuring pump.
  pump.setSpeed(255);  // Configured at MAX SPEED (255) but can be configured with lower speed to decrease power usage.
  pump.run(RELEASE);

  // Set up a Watchdog Timer.
  cli();
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDP2) | (1 << WDP1);  // Watchdog Timer set to 1 Sec interrupt.
  WDTCSR |= (1 << WDIE);
  sei();
}

void loop() {
  // Put the arduino into deep sleep mode. (Essential for saving power.)
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();

  // Upon Interrupt, the code will resume from here.
  int distance = sensor.read();  // get sensor reading. distance is in centimeters.
  if (distance <= THRESHOLD) {   // check sensor reading
    pump.run(FORWARD);
    delay(500);
    pump.run(RELEASE);
  }
}

ISR(WDT_vect) {}
