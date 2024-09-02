#include "PrecisTinyOled.h"
#include "fonts.h"

#ifndef Wire
	#include <Wire.h>
#endif

PrecisTinyOled::PrecisTinyOled() { }

bool PrecisTinyOled::_wireStarted = 0;

void PrecisTinyOled::I2C(long SPD) { _SPD = SPD; }

void PrecisTinyOled::_write(uint8_t com, uint8_t mode)
{
	Wire.beginTransmission(address);
	Wire.write(mode ? 0x40 : 0x80);
	Wire.write(com);
	Wire.endTransmission();
}
void PrecisTinyOled::_stream(uint8_t com, uint8_t mode)
{
	Wire.write(mode ? 0x40 : 0x80);
	Wire.write(com);
}

void PrecisTinyOled::_oCursor(uint8_t x, uint8_t y, uint8_t xlen, uint8_t ylen)
{
	Wire.beginTransmission(address);
	_stream(0x21, 0);
	_stream(x, 0);
	_stream(x + xlen - 1, 0);
	_stream(0x22, 0);
	_stream(y, 0);
	_stream(y + ylen - 1, 0);
	Wire.endTransmission();
}

void PrecisTinyOled::clear()
{
	_oCursor(0, 0, width, height/8);
	for (int i = 0; i < (height / 8) * width; i++)
	{
		_write(0,1);
	}
	_oCursor(0, 0, 0, 0);
}

void PrecisTinyOled::begin(uint8_t addr, uint8_t w, uint8_t h)
{
	address = addr;
	width = w;
	height = h;
	currX = 0;
	currY = 0;
	_currFontSize = 1;
	_currFontClr = 1;
	_wordWrapState = 0;
	if (_wireStarted != 1)
	{
		Wire.begin();
		if (_SPD > 255) Wire.setClock(_SPD);
		_wireStarted = 1;
	}
	Wire.beginTransmission(address);
	_stream(0xA8, 0);
	_stream(height == 32 ? 0x1F : 0x3F, 0);
	_stream(0x22, 0);
	_stream(0x00, 0);
	_stream(0x03, 0);
	_stream(0x20, 0);
	_stream(0x00, 0);
	_stream(0xDA, 0);
	_stream(height == 32 ? 0x02 : 0x12, 0);
	_stream(0x8D, 0);
	_stream(0x14, 0);
	_stream(0xAF, 0);
	_stream(0xA1, 0);
	_stream(0xC8, 0);
	Wire.endTransmission();
	clear();
}

void PrecisTinyOled::power(bool on)
{
	_write(on ? 0xAF : 0xAE, 0);
}

void PrecisTinyOled::contrast(uint8_t c)
{
	Wire.beginTransmission(address);
	_stream(0x81, 0);
	_stream(c, 0);
	Wire.endTransmission();
}

void PrecisTinyOled::invert(bool on)
{
	_write(on ? 0xA7 : 0xA6, 0);
}

void PrecisTinyOled::cursor(uint8_t x, uint8_t y)
{
	if (currX > -1 && currX < width) currX = x;
	if (currY > -1 && currY < height && y % 8 == 0) currY = y;
}

void PrecisTinyOled::style(uint8_t fonts, bool clr)
{
	_currFontSize = fonts;
	_currFontClr = clr;
}

void PrecisTinyOled::_writePage(uint8_t col, uint8_t page, uint8_t pageData)
{
	_oCursor(col, page, 1, 1);
	_write(pageData, 1);
}

void PrecisTinyOled::_text(uint8_t x, uint8_t y, char txt, uint8_t textSize, bool clr)
{
	char txtC = txt - (txt - 32 > 64 ? 64 : 32);
	uint8_t cByte = floor(txtC / 2);
	uint8_t cm = txtC % 2 ? 3 : 0;
	uint8_t rowSizer, colSizer, byteSizer, newPage, charLine;
	for (uint8_t row = 0; row < textSize && y + row * 8 < height; row++)
	{
		for (uint8_t col = 0; col < 3 * textSize && x + col < width; col++)
		{
			colSizer = floor(col / textSize);
			if (!clr) newPage = 0;
			else if (textSize == 1) newPage = pgm_read_byte(FONT + cByte * 6 + cm + colSizer);
			else
			{
				newPage = 0;
				charLine = pgm_read_byte(FONT + cByte * 6 + cm + colSizer);
				for (uint8_t sc = 0; sc < 8; sc++)
				{
					byteSizer = floor(sc / textSize) + (row * 8 / textSize);
					newPage = newPage | ((charLine & (1 << byteSizer)  ? 1 : 0) << sc);
				}
			}
			_writePage(x + col, floor(y / 8) + row, newPage);
		}
	}
}

void PrecisTinyOled::icon(uint8_t x, uint8_t y, Icon icond, uint8_t iconSize, bool clr)
{
	uint8_t rowSizer, colSizer, byteSizer, newPage, iconLine;
	for (uint8_t row = 0; row < iconSize && y + row * iconSize < height; row++)
	{
		rowSizer = floor(row / iconSize);
		for (uint8_t col = 0; col < 8 * iconSize && x + col < width; col++)
		{
			colSizer = floor(col / iconSize);
			if (!clr) newPage = 0;
			else if (iconSize == 1) newPage = icond[colSizer];
			else
			{
				newPage = 0;
				for (uint8_t sc = 0; sc < 8; sc++)
				{
					byteSizer = floor(sc / iconSize) + (row * 8 / iconSize);
					newPage = newPage | ((icond[rowSizer + colSizer] & (1 << byteSizer)  ? 1 : 0) << sc);
				}
			}
			_writePage(x + col, floor(y / 8) + row, newPage);
		}
	}
}

