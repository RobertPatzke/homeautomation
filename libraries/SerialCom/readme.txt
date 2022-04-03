This interrupt handled serial communication for the Arduino DUE was developed
at a time, where the Arduino libraries did not transmit with interrupts
but with a polling in main() after calling loop().
Meanwhile (> 2014) there is also interrupt handling with serial transmit.
But the ring buffers used are only 128 bytes large.
So a reason for using the class SerialCom could be the need for buffers of
different size, specific to UART0-UART3 and transmit or receive.

15.01.2019 (latest version)
This Library provides Objects (Class) for using UART and USARTs of SAM3x.
It was developed at MFP GmbH in Wunstorf/Germany.
You have to exchange the file 
- variant.cpp
- in packages/arduino/hardware/sam/1.6.11/variants
- by the file
- variant.cpp
- here in subfolder locCore
and you have to define 
#define useVariantMFP
to create your own Interrupt-Handlers.


