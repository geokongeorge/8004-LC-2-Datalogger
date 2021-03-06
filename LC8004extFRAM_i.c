//****************************LC8004extFRAM_i.c**********************************
//
//			FM24V10 FRAM functions
//
//***********************************************************************
//REV C: 12/2/16    Make sure all addresses are type unsigned long
//                  Reduce bit rate to 800KHz from 1MHz
//REV D  12/13/16   Include memory refresh after every read
//REV E  12/19/16   Limit # of readings/memory page to prevent page overrun errors
//                  Write to FRAM at 400KHz and read at 925KHz
//REV F  12/29/16   Debug SleepFRAM()
//REV G  01/04/17   Poll for FRAM ACK after each write to base address
//REV H  01/26/17   Adjust I2C baud rate generator value for Fcy=29.4912MHz (8004 rev ae)   
//REV I  07/28/17   Swap byte addresses in read_longFRAM() and write_longFRAM() so byte order in FRAM
//                  will be MSB - LSB instead of LSB - MSB
//                  Change all instances of long int to unsigned long int
//                  Change Reference designators in FRAM TEST to correspond with PCB-238 rev A board layout


#include "LC8004extFRAM_i.h"                                                    //external FRAM function prototypes
#include "LC8004delay_b.h"                                                      
#include <i2c.h>                                                                //I2C function prototypes
#include <uart.h>                                                               //UART functions




#define WP      LATFbits.LATF6                                                  //0=FRAM Enabled, 1=FRAM Write Protected(default)	(OUTPUT)

#define zero	0x30                                                            //ASCII "0"
#define one		0x31                                                            //ASCII "1"
#define two		0x32                                                            //ASCII "2"
#define three	0x33                                                            //ASCII "3"
#define four	0x34                                                            //ASCII "4"
#define five	0x35                                                            //ASCII "5"
#define six		0x36                                                            //ASCII "6"
#define seven	0x37                                                            //ASCII "7"
#define eight	0x38                                                            //ASCII "8"
#define nine	0x39                                                            //ASCII "9"



typedef union
{
    float g;                                                                    //32 bit float
    unsigned char x[4];                                                         //(4) bytes of 32 bit data
} xFRAMflt;

typedef union                                                                   //union for storing 32 bit value in 8 bit FRAM registers
{
    unsigned long f;                                                            //32 bit data
    unsigned char y[4];                                                         //(4) bytes of 32 bit data
} xFRAMul;

typedef union                                                                   //union for storing 16 bit value in 8 bit FRAM registers
{
    unsigned int ui;                                                            //16 bit data
    unsigned char z[2];                                                         //(2) bytes of 16 bit data
} xFRAMui;


typedef union                                                                   //union for storing 32 bit floating point value in 16 bit FRAM registers
{

	float f;                                                                    //32 bit float data
	unsigned int d[2];                                                          // (2) 16 bit words of 32 bit data

} tFRAMFloat;


typedef union                                                                   //union for storing 32 bit long value in 16 bit FRAM registers
{
	
	unsigned long l;                                                                     //32 bit long data 
	unsigned int e[2];                                                          // (2) 16 bit words of 32 bit data

} tFRAMLong;

extern void crlf(void);                                                         
extern float gageReading;
extern unsigned long TESTSECONDS;
unsigned long add=0;
extern unsigned char MSB;                                                       
extern unsigned char LSB;                                                       
extern unsigned char MMSB;                                                      
extern unsigned char MMMSB; 
char NackTest=0;                                                                 
char TEST=0;
unsigned int config2read=0x18;                                                  //d64 value for I2C1BRG at 925KHz   
unsigned int config2write=0x18;                                                 //d64 value for I2C1BRG at 925KHz   
//******************************************************************************
//                              displayError
//
//      Display the Reference Designation of the bad FRAM
//
//      Parameters received: unsigned int fram
//
//      Returns: nothing
//
//******************************************************************************
void displayError(unsigned int fram)                                        
{
    char errorAtU2[]={"U2 FAILURE!"};
    char errorAtU4[]={"U4 FAILURE!"};
    char errorAtU5[]={"U5 FAILURE!"};
    char errorAtU6[]={"U6 FAILURE!"};
    

    crlf();
    if(fram==1)
        putsUART1(errorAtU2);
    if(fram==2)
        putsUART1(errorAtU4);
    if(fram==3)
        putsUART1(errorAtU5);
    if(fram==4)
        putsUART1(errorAtU6);   
    while(BusyUART1());
    crlf();
}

