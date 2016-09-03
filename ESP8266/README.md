# WIFI
Everything is working and can be used, it should be stable at this point.

Notes:
Be careful with uart1, i'm using modified variants of the uart1 functions in the .c file. 
So changes made in uart1.c and uart1.h have no effect on these files!

Important:
The uart1 speed was upped to 11520 Baud, which you can see in ESP8266.h.
The module has to be manually reconfigured with the command which is commented next to the BAUD_RATE constant
in the same file or otherwise it WILL NOT WORK!