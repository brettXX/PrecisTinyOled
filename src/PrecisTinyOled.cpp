#include "PrecisTinyOled.h"
#include "fonts.h"

PrecisTinyOled::PrecisTinyOled() {}

void PrecisTinyOled::I2C(long SPD)
{
  _SPD = SPD;
}

void PrecisTinyOled::_write(byte com, byte mode)
{
  Wire.beginTransmission(address);
  Wire.write(mode ? 0x40 : 0x80);
  Wire.write(com);
  Wire.endTransmission();
}

void PrecisTinyOled::_oCursor(byte x, byte y, byte xlen, byte ylen)
{
  _write(0x21, 0);
  _write(x, 0);
  _write(x + xlen - 1, 0);
  _write(0x22, 0);
  _write(y, 0);
  _write(y + ylen - 1, 0);
}

void PrecisTinyOled::clear()
{
  for (int r = 0; r < height / 8; r++)
  {
    _write(0x21, 0);
    _write(0, 0);
    _write(127, 0);
    _write(0x22, 0);
    _write(r, 0);
    _write(r, 0);
    for (int i = 0; i < width; i++)
    {
      _write(0, 1);
    }
  }
  _oCursor(0, 0, 0, 0);
}

void PrecisTinyOled::begin(byte addr, byte w, byte h)
{
  address = addr;
  width = w;
  height = h;
  currX = 0;
  currY = 0;
  _currFontSize = 1;
  _currFontClr = 1;
  Wire.begin();
  if (_SPD > 255) Wire.setClock(_SPD);
  _write(0xA8, 0);
  _write(height == 32 ? 0x1F : 0x3F, 0);
  _write(0x22, 0);
  _write(0x00, 0);
  _write(0x03, 0);
  _write(0x20, 0);
  _write(0x00, 0);
  _write(0xDA, 0);
  _write(height == 32 ? 0x02 : 0x12, 0);
  _write(0x8D, 0);
  _write(0x14, 0);
  _write(0xAF, 0);
  _write(0xA1, 0);
  _write(0xC8, 0);
  clear();
}

void PrecisTinyOled::power(bool on)
{
  _write(on ? 0xAF : 0xAE, 0);
}

void PrecisTinyOled::contrast(byte c)
{
  _write(0x81, 0);
  _write(c, 0);
}

void PrecisTinyOled::invert(bool on)
{
  _write(on ? 0xA7 : 0xA6, 0);
}

void PrecisTinyOled::cursor(byte x, byte y)
{
  if (currX > -1 && currX < width) currX = x;
  if (currY > -1 && currY < height && y % 8 == 0) currY = y;
}

void PrecisTinyOled::style(byte fonts, bool clr)
{
  _currFontSize = fonts;
  _currFontClr = clr;
}

void PrecisTinyOled::_writePage(byte col, byte page, byte pageData)
{
  _oCursor(col, page, 1, 1);
  _write(pageData, 1);
}

void PrecisTinyOled::_text(byte x, byte y, String txt, byte textSize, bool clr)
{
  char txtC;
  txtC = txt.charAt(0) - (txt.charAt(0) - 32 > 64 ? 64 : 32);
  byte cByte = floor(txtC / 2);
  byte cm = txtC % 2 ? 3 : 0;
  byte rowSizer, colSizer, byteSizer, newPage, charLine;
  for (byte row = 0; row < textSize && y + row * 8 < height; row++)
  {
    for (byte col = 0; col < 3 * textSize && x + col < width; col++)
    {
      colSizer = floor(col / textSize);
      if (!clr) newPage = 0;
      else if (textSize == 1) newPage = pgm_read_byte(FONT + cByte * 6 + cm + colSizer);
      else
      {
        newPage = 0;
        charLine = pgm_read_byte(FONT + cByte * 6 + cm + colSizer);
        for (byte sc = 0; sc < 8; sc++)
        {
          byteSizer = floor(sc / textSize) + (row * 8 / textSize);
          newPage = newPage | ((charLine & (1 << byteSizer)  ? 1 : 0) << sc);
        }
      }
      _writePage(x + col, floor(y / 8) + row, newPage);
    }
  }
}

