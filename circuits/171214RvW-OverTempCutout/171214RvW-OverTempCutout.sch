EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Amplifier_Operational:TS912 U1
U 1 1 618911AB
P 4850 4100
F 0 "U1" H 4850 4467 50  0000 C CNN
F 1 "TS912" H 4850 4376 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 4850 4100 50  0001 C CNN
F 3 "www.st.com/resource/en/datasheet/ts912.pdf" H 4850 4100 50  0001 C CNN
	1    4850 4100
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:TS912 U1
U 2 1 6189629D
P 7450 4000
F 0 "U1" H 7450 4367 50  0000 C CNN
F 1 "TS912" H 7450 4276 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 7450 4000 50  0001 C CNN
F 3 "www.st.com/resource/en/datasheet/ts912.pdf" H 7450 4000 50  0001 C CNN
	2    7450 4000
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:TS912 U1
U 3 1 61897CCD
P 4900 5650
F 0 "U1" H 4858 5696 50  0000 L CNN
F 1 "TS912" H 4858 5605 50  0000 L CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 4900 5650 50  0001 C CNN
F 3 "www.st.com/resource/en/datasheet/ts912.pdf" H 4900 5650 50  0001 C CNN
	3    4900 5650
	1    0    0    -1  
$EndComp
$Comp
L Device:R_POT_US R2
U 1 1 618A4318
P 4100 3300
F 0 "R2" H 4033 3346 50  0000 R CNN
F 1 "5K" H 4033 3255 50  0000 R CNN
F 2 "Potentiometer_THT:Potentiometer_Vishay_T73XX_Horizontal" H 4100 3300 50  0001 C CNN
F 3 "~" H 4100 3300 50  0001 C CNN
	1    4100 3300
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R1
U 1 1 618A4C1E
P 4300 3000
F 0 "R1" H 4368 3046 50  0000 L CNN
F 1 "10K" H 4368 2955 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0516_L15.5mm_D5.0mm_P20.32mm_Horizontal" V 4340 2990 50  0001 C CNN
F 3 "~" H 4300 3000 50  0001 C CNN
	1    4300 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:D D1
U 1 1 618A5419
P 8550 2950
F 0 "D1" H 8550 2850 50  0000 C CNN
F 1 " " H 8550 3075 50  0000 C CNN
F 2 "Diode_THT:D_5KP_P10.16mm_Horizontal" H 8550 2950 50  0001 C CNN
F 3 "~" H 8550 2950 50  0001 C CNN
	1    8550 2950
	-1   0    0    1   
$EndComp
$Comp
L Device:CP C1
U 1 1 618A51E3
P 7850 2550
F 0 "C1" H 7968 2596 50  0000 L CNN
F 1 "1uF" H 7968 2505 50  0000 L CNN
F 2 "Capacitor_THT:C_Axial_L22.0mm_D9.5mm_P27.50mm_Horizontal" H 7888 2400 50  0001 C CNN
F 3 "~" H 7850 2550 50  0001 C CNN
	1    7850 2550
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x04_Odd_Even J4
U 1 1 618A83F0
P 3800 2000
F 0 "J4" H 3850 2317 50  0000 C CNN
F 1 "Conn_02x04_Odd_Even" H 3850 2226 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x04_P2.54mm_Vertical" H 3800 2000 50  0001 C CNN
F 3 "~" H 3800 2000 50  0001 C CNN
	1    3800 2000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0101
U 1 1 618A99B2
P 3600 1450
F 0 "#PWR0101" H 3600 1300 50  0001 C CNN
F 1 "+5V" H 3615 1623 50  0000 C CNN
F 2 "" H 3600 1450 50  0001 C CNN
F 3 "" H 3600 1450 50  0001 C CNN
	1    3600 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 1900 3600 1450
Wire Wire Line
	4100 1900 4100 1450
Wire Wire Line
	4100 1450 3600 1450
Connection ~ 3600 1450
$Comp
L power:GND #PWR0102
U 1 1 618ACC39
P 3600 2450
F 0 "#PWR0102" H 3600 2200 50  0001 C CNN
F 1 "GND" H 3605 2277 50  0000 C CNN
F 2 "" H 3600 2450 50  0001 C CNN
F 3 "" H 3600 2450 50  0001 C CNN
	1    3600 2450
	1    0    0    -1  
$EndComp
NoConn ~ 4100 2100
NoConn ~ 4100 2200
Wire Wire Line
	3600 2200 3600 2450
Text Notes 1900 1600 0    50   ~ 0
Bridge A(2) and B(4) to unlatch
Wire Wire Line
	3450 2100 3600 2100
NoConn ~ 4100 3150
$Comp
L power:GND #PWR0103
U 1 1 618B486D
P 4100 3700
F 0 "#PWR0103" H 4100 3450 50  0001 C CNN
F 1 "GND" H 4105 3527 50  0000 C CNN
F 2 "" H 4100 3700 50  0001 C CNN
F 3 "" H 4100 3700 50  0001 C CNN
	1    4100 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 3450 4100 3700
