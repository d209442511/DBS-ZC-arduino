#define BLINKER_PRINT Serial   
#define BLINKER_WIFI
#define BLINKER_MIOT_LIGHT
#include <FastLED.h>
#include <Blinker.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>


/* 联网测试指示灯*/
BlinkerButton Button("btn-abc");
//五个开关按键 一个滑条 一个调色盘
BlinkerButton Button1("ButtonKey1");
BlinkerButton Button2("ButtonKey2");
BlinkerButton Button3("ButtonKey3");
BlinkerButton Button4("ButtonKey4");
BlinkerButton Button5("ButtonKey5");
BlinkerSlider Slider1("SliderKey");
BlinkerRGB RGBWS2812("RGBKey");



char auth[] = "164c25097b03";/****秘钥****/
#define PIN 15        //  DIN PIN (GPIO15, D8)  //引脚
#define NUMPIXELS 16  // Number of you led      //灯珠数
CRGB leds[NUMPIXELS];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);//////////////////////Adafruit_NeoPixel


int openState = 0;/////////////////////////////灯效模式，默认关灯
int freq_flash = 25;/////////////////////////灯效节奏
uint8_t colorR, colorG, colorB, colorW;
bool wsState;
uint8_t wsMode = BLINKER_CMD_MIOT_DAY;
int brt_set = 100;
bool WIFI_Status = true;

/*微信配网*/
void smartConfig()//配网函数
{
  WiFi.mode(WIFI_STA);
  Serial.println("\r\nWait for Smartconfig...");
  WiFi.beginSmartConfig();//等待手机端发出的用户名与密码
  while (1)
  {
    Serial.print(".");
    digitalWrite(LED_BUILTIN, HIGH);  
    delay(1000);                      
    digitalWrite(LED_BUILTIN, LOW);    
    delay(1000);                      
    if (WiFi.smartConfigDone())//退出等待
    {
      Serial.println("SmartConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      break;
    }
  }
}

void WIFI_Set()
{
    Serial.begin(115200);
    delay(10);
    pinMode (LED_BUILTIN, OUTPUT);
    Serial.println("\r\n正在连接");
    int count = 0;
    while(WiFi.status()!=WL_CONNECTED)
    {
        if(WIFI_Status)
        {
            Serial.print(".");
            digitalWrite(LED_BUILTIN, HIGH);  
            delay(500);                       
            digitalWrite(LED_BUILTIN, LOW);    
            delay(500);                 
            count++;
            if(count>=5)//5s
            {
                WIFI_Status = false;
                Serial.println("WiFi连接失败，请用手机进行配网"); 
            }
        }
        else
        {
            smartConfig();  //微信智能配网
        }
     }  
     Serial.printf("连接成功");  
     Serial.print("IP:");
     Serial.println(WiFi.localIP());
}


// 按下按键即会执行联网测试指示灯
void Button_callback(const String & state)
{
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}



void pixelShow()
{

  pixels.setBrightness(colorW);

  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, colorR, colorG, colorB);

  }
  pixels.show();

}
/////////////////////////////////////////频率读取


void tip_led()
{
  digitalWrite(2, LOW);
  digitalWrite(2, HIGH);
  delay(50);
  digitalWrite(2, LOW);
  Blinker.vibrate();
}

void slider1_callback(int32_t value)
{
  tip_led();
  Blinker.vibrate();///震动
  BLINKER_LOG("get slider value: ", value);
  freq_flash = value;
}


////////////////////////////////////////////////调色盘

void ws2812_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
{
  tip_led();
  button_clear();
  BLINKER_LOG("R value: ", r_value);
  BLINKER_LOG("G value: ", g_value);
  BLINKER_LOG("B value: ", b_value);
  BLINKER_LOG("Rrightness value: ", bright_value);
  /////颜色和亮度赋值
  colorR = r_value;
  colorG = g_value;
  colorB = b_value;
  colorW = bright_value;
  openState = 6;
  //Text1.print("灯光模式：" , "单颜色");
}



void button_clear()//按键相应
{
  Button1.print("off");
  Button2.print("off");
  Button3.print("off");
  Button4.print("off");
  Button5.print("off");
  Button1.color("#008000");
  Button2.color("#008000");
  Button3.color("#008000");
  Button4.color("#008000");
  Button5.color("#008000");
}

