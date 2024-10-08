# PrecisTinyOled Version 1.1.0
This small OLED Library is primarily for use with ATtiny 84/85/88/861 (ATTinyCore), ATmega 168/328, CH32V003, and CH32X035/33 via I2C, but it should work with just about any Microcontroller capable of utilizing the Wire library and I2C-based 128x32 and 128x64 oleds. The default SDA and SCL pins must be used.

This library was created to require minimal resources and be fairly easy to use. While it lacks support for geometric shapes, custom icons and images can be created and even replace some facets of those shapes, such as for borders.

The most basic use would be:
```cpp
#include "PrecisTinyOled.h"
PrecisTinyOled oled;          // Create one or more oled objects.

void setup(){
  // This initializes the oled with its I2C address, pixel width, and pixel height (32 or 64)
  // and also starts Wire using the default SDA and SCL pins.
  oled.begin(0x3C, 128, 64); 
}

void loop(){
// Your code to determine when to print text to screen
 oled.print("Sample text.");  // Print text at current location (starting at top left).
}
```

Multiple OLEDs can be used, each acting as a separate object for the PrecisTinyOled class. Each one requires a unique I2C address. Lots of screen draws could be a potential drain on memory, so your mileage will vary with multiple displays.

Screen data is written to the display immediately. The *print* method can be used to display text. The *numbers* method is also available to display larger numbers and some symbols. Custom images and scalable icons can be added via sketch.

Create a new OLED object by declaring it as a *PrecisTinyOled* type:
```cpp
PrecisTinyOled oled;  // All examples assume that oled is the designated display.
```
If desired, the I2C speed can be changed from its default value (typically 100000) to a faster rate of 400000 if supported on all connected devices.
```cpp
oled.I2C([I2C Frequency]);   // typical speeds are 100000 and 400000.
```
Then use the *begin* method to initialize it: 
```cpp
oled.begin([i2c address], [screen width (128)], [screen height (32 or 64)]);
```
Text methods are:
```cpp
oled.cursor([x], [y]);           // Set text postion (x = 0 to width, y = 0 to height).
oled.wrapMode([bool]);           // Set text wrap to on or off; text wrap may not work properly in all cases.
oled.style([scale], [bool]);     // Set the scale to 1 (normal) or more (larger) and whether to display or delete.
oled.print([text or integer]);   // Displays text or 32-bit unsigned integer.
oled.println([text or integer]); // Displays text or 32-bit unsigned integer, followed by a line break.
oled.printc([text or integer]);  // Displays a line of text or 32-bit unsigned integer that is centered horizontally.
oled.printBinary([integer], [bits], [bool]);  // Converts number to binary and displays it with the specified number of bits, and with or without space between every 8 bits.
oled.CR();                       // Prints a line break (or use \n with the print method).
```
If you would like to display slightly larger and nicer numbers on a single line, use the *number* method instead of *print*. Only 0-9#$%+-.: can be displayed using this Icon-based method. A line of numbers is automatically centered horizontally when x = 255.
```cpp
oled.number([x],[y], [(combination of 0-9#$%+-.:) or (integer)], [scale], [bool]);  // Print number characters or 32-bit unsigned integer at [x],[y] at [scale] with [bool] to determine display/delete.
```
Icons can be created using the *Icon* type. The format is 8 vertically-aligned bytes. They are displayed using the *icon* method.
```cpp
Icon ICON1 = {16,56,124,254,254,124,56,16};    // sample icon
oled.icon([x], [y], [Icon], [scale], [bool]);  // Print [Icon] at [x],[y] at [scale] with [bool] to determine display/delete.
```
Images can be created as either *byte[]* or *const byte[]*. The format consists of any number of vertically-aligned bytes spanning from left to right, then next row left to right, etc. They are displayed using the *image* method.
```cpp
const byte IMG1[8] = {16,56,124,254,254,124,56,16};  // sample image of 8 pixels across by 8 (1 page) down
oled.image([x], [y], [width], [vertical pages], [image], [bool]); // Print [image] at [x],[y] spanning the [width] by pages down with [bool] to determine display/delete.
```
Other methods:
```cpp
oled.contrast([0-255]);  // Change screen brightness.
oled.invert([bool]);     // Invert screen image.
oled.power([bool]);      // Turn screen image on and off.
oled.progressBar([x], [y], [width], [progress %]);   // Draw progress bar starting at [x],[y] spanning the [width] using the [progress % (0-100)].
```
### Notes
Obviously, drawing to an oled works best at the fastest possible speeds. This means that as long as all connected I2C devices are compatible with 400000, that setting should be used. Likewise, an ATtiny should be clocked at 16MHz for optimal speed, but 8MHz will also work. If multiple oleds are connected, you need only set the I2C speed once.

While this library is compatible with the CH32V003, flash and RAM space would be extremely tight. The CH32X035 and CH32X033 would have no problem in that regard.

The best scaling factors for text and icons is 1 to 4 (x1 to x4). The basic text scale is 4x8 pixels, while icons are 8x8. This means that a scale of 2 would make 8x16 pixels for text or 16x16 pixels for icons. The larger the scale, the slower the render. Images cannot be scaled.

The current position for text placement is stored in the *currX* and *currY* variables (oled.currX and oled.currY). The *print*, *number*, *icon*, and *image* methods associate any y values with the closest page.

Since the text font is designed to conserve space, it is very minimalistic. As such, it is blocky, narrow, and lacks lower-case letters. If you wish to use more-defined characters, custom icons can be created for 8x8, while bitmaps can be created for large sizes if only a few are needed on the display.

The following Icons are pre-set: *iFILL* (solid 8x8), *iSHADE* (dithered 8x8), and *iNULL* (empty 8x8).

Both the *print* and *number* methods require a char string or integer (up to 32 bits), but float (decimal places) would have to be converted first, such as by using *dtostrf*:
```cpp
char txt[16];
dtostrf(float, 1, 2, txt);  // Convert float to txt with 2 decimal places.
```

### Releases
1.1.0   Added println, printBinary, and support for integers with print.
