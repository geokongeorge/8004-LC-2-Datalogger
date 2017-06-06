#include <uart.h>
#include "LC8004intEEPROM_a.h"

#define	cr	0x0D;

static unsigned int tempW0;
static unsigned int tempW1;

typedef union						//union for storing 32 bit floating point value in 16 bit EEPROM registers
{
    float f;						//32 bit float data
    unsigned int d[2];				// (2) 16 bit words of 32 bit data
} tEEPROMFloat;

typedef union						//union for storing 32 bit long value in 16 bit EEPROM registers
{
    long l;							//32 bit long data
    unsigned int e[2];				// (2) 16 bit words of 32 bit data
} tEEPROMLong;

//***************************************************************************
//				popW()
//
//	Restore W0 and W1 from temporary registers
//
//	Parameters received: none
//	Returns: nothing
//
//
//***************************************************************************
//void popW(void)
//{
//    WREG0=tempW0;
//    WREG1=tempW1;
//}


//***************************************************************************
//				pushW()
//
//	Save W0 and W1 to temporary registers
//
//	Parameters received: none
//	Returns: nothing
//
//
//***************************************************************************
//void pushW(void)
//{
//    tempW0=WREG0;
//    tempW1=WREG1;
//}

	
//***************************************************************************
//				read_intEEPROM()
//
//	Read data stored at EEPROM address
//
//	Parameters received: EEPROM address
//	Returns: EEPROM data
//
//
//***************************************************************************
unsigned int read_intEEPROM(unsigned int add)
{
    unsigned int EEdata;

    shutdownTimerwithReset(1);									//start the reset timer to reset if hangs
    //pushW();				//save W0 and W1
    TBLPAG = 0x7F;
    WREG0 = add;			 //address to read from
    asm volatile ("TBLRDL [w0],w4");
    Nop();
    EEdata = WREG4; 		//WREG4 will contain data read from eeprom
    //popW();					//restore W0 and W1
    StopshutdownTimer();									//stop the reset timer
    return EEdata;			//give this data to the calling function
}



//***************************************************************************
//				write_intEEPROM()
//
//	Write data to EEPROM address
//
//	Parameters received: EEPROM address, EEPROM data
//	Returns: 0 = No error
//  Non-Zero = Wr Error (MCLR reset or WDT during Erase/Wr EEPROM)
//***************************************************************************
int write_intEEPROM(unsigned int add, unsigned int data)
{
    int TBLPAG_SAV, SR_SAV;

    if(NVMCONbits.WRERR)
        return(NVMADR);  //Error, return last failed address

    SR_SAV = SR;
    shutdownTimerwithReset(1);	//start the reset timer to reset if hangs

    TBLPAG_SAV = TBLPAG;
    SRbits.IPL = 0x7;		//Disable interrupts
    TBLPAG = 0x7F;
    NVMADR=(add | 0xF000);			//point to EEPROM address
    NVMCON = 0x4044;		//select data EEPROM word, erase, wren bits
    __builtin_tblwtl((add | 0xF000), data);

    __builtin_write_NVM();		//erase
    while(NVMCONbits.WR); 		//wait for write to complete

    NVMCON = 0x4004;
    __builtin_tblwtl((add | 0xF000), data);
    __builtin_write_NVM();		//write
    while(NVMCONbits.WR); 		//wait for write to complete

    TBLPAG = TBLPAG_SAV;		//Restore TBLPAG
    SR = SR_SAV;
    StopshutdownTimer();		//stop the reset timer

    if(NVMCONbits.WRERR)
        return(NVMADR);  //Error, current failed address
    else
        return(0);
}


//***************************************************************************
//				read_signedintEEPROM()
//
//	Read data stored at EEPROM address
//
//	Parameters received: EEPROM address
//	Returns: EEPROM data
//
//
//***************************************************************************
int read_signedintEEPROM(unsigned int add)
{
    int EEdata;

    shutdownTimerwithReset(1);									//start the reset timer to reset if hangs
    //pushW();				//save W0 and W1
    TBLPAG = 0x7F;
    WREG0 = add;			 //address to read from
    asm volatile ("TBLRDL [w0],w4");
    Nop();
    EEdata = WREG4; 		//WREG4 will contain data read from eeprom
    //popW();					//restore W0 and W1
    StopshutdownTimer();									//stop the reset timer
    return EEdata;			//give this data to the calling function
}


