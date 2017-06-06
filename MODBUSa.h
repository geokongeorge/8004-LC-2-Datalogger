#ifndef MODBUSA_H
#define MODBUSA_H


typedef union{
    unsigned int c;                                                             //16 bit data
	char z[2];                                                                  //(2) Bytes of 16bit data
}csum;

extern unsigned int baudrate;
extern unsigned char MSB;                                                       
extern unsigned char LSB;                                                       
extern unsigned char MMSB;                                                      
extern unsigned char MMMSB; 
extern unsigned int TimeOut;                                                    //15 second timeout
extern void handleCOMError(void);

//char MODBUSbuf[8];

#define TEST        true
#define GENERATE    false

//************************PROTOTYPES*************************************
void configMODBUStimers(void);
void MODBUScomm(void);
unsigned char MODBUS_RX(void);
unsigned int CRC(_Bool,unsigned char);                                                             

#endif
