
/*BUZZER */
/*******************************
 * IO ------------------> PIN 3
 ********************************/
const int buzzerPin=3;
/* if PM2.5 > 120, do warning */
#define PM25_WARN  120

/*DHT11*/
/*******************************
 * Out ------------------> PIN 4
 ********************************/
dht11 DHT11;
int dht11ReadPin=4;

/*GP2Y1010AUOF*/
/*******************************
 * GP2Y1010AU0F ---------> Arduino
 * LED ------------------> PIN 13 //ESP8266
 * LED ------------------> PIN 2
 * Dust -----------------> A0
 
 *
 * 1.Blue    ---------  V-LED
 * 2.Green   ---------  LED-GND
 * 3.White   ---------  LED
 * 4.Yellow  ---------  S-GND
 * 5.Black   ---------  VO
 * 6.Red     ---------  VCC
 ********************************/
#ifdef CONFIG_ESP8266
#define GP2Y_LED_PIN  13
#elif defined(CONFIG_ECN)
#define GP2Y_LED_PIN  2
#else
#define GP2Y_LED_PIN  13
#endif

int ledPowerPin=GP2Y_LED_PIN;
int dustPin=0;

#define GP2Y_TIME_LED_ON  280
#define GP2Y_TIME_READDUST  40
#define GP2Y_TIME_LED_OFF  9680

#ifdef CONFIG_BUZZER
void buzzer_setup(void)
{
  /* init buzzer */
  pinMode(buzzerPin,OUTPUT);//设置数字IO脚模式，OUTPUT为输出
  digitalWrite(buzzerPin, HIGH); /* disable buzzer by default */
}
#endif

void sensor_setup(void)
{
#ifdef CONFIG_BUZZER
  buzzer_setup();
#endif

  /* init LedPower for GP2Y1010AU0F */
  pinMode(ledPowerPin, OUTPUT);//ledPower  GP2Y1010AU0F
}

/* read data from dust sensor */
void pm25_get_ppm(){
  int dustVal=0;

  pm_i=pm_i+1;
  digitalWrite(ledPowerPin, LOW); // power on the LED
  delayMicroseconds(GP2Y_TIME_LED_ON);
  dustVal=analogRead(dustPin); // read the dust value
  DbgPrintln(dustVal);  

  ppm = ppm + dustVal;
  delayMicroseconds(GP2Y_TIME_READDUST);
  digitalWrite(ledPowerPin,HIGH); // turn the LED off
  delayMicroseconds(GP2Y_TIME_LED_OFF);  
}

void pmData(){
  float ppmpercf = 0;
  float voltage = 0;
  float dustdensity = 0;

  pm25_get_ppm();
  
  voltage = ppm/pm_i*0.0049;
  dustdensity = 0.17*voltage-0.1;
  ppmpercf = (voltage-0.0256)*120000;
  if (ppmpercf < 0)
    ppmpercf = 0;
  if (dustdensity < 0 )
    dustdensity = 0;
  if (dustdensity > 0.5)
    dustdensity = 0.5;  
  pm_i=0;
  ppm=0;  
  pm2_5=dustdensity*1000;
  
  DbgPrint("voltage :");
  DbgPrintln(voltage);
  DbgPrint("dustdensity :");
  DbgPrintln(dustdensity);
  DbgPrint("ppmpercf :");
  DbgPrintln(ppmpercf);
  
  nokia5110_show_pm25(pm2_5);
  buzzerWarn(pm2_5, PM25_WARN);
}

void dht11Read(){
  int chk = 0;
  
  DbgPrint(F("====Read"));
  checkFreeMem();
  
  DbgPrint(F("Read sensor: "));
  chk = DHT11.read(dht11ReadPin);
  switch (chk)
  {
  case DHTLIB_OK: 
    DbgPrintln(F("OK")); 
    break;
    
  case DHTLIB_ERROR_CHECKSUM: 
    DbgPrintln("Checksum error"); 
    break;
    
  case DHTLIB_ERROR_TIMEOUT: 
    DbgPrintln("Time out error"); 
    break;
    
  default: 
    DbgPrintln("Unknown error"); 
    break;
  }
  
  DbgPrint("Humidity (%): ");
  DbgPrintln((float)DHT11.humidity, 2);
  DbgPrint("Temperature (oC): ");
  DbgPrintln((float)DHT11.temperature-2, 2);
  
  nokia5110_show_temp(DHT11.temperature-2, DHT11.humidity);

  DbgPrint(F("====Read done"));
  checkFreeMem();

}

void buzzerWarn(int val, int val_warn)
{
  if(val < val_warn){
    return;
  }
  
  digitalWrite(buzzerPin, LOW);
  delay(1000);
  digitalWrite(buzzerPin, HIGH);
}

void collectSensorData(uint8_t sensorIndex){
  DbgPrintln(currentSensorIndex);
  
  switch(sensorIndex){
  case SENSOR_INDEX_TEMP:
    dht11Read();
    sensorDataInt[SENSOR_INDEX_TEMP] = DHT11.temperature-2;
    sensorDataLength[sensorIndex]=sprintf(sensorData[sensorIndex],"{\"value\":%d}",DHT11.temperature-2);
    break;

  case SENSOR_INDEX_HUMIDITY:
    dht11Read();
    sensorDataInt[SENSOR_INDEX_HUMIDITY] = DHT11.humidity;
    sensorDataLength[sensorIndex]=sprintf(sensorData[sensorIndex],"{\"value\":%d}",DHT11.humidity);
    break;

  case SENSOR_INDEX_PM25:
    pmData();
    sensorDataInt[SENSOR_INDEX_PM25] = pm2_5;
    sensorDataLength[sensorIndex]=sprintf(sensorData[sensorIndex],"{\"value\":%d}",pm2_5);
    break;
  }
}

void collectSensorAllData(void)
{
  dht11Read();
  delay(2000);
  
  pmData();
  delay(2000);
}

void inc_current_sensor_index()
{
  currentSensorIndex ++;
  if(currentSensorIndex == SENSOR_NR){
    currentSensorIndex = 0;
  }
}

#ifdef CONFIG_ECN
void collectAndSendEcn(void)
{
  uint32_t currentTime = 0;
  
  if(useEthernet){
    ether.packetLoop(ether.packetReceive());
    
    currentTime = millis();
    if( (currentTime < lastConnectionTime) ||
      (currentTime - lastConnectionTime > PostingInterval) ){
      collectSensorData(currentSensorIndex);
      
      DbgPrint("get_dns:");
      DbgPrintln(ecn_get_dns);
      
      if(ecn_get_dns){
        sendSensorData(currentSensorIndex);
      } else{
        collectSensorAllData();
        lastConnectionTime = currentTime;
      }

      inc_current_sensor_index();
    }
  }
  else{
    collectSensorAllData();
  }

}
#endif

void sendSensorData(uint8_t sensorIndex) {
#ifdef CONFIG_ECN
  ecn_send_sensor_yeelink(DeviceID, SensorID[sensorIndex],
    yeeAPIKey,
    sensorDataLength[sensorIndex],
    sensorData[sensorIndex]);
#endif

  lastConnectionTime = millis(); 
}