void PrecisTinyOled::icon(byte x, byte y, Icon icond, byte iconSize, bool clr)
{
  byte rowSizer, colSizer, byteSizer, newPage, iconLine;
  for (byte row = 0; row < iconSize && y + row * iconSize < height; row++)
  {
    rowSizer = floor(row / iconSize);
    for (byte col = 0; col < 8 * iconSize && x + col < width; col++)
    {
      colSizer = floor(col / iconSize);
      if (!clr) newPage = 0;
      else if (iconSize == 1) newPage = icond[colSizer];
      else
      {
        newPage = 0;
        for (byte sc = 0; sc < 8; sc++)
        {
          byteSizer = floor(sc / iconSize) + (row * 8 / iconSize);
          newPage = newPage | ((icond[rowSizer + colSizer] & (1 << byteSizer)  ? 1 : 0) << sc);
        }
      }
      _writePage(x + col, floor(y / 8) + row, newPage);
    }
  }
}

void PrecisTinyOled::number(byte x, byte y, char * n, byte iconSize, bool clr)
{
  currX = x;
  currY = y;
  byte val;
  for (byte i = 0; i < strlen(n); i++)
  {
    switch (n[i])
    {
      case 48 ... 57:
        val = n[i] - 48;
        break;
      case 35:
        val = 10;
        break;
      case 36:
        val = 11;
        break;
      case 37:
        val = 12;
        break;
      case 43:
        val = 13;
        break;
      case 45:
        val = 14;
        break;
      case 46:
        val = 15;
        break;
      case 58:
        val = 16;
        break;
        default:
        val = 255;
        break;
    }
    if (val < 255)
    {
      icon(currX, y, number_array[val], iconSize, clr);
      currX += iconSize == 1 ? 8 : iconSize * 8;
    }
  }
}

void PrecisTinyOled::image(byte x, byte y, byte w, byte hb, const byte *img, bool clr)
{
  for (byte row = 0; row < hb && hb < height; row++)
  {
    for (byte col = 0; col < w && x + col < width; col++)
    {
      _writePage(x + col, floor(y / 8) + row, clr ? img[row + col] : 0);
    }
  }
}

void PrecisTinyOled::image(byte x, byte y, byte w, byte hb, byte *img, bool clr)
{
  for (byte row = 0; row < hb && hb < height; row++)
  {
    for (byte col = 0; col < w && x + col < width; col++)
    {
      _writePage(x + col, floor(y / 8) + row, clr ? img[row + col] : 0);
    }
  }
}

void PrecisTinyOled::CR()
{
  currX = 0;
  byte oldY = currY;
  currY += _currFontSize * 8;
  if (currY > height - 1 - _currFontSize * 8)
  {
    cursor(0, 0);
    clear();
  }
}

void PrecisTinyOled::print(String txt)
{
  String ch;
  int wrapPos, wrapPos2;
  bool wrapSkip = 0;
  byte hyphenate = 0;
  for (byte i = 0; i < txt.length(); i++)
  {
    ch = txt[i];
    if (_wordWrapState && ch != " " && i > 0 && (txt.charAt(i - 1) == 32 ))
    {
      wrapPos = txt.indexOf(" ", i ) - i;
      wrapPos = wrapPos < 0 ? txt.length() - i : wrapPos;
      wrapPos2 = txt.indexOf("-", i ) - i;
      wrapPos2 = wrapPos2 < 0 ? txt.length() - i : wrapPos2;
      if  (currX + (wrapPos * ( _currFontSize * 4 < 8 ? 8 : _currFontSize * 4)) > width - 1)
      {
        if (currX + (wrapPos2 * ( _currFontSize * 4 < 8 ? 8 : _currFontSize * 4)) <= width - 1) hyphenate = wrapPos2 + i;
        else
        {
          CR();
          if (ch == " ") wrapSkip = 1;
        }
      }
    }
    if (ch == String("\n")) CR();
    else
    {
      if (currX > width - (_currFontSize * 8)) CR();
      if (!wrapSkip)
      {
        _text(currX, currY, ch, _currFontSize, _currFontClr);
        currX += _currFontSize * 4 < 8 ? 8 : _currFontSize * 4;
      }
      wrapSkip = 0;
      if (hyphenate == i && i > 0)
      {
        CR();
        hyphenate = 0;
      }
    }
  }
}

void PrecisTinyOled::print(int txt)
{
  println(String(txt));
}
void PrecisTinyOled::print(long txt)
{
  println(String(txt));
}
void PrecisTinyOled::print(float txt)
{
  println(String(txt));
}
void PrecisTinyOled::println(String txt)
{
  print(txt + "\n");
}
void PrecisTinyOled::println(int txt)
{
  println(String(txt) + "\n");
}
void PrecisTinyOled::println(long txt)
{
  println(String(txt) + "\n");
}
void PrecisTinyOled::println(float txt)
{
  println(String(txt) + "\n");
}

void PrecisTinyOled::wrapMode(bool state)
{
  _wordWrapState = state;
}
