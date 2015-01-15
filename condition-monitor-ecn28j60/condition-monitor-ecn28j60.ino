/* Arduino Condition Monitor Devices */
/***************************************************
 * Airmonitor Config File
 ****************************************************/
/* Free SRAM Memory check */
#define CONFIG_CHECK_FREEMEM 1

#define CONFIG_ECN  1

//#define CONFIG_BUZZER
#define CONFIG_SERIAL_DEBUG  1
#define CONFIG_NOKIA5110  1

#define CONFIG_YEELINK 1
//#define CONFIG_LEWEE 1
//#define CONFIG_THINGSSPEAK  1

/*************************************************
 * include file
 *************************************************/

#include <dht11.h>
#include <avr/pgmspace.h>

#define DbgSerial Serial

#ifdef CONFIG_SERIAL_DEBUG
#define DbgPrintln(...)  DbgSerial.println(__VA_ARGS__)
#define DbgPrint(...)  DbgSerial.print(__VA_ARGS__)
#else /* !CONFIG_SERIAL_DEBUG */
#define DbgPrintln(...)
#define DbgPrint(...)
#endif/* CONFIG_SERIAL_DEBUG */

#define WarnPrint(...)  DbgSerial.println(__VA_ARGS__)
#define WarnPrintln(...)  DbgSerial.print(__VA_ARGS__)

/* before variable dont use define, which might generate compile BUG */
int pm_i=0;
float ppm=0;
int pm2_5=0;

/***********************************
 * YEEILINK
 ************************************/
#define SENSOR_NR 3
#define SENSOR_INDEX_TEMP       0
#define SENSOR_INDEX_HUMIDITY  1
#define SENSOR_INDEX_PM25      2

uint8_t currentSensorIndex = 0;
int sensorDataInt[SENSOR_NR];
char sensorData[SENSOR_NR][30];
uint8_t sensorDataLength[SENSOR_NR];

#ifdef CONFIG_YEELINK
#define yeeWebsiteIp "42.96.164.52"
#define yeeWebsite "api.yeelink.net"
#define yeeAPIKey "13529a2762b65b3ce5b08fa107255b53"
#define DeviceID "14041"
const int SensorID[SENSOR_NR] = {
  23284,23285,23283};
#endif

#ifdef CONFIG_LEWEE
#define leweeAPIIp "42.121.254.11"
#define LEWEE_API_KEY "6e381e9065704507a5019cdf5c205844"
#define DeviceID  "condmon"
int SensorID[SENSOR_NR] PROGMEM = {
  1,2,3};
#endif

#ifdef CONFIG_THINGSPEAK
#define thingspeakIP "184.106.153.149" // thingspeak.com
#define THINGSPEAK_UPDATE = "GET /update?key=S0SCGNNK4W63GDZL&field";
int SensorID[SENSOR_NR] PROGMEM = {
  1,2,3};
#endif

uint32_t lastConnectionTime = 0;
const uint16_t PostingInterval = 10000;

#ifdef CONFIG_CHECK_FREEMEM
#define CHECK_MEM_SIZE  1024
int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

int checkFreeByMalloc()
{
  int i = 0 ;

  for( i = CHECK_MEM_SIZE; i>0; i--)
  {
    char* c = (char*)malloc(i);

    if(c){
      free(c);
      c = NULL;
      goto l_end;
    }
  }

  i = 0;
l_end:
  return i;
}

void checkFreeMem()
{
  int ramSize = freeRam();

  WarnPrint("Mem left:");
  WarnPrintln(ramSize);
}
#else
#define checkFreeMem(...)
#endif

/* setup function for arduino devices */
void setup()
{
  nokia5110_setup();

  sensor_setup();

#ifdef CONFIG_ECN
  ecn_setup();
#endif

  DbgPrint(F("after setup"));
  checkFreeMem();
}

/* main loop function */
void loop()
{
#ifdef CONFIG_ECN
  collectAndSendEcn();
#else
  collectSensorAllData();
#endif
}

