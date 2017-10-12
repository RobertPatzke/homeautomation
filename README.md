# Decentral homeautomation
Decentral homeautomation deals with autonomous smart devices (agents) for controlling technical equipment at your home. 
It is creating a kind of perfect butler, who helps anybody, particularly elder and handicaped people, 
with their daily tasks at home. 

This repository will contain software for the development of autonomous smart devices. Based on the software concept of [Arduino](https://www.arduino.cc/), we started with tools for a time control of the loop function with [LoopCheck](https://github.com/RobertPatzke/homeautomation/tree/developer/libraries/LoopCheck). This was followed by a software for sending (Twitter) and receiving (Follower) broadcast messages based on an API for the network with [SocManNet](https://github.com/RobertPatzke/homeautomation/tree/developer/libraries/SocManNet). At present, this is tested with Arduino Due (Ethernet shield) and ESP32, ESP8266 is under test. One goal for the future is to provide broadcast capabilities with other networks (e.g. ZigBee) in SocManNet.

Soon will follow the software for finite state machines as the foundation of smart devices.

# Using GitHub
I decided to use Git and GitHub because of the filter effect on uploaded software. 
So my students or any other interested programmer may improve (hopefully) the software 
and I can check the changes before they go to the repository.
Knowing my own weakness with quick and dirty programming I created at once a second branch and named it {developer} 
to start with content there. 
I know, that with Git it would not be necessary, but it is easier for me to publish some code 
which may cause criticism when there is an implicit promise to check/improve it 
before it is published in the master repository.

# Current development tools and targets
At present I use the Arduino library environment for targets Arduino Due and development boards 
for the WiFi-SoCs ESP8266 and [ESP32](https://github.com/espressif/arduino-esp32). 
For the software development I use [Eclipse/Sloeber](http://eclipse.baeyens.it/) on Ubuntu (and sometimes also on Windows). 
One of my students created a bootable USB-Stick with Linux MINT and Sloeber, 
which will be a give-away for students joining the microprocessor laboratory at the University of 
Applied Sciences and Arts in Hannover/Germany. 
My hope is, that these students will have fun with programming and do that also at home.

For the content (source files) of the projects in my Eclipse workspace I use Linux links (not the source code linking of the Eclipse environment). This is because of having a simple #include structure of the header files, as it is with the Arduino libraries, in different development environments. E.g. the software is also tested with typical console applications in C/C++ with Eclipse on Ubuntu without copying files.