//***************************************************************************
//				write_Int_FRAM()
//
//	Write 16 bit data to external FRAM address
//
//	Parameters received: unsigned long FRAM absolute address
//						 unsigned int 16 bit data to be stored
//	Returns: nothing
//
//
//***************************************************************************
void write_Int_FRAM(unsigned long absAdd, unsigned int data)                  
{
    xFRAMul	 	tempaddress;                                                    //variable tempaddress is type xFRAMul
    xFRAMui		tempdata;                                                       //variable tempdata is type xFRAMui
    unsigned char writeloop=0;                                                  //loop index
   
    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                          I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);

    WP=0;                                                                       //Disable Write Protection

    for(writeloop=0;writeloop<2;writeloop++)
    {
        __builtin_disi(0x3FFF);                                                 //Disable Interrupts    
        OpenI2C1(config1, config2write);                                        //open the I2C1 module
        IdleI2C1();                                                             //make sure bus is idle   
        StartI2C1();                                                            //transmit Start bit
        while(I2C1CONbits.SEN);                                                 //wait till start sequence is complete

        do{                                                                     
            NackTest=selectBank(absAdd,0);                                      //Select the appropriate FRAM   
        }while(NackTest);                                                        

        tempaddress.f=add;                                                      //convert 16 bit FRAM address into 2 bytes

        MasterWriteI2C1(tempaddress.y[1]);                                      //address high byte

        IdleI2C1();                                                             //wait till address high byte is transmitted  
        
        if(writeloop==0)                                                        //Data MSB address
        {
            MasterWriteI2C1(tempaddress.y[0]);                                  //address low byte
        }
        else
        if(writeloop==1)
        {
            tempaddress.y[0]+=1;                                                //increment the address for LSB storage
            MasterWriteI2C1(tempaddress.y[0]);                                  //address low byte + 1
        }

        IdleI2C1();                                                             //wait till address low byte is transmitted   
        tempdata.ui=data;                                                       //convert 16 bit data into 2 bytes

        //transmit the data
        if(writeloop==0)
        {
            MasterWriteI2C1(tempdata.z[1]);                                     //write the data high byte 
        }
        else
        if(writeloop==1)
        {
            MasterWriteI2C1(tempdata.z[0]);                                     //write the data low byte 
        }
        
        IdleI2C1();                                                             //wait till data high byte is transmitted 
        StopI2C1();
        while(I2C1CONbits.PEN);                                                 //wait till stop sequence is complete
        CloseI2C1();                                                            //close the I2C module    
        __builtin_disi(0x0000);                                                 //Re-enable interrupts          
    }                                                                           //end of for loop

    WP=1;                                                                       //Write protect the FRAM
    return 0;                                                                   //Write was good    
}






	
//***************************************************************************
//				read_Int_FRAM()
//
//	Read data stored at external FRAM address
//
//	Parameters received: unsigned long FRAM absolute address
//	Returns: unsigned int FRAM data
//
//
//***************************************************************************
unsigned int read_Int_FRAM(unsigned long absAdd)
{
    xFRAMul	tempaddress;                                                        //variable tempaddress is type xFRAMul
    xFRAMui	tempdata;                                                           //variable tempdata is type xFRAMui
    unsigned char readloop=0;                                                   //loop index
    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                            I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);

    __builtin_disi(0x3FFF);                                                     //Disable Interrupts    
    OpenI2C1(config1, config2read);                                             //open the I2C1 module
    IdleI2C1();                                                                 //make sure bus is idle 
    StartI2C1();                                                                //transmit Start bit
    while(I2C1CONbits.SEN);                                                     //wait till start sequence is complete

    for(readloop=0;readloop<2;readloop++)
    {
        do{                                                                     
            NackTest=selectBank(absAdd,0);                                      //select the appropriate FRAM   
        }while(NackTest);                                                        

        IdleI2C1();                                                             //wait till write is done
        while(I2C1STATbits.TRSTAT);
        tempaddress.f=add;                                                      //convert 16 bit FRAM address into 2 bytes

        //transmit the address:
        MasterWriteI2C1(tempaddress.y[1]);                                      //address high byte
        IdleI2C1();                                                             //wait till write is done  
        while(I2C1STATbits.TRSTAT);
        delay(20);                                                                   
        if(readloop==0)                                                         //data MSB address
            MasterWriteI2C1(tempaddress.y[0]);                                  //address low byte
        else
            if(readloop==1)
            {
                tempaddress.y[0]+=1;                                            //increment the address for LSB storage
                MasterWriteI2C1(tempaddress.y[0]);                              //address low byte + 1
            }

        IdleI2C1();                                                      
        while(I2C1STATbits.TRSTAT);                                             //wait till write is done
        delay(20);                                                           
        IdleI2C1();                                                       
        RestartI2C1();                                                          //change the direction of the bus

        delay(8);                                                         

        do{                                                                     
            NackTest=selectBank(absAdd,1);                                      //select the appropriate FRAM  
        }while(NackTest);                                                        


        IdleI2C1();                                                             //wait till read address is transmitted and ACK'd  
        while(I2C1STATbits.TRSTAT);

        if(readloop==0)                                                         //get data high byte from FRAM
        {
            tempdata.z[1]=MasterReadI2C1();                                     //and store in TempData MSB
            MSB=tempdata.z[1];                                                  //Binary 
        }
        else
        if(readloop==1)                                                         //get data low byte from FRAM
        {
            tempdata.z[0]=MasterReadI2C1();                                     //and store in TempData LSB
            LSB=tempdata.z[0];                                                  //Binary 
        }

        NotAckI2C1();                                                           //NACK
        IdleI2C1();                                                       
        RestartI2C1();                                                          //change the direction of the bus

        delay(8);                                                                                                                       
    }                                                                           //end of for loop

    StopI2C1();
    while(I2C1CONbits.PEN);                                                     //wait till stop sequence is complete

    CloseI2C1();                                                                //close the I2C1 module 
    __builtin_disi(0x0000);                                                     //Re-enable interrupts      

    return tempdata.ui;                                                         //Return the 16 bit data
}


