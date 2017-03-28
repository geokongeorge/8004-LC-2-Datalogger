//****************************AD5241a.c**********************************
//
//			AD5241 Digital Pot functions
//
//***********************************************************************


#include "AD5241a.h"                                                            //function prototypes
#include <i2c.h>                                                                //I2C function prototypes

//***************************************************************************
//				read_AD5241()
//
//	Read 8 bit data from AD5241
//
//	Parameters received: none
//	Returns: unsigned char value
//
//
//***************************************************************************
unsigned char read_AD5241(void)
{
    unsigned int config1 = (I2C2_ON & I2C2_IDLE_CON & I2C2_CLK_HLD &
                          I2C2_IPMI_DIS & I2C2_7BIT_ADD &
                            I2C2_SLW_EN & I2C2_SM_DIS &
                            I2C2_GCALL_DIS & I2C2_STR_DIS &
                            I2C2_NACK & I2C2_ACK_DIS & I2C2_RCV_DIS &
                            I2C2_STOP_DIS & I2C2_RESTART_DIS &
                            I2C2_START_DIS);

    unsigned int config2=0x10;                                                  //value for I2C2BRG at 400KHz    
    unsigned char potRead=0x59;                                                 //slave address for read
    unsigned char value=0;  
    
    //Read the value of the pot:
    OpenI2C2(config1, config2);                                                 //open the I2C2 module
    StartI2C2();                                                                //transmit Start bit
    while(I2C2CONbits.SEN);                                                     //wait till start sequence is complete

    IdleI2C2();                                                                 //wait till address is transmitted and ACK'd  

    //transmit the slave read address:
    MasterWriteI2C2(potRead);                                                   //slave address (FRAME 1)
    IdleI2C2();                                                                 //wait till address is transmitted  

    //get the data:
    value=MasterReadI2C2();                                                     //get the data byte (FRAME 2)
    IdleI2C2();                                                                 //wait till instruction byte is transmitted         
    
    StopI2C2();
    while(I2C2CONbits.PEN);                                                     //wait till stop sequence is complete        
        
    CloseI2C2();                                                                //close the I2C module
    __builtin_disi(0x0000);                                                     //Re-enable interrupts   
    return value;                                                               //return the pot value
}

//***************************************************************************
//				write_AD5241()
//
//	Write 8 bit data to AD5241
//
//	Parameters received: unsigned char tap
//	Returns: nothing
//
//
//***************************************************************************
void write_AD5241(unsigned char tap)
{
    unsigned int config1 = (I2C2_ON & I2C2_IDLE_CON & I2C2_CLK_HLD &
                          I2C2_IPMI_DIS & I2C2_7BIT_ADD &
                            I2C2_SLW_EN & I2C2_SM_DIS &
                            I2C2_GCALL_DIS & I2C2_STR_DIS &
                            I2C2_NACK & I2C2_ACK_DIS & I2C2_RCV_DIS &
                            I2C2_STOP_DIS & I2C2_RESTART_DIS &
                            I2C2_START_DIS);

    unsigned int config2=0x10;                                                  //value for I2C2BRG at 400KHz    

    unsigned char potWrite=0x58;                                                //slave address for write
    //unsigned char potRead=0x59;                                                 //slave address for read
    unsigned char instByte=0x00;                                                //instruction byte
    //unsigned char value=0;                                                      

        __builtin_disi(0x3FFF);                                                 //Disable Interrupts    REV B
        OpenI2C2(config1, config2);                                             //open the I2C2 module
        IdleI2C2();                                                             //wait till bus is stable   
        
        StartI2C2();                                                            //transmit Start bit
        while(I2C2CONbits.SEN);                                                 //wait till start sequence is complete
        
        IdleI2C2();                                                             //wait till address is transmitted and ACK'd 

        //transmit the slave write address:
        MasterWriteI2C2(potWrite);                                              //slave address (FRAME 1)
        IdleI2C2();                                                             //wait till address is transmitted  

        //transmit the instruction byte:
        MasterWriteI2C2(instByte);                                              //write the instruction byte (FRAME 2)
        IdleI2C2();                                                             //wait till instruction byte is transmitted 

        //transmit the data:
        MasterWriteI2C2(tap);                                                   //write the data byte (FRAME 3)
        IdleI2C2();                                                             //wait till instruction byte is transmitted         
        StopI2C2();
        while(I2C2CONbits.PEN);                                                 //wait till stop sequence is complete
        CloseI2C2();                                                            //close the I2C module
        __builtin_disi(0x0000);                                                 //Re-enable interrupts  REV B        
}
