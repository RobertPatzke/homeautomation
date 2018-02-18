# Over The Air update for WiFi modules
For testing two methods are implemented, ArduinoOTA and WebServerOTA. Select the method with one off
* #define smnArduinoOTA
* #define smnWebServerOTA

## Issue 1
Till now, programming of ESP8266 devices over the air does not work reliable.
It depends very much on the environment and hardware.

### With Sonoff 4CH (WebServerOTA):
1. Download of Programm seems OK, an accordingly message is displayed in Browser (see below). 
On serial Interface I see message stating a boot start. Then another from Watchdocg (wdt delay).
But rebooting does not really happen. After hard reset by interrupting the power supply the old software starts.

2. Repeat the Download with the same (new) bin-File. Same behaviour as with 1., but after hard reset, the new software starts.

Message on serial after rebooting

 ets Jan  8 2013,rst cause:1, boot mode:(3,7)

 load 0x4010f000, len 1384, room 16 
 tail 8
 chksum 0x2d
 csum 0x2d
 v3ffe903d
 @cp:0
 ld
 ) 

NOW I START HARDWARE RESET


3. Repeat the Download with a new version number of software (otherwise I cannot control the update). Now it worked perfectly. The message in the browser is as before:

Update Success! Rebooting...

The output on serial is exactly the same as above.

4. ok

5. ok

### With Sonoff Basic (WebServerOTA)

1. Program download successful (see brwoser message above). But device does not start. Serial message is:

 ets Jan  8 2013,rst cause:1, boot mode:(3,6)

 load 0x4010f000, len 1384, room 16 
 tail 8
 chksum 0x2d
 csum 0x2d
 v3ffe903d
 @cp:

NOW I START HARDWARE RESET

No reaction but very big traffic on serial with a strange bit rate (I cannot recognize anything).

Flashing via serial. OTA procedure seemed to have killed the EEPROM. 
Using a program now, which overwrites the EEPROM with configuration data on start.

2. Same as 1. Now serial message is:

 ets Jan  8 2013,rst cause:1, boot mode:(1,6)


 ets Jan  8 2013,rst cause:4, boot mode:(1,6)

 wdt reset

NOW I START HARDWARE RESET

Old software starts correct (so update did not happen.)

3. Same as 1. Serial message:

 ets Jan  8 2013,rst cause:1, boot mode:(3,6)

 load 0x4010f000, len 1384, room 16 
 tail 8
 chksum 0x2d
 csum 0x2d
 v3ffe903d
 @cp:

NOW I START HARDWARE RESET

Same reaction as with 1. So it does not work. Software download seems to be ok, but flashing does not work.

### Result

#### It is working with Sonoff 4CH with one mistake after flashing via serial. CPU module is PSF-A (ESP8285).
#### It is not working with Sonoff Basic. CPU module is ESP8266.

## Security aspects
There is no extra password for updating the firmware.
Security is achieved by the concept of Decentral Home Automation (DHA).
All devices run a state machine and have to enter an UPDATE STATUS before updating.
In UPDATE STATUS the communication for update is initialised (UpdateOTA.begin()).
Devices with local physical input (button, switch, etc.) have to be physically manipulated to enter UPDATE STATUS.

Devices without physical input may be driven into UPDATE STATE by other devices with physical input.
But this scenary is not defined till now. 
It has to be organized, that this procedure may not be driven by foreign devices.

