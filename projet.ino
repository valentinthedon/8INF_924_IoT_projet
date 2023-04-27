#include <MKRWAN.h>

#define PACKET_INTERVAL 5000
#define SENSOR_INTERVAL 400

LoRaModem modem;

String appEui = "0000000000000000";
String appKey = "BC3525DA457BDB58C49E5854F4C6D77F";

const int gasSensorPin = A1;
const int motorPin = 1;

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.println("Serial Started");
  
  if(!modem.begin(US915)) {
    Serial.println("Failed to start module");
    exit(1);
  };
  
  int connected;
  appKey.trim();
  appEui.trim();

  connected = modem.joinOTAA(appEui, appKey);

  if (!connected) {
    Serial.println("An error occur while try to connect");
    exit(1);
  } else {
    Serial.println("Connected to the gateway");
  }
}


int sendPacket(int gasVal, int motorVal){
  modem.beginPacket();
  modem.print(gasVal);
  modem.print(motorVal);
  int err = modem.endPacket(true);
  if(err > 0){
    Serial.print("<-- Sent Packet with value : "); 
    Serial.print(gasVal);
    Serial.print("\t"); 
    Serial.println(motorVal);   
  } else {
    Serial.println("/!\ An error occur, a packet WASN'T sent");
  }
  return err;
}


void loop() {
  int time = millis();
  int gasSensorDataCount = 0;
  int gasSensorSum = 0;
  int gasSensorVal = 0;
  int motorVal = 0;
  while(time + PACKET_INTERVAL > millis()){
    gasSensorVal = analogRead(gasSensorPin);
    gasSensorSum += gasSensorVal;
    Serial.print("(i) Read Data : ");
    Serial.println(gasSensorVal);    
    gasSensorDataCount++;
    delay(SENSOR_INTERVAL);    
  }

  int gasSensorAvgVal = gasSensorSum / gasSensorDataCount;  
  if(gasSensorAvgVal > 850) {
    motorVal = 255;            // On demarre le moteur vittesse maximale
    sendPacket(gasSensorAvgVal, 1);
  }
  else {
    motorVal = 0;
    sendPacket(gasSensorAvgVal, 0);
  }

  analogWrite(motorPin, motorVal); 
}