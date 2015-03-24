/* Arduino Condition Monitor Devices */
/***************************************************
 * Airmonitor Config File
 ****************************************************/
/* Free SRAM Memory check */
//#define CONFIG_CHECK_FREEMEM 1

//#define CONFIG_SERIAL_DEBUG  1

//#define CONFIG_BUZZER

//#define CONFIG_ESP8266_UARTWIFI 1
#define CONFIG_ESP8266_SIMPLE 1

#define CONFIG_NOKIA5110  1

#define CONFIG_YEELINK 1
//#define CONFIG_LEWEE 1
//#define CONFIG_THINGSPEAK  1

/*************************************************
 * include file
 *************************************************/
#include <SoftwareSerial.h>
#ifdef CONFIG_ESP8266_UARTWIFI
//#include <uartWIFI.h>
#endif

#ifdef CONFIG_ESP8266_SIMPLE
#define _DBG_RXPIN_ 10
#define _DBG_TXPIN_ 11
#endif

#include <dht11.h>
#include <avr/pgmspace.h>

#if defined(CONFIG_ESP8266) || defined(CONFIG_ESP8266_SIMPLE)
#define DbgSerial  mySerial
#else
#define DbgSerial Serial
#endif

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

#ifdef CONFIG_ESP8266_SIMPLE
SoftwareSerial mySerial(_DBG_RXPIN_,_DBG_TXPIN_);
#endif

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
#define yeeWebsiteIp F("42.96.164.52")
#define yeeWebsite "api.yeelink.net"
#define yeeAPIKey F("13529a2762b65b3ce5b08fa107255b53")
#define DeviceID F("14041")
const int SensorID[SENSOR_NR] = {
  23284,23285,23283};
#endif

#ifdef CONFIG_LEWEE
#define leweeAPIIp F("42.121.254.11")
#define LEWEE_API_KEY F("6e381e9065704507a5019cdf5c205844")
#define DeviceID  F("condmon")
int SensorID[SENSOR_NR] PROGMEM = {
  1,2,3};
#endif

#ifdef CONFIG_THINGSPEAK
#define thingspeakIP F("184.106.153.149") // thingspeak.com
#define THINGSPEAK_UPDATE  F("GET /update?key=S0SCGNNK4W63GDZL&field")
#define DeviceID  F("xxx")
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
#if !defined(CONFIG_ESP8266) && !defined(CONFIG_ESP8266_SIMPLE)
  DbgSerial.begin(9600);
  DbgPrintln(F("Serial Init done..."));
#endif

  nokia5110_setup();

  sensor_setup();

#if defined(CONFIG_ESP8266) || defined(CONFIG_ESP8266_SIMPLE)
  esp8266_setup();
#endif

  DbgPrint(F("after setup"));
  checkFreeMem();
}

/* main loop function */
void loop()
{
#if defined(CONFIG_ESP8266)  || defined(CONFIG_ESP8266_SIMPLE)
  collectAndSendEsp8266();
#else
  collectSensorAllData();
#endif
}