//***************************************************************************
//				write_longFRAM()
//
//	Write 32 bit long data to FRAM address
//
//	Parameters received: unsigned long FRAM data, FRAM address
//	Returns: nothing
//
//
//***************************************************************************
void write_longFRAM(unsigned long l, unsigned long add)                                                  
{
	tFRAMLong temp;                                                             //variable temp is type tEEPROMFloat
	temp.l=l;                                                                   //32 bit data

    write_Int_FRAM(add, temp.e[1]);                                             //write upper 16 bits of 32 to address      
    write_Int_FRAM(add+2, temp.e[0]);                                           //write lower 16 bits of 32 to address+1	

}


//***************************************************************************
//				read_longFRAM()
//
//	Read 32 bit long data stored at FRAM address
//
//	Parameters received: FRAM address
//	Returns: long data
//
//
//***************************************************************************
unsigned long read_longFRAM(unsigned long add)                                           
{
    unsigned long tempLong=0;                                                   

	tFRAMLong temp;                                                             //variable temp is type tEEPROMlong

    temp.e[1]=read_Int_FRAM(add);                                               //read lower 16 bits of 32 to temp.e[1] 
	temp.e[0]=read_Int_FRAM(add+2);                                             //read upper 16 bits of 32 to temp.e[0]     

	tempLong=temp.l;
	return tempLong;
}


//***************************************************************************
//				read_float()
//
//	Read 32 bit float data stored at FRAM address
//
//	Parameters received: FRAM address
//	Returns: float data
//
//
//***************************************************************************
float read_float(unsigned long add)                                           
{
	float tempfloat=0.0;                                                   

	tFRAMFloat temp;                                                            //variable temp is type tFRAMfloat

    temp.d[1]=read_Int_FRAM(add);                                               //read lower 16 bits of 32 to temp.e[1] 
	temp.d[0]=read_Int_FRAM(add+2);                                             //read upper 16 bits of 32 to temp.e[0]     

	tempfloat=temp.f;
	return tempfloat;
}