///////////////////////////////////////////////////////////////////btn1
void button1_callback(const String & state)
{
  tip_led();
  BLINKER_LOG("get button state: ", state);

  if (state == BLINKER_CMD_ON) {
    BLINKER_LOG("日光模式开启");
    tip_led();
    button_clear();
    Button1.color("#DC143C");
    Button1.print("on");
    openState = 1;
  }
  else if (state == BLINKER_CMD_OFF) {
    BLINKER_LOG("日光模式关闭!");
    tip_led();
    button_clear();
    Button1.color("#008000");
    openState = 0;
  }

}




//////////////////////////////////////////////////////////btn2

void button2_callback(const String & state)
{
  tip_led();
  BLINKER_LOG("get button state: ", state);

  if (state == BLINKER_CMD_ON) {
    BLINKER_LOG("月光模式开启");
    tip_led();
    button_clear();
    Button2.print("on");
    Button2.color("#DC143C");
    openState = 2;
  }
  else if (state == BLINKER_CMD_OFF) {
    BLINKER_LOG("月光模式关闭!");
    tip_led();
    button_clear();
    openState = 0;
  }

}


///////////////////////////////////////////////////////////////////3
void button3_callback(const String & state)
{
  tip_led();
  BLINKER_LOG("get button state: ", state);

  if (state == BLINKER_CMD_ON) {
    BLINKER_LOG("温馨模式开启");
    tip_led();
    button_clear();
    Button3.print("on");
    Button3.color("#DC143C");
    openState = 3;
    brt_set = colorW;
  }
  else if (state == BLINKER_CMD_OFF) {
    BLINKER_LOG("温馨模式关闭!");
    tip_led();
    button_clear();
    openState = 0;
  }

}

////////////////////////////////////////////////////////////////////////4

void button4_callback(const String & state)
{
  tip_led();
  BLINKER_LOG("get button state: ", state);

  if (state == BLINKER_CMD_ON) {
    BLINKER_LOG("电脑模式开启");
    tip_led();
    button_clear();
    Button4.print("on");
    Button4.color("#DC143C");
    openState = 4;
  }
  else if (state == BLINKER_CMD_OFF) {
    BLINKER_LOG("电脑模式关闭!");
    tip_led();
    button_clear();
    openState = 0;
  }

}

////////////////////////////////////////////////////////////////////////
void button5_callback(const String & state)
{
  tip_led();
  BLINKER_LOG("get button state: ", state);

  if (state == BLINKER_CMD_ON) {
    BLINKER_LOG("电视模式开启");
    tip_led();
    button_clear();
    Button5.print("on");
    Button5.color("#DC143C");
    openState = 5;
  }
  else if (state == BLINKER_CMD_OFF) {
    BLINKER_LOG("电视模式关闭!");
    tip_led();
    button_clear();
    openState = 0;

  }

}



void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);

  Blinker.vibrate();

  uint32_t BlinkerTime = millis();

  Blinker.print("millis", BlinkerTime);
}


uint32_t getColor()
{

  uint32_t color = colorR << 16 | colorG << 8 | colorB;

  return color;
}


void miotPowerState(const String & state)//语音助手开关灯
{
  BLINKER_LOG("need set power state: ", state);

  if (state == BLINKER_CMD_ON) {
    tip_led();
    BlinkerMIOT.powerState("on");
    BlinkerMIOT.print();

    wsState = true;

    if (colorW == 0) colorW = 255;
    openState = 6;
  }
  else if (state == BLINKER_CMD_OFF) {
    tip_led();

    BlinkerMIOT.powerState("off");
    BlinkerMIOT.print();

    wsState = false;
    openState = 0;
  }
}

void miotColor(int32_t color)
{
  BLINKER_LOG("need set color: ", color);

  colorR = color >> 16 & 0xFF;
  colorG = color >>  8 & 0xFF;
  colorB = color       & 0xFF;

  BLINKER_LOG("colorR: ", colorR, ", colorG: ", colorG, ", colorB: ", colorB);
  openState = 6;
  //pixelShow();

  BlinkerMIOT.color(color);
  BlinkerMIOT.print();
}