$Comp
L power:+5V #PWR0104
U 1 1 618B8B08
P 4300 2700
F 0 "#PWR0104" H 4300 2550 50  0001 C CNN
F 1 "+5V" H 4315 2873 50  0000 C CNN
F 2 "" H 4300 2700 50  0001 C CNN
F 3 "" H 4300 2700 50  0001 C CNN
	1    4300 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 2700 4300 2850
Wire Wire Line
	4300 3150 4300 3300
Wire Wire Line
	4300 3300 4250 3300
Wire Wire Line
	4550 4000 4550 3300
Wire Wire Line
	4550 3300 4300 3300
Connection ~ 4300 3300
Wire Wire Line
	5150 4100 5400 4100
$Comp
L Device:R_US R12
U 1 1 618BB0A3
P 5550 4100
F 0 "R12" V 5650 4050 50  0000 L CNN
F 1 "47K" V 5450 4000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0516_L15.5mm_D5.0mm_P20.32mm_Horizontal" V 5590 4090 50  0001 C CNN
F 3 "~" H 5550 4100 50  0001 C CNN
	1    5550 4100
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_US R13
U 1 1 618BD670
P 6200 3600
F 0 "R13" H 6268 3646 50  0000 L CNN
F 1 "47K" H 6268 3555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0516_L15.5mm_D5.0mm_P20.32mm_Horizontal" V 6240 3590 50  0001 C CNN
F 3 "~" H 6200 3600 50  0001 C CNN
	1    6200 3600
	-1   0    0    1   
$EndComp
$Comp
L Device:R_US R14
U 1 1 618BDBEC
P 6200 4350
F 0 "R14" H 6268 4396 50  0000 L CNN
F 1 "47K" H 6268 4305 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0516_L15.5mm_D5.0mm_P20.32mm_Horizontal" V 6240 4340 50  0001 C CNN
F 3 "~" H 6200 4350 50  0001 C CNN
	1    6200 4350
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 618BFEA1
P 6200 4750
F 0 "#PWR0105" H 6200 4500 50  0001 C CNN
F 1 "GND" H 6205 4577 50  0000 C CNN
F 2 "" H 6200 4750 50  0001 C CNN
F 3 "" H 6200 4750 50  0001 C CNN
	1    6200 4750
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0106
U 1 1 618C029F
P 6200 3300
F 0 "#PWR0106" H 6200 3150 50  0001 C CNN
F 1 "+5V" H 6215 3473 50  0000 C CNN
F 2 "" H 6200 3300 50  0001 C CNN
F 3 "" H 6200 3300 50  0001 C CNN
	1    6200 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 3750 6200 4100
Wire Wire Line
	5700 4100 6200 4100
Connection ~ 6200 4100
Wire Wire Line
	6200 4100 6200 4200
Wire Wire Line
	6200 3300 6200 3450
Wire Wire Line
	6200 4500 6200 4750
$Comp
L power:+5V #PWR0107
U 1 1 618C4658
P 4800 5100
F 0 "#PWR0107" H 4800 4950 50  0001 C CNN
F 1 "+5V" H 4815 5273 50  0000 C CNN
F 2 "" H 4800 5100 50  0001 C CNN
F 3 "" H 4800 5100 50  0001 C CNN
	1    4800 5100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0108
U 1 1 618C5048
P 4800 6200
F 0 "#PWR0108" H 4800 5950 50  0001 C CNN
F 1 "GND" H 4805 6027 50  0000 C CNN
F 2 "" H 4800 6200 50  0001 C CNN
F 3 "" H 4800 6200 50  0001 C CNN
	1    4800 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 5950 4800 6200
Wire Wire Line
	4800 5100 4800 5350
Wire Wire Line
	3450 2100 3450 4200
Wire Wire Line
	4550 4200 3450 4200
Wire Wire Line
	4100 5900 4100 6000
Wire Wire Line
	4350 5450 4100 5450
Wire Wire Line
	4350 5350 4350 5450
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 618A7479
P 4100 5900
F 0 "#FLG0101" H 4100 5975 50  0001 C CNN
F 1 "PWR_FLAG" H 4100 6073 50  0000 C CNN
F 2 "" H 4100 5900 50  0001 C CNN
F 3 "~" H 4100 5900 50  0001 C CNN
	1    4100 5900
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 618A6F85
P 4100 5450
F 0 "#FLG0102" H 4100 5525 50  0001 C CNN
F 1 "PWR_FLAG" H 4100 5623 50  0000 C CNN
F 2 "" H 4100 5450 50  0001 C CNN
F 3 "~" H 4100 5450 50  0001 C CNN
	1    4100 5450
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0109
U 1 1 618A6156
P 4350 5350
F 0 "#PWR0109" H 4350 5200 50  0001 C CNN
F 1 "+5V" H 4365 5523 50  0000 C CNN
F 2 "" H 4350 5350 50  0001 C CNN
F 3 "" H 4350 5350 50  0001 C CNN
	1    4350 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 618A5869
