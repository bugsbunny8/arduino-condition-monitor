/* Air Monitor Devices */

#include <dht11.h>
#include <LCD5110_Graph.h>
#include <SoftwareSerial.h>
#include <uartWIFI.h>

#define DbgSerial  mySerial
#define DbgPrintln(...)  DbgSerial.println(__VA_ARGS__)
#define DbgPrint(...)  DbgSerial.print(__VA_ARGS__)

int dustVal=0;
int pm_i=0;
float ppm=0;
float voltage = 0;
static float dustdensity = 0;
float ppmpercf = 0;
int pm2_5=0;

/***********************************
 * YEEILINK
 ************************************/
#define SENSOR_NR 3
uint8_t currentSensorIndex = 0;
char sensorData[SENSOR_NR][30];
uint8_t sensorDataLength[SENSOR_NR];

char website[] ="api.yeelink.net";
char APIKey[] PROGMEM = "13529a2762b65b3ce5b08fa107255b53";
char DeviceID[] PROGMEM = "14014";
const uint16_t SensorID[SENSOR_NR] = {
  23284,23285,23283};
uint32_t lastConnectionTime = 0;
const uint16_t PostingInterval = 5000;

void setup()
{
  sensor_setup();
  
#ifdef CONFIG_ECN
  ecn_setup();
#endif
}

void collectAllData(void)
{
  dht11Read();
  delay(2000);
  pmData();
  delay(2000);
}

void loop()
{
  #if !defined(CONFIG_ECN)
  collectAllData();
#else
  if(useEthernet){
    ether.packetLoop(ether.packetReceive());
    if( millis() < lastConnectionTime ||
      millis() - lastConnectionTime > PostingInterval){
      collectSensorData(currentSensorIndex);
      Serial.print("get_dns:");
      Serial.println(ecn_get_dns);
      if(ecn_get_dns){
        sendSensorData(currentSensorIndex);
      }
      else{
        collectAllData();
        lastConnectionTime = millis();
      }

      currentSensorIndex ++;
      if(currentSensorIndex == SENSOR_NR){
        currentSensorIndex = 0;
      }
    }
  }
  else{
    collectAllData();
  }
#endif
}


void sendSensorData(uint8_t sensorIndex) {
#ifdef CONFIG_ECN
  ecn_send_sensor_yeelink(DeviceID, SensorID[sensorIndex],
    APIKey,
    sensorDataLength[sensorIndex],
    sensorData[sensorIndex]);
#endif

  lastConnectionTime = millis(); 
}



