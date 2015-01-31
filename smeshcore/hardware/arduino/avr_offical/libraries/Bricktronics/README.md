Bricktronics Library v1.0
December 3rd, 2012
http://www.wayneandlayne.com/bricktronics

Connect your LEGO Mindstorms NXT sensors and motors to your Arduino using this library.  Examples are included--ones that don't need much building are in the Basics/ directory.  Most of the other examples are code from the book "Make: LEGO and Arduino Projects" by John Baichtal, Matthew Beckler, and Adam Wolf.

Installation
============
Quit the Arduino application.
Uncompress the file containing the library.  There should be a directory called Bricktronics, and inside that directory should be a bunch of files and directories, including this one, README.md.
Copy the Bricktronics folder.

Find your sketchbook directory.  On Windows, this is usually My Documents\Arduino.  On Mac, it is usually Documents/Arduino.  On 
Linux, it is usually ~/Arduino.
If there isn't a libraries directory in your sketchbook directory, create one.
Paste the Bricktronics library inside the libraries directory in your sketchbook.

After all of this, you should be able to open your sketchbook directory, open your libraries directory, open your Bricktronics directory, and see a few directories and files, including this one, README.md.

Restart the Arduino application. The Bricktronics library should appear in the Sketch->Import Library menu, and there should be Bricktronics examples in File->Examples->Bricktronics.

Usage
=====
Usage instructions are available at the Bricktronics website (http://www.wayneandlayne.com/bricktronics).

Licensing
=========
The Adafruit MCP23017 is BSD licensed.  The Bricktronics library itself is GPL, except for the ColorSensor part, which is MPL 1.0.

The Bricktronics library is open source software, and you're free to use it without Bricktronics products--but purchasing our products is the best way to make sure we're able to spend time making more open source software and hardware in the future.  The second best way is to tell all your friends how much you like our stuff!

Feedback
========
If you have any questions or comments, please feel free to contact us at our public web forum (http://www.wayneandlayne.com/forum) or privately through email (http://www.wayneandlayne.com/contact).

Acknowledgements
================
There were a lot of other libraries used in the creation of the Bricktronics library.  Special thanks to Adafruit, William Grieman, jrraines, the leJOS team, LEGO team!

Adafruit MCP23017 Library
=========================
The Bricktronics library contains the Adafruit MCP23017 library.  It's used to control the MCP23017 port expander used for slower IO.

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, check license.txt for more information
All text above must be included in any redistribution

https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library

SoftI2cMaster Library
=====================
The LEGO Ultrasonic sensor has some quirks that we need to work around in firmware.  We need repeated start/restart support, and we need to tweak the clock bit after a read.  We modified the SoftI2CLibrary by William Greiman to do this.

http://forums.adafruit.com/viewtopic.php?f=25&t=13722

leJOS
=====
We used the leJOS and NXT firmware to get the Color Sensor working with Arduino.  Thanks!

DigitalWriteFast
================
We need a little faster performance than digitalWrite for the Color Sensor--but as we're playing around with alternative Arduino architectures like the ChipKit, we decided to try out a set of macros that are almost as fast as twiddling the PORT registers directly.  jrraines implemented these for Arduino, based on some work by Paul Stoffregen.

http://code.google.com/p/digitalwritefast/

Wayne and Layne, LLC and our products are not connected to or endorsed by the LEGO Group. LEGO, Mindstorms, and NXT are trademarks of the LEGO Group.