//***************************************************************************
//				write_Flt_FRAM()
//
//	Write 32 bit data to external FRAM address
//
//	Parameters received: unsigned long FRAM absolute address
//						 32 bit data to be stored
//	Returns: nothing
//
//
//***************************************************************************
void write_Flt_FRAM(unsigned long absAdd, float f)                            
{
    xFRAMul tempaddress;                                                        //variable tempaddress is type xFRAMul
    xFRAMflt tempdata;                                                          //variable tempdata is type xFRAMflt
    unsigned char outerloop=0;                                                  //outer loop index
    unsigned char writeloop=0;                                                  //inner loop index
    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                            I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);

    WP=0;                                                                       //Disable Write Protection

    for(outerloop=0;outerloop<2;outerloop++)
    {

        for(writeloop=0;writeloop<2;writeloop++)
	{
            __builtin_disi(0x3FFF);                                             //Disable Interrupts    
            OpenI2C1(config1, config2write);                                    //open the I2C1 module
            StartI2C1();                                                        //transmit Start bit
            while(I2C1CONbits.SEN);                                             //wait till start sequence is complete

            do{                                                                 
                NackTest=selectBank(absAdd,0);                                  //select the appropriate FRAM   
            }while(NackTest);                                                    
            IdleI2C1();                                                         //wait till address is transmitted and ACK'd    

            if(outerloop==0)
                tempaddress.f=add;                                              //convert 16 bit FRAM address into 2 bytes
            else
                tempaddress.f=add+2;                                            //next 16 bit address for FP bytes 3 & 4

            //transmit the address:
            MasterWriteI2C1(tempaddress.y[1]);                                  //address high byte

            IdleI2C1();                                                         //wait till address high byte is transmitted   
            if(writeloop==0)                                                    //Data MSB address
            {
                MasterWriteI2C1(tempaddress.y[0]);                              //address low byte
            }
            else
            if(writeloop==1)
            {
                tempaddress.y[0]+=1;                                            //increment the address for LSB storage
                MasterWriteI2C1(tempaddress.y[0]);                              //address low byte + 1
            }

            IdleI2C1();                                                         //wait till address low byte is transmitted
            tempdata.g=f;                                                       //convert 16 bit data into 2 bytes

            //transmit the data
            if(writeloop==0 && outerloop==0)
            {
                MasterWriteI2C1(tempdata.x[3]);                                 //write the data MSB 
            }
            else
            if(writeloop==1 && outerloop==0)
            {
                MasterWriteI2C1(tempdata.x[2]);                                 //write the data MSB-1 
            }
            else
            if(writeloop==0 && outerloop==1)
            {
                MasterWriteI2C1(tempdata.x[1]);                                 //write the data MSB-2 
            }
            else
            if(writeloop==1 && outerloop==1)
            {
                MasterWriteI2C1(tempdata.x[0]);                                 //write the data LSB 
            }

            IdleI2C1();                                                         //wait till data byte is transmitted    
            StopI2C1();
            while(I2C1CONbits.PEN);                                             //wait till stop sequence is complete
            CloseI2C1();                                                        //close the I2C1 module 
            __builtin_disi(0x0000);                                             //Re-enable interrupts               
            
	}                                                                           //end of writeloop

	tempaddress.y[1]++;                                                         //increment the address pointer

    }                                                                           //end of outerloop

    WP=1;                                                                       //Write protect the FRAM
    return 0;                                                                   
}



