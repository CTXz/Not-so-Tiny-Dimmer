<!-- omit in toc -->
# Tiny Dimmer

![](MainShowcase.gif)

A cool little ATtiny 25/45/85 based dimmer for WS2812 RGB strips.

## Contents
- [Contents](#contents)
- [Features](#features)
- [Hardware](#hardware)
  - [Schematic](#schematic)
  - [Perfboard](#perfboard)
  - [Showcase](#showcase)
- [Flashing the firmware](#flashing-the-firmware)
- [Project log](#project-log)

## Features

The Tiny dimmer is a minimal WS2812 dimmer that exposes a potentiometer to control the strips brightness, and a button to toggle between different colors stored inside the program flash. The WS2812 strip size, patches (colors and animations), other hardware and firmware parameters can be configured in the [configuration header](src/config.h).

The dimmer also comes with potentiometer and push button noise reduction to reduce LED flicker and false color toggling. These features can compensate for sloppy hardware jobs but cost program cycles. The noise potentiometer and button noise reduction can be adjusted or disabled in the [configuration header](src/config.h)

The dimmer firmware has been written for ATtiny45 and ATtiny85 chips, but will tightly fit onto a ATtiny25 chip with fewer and more simple patches (ex. no animations). The size of the software may be reduced by disabling certain software parameters in the [configuration header](src/config.h), reducing 16 or 32 bit integers to 8 bit integers where its possible etc. however, it is really recommended to simply use an ATiny45 or ATiny85 instead if one wants make full use of all features.

## Hardware

The following components are required to build a tiny dimmer:

|Component|Quantity|Description|
|---------|--------|-----------|
|WS2812 strip|1|The dimmer drives addressable WS2812 RGB strips, as those are simply very common and are what I had at home.|
|ATtiny 85/45/25|1|The chip that runs the firmware, hence the name "TinyDimmer". Although the firmware will barely fit on a ATtiny25, it is highly recommended to use an ATiny45 or ATiny85 if additional features are to be added to the software later.|
|1K resistor|1|Used to limit the current to prevent the brightness pot from burning trough.|
|10k linear potentiometer|1|Used to set the brightness. 10k linear pots are recommended, but any linear pot between 1k and 100k should do the job.|
|10uF Capacitor|1|Optional, but helps decoupling power supply noise.|
|Push Button|1|Used to toggle between colors and to activate fading.|
|1x6 2.54mm Female header|1|Exposes a SPI header to program the MCU.|
|Proto Perfboard|1|In the perfboard layout below I've used a 10x24 Perfboard, however much less is required.|

The dimmer can also be easily be built with a digispark board.

The price for the components can range anywhere between 5 to 15 EUR, depending from where you purchase the components, and what their quality is.

### Schematic

![Schematic.png](Schematic.png)

### Perfboard

Top view:
![](TinyDimmerTop.png)

### Showcase

Perfboard:
![](PCBShowcase.jpg)

Programming via SPI with ArduinoISP:
![](ShowcaseProgramming.jpg)

## Flashing the firmware

Flashing the firmware is possible even with the chips attached to the board. Hence it is recommended to solder an SPI header onto the board if you're planning to update the software and do not wish to open up the device for every firmware update.

To flash the firmware, an SPI programmer is required. Since I still have a few spare Arduino's lying around, I decided to use an Arduino Leonardo with the [Arduino ISP firmware](https://www.arduino.cc/en/tutorial/arduinoISP) installed to program the TinyDimmer.

Since the firmware has been written in [PlatformIO](https://platformio.org/), it is easiest to download the IDE, simply import the project and flash it from there.

Should your tiny dimmer not use an ATtiny85, you must change the `board` directive in the [PlatformIO configuration file](platformio.ini) to your ATtiny model.   

In addition to that, the [PlatformIO configuration file](platformio.ini) also expects a stk500v1 programmer (compatible with Arduino over ISP). If you are not programming the Tiny dimmer via an Arduino, you must change `upload_protocol` to the the corresponding programmer in the configuration file (See the [PlatformIO atemlavr documentation](https://docs.platformio.org/en/latest/platforms/atmelavr.html)).

Once everything is set in PlatformIO, make sure the parameters in the [configuration header](src/config.h) match your needs and simply click the upload button. The firmware should be flashed onto your TinyDimmer in no time. 

If you're planning to flash the firmware without the PlatformIO IDE, **make sure to program the ATtiny's fuses for it to run at 16Mhz!**

## Project log

The Tiny dimmer was built mostly out of necessity for a kitchen light and quarantine boredom. The choice of components was initially more or less a result of the limited number of spare parts that I had lying around at home. In particular, I had a digispark with an ATtiny25 soldered onto it, which I figured could suffice for a simple kitchen light.
Working with 2 KB of program flash and 128 B of RAM certainly came with its challenges. The tiny amount of program memory put the use of the Arduino library and the common NeoPixelBus library completely out of question and I was forced to write the code in pure AVR C.

To drive the WS2812 strip I decided to give [cpldcpu's light_ws2812](https://github.com/cpldcpu/light_ws2812) library a shot, which I had essentially stripped down to the `ws2812_sendarray_mask` function. Since my plan was to control all pixels of the strip simultaneously (yes, I'm aware I could've used non addressable strips for that, but WS2812 strips are all I have lying around... besides they just need one pin to be driven), I had to find a way to alter the `ws2812_sendarray_mask` function to no longer accept a pixel buffer, which wastes an enormous amount of RAM, but rather a RGB object which it would apply to all LEDs.

Easier said than done, as the function is incredibly time sensitive. That's the  reason as to why the `ws2812_sendarray_mask` function's main loop, which transmits the color data to the strip, has been written in AVR assembly (unfortunately a assembly languages that I'm still very unfamiliar with). Applying the slightest changes always ended up breaking the function, until I figured that the compiler optimization might be the reason behind that. Turns out, by turning the compiler optimization using the `#pragma GCC pop_options` macro, the main loop becomes allot less sensitive to changes. I haven't taken a look at the pulse time differences that arise with and without optimization enabled, but it might be something interesting to look into. Never the less, I was happy that I could finally control the WS2812 strip as desired, without blowing away the ATtinys 128 B of RAM, and thus I rewrote the `ws2812_send_array` function to `ws2812_set_all` which sets all pixels to a provided RGB value.

From here, implementing the firmware was rather straight forward. Initially I planned to assign the push button an INT0 interrupt. However, after implementing button hold detection and noise reduction, support for interrupts has been dropped in favour of a pure state polling implementation.

Adding the fade animation was a little tricky in that its maximum speed, provided its best resolution, was heavily limited by the speed of the `ws2812_set_all` function. Hence, for faster fading speeds, I was forced to increase the step size for the colors, meaning faster fading speeds come at the cost of smoothness/resolution.

All in all, this was a fun project to kill some quarantine time and most importantly, I'm more than happy with the way the result turned out and it was nice to put my AVR C skills to use again. Sure, I could've bought a cheap chinese LED strip controller, but where's the fun in that. This controller is all about open source and allows me to extend its features by reprogramming it. If I ever plan on discarding it, I can simply pop out and desolder all components are reuse them for another project.

--- Update ---

I have since reworked the software quite a bit. The ws2812 functions have been reduced to a set of 4 core functions, `ws2812_prep_tx()`, `ws2812_wait_rst()`, `ws2812_tx_byte()`, `ws2812_end_tx()`. With these functions I have then implemented a hardware abstraction layer in the [strip.c](src/strip.c) file, which defines various routines to control the LED strip. Patches have been completely reworked and are now defined in the [configuration header](src/config.h) using macros provided by the [patch_macros.h](src/patch_macros.h) header file. Using macros allows for very flexible patches that aren't limited to a single function or some sort of array of a specific data type. This made room for the easy implementation of animations.