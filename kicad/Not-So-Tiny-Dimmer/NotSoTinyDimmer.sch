EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Not-So-Tiny Dimmer"
Date ""
Rev "1.3.3"
Comp ""
Comment1 ""
Comment2 "https://tapr.org/the-tapr-open-hardware-license/"
Comment3 "License: TAPR Open Hardware License v1.0"
Comment4 "Author: Patrick Pedersen"
$EndDescr
Text Notes 8750 3800 0    50   ~ 0
ICSP
Wire Notes Line
	950  6300 10700 6300
Wire Notes Line
	950  3650 950  6300
Wire Wire Line
	10250 5000 9950 5000
Wire Wire Line
	10250 5150 10250 5000
Wire Wire Line
	10250 4800 9950 4800
Wire Wire Line
	10250 4650 10250 4800
Wire Wire Line
	9300 4900 9450 4900
Wire Wire Line
	9450 5000 9300 5000
Wire Wire Line
	10100 4900 9950 4900
Wire Wire Line
	9300 4800 9450 4800
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J2
U 1 1 5EAC515A
P 9650 4900
F 0 "J2" H 9700 5217 50  0000 C CNN
F 1 "Conn_02x03_Odd_Even" H 9700 5126 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x03_P2.54mm_Vertical" H 9650 4900 50  0001 C CNN
F 3 "~" H 9650 4900 50  0001 C CNN
	1    9650 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 5750 3650 5950
Connection ~ 3650 5750
Connection ~ 2750 5750
Wire Wire Line
	2750 5750 3650 5750
Wire Wire Line
	3650 5400 3650 5750
Connection ~ 2250 5750
Wire Wire Line
	1750 5750 2250 5750
Wire Wire Line
	1750 4300 1750 5750
Connection ~ 2250 4100
Wire Wire Line
	1600 4100 2250 4100
Wire Wire Line
	2250 5750 2750 5750
Wire Wire Line
	2250 5050 2250 5750
Wire Wire Line
	2750 5050 2750 5750
Wire Wire Line
	3650 4100 3650 4500
Connection ~ 3650 4100
Connection ~ 2750 4100
Wire Wire Line
	2750 4100 3650 4100
Wire Wire Line
	2250 4100 2250 4750
Wire Wire Line
	2750 4100 2250 4100
Wire Wire Line
	2750 4750 2750 4100
$Comp
L Device:CP1 C1
U 1 1 5E9CA3FB
P 2250 4900
F 0 "C1" H 2365 4946 50  0000 L CNN
F 1 "10uF" H 2365 4855 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D5.0mm_P2.00mm" H 2250 4900 50  0001 C CNN
F 3 "~" H 2250 4900 50  0001 C CNN
	1    2250 4900
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5E9C847F
P 2750 4900
F 0 "C2" H 2865 4946 50  0000 L CNN
F 1 "100nF" H 2865 4855 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D10.0mm_W2.5mm_P5.00mm" H 2788 4750 50  0001 C CNN
F 3 "~" H 2750 4900 50  0001 C CNN
	1    2750 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 4000 3650 4100
Wire Wire Line
	1750 4300 1600 4300
$Comp
L Connector:Jack-DC J1
U 1 1 5E9BBCF5
P 1300 4200
F 0 "J1" H 1357 4525 50  0000 C CNN
F 1 "Jack-DC" H 1357 4434 50  0000 C CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 1350 4160 50  0001 C CNN
F 3 "~" H 1350 4160 50  0001 C CNN
	1    1300 4200
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR03
U 1 1 5E8839D8
P 10250 4650
F 0 "#PWR03" H 10250 4500 50  0001 C CNN
F 1 "+5V" H 10265 4823 50  0000 C CNN
F 2 "" H 10250 4650 50  0001 C CNN
F 3 "" H 10250 4650 50  0001 C CNN
	1    10250 4650
	1    0    0    -1  
$EndComp
Text GLabel 9300 5000 0    50   Input ~ 0
RST-PB5
$Comp
L power:GND #PWR04
U 1 1 5E87AFDD
P 10250 5150
F 0 "#PWR04" H 10250 4900 50  0001 C CNN
F 1 "GND" H 10255 4977 50  0000 C CNN
F 2 "" H 10250 5150 50  0001 C CNN
F 3 "" H 10250 5150 50  0001 C CNN
	1    10250 5150
	1    0    0    -1  
