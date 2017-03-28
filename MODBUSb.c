//****************************MODBUSa.c**********************************
//
//			8004 MODBUS COMMUNICATION ROUTINES
//
//***********************************************************************
//REV A: 03/16/17                                                               Implement reception of MODBUS packet
//                                                                              Implement 1.5 & 3.5 Timers
//                                                                              Implement CRC generation and checking
//REV B: 03/24/17                                                               Incorporate MODBUS slave addressing


#include "MODBUSb.h"  
#include <xc.h>
#include <uart.h>
#include <stdbool.h>


char MODBUSbuf[8];


void MODBUScomm(void)
{
    unsigned char arraysize=0;
    csum       csumdata;                                                        //csumdata[1] is MSB, csumdata[0] is LSB
    
    //Determine if crc value is correct:
    arraysize=MODBUS_RX();                                                      //Receive the incoming MODBUS packet
    csumdata.c=CRC(TEST,arraysize);                                             //compute the crc checksum
    if((MODBUSbuf[(arraysize-1)] != csumdata.z[1]) | (MODBUSbuf[(arraysize-2)] != csumdata.z[0]))  //received crc does not agree with computed crc
    {
        IFS3bits.T9IF=1;                                                        //exit
        return;
    }
    //crc value is correct
    Nop();
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
        
    if(test)                                                                    //check received CRC against computed CRC
    {
        for(i=0;i<sizeof(MODBUSbuf)-2;i++)
        {
            crcREG^=MODBUSbuf[i];                                               //XOR crcREG with MODBUSbuf{i}
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
    }
    
    return crcREG;
}


unsigned char MODBUS_RX(void)
{
    //Enable Packet Timers:
    PMD1bits.T2MD=0;                                                            //Enable TMR2 module for intercharacter timing (TMR_A)
    PMD3bits.T6MD=0;                                                            //Enable TMR6 module for interframe timing and end of packet detect (TMR_B)
    
    
    unsigned char i;   
    char MODBUSdata=0;
    unsigned char T2counts=0;

    
    for(i=0;i<4;i++)                                                            //empty the Rx buffer
    {
        MODBUSdata = ReadUART1();
    }

    while (!IFS3bits.T9IF) //enable COM for Timeout period
    {
        configMODBUStimers();                                                   //Setup TMR2 & TMR6 for packet timing

        for (i = 0; i < 8; i++) //clear the buffer
        {
            MODBUSbuf[i] = 0;
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
                    testPoint(1,1);
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
                    {
                        testPoint(1,2);
                        return 0;
                    }
                    else
                    {
                        testPoint(1,3);
                        return i;                                               //RETURN # OF CHARS IN ARRAY
                    }
                }
                

                T2CONbits.TON=0;
                T6CONbits.TON=0;                                                //Stop TMR6
                
                MODBUSdata = ReadUART1();                                       //get the char from the USART buffer
                
                MODBUSbuf[i] = MODBUSdata;                                      //store the received char in the buffer(i)
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