//***************************************************************************
//				read_Flt_FRAM()
//
//	Read 32 bit float data stored at external FRAM address
//
//	Parameters received: unsigned long FRAM absolute address, switch for storage
//				
//	Stores result in global FP variable gageReading
//
//***************************************************************************
void read_Flt_FRAM(unsigned long absAdd, unsigned int x)
{
    xFRAMul	tempaddress;                                                        //variable tempaddress is type xFRAMul
    xFRAMflt	tempdata;                                                       //variable tempdata is type xFRAMflt

    unsigned char outerloop=0;                                                  //outer loop index
    unsigned char readloop=0;                                                   //inner loop index

    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                            I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);


    for(outerloop=0;outerloop<2;outerloop++)
    {
        for(readloop=0;readloop<2;readloop++)
	{
            __builtin_disi(0x3FFF);                                             //Disable Interrupts               
            OpenI2C1(config1, config2read);						
            IdleI2C1();                                                         //make sure bus is idle 
            StartI2C1();                                                        //transmit Start bit
            while(I2C1CONbits.SEN);                                             //wait till start sequence is complete

            do{                                                                 
                NackTest=selectBank(absAdd,0);                                  //Select the appropriate FRAM   
            }while(NackTest);                                                   

            IdleI2C1();                                                         //wait till write is done   
            while(I2C1STATbits.TRSTAT);

            if(outerloop==0)
                tempaddress.f=add;                                              //convert 16 bit FRAM address into 2 bytes
            else
                tempaddress.f=add+2;                                            //next 16 bit address for FP bytes 3 & 4

            //transmit the address:
            MasterWriteI2C1(tempaddress.y[1]);                                  //address high byte
            IdleI2C1();                                                         //wait till write is done   
            while(I2C1STATbits.TRSTAT);

            if(readloop==0)                                                     //data MSB address
                MasterWriteI2C1(tempaddress.y[0]);                              //address low byte
            else
                if(readloop==1)
		{
                    tempaddress.y[0]+=1;                                        //increment the address for LSB storage
                    MasterWriteI2C1(tempaddress.y[0]);                          //address low byte + 1
		}

            IdleI2C1();                                                       
            while(I2C1STATbits.TRSTAT);                                         //wait till write is done
            IdleI2C1();                                                       
            RestartI2C1();                                                      //change the direction of the bus

            delay(8);                                                         

            do{                                                                    
                NackTest=selectBank(absAdd,1);                                  //Select the appropriate FRAM   
            }while(NackTest);                                                    

            IdleI2C1();                                                         //wait till read address is transmitted and ACK'd  
            while(I2C1STATbits.TRSTAT);

            if(readloop==0 && outerloop==0)                                     //get the data MSB from FRAM
            {
                tempdata.x[3]=MasterReadI2C1();                                 //and store in TempData MSB
                MSB=tempdata.x[3];                                              //Binary 
            }
            else
            if(readloop==1 && outerloop==0)                                     //get the data MSB-1 from FRAM
            {
                tempdata.x[2]=MasterReadI2C1();                                 //and store in TempData MSB-1
                MMSB=tempdata.x[2];                                             //Binary 
            }
            else
            if(readloop==0 && outerloop==1)                                     //get the data MSB-2 from FRAM
            {
                tempdata.x[1]=MasterReadI2C1();                                 //and store in TempData MSB-1
                MMMSB=tempdata.x[1];                                            //Binary 
            }
            else
            if(readloop==1 && outerloop==1)                                     //get the data LSB from FRAM
            {
                tempdata.x[0]=MasterReadI2C1();                                 //and store in TempData MSB-1
                LSB=tempdata.x[0];                                              //Binary 
            }

            NotAckI2C1();                                                       //NACK
            IdleI2C1();                                                       
            RestartI2C1();                                                      //change the direction of the bus

            delay(8);                                                         
	}                                                                           //end of writeloop
		
	tempaddress.y[1]++;                                                         //increment the address pointer

    }                                                                           //end of outerloop

    StopI2C1();
    while(I2C1CONbits.PEN);                                                     //wait till stop sequence is complete

    CloseI2C1();                                                                //close the I2C1 module 
    __builtin_disi(0x0000);                                                     //Re-enable interrupts        

    switch(x)
    {
        case 0:
            if(tempdata.g==0xFFFFFFFF)						
                gageReading=999999.9;						
            else								
                gageReading=tempdata.g;                                         //store the floating point value in global gageReading
            break;
        case 1:
            if(tempdata.g==0xFFFFFFFF)						
                TESTSECONDS=999999.9;						
            else								
                TESTSECONDS=tempdata.g;                                         //store the 32 bit value in TESTFLT
            break;
        default:
            break;
    }

}


//***************************************************************************
//				selectBank()
//
//	Select the correct 1Mb FRAM (FM24V10) to read/write data
//
//	Parameters received: unsigned long FRAM absolute address
//                           unsigned char x:   0(write)
//                                              1(read)
//
//  Control Byte:   7   6   5   4   3   2   1   0
//                  1   0   1   0   A2  A1  PG  R/W
//
//	Returns: 0 if FRAM write is Ack'd, 1 if Nack'd
//
//
//***************************************************************************

