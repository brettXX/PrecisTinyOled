#ifndef PrecisTinyOled_h
	#define PrecisTinyOled_h
	#include <Arduino.h>
	#ifdef __AVR__
		#include <avr/pgmspace.h>
	#endif
	
	typedef const uint8_t Icon[8];
	extern Icon iFILL;
	extern Icon iSHADE;
	extern Icon iNULL;
	
	#define ON true
	#define OFF false
	
	class PrecisTinyOled
	{
		public:
		static bool _wireStarted;
		uint8_t address;
		uint8_t width;
		uint8_t height;
		uint8_t currX;
		uint8_t currY;
		PrecisTinyOled();
		void clear();
		void power(bool on);
		void contrast(uint8_t c);
		void invert(bool on);
		void cursor(uint8_t x, uint8_t y);
		void style(uint8_t fonts, bool clr);
		void icon(uint8_t x, uint8_t y, Icon icond, uint8_t iconSize, bool clr);
		void number(uint8_t x, uint8_t y, const char * n, uint8_t iconSize, bool clr);
		void number(uint8_t x, uint8_t y, uint32_t val, uint8_t iconSize, bool clr);
		void image(uint8_t x, uint8_t y, uint8_t w, uint8_t hb, uint8_t *img, bool clr);
		void image(uint8_t x, uint8_t y, uint8_t w, uint8_t hb, const uint8_t *img, bool clr);
		void CR();
		void print(const char * txt);
		void print(uint32_t val);
		void println(const char * txt);
		void println(uint32_t val);
		void printc(const char * txt);
		void printc(uint32_t val);
		void printBinary(uint32_t val, uint8_t size, uint8_t brk);
		void wrapMode(bool state);
		void begin(uint8_t addr, uint8_t w, uint8_t h);
		void I2C(long SPD);
		void progressBar(uint8_t x, uint8_t y, uint8_t width, uint8_t progress);
		void _text(uint8_t x, uint8_t y, char txt, uint8_t textSize, bool clr);
		int _charPos(const char * haystack, char needle, uint8_t startPos);
		void _stream(uint8_t com, uint8_t mode);
		void startStream();
		void endStream();
		void _write(uint8_t com, uint8_t mode);
		
		private:
		long _SPD;
		uint8_t _currFontSize;
		bool _currFontClr;
		bool _wordWrapState;
		void _writePage(uint8_t col, uint8_t page, uint8_t pageData);
		void _oCursor(uint8_t x, uint8_t y, uint8_t xlen, uint8_t ylen);
		void _processImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h,  uint8_t *img, bool MEM);
	};
#endif
