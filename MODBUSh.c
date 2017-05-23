//****************************MODBUSh.c**********************************
//
//			8004 MODBUS COMMUNICATION ROUTINES
//
//***********************************************************************
//REV A: 03/16/17                                                               Implement reception of MODBUS packet
//                                                                              Implement 1.5 & 3.5 Timers
//                                                                              Implement CRC generation and checking
//REV B: 03/24/17                                                               Incorporate MODBUS slave addressing & decoding
//REV C: 03/28/17                                                               Incorporate COMMAND decoding
//REV D: 04/14/17                                                               Incorporate paging for memory addressing
//REV E: 05/02/17                                                               Include LC8004extFRAM_i.h
//                                                                              Add Status register handling
//REV F: 05/02/17                                                               Continue to add functions to the Status register (BT,OP,SR,WF)
//REV G: 05/02/17                                                               Add CFG selection
//REV H: 05/04/17                                                               Change CFG bits to LSB of MODBUS STATUS1 register
//                                                                              Change X and Logging bits to MSB of MODBUS STATUS1 register
//                                                                              Add 'R', "RESET" & switch to cmd line functions


#include "MODBUSh.h"  
#include "FRAM_ADDRESSd.h"                                                      //REV H    
#include "LC8004extFRAM_i.h"
#include <xc.h>
#include <uart.h>
#include <stdbool.h>


unsigned char MODBUS_RXbuf[125];                                                //125 registers max
unsigned char MODBUS_TXbuf[125];                                                //125 registers max   
unsigned int modbusaddressvalue;
unsigned int ECHO=0;                                                            //REV D
unsigned int i=0;                                                               //REV CB


