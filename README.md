# Decentral homeautomation
Decentral homeautomation deals with autonomous smart devices (agents) for controlling technical equipment at your home. 
It is creating a kind of perfect butler, who helps anybody, particularly elder and handicaped people, 
with their daily tasks at home. 

This repository contains software for the development of autonomous smart devices. Software in folder *libraries* is based on the concept of [Arduino](https://www.arduino.cc/) and we started with tools for a time control of the loop function with [LoopCheck](https://github.com/RobertPatzke/homeautomation/tree/developer/libraries/LoopCheck). This was followed by a software for sending (Twitter) and receiving (Follower) broadcast messages based on an API for the network with [SocManNet](https://github.com/RobertPatzke/homeautomation/tree/developer/libraries/SocManNet). At present, this is tested with Arduino Due (Ethernet shield) ESP32 and ESP8266. One goal for the future is to provide broadcast capabilities with other networks in SocManNet. In March 21 we started with Measurement-Beacons on Bluetooth Low Energy.

Software in folder *apps* is that for Android devices, written in Java. At present, there is no fixed concept for the structure of that part of the repository (see notes to Android development below). So far, the structure is related to that of folder *libraries*. Because we want to avoid console applications for Android devices, there is no relation between the examples of Arduino and Android.

Folder *devices* contains software for finite state machines in *baseDevice* as the foundation of smart devices.

# Using GitHub
I decided to use Git and GitHub because of the filter effect on uploaded software. 
So my students or any other interested programmer may improve (hopefully) the software 
and I can check the changes before they go to the main repository.
Knowing my own weakness with quick and dirty programming I created at once a second branch and named it *developer* 
to start with content there. 
I know, that with Git it would not be necessary, but it is easier for me to publish some code 
which may cause criticism when there is an implicit promise to check/improve it 
before it is published in the master repository.

# Current development tools and targets
## Arduino (C++)
At present I use the Arduino library environment for targets Arduino Due and development boards 
for the WiFi-SoCs ESP8266 and [ESP32](https://github.com/espressif/arduino-esp32). 
For the software development I use [Eclipse/Sloeber](http://eclipse.baeyens.it/) on Ubuntu (and sometimes also on Windows). 
One of my students created a bootable USB-Stick with Linux MINT and Sloeber, 
which will be a give-away for students joining the microprocessor laboratory at the University of 
Applied Sciences and Arts in Hannover/Germany. 
My hope is, that these students will have fun with programming and do that also at home.

For the content (source files) of the projects in my Eclipse workspace I use Linux links (not the source code linking of the Eclipse environment). This is because of having a simple #include structure of the header files, as it is with the Arduino libraries, in different development environments. E.g. the software is also tested with typical console applications in C/C++ with Eclipse on Ubuntu without copying files.

## Android (Java)
I started APP development also with Eclipse and a plug-in for Android, available from Google. But meanwhile the Android Studio from Google is more easy to handle and that ist the main request when students start to develop APPs the first time in their life. For me, the structure of projects is an issue, because I do not want to force students to use special package names. So I decided not to store projects or project information in the repository. Instead I use symbolic links from my git repository to the project environment. That also works with Windows (tested with 7) by using the command tool mklink. Of course it does not work with links made with windows explorer in graphical environment, because that are not direct links but extra files with extension .lnk which cannot be used by the IDE.
