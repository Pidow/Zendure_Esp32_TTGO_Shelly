# Zendure & Shelly to TTGO

![SCREENLOGO](https://github.com/Pidow/Envoy-ESP8266/blob/main/PIC/defaut.jpg?raw=true)
![SCREENLOGO](https://github.com/Pidow/Envoy-ESP8266/blob/main/PIC/Picture1.gif?raw=true)

TARGET:
Display Shelly & Zendure gateway information trough Wifi.
Track the solar production and electric consumption with Shelly Pro 3EM information
Follow the Zendure battery load trough the Zendure's MQTT message reception.


## VERSION HISTORY
*   NONE

## FIRST VERSION:

Overview:
*   TTGO : 20€ https://fr.aliexpress.com/item/33048962331.html 
*   Shelly pro 3EM: 120€ https://fr.zendure.com/products/shelly?variant=41867203084424
*   Battery pack with Hyper2000 https://fr.zendure.com/

## How it's work:
First, install TTGO parameter in arduino IDE.
Fill wifi SSID, Password; Zendure mail & Serial number.
You must generate your MQTT password by active line 75 (zendure();)
Upload software to TTGO, save information shown on screen.
Fill this information in MQTT parameter, disable line 75, and upload software to TTGO
I added the possibility to turn off the screen the night using var hourstart, hourstop.

The system is ready !

## Description

### PRODUCTION INFORMATION

### CONSUMPTION INFORMATION


### BATTERY INFORMATION

## 3D PRINT
CASE PROTECTION (in progress)



## Note:
No RTC needed, the system is connected to Network Time Protocol.

> I'm available if any question.
> 
> Have fun !
> 
>    ARNAUD TOCABENS.
