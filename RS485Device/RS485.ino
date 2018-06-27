#define D0 16
#define D1 5 // I2C Bus SCL (clock)
#define D2 4 // I2C Bus SDA (data)
#define D3 0
#define D4 2 // Same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO 
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9 3 // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)

#define SSerialTxControl D6   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Id  D1

/********************************************************************/
// First we include the libraries
#include <OneWire.h>
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin D4 on the Arduino
#define ONE_WIRE_BUS D4
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature dallas(&oneWire);
/********************************************************************/




#include <SoftwareSerial.h>

SoftwareSerial mySerial(D7, D5); // RX, TX

#define CMD_READTEMPERATURE 2
#define CMD_DEVICE 0x80

// the setup function runs once when you press reset or power the board
void setup() {
  //
  Serial.begin(115200);

  // dallas temperature
  dallas.begin();

  pinMode(Id, INPUT);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);

  // set the data rate for the SoftwareSerial port
  mySerial.begin(57600);

  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, RS485Receive);
}

union Float {
  float value;
  unsigned char bytes[4];
};

// the loop function runs over and over again forever
void loop() {

  int idd = digitalRead(Id);

  bool cmdavailable = false;
  byte reqid;
  byte cmdid;
  byte cmdlen;
  Float temperature; 
  
  while(mySerial.available())
  {
    reqid = mySerial.read();
    Serial.println(reqid);
    if (reqid == idd)
    {
      cmdavailable = true;
    }
    cmdid = mySerial.read();
    Serial.println(reqid);    
    cmdlen = mySerial.read();
    Serial.println(cmdlen);    
    
    if (cmdavailable == true)
    {
      switch(cmdid) {
        case CMD_READTEMPERATURE:

          purgeMySerialRead(cmdlen);
          
          dallas.requestTemperatures();
          temperature.value = dallas.getTempCByIndex(0);
          Serial.print("CMD_READTEMPERATURE: ");
          Serial.println(temperature.value);
  
          digitalWrite(SSerialTxControl, RS485Transmit);
          mySerial.write(idd);
          mySerial.write(CMD_READTEMPERATURE + CMD_DEVICE);
          mySerial.write(4);
          mySerial.write(temperature.bytes, 4);
          delay(10);
          digitalWrite(SSerialTxControl, RS485Receive);
          
          break;
          
        default:
          purgeMySerialRead(cmdlen);
          break;
      }
    }
    else
    {
      purgeMySerialRead(cmdlen);
    }
  }
}

void purgeMySerialRead(int len)
{
  for(int i = 0; i<len; i++)
    mySerial.read();
}