void PrecisTinyOled::number(uint8_t x, uint8_t y, const char * n, uint8_t iconSize, bool clr)
{
	if (x == 255) x = (floor((width - strlen(n) * iconSize * 8) / 2) / 8) * 8;
	currX = x;
	currY = y;
	uint8_t val;
	for (uint8_t i = 0; i < strlen(n); i++)
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

void PrecisTinyOled::number(uint8_t x, uint8_t y, uint32_t val, uint8_t iconSize, bool clr)
{
	char num[24];
	ultoa(val, num, 10);
	number(x, y, num, iconSize, clr);
}

void PrecisTinyOled::image(uint8_t x, uint8_t y, uint8_t w, uint8_t hb, const uint8_t *img, bool clr)
{
	for (uint8_t row = 0; row < hb && hb < height; row++)
	{
		for (uint8_t col = 0; col < w && x + col < width; col++)
		{
			_writePage(x + col, floor(y / 8) + row, clr ? img[row + col] : 0);
		}
	}
}

void PrecisTinyOled::image(uint8_t x, uint8_t y, uint8_t w, uint8_t hb, uint8_t *img, bool clr)
{
	for (uint8_t row = 0; row < hb && hb < height; row++)
	{
		for (uint8_t col = 0; col < w && x + col < width; col++)
		{
			_writePage(x + col, floor(y / 8) + row, clr ? img[row + col] : 0);
		}
	}
}
 
void PrecisTinyOled::CR()
{
	currX = 0;
	uint8_t oldY = currY;
	currY += _currFontSize * 8;
	if (currY > height - 1 - _currFontSize * 8)
	{
		cursor(0, 0);
		clear();
	}
}

int PrecisTinyOled::_charPos(const char * haystack, char needle, uint8_t startPos)
{
	for (int i=startPos;i<strlen(haystack);i++)
	{
		if (haystack[i] == needle) return i;
	}
	return -1;
}

void PrecisTinyOled::print(const char * txt)
{
	int wrapPos, wrapPos2;
	bool wrapSkip = 0;
	uint8_t hyphenate = 0;
	for (uint8_t i = 0; i < strlen(txt); i++)
	{
		if (_wordWrapState && txt[i] != ' ' && i > 0 && (txt[i-1] == ' '))
		{
			wrapPos = _charPos(txt, ' ', i ) - i;
			wrapPos2 = _charPos(txt, '-', i ) - i;
			wrapPos = wrapPos < 0 ? strlen(txt) - i : wrapPos;
			wrapPos2 = wrapPos2 < 0 ? strlen(txt) - i : wrapPos2;
			if  (currX + (wrapPos *  _currFontSize * 4) > width - 1)
			{
				if (currX + (wrapPos2 *  _currFontSize * 4) <= width - 1) hyphenate = wrapPos2 + i;
				else
				{
					CR();
					if (txt[i] == 32) wrapSkip = 1;
				}
			}
		}
		if (txt[i] == '\n') CR(); 
		else
		{
			if (currX > width - (_currFontSize * 8)) CR();
			if (!wrapSkip)
			{
				_text(currX, currY, txt[i], _currFontSize, _currFontClr);
				currX += _currFontSize * 4;
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

void PrecisTinyOled::print(uint32_t val)
{
	char num[24];
	ultoa(val, num, 10);
	print(num);
}


void PrecisTinyOled::println(const char * txt)
{
	print(txt);
	CR();
}

void PrecisTinyOled::println(uint32_t val)
{
	print(val);
	CR();
}

void PrecisTinyOled::printc(const char * txt)
{
	cursor((floor((width - 1 - strlen(txt) * _currFontSize * 3) / 2) / 8) * 8, currY);
	print(txt);
}

void PrecisTinyOled::printc(uint32_t val)
{
	char num[11];
	ultoa(val, num, 10);
	printc(num);
}

void PrecisTinyOled::printBinary(uint32_t val, uint8_t size, uint8_t brk = 0)
{
	if (size > 24) size = 32;
	else if (size > 16) size = 24;
	else if (size > 8) size = 16;
	else size = 8;
	char bit[64];
	uint8_t cnt = 0;
	for (uint8_t i = 0; i < size; i++)
	{
		bit[cnt] = bitRead(val, size - i - 1) ? '1' : '0';
		cnt++;
		if (brk && i > 0 && (i % 8) == 0)
		{
			bit[cnt] = ' ';
			cnt++;
		}
	}
	bit[cnt] = '\0';
	print(bit);
}

void PrecisTinyOled::wrapMode(bool state)
{
	_wordWrapState = state;
}

void PrecisTinyOled::progressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t progress)
{
	for (uint8_t i = 0; i < width; i += 8)
	{
		icon(x + i, y, i * 100 / width <= progress ? iFILL : iSHADE, 1, 1);
	}
}