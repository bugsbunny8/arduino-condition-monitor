/****************************************
 * ESP8266 Pin define
 ****
 * PIN define
 *UTXD(TXD)      GND(GND)
 *CH_PD(3.3V)    GPIO2(NULL)
 *RST(NULL)      GPIO0(NULL)
 *VCC(3.3v)      URXD(RXD)
 *
 * PIN connection
 ***
 * UTXD <----------------> RX0
 * URXD <----------------> TX1

 * SoftSerial
 * PIN2(RX)
 * PIN3(TX)
***************************************/
#define HOME_SSID "TP-LINK_ZPY"
#define HOME_PASSWORD "zpy810304?"

#define ESP8266_SERIAL_PRINT(...)  Serial.print(__VA_ARGS__)
#define ESP8266_SERIAL_PRINTLN(...) Serial.println(__VA_ARGS__)

#ifdef CONFIG_ESP8266_UARTWIFI

WIFI esp8266wifi;

void esp8266_setup()
{
  bool wifi_start = false;
  String wifi_ip;

  esp8266wifi.begin();

  wifi_start = esp8266wifi.Initialize(STA, HOME_SSID, HOME_PASSWORD);
  if(!wifi_start){
    DbgPrintln(F("start wifi failed!\n"));
  }
  delay(8000);
  wifi_ip = esp8266wifi.showIP();
}

int getIntLength(int someValue) {
  // there's at least one byte:
  int digits = 1;

  // continually divide the value by ten,
  // adding one to the digit count for each
  // time you divide, until you're at 0:
  int dividend = someValue /10;
  while (dividend > 0) {
    dividend = dividend /10;
    digits++;
  }
  // return the number of digits:
  return digits;
}

void esp8266SendData(int deviceId, int sensorId, int Data)
{
  String httpCmd;
  int cmdLength = 10 + getIntLength(Data);

  if (!esp8266wifi.ipConfig(TCP, yeeWebsite, 80)) {
    esp8266wifi.closeMux();
    return;
  }

  httpCmd = "POST /v1.0/device/";
  httpCmd += String(deviceId);
  httpCmd += "/sensor/";
  httpCmd += String(sensorId);
  httpCmd += "/datapoints";
  httpCmd += " HTTP/1.1\r\n";
  httpCmd += "Host: api.yeelink.net\r\n";
  httpCmd += "Accept: *";
  httpCmd += "/";
  httpCmd += "*\r\n";
  httpCmd += "U-ApiKey: ";
  httpCmd += yeeAPIKey;
  httpCmd += "\r\n";
  httpCmd += "Content-Length: ";
  httpCmd += String(cmdLength);
  httpCmd += "\r\n";
  httpCmd += "Content-Type: application/x-www-form-urlencoded\r\n";
  httpCmd += "Connection: close\r\n";
  httpCmd += "\r\n";
  httpCmd += "{\"value\":";
  httpCmd += String(Data);
  httpCmd += "}\r\n";
  esp8266wifi.Send(httpCmd);
  
  lastConnectionTime = millis();
}  

void collectAndSendEsp8266(void)
{
  uint32_t currentTime = 0;

  currentTime = millis();
  if( (currentTime > lastConnectionTime) &&
  ( (currentTime - lastConnectionTime) <= PostingInterval) ){
    /* still in post range, collect data and return */
     collectSensorAllData();
     return;
  }

  collectSensorData(currentSensorIndex);
  esp8266SendData(DeviceID, SensorID[currentSensorIndex], sensorDataInt[currentSensorIndex]);
  inc_current_sensor_index();

#if 0  
  char recvMessage[400];
  if(esp8266wifi.ReceiveMessage(recvMessage)) 
  {
      DbgPrintln(recvMessage);   
  }
#endif

  delay(1000);
}

#endif

#ifdef CONFIG_ESP8266_SIMPLE

boolean connectWiFi(){
  ESP8266_SERIAL_PRINTLN(F("AT+CWMODE=1"));
  delay(2000);
  
  String cmd="AT+CWJAP=\"";
  cmd+=HOME_SSID;
  cmd+="\",\"";
  cmd+=HOME_PASSWORD;
  cmd+="\"";
  ESP8266_SERIAL_PRINTLN(cmd);
  
  delay(5000);
  if(Serial.find("OK")){
    mySerial.println(F("RECEIVED: OK"));
    return true;
  }else{
    mySerial.println(F("RECEIVED: Error"));
    return false;
  }
}

