# freetypeconverter
generate font files for microcontroller projects using the freetype library

The font file generated by this programm is intended to be used with this library: https://www.lcd-module.com/support/application-note/arduino-meets-ea-dog.html

## usage: 
```
-f <path to file>      path to file with font
-n <name>              name for header file with font and name of font constant
-s <number>            font-size
-b <number>            baseline for the font in pixels from top
-g <number>            height of font in bytes for generated file
-d                     printf debug messages
-h                     print this help
```