void miotMode(uint8_t mode)
{
  BLINKER_LOG("need set mode: ", mode);

  if (mode == BLINKER_CMD_MIOT_DAY) {
    // Your mode function
    button_clear();
    Button1.print("on");
    Button1.color("#DC143C");
    openState = 1;

  }
  else if (mode == BLINKER_CMD_MIOT_NIGHT) {
    // Your mode function
    button_clear();
    Button2.print("on");
    Button2.color("#DC143C");
    openState = 2;
  }
  else if (mode == BLINKER_CMD_MIOT_COLOR) {
    // Your mode function
    button_clear();
    openState = 6;

  }
  else if (mode == BLINKER_CMD_MIOT_WARMTH) {
    // Your mode function
    button_clear();
    Button3.print("on");
    Button3.color("#DC143C");
    openState = 3;
  }
  else if (mode == BLINKER_CMD_MIOT_TV) {
    // Your mode function
    button_clear();
    Button5.print("on");
    Button5.color("#DC143C");
    openState = 5;
  }
  else if (mode == BLINKER_CMD_MIOT_READING) {
    // Your mode function
  }
  else if (mode == BLINKER_CMD_MIOT_COMPUTER) {
    // Your mode function
    button_clear();
    Button4.print("on");
    Button4.color("#DC143C");
    openState = 4;
  }

  wsMode = mode;

  BlinkerMIOT.mode(mode);
  BlinkerMIOT.print();
}

void miotBright(const String & bright)
{
  BLINKER_LOG("need set brightness: ", bright);

  colorW = bright.toInt();

  BLINKER_LOG("now set brightness: ", colorW);

  pixelShow();

  BlinkerMIOT.brightness(colorW);
  BlinkerMIOT.print();
  openState = 6;
}

void miotColoTemp(int32_t colorTemp)
{
  BLINKER_LOG("need set colorTemperature: ", colorTemp);

  BlinkerMIOT.colorTemp(colorTemp);
  BlinkerMIOT.print();
}

void miotQuery(int32_t queryCode)
{
  BLINKER_LOG("MIOT Query codes: ", queryCode);

  switch (queryCode)
  {
    case BLINKER_CMD_QUERY_ALL_NUMBER :
      BLINKER_LOG("MIOT Query All");
      BlinkerMIOT.powerState(wsState ? "on" : "off");
      BlinkerMIOT.color(0);
      BlinkerMIOT.mode(0);
      BlinkerMIOT.colorTemp(1000);
      BlinkerMIOT.brightness(1);
      BlinkerMIOT.print();
      break;
    case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
      BLINKER_LOG("MIOT Query Power State");
      BlinkerMIOT.powerState(wsState ? "on" : "off");
      BlinkerMIOT.print();
      break;
    case BLINKER_CMD_QUERY_COLOR_NUMBER :
      BLINKER_LOG("MIOT Query Color");
      BlinkerMIOT.color(0);
      BlinkerMIOT.print();
      break;
    case BLINKER_CMD_QUERY_MODE_NUMBER :
      BLINKER_LOG("MIOT Query Mode");
      BlinkerMIOT.mode(0);
      BlinkerMIOT.print();
      break;
    case BLINKER_CMD_QUERY_COLORTEMP_NUMBER :
      BLINKER_LOG("MIOT Query ColorTemperature");
      BlinkerMIOT.colorTemp(1000);
      BlinkerMIOT.print();
      break;
    case BLINKER_CMD_QUERY_BRIGHTNESS_NUMBER :
      BLINKER_LOG("MIOT Query Brightness");
      BlinkerMIOT.brightness(1);
      BlinkerMIOT.print();
      break;
    default :
      BlinkerMIOT.powerState(wsState ? "on" : "off");
      BlinkerMIOT.color(0);
      BlinkerMIOT.mode(0);
      BlinkerMIOT.colorTemp(1000);
      BlinkerMIOT.brightness(1);
      BlinkerMIOT.print();
      break;
  }
}






/**************************灯光效果程序**************************
 * **************************************************
 * **************************************************/

////////////////////////////////////////////////彩虹灯
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    if (active())
    {
      break;
    }
    pixels.show();
    delay(freq_flash);
  }
}