void esp8266_setup()
{
  Serial.begin(9600);
  Serial.flush();
  ESP8266_SERIAL_PRINTLN(F("AT+RST"));
  
  mySerial.begin(9600);
  
  delay(3000);
  connectWiFi();
}

bool esp8266IpConfig(const __FlashStringHelper * webIp)
{  
  ESP8266_SERIAL_PRINTLN(F("AT+CIPMUX=1"));
  Serial.flush();
  delay(1000);
  
  ESP8266_SERIAL_PRINT(F("AT+CIPSTART=0,\"TCP\",\""));
  ESP8266_SERIAL_PRINT(webIp);
  ESP8266_SERIAL_PRINTLN(F("\",80"));
  delay(2000);

  if(Serial.find("Error")){
   return false; 
  }
  
  return true;
}

int getIntLength(int someValue) {
  // there's at least one byte:
  int digits = 1;

  // continually divide the value by ten, 
  // adding one to the digit count for each
  // time you divide, until you're at 0:
  int dividend = someValue /10;

  while (dividend > 0) {
    dividend = dividend /10;
    digits++;
  }
  // return the number of digits:
  return digits;
}

int esp8266SendF(const __FlashStringHelper * str, bool isSend)
{
  if(isSend){
    ESP8266_SERIAL_PRINT(str);
  }
  return strlen_P((const prog_char *)(str));
}

int esp8266SendlnF(const __FlashStringHelper * str, bool isSend)
{
  if(isSend){
    ESP8266_SERIAL_PRINTLN(str);
  }
  return strlen_P((const prog_char *)(str));
}

int esp8266SendStr(char * str, bool isSend)
{
  if(isSend){
    ESP8266_SERIAL_PRINT(str);
  }
  return strlen(str);
}

int esp8266SendInt(int val, bool isSend)
{
  String intStr = String(val);

  if(isSend){
    ESP8266_SERIAL_PRINT(intStr);
  }
  
  return intStr.length();
}

/* have to optimize codes like this, rather than using String aggregate it,
 * which might generate enormous SRAM memory usage! */
