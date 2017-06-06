//#include <p33FJ256GP710A.h>                 
#include <i2c.h> 
#include "LC8004rtc_a.h"
#include "LC8004delay_a.h"
#include <uart.h>

extern unsigned char BCDones;
extern unsigned char BCDtens;
//extern cflags PORT_CONTROL;   8004 REV B NEED TO DEBUG
extern int	EEPROMTest;			
unsigned char BCDone;
unsigned char BCDten;
unsigned char BCDtwenty;
unsigned char BCDthirty;
unsigned char BCDforty;
unsigned char BCDfifty;
unsigned int decimalRTC;

#define RTCControlAddress		0x0E
#define RTCStatusAddress		0x0F

#define CONTROL_PORTflagsaddress	0x7Fada	//8004 REV B

//cflags	PORT_CONTROL;                                                           //declare instance of cflags    8004 REV B NEED TO DEBUG

//Functions

void displayClock(unsigned char data)
{
    BCDone=0;						//initialize
    BCDten=0;
    BCDtwenty=0;
    BCDthirty=0;
    BCDforty=0;
    BCDfifty=0;

    if(data>=0x50) BCDfifty=1;		//extract BCDfifty
    if(data>=0x40) BCDforty=1;		//extract BCDforty
    if(data>=0x30) BCDthirty=1;		//extract BCDthirty
    if(data>=0x20) BCDtwenty=1;		//extract BCDtwenty
    if(data>=0x10) BCDten=1;		//extract BCDten
    BCDten=BCDten+BCDtwenty+BCDthirty+BCDforty+BCDfifty;	//sum them in BCDten

    BCDten+=0x30;					//convert BCDten to ascii
    putcUART1(BCDten);				//display tens
    while(BusyUART1());

    BCDone=data&0x0F;				//mask off upper nybble of RTCdata
    BCDone+=0x30;					//convert ones to ascii
    putcUART1(BCDone);				//display ones
    while(BusyUART1());
}


void disableAlarm(unsigned char alarm)
{
    unsigned char temp;

    if(alarm==1)											//RTC Alarm 1 (Reading)
    {
        //PORT_CONTROL.flags.Alarm1Enabled=0;					//Clear the Alarm1 enabled flag
	//EEPROMTest=write_intEEPROM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in EEPROM
      //  if(EEPROMTest)                                            
        //    displayEEPROMError(EEPROMTest);
			
	//PORT_CONTROL.control=read_intEEPROM(CONTROL_PORTflagsaddress);	
	//if(!PORT_CONTROL.flags.Alarm2Enabled)						
      //      IEC1bits.INT2IE=0;								//disable the INT2 interrupt if not in use by Alarm2
			
	temp=readClock(RTCControlAddress);					//get the value in the RTC Control Register
	temp&=0xFE;											//AND it with 11111110 to set A1IE to 0
	setClock(RTCControlAddress,temp);					//write updated Control register to RTC
		
	temp=readClock(RTCStatusAddress);					//get the value in the RTC Status Register
	temp&=0xFE;											//AND it with 11111110 to clear A1F
	setClock(RTCStatusAddress,temp);					//write updated Status register to RTC	
	return;	
    }

    if(alarm==2)											//RTC Alarm 2 (Control Port)
    {
        //PORT_CONTROL.flags.Alarm2Enabled=0;					//Clear the Alarm2 enabled flag
	//EEPROMTest=write_intEEPROM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in EEPROM
        //if(EEPROMTest)                                            
          //  displayEEPROMError(EEPROMTest);
						
	//PORT_CONTROL.control=read_intEEPROM(CONTROL_PORTflagsaddress);	
	//if(!PORT_CONTROL.flags.Alarm1Enabled)						
      //      IEC1bits.INT2IE=0;								//disable the INT2 interrupt if not in use by Alarm1
	temp=readClock(RTCControlAddress);					//get the value in the RTC Control Register
	temp&=0xFD;											//AND it with 11111101 to set A2IE to 0
	setClock(RTCControlAddress,temp);					//write updated Control register to RTC
		
	temp=readClock(RTCStatusAddress);					//get the value in the RTC Status Register
	temp&=0xFD;											//AND it with 11111101 to clear A2F
	setClock(RTCStatusAddress,temp);					//write updated Status register to RTC		
    }

    setClock(0x0F,0);										//Clear the RTC Alarm flags
}


