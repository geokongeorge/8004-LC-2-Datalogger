#ifndef MODBUSH_H
#define MODBUSH_H


typedef union{
    unsigned int c;                                                             //16 bit crc data
	unsigned char z[2];                                                         //(2) Bytes of 16bit crc data
}csum;



//REV H:
//MODBUS STATUS1 REGISTER:
typedef struct{                                                                 //   BIT          READ                              WRITE
	unsigned	_CFG:3;                                                          //   012    110   VW8                               VW8                       Lsb
                                                                                //          101   TH32                              TH32
                                                                                //          100   VW32                              VW32
                                                                                //          011   TH8                               TH8
                                                                                //          010   VW/TH Single                      VW/TH Single
                                                                                //          001   VW/TH16                           VW/TH16
                                                                                //          000   VW/TH4                            VW/TH4
    unsigned    _Setrtc:1;                                                       //   3                                        1=RTC Current Time Set REV CJ
	unsigned 	_Logint:1;                                                       //   4      1=Log Ints Enabled, 0=Disabled    1=Enable, 0=Disable
	unsigned 	_Wrap:1;                                                         //   5      1=Memory Wrap Enabled, 0=Disabled 1=Enable, 0=Disable
	unsigned	_BT:1;                                                           //   6      1=Bluetooth ON, 0=OFF             1=Turn BT ON, 0=Turn OFF
	unsigned	_BT_Timer:1;                                                     //   7      1=BT Timer Enabled, 0=Disabled    1=Enable BT Timer, 0=Disable
	unsigned	_OP:1;                                                           //   8      1=Output Port ON, 0=OFF           1=Turn ON, 0=Turn OFF
	unsigned	_OP_Timer:1;                                                     //   9      1=OP Timer Enabled, 0=Disabled    1=Enable OP Timer, 0=Disable
	unsigned	_Sync:1;                                                         //   10     1=Readings Sync'd, 0=not Sync'd   1=Sync Readings,0=Don't Sync
	unsigned 	_ST:1;                                                           //   11     1=Start Time Enabled,0=Disabled   1=Enable Start Time,0=Disable
    unsigned 	_SP:1;                                                           //   12     1=Stop Time Enabled,0=Disabled    1=Enable Stop Time,0=Disable    
    unsigned    _Readrtc:1;                                                      //   13                                       1=RTC Current Time Read
    unsigned    _X:1;                                                            //   14                                       1=Take single reading not stored                                  
	unsigned	_Logging:1;                                                      //   15     1=Started, 0=Stopped              1=Start, 0=Stop                 Msb    
}Status1ControlBits;
typedef union{ unsigned int status1;
Status1ControlBits status1flags;
}s1flags;
s1flags	S_1;


//REV CJ:
//MODBUS STATUS2 REGISTER:
typedef struct{                                                                 //   BIT          READ                              WRITE
	unsigned	_R:1;                                                            //   0              0                           1=Reset Memory Pointers           Lsb
    unsigned    _RST:1;                                                          //   1              0                           1=Reboot uC  
    unsigned    _CMD:1;                                                          //   2              0                           1=Reboot into command line interface
	unsigned 	_LD:1;                                                           //   3              0                           1=Load Defaults
	unsigned 	_CNV:1;                                                          //   4      1=Linear Conversion,0=Poly          1=Select Linear, 0=Select Poly
	unsigned	bit5:1;                                                          
	unsigned	bit6:1;                                                     
	unsigned	bit7:1;                                                           
	unsigned	bit8:1;                                                     
	unsigned	bit9:1;                                                         
	unsigned 	bit10:1;                                                           
    unsigned 	bit11:1;                                                              
    unsigned    bit12:1;                                                      
    unsigned    bit13:1;                                                                                             
	unsigned	bit14:1;
    unsigned    bit15:1;                                                        //                                                                                  Msb    
}Status2ControlBits;
typedef union{ unsigned int status2;
Status2ControlBits status2flags;
}s2flags;
s2flags	S_2;


extern unsigned int baudrate;
extern unsigned char MSB;                                                       
extern unsigned char LSB;                                                       
extern unsigned char MMSB;                                                      
extern unsigned char MMMSB; 
extern unsigned int TimeOut;                                                    //15 second timeout
extern void BT_D(void);                                                         //REV H
extern void BT_E(void);                                                         //REV H
extern void BTONE(void);                                                        //REV H
extern void BTZERO(void);                                                       //REV H
extern void CMD_LINE(void);                                                     //REV H
extern void DISLOGINT(void);                                                    //REV CI
extern void handleCOMError(void);
extern void ENLOGINT(void);                                                     //REV CI
extern void enableBT(void);                                                     //REV E
extern void disableBT(void);                                                    //REV E
extern void enableOP(void);                                                     //REV F
extern void disableOP(void);                                                    //REV F
extern void synch_one(void);                                                    //REV G
extern void synch_zero(void);                                                   //REV G
extern void wrap_zero(void);                                                    //REV F
extern void wrap_one(void);                                                     //REV F
extern void loadDefaults(void);                                                 //REV H
extern void MX1(void);                                                          //REV G
extern void MX4(void);                                                          //REV G
extern void MX8(void);                                                          //REV G
extern void MX8T(void);                                                         //REV G
extern void MX16(void);                                                         //REV G
extern void MX32(void);                                                         //REV G
extern void MX32T(void);                                                        //REV G
extern void O_D(void);                                                          //REV H
extern void O_E(void);                                                          //REV H
extern void R(void);                                                            //REV H
extern void READ_TIME(void);                                                    //REV H
extern void RST(void);                                                          //REV H
extern unsigned int START(void);                                                //REV H
extern unsigned int STOP(void);                                                 //REV H
extern void WRITE_TIME(void);                                                   //REV H
extern void X(void);                                                            //REV G


#define _RS485RX_EN LATFbits.LATF4                                              //REV H
#define RS485TX_EN  LATDbits.LATD15                                             //REV H

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
