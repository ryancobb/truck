#include "M5CoreInk.h"
#include "UNIT_ENCODER.h"
#include "WiFi.h"
#include "secrets.h"

UNIT_ENCODER sensor;

Ink_Sprite InkPageSprite(&M5.M5Ink);

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

void ButtonTest(char* str){
  InkPageSprite.clear();  //clear the screen.
  InkPageSprite.drawString(0,59,str);  //draw the string.
  InkPageSprite.pushSprite(); //push the sprite.
  delay(2000);
}

void setupWifi() {
  while (status != WL_CONNECTED) {
    InkPageSprite.drawString(10, 10, "Attempting to connect to ");
    InkPageSprite.drawString(10, 25, ssid);
    InkPageSprite.pushSprite();

    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  char IP[] = "xxx.xxx.xxx.xxx";
  IPAddress ip = WiFi.localIP();
  ip.toString().toCharArray(IP, 16);

  InkPageSprite.clear();
  InkPageSprite.drawString(10, 10, "WiFi Connected!");
  InkPageSprite.drawString(10, 25, IP);
  InkPageSprite.pushSprite();
}

void setup() {
  M5.begin(); //Initialize CoreInk.
  if( !M5.M5Ink.isInit()){  //check if the initialization is successful.
    Serial.printf("Ink Init faild");
  }
  M5.M5Ink.clear(); //Clear the screen.
  delay(1000);
  //creat ink Sprite.
  if( InkPageSprite.creatSprite(0,0,200,200,true) != 0 ){
    Serial.printf("Ink Sprite creat faild");
  }

  // setupWifi();

  sensor.begin();
}

signed short int last_value = 0;

void loop() {
  signed short int encoder_value = sensor.getEncoderValue();
  bool btn_status = sensor.getButtonStatus();

  if (last_value != encoder_value) {
    if (last_value < encoder_value) {
      sensor.setLEDColor(1, 0x000011);
    } else {
      sensor.setLEDColor(2, 0x111100);
    }
    last_value = encoder_value;
  } else {
    sensor.setLEDColor(0, 0x001100);
  }
  if (!btn_status) {
    sensor.setLEDColor(0, 0xC800FF);
  }
  delay(20);


    // if( M5.BtnUP.wasPressed()) ButtonTest("Btn UP Pressed");  //Scroll wheel up.
    // if( M5.BtnDOWN.wasPressed()) ButtonTest("Btn DOWN Pressed");  //Trackwheel scroll down.
    // if( M5.BtnMID.wasPressed()) ButtonTest("Btn MID Pressed");  //Dial down.
    // if( M5.BtnEXT.wasPressed()) ButtonTest("Btn EXT Pressed");  //Top button press.
    // if( M5.BtnPWR.wasPressed()){  //Right button press.
    //     ButtonTest("Btn PWR Pressed");
    //     M5.shutdown();  //Turn off the power, restart it, you need to wake up through the PWR button.
    // }
  // M5.update();  //Refresh device button.
}