void enableAlarm(unsigned char alarm)
{
    unsigned char temp;
    //char tempBUF[10];
	
    if(alarm==1)											//RTC Alarm 1 (Reading)
    {
        setClock(0x0A,0x80);								//Alarm1 when hours,minutes & seconds match
	IEC1bits.INT1IE=1;									//enable the INT1 interrupt 8004 REV B
	//PORT_CONTROL.flags.Alarm1Enabled=1;					//Set the Alarm1 enabled flag
	//EEPROMTest=write_intEEPROM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in EEPROM
      //  if(EEPROMTest)                                            
        //    displayEEPROMError(EEPROMTest);
		
	temp=readClock(RTCControlAddress);					//get the value in the RTC Control Register
	temp|=0x05;											//OR it with 00000101 to set A1E=1,INTCN=1
	setClock(RTCControlAddress,temp);					//write updated Control register to RTC
		
	temp=readClock(RTCStatusAddress);					//get the value in the RTC Status Register
	temp&=0xFE;											//AND it with 11111110 to clear A1F
	setClock(RTCStatusAddress,temp);					//write updated Status register to RTC	
    }

    if(alarm==2)											//RTC Alarm 2 (Control Port)
    {
        setClock(0x0D,0x80);								//Alarm2 when hours & minutes match
	//IEC1bits.INT1IE=1;									//enable the INT1 interrupt 8004 REV B
	//PORT_CONTROL.flags.Alarm2Enabled=1;					//Set the Alarm2 enabled flag
	//write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in EEPROM
      //  if(EEPROMTest)                                            
        //    displayEEPROMError(EEPROMTest);
        temp=readClock(RTCControlAddress);					//get the value in the RTC Control Register
	temp|=0x06;								//OR it with 00000110 to set A2E=1,INTCN=1
	setClock(RTCControlAddress,temp);					//write updated Control register to RTC
	temp=readClock(RTCStatusAddress);					//get the value in the RTC Status Register
	temp&=0xFD;											//AND it with 11111101 to clear A2F
	setClock(RTCStatusAddress,temp);					//write updated Status register to RTC
    enableINT1(); //enable INT1 (respond to RTC interrupt) 
    }

    setClock(0x0F,0);									//Clear the RTC Alarm flags

}
//***********************************************************************************************************************


void setClock(unsigned char address,unsigned char data)                         //Transmit timekeeping data to the DS3231 via I2C1 interface
{
    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                            I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);

    unsigned int config2=0x10;							//d64 value for I2CBRG at 400KHz    REV A
    //unsigned int config2=0x07;                                                  //I2C1BRG value for 400KHz   REV D

    OpenI2C1(config1, config2);							//open the I2C1 module
    IdleI2C1();									//make sure bus is idle
    StartI2C1();
    while(I2C1CONbits.SEN);							//wait till start sequence is complete

    MasterWriteI2C1(0xD0);							//DS3231 RTC Slave address write byte
    while(I2C1STATbits.TBF);							//wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(2);												

    MasterWriteI2C1(address);							//select the appropriate DS3231 register
    while(I2C1STATbits.TBF);							//wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(2);												

    if(address!=RTCControlAddress && address!=RTCStatusAddress)                  //REV D
    {
        toBCD(data);								//convert to BCD
        data=0;									//clear the data

        switch(BCDtens)								//setup DS3231 register bits
        {
            case 5:
                data=0x50;
                break;
            case 4:
                data=0x40;
                break;
            case 3:
                data=0x30;
                break;
            case 2:
                data=0x20;
                break;
            case 1:
                data=0x10;
                break;
            default:
                break;
        }

        data+=BCDones;								//add the lower nybble
	
        if((address==0x0A)|(address==0x0D))						//Set A1M4/A2M4 in RTC address 0A by OR'ing data with 10000000	VER b
            data|=0x80;
    }

    MasterWriteI2C1(data);							//write the data to the DS3231
    while(I2C1STATbits.TBF);							//wait till data is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(2);												
	
    StopI2C1();
    while(I2C1CONbits.PEN);							//wait till stop sequence is complete

    CloseI2C1();									//close the I2C1 module

}

