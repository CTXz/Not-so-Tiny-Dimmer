<!-- omit in toc -->
# Not-so-Tiny Dimmer 

![](img/MainShowcase.gif)

What initaly just started as minimalist ATtiny based WS2812 LED strip dimmer, quickly turned into a cheap and feature rich ATtiny based WS2812 strip controller with single color, multi color and animation support. Non-addressable LED strips are also supported, but limited to single color features only.

## Contents
- [Contents](#contents)
- [Overview](#overview)
  - [Software](#software)
    - [Patches](#patches)
      - [`PATCH_SET_ALL`](#patchsetall)
      - [`PATCH_DISTRIBUTE`](#patchdistribute)
      - [`PATCH_ANIMATION_BREATHE_POT_CTRL`](#patchanimationbreathepotctrl)
      - [`PATCH_ANIMATION_BREATHE_RAND_POT_CTRL`](#patchanimationbreatherandpotctrl)
      - [`PATCH_ANIMATION_BREATHE_RAINBOW_POT_CTRL`](#patchanimationbreatherainbowpotctrl)
      - [`PATCH_ANIMATION_BREATHE_ARR_POT_CTRL`](#patchanimationbreathearrpotctrl)
      - [`PATCH_ANIMATION_RAINBOW`](#patchanimationrainbow)
      - [`PATCH_ANIMATION_RAINBOW_POT_CTRL`](#patchanimationrainbowpotctrl)
      - [`PATCH_ANIMATION_SWAP`](#patchanimationswap)
      - [`PATCH_ANIMATION_SWAP_POT_CTRL`](#patchanimationswappotctrl)
      - [`PATCH_ANIMATION_ROTATE_RAINBOW`](#patchanimationrotaterainbow)
      - [`PATCH_ANIMATION_RAIN`](#patchanimationrain)
      - [`PATCH_ANIMATION_RAIN_POT_CTRL`](#patchanimationrainpotctrl)
  - [Hardware](#hardware)
- [Hardware](#hardware-1)
  - [Schematic](#schematic)
  - [Perfboard](#perfboard)
  - [Showcase](#showcase)
- [Flashing the firmware](#flashing-the-firmware)
- [Credits](#credits)
- [Project log](#project-log)
- [License](#license)

## Overview

The Not-so-Tiny dimmer is a open source ATtiny Based WS2812 RGB strip controller that offers support for single and multi color lights, as well various animations. Alternatively, the controller can also be used to drive non-addressable LED strips, but is only limited to single color patches and animations. 

### Software

The controller offers a patch bank with up to 10 patches, where each patch slot can be hard-coded in the [configuration header](src/config.h) to something as simple as projecting a single color across the strip, all the way to projecting flashy rainbow animations. If less than 10 patches are desired, the `NUM_PATCHES` directive can be reduced in the [configuration header](src/config.h).

Non-addressable RGB strips are also supported by the firmware by changing the `STRIP_TYPE` configuration in the [configuration header](src/config.h) to `NON_ADDR`, but will limit the selection to single color patches. Whether a patch is supported on non-addressable strips is indicated in the comments of the [patch macro definitions](src/patch_macros.h). A [template configuration for non-addressable strips](src/config_templates/config_non-addressable.h) can be found in the [configuration_templates directory](src/config_templates/).

**Please note that the required amount of program flash rises with the number and complexity of patches. Thus, the use of an ATtiny25 is likely to only limited to a few single color patches.** For ATtiny25 based controllers, a [minimal configuration file template](src/config_templates/config_minimal.h) has been provided.

Due to the fact that patches are implemented C preprocessor directives, increasing the maximum number of patches requires addition of case statements in the [update_strip](https://github.com/CTXz/Tiny-Dimmer/blob/master/src/main.c#L193) function, along with a change of the `NUM_PATCHES` directive.

#### Patches

The following section lists all available patches that can be assigned to any of the 10 patch slots defined in the [configuration header](src/config.h):

##### `PATCH_SET_ALL`

Parameters:
|Type|Name|Description|
|----|----|-----------|
|`uint8_t`|R|Red value (0 - 255)|
|`uint8_t`|G|Green value (0 - 255)|
|`uint8_t`|B|Blue value (0 - 255)|

Description:

Sets the entire LED strip to one color. The brightness can be adjusted by the potentiometer.

Supported on non-addressable strips: Yes

Example:
```c
#define PATCH_0 PATCH_SET_ALL(255, 255, 255) // Fills strip with white
```

Showcase:

![](img/SetAllShowcase.jpg)

##### `PATCH_DISTRIBUTE`

Parameters:
|Type|Name|Description|
|----|----|-----------|
|`RGB_t[]`|RGB_ARR|An RGB_ARRAY() enclosed array of literal RGB arrays. Ex. `RGB_ARRAY({255, 255, 255}, {0, 1, 2}, ...)`|

Description:

Distributes the provided array of RGB values evenly accross the entire LED strip. The brightness can be adjusted by the potentiometer.

Supported on non-addressable strips: No

Example:

```c
// First strip half pink, second cyan
#define PATCH_0 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {10, 255, 202},    \
                {255, 20, 127}     \
        )                          \
)
```

Showcase:

![](img/DistributeShowcase.jpg)

##### `PATCH_ANIMATION_BREATHE_POT_CTRL`

Parameters:
|Type|Name|Description|
|----|----|-----------|
|`uint8_t`|R|Red value|
|`uint8_t`|G|Green value|
|`uint8_t`|B|Blue value|

Description:

"Breathes" the provided RGB value across the entire strip. The duration of a "breath" can be altered by the potentiometer.

Supported on non-addressable strips: Yes

Example:

```c
#define PATCH_0 PATCH_ANIMATION_BREATHE_POT_CTRL(255, 255, 255) // Breathes white color
```

Showcase:

![](img/BreatheShowcase.gif)


##### `PATCH_ANIMATION_BREATHE_RAND_POT_CTRL`

Description:

"Breathes" random RGB values across the entire strip.
The duration of the "breath" can be altered by the potentiometer.
Due to the rather poor randomness of rand(), the outcomes tend
to be similar.

Supported on non-addressable strips: Yes

Example:
```c
#define PATCH_0 PATCH_ANIMATION_BREATHE_RAND_POT_CTRL
```

##### `PATCH_ANIMATION_BREATHE_RAINBOW_POT_CTRL`

Parameters:

|Type|Name|Description|
|----|----|-----------|
|`uint8_t`|RGB_STEP_SIZE|Color steps after every "breath". A greater step size means the color difference  between each breath becomes more noticeable.|

Description:
c
Gradiently "Breathes" trough the rgb spectrum. The duration of the "breath" can be altered by the potentiometer.

Supported on non-addressable strips: Yes

Example:
```c
#define PATCH_0 PATCH_ANIMATION_BREATHE_RAINBOW_POT_CTRL(10) // Breathes trough rainbow with 10 steps in color between each breath
```

##### `PATCH_ANIMATION_BREATHE_ARR_POT_CTRL`

Parameters:

|Type|Name|Description|
|----|----|-----------|
|`RGB_t[]`|RGB_ARR|An RGB_ARRAY() enclosed array of literal RGB arrays, ex. RGB_ARRAY({255, 255, 255}, {0, 1, 2}, ...)|

Description:

Gradiently "Breathes" trough the RGB array. The duration of the "breath" can be altered by the potentiometer.

Supported on non-addressable strips: Yes

Example:
```c
// Breathes trough red, green and blue
#define PATCH_0 PATCH_ANIMATION_BREATHE_ARR_POT_CTRL ( \
        RGB_ARRAY (                                    \
                {255, 0, 0},                           \
                {0, 255, 0},                           \
                {0, 0, 255}                            \
        )                                              \
)
```

##### `PATCH_ANIMATION_RAINBOW`

Parameters:

|Type|Name|Description|
|----|----|-----------|
|`uint8_t`|STEP_SIZE|Color steps (0 - 255) between each call. A greater value results in faster fading.|

Description:

Gradiently fades all LEDs simultaneously trough the RGB spectrum.

Supported on non-addressable strips: Yes

Example:
```c
#define PATCH_0 PATCH_ANIMATION_RAINBOW(10) // Fade trough RGB spectrum with 10 steps between each color change
```

Showcase:

![](img/RainbowShowcase.gif)

##### `PATCH_ANIMATION_RAINBOW_POT_CTRL`

Description:

Gradiently fades all LEDs simultaneously trough the RGB spectrum.
The step size, and thus fade speed, can be altered by the potentiometer.

Supported on non-addressable strips: Yes

Example:

```c
#define PATCH_0 PATCH_ANIMATION_RAINBOW_POT_CTRL
```

##### `PATCH_ANIMATION_SWAP`

Parameters:

|Type|Name|Description|
|----|----|-----------|
|`uint8_t`|RFH|Red value of first strip half|
|`uint8_t`|GFH|Green value of first strip half|
|`uint8_t`|BFH|Blue value of first strip half|
|`uint8_t`|RFH|Red value of second strip half|
|`uint8_t`|GFH|Green value of second strip half|
|`uint8_t`|BFH|Blue value of second strip half|
|`unsigned long`|SWAP_TIME|Time (ms) after which the halves get swapped|

Description:

Splits the strip in two halves and continuously swaps their colors.

Supported on non-addressable strips: No

Example:
```c
#define PATCH_9 PATCH_ANIMATION_SWAP(255, 0, 0, 0, 0, 255, 1000) // Swaps between off and a random color
```

Showcase:

![](img/SwapShowcase.gif)

##### `PATCH_ANIMATION_SWAP_POT_CTRL`

Parameters:

|Type|Name|Description|
|----|----|-----------|
|`uint8_t`|RFH|Red value of first strip half|
|`uint8_t`|GFH|Green value of first strip half|
|`uint8_t`|BFH|Blue value of first strip half|
|`uint8_t`|RFH|Red value of second strip half|
|`uint8_t`|GFH|Green value of second strip half|
|`uint8_t`|BFH|Blue value of second strip half|

Description:

Splits the strip in two halves and continuously swaps their colors. The swap time can be altered by the potentiometer.

Supported on non-addressable strips: No

```c
#define PATCH_9 PATCH_ANIMATION_SWAP(255, 0, 0, 0, 0, 255) // Swaps between red and blue at a speed set by the potentiometer
```

##### `PATCH_ANIMATION_ROTATE_RAINBOW`

Description:

Rotates the rgb spectrum across the strip.

Example:
```c
#define PATCH_0 PATCH_ANIMATION_ROTATE_RAINBOW
```

Supported on non-addressable strips: No

Showcase:

![](img/MainShowcase.gif)

##### `PATCH_ANIMATION_RAIN`

Parameters:

|Type|Name|Description|
|----|----|-----------|
|`uint8_t`|_R|Red color value|
|`uint8_t`|_G|Green color value|
|`uint8_t`|_B|Blue color value|
|`uint16_t`|MAX_DROPS|Maximum amount of visible "droplets" at a time|
|`uint16_t`|MIN_T_APPART|Minimum time in ms between drops|
|`uint16_t`|MAX_T_APPART|Maximum time in ms between drops|
|`uint8_t`|STEP_SIZE|Step size of droplet fading|

Description:

Creates a rain effect across the strip.
Note that this effect makes use of an RGB buffer and will linearly increase memory consumption with strip size.

Supported on non-addressable strips: No

Example:

```c
// White and cyan rain with max 10 drops, a minimum time of 0ms, maximum time of 100ms and step size of 10
#define PATCH_0 PATCH_ANIMATION_RAIN(255, 255, rand()%255, 10, 0, 100, 10)
```

Showcase:

![](img/RainShowcase.gif)


##### `PATCH_ANIMATION_RAIN_POT_CTRL`

Parameters:

|Type|Name|Description|
|----|----|-----------|
|`uint8_t`|_R|Red color value|
|`uint8_t`|_G|Green color value|
|`uint8_t`|_B|Blue color value|

Description:

Creates a rain effect across the strip. The "intensity" of the rain can be adjusted with the potentiometer. Note that this effect makes use of an RGB buffer and will linearly increase memory consumption with strip size.

Supported on non-addressable strips: No

Example:

```c
// White and cyan rain with potentiometer intensity control
#define PATCH_0 PATCH_ANIMATION_RAIN(255, 255, rand()%255)
```

### Hardware

To navigate trough the patch bank, a single push button is provided. Once the last patch has been reached, the first patch is loaded again upon button press.

Many patches also offer a tweakable parameter, such as brightness and speed control. A potentiometer is used to adjust these parameters in real time.

To compensate for sloppy hardware jobs, the firmware also comes with potentiometer and push button noise reduction to reduce LED flicker and false color toggling. It should be noted however, that these features cost program flash and runtime cycles and can be adjusted or disabled in the [configuration header](src/config.h)

The controller firmware has been written for and ATtiny85 chips, but will tightly fit onto a ATtiny25 chip if fewer and minimal patches are used. Unless the controller is planned to be used as a simple single-color WS2812 RGB dimmer, it is really recommended to at least use an ATiny45, preferably an ATiny85 if one wants make full use of all features.

## Hardware

The following components are required to build a Not-so-Tiny dimmer:

|Component|Quantity|Description|
|---------|--------|-----------|
|WS2812 strip|1|The controller drives addressable WS2812 RGB strips, as those are simply very common and are what I had at home.|
|ATtiny 85/45/25|1|The chip that runs the firmware, hence the name "Tiny" in the name. Although the firmware will barely fit on a ATtiny25, it is highly recommended to use an ATiny45 or ATiny85 if additional features are to be added to the software later.|
|1K resistor|1|Used to limit the current to prevent the brightness pot from burning trough.|
|10k linear potentiometer|1|Used to set the brightness. 10k linear pots are recommended, but any linear pot between 1k and 100k should do the job.|
|10uF Capacitor|1|Optional, but helps decoupling power supply noise.|
|Push Button|1|Used to toggle between colors and to activate fading.|
|1x6 2.54mm Female header|1|Exposes a SPI header to program the MCU.|
|Proto Perfboard|1|In the perfboard layout below I've used a 10x24 Perfboard, however much less is required.|

The controller can also be easily be built with a digispark board.

The price for the components can range anywhere between 5 to 15 EUR, depending from where you purchase the components, and what their quality is.

### Schematic

![Schematic.png](img/Schematic.png)

### Perfboard

Top view:
![](img/NotSoTinyDimmerTop.png)

### Showcase

Perfboard:
![](img/PCBShowcase.jpg)

Programming via SPI with ArduinoISP:
![](img/ShowcaseProgramming.jpg)

## Flashing the firmware

Flashing the firmware is possible even with the chips attached to the board. Hence it is recommended to solder an SPI header onto the board if you're planning to update the software and do not wish to open up the device for every firmware update.

To flash the firmware, an SPI programmer is required. Since I still have a few spare Arduino's lying around, I decided to use an Arduino Leonardo with the [Arduino ISP firmware](https://www.arduino.cc/en/tutorial/arduinoISP) installed to program the controller.

Since the firmware has been written in [PlatformIO](https://platformio.org/), it is easiest to download the IDE, simply import the project and flash it from there.

Should your Not-so-Tiny dimmer not use an ATtiny85, you must change the `board` directive in the [PlatformIO configuration file](platformio.ini) to your ATtiny model.   

In addition to that, the [PlatformIO configuration file](platformio.ini) also expects a stk500v1 programmer (compatible with Arduino over ISP). If you are not programming the controller via an Arduino, you must change `upload_protocol` to the the corresponding programmer in the configuration file (See the [PlatformIO atemlavr documentation](https://docs.platformio.org/en/latest/platforms/atmelavr.html)).

Once everything is set in PlatformIO, make sure the parameters in the [configuration header](src/config.h) match your needs and simply click the upload button. The firmware should be flashed onto your controller in no time. 

If you're planning to flash the firmware without the PlatformIO IDE, **make sure to program the ATtiny's fuses for it to run at 16Mhz!**

## Credits

Big credits to @cpldcpu's [light_ws2812](https://github.com/cpldcpu/light_ws2812) library as plenty of hise code has been used to drive the WS2812 strip.

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

--- Update ---

I think it's time to admit that this project is no longer a minimal dimmer, in fact, it has become quite the opposite and is turning into a feature bloated over-engineered ATtiny based WS2812 controller that, at least if the quarantine boredom won't end soon, will be connected, and will require connection, to the world wide web, feature the clunkiest most inaccurate and unnecessary voice control, feature incredibly water and dust sensitive touch controls and some form of AI or who knows what the future offers.

In all seriousness, except for the fact that I've now renamed the project to Not-so-Tiny dimmer, the device can now be more accurately considered a LED strip controller. While I personally do take large joy in adding and using the additional flashy animations, I've taken into account that we're starting to cross the RGB hell border in the eyes of many people, and have hence made sure that the software architecture is very flexible and allows one to take full control of which features and patches to be implemented. Hell, if desired, the controller software can be reduced to only support a single light setting along with brightness control.

## License

All code in this project is licensed by the GNU GPLv3 license and hardware is licensed by the [TAPR Open Hardware License](https://tapr.org/the-tapr-open-hardware-license/).

Both licenses require to provide a copy of the modified source code and/or hardware documentation for any derivative work.

GNU GPLv3
```
                    GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                            Preamble

  The GNU General Public License is a free, copyleft license for
software and other kinds of works.

  The licenses for most software and other practical works are designed
to take away your freedom to share and change the works.  By contrast,
the GNU General Public License is intended to guarantee your freedom to
share and change all versions of a program--to make sure it remains free
software for all its users.  We, the Free Software Foundation, use the
GNU General Public License for most of our software; it applies also to
any other work released this way by its authors.  You can apply it to
your programs, too.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
them if you wish), that you receive source code or can get it if you
want it, that you can change the software or use pieces of it in new
free programs, and that you know you can do these things.

  To protect your rights, we need to prevent others from denying you
these rights or asking you to surrender the rights.  Therefore, you have
certain responsibilities if you distribute copies of the software, or if
you modify it: responsibilities to respect the freedom of others.

  For example, if you distribute copies of such a program, whether
gratis or for a fee, you must pass on to the recipients the same
freedoms that you received.  You must make sure that they, too, receive
or can get the source code.  And you must show them these terms so they
know their rights.

  Developers that use the GNU GPL protect your rights with two steps:
(1) assert copyright on the software, and (2) offer you this License
giving you legal permission to copy, distribute and/or modify it.

  For the developers' and authors' protection, the GPL clearly explains
that there is no warranty for this free software.  For both users' and
authors' sake, the GPL requires that modified versions be marked as
changed, so that their problems will not be attributed erroneously to
authors of previous versions.

  Some devices are designed to deny users access to install or run
modified versions of the software inside them, although the manufacturer
can do so.  This is fundamentally incompatible with the aim of
protecting users' freedom to change the software.  The systematic
pattern of such abuse occurs in the area of products for individuals to
use, which is precisely where it is most unacceptable.  Therefore, we
have designed this version of the GPL to prohibit the practice for those
products.  If such problems arise substantially in other domains, we
stand ready to extend this provision to those domains in future versions
of the GPL, as needed to protect the freedom of users.

  Finally, every program is threatened constantly by software patents.
States should not allow patents to restrict development and use of
software on general-purpose computers, but in those that do, we wish to
avoid the special danger that patents applied to a free program could
make it effectively proprietary.  To prevent this, the GPL assures that
patents cannot be used to render the program non-free.

  The precise terms and conditions for copying, distribution and
modification follow.

                       TERMS AND CONDITIONS

  1. Definitions.

  "This License" refers to version 3 of the GNU General Public License.

  "Copyright" also means copyright-like laws that apply to other kinds of
works, such as semiconductor masks.

  "The Program" refers to any copyrightable work licensed under this
License.  Each licensee is addressed as "you".  "Licensees" and
"recipients" may be individuals or organizations.

  To "modify" a work means to copy from or adapt all or part of the work
in a fashion requiring copyright permission, other than the making of an
exact copy.  The resulting work is called a "modified version" of the
earlier work or a work "based on" the earlier work.

  A "covered work" means either the unmodified Program or a work based
on the Program.

  To "propagate" a work means to do anything with it that, without
permission, would make you directly or secondarily liable for
infringement under applicable copyright law, except executing it on a
computer or modifying a private copy.  Propagation includes copying,
distribution (with or without modification), making available to the
public, and in some countries other activities as well.

  To "convey" a work means any kind of propagation that enables other
parties to make or receive copies.  Mere interaction with a user through
a computer network, with no transfer of a copy, is not conveying.

  An interactive user interface displays "Appropriate Legal Notices"
to the extent that it includes a convenient and prominently visible
feature that (1) displays an appropriate copyright notice, and (2)
tells the user that there is no warranty for the work (except to the
extent that warranties are provided), that licensees may convey the
work under this License, and how to view a copy of this License.  If
the interface presents a list of user commands or options, such as a
menu, a prominent item in the list meets this criterion.

  1. Source Code.

  The "source code" for a work means the preferred form of the work
for making modifications to it.  "Object code" means any non-source
form of a work.

  A "Standard Interface" means an interface that either is an official
standard defined by a recognized standards body, or, in the case of
interfaces specified for a particular programming language, one that
is widely used among developers working in that language.

  The "System Libraries" of an executable work include anything, other
than the work as a whole, that (a) is included in the normal form of
packaging a Major Component, but which is not part of that Major
Component, and (b) serves only to enable use of the work with that
Major Component, or to implement a Standard Interface for which an
implementation is available to the public in source code form.  A
"Major Component", in this context, means a major essential component
(kernel, window system, and so on) of the specific operating system
(if any) on which the executable work runs, or a compiler used to
produce the work, or an object code interpreter used to run it.

  The "Corresponding Source" for a work in object code form means all
the source code needed to generate, install, and (for an executable
work) run the object code and to modify the work, including scripts to
control those activities.  However, it does not include the work's
System Libraries, or general-purpose tools or generally available free
programs which are used unmodified in performing those activities but
which are not part of the work.  For example, Corresponding Source
includes interface definition files associated with source files for
the work, and the source code for shared libraries and dynamically
linked subprograms that the work is specifically designed to require,
such as by intimate data communication or control flow between those
subprograms and other parts of the work.

  The Corresponding Source need not include anything that users
can regenerate automatically from other parts of the Corresponding
Source.

  The Corresponding Source for a work in source code form is that
same work.

  2. Basic Permissions.

  All rights granted under this License are granted for the term of
copyright on the Program, and are irrevocable provided the stated
conditions are met.  This License explicitly affirms your unlimited
permission to run the unmodified Program.  The output from running a
covered work is covered by this License only if the output, given its
content, constitutes a covered work.  This License acknowledges your
rights of fair use or other equivalent, as provided by copyright law.

  You may make, run and propagate covered works that you do not
convey, without conditions so long as your license otherwise remains
in force.  You may convey covered works to others for the sole purpose
of having them make modifications exclusively for you, or provide you
with facilities for running those works, provided that you comply with
the terms of this License in conveying all material for which you do
not control copyright.  Those thus making or running the covered works
for you must do so exclusively on your behalf, under your direction
and control, on terms that prohibit them from making any copies of
your copyrighted material outside their relationship with you.

  Conveying under any other circumstances is permitted solely under
the conditions stated below.  Sublicensing is not allowed; section 10
makes it unnecessary.

  3. Protecting Users' Legal Rights From Anti-Circumvention Law.

  No covered work shall be deemed part of an effective technological
measure under any applicable law fulfilling obligations under article
11 of the WIPO copyright treaty adopted on 20 December 1996, or
similar laws prohibiting or restricting circumvention of such
measures.

  When you convey a covered work, you waive any legal power to forbid
circumvention of technological measures to the extent such circumvention
is effected by exercising rights under this License with respect to
the covered work, and you disclaim any intention to limit operation or
modification of the work as a means of enforcing, against the work's
users, your or third parties' legal rights to forbid circumvention of
technological measures.

  4. Conveying Verbatim Copies.

  You may convey verbatim copies of the Program's source code as you
receive it, in any medium, provided that you conspicuously and
appropriately publish on each copy an appropriate copyright notice;
keep intact all notices stating that this License and any
non-permissive terms added in accord with section 7 apply to the code;
keep intact all notices of the absence of any warranty; and give all
recipients a copy of this License along with the Program.

  You may charge any price or no price for each copy that you convey,
and you may offer support or warranty protection for a fee.

  5. Conveying Modified Source Versions.

  You may convey a work based on the Program, or the modifications to
produce it from the Program, in the form of source code under the
terms of section 4, provided that you also meet all of these conditions:

    a) The work must carry prominent notices stating that you modified
    it, and giving a relevant date.

    b) The work must carry prominent notices stating that it is
    released under this License and any conditions added under section
    7.  This requirement modifies the requirement in section 4 to
    "keep intact all notices".

    c) You must license the entire work, as a whole, under this
    License to anyone who comes into possession of a copy.  This
    License will therefore apply, along with any applicable section 7
    additional terms, to the whole of the work, and all its parts,
    regardless of how they are packaged.  This License gives no
    permission to license the work in any other way, but it does not
    invalidate such permission if you have separately received it.

    d) If the work has interactive user interfaces, each must display
    Appropriate Legal Notices; however, if the Program has interactive
    interfaces that do not display Appropriate Legal Notices, your
    work need not make them do so.

  A compilation of a covered work with other separate and independent
works, which are not by their nature extensions of the covered work,
and which are not combined with it such as to form a larger program,
in or on a volume of a storage or distribution medium, is called an
"aggregate" if the compilation and its resulting copyright are not
used to limit the access or legal rights of the compilation's users
beyond what the individual works permit.  Inclusion of a covered work
in an aggregate does not cause this License to apply to the other
parts of the aggregate.

  6. Conveying Non-Source Forms.

  You may convey a covered work in object code form under the terms
of sections 4 and 5, provided that you also convey the
machine-readable Corresponding Source under the terms of this License,
in one of these ways:

    a) Convey the object code in, or embodied in, a physical product
    (including a physical distribution medium), accompanied by the
    Corresponding Source fixed on a durable physical medium
    customarily used for software interchange.

    b) Convey the object code in, or embodied in, a physical product
    (including a physical distribution medium), accompanied by a
    written offer, valid for at least three years and valid for as
    long as you offer spare parts or customer support for that product
    model, to give anyone who possesses the object code either (1) a
    copy of the Corresponding Source for all the software in the
    product that is covered by this License, on a durable physical
    medium customarily used for software interchange, for a price no
    more than your reasonable cost of physically performing this
    conveying of source, or (2) access to copy the
    Corresponding Source from a network server at no charge.

    c) Convey individual copies of the object code with a copy of the
    written offer to provide the Corresponding Source.  This
    alternative is allowed only occasionally and noncommercially, and
    only if you received the object code with such an offer, in accord
    with subsection 6b.

    d) Convey the object code by offering access from a designated
    place (gratis or for a charge), and offer equivalent access to the
    Corresponding Source in the same way through the same place at no
    further charge.  You need not require recipients to copy the
    Corresponding Source along with the object code.  If the place to
    copy the object code is a network server, the Corresponding Source
    may be on a different server (operated by you or a third party)
    that supports equivalent copying facilities, provided you maintain
    clear directions next to the object code saying where to find the
    Corresponding Source.  Regardless of what server hosts the
    Corresponding Source, you remain obligated to ensure that it is
    available for as long as needed to satisfy these requirements.

    e) Convey the object code using peer-to-peer transmission, provided
    you inform other peers where the object code and Corresponding
    Source of the work are being offered to the general public at no
    charge under subsection 6d.

  A separable portion of the object code, whose source code is excluded
from the Corresponding Source as a System Library, need not be
included in conveying the object code work.

  A "User Product" is either (1) a "consumer product", which means any
tangible personal property which is normally used for personal, family,
or household purposes, or (2) anything designed or sold for incorporation
into a dwelling.  In determining whether a product is a consumer product,
doubtful cases shall be resolved in favor of coverage.  For a particular
product received by a particular user, "normally used" refers to a
typical or common use of that class of product, regardless of the status
of the particular user or of the way in which the particular user
actually uses, or expects or is expected to use, the product.  A product
is a consumer product regardless of whether the product has substantial
commercial, industrial or non-consumer uses, unless such uses represent
the only significant mode of use of the product.

  "Installation Information" for a User Product means any methods,
procedures, authorization keys, or other information required to install
and execute modified versions of a covered work in that User Product from
a modified version of its Corresponding Source.  The information must
suffice to ensure that the continued functioning of the modified object
code is in no case prevented or interfered with solely because
modification has been made.

  If you convey an object code work under this section in, or with, or
specifically for use in, a User Product, and the conveying occurs as
part of a transaction in which the right of possession and use of the
User Product is transferred to the recipient in perpetuity or for a
fixed term (regardless of how the transaction is characterized), the
Corresponding Source conveyed under this section must be accompanied
by the Installation Information.  But this requirement does not apply
if neither you nor any third party retains the ability to install
modified object code on the User Product (for example, the work has
been installed in ROM).

  The requirement to provide Installation Information does not include a
requirement to continue to provide support service, warranty, or updates
for a work that has been modified or installed by the recipient, or for
the User Product in which it has been modified or installed.  Access to a
network may be denied when the modification itself materially and
adversely affects the operation of the network or violates the rules and
protocols for communication across the network.

  Corresponding Source conveyed, and Installation Information provided,
in accord with this section must be in a format that is publicly
documented (and with an implementation available to the public in
source code form), and must require no special password or key for
unpacking, reading or copying.

  7. Additional Terms.

  "Additional permissions" are terms that supplement the terms of this
License by making exceptions from one or more of its conditions.
Additional permissions that are applicable to the entire Program shall
be treated as though they were included in this License, to the extent
that they are valid under applicable law.  If additional permissions
apply only to part of the Program, that part may be used separately
under those permissions, but the entire Program remains governed by
this License without regard to the additional permissions.

  When you convey a copy of a covered work, you may at your option
remove any additional permissions from that copy, or from any part of
it.  (Additional permissions may be written to require their own
removal in certain cases when you modify the work.)  You may place
additional permissions on material, added by you to a covered work,
for which you have or can give appropriate copyright permission.

  Notwithstanding any other provision of this License, for material you
add to a covered work, you may (if authorized by the copyright holders of
that material) supplement the terms of this License with terms:

    a) Disclaiming warranty or limiting liability differently from the
    terms of sections 15 and 16 of this License; or

    b) Requiring preservation of specified reasonable legal notices or
    author attributions in that material or in the Appropriate Legal
    Notices displayed by works containing it; or

    c) Prohibiting misrepresentation of the origin of that material, or
    requiring that modified versions of such material be marked in
    reasonable ways as different from the original version; or

    d) Limiting the use for publicity purposes of names of licensors or
    authors of the material; or

    e) Declining to grant rights under trademark law for use of some
    trade names, trademarks, or service marks; or

    f) Requiring indemnification of licensors and authors of that
    material by anyone who conveys the material (or modified versions of
    it) with contractual assumptions of liability to the recipient, for
    any liability that these contractual assumptions directly impose on
    those licensors and authors.

  All other non-permissive additional terms are considered "further
restrictions" within the meaning of section 10.  If the Program as you
received it, or any part of it, contains a notice stating that it is
governed by this License along with a term that is a further
restriction, you may remove that term.  If a license document contains
a further restriction but permits relicensing or conveying under this
License, you may add to a covered work material governed by the terms
of that license document, provided that the further restriction does
not survive such relicensing or conveying.

  If you add terms to a covered work in accord with this section, you
must place, in the relevant source files, a statement of the
additional terms that apply to those files, or a notice indicating
where to find the applicable terms.

  Additional terms, permissive or non-permissive, may be stated in the
form of a separately written license, or stated as exceptions;
the above requirements apply either way.

  8. Termination.

  You may not propagate or modify a covered work except as expressly
provided under this License.  Any attempt otherwise to propagate or
modify it is void, and will automatically terminate your rights under
this License (including any patent licenses granted under the third
paragraph of section 11).

  However, if you cease all violation of this License, then your
license from a particular copyright holder is reinstated (a)
provisionally, unless and until the copyright holder explicitly and
finally terminates your license, and (b) permanently, if the copyright
holder fails to notify you of the violation by some reasonable means
prior to 60 days after the cessation.

  Moreover, your license from a particular copyright holder is
reinstated permanently if the copyright holder notifies you of the
violation by some reasonable means, this is the first time you have
received notice of violation of this License (for any work) from that
copyright holder, and you cure the violation prior to 30 days after
your receipt of the notice.

  Termination of your rights under this section does not terminate the
licenses of parties who have received copies or rights from you under
this License.  If your rights have been terminated and not permanently
reinstated, you do not qualify to receive new licenses for the same
material under section 10.

  9. Acceptance Not Required for Having Copies.

  You are not required to accept this License in order to receive or
run a copy of the Program.  Ancillary propagation of a covered work
occurring solely as a consequence of using peer-to-peer transmission
to receive a copy likewise does not require acceptance.  However,
nothing other than this License grants you permission to propagate or
modify any covered work.  These actions infringe copyright if you do
not accept this License.  Therefore, by modifying or propagating a
covered work, you indicate your acceptance of this License to do so.

  10. Automatic Licensing of Downstream Recipients.

  Each time you convey a covered work, the recipient automatically
receives a license from the original licensors, to run, modify and
propagate that work, subject to this License.  You are not responsible
for enforcing compliance by third parties with this License.

  An "entity transaction" is a transaction transferring control of an
organization, or substantially all assets of one, or subdividing an
organization, or merging organizations.  If propagation of a covered
work results from an entity transaction, each party to that
transaction who receives a copy of the work also receives whatever
licenses to the work the party's predecessor in interest had or could
give under the previous paragraph, plus a right to possession of the
Corresponding Source of the work from the predecessor in interest, if
the predecessor has it or can get it with reasonable efforts.

  You may not impose any further restrictions on the exercise of the
rights granted or affirmed under this License.  For example, you may
not impose a license fee, royalty, or other charge for exercise of
rights granted under this License, and you may not initiate litigation
(including a cross-claim or counterclaim in a lawsuit) alleging that
any patent claim is infringed by making, using, selling, offering for
sale, or importing the Program or any portion of it.

  11. Patents.

  A "contributor" is a copyright holder who authorizes use under this
License of the Program or a work on which the Program is based.  The
work thus licensed is called the contributor's "contributor version".

  A contributor's "essential patent claims" are all patent claims
owned or controlled by the contributor, whether already acquired or
hereafter acquired, that would be infringed by some manner, permitted
by this License, of making, using, or selling its contributor version,
but do not include claims that would be infringed only as a
consequence of further modification of the contributor version.  For
purposes of this definition, "control" includes the right to grant
patent sublicenses in a manner consistent with the requirements of
this License.

  Each contributor grants you a non-exclusive, worldwide, royalty-free
patent license under the contributor's essential patent claims, to
make, use, sell, offer for sale, import and otherwise run, modify and
propagate the contents of its contributor version.

  In the following three paragraphs, a "patent license" is any express
agreement or commitment, however denominated, not to enforce a patent
(such as an express permission to practice a patent or covenant not to
sue for patent infringement).  To "grant" such a patent license to a
party means to make such an agreement or commitment not to enforce a
patent against the party.

  If you convey a covered work, knowingly relying on a patent license,
and the Corresponding Source of the work is not available for anyone
to copy, free of charge and under the terms of this License, through a
publicly available network server or other readily accessible means,
then you must either (1) cause the Corresponding Source to be so
available, or (2) arrange to deprive yourself of the benefit of the
patent license for this particular work, or (3) arrange, in a manner
consistent with the requirements of this License, to extend the patent
license to downstream recipients.  "Knowingly relying" means you have
actual knowledge that, but for the patent license, your conveying the
covered work in a country, or your recipient's use of the covered work
in a country, would infringe one or more identifiable patents in that
country that you have reason to believe are valid.

  If, pursuant to or in connection with a single transaction or
arrangement, you convey, or propagate by procuring conveyance of, a
covered work, and grant a patent license to some of the parties
receiving the covered work authorizing them to use, propagate, modify
or convey a specific copy of the covered work, then the patent license
you grant is automatically extended to all recipients of the covered
work and works based on it.

  A patent license is "discriminatory" if it does not include within
the scope of its coverage, prohibits the exercise of, or is
conditioned on the non-exercise of one or more of the rights that are
specifically granted under this License.  You may not convey a covered
work if you are a party to an arrangement with a third party that is
in the business of distributing software, under which you make payment
to the third party based on the extent of your activity of conveying
the work, and under which the third party grants, to any of the
parties who would receive the covered work from you, a discriminatory
patent license (a) in connection with copies of the covered work
conveyed by you (or copies made from those copies), or (b) primarily
for and in connection with specific products or compilations that
contain the covered work, unless you entered into that arrangement,
or that patent license was granted, prior to 28 March 2007.

  Nothing in this License shall be construed as excluding or limiting
any implied license or other defenses to infringement that may
otherwise be available to you under applicable patent law.

  12. No Surrender of Others' Freedom.

  If conditions are imposed on you (whether by court order, agreement or
otherwise) that contradict the conditions of this License, they do not
excuse you from the conditions of this License.  If you cannot convey a
covered work so as to satisfy simultaneously your obligations under this
License and any other pertinent obligations, then as a consequence you may
not convey it at all.  For example, if you agree to terms that obligate you
to collect a royalty for further conveying from those to whom you convey
the Program, the only way you could satisfy both those terms and this
License would be to refrain entirely from conveying the Program.

  13. Use with the GNU Affero General Public License.

  Notwithstanding any other provision of this License, you have
permission to link or combine any covered work with a work licensed
under version 3 of the GNU Affero General Public License into a single
combined work, and to convey the resulting work.  The terms of this
License will continue to apply to the part which is the covered work,
but the special requirements of the GNU Affero General Public License,
section 13, concerning interaction through a network will apply to the
combination as such.

  14. Revised Versions of this License.

  The Free Software Foundation may publish revised and/or new versions of
the GNU General Public License from time to time.  Such new versions will
be similar in spirit to the present version, but may differ in detail to
address new problems or concerns.

  Each version is given a distinguishing version number.  If the
Program specifies that a certain numbered version of the GNU General
Public License "or any later version" applies to it, you have the
option of following the terms and conditions either of that numbered
version or of any later version published by the Free Software
Foundation.  If the Program does not specify a version number of the
GNU General Public License, you may choose any version ever published
by the Free Software Foundation.

  If the Program specifies that a proxy can decide which future
versions of the GNU General Public License can be used, that proxy's
public statement of acceptance of a version permanently authorizes you
to choose that version for the Program.

  Later license versions may give you additional or different
permissions.  However, no additional obligations are imposed on any
author or copyright holder as a result of your choosing to follow a
later version.

  15. Disclaimer of Warranty.

  THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY
OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM
IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

  16. Limitation of Liability.

  IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS
THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY
GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE
USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF
DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD
PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),
EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGES.

  17. Interpretation of Sections 15 and 16.

  If the disclaimer of warranty and limitation of liability provided
above cannot be given local legal effect according to their terms,
reviewing courts shall apply local law that most closely approximates
an absolute waiver of all civil liability in connection with the
Program, unless a warranty or assumption of liability accompanies a
copy of the Program in return for a fee.

                     END OF TERMS AND CONDITIONS

            How to Apply These Terms to Your New Programs

  If you develop a new program, and you want it to be of the greatest
possible use to the public, the best way to achieve this is to make it
free software which everyone can redistribute and change under these terms.

  To do so, attach the following notices to the program.  It is safest
to attach them to the start of each source file to most effectively
state the exclusion of warranty; and each file should have at least
the "copyright" line and a pointer to where the full notice is found.

    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

Also add information on how to contact you by electronic and paper mail.

  If the program does terminal interaction, make it output a short
notice like this when it starts in an interactive mode:

    <program>  Copyright (C) <year>  <name of author>
    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type `show c' for details.

The hypothetical commands `show w' and `show c' should show the appropriate
parts of the General Public License.  Of course, your program's commands
might be different; for a GUI interface, you would use an "about box".

  You should also get your employer (if you work as a programmer) or school,
if any, to sign a "copyright disclaimer" for the program, if necessary.
For more information on this, and how to apply and follow the GNU GPL, see
<https://www.gnu.org/licenses/>.

  The GNU General Public License does not permit incorporating your program
into proprietary programs.  If your program is a subroutine library, you
may consider it more useful to permit linking proprietary applications with
the library.  If this is what you want to do, use the GNU Lesser General
Public License instead of this License.  But first, please read
<https://www.gnu.org/licenses/why-not-lgpl.html>.
```

TAPR Open Hardware License v1.0
```
The TAPR Open Hardware License
Version 1.0 (May 25, 2007)
Copyright 2007 TAPR - http://www.tapr.org/OHL

PREAMBLE

Open Hardware is a thing - a physical artifact, either electrical or
mechanical - whose design information is available to, and usable by,
the public in a way that allows anyone to make, modify, distribute, and
use that thing.  In this preface, design information is called
"documentation" and things created from it are called "products."

The TAPR Open Hardware License ("OHL") agreement provides a legal
framework for Open Hardware projects.  It may be used for any kind of
product, be it a hammer or a computer motherboard, and is TAPR's
contribution to the community; anyone may use the OHL for their Open
Hardware project.

Like the GNU General Public License, the OHL is designed to guarantee
your freedom to share and to create.  It forbids anyone who receives
rights under the OHL to deny any other licensee those same rights to
copy, modify, and distribute documentation, and to make, use and
distribute products based on that documentation.

Unlike the GPL, the OHL is not primarily a copyright license.  While
copyright protects documentation from unauthorized copying, modification, 
and distribution, it has little to do with your right to make, distribute, 
or use a product based on that documentation.  For better or worse, patents 
play a significant role in those activities.  Although it does not prohibit 
anyone from patenting inventions embodied in an Open Hardware design, and 
of course cannot prevent a third party from enforcing their patent rights, 
those who benefit from an OHL design may not bring lawsuits claiming that
design infringes their patents or other intellectual property.

The OHL addresses unique issues involved in the creation of tangible,
physical things, but does not cover software, firmware, or code loaded
into programmable devices.  A copyright-oriented license such as the GPL
better suits these creations.

How can you use the OHL, or a design based upon it?  While the terms and
conditions below take precedence over this preamble, here is a summary:

*  You may modify the documentation and make products based upon it.

*  You may use products for any legal purpose without limitation.

*  You may distribute unmodified documentation, but you must include the
complete package as you received it.

*  You may distribute products you make to third parties, if you either
include the documentation on which the product is based, or make it
available without charge for at least three years to anyone who requests
it.

*  You may distribute modified documentation or products based on it, if
you:
    *  License your modifications under the OHL.
    *  Include those modifications, following the requirements stated
       below.
    *  Attempt to send the modified documentation by email to any of the
       developers who have provided their email address.  This is a good
       faith obligation - if the email fails, you need do nothing more
       and may go on with your distribution.

*  If you create a design that you want to license under the OHL, you
should:
    *  Include this document in a file named LICENSE (with the appropriate
       extension) that is included in the documentation package.
    *  If the file format allows, include a notice like "Licensed under
       the TAPR Open Hardware License (www.tapr.org/OHL)" in each
       documentation file.  While not required, you should also include
       this notice on printed circuit board artwork and the product
       itself; if space is limited the notice can be shortened or
       abbreviated.
    *  Include a copyright notice in each file and on printed circuit
       board artwork.
    *  If you wish to be notified of modifications that others may make,
       include your email address in a file named "CONTRIB.TXT" or
       something similar.

*  Any time the OHL requires you to make documentation available to
others, you must include all the materials you received from the
upstream licensors.  In addition, if you have modified the
documentation:
    *  You must identify the modifications in a text file (preferably
       named "CHANGES.TXT") that you include with the documentation. 
       That file must also include a statement like "These modifications
       are licensed under the TAPR Open Hardware License."
    *  You must include any new files you created, including any
       manufacturing files (such as Gerber files) you create in the
       course of making products.
    *  You must include both "before" and "after" versions of all files
       you modified.
    *  You may include files in proprietary formats, but you must also
       include open format versions (such as Gerber, ASCII, Postscript,
       or PDF) if your tools can create them.

TERMS AND CONDITIONS

1.   Introduction
1.1  This Agreement governs how you may use, copy, modify, and
distribute Documentation, and how you may make, have made, and
distribute Products based on that Documentation.  As used in this
Agreement, to "distribute" Documentation means to directly or indirectly
make copies available to a third party, and to "distribute" Products
means to directly or indirectly give, loan, sell or otherwise transfer
them to a third party.

1.2  "Documentation" includes:
     (a) schematic diagrams;
     (b) circuit or circuit board layouts, including Gerber and other
         data files used for manufacture;
     (c) mechanical drawings, including CAD, CAM, and other data files
         used for manufacture;
     (d) flow charts and descriptive text; and
     (e) other explanatory material.
Documentation may be in any tangible or intangible form of expression,
including but not limited to computer files in open or proprietary
formats and representations on paper, film, or other media.

1.3  "Products" include:
     (a) circuit boards, mechanical assemblies, and other physical parts
         and components;
     (b) assembled or partially assembled units (including components
         and subassemblies); and
     (c) parts and components combined into kits intended for assembly
         by others;
which are based in whole or in part on the Documentation.

1.4  This Agreement applies to any Documentation which contains a
notice stating it is subject to the TAPR Open Hardware License, and to
all Products based in whole or in part on that Documentation.  If
Documentation is distributed in an archive (such as a "zip" file) which
includes this document, all files in that archive are subject to this
Agreement unless they are specifically excluded.  Each person who
contributes content to the Documentation is referred to in this
Agreement as a "Licensor."

1.5  By (a) using, copying, modifying, or distributing the
Documentation, or (b) making or having Products made or distributing
them, you accept this Agreement, agree to comply with its terms, and
become a "Licensee."  Any activity inconsistent with this Agreement will
automatically terminate your rights under it (including the immunities
from suit granted in Section 2), but the rights of others who have
received Documentation, or have obtained Products, directly or
indirectly from you will not be affected so long as they fully comply
with it themselves.

1.6  This Agreement does not apply to software, firmware, or code
loaded into programmable devices which may be used in conjunction with
Documentation or Products.  Such software is subject to the license
terms established by its copyright holder(s).

2.   Patents
2.1  Each Licensor grants you, every other Licensee, and every
possessor or user of Products a perpetual, worldwide, and royalty-free
immunity from suit under any patent, patent application, or other
intellectual property right which he or she controls, to the extent
necessary to make, have made, possess, use, and distribute Products.
This immunity does not extend to infringement arising from modifications
subsequently made by others.

2.2  If you make or have Products made, or distribute Documentation
that you have modified, you grant every Licensor, every other Licensee,
and every possessor or user of Products a perpetual, worldwide, and
royalty-free immunity from suit under any patent, patent application, or
other intellectual property right which you control, to the extent
necessary to make, have made, possess, use, and distribute Products.
This immunity does not extend to infringement arising from modifications
subsequently made by others.

2.3  To avoid doubt, providing Documentation to a third party for the
sole purpose of having that party make Products on your behalf is not
considered "distribution,"\" and a third party's act of making Products
solely on your behalf does not cause that party to grant the immunity
described in the preceding paragraph.

2.4  These grants of immunity are a material part of this Agreement,
and form a portion of the consideration given by each party to the
other.  If any court judgment or legal agreement prevents you from
granting the immunity required by this Section, your rights under this
Agreement will terminate and you may no longer use, copy, modify or
distribute the Documentation, or make, have made, or distribute
Products.

3.   Modifications
You may modify the Documentation, and those modifications will become
part of the Documentation.  They are subject to this Agreement, as are
Products based in whole or in part on them.  If you distribute the
modified Documentation, or Products based in whole or in part upon it,
you must email the modified Documentation in a form compliant with
Section 4 to each Licensor who has provided an email address with the
Documentation.  Attempting to send the email completes your obligations
under this Section and you need take no further action if any address
fails.

4.   Distributing Documentation
4.1  You may distribute unmodified copies of the Documentation in its
entirety in any medium, provided that you retain all copyright and other
notices (including references to this Agreement) included by each
Licensor, and include an unaltered copy of this Agreement.
4.2  You may distribute modified copies of the Documentation if you
comply with all the requirements of the preceding paragraph and:
     (a) include a prominent notice in an ASCII or other open format
         file identifying those elements of the Documentation that you
         changed, and stating that the modifications are licensed under
         the terms of this Agreement;
     (b) include all new documentation files that you create, as well as
         both the original and modified versions of each file you change
         (files may be in your development tool's native file format,
         but if reasonably possible, you must also include open format,
         such as Gerber, ASCII, Postscript, or PDF, versions);
     (c) do not change the terms of this Agreement with respect to
         subsequent licensees; and
     (d) if you make or have Products made, include in the Documentation
         all elements reasonably required to permit others to make
         Products, including Gerber, CAD/CAM and other files used for
         manufacture.

5.   Making Products
5.1  You may use the Documentation to make or have Products made,
provided that each Product retains any notices included by the Licensor
(including, but not limited to, copyright notices on circuit boards).
5.2  You may distribute Products you make or have made, provided that
you include with each unit a copy of the Documentation in a form
consistent with Section 4.  Alternatively, you may include either (i) an
offer valid for at least three years to provide that Documentation, at
no charge other than the reasonable cost of media and postage, to any
person who requests it; or (ii) a URL where that Documentation may be
downloaded, available for at least three years after you last distribute
the Product.

6.   NEW LICENSE VERSIONS
TAPR may publish updated versions of the OHL which retain the same
general provisions as the present version, but differ in detail to
address new problems or concerns, and carry a distinguishing version
number.  If the Documentation specifies a version number which applies
to it and "any later version", you may choose either that version or any
later version published by TAPR.  If the Documentation does not specify
a version number, you may choose any version ever published by TAPR. 
TAPR owns the copyright to the OHL, but grants permission to any person
to copy, distribute, and use it in unmodified form.

7.   WARRANTY AND LIABILITY LIMITATIONS
7.1  THE DOCUMENTATION IS PROVIDED ON AN"AS-IS" BASIS WITHOUT
WARRANTY OF ANY KIND, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  ALL
WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
TITLE, ARE HEREBY EXPRESSLY DISCLAIMED.
7.2  IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW WILL ANY LICENSOR
BE LIABLE TO YOU OR ANY THIRD PARTY FOR ANY DIRECT, INDIRECT,
INCIDENTAL, CONSEQUENTIAL, PUNITIVE, OR EXEMPLARY DAMAGES ARISING OUT OF
THE USE OF, OR INABILITY TO USE, THE DOCUMENTATION OR PRODUCTS,
INCLUDING BUT NOT LIMITED TO CLAIMS OF INTELLECTUAL PROPERTY
INFRINGEMENT OR LOSS OF DATA, EVEN IF THAT PARTY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.
7.3  You agree that the foregoing limitations are reasonable due to
the non-financial nature of the transaction represented by this
Agreement, and acknowledge that were it not for these limitations, the
Licensor(s) would not be willing to make the Documentation available to
you.
7.4  You agree to defend, indemnify, and hold each Licensor harmless
from any claim brought by a third party alleging any defect in the
design, manufacture, or operation of any Product which you make, have
made, or distribute pursuant to this Agreement.
                                 ####
```