void MODBUScomm(void)
{
    unsigned char a=0;                                                          //loop index
    unsigned char arraysize=0;
    unsigned long memaddressStart=0;     
    unsigned int testvalue=0;                                                   //REV H
    csum       csumdata;                                                        //csumdata[1] is MSB, csumdata[0] is LSB
    csum       value;                                                           //value[1] is MSB, value[0] is LSB]
    csum       registers;                                                       //registers[1] is MSB, registers [0] is LSB
    bitflags   tempStatusValue;                                                 //temporary register for comparison REV CB 
    bitflags   tempValueValue;                                                  //convert value into bitfield register
    bitflags2  tempStatus2Value;                                                //temporary register for comparison REV H
    bitflags2  tempValue2Value;                                                 //convert value into bitfield register REV H
        
    for(a=0;a<125;a++)                                                        //TEST REM
    {
        MODBUS_TXbuf[a]=0;                                                      //Clear the MODBUS_TXbuf[]                 
    }                                                                           
    
    //Determine if crc value is correct:
    arraysize=MODBUS_RX();                                                      //Receive the incoming MODBUS packet
    csumdata.c=CRC(TEST,arraysize);                                             //compute the crc checksum
    if((MODBUS_RXbuf[(arraysize-1)] != csumdata.z[1]) | (MODBUS_RXbuf[(arraysize-2)] != csumdata.z[0]))  //received crc does not agree with computed crc
    {
        IFS3bits.T9IF=1;                                                        //exit if crc error
        return;
    }
    
    //crc value is correct so test if address is correct:
    modbusaddressvalue=read_Int_FRAM(MODBUSaddress);
    if(MODBUS_RXbuf[ADDRESS]!=modbusaddressvalue)                                  //
    {
        IFS3bits.T9IF=1;                                                        //exit if not an address match
        return;
    }      
    
    
    //address is correct so get the starting memory address that the function will work with:
    if(MODBUS_RXbuf[COMMAND]==READ_HOLDING)
    {
        //get the memory page                                                   //REV D
        memaddressStart=read_Int_FRAM(MODBUS_PAGEaddress);                      //REV D
        memaddressStart<<=8;                                                    //Shift the page 8 bits to the left
        memaddressStart|=MODBUS_RXbuf[REGISTER_MSB];                            //Add the MSB
        memaddressStart<<=8;                                                    //Shift 8 bits to the left
        memaddressStart|=MODBUS_RXbuf[REGISTER_LSB];                            //Add the LSB
    }
    
    if(MODBUS_RXbuf[COMMAND]==WRITE_HOLDING)
    {
        memaddressStart=0x07;                                                   //load 0x07
        memaddressStart<<=8;                                                    //Shift the 0x7 8 bits to the left
        memaddressStart|=MODBUS_RXbuf[REGISTER_MSB];                            //Add the MSB
        memaddressStart<<=8;                                                    //Shift 8 bits to the left
        memaddressStart|=MODBUS_RXbuf[REGISTER_LSB];                            //Add the LSB        
    }
    
    //get & perform the function:
    switch(MODBUS_RXbuf[COMMAND])
    {
        case READ_HOLDING:                                                      //REV C
            //get the number of registers to be read:
            registers.z[1]=MODBUS_RXbuf[4];
            registers.z[0]=MODBUS_RXbuf[5];
            
            for(a=0;a<((registers.c)*2);a+=2)
            {
                value.c=read_Int_FRAM((memaddressStart)+a);
                MODBUS_TXbuf[a+3]=value.z[1];                                   //load the MSB of the value into MODBUS_TXbuf[] odd registers starting at 0x03
                MODBUS_TXbuf[a+4]=value.z[0];                                   //load the LSB of the value into MODBUX_TXbuf[] even registers starting at 0x04
            }
            
            MODBUS_TXbuf[BYTE_COUNT]=2*(registers.c);                           //REV C
            break;
            
        case WRITE_HOLDING:
            if((memaddressStart>=0x7FA6C) && (memaddressStart<=0x7FFFF))        //Registers are not write protected
            {
                tempStatusValue.status=read_Int_FRAM(MODBUS_STATUS1address);         //REV CB
                MODBUS_TXbuf[REGISTER_MSB]=MODBUS_RXbuf[REGISTER_MSB];          //Load the TXbuf[] with Register address MSB
                MODBUS_TXbuf[REGISTER_LSB]=MODBUS_RXbuf[REGISTER_LSB];          //Load the TXbuf[] with Register address LSB
                value.z[1]=MODBUS_RXbuf[WRITE_DATA_MSB];                        //get the write data MSB
                MODBUS_TXbuf[WRITE_DATA_MSB]=MODBUS_RXbuf[WRITE_DATA_MSB];      //Load the TXbuf[] with data MSB
                value.z[0]=MODBUS_RXbuf[WRITE_DATA_LSB];                        //get the write data LSB
                MODBUS_TXbuf[WRITE_DATA_LSB]=MODBUS_RXbuf[WRITE_DATA_LSB];      //Load the TXbuf[] with data LSB
                write_Int_FRAM(memaddressStart, value.c);                       //Write to FRAM Register             
            }
            else
            {
                IFS3bits.T9IF=1;                                                //exit if write protected
                return;                
            }
            Nop();
            break;
            
        case WRITE_MULTIPLE:
            Nop();
            break;
            
        default:
            Nop();
            break;
    }
    
    //Fill in remaining registers:
    MODBUS_TXbuf[ADDRESS]=MODBUS_RXbuf[ADDRESS];
    MODBUS_TXbuf[COMMAND]=MODBUS_RXbuf[COMMAND];
    
    //calculate and append the crc value to the end of the array (little endian)
    if(MODBUS_RXbuf[COMMAND]==READ_HOLDING)
    {
        csumdata.c=CRC(GENERATE,MODBUS_TXbuf[BYTE_COUNT]+5);                    //compute the crc checksum on the MODBUS_TXbuf[]
        MODBUS_TXbuf[a+3]= csumdata.z[0];                                       //attach crc LSB
        MODBUS_TXbuf[a+4]= csumdata.z[1];                                       //attach crc MSB
        ECHO=0;                                                                 //REV D
    }
    else
    {
        csumdata.c=CRC(GENERATE,8);                                             //REV D
        MODBUS_TXbuf[6]= csumdata.z[0];                                         //attach crc LSB
        MODBUS_TXbuf[7]= csumdata.z[1];                                         //attach crc MSB
        ECHO=1;                                                                 //REV D
    }
    
    //Transmit the array:
    MODBUS_TX(ECHO);    
    
    //Perform the requested function if Status register was written            //REV CB
    
    
    if(memaddressStart==0x7fea4 && MODBUS_RXbuf[COMMAND]==WRITE_HOLDING)        //write to STATUS1 Register  REM REV H
    //if(memaddressStart==MODBUS_STATUS1address && MODBUS_RXbuf[COMMAND]==WRITE_HOLDING)        //write to STATUS1 Register  REV H        
    {
        if(tempStatusValue.status != value.c)                                        //if new value is different than what's present
        {
            tempValueValue.status = value.c;                                            //store received flags
            
            switch(tempValueValue.statusflags.bit012)                           //REV H
            {
                if (tempStatusValue.statusflags.bit012 == tempValueValue.statusflags.bit012)    //no difference between received and stored value
                            break;  
                
                if(tempStatusValue.statusflags.bit15)                            //if logging    REV H
                    break;                                                      //Don't allow CFG change    REV H
                
                case 0:                                                         //MX4   REV H
                    MX4();                                                      //Configure for 4VW/4TH
                    break;
		
                case 1:
                    MX16();
                    break;
            
                case 2:
                    MX1();
                    break;
                
                case 3:
                    MX8T();
                    break;
            
                case 4:
                    MX32();
                    break;
            
                case 5:
                    MX32T();
                    break;
            
                case 6:
                    MX8();
                    break;
            
                default:
                    break;
            }
			
            
            for(i=3;i<16;i++)                                                   //REV H
            {

                switch(i)
                {

                    case 3:                                                     //unused
                        if (tempStatusValue.statusflags.bit3 == tempValueValue.statusflags.bit3)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValueValue.statusflags.bit3)                     //load the RTC current time registers from FRAM
                            WRITE_TIME();
                        tempValueValue.statusflags.bit3=0;                      //clear this bit on exit
                        break;
                        
                    case 4:
                        if (tempStatusValue.statusflags.bit4 == tempValueValue.statusflags.bit4)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValueValue.statusflags.bit4)                     //enable Log Intervals
                            ENLOGINT();
                        else
                            DISLOGINT();                                        //disable Log Intervals
                        break;
                    
                    case 5:
                        if (tempStatusValue.statusflags.bit5 == tempValueValue.statusflags.bit5)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValueValue.statusflags.bit5)
                            wrap_one();
                        else
                            wrap_zero();
                        break;
		
                    case 6:
                        if (tempStatusValue.statusflags.bit6 == tempValueValue.statusflags.bit6)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValueValue.statusflags.bit6)
                            enableBT();
                        else
                            disableBT();
                        break;
		
                    case 7:
                        if (tempStatusValue.statusflags.bit7 == tempValueValue.statusflags.bit7)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValueValue.statusflags.bit7)
                            Nop();
                            //enableBTtimer();
                        else
                            Nop();
                            //disableBTtimer();
                        break;
		
                    case 8:
                        if (tempStatusValue.statusflags.bit8 == tempValueValue.statusflags.bit8)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValueValue.statusflags.bit8)
                            enableOP();                                         //REV F
                        else
                            disableOP();                                        //REV F
                        break;
		
                    case 9:
                        if (tempStatusValue.statusflags.bit9 == tempValueValue.statusflags.bit9)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValueValue.statusflags.bit9)
                            Nop();
                            //enableCPtimer()l
                        else
                            Nop();
                            //disableCPtimer();
                        break;
		
                    case 10:
                        if (tempStatusValue.statusflags.bit10 == tempValueValue.statusflags.bit10)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValueValue.statusflags.bit10)
                            synch_one();
                        else
                            synch_zero();
                        break;
                        
                    case 11:
                        if (tempStatusValue.statusflags.bit11 == tempValueValue.statusflags.bit11)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValueValue.statusflags.bit11)
                            Nop();
                            //EnableStartTime();
                        else
                            Nop();
                            //DisableStartTime();
                        break;
                        
                    case 12:
                        if (tempStatusValue.statusflags.bit12 == tempValueValue.statusflags.bit12)    //no difference between received and stored value
                            break;      
                        
                        if(tempValueValue.statusflags.bit12)
                            Nop();
                            //enableStopTime();
                        else
                            Nop();
                            //disableStopTime();
                        break;
                        
                    case 13:                                                    //
                        if (tempStatusValue.statusflags.bit13 == tempValueValue.statusflags.bit13)    //no difference between received and stored value
                            break;      
                        
                        if(tempValueValue.statusflags.bit13)
                            READ_TIME();                                        //get the RTC current time registers & store in FRAM
                        tempValueValue.statusflags.bit13=0;                     //clear this bit on exit                            
                        break;
                        
                    case 14:                                                    //REV G
                        if (tempStatusValue.statusflags.bit14 == tempValueValue.statusflags.bit14)    //no difference between received and stored value
                            break;      
                        
                        X();                                                    //take 'X' Reading  
                        tempValueValue.statusflags.bit14=0;                     //clear this bit on exit
                        break;   
                        
                    case 15:    
                        if (tempStatusValue.statusflags.bit15 == tempValueValue.statusflags.bit15)    //no difference between received and stored value
                            break;
                        
                        if(tempValueValue.statusflags.bit15)
                            testvalue=START();                                  //Start Logging
                        else
                            testvalue=STOP();                                   //Stop Logging
                        break;                        
		
                    default:
                        break;
                }
            }
            

        
        }
        
    }

    
    
    if(memaddressStart==MODBUS_STATUS2address && MODBUS_RXbuf[COMMAND]==WRITE_HOLDING)        //write to STATUS2 Register  REV H
    {
        if(tempStatus2Value.status2 != value.c)                                  //if new value is different than what's present
        {
            tempValue2Value.status2 = value.c;                                   //store received flags
            
            for(i=0;i<16;i++)                                                   //REV H
            {

                switch(i)
                {

                    case 0:                                                     
                        if (tempStatus2Value.status2flags.bit0 == tempValue2Value.status2flags.bit0)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValue2Value.status2flags.bit0)                   //Reset memory pointers REV H
                            R();
    
                        tempValue2Value.status2flags.bit0=0;                    //clear this bit on exit
                        break;
                        
                    case 1:
                        if (tempStatus2Value.status2flags.bit1 == tempValue2Value.status2flags.bit1)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValue2Value.status2flags.bit1)
                            RST();                                              //Reset uC  REV H

                        tempValue2Value.status2flags.bit1=0;                    //clear this bit on exit
                        break;
                    
                    case 2:
                        if (tempStatus2Value.status2flags.bit2 == tempValue2Value.status2flags.bit2)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValue2Value.status2flags.bit2)
                            CMD_LINE();
                        tempValue2Value.status2flags.bit2=0;                    //clear this bit on exit
                        break;
		
                    case 3:
                        if (tempStatus2Value.status2flags.bit3 == tempValue2Value.status2flags.bit3)    //no difference between received and stored value
                            break;                        
                        
                        if(tempValue2Value.status2flags.bit3)
                            loadDefaults();                                     //load default settings
                        tempValue2Value.status2flags.bit3=0;                    //clear this bit on exit                        
                        break;
		
                    case 4:
                        break;
		
                    case 5:
                        break;
		
                    case 6:
                        break;
		
                    case 7:
                        break;
                        
                    case 8:
                        break;
                        
                    case 9:
                        break;
                        
                    case 10: 
                        break;
                        
                    case 11:  
                        break;   
                        
                    case 12:
                        break;         
                        
                    case 13:
                        break;           

                    case 14:
                        break;           

                    case 15:
                        break;                                   
		
                    default:
                        break;
                }
            }
            

        
        }
        
    }
    
    
    IFS3bits.T9IF=1;                                                            //exit if not an address match
    return;
}


