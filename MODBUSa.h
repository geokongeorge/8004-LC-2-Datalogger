#ifndef MODBUSA_H
#define MODBUSA_H


#define TEST        true
#define GENERATE    false

//************************PROTOTYPES*************************************
void configMODBUStimers(void);
void MODBUScomm(void);
unsigned char MODBUS_RX(void);
unsigned int CRC(_Bool,unsigned char);                                                             

#endif
