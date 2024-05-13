#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95(12, 6);

int LED = 13;
const int LED_TX = PIN_LED_TXL;
const int LED_RX = PIN_LED_RXL;

unsigned long lastPacketTime = 0;
unsigned long noConnectionDuration = 0;
unsigned long noConnectionSendTime = 0;
unsigned long elapsedPacketTime = 0;
int sentFlag = 0;


String lastReceivedData;

float frequency = 921.2;

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(LED_TX, OUTPUT);
  pinMode(LED_RX, OUTPUT);

  SerialUSB.begin(9600);
  while (!SerialUSB);

  if (rf95.init() == false)
  {
    SerialUSB.println("Radio Init Failed - Freezing");
    while (1);
  }
  else
  {
    digitalWrite(LED, HIGH);
    digitalWrite(LED_TX, LOW);
    digitalWrite(LED_RX, LOW);
    delay(500);
    digitalWrite(LED, LOW);
    digitalWrite(LED_TX, HIGH);
    digitalWrite(LED_RX, HIGH);
    delay(500);
  }

  rf95.setFrequency(frequency);
}

void loop()
{
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.available())
  {
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      digitalWrite(LED_RX, LOW);
      lastPacketTime = millis();  // 마지막 패킷 수신 시간 업데이트

      String receivedData = (char*)buf;
      receivedData += "_";
      receivedData += rf95.lastRssi();
      receivedData += "_";

      elapsedPacketTime = millis() - lastPacketTime;
      lastReceivedData = receivedData;
      receivedData += elapsedPacketTime;

      SerialUSB.print(receivedData);
      sentFlag = 1;
    }
  }

  if((millis() - lastPacketTime > 2000) && (millis() - noConnectionSendTime > 1000) && (sentFlag == 1)){
    
    noConnectionDuration = millis() - lastPacketTime;
    String noConncetionSerialData = lastReceivedData + noConnectionDuration;
    noConnectionSendTime = millis();

    SerialUSB.print(noConncetionSerialData);  
  }

  // Turn off status LED if we haven't received a packet after 1 second
  if (millis() - lastPacketTime > 1000)
  {
    digitalWrite(LED, LOW); // Turn off status LED
  }
}
