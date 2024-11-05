#include <StandardCplusplus.h>
#include <system_configuration.h>
#include <unwind-cxx.h>
#include <utility.h>

/*
  "black box" slave
  waits for a query character, retransmits all data between that and the end character. THen, when it sees the end character, it appends its own state to 
  the retransmit, followed by the end char

*/




// macros for hw input pins

#define SW0 D10
#define SW1 D1
#define SW2 D2
#define SW3 D3
#define SW4 D4
#define SW5 D5
#define B1 D8
#define B2 D9



uint inputs[] {
  SW0,
  SW1,
  SW2,
  SW3,
  SW4,
  SW5,
  B1,
  B2
};
#define SIZE 8
int states[SIZE];

template <typename T>
uint len_of(T list){
  return sizeof(list); // / sizeof(T);
}

void setup() {
  // initalize all as INPUT_PULLUP
  for (auto i : inputs){
    pinMode(i, INPUT_PULLUP);
  }

  for (int n  = 0; n < SIZE; n++) {
    auto i = inputs[n];
    int v = digitalRead(i);
    states[n] = v;
  }
  
  Serial.begin(9600);
  Serial.println("initializing");
}

void loop() {
  int n = 0;

  bool reprint = false;
  for (int n =0 ; n < SIZE; n++) {
    auto i = inputs[n];
    int v = digitalRead(i);
    if (states[n] != v){
      reprint = true;
    }
    states[n] = v;
  }

  if (reprint){
    for (int n =0 ; n < SIZE; n++){
      Serial.print(states[n]);
      if ((n+1) % 4 == 0){
        Serial.print('.');
      }
    }
    Serial.println();
  }
  delay(100);
}