unsigned char readClock(unsigned char address)				//Receive timekeeping data from the DS3231 via I2C1 interface
{
    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                            I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);

    unsigned int config2=0x10;							//d64 value for I2CBRG at 400KHz    REV A
    //unsigned int config2=0x07;                                                  //I2C1BRG value for 400KHz   REV D
    unsigned char data;								//data read from DS3231

    OpenI2C1(config1, config2);								//open the I2C1 module
    IdleI2C1();												//make sure bus is idle
    StartI2C1();
    while(I2C1CONbits.SEN);									//wait till start sequence is complete

    delay(2);												//VER b

    MasterWriteI2C1(0xD0);									//DS3231 RTC Slave address write byte
    while(I2C1STATbits.TBF);									//wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(2);												//VER b

    MasterWriteI2C1(address);								//select the appropriate DS3231 register
    while(I2C1STATbits.TBF);									//wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(2);												//VER b

    StopI2C1();

    delay(2);												//VER b

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    StartI2C1();
    while(I2C1CONbits.SEN);									//wait till start sequence is complete

    delay(2);												//VER b

    MasterWriteI2C1(0xD1);									//DS3231 RTC Slave address read byte
    while(I2C1STATbits.TBF);									//wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    delay(2);												//VER b

    data = MasterReadI2C1();									//read the data from the DS3231
    while(I2C1STATbits.TBF);									//wait till data is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    delay(2);												//VER b

    NotAckI2C1();											//NACK the DS3231

    delay(2);												//VER b

    StopI2C1();
    while(I2C1CONbits.PEN);									//wait till stop sequence is complete


    CloseI2C1();												//close the I2C1 module

    return data;											//return the integer DS3231 data
}


unsigned int RTCtoDecimal(unsigned char data)
{
    BCDone=0;												//initialize
    BCDten=0;
    BCDtwenty=0;
    BCDthirty=0;
    BCDforty=0;
    BCDfifty=0;

    if(data>=0x50) BCDfifty=1;		//extract BCDfifty
    if(data>=0x40) BCDforty=1;		//extract BCDforty
    if(data>=0x30) BCDthirty=1;		//extract BCDthirty
    if(data>=0x20) BCDtwenty=1;		//extract BCDtwenty
    if(data>=0x10) BCDten=1;		//extract BCDten
    BCDten=BCDten+BCDtwenty+BCDthirty+BCDforty+BCDfifty;	//sum them in BCDten

    BCDten*=10;						//multiply BCDten by ten
    BCDone=data&0x0F;				//mask off upper nybble of RTCdata
	
    decimalRTC=BCDten+BCDone;		//add tens and ones
    return decimalRTC;				//return value
}

/*
unsigned char Start1HzSQW(void)
{
    unsigned char temp;

    temp=readClock(RTCControlAddress);						//save original value in the Control Register to temp
    setClock(RTCControlAddress,0);							//write 0 to the Control Register to start 1Hz output
    return temp;
}
*/

void start32KHz(void)
{
    unsigned char temp;

    temp=readClock(RTCStatusAddress);						//read the value of the status register
    temp=(temp|0x08);										//OR it with 0x08 to set the EN32kHz bit
    setClock(RTCStatusAddress,temp);						//load it to the RTC
}

/*
void Stop1HzSQW(unsigned char value)
{
    setClock(RTCControlAddress,value);						//restore the Control Register to its previous state
}
*/

void stop32KHz(void)
{
    unsigned char temp;

    temp=readClock(RTCStatusAddress);						//read the value of the status register
    temp=(temp&&0xFE);										//AND it with 0xFE to clear the EN32kHz bit
    setClock(RTCStatusAddress,temp);						//load it to the RTC
}
