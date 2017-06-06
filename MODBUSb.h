#ifndef MODBUSB_H
#define MODBUSB_H


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

//Buffer Index Definitions:
#define ADDRESS         0
#define COMMAND         1
#define REGISTER_MSB    2
#define REGISTER_LSB    3
#define COUNT_MSB       4
#define COUNT_LSB       5

//Modbus Command Definitions:
#define READ_HOLDING    0x03
#define WRITE_HOLDING   0x06
#define WRITE_MULTIPLE  0x10


//************************PROTOTYPES*************************************
void configMODBUStimers(void);
void MODBUScomm(void);
unsigned char MODBUS_RX(void);
unsigned int CRC(_Bool,unsigned char);                                                             

#endif
