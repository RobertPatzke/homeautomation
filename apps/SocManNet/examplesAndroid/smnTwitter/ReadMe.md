# Example for using Twitter class

The task of Twitter is to send cyclic broadcast messages with a fixed schedule.
You can choose 3 different speeds, Speed.low (a message every 10 seconds), Speed.normal 
(a message every second) and Speed.high (10 messages in a second.) 
After You have initialised Twitter with selecting speed and number of elements, 
You simply fill elements with Your latest information. You do not care about sending broadcast messages. 

Twitter has basic communicated elements. The telegram structure is explained 
[here (in German language)](http://homeautomation.x-api.de/wikidha/index.php?title=Telegramm).
You cannot change the structure of the basic elements without changing the program code of Twitter class.
But the content of most elements may be set because they are global attributes of Twitter class.

You can define Your own broadcast elements with the initialisation of Twitter class. 
You set the number of integer, float and text values you want to transmit. 
Currently the number of elements is limited to 4 for each variable type. 
The reason is the small memory of many microcontrollers (e.g. 8 Bit Arduinos).
If You want to use more elements, You will have to change some parameters in Twitter class.
