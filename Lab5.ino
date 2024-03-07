#include "IntervalTimer.h"

// 1 no interrupt
// 2 interrupt with time
// 3 reset with count
// 4 interrupt with button
int demo = 1;
int timeToInterrupt = 10200 * 1000; // count 51
int countToInterrupt = 33;

IntervalTimer myTimer;

const int TIMING = 1000;

int SER = 14; //data
int LATCH = 15; //RCLK
int CLOCK = 16; //SRCLK

int pshBttn = 19;
int ledPin = 13;

volatile int count[2] = {0, 0};

unsigned long lastTime = 0;

// decimal representations of flipped(common anode) binary numbers
int numbers[10] = {136, 235, 76, 73, 43, 25, 24, 203, 8, 9};

void setup() {
  Serial.begin(9600);
  // set pin modes
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(SER, OUTPUT);

  pinMode(pshBttn, INPUT);
  pinMode(ledPin, OUTPUT);

  // start interrupt/interval timer if in demo 2
  if (demo == 2) {
    myTimer.begin( restartCount, timeToInterrupt ); 
  }

  // attach an interrupt to the push button switch
  attachInterrupt(digitalPinToInterrupt(pshBttn), buttonInterrupt, FALLING);
  // attachInterrupt(digitalPinToInterrupt(pshBttn), lightOn, CHANGE);
}

void loop() {
  // check if interrupt count has reached if in demo 3
  if (demo == 3 && countToInterrupt == ((count[0] * 10) + count[1]) ){
    restartCount();
  }
  // change the count after the set timing interval
  if ((millis() - lastTime) >= TIMING){
    noInterrupts();
    countUp();
    lastTime = millis();
    interrupts();
  }
}

void countUp() {
  // set latch low until changes are made then return to high
  digitalWrite(LATCH, LOW);
  shiftOut(SER, CLOCK, MSBFIRST, numbers[count[1]]);
  shiftOut(SER, CLOCK, MSBFIRST, numbers[count[0]]);
  digitalWrite(LATCH, HIGH);
  // set numbers back to 0 if 9 was reached
  count[1]++;
  if (count[1] > 9) {
    count[1] = 0;
    count[0]++;
    if (count[0] > 9) {
      count[0] = 0;
    }
  }
}

void buttonInterrupt() {
  // check we are in demo 4
  if (demo == 4){
    restartCount();
  }
}

void restartCount() {
  // sets count to 0, and changes stored time to start recount immediately
  count[1] = 0;
  count[0] = 0;
  lastTime = millis();
}

void lightOn() {
  int buttonInput = digitalRead(pshBttn);
  digitalWrite(ledPin, !buttonInput);
}
