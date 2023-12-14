# GPS-based environmental sensor data logger

### Team members

* Bohdan Tsariuchenko (responsible for .... documentation)
* Migulia Maksim (responsible for .., elaboration of schemes, documentation)
* Yaroslav (responsible for ...)


## GitHub repository structure

   ```c
PROJECT        // PlatfomIO project
├── include
│  └── timer.h      
├── lib             // Libraries
│  └── gpio
│       └── gpio.c
│       └── gpio.h
│   └── oled
│        └── font.h
│        └── oled.c
│        └── oled.h
│   └── twi
│        └── twi.c
│        └── twi.h
│   └── uart
│        └── uart.c
│        └── uart.h
├── src             // Source file(s)
│   └── main.c
└── platformio.ini  // Project Configuration File
 ```

### Table of contents

* [Projekt](#objectives)
* [Popis HW](#hardware)
* [Video](#video)
* [Zdroje](#references)

<a name="objectives"></a>
## Project
Create a comprehensive data logging system using an AVR microcontroller. The system integrates GPS functionality for location tracking and an I2C environment sensor to capture data related to environmental conditions. The project aims to log and display sensor data and provide the capability to export the collected information for analysis.

## Project objectives:

1. Create a program capable of reading: 
- GPS data
- Humidity and temperature data
2. The acquired data is displayed in real time on the OLED display.
3. Export GPS, humidity and temperature data to txt or exel file


<a name="hardware"></a>
## Description HW
### Schematic wiring diagram
![Max_DE2](https://github.com/MaksimMigulia97/digital_electronics-2/assets/99403646/05c29027-3b62-4b4a-b1e0-ba9bfbfa2d46)

### [1] Arduino UNO, ATmega328P:
- CPU frequency up to 20MHz
- FLASH 32KB
- EEPROM 1KB
- SRAM 2KB
- 14x digital I/O
- 1x 10-bit ADC 6 channels
- 6 PWM channels
- Power supply 1.8 - 5.5V

### [2] Modul DHT11:
- Form of output: digital output
- Humidity measurement range: 20%~90% RH
- Humidity measurement error: ± 5% RH
- Temperature measurement range: 0~60℃
- Temperature measurement error: ±2℃
- Operating voltage: 5V
- Size: 28 x 12 x 8 mm

### [3] OLED Display:
- Bus: IIC I2C
- Diagonal: 0.96" 
- Resolution: 128 x 64px
- Color: Blue
- Power: 40mW 
- Power: 3.3 - 5V DC 
- Operating Temperature: -30 to +80°C 
- Dimensions: 27 x 27 x 4.1 mm 
- Driver: SSD1306

### [4] Modul GPS:
- Type: NEO-6M GYNEO6MV2[1]: 
- Interface: RS232 TTL
- Default speed: 9600bps
- Power supply: 3.3V to 5V
- Dimensions: 36 x 24 mm

<a name="modules"></a>

## Flowcharts
![image](https://github.com/Xtsari00/notReal-Digital-Electronics-2/assets/99403641/7bd48cb2-cc2b-4851-829f-f80f9d2067c6)

## Signal analisys 
![1](https://github.com/Xtsari00/notReal-Digital-Electronics-2/assets/99403641/30282f5b-707b-4bbf-890b-a5edb8bb6f31)
![image](https://github.com/Xtsari00/notReal-Digital-Electronics-2/assets/99403641/f3e1faac-cd07-495f-bef1-c60f92e3e0b9)


## Testing and results
![image](https://github.com/Xtsari00/notReal-Digital-Electronics-2/assets/99403641/02b0ff3f-9a03-41ff-93a2-cb791b5dc97a)

![image](https://github.com/Xtsari00/notReal-Digital-Electronics-2/assets/99403641/94a315ad-299b-42d3-b931-5311ffa84938)

![photo1702498282](https://github.com/Xtsari00/notReal-Digital-Electronics-2/assets/99403641/68fed1d6-c474-4499-b25e-26a2e23109e2)



## References 
https://youtu.be/xz-EPdDKu1A
https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
