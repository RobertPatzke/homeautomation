# Social Manufacturing Network
The naming Social Manufacturing Network (SMN) comes from the basic technology of using broadcast communication for data exchange between devices.
With using broadcast, any information sent by one device may be received by any other device or all devices.
This reminds to the social network [Twitter](https://en.wikipedia.org/wiki/Twitter), where tweets (short messages) are posted once and anybody can read them. If he is a Follower, he is informed about a new tweet from the Twitter, which he is following.
But with using physical broadcast, there is no server, as it is with Twitter. The messages are directly spread over the whole network and any device which opens its ears (filters the tweet) receives them.
The link to manufacturing comes from the original environment for using the broadcasting. The network was created by company [MFP](http://www.mfp-portal.de) in 2010 with the development of an agent based Manufacturing Execution System (AMES) within the government funded research project [AGILITA](http://www.agilita-projekt.de).

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

For each new name of a message and a corresponding set of user data a new instance of Twitter has to be created. At present, the number of Twitter objects is limited to 4.

# Class Follower
The C++ class Follower receives messages from class SocManNet and filters them by name. Messages are parsed and the corresponding user data sets are filled with content. The user is informed with the booleans newPDU (a new message came in) and newValue (the content of a user variable changed) about the state of his data.

For each name of a message and a corresponding set of user data a new instance of Follower has to be created. At present, the number of Follower objects is limited to 4.
