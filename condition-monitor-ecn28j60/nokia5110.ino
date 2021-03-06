#ifdef CONFIG_NOKIA5110
#include <LCD5110_Graph.h>

//nokia 5110
char AIR_MONITOR_VERSION[] PROGMEM ="v1.0";

/******************************
 * Pin define 
 * 5110  ------------------  Arduino
 * RST --------------------> PIN 9
 * CE  --------------------> PIN 8
 * DC  --------------------> PIN 7
 * DIN --------------------> PIN 6
 * CLK --------------------> PIN 5
 * VCC --------------------> VCC
 * BL  --------------------> 3.3V
 * GND --------------------> GND
 ******************************/
 #define nokia5110_PIN_CLK    5
 #define nokia5110_PIN_DIN    6
 #define nokia5110_PIN_DC     7
 #define nokia5110_PIN_CE     8
 #define nokia5110_PIN_RST    9
 
 LCD5110 myGLCD(nokia5110_PIN_CLK, nokia5110_PIN_DIN,
   nokia5110_PIN_DC, nokia5110_PIN_RST, nokia5110_PIN_CE);

extern uint8_t SmallFont[];
extern uint8_t BigNumbers[];
uint8_t temperature_ico[] PROGMEM={
  0x02,0x05,0x7A,0x84,0x84,0x48};//
uint8_t cls[] PROGMEM={
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t pm2_5_ico[] PROGMEM={
  0x00,0x00,0x00,0xF0,0xF8,0x8C,0x8C,0x8C,0x88,0xF0,0x70,0x00,0x00,0xD0,0x50,0x70,
  0x00,0x00,0x00,0x70,0x50,0xD0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x3F,0x3F,0x01,0x01,0x39,0x3D,0x08,0x3C,0x08,0x3C,0x39,0x01,0x01,0x00,0x01,
  0x00,0x01,0x01,0x01,0x40,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,
  0x1C,0x10,0x1C,0x00,0x5C,0x54,0x7C,0x00,0x40,0x20,0x1C,0x02,0x39,0x04,0x38,0x04,
  0x38,0x04,0x05,0x07,0x00,0x00,0x00,0x00,0x00,0x00};

void nokia5110_setup(void)
{
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  myGLCD.drawLine(0,10,83,10);
  myGLCD.drawLine(70,0,70,10);
  myGLCD.print("--:-- --/--",2,2);
  myGLCD.print(AIR_MONITOR_VERSION,55,15);
  myGLCD.print("------ By:ZPY",0,40);
  myGLCD.update(); 
}

void nokia5110_show_temp(int temperature, int humidity)
{
  myGLCD.drawBitmap(0,15, cls, 84, 25);
  myGLCD.update();
  myGLCD.drawBitmap(35, 30, temperature_ico, 6, 8);
  myGLCD.setFont(SmallFont);
  myGLCD.print("%",73,32);
  myGLCD.setFont(BigNumbers);
  myGLCD.printNumI(temperature, 5,15);
  myGLCD.printNumI(humidity, 45,15);
  myGLCD.update();
}

void nokia5110_show_pm25(int pm25)
{
  myGLCD.drawBitmap(0, 15, cls, 84, 25);
  myGLCD.update();
  myGLCD.drawBitmap(8, 15, pm2_5_ico, 30, 24);
  myGLCD.setFont(BigNumbers);
  myGLCD.printNumI(pm25, 40, 15);
  myGLCD.update();
}
#else
void nokia5110_setup(void)
{
}

void nokia5110_show_temp(int temperature, int humidity)
{
}

void nokia5110_show_pm25(int pm25)
{
}

#endif