////////////////////////////////////////////////////////////////
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
    if (active())
    {
      break;
    }
  }
}

void colorScan()//跑马灯
{
  colorWipe(pixels.Color(255, 0, 0), freq_flash); // Red
  colorWipe(pixels.Color(0, 255, 0), freq_flash); // Green
  colorWipe(pixels.Color(0, 0, 255), freq_flash);
  colorWipe(pixels.Color(0, 255, 255), freq_flash);
  colorWipe(pixels.Color(255, 0, 255), freq_flash);
  colorWipe(pixels.Color(255, 0, 0), freq_flash);

}

///////////////////////////////////FASTLEDS的示例效果
void fadeall() {
  for (int i = 0; i < NUMPIXELS; i++) {
    leds[i].nscale8(250);
  }
}


void cylon() {
  static uint8_t hue = 0;

  // First slide the led in one direction
  for (int i = 0; i < NUMPIXELS; i++) {
    // Set the i'th led to red
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    if (active())
    {
      break;
    }
    delay(freq_flash);
  }

  // Now go in the other direction.
  for (int i = (NUMPIXELS) - 1; i >= 0; i--) {
    // Set the i'th led to red
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    if (active())
    {
      break;
    }
    delay(freq_flash);
  }
}


void breath()//呼吸灯
{
  wsState = true;

  for (int brt = 1; brt < brt_set + 1; brt++) {
    colorW = brt;
    if (colorW % 5 == 0)
    {
      pixelShow();
    }
    if (active())
    {
      break;
    }
    delay(freq_flash * 2);
  }
  for (int brt = brt_set; brt > 0 ; brt--) {
    colorW = brt;
    if (colorW % 5 == 0)
    {
      pixelShow();
    }
    if (active())
    {
      break;
    }
    delay(freq_flash);

  }

}


void setup()
{
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  LEDS.addLeds<WS2812, PIN, RGB>(leds, NUMPIXELS);
  pixels.begin();
  pixels.show();
  bool WIFI_Status = true;
  Blinker.attachData(dataRead);

  WIFI_Set();
  // 初始化blinker
    Blinker.begin(auth, WiFi.SSID().c_str(), WiFi.psk().c_str());

  BlinkerMIOT.attachPowerState(miotPowerState);
  BlinkerMIOT.attachColor(miotColor);
  BlinkerMIOT.attachMode(miotMode);
  BlinkerMIOT.attachBrightness(miotBright);
  BlinkerMIOT.attachColorTemperature(miotColoTemp);
  BlinkerMIOT.attachQuery(miotQuery);
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  Slider1.attach(slider1_callback);
  RGBWS2812.attach(ws2812_callback);
  Button1.attach(button1_callback);
  Button2.attach(button2_callback);
  Button3.attach(button3_callback);
  Button4.attach(button4_callback);
  Button5.attach(button5_callback);
  pixels.setBrightness(0);
  button_clear();
}















void mode_1()
{
  colorW = 50;
  cylon();
}

void mode_2()
{

  colorR = 25;
  colorG = 0;
  colorB = 240;
  colorW = 50;
  wsState = true;
  pixelShow();
}

void mode_3()
{

  breath();
}

void mode_4()
{
  rainbow(freq_flash);
  colorW = 50;
}


void mode_5()
{
  colorW = 50;
  colorScan();
}



unsigned int nowState = 0;

bool active()
{
  Blinker.run();
  if (openState != nowState)
  {
    nowState = openState;
    Serial.println("Operating in new mode " + String(openState));
    return true;
  }
  if (openState == nowState)
  {
    //Serial.println("Operating in same mode " + String(openState));
    return false;

  }
}













void loop()
{

  Blinker.run();


  switch (openState)
  {
    case 0:
      wsState = false;
      openState = 0;
      colorR = 255;
      colorG = 255;
      colorB = 255;
      colorW = 0;
      pixelShow();
      break;
    case 1:
      mode_1();
      break;

    case 2:
      mode_2();
      break;
    case 3:
      mode_3();
      break;
    case 4:
      mode_4();
      break;
    case 5:
      mode_5();
      break;
    case 6:
      pixelShow();
      break;

    default:

      break;




  }



}
