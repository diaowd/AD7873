#ifndef _AD7873_H_
#define _AD7873_H_
#endif

#include <SPI.h>

#define CTRL_REG 0x87
#define MODE 3
#define MODE_8BIT 1
#define MODE_12BIT 0
#define ID 0x4000
#define ID_REQ 0xE0
#define READ_X 0xD4
#define READ_Y 0x94
#define READ_Z1 0xB4
#define READ_Z2 0xC4


class AD7873 {
  public:
    AD7873(uint8_t, uint8_t, uint8_t, uint16_t, uint16_t);
    AD7873(uint8_t, uint8_t, uint8_t, int16_t, int16_t , int16_t , int16_t , uint16_t,  uint16_t);
    bool begin();
    bool reqID();
    uint16_t readY();
    uint16_t readX();
    
    void start(void);
    void end(void);
    void toggle(void);
    uint16_t transfer16(void);

  private:
    uint8_t  _CS, _BUSY, _IRQ, calibrating;
    int16_t _YLOW, _YUPP, _XLOW, _XUPP;
    uint16_t _XRES, _YRES;
    int m_spiMode;
};

