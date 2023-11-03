/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)
 */
 
#include <SPI.h>
#include "printf.h"
#include "RF24.h"       

// can't use pin 1 as it is the hardware serial port, it will go high and low when you are printing messages
#define in1 2
#define in2 3
#define in3 4
#define in4 5

#define CE_PIN 7
#define CSN_PIN 8
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
 
// Let these addresses be used for the pair
uint8_t address[][6] = { "1Nods", "2Nods" };
// It is very helpful to think of an address as a path instead of as
// an identifying device destination
 
// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit
bool radioNumber = 1;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit
 
// Used to control whether this node is sending or receiving
bool role = false;  // true = TX role, false = RX role
 
// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
float payload = 0.0;
 
void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  digitalWrite(in3, 0);
  digitalWrite(in4, 0);
  Serial.begin(115200);

 
  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
 
  // print example's introductory prompt
  // Serial.println(F("RF24/examples/GettingStarted"));
 
  // To set the radioNumber via the Serial monitor on startup

  radioNumber = 0;

 
  // role variable is hardcoded to RX behavior, inform the user of this
 
  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.
 
  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(payload));  // float datatype occupies 4 bytes
 
  // set the TX address of the RX node into the TX pipe
  radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0
 
  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1
 
  // additional setup specific to the node's role
  radio.startListening();  // put radio in RX mode

 
  // For debugging info
  printf_begin();             // needed only once for printing details
  radio.printDetails();       // (smaller) function that prints raw register values
  radio.printPrettyDetails(); // (larger) function that prints human readable data
 
}  // setup
 
void loop() {
  radio.startListening();
  uint8_t pipe;
  if (radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
    uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
    radio.read(&payload, bytes);             // fetch payload from FIFO
    Serial.print(F("Received "));
    Serial.print(bytes);  // print the size of the payload
    Serial.print(F(" bytes on pipe "));
    Serial.print(pipe);  // print the pipe number
    Serial.print(F(": "));
    Serial.println(payload);  // print the payload's value
    int input = (int)payload;
    switch (input) {
      case 'w':
      case 'W':
      // W (forward)
          digitalWrite(in1, 1);
          digitalWrite(in2, 0);
          digitalWrite(in3, 1);
          digitalWrite(in4, 0);
          break; // missed all the breaks, so it was always the last case
      case 's':
      case 'S':
      // S (stop)
          digitalWrite(in1, 0);
          digitalWrite(in2, 0);
          digitalWrite(in3, 0);
          digitalWrite(in4, 0);
          break;
      case 'x':
      case 'X':
      // X (back)
          digitalWrite(in1, 0);
          digitalWrite(in2, 1);
          digitalWrite(in3, 0);
          digitalWrite(in4, 1);
          break;
      case 'd':
      case 'D':
      // D (right)
          digitalWrite(in1, 1);
          digitalWrite(in2, 0);
          digitalWrite(in3, 0);
          digitalWrite(in4, 0);
          break;
      case 'a':
      case 'A':
      // A (right)
          digitalWrite(in1, 0);
          digitalWrite(in2, 0);
          digitalWrite(in3, 1);
          digitalWrite(in4, 0);
          break;
    }
  }
}
 
