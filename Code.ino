#include <ModbusRtu.h>
#include <SoftwareSerial.h>

#define BAUD_RATE 9600

// data array for modbus network sharing
uint16_t au16data[16];
uint8_t u8state;


Modbus master(0); // this is master and RS-232 or USB-FTDI via software serial

/**
 * This is an structe which contains a query to an 1slave device
 */
modbus_t telegram;

unsigned long u32wait;

//SoftwareSerial mySerial(3, 5);//Create a SoftwareSerial object so that we can use software serial. Search "software serial" on Arduino.cc to find out more details.
SoftwareSerial mySerial(12, 13);

void setup() {
  Serial.begin(9600);//use the hardware serial if you want to connect to your computer via usb cable, etc.
  master.begin( &mySerial, 9600 ); // begin the ModBus object. The first parameter is the address of your SoftwareSerial address. Do not forget the "&". 9600 means baud-rate at 9600
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u32wait = millis() + 1000;
  u8state = 0; 
}

void loop() {
  switch( u8state ) {
  case 0: 
    if (millis() > u32wait) u8state++; // wait state
    break;
  case 1: 
    telegram.u8id = 1; // slave address
    telegram.u8fct = 3; // function code (this one is registers read)
    telegram.u16RegAdd = 2; // start address in slave
    telegram.u16CoilsNo = 8; // number of elements (coils or registers) to read
    telegram.au16reg = au16data; // pointer to a memory array in the Arduino

    master.query( telegram ); // send query (only once)
    u8state++;
    break;
  case 2:
    master.poll(); // check incoming messages
    if (master.getState() == COM_IDLE) {
      u8state = 0;
      u32wait = millis() + 2000; 
        for(int i=0;i<8;i++){
            Serial.print(au16data[i]);//Or do something else!
            Serial.print('\t');//Or do something else!
        }
        Serial.println('\t');//Or do something else!
    }
    break;
  }
}