#ifdef CONFIG_YEELINK
/* SEND or GetString Length*/
int esp8266SendGetLenYeelink(bool isSend, const __FlashStringHelper * deviceId, int sensorId, int Data)
{
  int httpCmdLen = 0;
  int cmdLength = 10 + getIntLength(Data);

  httpCmdLen = esp8266SendF(F("POST /v1.0/device/"), isSend);
  httpCmdLen += esp8266SendF(deviceId, isSend);
  httpCmdLen += esp8266SendF(F("/sensor/"), isSend);
  httpCmdLen += esp8266SendInt(sensorId, isSend);
  httpCmdLen += esp8266SendF(F("/datapoints"), isSend);
  httpCmdLen += esp8266SendF(F( " HTTP/1.1\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Host: api.yeelink.net\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Accept: *"), isSend);
  httpCmdLen += esp8266SendF(F("/"), isSend);
  httpCmdLen += esp8266SendF(F("*\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("U-ApiKey: "), isSend);
  httpCmdLen += esp8266SendF(yeeAPIKey, isSend);
  httpCmdLen += esp8266SendF(F("\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Content-Length: "), isSend);
  httpCmdLen += esp8266SendInt(cmdLength, isSend);
  httpCmdLen += esp8266SendF(F("\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Content-Type: application/x-www-form-urlencoded\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Connection: close\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("{\"value\":"), isSend);
  httpCmdLen += esp8266SendInt(Data, isSend);
  httpCmdLen += esp8266SendlnF(F("}\r\n"), isSend);
  
  return httpCmdLen;
}
#endif

#ifdef CONFIG_LEWEE
int esp8266SendGetLenLewee(bool isSend, const __FlashStringHelper * deviceId, int sensorId, int Data)
{
  int httpCmdLen = 0;
  int cmdLength = 10 + getIntLength(Data);

  httpCmdLen = esp8266SendF(F("POST /api/V1/gateway/UpdateSensors/"), isSend);
  httpCmdLen += esp8266SendF(deviceId, isSend);
  httpCmdLen += esp8266SendF(F(" HTTP/1.1\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Host: www.lewei50.com\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Accept: *"), isSend);
  httpCmdLen += esp8266SendF(F("/"), isSend);
  httpCmdLen += esp8266SendF(F("*\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("userkey: "), isSend);
  httpCmdLen += esp8266SendF(LEWEE_API_KEY, isSend);
  httpCmdLen += esp8266SendF(F("\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Content-Length: "), isSend);
  httpCmdLen += esp8266SendInt(cmdLength, isSend);
  httpCmdLen += esp8266SendF(F("\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Content-Type: application/x-www-form-urlencoded\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("Connection: close\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("\r\n"), isSend);
  httpCmdLen += esp8266SendF(F("[{\"Name\":\""), isSend);
  httpCmdLen += esp8266SendInt(sensorId, isSend);
  httpCmdLen += esp8266SendF(F("\",\"Value\":\""), isSend);
  httpCmdLen += esp8266SendInt(Data, isSend);
  httpCmdLen += esp8266SendlnF(F("\"}]\r\n"), isSend);

  return httpCmdLen;
}
#endif

#ifdef CONFIG_THINGSPEAK
int esp8266SendGetLenThingspeak(bool isSend, const __FlashStringHelper * deviceId, int sensorId, int Data)
{
  int httpCmdLen = 0;

  httpCmdLen = esp8266SendF(THINGSPEAK_UPDATE, isSend);
  httpCmdLen += esp8266SendInt(sensorId, isSend);
  httpCmdLen += esp8266SendF(F("="), isSend);
  httpCmdLen += esp8266SendInt(Data, isSend);
  httpCmdLen += esp8266SendlnF(F("\r\n"), isSend);

  return httpCmdLen;
}
#endif

typedef int (*sendGetLenFunc_t)(bool, const __FlashStringHelper * , int , int );

void esp8266SendData(const __FlashStringHelper * deviceId, int sensorId, int Data)
{
  int httpCmdLen = 0;
  
  const __FlashStringHelper * apiWebsiteIp;
  sendGetLenFunc_t esp8266sendGetLenFunc;
  
  mySerial.println(deviceId);
  mySerial.print(F("sensorId:"));
  mySerial.println(sensorId);
  
#ifdef CONFIG_YEELINK
  apiWebsiteIp = yeeWebsiteIp;
  esp8266sendGetLenFunc = esp8266SendGetLenYeelink;
#endif

#ifdef CONFIG_LEWEE
  apiWebsiteIp = leweeAPIIp;
  esp8266sendGetLenFunc = esp8266SendGetLenLewee;
#endif

#ifdef CONFIG_THINGSPEAK
  apiWebsiteIp = thingspeakIP;
  esp8266sendGetLenFunc = esp8266SendGetLenThingspeak;
#endif

  if (!esp8266IpConfig(apiWebsiteIp)) {
    ESP8266_SERIAL_PRINTLN(F("AT+CIPCLOSE=0"));
    return;
  }
  
  httpCmdLen = esp8266sendGetLenFunc(false, deviceId, sensorId, Data);

  ESP8266_SERIAL_PRINT(F("AT+CIPSEND=0,"));
  ESP8266_SERIAL_PRINTLN(httpCmdLen);
  
  checkFreeMem();
  
  if(Serial.find(">")){
    /* Real Send */
    esp8266sendGetLenFunc(true, deviceId, sensorId, Data);;
  }else{
    ESP8266_SERIAL_PRINTLN(F("AT+CIPCLOSE"));
  }
  
}  

void collectAndSendEsp8266(void)
{
  char recvMessage[400];
  uint32_t currentTime = 0;

  currentTime = millis();
  if( (currentTime > lastConnectionTime) &&
  ( (currentTime - lastConnectionTime) <= PostingInterval) ){
    /* still in post range, collect data and return */
     collectSensorAllData();
     delay(500);

     return;
  }

  collectSensorData(currentSensorIndex);
  esp8266SendData(DeviceID, SensorID[currentSensorIndex], sensorDataInt[currentSensorIndex]);
  inc_current_sensor_index();
  lastConnectionTime = currentTime;
  
  delay(1000);
}

#endif