//***************************************************************************
//				testInternalEEPROM()
//
//	Write 0x0000 and 0xFFFF to EEPROM address and read back to confirm. 
//	Store original value to temporary register and restore after each
//	address tested. Exit if any key is hit or bad memory location is
//	detected. EEPROM memory locations tested: 0x7FF000 -> 0x7FFFFE. The 7F
//	is appended in write_intEEPROM() and read_intEEPROM().
//
//	Percent Complete calculation: ((testIndex-0xF000)/0xFFF)*100
//
//	Parameters received: none
//	Returns: 0 if memory ok, address of bad memory location if memory error 
//
//
//***************************************************************************
unsigned int testInternalEEPROM(void)
{
    char testBUF[8];						//temporary storage for percentComplete display data
    char PercentComplete[]={"   % Complete"};
    char intEEtest[]={"INTERNAL EEPROM TEST:"};
    char RxDataTemp=0;
    unsigned int originalEEPROM;			//temporary location of original EEPROM value
    unsigned int tempIntEEPROM;				//temporary test value
    unsigned int testIndex=61438;			//will get incremented to 61440 (0xF000) at
								//beginning of dowhile loop
    float percentComplete=0.0;				//percent of memory test completed
    float testIndexFloat=0.0;

    putcUART1(0x0D);						//<CR>
    while(BusyUART1());
    putcUART1(0x0A);						//<LF>
    while(BusyUART1());
    putsUART1(intEEtest);					//"INTERNAL EEPROM TEST:"
    while(BusyUART1());
    putcUART1(0x0D);						//<CR>
    while(BusyUART1());
    putcUART1(0x0A);						//<LF>
    while(BusyUART1());
    putsUART1(PercentComplete);				//"% Complete"
    while(BusyUART1());
    putcUART1(0x0D);						//<CR>
    while(BusyUART1());

    IFS0bits.U1RXIF=0;						//clear the UART interrupt flag

    do
    {
        testIndex+=2;						//double-increment index

	originalEEPROM=read_intEEPROM(testIndex);	//temporarily store original EEPROM value
	write_intEEPROM(testIndex,0x0000);	//write all zeros to EEPROM memory location
	tempIntEEPROM=read_intEEPROM(testIndex);	//read back

	if(tempIntEEPROM!=0x0000)			//error
	{									//handle error here
            return testIndex;				//return the bad memory address
	}
		
	write_intEEPROM(testIndex,0xFFFF);	//write all ones to EEPROM memory location
	tempIntEEPROM=read_intEEPROM(testIndex);	//read back
	if(tempIntEEPROM!=0xFFFF)			//error
	{									//handle error here
            return testIndex;				//return the bad memory address
	}

	write_intEEPROM(testIndex,originalEEPROM);	//reload original value if EEPROM tested ok

	testIndexFloat=testIndex;			//convert testIndex to float
	percentComplete=((testIndex-61440.0)/4095.0)*100.0;	//calculate percentage of memory tested
	sprintf(testBUF,"%.0f",percentComplete);					
	putsUART1(testBUF);								//display it
	while(BusyUART1());	
	putcUART1(0x0D);								//<CR>
	while(BusyUART1());	

    }while(testIndex<0xFFFE && !IFS0bits.U1RXIF);	//loop until complete or interrupted by keystroke
					
    IFS0bits.U1RXIF=0;						//clear the UART interrupt flag
    RxDataTemp=ReadUART1();						//get the char from the UART buffer to clear it
	
    return 0;
}


//***************************************************************************
//				write_floatEEPROM()
//
//	Write 32 bit float data to EEPROM address
//
//	Parameters received: FP EEPROM data, EEPROM address
//	Returns: nothing
//
//
//***************************************************************************
void write_floatEEPROM(float f, unsigned int add)
{
    int EEPROMTest=0;

    tEEPROMFloat temp;					//variable temp is type tEEPROMFloat

    temp.f=f;							//32 bit data

    EEPROMTest=write_intEEPROM(add, temp.d[0]);	//write lower 16 bits of 32 to address
            //if(EEPROMTest)                                            //REM VER 6.0.0
            //    displayEEPROMError(EEPROMTest);                       //REM VER 6.0.0
    EEPROMTest=write_intEEPROM(add+2, temp.d[1]);	//write upper 16 bits of 32 to address+1
            //if(EEPROMTest)                                            //REM VER 6.0.0
            //    displayEEPROMError(EEPROMTest);                       //REM VER 6.0.0
}


//***************************************************************************
//				read_floatEEPROM()
//
//	Read 32 bit float data stored at EEPROM address
//
//	Parameters received: EEPROM address
//	Returns: float data
//
//
//***************************************************************************
float read_floatEEPROM(unsigned int add)
{
    float tempFloat=0.0;

    tEEPROMFloat temp;					//variable temp is type tEEPROMFloat

    temp.d[0]=read_intEEPROM(add);		//read lower 16 bits of 32 to temp.d[0]
    temp.d[1]=read_intEEPROM(add+2);	//read upper 16 bits of 32 to temp.d[1]

    tempFloat=temp.f;
    return tempFloat;
}

//***************************************************************************
//				write_longEEPROM()
//
//	Write 32 bit long data to EEPROM address
//
//	Parameters received: long EEPROM data, EEPROM address
//	Returns: nothing
//
//
//***************************************************************************
void write_longEEPROM(long l, unsigned int add)
{
    int EEPROMTest=0;

    tEEPROMLong temp;					//variable temp is type tEEPROMFloat

    temp.l=l;							//32 bit data

    EEPROMTest=write_intEEPROM(add, temp.e[0]);	//write lower 16 bits of 32 to address
            //if(EEPROMTest)                                            //REM VER 6.0.0
            //    displayEEPROMError(EEPROMTest);                       //REM VER 6.0.0
    EEPROMTest=write_intEEPROM(add+2, temp.e[1]);	//write upper 16 bits of 32 to address+1
            //if(EEPROMTest)                                            //REM VER 6.0.0
            //    displayEEPROMError(EEPROMTest);                       //REM VER 6.0.0
}


//***************************************************************************
//				read_longEEPROM()
//
//	Read 32 bit long data stored at EEPROM address
//
//	Parameters received: EEPROM address
//	Returns: long data
//
//
//***************************************************************************
long read_longEEPROM(unsigned int add)
{
    long tempLong=0;

    tEEPROMLong temp;					//variable temp is type tEEPROMlong

    temp.e[0]=read_intEEPROM(add);		//read lower 16 bits of 32 to temp.d[0]
    temp.e[1]=read_intEEPROM(add+2);	//read upper 16 bits of 32 to temp.d[1]

    tempLong=temp.l;
    return tempLong;
}
