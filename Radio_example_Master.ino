#include <SPI.h>

//Radio Head Library: 
#include <RH_RF95.h>

// We need to provide the RFM95 module's chip select and interrupt pins to the 
// rf95 instance below.On the SparkFun ProRF those pins are 12 and 6 respectively.
RH_RF95 rf95(12, 6);

int LED = 13; //Status LED on pin 13

int packetCounter = 0; //Counts the number of packets sent
long timeSinceLastPacket = 0; //Tracks the time stamp of last packet received
// The broadcast frequency is set to 921.2, but the SADM21 ProRf operates
// anywhere in the range of 902-928MHz in the Americas.
// Europe operates in the frequencies 863-870, center frequency at 
// 868MHz.This works but it is unknown how well the radio configures to this frequency:
//float frequency = 864.1;
float frequency = 921.2;

void setup()
{
  pinMode(LED, OUTPUT);

  SerialUSB.begin(9600);
  // It may be difficult to read serial messages on startup. The following
  // line will wait for serial to be ready before continuing. Comment out if not needed.
  while(!SerialUSB);
  SerialUSB.println("RFM Server!");

  //Initialize the Radio. 
  if (rf95.init() == false){
    SerialUSB.println("Radio Init Failed - Freezing");
    while (1);
  }
  else{
  // An LED indicator to let us know radio initialization has completed.
    SerialUSB.println("Receiver up!");
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
     if (!SD.begin(10)) {
      SerialUSB.println("SD card initialization failed!");
      while (1);
  }

   dataFile = SD.open("data.txt", FILE_WRITE);
    if (!dataFile) {
      SerialUSB.println("Failed to create data file!");
      while (1);
    }

  rf95.setFrequency(frequency); 

 // The default transmitter power is 13dBm, using PA_BOOST.
 // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
 // you can set transmitter powers from 5 to 23 dBm:
 // rf95.setTxPower(14, false);
}

void parseReceivedData(const char* receivedData) {
  // Create a temporary buffer to store each value
  char tempValue[20];
  
  // Parse the receivedData string
  int i = 0;
  int j = 0;
  while (receivedData[i] != '\0') {
    if (receivedData[i] != ' ') {
      // Add the current character to the temporary buffer
      tempValue[j] = receivedData[i];
      j++;
    } else {
      // Null-terminate the temporary buffer to form a string
      tempValue[j] = '\0';
      
      // Convert the string to the corresponding data type and print it
      if (j > 0) {
        if (i == 0) {
          SerialUSB.print("CO2PPM: ");
          SerialUSB.println(atoi(tempValue));
        } else if (i == 2) {
          SerialUSB.print("TVOCPPB: ");
          SerialUSB.println(atoi(tempValue));
        } else if (i == 4) {
          SerialUSB.print("Temperature: ");
          SerialUSB.println(atof(tempValue));
        } else if (i == 6) {
          SerialUSB.print("Pressure: ");
          SerialUSB.println(atoi(tempValue));
        } else if (i == 8) {
          SerialUSB.print("Altitude: ");
          SerialUSB.println(atof(tempValue));
        } else if (i == 10) {
          SerialUSB.print("Humidity: ");
          SerialUSB.println(atof(tempValue));
        }
      }
      
      // Reset the temporary buffer index
      j = 0;
    }
    
    // Move to the next character in the receivedData string
    i++;
  }
}

void loop()
{
  if (rf95.available()){
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)){
      digitalWrite(LED, HIGH); //Turn on status LED
      timeSinceLastPacket = millis(); //Timestamp this packet

      SerialUSB.print("Got message: ");
      SerialUSB.print("")
      SerialUSB.print((char*)buf);
      //SerialUSB.print(" RSSI: ");
      //SerialUSB.print(rf95.lastRssi(), DEC);
      SerialUSB.println();

       dataFile.println((char*)buf);
      dataFile.flush();

      // Send a reply
      uint8_t toSend[] = "Hello Back!"; 
      rf95.send(toSend, sizeof(toSend));
      rf95.waitPacketSent();
      SerialUSB.println("Sent a reply");
      digitalWrite(LED, LOW); //Turn off status LED

    }
    else
      SerialUSB.println("Recieve failed");
  }
  //Turn off status LED if we haven't received a packet after 1s
  if(millis() - timeSinceLastPacket > 1000){
    digitalWrite(LED, LOW); //Turn off status LED
    timeSinceLastPacket = millis(); //Don't write LED but every 1s
  }
}
