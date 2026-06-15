# ESP32 Home automation
## Description 
- This is a ESP32 S3 Based Home Automation devices which has 6 Relay switches that can be controlled over the web 
- It also has a BME280 Sensor for monitoring temprature,humidity and pressure in the realtime and can be monitored using a mobile app or a Webpage.
- Also had a FTDI header for easy programming
- Along with web control there is provison to add any physical switches and the case has space to add touch switches too
## Parts Used
- ESP32 S3
- BME280 Sensor
- 6 relays
- Hi-Link 20w 5v AC-DC converter
## Usage 
it is very simple to use just need to plugin the FTDI programmer and Upload the code after adding your SSID nad password so that it will work and on you browser open 192.168.1.100 ive set this as the static IP for accessing the webpage running on the ESP

## PCB Schematic and CAD 

<img width="757" height="824" alt="Screenshot 2026-04-12 232416" src="https://github.com/user-attachments/assets/fd8f8eca-e45e-47ea-933a-2b3f7366066d" />
<img width="905" height="753" alt="Screenshot 2026-04-13 002313" src="https://github.com/user-attachments/assets/09a5203a-bd17-4c73-a73d-5d173c71b695" />
<img width="742" height="810" alt="Screenshot 2026-04-12 233122" src="https://github.com/user-attachments/assets/8d473fcf-b93b-499e-909e-81737ae9b98a" />
## BUILD 
<img width="1200" height="1599" alt="WhatsApp Image 2026-06-16 at 12 44 00 AM (1)" src="https://github.com/user-attachments/assets/52e580c6-606e-43ba-8ad5-0d690d7ced0d" />
<img width="1599" height="1200" alt="WhatsApp Image 2026-06-16 at 12 43 59 AM" src="https://github.com/user-attachments/assets/af62433a-4f95-4ea6-a36f-f895172df01a" />
<img width="1599" height="1200" alt="WhatsApp Image 2026-06-16 at 12 43 42 AM" src="https://github.com/user-attachments/assets/74dd052b-612c-4fd6-ac33-62a8a549e4fc" />
<img width="1200" height="1599" alt="WhatsApp Image 2026-06-16 at 12 43 41 AM" src="https://github.com/user-attachments/assets/a1620d22-9116-4ec6-8021-fc245318fdd3" />
<img width="1599" height="1200" alt="WhatsApp Image 2026-06-16 at 12 44 00 AM" src="https://github.com/user-attachments/assets/ecced0a3-61f1-4656-aa7f-15bcbc5635c1" />
<img width="1599" height="1200" alt="WhatsApp Image 2026-06-16 at 12 44 00 AM (2)" src="https://github.com/user-attachments/assets/f1ea025b-6b6e-411b-9ab9-e29c73ef0ef7" />
<img width="957" height="666" alt="image" src="https://github.com/user-attachments/assets/037b5820-53e3-4e4e-b51e-0183943786cf" />

demo link https://drive.google.com/file/d/14zfI7igBqzAfgmiqQ8RciHNbKA9al0T5/view?usp=sharing

## BOM 
| Name                     | Purpose                     | Quantity | Total Cost (USD) | Link                                                                 | Distributor |
|--------------------------|----------------------------|----------|------------------|----------------------------------------------------------------------|-------------|
| Passive Components       | Ressitor and ETC           | 1        | 3.00             |                                                                      | LCSC        |
| BWGMRJWX50-10WJ          | Antenna Stem               | 1        | 0.94             |                                                                      | LCSC        |
| BWIPX1-SMA-1.13L100      | Antenna                    | 1        | 0.94             |                                                                      | LCSC        |
| MIC5219-3.3YM5-TD        | 3.3v Regulator             | 1        | 1.23             | https://www.lcsc.com/product-detail/C42422052.html                  | LCSC        |
| SMBJ5.0A                 | TVS diode                  | 1        | 0.50             | https://www.lcsc.com/product-detail/C83333.html                     | LCSC        |
| DB128V-5.0-2P-BK-S       | Terminal Block 2P          | 1        | 0.80             | https://www.lcsc.com/product-detail/C424692.html                    | LCSC        |
| 1N4007 M7                | Diode                      | 1        | 0.64             | https://www.lcsc.com/product-detail/C20754788.html                  | LCSC        |
| TS665CJ                  | Switch                     | 1        | 0.83             | https://www.lcsc.com/product-detail/C393938.html                    | LCSC        |
| DB128V-5.08-3P-GN-S      | Terminal Block 3P          | 1        | 1.45             | https://www.lcsc.com/product-detail/C2915640.html                   | LCSC        |
| MOV14D471K206RV080       | its a MOV for protection   | 1        | 0.52             | https://www.lcsc.com/product-detail/C22390366.html                  | LCSC        |
| ESP32-S3-WROOM-1-N4      | the brains                 | 1        | 4.64             | https://www.lcsc.com/product-detail/C2913197.html                   | LCSC        |
| BME280                   | Sensor                     | 1        | 7.24             | https://www.lcsc.com/product-detail/C92489.html                     | LCSC        |
| Relay 5v                 | to switch                  | 6        | 2.88             | https://www.lcsc.com/product-detail/C35449.html                     | LCSC        |
| PC817C                   | optocoupler                | 1        | 0.55             | https://www.lcsc.com/product-detail/C20612594.html                  | LCSC        |
| HLK-20M05                | onboard 5v                 | 1        | 4.58             | https://www.lcsc.com/product-detail/C465406.html                    | LCSC        |
