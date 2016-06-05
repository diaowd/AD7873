#include "Arduino.h"

#include <SPI.h>
#define WAIT 50
#include "AD7873.h"


#if defined (SPI_HAS_TRANSACTION)   // esp8266 core SPI
static SPISettings mySPISettings;
#elif defined (__AVR__)   // avr core SPI
static uint8_t SPCRbackup;
static uint8_t mySPCR;
#endif

// Default screen end limit values. Calibrate for your own


AD7873::AD7873(uint8_t cspin, uint8_t busypin, uint8_t irqpin, int16_t y_low, int16_t  y_upp, int16_t  x_low, int16_t  x_upp, uint16_t x_res,  uint16_t y_res)
{
  calibrating = false;
  _YLOW = y_low;
  _YUPP = y_upp;
  _XLOW = x_low;
  _XUPP = x_upp;
  _XRES =  x_res;
  _YRES =  y_res;
  _CS = cspin;
  _BUSY = busypin;
  _IRQ = irqpin;
}
//  _YLOW = 390; // calibrated values for the Adafruit one I had
//  _YUPP = 3780;
//  _XLOW = 350;
//  _XUPP = 3737;

AD7873::AD7873(uint8_t cspin, uint8_t busypin, uint8_t irqpin, uint16_t x_res, uint16_t y_res)
{
  calibrating = true;
  _YLOW = 0; // calibrated values for the Adafruit one I had
  _YUPP = 4000;
  _XLOW = 0;
  _XUPP = 4000;

  _XRES =  x_res;
  _YRES =  y_res;
  _CS = cspin;
  _BUSY = busypin;
  _IRQ = irqpin;
}

bool AD7873::begin()
{
  pinMode(_BUSY, INPUT);
  pinMode(_IRQ, INPUT);

  pinMode(_CS, OUTPUT);
  digitalWrite(_CS, HIGH);
#if defined (SPI_HAS_TRANSACTION)
  SPI.begin();
  mySPISettings = SPISettings(1000000, MSBFIRST, SPI_MODE0); // 1Mhz seems safe, didnt read the datasheet, still works
#elif defined (__AVR__)
  SPCRbackup = SPCR;
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE0);
  mySPCR = SPCR; // save our preferred state
  //Serial.print("mySPCR = 0x"); Serial.println(SPCR, HEX);
  SPCR = SPCRbackup;  // then restore
#elif defined (__arm__)
  SPI.begin();
  SPI.setClockDivider(84);
  SPI.setDataMode(SPI_MODE0);
#endif
  m_spiMode = SPI_MODE0;

  return reqID();
}


bool AD7873::reqID()
{
#if defined (SPI_HAS_TRANSACTION)
  SPI.beginTransaction(mySPISettings);
#endif
  digitalWrite(_CS, LOW);
  SPI.transfer(ID_REQ); // request to read from WHO_AM_I register on AD7873
  delayMicroseconds(WAIT);
  uint16_t buff = transfer16(); // read from WHO_AM_I register
  digitalWrite(_CS, HIGH);
#if defined (SPI_HAS_TRANSACTION)
  SPI.endTransaction();
#endif
  Serial.println(buff, BIN);
  if (buff == ID) return true;
  else return false;
}

uint16_t AD7873::readY()
{
start:
  uint16_t x;
  start();
  SPI.transfer(READ_Y);
  delayMicroseconds(WAIT);
  x = transfer16();
  end();
  if (x == 0)
  {
    goto start;
  }
  else
  {
    if (calibrating == true)
    {
      return (x >> 3);
    }
    else
    {
      return constrain(map((x >> 3), _YLOW, _YUPP, 0, _YRES), 0, _YRES); // Y limits depends on your touchscreen
    }
  }


}

uint16_t AD7873::readX()
{
start:
  uint16_t x;
  start();
  SPI.transfer(READ_X);
  delayMicroseconds(WAIT);
  x = transfer16();
  end();
  if (x == 0)
  {
    goto start;
  }
  else
  {
    if (calibrating == true)
    {
      return (x >> 3);
    }
    else
    {
      return  constrain(map(((x >> 3)), _XUPP, _XLOW, 0, _XRES), 0, _XRES); //x coords are inverted according datasheet
    }
  }
}
void AD7873::start(void)
{
#if defined (SPI_HAS_TRANSACTION)
  SPI.beginTransaction(mySPISettings);
#endif
  digitalWrite(_CS, LOW);
}

void AD7873::end(void)
{
  digitalWrite(_CS, HIGH);
#if defined (SPI_HAS_TRANSACTION)
  SPI.endTransaction();
#endif
}

uint16_t AD7873::transfer16(void)
{
  uint16_t high = SPI.transfer(0);
  high = high << 8;
  uint8_t buff = SPI.transfer(0);
  high = buff | high;
  return high;
}

