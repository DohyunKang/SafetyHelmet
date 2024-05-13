#include <SPI.h>
#include <RH_RF95.h>
#include <Wire.h>

#include <DFRobot_CCS811.h>
#include <DFRobot_BME280.h>
#include <SparkFunLSM6DS3.h>
// 헤더 추가

#define LED_STAT 13
#define DEVICE_ID 06
// 핀 배정


const int LED_TX = PIN_LED_TXL;
const int LED_RX = PIN_LED_RXL;
const int buzzerPin = 9;
// 정수 선언

unsigned long timestamp_Acc, timestamp_Env, timestamp_now, timestamp_Tone, timestamp_Fall;
int tone_flag = 0;
int StatusCode = 1;
int freefall_flag = 0;
//부저 플래그 선언

float X2;
float Y2;
float Z2;

#define SEA_LEVEL_PRESSURE 1015.0f

LSM6DS3 myIMU( I2C_MODE, 0x6B );
LSM6DS3 SensorOne( I2C_MODE, 0x6A );
LSM6DS3 SensorTwo( I2C_MODE, 0x6B );

typedef DFRobot_BME280_IIC BME;
BME bme(&Wire, 0x76);
//wire 핀 주소 지정

RH_RF95 rf95(12,6);
DFRobot_CCS811 CCS811;

int packetCounter = 0;
long timeSinceLastPacket = 0 ;

float frequency = 921.2;      //Broadcast Frequency

float AccelMean;

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
  pinMode(buzzerPin, OUTPUT);

  SerialUSB.begin(9600);
  
  /*
  while(!SerialUSB);                //USB_Mode
  SerialUSB.println("Client ON");   //USB_Mode 
  */
  
  while(rf95.init() == false){
    SerialUSB.println("Radio Init Failed");
    delay(1000);
  }
  //통신 초기화

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

  if( myIMU.beginCore() != 0 ){
    Serial.print("Error at Gyro().\n");
  }
  else{
    Serial.print("\n Gyro() passed.\n");
  }
  //gyro 센서 점검
  SensorTwo.begin();
	uint8_t errorAccumulator = 0;   //Error accumulation variable
  uint8_t dataToWrite = 0;        //Temporary variable

  //Setup the accelerometer******************************
  dataToWrite = 0; //Start Fresh!
  /*
  dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
  dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
  dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;

  errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);
  errorAccumulator += myIMU.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);

 	errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_WAKE_UP_DUR, 0x00 );        // Write 00h into WAKE_UP_DUR 

  // Set FF threshold (FF_THS[2:0] = 011b)
  // Set six samples event duration (FF_DUR[5:0] = 000110b)
  // Write 33h into FREE_FALL 
  errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_FREE_FALL, 0x33);

  // FF interrupt driven to INT1 pin
  // Write 10h into MD1_CFG
	errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_MD1_CFG, 0x10 );

  // Also route to INT2 pin
  // Write 10h into MD1_CFG
	errorAccumulator += myIMU.writeRegister( LSM6DS3_ACC_GYRO_MD2_CFG, 0x10 );

  // Latch interrupt
  // Write 01h into TAP_CFG 
  errorAccumulator += myIMU.writeRegister(LSM6DS3_ACC_GYRO_TAP_CFG1, 0x01);

  if( errorAccumulator ){
	  Serial.println("Problem configuring the device.");
  }
  else{
	  Serial.println("Device O.K.");
  }	
  */
}

void msg(int id, int status, float val1, float val2, float val3, float val4, float val5, float val6){
  digitalWrite(LED_TX, LOW);
  char toSend[70];
  sprintf(toSend, "%02d_%1d_%07.2f_%07.2f_%07.2f_%07.2f_%07.2f_%07.2f",id, status, val1, val2, val3, val4, val5, val6);
  rf95.send((uint8_t *)toSend, sizeof(toSend));
  digitalWrite(LED_TX, HIGH);
}
//출력 메시지 함수



void loop() {
  timestamp_now = millis();
  if (timestamp_now - timestamp_Env >= 1000 ){

    if(CCS811.checkDataReady() == true){
      SerialUSB.println();
      SerialUSB.println("--------------------");
      SerialUSB.print("CO2  (unit ppm):            "); SerialUSB.println(CCS811.getCO2PPM());
      SerialUSB.print("TVOC (unit ppb):            "); SerialUSB.println(CCS811.getTVOCPPB());
      } 
    else {
        SerialUSB.println("Data is not ready!");
    }
    CCS811.writeBaseLine(0x8483);
    float CO2PPM  = CCS811.getCO2PPM();
    float TVOCPPB = CCS811.getTVOCPPB();
    float temp    = bme.getTemperature();
    float press   = bme.getPressure() / 100.00;
    float alti    = bme.calAltitude(SEA_LEVEL_PRESSURE, press * 100);
    float humi    = bme.getHumidity();

    SerialUSB.print("temperature (unit Celsius): ");
    SerialUSB.println(temp);
    SerialUSB.print("pressure (unit pa):         ");
    SerialUSB.println(press);
    SerialUSB.print("altitude (unit meter):      ");
    SerialUSB.println(alti);
    SerialUSB.print("humidity (unit percent):    ");
    SerialUSB.println(humi);
    SerialUSB.print("StatusCode :                ");
    SerialUSB.println(StatusCode);
    SerialUSB.print("AccelMean :                 ");
    SerialUSB.println(AccelMean);
    msg(DEVICE_ID, StatusCode, CO2PPM, TVOCPPB, temp, press, alti, humi);
    timestamp_Env = millis();
  }
  if(timestamp_now - timestamp_Acc >= 50){
    X2 = SensorTwo.readFloatAccelX();
    Y2 = SensorTwo.readFloatAccelY();
    Z2 = SensorTwo.readFloatAccelZ();

    uint8_t readDataByte = 0;


    AccelMean = (AccelMean + sqrt(X2*X2 + Y2*Y2 + Z2*Z2))/2;

    //Read the wake-up source register
    myIMU.readRegister(&readDataByte, LSM6DS3_ACC_GYRO_WAKE_UP_SRC);
    //Mask off the FF_IA bit for free-fall detection
    readDataByte &= 0x20;
    //Check for free-fall
    if(readDataByte){
      freefall_flag = 1;
      timestamp_Fall = millis();
      AccelMean = 1;
    }
    if((freefall_flag == 1) && (timestamp_now - timestamp_Fall > 3000)){
      if (AccelMean > 1.15 || AccelMean < 0.85) {
        freefall_flag = 0;
        StatusCode = 1;
        digitalWrite(buzzerPin, LOW);
      }
    }
    if((freefall_flag == 1) && (timestamp_now + 10 - timestamp_Fall >= 10000)){
      StatusCode = 2;
    }

    if((StatusCode == 2) && (tone_flag == 0) && (timestamp_now - timestamp_Tone > 3000)){
      digitalWrite(buzzerPin, HIGH);
      tone_flag = 1;
      timestamp_Tone = millis();
      SerialUSB.println("buzzer activated");
    }
    if((StatusCode == 2) && (tone_flag == 1) && (timestamp_now + 100 - timestamp_Tone > 2000)){
      digitalWrite(buzzerPin, LOW);
      tone_flag = 0;
      SerialUSB.println("buzzer deactivated");
    }
    timestamp_Acc = millis();
  }
}