unsigned char selectBank(unsigned long absAdd, unsigned char x)                 
{

    //select appropriate FRAM bank and adjust addressing to 0-131040 for each FRAM:

    if(absAdd>=0 && absAdd<131040)                                              //FRAM1 (U2)
    {
        if(absAdd>=0 && absAdd<65520)                                           //FRAM1 page 0  
        {
            add=absAdd;                                                         //addressing 0-65520
            if(x)
                MasterWriteI2C1(0xA1);                                          //Control Byte for FRAM1 page0 Read
            else
                MasterWriteI2C1(0xA0);                                          //Control Byte for FRAM1 page0 Write
            if(testNACK())                                                      //FRAM NACK'd
                return 1;
            return 0;                                                           //FRAM ACK'd
        }

        if(absAdd>=65520 && absAdd<131040)                                      //FRAM1 page 1
        {
            add=absAdd-65520;                                                   //scale addressing to 0-65520
            if(x)
                MasterWriteI2C1(0xA3);                                          //Control Byte for FRAM1 page1 Read
            else
                MasterWriteI2C1(0xA2);                                          //Control Byte for FRAM1 page1 Write
            if(testNACK())                                                      //FRAM NACK'd
                return 1;
            return 0;                                                           //FRAM ACK'd
        }
    }

    if(absAdd>=131040 && absAdd<262080)                                         //FRAM2 (U4)
    {
        if(absAdd>=131040 && absAdd<196560)                                     //FRAM2 page 0  
        {
            add=absAdd-131040;                                                  //scale addressing to 0-65535
            if(x)
                MasterWriteI2C1(0xA5);                                          //Control Byte for FRAM2 page0 Read
            else
                MasterWriteI2C1(0xA4);                                          //Control Byte for FRAM2 page0 Write
            if(testNACK())                                                      //FRAM NACK'd
                return 1;
            return 0;                                                           //FRAM ACK'd
        }

        if(absAdd>=196560 && absAdd<262080)                                     //FRAM2 page 1
        {
            add=absAdd-196560;                                                  //scale addressing to 0-65536
            if(x)
                MasterWriteI2C1(0xA7);                                          //Control Byte for FRAM2 page1 Read
            else
                MasterWriteI2C1(0xA6);                                          //Control Byte for FRAM2 page1 Write
            if(testNACK())                                                      //FRAM NACK'd
                return 1;
            return 0;                                                           //FRAM ACK'd
        }
    } 
    
    if(absAdd>=262080 && absAdd<393120)                                         //FRAM3 (U5)
    {
        if(absAdd>=262080 && absAdd<327600)                                     //FRAM3 page 0  
        {
            add=absAdd-262080;                                                  //scale addressing to 0-65535
            if(x)
                MasterWriteI2C1(0xA9);                                          //Control Byte for FRAM3 page0 Read
            else
                MasterWriteI2C1(0xA8);                                          //Control Byte for FRAM3 page0 Write
            if(testNACK())                                                      //FRAM NACK'd
                return 1;
            return 0;                                                           //FRAM ACK'd
        }

        if(absAdd>=327600 && absAdd<393120)                                     //FRAM3 page 1
        {
            add=absAdd-327600;                                                  //scale addressing to 0-65536
            if(x)
                MasterWriteI2C1(0xAB);                                          //Control Byte for FRAM3 page1 Read
            else
                MasterWriteI2C1(0xAA);                                          //Control Byte for FRAM3 page1 Write
            if(testNACK())                                                      //FRAM NACK'd
                return 1;
            return 0;                                                           //FRAM ACK'd
        }
    }     
    
    //if(absAdd>=393120 && absAdd<524160)                                         //FRAM4 (U6)  REM REV I
    if(absAdd>=393120 && absAdd<524287)                                         //FRAM4 (U6)    REV I
    {
        if(absAdd>=393120 && absAdd<458640)                                     //FRAM4 page 0  
        {
            add=absAdd-393120;                                                  //scale addressing to 0-65535
            if(x)
                MasterWriteI2C1(0xAD);                                          //Control Byte for FRAM4 page0 Read
            else
                MasterWriteI2C1(0xAC);                                          //Control Byte for FRAM4 page0 Write
            if(testNACK())                                                      //FRAM NACK'd
                return 1;
            return 0;                                                           //FRAM ACK'd
        }

        if(absAdd>=458640 && absAdd<524288)                                     //FRAM4 page 1
        {
            add=absAdd-458640;                                                  //scale addressing to 0-65536
            if(x)
                MasterWriteI2C1(0xAF);                                          //Control Byte for FRAM4 page1 Read
            else
                MasterWriteI2C1(0xAE);                                          //Control Byte for FRAM4 page1 Write
            if(testNACK())                                                       //FRAM NACK'd
                return 1;
            return 0;                                                           //FRAM ACK'd
        }
    }         
 
}


void sleepFRAM(void)                                                            
{
    unsigned char f=0;                                                          
    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                            I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);
    

    __builtin_disi(0x3FFF);                                                     //Disable Interrupts       
   
    WP=0;                                                                       //Disable Write Protection  
    for(f=0xA1;f<0xAE;f+=0x04)                                                  
    {


        OpenI2C1(config1, config2write);                                        //open the I2C1 module
        IdleI2C1();                                                             //make sure bus is idle 

        StartI2C1();                                                            //transmit Start bit
        while(I2C1CONbits.SEN);                                                 //wait till start sequence is complete

        while(I2C1STATbits.TRSTAT);                                             //Transmit status flag

        //transmit the reserved slave address:
        MasterWriteI2C1(0xF8);                                                  //reserved slave address
        while(I2C1STATbits.TRSTAT);                                             //Transmit status flag

        MasterWriteI2C1(f);                                                     //Select the FRAM
                                                                                //wait till write is done
        while(I2C1STATbits.TRSTAT);                                             //Transmit status flag

        StartI2C1();                                                            //Restart 
        while(I2C1CONbits.SEN);                                                 //wait till start sequence is complete
        while(I2C1STATbits.TRSTAT);                                             //Transmit status flag    

        //transmit the reserved slave ID:
        MasterWriteI2C1(0x86);                                                  //reserved slave ID
        while(I2C1STATbits.TRSTAT);                                             //Transmit status flag

        StopI2C1();
        while(I2C1CONbits.PEN);                                                 //wait till stop sequence is complete

        CloseI2C1();                                                            //close the I2C1 module 
    }
    WP=1;                                                                       //Enable Write Protection
    __builtin_disi(0x0000);                                                     //Re-enable interrupts        
 
}