$EndComp
Text GLabel 10100 4900 2    50   Input ~ 0
MOSI-PB0
Text GLabel 9300 4900 0    50   Input ~ 0
SCK-PB2
Text GLabel 9300 4800 0    50   Input ~ 0
MISO-PB1
$Comp
L power:GND #PWR02
U 1 1 5E8578B9
P 3650 5950
F 0 "#PWR02" H 3650 5700 50  0001 C CNN
F 1 "GND" H 3655 5777 50  0000 C CNN
F 2 "" H 3650 5950 50  0001 C CNN
F 3 "" H 3650 5950 50  0001 C CNN
	1    3650 5950
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 5E8568EC
P 3650 4000
F 0 "#PWR01" H 3650 3850 50  0001 C CNN
F 1 "+5V" H 3665 4173 50  0000 C CNN
F 2 "" H 3650 4000 50  0001 C CNN
F 3 "" H 3650 4000 50  0001 C CNN
	1    3650 4000
	1    0    0    -1  
$EndComp
$Comp
L dk_Embedded-Microcontrollers:ATTINY85-20PU U1
U 1 1 5E854FF7
P 3750 4900
F 0 "U1" H 3978 4903 60  0000 L CNN
F 1 "ATTINYx5" H 3978 4797 60  0000 L CNN
F 2 "digikey-footprints:DIP-8_W7.62mm" H 3950 5100 60  0001 L CNN
F 3 "http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en589894" H 3950 5200 60  0001 L CNN
F 4 "ATTINY85-20PU-ND" H 3950 5300 60  0001 L CNN "Digi-Key_PN"
F 5 "ATTINY85-20PU" H 3950 5400 60  0001 L CNN "MPN"
F 6 "Integrated Circuits (ICs)" H 3950 5500 60  0001 L CNN "Category"
F 7 "Embedded - Microcontrollers" H 3950 5600 60  0001 L CNN "Family"
F 8 "http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en589894" H 3950 5700 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/microchip-technology/ATTINY85-20PU/ATTINY85-20PU-ND/735469" H 3950 5800 60  0001 L CNN "DK_Detail_Page"
F 10 "IC MCU 8BIT 8KB FLASH 8DIP" H 3950 5900 60  0001 L CNN "Description"
F 11 "Microchip Technology" H 3950 6000 60  0001 L CNN "Manufacturer"
F 12 "Active" H 3950 6100 60  0001 L CNN "Status"
	1    3750 4900
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4200 4800 3950 4800
NoConn ~ 3950 5000
$Comp
L power:PWR_FLAG #FLG01
U 1 1 5EB819B2
P 2250 3950
F 0 "#FLG01" H 2250 4025 50  0001 C CNN
F 1 "PWR_FLAG" H 2250 4123 50  0000 C CNN
F 2 "" H 2250 3950 50  0001 C CNN
F 3 "~" H 2250 3950 50  0001 C CNN
	1    2250 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 3950 2250 4100
$Comp
L power:PWR_FLAG #FLG02
U 1 1 5EB83634
P 2250 5950
F 0 "#FLG02" H 2250 6025 50  0001 C CNN
F 1 "PWR_FLAG" H 2250 6123 50  0000 C CNN
F 2 "" H 2250 5950 50  0001 C CNN
F 3 "~" H 2250 5950 50  0001 C CNN
	1    2250 5950
	-1   0    0    1   
$EndComp
Wire Wire Line
	2250 5950 2250 5750
Wire Wire Line
	4400 5200 3950 5200
Connection ~ 4400 5200
Wire Wire Line
	4300 4900 3950 4900
Connection ~ 4300 4900
Connection ~ 4100 4700
Wire Wire Line
	3950 4700 4100 4700
Wire Notes Line
	10700 6300 10700 3650
Wire Notes Line
	8700 6300 8700 3650
Wire Notes Line
	950  3650 10700 3650
Wire Wire Line
	4400 4600 4400 5200
Wire Wire Line
	4300 4600 4300 4900
Wire Wire Line
	4200 4600 4200 4800
Wire Wire Line
	4100 4600 4100 4700
Text GLabel 4200 4600 1    50   Input ~ 0
MISO-PB1
Text GLabel 4300 4600 1    50   Input ~ 0
SCK-PB2
Text GLabel 4400 4600 1    50   Input ~ 0
RST-PB5
Text GLabel 4100 4600 1    50   Input ~ 0
MOSI-PB0
Wire Wire Line
	5350 4900 4300 4900
Wire Wire Line
	4650 5200 4400 5200
$Comp
L Device:R_POT RV1
U 1 1 5E865E0F
P 5000 5100
F 0 "RV1" H 4931 5146 50  0000 R CNN
F 1 "10k" H 4931 5055 50  0000 R CNN
F 2 "Potentiometer_THT:Potentiometer_Alpha_RD901F-40-00D_Single_Vertical" H 5000 5100 50  0001 C CNN
F 3 "~" H 5000 5100 50  0001 C CNN
	1    5000 5100
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6000 5750 6700 5750
Wire Wire Line
	6700 4900 6700 5750
