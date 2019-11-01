#include <ModbusRtu.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

//#define DEBUG
#define BAUD_RATE 9600

#define OUPUT_LED 5
#define INPUT_SWITCH 4
#define RX_PIN 12
#define TX_PIN 13

String ip_address = "91";
const char * ssid = "home_wg";
const char * password = "omisoksbwn";

IPAddress ip(192, 168, 0, ip_address.toInt());
IPAddress gateway_dns(192, 168, 0, 1);
const char *mqtt_server = "192.168.0.7";
// data array for modbus network sharing
uint16_t deviceData[16];
uint8_t u8state;
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
Modbus master(0); // this is master and RS-232 or USB-FTDI via software serial

/**
   This is an structe which contains a query to an 1slave device
*/
modbus_t power_device;

unsigned long u32wait;

SoftwareSerial mySerial(RX_PIN, TX_PIN);


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
  Serial.begin(115200);//use the hardware serial if you want to connect to your computer via usb cable, etc.
  master.begin( &mySerial, 9600 ); // begin the ModBus object. The first parameter is the address of your SoftwareSerial address. Do not forget the "&". 9600 means baud-rate at 9600
  master.setTimeOut( 2000 ); // if there is no answer in 2000 ms, roll over
  u32wait = millis() + 1000;
  u8state = 0;
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}
char publishValue[100];
int session;

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "lens_5kchta2dWhdRS2AQz0TeTuhplPE";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str(), "openhabian", "om"))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
void loop() {
      if (!client.connected())
    {
        reconnect();
    }
  while (!digitalRead(INPUT_SWITCH)) {
    switch ( u8state ) {
      case 0:
        if (millis() > u32wait) u8state++; // wait state
        break;
      case 1:
        power_device.u8id = 1; // slave address
        power_device.u8fct = 3; // function code (this one is registers read)
        power_device.u16RegAdd = 2; // start address in slave
        power_device.u16CoilsNo = 8; // number of elements (coils or registers) to read
        power_device.au16reg = deviceData; // pointer to a memory array in the Arduino

        master.query( power_device ); // send query (only once)
        u8state++;
        break;
      case 2:
        master.poll(); // check incoming messages
        if (master.getState() == COM_IDLE) {
          u8state = 0;
          u32wait = millis() + 5000;
          if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
            sprintf(publishValue, "{\"iv\":%d,\"ov\":%d,\"oa\":%d,\"op\":%d,\"os\":%d}",deviceData[3], deviceData[0], deviceData[1], deviceData[2], deviceData[7]);
            client.publish(getCharArray("home/esp8266/" + ip_address + "/sensor"), getCharArray(publishValue));
            
          } else
            setup_wifi();

#if defined(DEBUG)
          for (int i = 0; i < 8; i++) {
            Serial.print(deviceData[i]);//Or do something else!
            Serial.print('\t');//Or do something else!
          }
          Serial.println('\t');//Or do something else!
#endif
        }
        break;
    }
    digitalWrite(OUPUT_LED, LOW);
  }
  client.loop();
}
char *getCharArray(String text)
{
    char *subTopicArray = const_cast<char *>(text.c_str());
    return subTopicArray;
}