//***************************************************************************
//				testFRAM()
//
//	Write 0x0000 and 0xFFFF to FRAM address and read back to confirm.
//	Store original value to temporary register and restore after each
//	address tested. Exit if any key is hit or bad memory location is
//	detected. FRAM memory locations tested:
//	ABSOLUTE ADDRESS	FRAM #          FRAM ADDRESS
//	0-63999                 1				0-63999
//	64000-127999			2				0-63999
//	128000-191999			3				0-63999
//	192000-255999			4				0-63999
//	256000-319999			5				0-63999
//	320000-383999			6				0-63999
//	
//	Percent Complete calculation: (testIndex/0xFFFF)*100
//
//	Parameters received: FRAM # (1-6)
//	Returns: 0 if memory ok, address of bad memory location if memory error 
//
//
//***************************************************************************
unsigned int testFRAM(unsigned int fram)
{
    char PercentComplete[]={"   % Complete"};
    char ExtFRAMtest1[]={"U3:"};                                                
    char ExtFRAMtest2[]={"U5:"};                                                
    char ExtFRAMtest3[]={"U6:"};                                                
    char ExtFRAMtest4[]={"U7:"};                                                
    char RxDataTemp=0;
    unsigned int originalFRAM=0;                                                //temporary location of original FRAM value
    unsigned int tempExtFRAM=0;                                                 //temporary test value
    unsigned long absadd=0;                                                     //FRAM absolute address
    float percentComplete=0.0;                                                  //percent of memory test completed
    float absaddFloat=0.0;
    
    TEST=1;                                                                     //let extFRAM functions know that TEST is occurring

    putcUART1(0x0D);                                                            //<CR>
    while(BusyUART1());
    putcUART1(0x0A);                                                            //<LF>
    while(BusyUART1());

    switch(fram)                                                                //DISPLAY
    {
        case 1:
            absadd=0;
            putsUART1(ExtFRAMtest1);                                            //"U3:"
            break;
        case 2:
            absadd=64000;
            putsUART1(ExtFRAMtest2);                                            //"U5:"
            break;
        case 3:
            absadd=128000;
            putsUART1(ExtFRAMtest3);                                            //"U6:"
            break;
        case 4:
            absadd=192000;
            putsUART1(ExtFRAMtest4);                                            //"U7:"
            break;
        default:
            break;
    }
    while(BusyUART1());

    putcUART1(0x0D);                                                            //<CR>
    while(BusyUART1());
    putcUART1(0x0A);                                                            //<LF>
    while(BusyUART1());
    putsUART1(PercentComplete);                                                 //"% Complete"
    while(BusyUART1());
    putcUART1(0x0D);                                                            //<CR>
    while(BusyUART1());

    IFS0bits.U1RXIF=0;                                                          //clear the UART interrupt flag


    do
    {
        //start and stop timers for each communication
        originalFRAM=read_Int_FRAM(absadd);                                     //temporarily store original FRAM value
        write_Int_FRAM(absadd,0x0000);                                          //write all zeros to FRAM memory location
        tempExtFRAM=read_Int_FRAM(absadd);                                      //read back

        if(tempExtFRAM!=0x0000)                                                 //error
        {

            switch(fram)
            {
                case 1:                                                         //return the bad memory address for U3 - not implemented
                    break;
                case 2:                                                         //return the bad memory address for U5 - not implemented
                    absadd-=64000;
                    break;
                case 3:                                                         //return the bad memory address for U6 - not implemented
                    absadd-=128000;
                    break;
                case 4:                                                         //return the bad memory address for U7 - not implemented
                    absadd-=192000;
                    break;
                default:
                    break;
            }
            Nop();
            displayError(fram);                                                 //Display the bad FRAM
            fram+=1;
            return 1;
        }
		
        write_Int_FRAM(absadd,0xFFFF);                                          //write all ones to FRAM memory location
        TEST=1;                                                                 //allow reading 0xFFFF	
        tempExtFRAM=read_Int_FRAM(absadd);                                      //read back
        TEST=0;								

        if(tempExtFRAM!=0xFFFF)                                                 //error
        {									

            switch(fram)
            {
                case 1:                                                         //return the bad memory address for U3 - not implemented
                    break;
                case 2:                                                         //return the bad memory address for U5 - not implemented
                    absadd-=64000;
                    break;
                case 3:                                                         //return the bad memory address for U6 - not implemented
                    absadd-=128000;
                    break;
                case 4:                                                         //return the bad memory address for U7 - not implemented
                    absadd-=192000;
                    break;
                default:
                    break;
            }
            displayError(fram);
            fram+=1;
            return 1;
	}

	write_Int_FRAM(absadd,originalFRAM);                                        //reload original value if FRAM tested ok

	switch(fram)                                                                //convert adsadd to float and offset for percentComplete calculation
	{
            case 1:                                                             //FRAM #1: absaddFloat=absadd
                absaddFloat=absadd;
                break;
            case 2:                                                             //FRAM #2: absaddFloat=absadd-64000
                absaddFloat=absadd-64000.0;
                break;
            case 3:                                                             //FRAM #3: absaddFloat=absadd-128000
                absaddFloat=absadd-128000.0;
                break;
            case 4:                                                             //FRAM #4: absaddFloat=absadd-192000
                absaddFloat=absadd-192000.0;
                break;
            default:
            break;
	}

        percentComplete=(absaddFloat/640.0);                                    //calculate percentage of memory tested

        if(percentComplete==0.0)                                                
        {
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==5.0)                                                
        {
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==10.0)                                               
        {
            putcUART1(one);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==15.0)                                               
        {
            putcUART1(one);
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==20.0)                                               
        {
            putcUART1(two);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==25.0)                                               
        {
            putcUART1(two);
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==30.0)                                               
        {
            putcUART1(three);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==35.0)                                               
        {
            putcUART1(three);
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==40.0)                                               
        {
            putcUART1(four);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==45.0)                                               
        {
            putcUART1(four);
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==50.0)                                               
        {
            putcUART1(five);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==55.0)                                               
        {
            putcUART1(five);
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==60.0)                                               
        {
            putcUART1(six);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==65.0)                                               
        {
            putcUART1(six);
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==70.0)                                               
        {
            putcUART1(seven);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==75.0)                                               
        {
            putcUART1(seven);
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR> 
        }

        if(percentComplete==80.0)                                               
        {
            putcUART1(eight);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==85.0)                                               
        {
            putcUART1(eight);
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==90.0)                                               
        {
            putcUART1(nine);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete==95.0)                                               
        {
            putcUART1(nine);
            putcUART1(five);
            putcUART1(0x0D);                                                    //<CR>  
        }

        if(percentComplete>99.0)                                                
        {
            putcUART1(one);
            putcUART1(zero);
            putcUART1(zero);
            putcUART1(0x0D);                                                    //<CR>  
        }
 

	absadd+=1;                                                                  //increment absadd
	if(fram==1 && absadd>=64000)                                                //break out of dowhile if all memory tested
            break;

    if(fram==2 && absadd>=128000)
            break;

	if(fram==3 && absadd>=192000)			
            break;

	if(fram==4 && absadd>=256000)			
            break;

    }while(absadd<256000 && !IFS0bits.U1RXIF);                                  //loop until complete or interrupted by keystroke

    IFS0bits.U1RXIF=0;                                                          //clear the UART interrupt flag
    RxDataTemp=ReadUART1();                                                     //get the char from the UART buffer to clear it
	
    return 0;
}

//***************************************************************************
//				testNACK()
//
//	Tests for NACK from FRAM after write to each base address
//				
//	Returns 0 if FRAM ACKs, 1 if FRAM NACKs
//
//***************************************************************************
unsigned char testNACK(void)
{
    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                            I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);

    if(I2C1STATbits.ACKSTAT)                                                    //Did the FRAM NACK?
    {                                                                           //Close the I2C bus if it did
        IdleI2C1();
        StopI2C1();
        while(I2C1CONbits.PEN);
        CloseI2C1();

        OpenI2C1(config1, config2read);                                         //reopen the I2C1 module
        IdleI2C1();                                                             //make sure bus is idle 
        StartI2C1();                                                            //transmit Start bit
        while(I2C1CONbits.SEN);                                                 //wait till start sequence is complete           
        
        return 1;                                                               //return 1
    }
    return 0;                                                                   //FRAM ACK'd so return 0
}