Wire Wire Line
	6550 4900 6700 4900
Wire Wire Line
	6000 5350 6000 5750
Wire Wire Line
	6000 4900 5650 4900
Connection ~ 6000 4900
Wire Wire Line
	6000 5050 6000 4900
$Comp
L Device:CP1 C3
U 1 1 5E9FB50B
P 6000 5200
F 0 "C3" H 6115 5246 50  0000 L CNN
F 1 "100nF" H 6115 5155 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D10.0mm_W2.5mm_P5.00mm" H 6000 5200 50  0001 C CNN
F 3 "~" H 6000 5200 50  0001 C CNN
	1    6000 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 4900 6000 4900
$Comp
L Device:R R2
U 1 1 5E9F7182
P 5500 4900
F 0 "R2" V 5400 4900 50  0000 C CNN
F 1 "1k" V 5600 4900 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 5430 4900 50  0001 C CNN
F 3 "~" H 5500 4900 50  0001 C CNN
	1    5500 4900
	0    1    1    0   
$EndComp
$Comp
L dk_Pushbutton-Switches:GPTS203211B S1
U 1 1 5E8584FE
P 6350 4900
F 0 "S1" H 6200 5050 50  0000 C CNN
F 1 "Push Button" H 6350 5084 50  0001 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 6550 5100 50  0001 L CNN
F 3 "http://switches-connectors-custom.cwind.com/Asset/GPTS203211BR2.pdf" H 6550 5200 60  0001 L CNN
F 4 "CW181-ND" H 6550 5300 60  0001 L CNN "Digi-Key_PN"
F 5 "GPTS203211B" H 6550 5400 60  0001 L CNN "MPN"
F 6 "Switches" H 6550 5500 60  0001 L CNN "Category"
F 7 "Pushbutton Switches" H 6550 5600 60  0001 L CNN "Family"
F 8 "http://switches-connectors-custom.cwind.com/Asset/GPTS203211BR2.pdf" H 6550 5700 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/cw-industries/GPTS203211B/CW181-ND/3190590" H 6550 5800 60  0001 L CNN "DK_Detail_Page"
F 10 "SWITCH PUSHBUTTON SPST 1A 30V" H 6550 5900 60  0001 L CNN "Description"
F 11 "CW Industries" H 6550 6000 60  0001 L CNN "Manufacturer"
F 12 "Active" H 6550 6100 60  0001 L CNN "Status"
	1    6350 4900
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x03 WS2812
U 1 1 5EA588DD
P 8200 4700
F 0 "WS2812" H 8280 4742 50  0000 L CNN
F 1 "Screw_Terminal_01x03" H 8280 4651 50  0001 L CNN
F 2 "TerminalBlock_TE-Connectivity:TerminalBlock_TE_282834-3_1x03_P2.54mm_Horizontal" H 8200 4700 50  0001 C CNN
F 3 "~" H 8200 4700 50  0001 C CNN
	1    8200 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 4100 4650 4100
Wire Wire Line
	8000 4100 8000 4600
Wire Wire Line
	8000 4800 8000 5750
Connection ~ 6000 5750
Wire Wire Line
	6000 5750 5000 5750
Connection ~ 6700 5750
Wire Wire Line
	4850 5100 3950 5100
Wire Wire Line
	5000 4100 5000 4950
Wire Wire Line
	5000 5250 5000 5750
Connection ~ 5000 5750
Wire Wire Line
	5000 5750 3650 5750
$Comp
L Device:R R1
U 1 1 5EB0B5D0
P 4650 4400
F 0 "R1" H 4720 4446 50  0000 L CNN
F 1 "10k" H 4720 4355 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 4580 4400 50  0001 C CNN
F 3 "~" H 4650 4400 50  0001 C CNN
	1    4650 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 4250 4650 4100
Connection ~ 4650 4100
Wire Wire Line
	4650 4550 4650 5200
Text Notes 8050 4500 0    50   ~ 0
5V
Text Notes 8050 4950 0    50   ~ 0
GND
Text Notes 7900 4700 0    50   ~ 0
DIN
$Comp
L Graphic:Logo_Open_Hardware_Small #LOGO1
U 1 1 5EBC2D69
P 10500 3300
F 0 "#LOGO1" H 10500 3575 50  0001 C CNN
F 1 "Logo_Open_Hardware_Small" H 10500 3075 50  0001 C CNN
F 2 "" H 10500 3300 50  0001 C CNN
F 3 "~" H 10500 3300 50  0001 C CNN
	1    10500 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 4100 5000 4100
Connection ~ 5000 4100
Wire Wire Line
	5000 4100 8000 4100
Wire Wire Line
	4100 4700 8000 4700
Wire Wire Line
	6700 5750 8000 5750
$EndSCHEMATC