void configMODBUStimers(void)
{
    
    //Tcy=33.9084201389E-9S (Fcy=29.4912MHz with HSPLL x 4 and 14.7456MHz xtal)
    switch(baudrate)
    {   
        case 191:                                                               //9600 baud (use 1:8 prescaler))
            PR2=5202;                                                           //5202*8*Tcy = 1.4111mS intercharacter timeout
            PR6=12139;                                                          //12139*8*Tcy = 3.2929mS end of packet timeout
            break;
            
        case 63:                                                                //115200 baud
            PR2=3468;                                                           //3468*Tcy = 117.6uS intercharacter timeout
            PR6=8092;                                                           //8092*Tcy = 274.4uS end of packet timeout
            break;

        case 31:                                                                //230400 baud
            PR2=1734;                                                           //1734*Tcy = 58.8uS intercharacter timeout
            PR6=4046;                                                           //4046*Tcy = 137.2uS end of packet timeout            
            break;
            
        case 15:                                                                //460800 baud
            PR2=867;                                                            //867*Tcy = 29.4uS intercharacter timeout
            PR6=2023;                                                           //2023*Tcy = 68.6uS end of packet timeout                        
            break;            
    }
    
    T2CONbits.TON=0;                                                            //Make sure TMR2 is off
    T6CONbits.TON=0;                                                            //Make sure TMR6 is off
    
    T2CONbits.TCS=0;                                                            //Use Fcy as TMR2 clock
    T6CONbits.TCS=0;                                                            //Use Fcy as TMR6 clock
    
    T2CONbits.TGATE=0;                                                          //Disable TMR2 gated timer mode
    T6CONbits.TGATE=0;                                                          //Disable TMR6 gated timer mode
    
    if(baudrate==191)                                                           //9600 baud
    {
        T2CONbits.TCKPS=1;                                                      //Use 1:8 prescale value for TMR2
        T6CONbits.TCKPS=1;                                                      //Use 1:8 prescale value for TMR6
    }
    else
    {
        T2CONbits.TCKPS=0;                                                      //Use 1:1 prescale value for TMR2
        T6CONbits.TCKPS=0;                                                      //Use 1:1 prescale value for TMR6
    }        
    
    TMR2=0;                                                                     //Clear the TMR2 register
    TMR6=0;                                                                     //Clear the TMR6 register
    
    IEC0bits.T2IE=0;                                                            //Disable TMR2 interrupt
    IEC2bits.T6IE=0;                                                            //Disable TMR6 interrupt
    
    IFS0bits.T2IF=0;                                                            //Clear the TMR2 interrupt flag
    IFS2bits.T6IF=0;                                                            //Clear the TMR6 interrupt flag

}

