//****************************MODBUSa.c**********************************
//
//			8004 MODBUS COMMUNICATION ROUTINES
//
//***********************************************************************
//REV A: 03/16/17                                                               Implement reception of MODBUS packet
//                                                                              Implement 1.5 & 3.5 Timers
//                                                                              Implement CRC generation and checking


#include "CRCTEST.h"  
#include <xc.h>
#include <stdbool.h>

//char MODBUSbuf[8]={0x01,0x03,0x01,0x00,0x00,0x08,0x45,0xF0};
//char MODBUSbuf[8]={0x02,0x03,0x01,0x00,0x00,0x08,0x45,0xF0};
unsigned char MODBUSbuf[8]={0x01,0x03,0xfe,0xa8,0x00,0x01,0x45,0xF0};
//unsigned char MODBUSbuf[8]={0x01,0x03,0x80,0x7f,0x7f,0x7f,0x45,0xF0};

main()
{
    unsigned int result=0;
    
    
    result=CRC(true,8);
    Nop();
}
unsigned int CRC(_Bool test, unsigned char size)
{
    unsigned int crc=0xFFFF;
    unsigned int i=0;
    unsigned int x=0;
    
    
    if(test)                                                                    //check received CRC against computed CRC
    {
        for(i=0;i<sizeof(MODBUSbuf)-2;i++)
        {
            crc^=MODBUSbuf[i];                                                  //XOR crcREG with MODBUSbuf{i}
            for(x=0;x<8;x++)
            {

                if((crc & 0x0001) !=0)
                {
                    crc>>=1;
                    crc^=0xA001;
                }
                else
                {
                    crc>>=1;
                }

            }

        }
        return crc;
    }
 
}

