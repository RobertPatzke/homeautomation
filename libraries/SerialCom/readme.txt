15.01.2019
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

Examples follow ... Robert Patzke

