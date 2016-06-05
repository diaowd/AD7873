#define TESTING 3
#include <SPI.h>
#include <AD7873.h>
#define cspin 15
#define busypin 5
#define irqpin 2

#define X_RES 240 //I had the one from Adafruit
#define Y_RES 320

#define FINGERTIME 200
long timeISR = 0;

int16_t xbuff, ybuff;

char low[4] = "low", upp[4] = "upp";

AD7873 touch = AD7873(cspin, busypin, irqpin, X_RES, Y_RES);

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(cspin, OUTPUT);
  digitalWrite(cspin, HIGH);

  if (!touch.begin()) {
    Serial.println("Not found!");
    while (1);
  }
  else
  {
    Serial.println("Good to go!");
    attachInterrupt(digitalPinToInterrupt(irqpin), pressed, FALLING);
  }
}

void loop() {
}

void pressed() {
  detachInterrupt(digitalPinToInterrupt(irqpin));
  if ((millis() - timeISR) > FINGERTIME)
  {
    ybuff = touch.readY();
    xbuff = touch.readX();

    if (ybuff < 2000)
    {
      Serial.print("y_"); Serial.print("low"); Serial.print(": "); Serial.println(ybuff);
    }
    else
    {
      Serial.print("y_"); Serial.print("upp"); Serial.print(": "); Serial.println(ybuff);
    }

    if (xbuff < 2000)
    {
      Serial.print("x_"); Serial.print("low"); Serial.print(": "); Serial.println(xbuff);
    }
    else
    {
      Serial.print("x_"); Serial.print("upp"); Serial.print(": "); Serial.println(xbuff);
    }
  }
  timeISR = millis();
  attachInterrupt(digitalPinToInterrupt(irqpin), pressed, FALLING);
}
