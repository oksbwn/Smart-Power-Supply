#include <ModbusRtu.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//#define DEBUG
#define BAUD_RATE 9600

#define OUPUT_LED 5
#define INPUT_SWITCH 4
const char * ssid = "home_wg";
const char * password = "omisoksbwn";

IPAddress ip(192, 168, 0, 64);
IPAddress gateway_dns(192, 168, 0, 1);

// data array for modbus network sharing
uint16_t au16data[16];
uint8_t u8state;


Modbus master(0); // this is master and RS-232 or USB-FTDI via software serial

/**
   This is an structe which contains a query to an 1slave device
*/
modbus_t telegram;

unsigned long u32wait;

//SoftwareSerial mySerial(3, 5);//Create a SoftwareSerial object so that we can use software serial. Search "software serial" on Arduino.cc to find out more details.
SoftwareSerial mySerial(12, 13);


void setup_wifi() {

  delay(10);

#if defined(DEBUG)
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
#endif

  WiFi.config(ip, gateway_dns, gateway_dns);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

#if defined(DEBUG)
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void setup() {
  pinMode(OUPUT_LED, OUTPUT);
  pinMode(INPUT_SWITCH, INPUT_PULLUP);
  Serial.begin(9600);//use the hardware serial if you want to connect to your computer via usb cable, etc.
  master.begin( &mySerial, 9600 ); // begin the ModBus object. The first parameter is the address of your SoftwareSerial address. Do not forget the "&". 9600 means baud-rate at 9600
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u32wait = millis() + 1000;
  u8state = 0;
  setup_wifi();
}
char publishValue[100];
int session;
void loop() {
  session=millis();
  while (!digitalRead(INPUT_SWITCH)) {
    switch ( u8state ) {
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
          u32wait = millis() + 1000;
          if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

            HTTPClient http;  //Declare an object of class HTTPClient
            sprintf(publishValue, "http://192.168.0.7/smps/saveData.php?session=%d&iv=%d&ov=%d&oa=%d&op=%d&os=%d", session, au16data[3], au16data[0], au16data[1], au16data[2], au16data[7]);
            http.begin(publishValue);  //Specify request destination
            int httpCode = http.GET();                                                                  //Send the request
            if (httpCode > 0) { //Check the returning code
              if (httpCode == 200)
                digitalWrite(OUPUT_LED, HIGH);
#if defined(DEBUG)
              String payload = http.getString();   //Get the request response payload
              Serial.println(payload);
#endif//Print the response payload
            }
            http.end();   //Close connection
          } else
            setup_wifi();

#if defined(DEBUG)
          for (int i = 0; i < 8; i++) {
            Serial.print(au16data[i]);//Or do something else!
            Serial.print('\t');//Or do something else!
          }
          Serial.println('\t');//Or do something else!
#endif
        }
        break;
    }
    digitalWrite(OUPUT_LED, LOW);
  }
}