unsigned int CRC(_Bool test, unsigned char size)
{
    unsigned int crcREG=0xFFFF;
    unsigned int i=0;
    unsigned int x=0;
        
    for(i=0;i<size-2;i++)                                                   //CRC calculation
    {
        if(test)
            crcREG^=MODBUS_RXbuf[i];                                               
        else
            crcREG^=MODBUS_TXbuf[i]; 
        for(x=0;x<8;x++)
        {
            if((crcREG & 0x0001) !=0)
            {
                crcREG>>=1;
                crcREG^=0xA001;
            }
            else
            {
                crcREG>>=1;
            }
        }

    }
    
    return crcREG;
}


unsigned char MODBUS_RX(void)
{
    //Enable Packet Timers:
    PMD1bits.T2MD=0;                                                            //Enable TMR2 module for intercharacter timing (TMR_A)
    PMD3bits.T6MD=0;                                                            //Enable TMR6 module for interframe timing and end of packet detect (TMR_B)
    
    
    unsigned char i;   
    unsigned char MODBUSdata=0;
    unsigned char T2counts=0;

    
    for(i=0;i<4;i++)                                                            //empty the Rx buffer
    {
        MODBUSdata = ReadUART1();
    }

    while (!IFS3bits.T9IF) //enable COM for Timeout period
    {
        configMODBUStimers();                                                   //Setup TMR2 & TMR6 for packet timing

        for (i = 0; i < 125; i++) //clear the buffer
        {
            MODBUS_RXbuf[i] = 0;
        }

        i = 0;                                                                  //initialize char buffer index

        MODBUSdata = 0; 

        while (!IFS3bits.T9IF)
        {
            while (!IFS3bits.T9IF)
            {

                while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS3bits.T9IF  && !IFS0bits.T2IF && !IFS2bits.T6IF); //read the MODBUS transmission
                if(U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR) 
                    handleCOMError();                                           //if communications error

                if (IFS3bits.T9IF)                                              //break out of loop if 15 seconds of inactivity
                    return 0;                                                     
                
                if(IFS0bits.T2IF)                                               //1.5 character timeout occurred
                {
                    T2CONbits.TON=0;                                            //Stop TMR2 
                    IFS0bits.T2IF=0;                                            //clear the interrupt flag
                    T2counts+=1;                                                //increment the intercharacter timeout register
                    TMR2=0;                                                     //Clear the TMR2 register            
                    T2CONbits.TON=1;                                            //Restart TMR2
                    continue;
                }
                
                if(IFS2bits.T6IF)                                               //Rx Packet complete    
                {
                    PMD1bits.T2MD=1;                                            //Disable TMR2 module
                    PMD3bits.T6MD=1;                                            //Disable TMR6 module
                    IFS3bits.T9IF=1;                                            //Set the T9IF go to sleep
                    if(T2counts!=2)                                             //2 1.5 character timeouts occur before  end of packet   
                        return 0;
                    else
                        return i;                                               //RETURN # OF CHARS IN ARRAY
                }
                

                T2CONbits.TON=0;
                T6CONbits.TON=0;                                                //Stop TMR6
                
                MODBUSdata = ReadUART1();                                       //get the char from the USART buffer
                
                MODBUS_RXbuf[i] = MODBUSdata;                                      //store the received char in the buffer(i)
                MODBUSdata=0;                                                   //zero the MODBUSdata
                i++;
                
                TMR2=0;
                TMR6=0;                                                         //Reset the TMR6 register

                T6CONbits.TON=1;                                                //Start TMR6 for end of packet detection 
                T2CONbits.TON=1;                                                //Start TMR2 for intercharacter timing
                
            }                                                                   

        }                                                                       

    }                                                                           
    
    return 0;
}

void MODBUS_TX(unsigned int echo)                                               //REV D
{
    unsigned char i; 
    
    _RS485RX_EN=1;                                                              //Disable the RS485 Rx  REV H
    RS485TX_EN=1;                                                               //Enable the RS485 Tx   REV H
    
    if(!echo)
    {
        for(i=0;i<MODBUS_TXbuf[BYTE_COUNT]+5;i++)                                   
        {
            putcUART1(MODBUS_TXbuf[i]);
            while(BusyUART1());
        }
    }
    else
    {
        for(i=0;i<8;i++)                                                        //REV D
        {
            putcUART1(MODBUS_TXbuf[i]);
            while(BusyUART1());
        }        
    }
    
    RS485TX_EN=0;                                                               //Disable the RS485 Tx   REV H
    _RS485RX_EN=0;                                                              //Enable the RS485 Rx  REV H
}