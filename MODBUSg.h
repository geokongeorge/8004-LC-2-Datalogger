#ifndef MODBUSG_H
#define MODBUSG_H


typedef union{
    unsigned int c;                                                             //16 bit crc data
	unsigned char z[2];                                                         //(2) Bytes of 16bit crc data
}csum;



//MODBUS STATUS REGISTER:
typedef struct{                                                                 //      READ                              WRITE
	unsigned	bit0:1;                                                         //1=Started, 0=Stopped              1=Start, 0=Stop                  Lsb
	unsigned 	bit1:1;                                                         //1=Log Ints Enabled, 0=Disabled    1=Enable, 0=Disable
	unsigned 	bit2:1;                                                         //1=Memory Wrap Enabled, 0=Disabled 1=Enable, 0=Disable
	unsigned	bit3:1;                                                         //1=Bluetooth ON, 0=OFF             1=Turn BT ON, 0=Turn OFF
	unsigned	bit4:1;                                                         //1=BT Timer Enabled, 0=Disabled    1=Enable BT Timer, 0=Disable
	unsigned	bit5:1;                                                         //1=Control Port ON, 0=OFF          1=Turn ON, 0=Turn OFF
	unsigned	bit6:1;                                                         //1=CP Timer Enabled, 0=Disabled    1=Enable CP Timer, 0=Disable
	unsigned	bit7:1;                                                         //1=Readings Sync'd, 0=not Sync'd   1=Sync Readings,0=Don't Sync
	unsigned	bit8910:3;                                                      //110   VW8 		
                                                                                //101   TH32
                                                                                //100   VW32
                                                                                //011   TH8
                                                                                //010   VW/TH Single
                                                                                //001   VW/TH16
                                                                                //000   VW/TH4
	unsigned 	bit11:1;                                                        //1=Start Time Enabled,0=Disabled   1=Enable Start Time,0=Disable
    unsigned 	bit12:1;                                                        //1=Stop Time Enabled,0=Disabled    1=Enable Stop Time,0=Disable    Msb
    unsigned    bit13:1;                                                        //'X' command   1=Take Reading & store in X memory
}StatusBits;
typedef union{ unsigned int status;
StatusBits statusflags;
}bitflags;


extern unsigned int baudrate;
extern unsigned char MSB;                                                       
extern unsigned char LSB;                                                       
extern unsigned char MMSB;                                                      
extern unsigned char MMMSB; 
extern unsigned int TimeOut;                                                    //15 second timeout
extern void handleCOMError(void);
extern void enableBT(void);                                                     //REV E
extern void disableBT(void);                                                    //REV E
extern void enableOP(void);                                                     //REV F
extern void disableOP(void);                                                    //REV F
extern void synch_one(void);                                                    //REV G
extern void synch_zero(void);                                                   //REV G
extern void wrap_zero(void);                                                    //REV F
extern void wrap_one(void);                                                     //REV F
extern void MX1(void);                                                          //REV G
extern void MX4(void);                                                          //REV G
extern void MX8(void);                                                          //REV G
extern void MX8T(void);                                                         //REV G
extern void MX16(void);                                                         //REV G
extern void MX32(void);                                                         //REV G
extern void MX32T(void);                                                        //REV G
extern void X(void);                                                            //REV G

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
void MODBUS_TX(unsigned int);                                                   //REV D
unsigned int CRC(_Bool,unsigned char);                                                             

#endif
