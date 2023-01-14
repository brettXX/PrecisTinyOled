#ifndef PrecisTinyOled_h
#define PrecisTinyOled_h
#include <Arduino.h>
#include <avr/pgmspace.h>

typedef const byte Icon[8];

#ifndef Wire
#include <Wire.h>
#endif

#define ON true
#define OFF false

class PrecisTinyOled
{
  public:
    static bool _wireStarted;
    byte address;
    byte width;
    byte height;
    byte currX;
    byte currY;
    PrecisTinyOled();
    void clear();
    void power(bool on);
    void contrast(byte c);
    void invert(bool on);
    void cursor(byte x, byte y);
    void style(byte fonts, bool clr);
    void icon(byte x, byte y, Icon icond, byte iconSize, bool clr);
    void number(byte x, byte y, char * n, byte iconSize, bool clr);
    void image(byte x, byte y, byte w, byte hb, byte *img, bool clr);
    void image(byte x, byte y, byte w, byte hb, const byte *img, bool clr);
    void CR();
    void print(char * txt);
    void wrapMode(bool state);
    void begin(byte addr, byte w, byte h);
    void I2C(long SPD);
    void progressBar(byte x, byte y, byte width, byte progress);
    
  private:
    long _SPD;
    byte _currFontSize;
    bool _currFontClr;
    bool _wordWrapState;
    void _write(byte com, byte mode);
    void _write(byte com);
    void _writePage(byte col, byte page, byte pageData);
    void _oCursor(byte x, byte y, byte xlen, byte ylen);
    void _text(byte x, byte y, char txt, byte textSize, bool clr);
    int _charPos(char * haystack, char needle, byte startPos);
    void _processImage(byte x, byte y, byte w, byte h,  byte *img, bool MEM);
};
#endif
