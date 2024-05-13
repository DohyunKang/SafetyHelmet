#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

#include <DFRobot_CCS811.h>
#include <DFRobot_BME280.h>

#define LED_STAT 13
const int LED_TX = PIN_LED_TXL;
const int LED_RX = PIN_LED_RXL;

#define SEA_LEVEL_PRESSURE 1015.0f

typedef DFRobot_BME280_IIC BME;
BME bme(&Wire, 0x76);

RH_RF95 rf95(12,6);
DFRobot_CCS811 CCS811;

int packetCounter = 0;
long timeSinceLastPacket = 0 ;

float frequency = 921.2;      //Broadcast Frequency


void printLastOperateStatus(BME::eStatus_t eStatus){
  switch(eStatus) {
  case BME::eStatusOK:                    SerialUSB.println("everything ok");       break;
  case BME::eStatusErr:                   SerialUSB.println("unknow error");        break;
  case BME::eStatusErrDeviceNotDetected:  SerialUSB.println("device not detected"); break;
  case BME::eStatusErrParameter:          SerialUSB.println("parameter error");     break;
  default:                                SerialUSB.println("unknow status");       break;
}
}


void setup() 
{
  pinMode(LED_STAT, OUTPUT);
  pinMode(LED_RX, OUTPUT);
  pinMode(LED_TX, OUTPUT);
  SerialUSB.begin(9600);
  
  while(!SerialUSB);                //USB_Mode
  SerialUSB.println("Client ON");   //USB_Mode 
  
  while(rf95.init() == false){
    SerialUSB.println("Radio Init Failed");
    delay(1000);
  }

  SerialUSB.println("Transmitter up!"); 
  digitalWrite(LED_STAT, HIGH);
  digitalWrite(LED_RX, LOW);
  digitalWrite(LED_TX, LOW);
  delay(500);
  digitalWrite(LED_STAT, LOW);
  digitalWrite(LED_RX, HIGH);
  digitalWrite(LED_TX, HIGH);
  delay(500);

  while(CCS811.begin() != 0){
      SerialUSB.println("failed to init chip, please check if the chip connection is fine");
      delay(1000);
  }
  bme.reset();
  SerialUSB.println("BME read data Test");
  while(bme.begin() != BME::eStatusOK) {
    SerialUSB.println("bme begin faild");
    printLastOperateStatus(bme.lastOperateStatus);
    delay(2000);
  }
  SerialUSB.println("BME begin Success");
  delay(1000);

  rf95.setFrequency(frequency);
  rf95.setTxPower(23, false);
}

void loop() {
  if (CCS811.checkDataReady() == true) {
    SerialUSB.print("CO2: ");
    SerialUSB.print(CCS811.getCO2PPM());
    SerialUSB.print("ppm, TVOC: ");
    SerialUSB.print(CCS811.getTVOCPPB());
    SerialUSB.println("ppb");
  } else {
    SerialUSB.println("Data is not ready!");
  }
  CCS811.writeBaseLine(0x8483);
  uint8_t CO2PPM = CCS811.getCO2PPM();
  uint8_t TVOCPPB = CCS811.getTVOCPPB();
  float temp = bme.getTemperature();
  uint8_t press = bme.getPressure();
  float alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
  float humi = bme.getHumidity();

  SerialUSB.print("temperature (unit Celsius): ");
  SerialUSB.println(temp);
  SerialUSB.print("pressure (unit pa):         ");
  SerialUSB.println(press);
  SerialUSB.print("altitude (unit meter):      ");
  SerialUSB.println(alti);
  SerialUSB.print("humidity (unit percent):    ");
  SerialUSB.println(humi);

  char toSend[100];
  sprintf(toSend, "%d %d %f %d %f %f", CO2PPM, TVOCPPB, temp, press, alti, humi);
  rf95.send((uint8_t *)toSend, sizeof(toSend));

  delay(1000);
}

/*void loop() {
  if(CCS811.checkDataReady() == true){
    SerialUSB.print("CO2: ");
    SerialUSB.print(CCS811.getCO2PPM());
    SerialUSB.print("ppm, TVOC: ");
    SerialUSB.print(CCS811.getTVOCPPB());
    SerialUSB.println("ppb");
    } 
  else {
      SerialUSB.println("Data is not ready!");
  }
  CCS811.writeBaseLine(0x8483);
  uint8_t CO2PPM = CCS811.getCO2PPM();
  uint8_t TVOCPPB = CCS811.getTVOCPPB();
  float   temp = bme.getTemperature();
  uint8_t    press = bme.getPressure();
  float   alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
  float   humi = bme.getHumidity();

  SerialUSB.print("temperature (unit Celsius): "); SerialUSB.println(temp);
  SerialUSB.print("pressure (unit pa):         "); SerialUSB.println(press);
  SerialUSB.print("altitude (unit meter):      "); SerialUSB.println(alti);
  SerialUSB.print("humidity (unit percent):    "); SerialUSB.println(humi);



  
  uint8_t toSend[100];
  sprintf(toSend, "%d %d %f %d %f %f", CO2PPM, TVOCPPB, temp, press, alti, humi );
  rf95.send(toSend, sizeof(toSend));

  delay(1000);
}*/
