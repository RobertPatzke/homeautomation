# Device Configuration Concept
Device configuration has to parts. One part is managing content of a non-volatile memory, organized in pages of 256 Bytes. The next part is the definition of content.

## Managing Configuration Memory
Class ConfigMem handles all details of using non-volatile memory for configuration data. The memory handling is done via [EEPROM class of Arduino](https://www.arduino.cc/en/Reference/EEPROM). A TCP server is part of ConfigMem at typical devices and also a client is used on typical computers (smartphones, tablets and PCs).

It has to be mentioned, that configuration is not a matter of standard operation with Twitter/Follower communication. You should avoid to transfer data between devices via configuration memory. It is foreseen to have an extra maintenance manager at devices and configuration memory gets part of it.

## Content of Configuration Memory
This is just under discussion.

