
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
 * LED ------------------> PIN 13
 ********************************/
int ledPowerPin=13;
int dustPin=0;

int delayTime=280;
int delayTime2=40;
float offTime=9680;

void buzzer_setup(void)
{
  /* init buzzer */
  pinMode(buzzerPin,OUTPUT);//设置数字IO脚模式，OUTPUT为输出
  digitalWrite(buzzerPin, HIGH); /* disable buzzer by default */
}

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
  pm_i=pm_i+1;
  digitalWrite(ledPowerPin, LOW); // power on the LED
  delayMicroseconds(delayTime);
  dustVal=analogRead(dustPin); // read the dust value
  ppm = ppm+dustVal;
  delayMicroseconds(delayTime2);
  digitalWrite(ledPowerPin,HIGH); // turn the LED off
  delayMicroseconds(offTime);  
}

void pmData(){
  pm25_get_ppm();
  
  DbgPrintln(dustVal);  
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
  // Serial.println("\n");
  int chk = 0;

  Serial.print("Read sensor: ");
  chk = DHT11.read(dht11ReadPin);
  switch (chk)
  {
  case DHTLIB_OK: 
    Serial.println("OK"); 
    break;
  case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("Checksum error"); 
    break;
  case DHTLIB_ERROR_TIMEOUT: 
    Serial.println("Time out error"); 
    break;
  default: 
    Serial.println("Unknown error"); 
    break;
  }
  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);
  Serial.print("Temperature (oC): ");
  Serial.println((float)DHT11.temperature-2, 2);
  
  nokia5110_show_temp(DHT11.temperature-2, DHT11.humidity);
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
  case 0:
    dht11Read();
    sensorDataLength[sensorIndex]=sprintf(sensorData[sensorIndex],"{\"value\":%d}",DHT11.temperature-2);
    break;

  case 1:
    dht11Read();
    sensorDataLength[sensorIndex]=sprintf(sensorData[sensorIndex],"{\"value\":%d}",DHT11.humidity);
    break;

  case 2:
    pmData();
    // Serial.println(dustdensity*1000);
    sensorDataLength[sensorIndex]=sprintf(sensorData[sensorIndex],"{\"value\":%d}",pm2_5);
    break;
  }
}

