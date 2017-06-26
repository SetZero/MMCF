// CAN Receive Example
//

#include <mcp_can.h>
#include <SPI.h>
#include "esp32-hal-ledc.h"

#define DO_DRAW false
#define DONT_DRAW true

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                        // Array to store serial string
bool servoState = DONT_DRAW;
bool changeState = false;

#define CAN0_INT 15                              // Set INT to pin 2
MCP_CAN CAN0(2);                               // Set CS to pin 10


void setup()
{
  Serial.begin(115200);
  
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);
  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
  ledcSetup(1, 50, 16); // channel 1, 50 Hz, 16-bit depth
  ledcAttachPin(27, 1);   // GPIO 22 on channel 1
}

void getData() {
   CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
}

void handleData() {
  if(rxId == 0x01) {
    switch(rxBuf[0]) {
      case 0x01: {
        byte sending[8]= {0}; 
        byte sndStat = CAN0.sendMsgBuf(0x0B, 0, 8, sending);
        if(sndStat == CAN_OK){ 
          Serial.print("Send Type");  
        }
        break;
      }
      case 0x02: {
          byte got[4] = {rxBuf[1], rxBuf[2], rxBuf[3], rxBuf[4]};
          float x = *(float *)&got; //byte to float convertion
          Serial.printf("Float: %f", x);
          if(x==5)x=4;
          if(x >= 0.1) {
            servoState = DONT_DRAW; 
            Serial.println(" -> Stop Draw!");
          } else {
            servoState = DO_DRAW;
            Serial.println(" -> Start Draw!");
          }
          break;
      }
      default:
        Serial.println("Unknown Command!");
      
    }
  }
}

void loop()
{
  if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
  {
    getData();
    handleData();
    delay(5);
    
    /*if(!servoState) {
      ledcWrite(1, 6300); 
      Serial.println(" ->  Drawing!");
    } else {
      ledcWrite(1, 2300); 
      Serial.println(" ->  NOT Drawing!");
    }*/
    if(!servoState) {
       /*for (int i=3300 ; i < 6500 ; i=i+100){
    ledcWrite(1, i);       // sweep the servo
    delay(100);
       }*/
       ledcWrite(1, 6500);
       delay(100);
   } else {
     ledcWrite(1, 3300); 
       delay(100);
   }
  }

}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
