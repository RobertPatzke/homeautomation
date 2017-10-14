# Source-code libraries
The libraries presented here are oriented at the structure as it is used with the [Arduino](https://www.arduino.cc/) environment. 
But at present there are no files like kewords.txt and readme.txt as with Arduino. 
Because we do not use the Arduino IDE for development but Eclipse/Sloeber. 
Any extra keywords would be defined in that environment.

Some library folders (e.g. SocManNet) contain more than one module 
(Twitter, Follower and SocManNet are placed in the same folder SocManNet).
This works perfectly with the IDE and gives us a better understandable organisation 
structure than that with an extra folder for each module.

# Examples
In the library folders there are subfolders named examplesArduino with typical Arduino sketches 
(.ino files with setup() and loop()) showing the usage of the library modules).

We also started a simulation of the Arduino environment on Linux integrated in the library modules 
and controlled by switches defined at the IDE and in 
[environment.h](https://github.com/RobertPatzke/homeautomation/blob/master/libraries/environment/environment.h).
So there will also be examples to use the libraries with typical console applications on Linux placed in subfolder examplesLinux 
(e.g. [testCppArduino](https://github.com/RobertPatzke/homeautomation/blob/master/libraries/LoopCheck/examplesLinux/testCppArduino.cpp). 
This was made for a quick testing (and debugging) of new software before it is downloaded to the target boards.
