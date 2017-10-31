# Social Manufacturing Network
The naming Social Manufacturing Network (SMN) comes from the basic technology of using broadcast communication for data exchange between devices.
With using broadcast, any information sent by one device may be received by any other device or all devices.
This reminds to the social network [Twitter](https://en.wikipedia.org/wiki/Twitter), where tweets (short messages) are posted once and anybody can read them. If he is a Follower, he is informed about a new tweet from the Twitter, which he is following.
But with using physical broadcast, there is no server, as it is with Twitter. The messages are directly spread over the whole network and any device which opens its ears (filters the tweet) receives them.
The link to manufacturing comes from the original environment for using the broadcasting. The network was created by company [MFP](http://www.mfp-portal.de) in 2010 with the development of an agent based Manufacturing Execution System (AMES) within the government funded research project [AGILITA](http://www.agilita-projekt.de).

It is important to understand, that the communication is not device oriented but information oriented. Twitters and Followers (see below) send and receive named objects describing physical values and application states. In most cases, the relevant information is linked to a physical device. But in many cases, e.g. for the measurement of outside temperature, there are different locations with the same objects and a Follower would have to distiguish between locations (see below). Of course, it would be possible to create new object names for the different locations, but than we would need a Follower instance for each location. The idea of handling process information instead of device information would be lost.

# Class SocManNet
The C++ class SocManNet is an API to the network for sending and receiving broadcast messages. The different physical interfaces are integrated via macros (#define). It depends on the CPU and the environment, which you are using. Your environment may be defined in file [environment.h](https://github.com/RobertPatzke/homeautomation/blob/developer/libraries/environment/environment.h) and the parameters for the network are expected in file [socManNetUser.h](https://github.com/RobertPatzke/homeautomation/blob/developer/libraries/environment/socManNetUser.h). At present SMN is used with
* Arduino Due with Ethernet shield for typical LAN
* ESP8266 for WiFi (Arduino libraries)
* ESP32 for WiFi and Bluetooth (Arduino libraries)

# Class Twitter
The C++ class Twitter creates named messages from user data (a mix of integers, floats and texts) and periodically hands them over to class SocManNet for sending the broaadcast. At present, there are three repetition speeds:
* lowSpeed    = one broadcast message every 10 seconds
* normalSpeed = one broadcast message every second
* highSpeed   = 10 broadcast messages in one second

The message is altered with a change of user data (new values for int, float or text). From users view, Twitter runs in background.
For decentral homeautomation typically device state and observation content is sent via Twitter.

For each new name of a message and a corresponding set of user data a new instance of Twitter has to be created. At present, the number of Twitter objects is limited to 4 by a precompiler defenition in Twitter.h (change it, if you need more twitters and your resources of RAM and Flash allow it).

# Class Follower
The C++ class Follower receives messages from class SocManNet and filters them by name. Messages are parsed and the corresponding user data sets are filled with content. The user is informed with the booleans newPDU (a new message came in) and newValue (the content of a user variable changed) about the state of his data.

For each name of a message and a corresponding set of user data a new instance of Follower has to be created. At present, the number of Follower objects is limited to 4 in Follower.h (change if needed).

# Class FollowMultDev
As mentioned above, there exist information objects (e.g. with name INFO1) presenting the same physical environment but with many devices of the same kind at different locations. Therefore we have a lot of Twitters sending INFO1. Although there is enough meta information (device name, device key, positions X/Y/Z, etc.) to distinguish between the different sources of INFO1, it may happen, that our Follower is not fast enough to handle two messages sent at (nearly) the same time. For this situation the class FollowMultDev was developed, which stores incoming objects of the same name in different locations identified by meta information *device key*. Your application may use an index (0, 1, ...) to access INFO1 form different devices.