P 4100 6000
F 0 "#PWR0110" H 4100 5750 50  0001 C CNN
F 1 "GND" H 4105 5827 50  0000 C CNN
F 2 "" H 4100 6000 50  0001 C CNN
F 3 "" H 4100 6000 50  0001 C CNN
	1    4100 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_US R10
U 1 1 618CDF0F
P 8200 2950
F 0 "R10" V 8100 2900 50  0000 L CNN
F 1 "10K" V 8300 2800 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0516_L15.5mm_D5.0mm_P20.32mm_Horizontal" V 8240 2940 50  0001 C CNN
F 3 "~" H 8200 2950 50  0001 C CNN
	1    8200 2950
	0    1    1    0   
$EndComp
Wire Wire Line
	7850 2700 7850 2950
Wire Wire Line
	7850 2950 8050 2950
Wire Wire Line
	8350 2950 8400 2950
$Comp
L Device:R_US R15
U 1 1 618D76AB
P 6900 2600
F 0 "R15" H 6968 2646 50  0000 L CNN
F 1 "47K" H 6968 2555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0516_L15.5mm_D5.0mm_P20.32mm_Horizontal" V 6940 2590 50  0001 C CNN
F 3 "~" H 6900 2600 50  0001 C CNN
	1    6900 2600
	-1   0    0    1   
$EndComp
$Comp
L Device:R_US R16
U 1 1 618D76B1
P 6900 3350
F 0 "R16" H 6968 3396 50  0000 L CNN
F 1 "47K" H 6968 3305 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0516_L15.5mm_D5.0mm_P20.32mm_Horizontal" V 6940 3340 50  0001 C CNN
F 3 "~" H 6900 3350 50  0001 C CNN
	1    6900 3350
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 618D76B7
P 6900 3750
F 0 "#PWR0111" H 6900 3500 50  0001 C CNN
F 1 "GND" H 6905 3577 50  0000 C CNN
F 2 "" H 6900 3750 50  0001 C CNN
F 3 "" H 6900 3750 50  0001 C CNN
	1    6900 3750
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0112
U 1 1 618D76BD
P 6900 2300
F 0 "#PWR0112" H 6900 2150 50  0001 C CNN
F 1 "+5V" H 6915 2473 50  0000 C CNN
F 2 "" H 6900 2300 50  0001 C CNN
F 3 "" H 6900 2300 50  0001 C CNN
	1    6900 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 2300 6900 2450
Wire Wire Line
	6900 3500 6900 3750
Wire Wire Line
	6900 2750 6900 2950
Wire Wire Line
	6200 4100 7150 4100
Wire Wire Line
	6900 2950 7100 2950
Wire Wire Line
	7100 2950 7100 3900
Wire Wire Line
	7100 3900 7150 3900
Connection ~ 6900 2950
Wire Wire Line
	6900 2950 6900 3200
$Comp
L power:+5V #PWR0113
U 1 1 618DA3D9
P 7850 2300
F 0 "#PWR0113" H 7850 2150 50  0001 C CNN
F 1 "+5V" H 7865 2473 50  0000 C CNN
F 2 "" H 7850 2300 50  0001 C CNN
F 3 "" H 7850 2300 50  0001 C CNN
	1    7850 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 2300 7850 2400
Wire Wire Line
	7100 2950 7850 2950
Connection ~ 7100 2950
Connection ~ 7850 2950
Wire Wire Line
	7750 4000 8700 4000
Wire Wire Line
	8700 4000 8700 2950
Wire Wire Line
	8700 2950 8700 1000
Wire Wire Line
	8700 1000 3400 1000
Wire Wire Line
	3400 1000 3400 2000
Wire Wire Line
	3400 2000 3600 2000
Connection ~ 8700 2950
Wire Wire Line
	7100 2950 7100 2000
Wire Wire Line
	7100 2000 4100 2000
Text Notes 1200 1900 0    50   ~ 0
3 = latch output.  0V = latched, +5V = not latched\n5 = input voltage from thermistor board
Text Notes 3500 3100 0    50   ~ 0
trip level adjust\n
Text Notes 750  4900 0    50   ~ 0
Operation:\nAt low temperature, pin 5 is high.  U1A out is low, U1B out is high\nWhen temperature increases, pin 5 decreases.  Eventually, U1A goes high,\n U1B out goes low, pulling down C1 and U1B+in, \nlatching low until power reset.
Text Notes 950  7150 0    50   ~ 0
This board is connected to the thermal regulator board (re-used from an old solar hot \nwater heater controller).\n\nThe thermal regulator board supplies power and the voltage reading from the overtemperature sensor.\nWhen the overtempcutoff board latches low, it turns off the power relay on \nthe thermal regulator board, cutting off the pump.\n
$EndSCHEMATC