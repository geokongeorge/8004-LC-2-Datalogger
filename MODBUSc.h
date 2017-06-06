#ifndef MODBUSC_H
#define MODBUSC_H


typedef union{
    unsigned int c;                                                             //16 bit crc data
	unsigned char z[2];                                                         //(2) Bytes of 16bit crc data
}csum;


extern unsigned int baudrate;
extern unsigned char MSB;                                                       
extern unsigned char LSB;                                                       
extern unsigned char MMSB;                                                      
extern unsigned char MMMSB; 
extern unsigned int TimeOut;                                                    //15 second timeout
extern void handleCOMError(void);



#define TEST        true
#define GENERATE    false

//Buffer Index Definitions:
#define ADDRESS         0                                                       //MODBUS drop address
#define COMMAND         1                                                       //Function command
#define BYTE_COUNT      2                                                       //# of data bytes being transmitted
#define REGISTER_MSB    2                                                       //Register Starting address MSB
#define REGISTER_LSB    3                                                       //Register Starting address LSB
#define COUNT_MSB       4                                                       //Number of Registers MSB
#define COUNT_LSB       5                                                       //Number of Registers LSB
#define WRITE_DATA_MSB  4                                                       //MSB of write data
#define WRITE_DATA_LSB  5                                                       //LSB of write data

//Modbus Command Definitions:
#define READ_HOLDING    0x03
#define WRITE_HOLDING   0x06
#define WRITE_MULTIPLE  0x10


//************************PROTOTYPES*************************************
void configMODBUStimers(void);
void MODBUScomm(void);
unsigned char MODBUS_RX(void);
void MODBUS_TX(void);
unsigned int CRC(_Bool,unsigned char);                                                             

#endif
