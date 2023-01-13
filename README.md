# PrecisTinyOled
This small OLED Library is primarily for use with ATtiny 84/85/88/861 (ATTinyCore) and ATmega 168/328 via I2C, but it should work with just about any Microcontroller capable of utilizing the Wire library and I2C-based 128x32 and 128x64 oleds. It lacks support for geometric shapes, but custom icons and images can be rendered. The most basic use would be:
```cpp
#include "PrecisTinyOled.h"
PrecisTinyOled oled;          // Create one or more oled objects, though more than one is not advised.

void setup(){
  oled.begin(0x3C, 128, 64);  // Initialize the oled with its I2C address, as well as pixel width and height (32 or 64).
}

void loop(){
// Your code to determine when to print text to screen
 oled.print("Sample text.");  // Print text at current location (starting at top left).
}
```

While multiple OLEDs can be used, each acting as a separate object for the PrecisTonyOled class and requiring a unique I2C address, this would be a potential drain on memory, so mileage may vary.

Screen data is written to the display immediately. The print method can be used to display text. The numbers method is also available to display larger numbers and some symbols. Custom images and scalable icons can be added via sketch.

Create a new OLED object by declaring it as a PrecisTinyOled type:
```cpp
PrecisTinyOled oled;  // All examples assume that oled is the designated display.
```
If desired, the I2C speed can be changed from its default value (typically 100000) to a faster rate of 400000 if supported on all connected devices.
```cpp
oled.I2C([I2C Frequency]);   // typical speeds are 100000 and 400000.
```
Then use the begin method to initialize it: 
```cpp
oled.begin([i2c address], [screen width (128)], [screen height (32 or 64)]);
```
Text methods are:
```cpp
oled.cursor([x], [y]);        // Set text postion (x = 0 to width, y = 0 to height).
oled.wrapMode([bool]);        // Set text wrap to on or off; text wrap may not work properly in all cases.
oled.style([scale], [bool]);  // Set the scale to 1 (normal) or more (larger) and whether to display or delete.
oled.print([text]);           // Displays text.
oled.CR();                    // Prints line break (or use \n with the print method).
```
If you would like to display slightly larger and nicer numbers, use the number method instead of print. Only 0-9#$%+-.: can be displayed using this Icon-based method.
```cpp
oled.number([x],[y], [combination of 0-9#$%+-.:], [scale], [bool]);  // Print characters at [x],[y] at [scale] with [bool] to determine display/delete.
```
Icons can be created using the Icon type. The format is 8 vertically-aligned bytes. They are displayed using the icon method.
```cpp
Icon ICON1 = {16,56,124,254,254,124,56,16};    // sample icon
oled.icon([x], [y], [Icon], [scale], [bool]);  // Print [Icon] at [x],[y] at [scale] with [bool] to determine display/delete.
```
Images can be created as either "byte[]" or "const byte[]". The format consists of any number of vertically-aligned bytes spanning from left to right, next row left to right, etc. They are displayed using the image method.
```cpp
const byte IMG1[8] = {16,56,124,254,254,124,56,16};  // sample image of 8 pixels across by 8 (1 page) down
oled.image([x], [y], [width], [vertical pages], [image], [bool]); // Print [image] at [x],[y] spanning the width by pages down with [bool] to determine display/delete.
```
Other methods:
```cpp
oled.contrast([0-255]);  // Change screen brightness.
oled.invert([bool]);     // Invert screen image.
oled.power([bool]);      // Turn screen image on and off.
```
### Notes
The best scaling factors for text and icons is 1 to 4 (x1 to x4). The basic text scale is 4x8 pixels, while icons are 8x8. This means that a scale of 2 would make 8x16 pixels for text or 16x16 pixels for icons. The larger the scale, the slower the render. Images cannot be scaled.

Since the text font is designed to conserve space, it is very minimalistic. As such, it is blocky and narrow. If you wish to use more-defined characters, custom icons can be created for 8x8, while bitmaps can be created for large sizes if only a few are needed on the display.

The current position for text placement is stored in the currX and currY variables (oled.currX and oled.currY).
