/*********************************************************************************
 * ECN28J60 SPI Interface
 * SPI: 10 (SS), 11 (MOSI), 12 (MISO), 13 (SCK).
 *VCC --------------------------> 3.3V
 *GND --------------------------> GND
 *CS  --------------------------> PIN 10
 *SI  --------------------------> PIN 11
 *SO  --------------------------> PIN 12
 *SCK --------------------------> PIN 13
 *INT --------------------------> PIN 2
 *********************************************************************************/
#ifdef CONFIG_ECN
#include <EtherCard.h>

#ifdef ARDUINO_MEGA
#define ARDUINO_SS_PIN  53
#else
#define ARDUINO_SS_PIN  10
#endif

int ECN_dcPin=ARDUINO_SS_PIN;

static byte ECN_MAC[] PROGMEM = { 
  0x74,0x69,0x69,0x2D,0x30,0x31};
static byte ECN_IP[] PROGMEM = { 
  192,168,1,122};
static byte ECN_MASK[] PROGMEM ={ 
  255,255,255,0};
static byte ECN_GW[] PROGMEM ={ 
  192,168,1,254};
static byte ECN_DNS[] PROGMEM ={ 
  192,168,1,1};

boolean useEthernet = false;
boolean ecn_get_dhcp = false;
boolean ecn_get_dns = false;
byte Ethernet::buffer[500];

void ecn_setup(void)
{  
  /*setup Ethernet Device*/

  if (ether.begin(sizeof Ethernet::buffer, ECN_MAC, ECN_dcPin) == 0){
    DbgPrintln(F("Failed to connect to Ethernet Controller..."));
    useEthernet = false;
  } 
  else{
    useEthernet = true;
#if 1

    DbgPrintln(F("Starting DHCP..."));
    if( !ether.dhcpSetup()){
      /* Setting up Static IP */
      DbgPrintln(F("getting dhcp error!"));
      ether.staticSetup(ECN_IP, ECN_GW, ECN_DNS, ECN_MASK);
      ecn_get_dhcp = false;
    }
    else{
      ecn_get_dhcp = true;
  
      DbgPrintln("dhcp get done");
    }
#else
    ether.staticSetup(ECN_IP, ECN_GW, ECN_DNS, ECN_MASK);
    ecn_get_dhcp = false;
    DbgPrintln("setup static IP done");
#endif
  }
#if 1
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("Netmask: ", ether.netmask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
#endif

  /*DNS Lookup can only work in setup phase, which will block all incoming packets */
  if(!ether.dnsLookup(yeeWebsite, true)){
    DbgPrintln("DNS lookup failed...");
    ecn_get_dns = false;
  }
  else{
    ecn_get_dns = true;
  }

#if 0
  ether.printIp("DNS IP:", ether.dnsip);
  ether.printIp("My IP:",ether.myip);
  ether.printIp("Server: ", ether.hisip);
#endif

  currentSensorIndex = 0;
}

void ecn_send_sensor_yeelink(char *dev_id, uint16_t sensor_id, char * api_key, uint16_t data_len, char * data)
{
  DbgPrintln(data);
  DbgPrintln(sensor_id);
  DbgPrintln();
  
  /* Create a Post for yeelink server,
   * and send request saving sessionID */
  Stash::prepare(PSTR("POST /v1.0/device/$F/sensor/$D/datapoints HTTP/1.1" "\r\n"
    "Host: api.yeelink.net" "\r\n"
    "U-ApiKey: $F" "\r\n"
    "Content-Length: $D" "\r\n"
    "Content-Type: application/x-www-form-urlencoded" "\r\n" "\r\n"                      
    "$S" "\r\n"),
  dev_id, sensor_id,
  api_key,
  data_len,data);

  ether.tcpSend();
}

#endif

