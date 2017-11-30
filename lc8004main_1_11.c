//-------------------------------------------------------------
//
//				LC8004
//		Single/4-Channel/8-Channel/16-Channel/32-Channel VW
//              8-Channel/32 Channel Thermistor
//              VW/Thermistor Datalogger
//		with USB/RS-232/RS-485/BT interface
//      Command-line and MODBUS protocols
//
//-------------------------------------------------------------


//-------------------------------------------------------------
//
//	COMPANY:	GEOKON, INC
//	DATE:		11/27/2017
//	DESIGNER: 	GEORGE MOORE
//	REVISION:   1.11
//	CHECKSUM:	0xae64 (MPLABX ver 3.15 and XC16 ver 1.26)
//	DATA(RAM)MEM:	8874/30720   29%
//	PGM(FLASH)MEM:  185640/261888 71%

//  Target device is Microchip Technology DsPIC33FJ256GP710A
//  clock is crystal type HSPLL @ 14.7456 MHz Crystal frequency
//  PLL prescaler (N1) = 2  (7.3728MHz)
//  PLL feedback divisor (M) = 16   (117.9648MHz)
//  PLL postscaler (N2) = 2 (58.9824 MHz)
//  This results in UART baud rates of 9600, 115200, 230400, 460800 & 921600 with 0% error
//  9600:   BRGH=1, U1BRG=191
//  115200: BRGH=0, U1BRG=63
//  230400: BRGH=0, U1BRG=31
//  460800: BRGH=0, U1BRG=15
//  921600: BRGH=0, U1BRG=7
//  Fcy = F_PLL/2 = 29.4912 MHz
//	Tcy= 33.9084201389 nS (4x faster than original HS oscillator)

//	Watchdog Timer is ENABLED during Take_One_Complete_Reading() via software control.
//	WDT timeout period is 4S
//	WDT is cleared with the ClrWdt() function and DISABLED after each successful VW read operation.


//	CONFIGURATION SETTINGS:                             (Set in LC8004main_x.h)
//
//      Oscillator                                          HSPLL
//      Clock Switching and Monitor                         Sw Disabled, Mon Enabled
//      WDT Postscaler                                      1:16
//      WDT Prescaler                                       1:128
//      Watchdog Timer                                      Disabled (software control)
//      POR Timer Value                                     128mS
//      Boot Segment Program Memory Write Protect           Boot Segment Program Memory may be written
//      Boot Segment Program Flash Memory Code Protection   No Boot Segment
//      Boot Segment Data FRAM Protection                   No Boot FRAM
//      Boot Segment Data RAM Protection                    No Boot RAM
//      Secure Segment Program Write Protect                Disabled
//      Secure Segment Program Flash Memory Code Protection No Secure Segment
//      Secure Segment Data FRAM Protection                 No Segment Data FRAM
//      Secure Segment Data RAM Protection                  No Secure RAM
//      General Code Segment Write Protect                  Disabled
//      General Segment Code Protection                     Disabled
//      Comm Channel Select                                 Use PGC/EMUC and PGD/EMUD



//     	REVISION HISTORY:

//	REVISION        DATE				DESCRIPTION

//      a       08/18/16			Based on LC-2MUXmain ver 6.0.13
//                                  REM all calls to internal EEPROM - assign to FRAM
//                                  Remove intEEPROM.c & .h from project
//                                  Map CFG memory to FRAM
//      b       08/24/16            Debug 15 second timeout timer
//                                  Debug all FRAM reads & writes
//                                  Change RTC Interrupt from INT2 to INT1
//      c       08/26/16            Include LC8004_b.c,h to increase speed of I2C reads/writes
//                                  Rewrite FRAM TEST routine
//      d       09/14/16            Incorporate RTC functions in main()
//                                  remove LC8004rtc_a.c,h from project
//                                  Debug Sleep() and waking from Sleep()
//                                  Minimize Sleep current
//                                  Debug +3X_X Test
//                                  Add 5V and 12V Excitation Test to Test Menu
//      e       09/15/16            Debug H-Bridge timings
//                                  NOTE: Sleep current are still too high - look at uC module and port configuration during sleep
//      f       09/19/16            Add AD5241 Digital Pot communications (AD5241.h/.c)
//                                  Modify Test Menu items 4 & 5 to 400Hz and 6000Hz Excitation
//                                  Add AD5241 pot selections in Test Menu
//      g       09/22/16            Remove control lines EFGH
//                                  Use Output Compare 1 (pin 72) to provide CLOCK for MAX7407 LPF
//                                  Use TMR2 as 16bit timer for Output Compare 1
//      h       09/30/16            Add PLL configurations to Test Menu
//                                  Only close I2C once - at shutdown immediately before Sleep()
//      i       10/05/16            Add GT6 (ref 8002 ver 5.6.x)
//      j       10/06/16            Add HT Therm capability (ref 8002 ver 5.7.x)
//      k       10/10/16            Read DS3231 temperature registers instead of internal thermistor measurement for 
//                                  internal temperature readings
//                                  debug external temperature reading for X command
//      L       11/7/16             Debug PLUCK for X command
//      m       11/9/16             Debug VW readings for X command
//                                  Use TMR7 with 1:8 prescaler for measuring VW frequency
//      n       11/28/16            Debug scanning and port on/off times
//      o       11/30/16            Convert VW counter from 16 bit to 32 bit
//      p       12/02/16            Populate unused interrupt vectors with asm"RESET"
//                                  Debug FRAM addressing - include LC8004extFRAM_c.h
//                                  Reduce FRAM I2C bit rate to 800KHz from 1MHz
//      q       12/06/16            Debug ext therm reading error
//      r       12/08/16            Cleanup loadDefaults()
//                                  Debug MX4 configuration - Fixed incorrect absolute addresses in lc8004main_q.h file - fixed in lc8004main_r.h file
//      s       12/08/16            Debug multichannel X command
//      t       12/09/16            Debug thermistor type selection in multichannel configs
//      u       12/14/16            Debug MX8T & MX32T thermistor readings
//      v       12/19/16            Include LC8004extFRAM_e.h, .c which refreshes FRAM after each read and limits # of readings/memory page to prevent page overrun
//      w       12/28/16            Write to FRAM at 400KHz and read at 925KHz
//                                  Debug logging start/stop times: If Start/Stop logging times are enabled and one logging sequence is complete, when the timed 
//                                  output port activates logging resumes
//                                  Reload Start & Stop logging times so they repeat on a daily basis
//      x       1/3/17              Reduce sleep current
//                                  Include LC8004extFRAM_f.h,.c
//      y       1/6/17              Include LC8004extFRAM_g.h,.c
//      z       1/10/17              Debug Start/Stop logging times (again)
//                                  Configure TMR8/9 as Shutdown Timer
//                                  Include LC8004delay_b.h,.c
//                                  Debug Sleep between readings
//                                  Debug INT1IF getting set when ADC gets enabled
//      aa      1/12/17             Debug binary download functions
//                                  work on reducing sleep currents even further
//      ab      1/18/18             Single Channel:For binary download, load an array with readings, compute & load the checksum and then TX the array.
//      ac      1/19/17             Include binary download for all other configurations
//      ad      1/20/17             Add Bluetooth command: BT1 = activate Bluetooth, BT0 = deactivate Bluetooth, BT = Bluetooth Status
//      ae      2/6/17              Experiment with HSPLL configuration to boost processing speed and allow for faster communication baud rates (460800bps)
//                                  Include LC8004extFRAM_h.c,.h
//                                  Enable failsafe clock monitoring and clock failure trap
//      af      2/7/17              Incorporate BT_CONNECT signal into communications  
//                                  Clock switch to HS oscillator when entering float getFrequency(void) and switch back to HSPLL oscillator when exiting
//      ag      2/13/17             Add Bluetooth control under RTC same as output port
//      ah      3/3/17              Debug Start/Stop Logging and Control Port interaction
//      ba      3/21/17             Incorporate MODBUS communications switch <:><:><:><CR>. Reboot into MODBUS communications
//                                  rename Comm() CMDcomm()
//                                  Add MODBUSa.c,.h
//                                  Debug MODBUS t3,5 end-of-frame detection & t1,5 intercharacter timing
//                                  Add ":::"<CR> to enter MODBUS comms and bootup with stream of ";;;;;;;"etc to enter command-line comms
//      bb      3/21/17             Add CRC computation
//      bc      3/24/17             Add MODBUS slave addressing
//                                  include MODBUSb.c,.h
//                                  Make a separate FRAM_ADDRESSa.h file for external FRAM addressing and include
//      bd      3/29/17             Incorporate MODBUS Command decoding
//                                  Perform one complete MODBUS transaction (read MODBUS address)
//      be      4/4/17              Include FRAM_ADDRESSb.h to remap FRAM memory so common functions are contiguous
//      bf      4/11/17             Implement MODBUS Read & write holding registers
//      bg      4/17/17             Incorporate paging for MODBUS addressing of holding registers
//      bh      4/18/17             Change Seconds_Since_Midnight value that is stored in FRAM from type 32 bit float to type 32 bit unsigned long  
//      bi      4/20/17             Store -0.0 in FRAM addresses for disabled channel value
//      ca      4/24/17             Same as rev bi but uC pinout for 8004 rev 1 pcb
//      cb      5/2/17              Modify pluck routines for DRV8839 H-bridge VW excitation
//                                  Modify order of initial HSPLL clock setup at program startup. Add Nop()s between HSPLL register writes. Add waiting for PLL to lock
//                                  Add MODBUS Status register handling
//                                  Add MODBUS Enable/Disable Bluetooth
//      cc      5/2/17              Continue adding MODBUS Control functions (WF)(OP) (WF) (SR)
//      cd      5/2/17              Add MODBUS CFG selection
//      ce      5/3/17              Incorporate MODBUS 'X' command
//      cf      5/12/17              Reorder members of S_1 register (MODBUS STATUS1) so that CFG bits are least significant and X,Logging are MSB
//                                  include MODBUSh.h,.c
//                                  add START() and STOP() functions
//                                  Debug VW Read functions
//      cg      5/16/17             Add MODBUS Clock Read and Clock Write
//      ch      5/17/17             Remove all references to cmd line networking
//                                  Add MODBUS 'R', "RESET" & CMD_LINE switch 
//      ci      5/19/17             Debug Log Intervals
//      ci      5/23/17             Add MODBUS Enable/Disable Log Intervals, Enable/Disable Start Time & Enable/Disable Stop Time 
//                                  Change all instances of long int to unsigned long int
//                                  On Start Logging reload Log Intervals Table
//                                  Add default Log Interval Tables for each configuration
//                                  Make sure that any entered Log Interval does not violate the minimum scan interval for that configuration
//      cj      5/24/17             Add MODBUS BT Timer Enable/Disable and Control Port Timer Enable/Disable   
//      ck      5/30/17             Eliminate separate MODBUS .c & .h files and combine all into lc8004main .c & .h files. Linker is giving me problems
//                                  Add MODBUS scheduled Start/Stop logging functions
//      cl      6/5/17              MODBUS: Multiply incoming register address x2 to translate to absolute memory address
//                                  Debug MODBUX RTC issue - forgot that RTC is BCD and needs to be translated to straight binary
//                                  Add MODBUS Enable Start Logging Time
//      cm      6/5/17              Add MODBUS Enable Stop Logging Time
//      cn      6/9/17              Debug inadvertent cast from float to int32 when writing to/reading from FRAM of Zero Readings, Gage Factors, Gage Offsets & 
//                                  polynomial coefficients
//      co      6/9/17              include FRAM_ADDRESSe.h
//      cp      6/20/17             Add 15S timeout timer to MODBUS comms
//                                  (MODBUS) Password protect the baud rate,address,Hardware Rev,Firmware Rev and Serial ## registers
//                                  Remove R1-R0 or R0-R1 selection. Set fixed at R1-R0
//                                  Include FRAM_ADDRESSf.h
//                                  Add MODBUS Preset Multiple Registers (0x10) functionality
//      cq      6/26/17             Include FRAM_ADDRESSg.h
//                                  Provide for MODBUS write multiple 32 bit u.i. and floats
//      cr      7/13/17             Debug scan intervals in MODBUS
//      da      7/14/17             Read V_AGC while VW frequency is being counted
//                                  Add gage type 98 for V_AGC
//      db      7/28/17             Improve accuracy of VW measurement
//                                  Change FRAM reference designators during FRAM TEST to correspond with PCB-238 rev A layout
//      dc      8/1117              Adjust ENABLE & CLOCK timings to work properly with 8004-17 & 8004-18 muxes
//                                  Change Software Revision response from dc to 1.0 to enable testing with LogView
//      1.0     8/18/17             Add MS command to enable testing with LogView
//                                  debug start/stop logging time issue if memory was not reset
//                                  Change VW Reading gate from 256mS to 128mS
//      1.1     8/24/17             Store whole # portion of 1st VW reading in FRAM. Use narrow-band pluck (+/- 10% of reading) for subsequent plucks
//                                  Store whole # portion of each subsequent pluck.
//                                  If -99999 occurs start again with full spectrum pluck
//                                  Incorporate FRAM_ADDRESSh.h
//      1.2     10/09/17            Add command "RSN" to Recover Serial Number. Add serial number recovery from probe functionality
//                                  Add BTR (Bluetooth Reset) command
//      1.3     10/17/17            Continue work on RSN functionality
//                                  Add function u.i. take_fast_analog_reading(u.c.)
//      1.4     10/24/17            Debug ID
//                                  Restore register values when exiting Read Serial Number
//                                  Return frequency (Hz) instead of digits for testing
//                                  Add digital 5-point moving average filter in take_fast_analog_reading()
//      1.5     10/25/17            Read Vth(max) at beginning of probe serial number read to more accurately set the data threshold value, instead of hard coding it
//      1.6     10/31/17            Sample 20 times instead of 16 times in take_fast_analog_reading so that the unfiltered[] inputs to y[] are all accounted for
//                                  Improve accuracy of VW reading
//      1.7     11/02/17            Experiment with Fcy = 29.4912MHz during VW read and various gate times
//      1.8     11/13/17            Modify Thermistor types from 0,1,2 to 1,2,3
//                                  Set Thermistor type 0 as disable thermistor channel switch
//      1.9     11/17/17            Add Modbus capability to initiate and read a gage serial number read
//                                  Utilize STATUS/CONTROL2 bit 8 for this function (1=trigger serial number read)
//                                  32 bit result stored in memory locations 0x7FF74(MSW)(Modbus register 0x7FBA) and 0x7FF76(LSW) (Modbus register 0x7FBB)
//      1.10    11/22/17            Debug LOG Table
//                                  Cleanup (remove) rem'd out code
//                                  Add back WDT enable when start logging
//      1.11    11/27/17            Add automatic write to MODBUS firmware revision register the current firmware revision on bootup.
//                                  
//
//
//
//				VW GAGE TYPES				
//		GAGE TYPE			DESCRIPTION
//		0				DISABLE CHANNEL
//		1				MID FREQUENCY SWEEP, 1400-3500 Hz
//		2				HIGH FREQUENCY SWEEP, 2800-4500 Hz
//		3				VERY LOW FREQUENCY SWEEP, 400-1200 Hz
//		4				LOW FREQUENCY SWEEP, 1200-2800 Hz
//		5				VERY HIGH FREQUENCY SWEEP, 2500-4500 Hz
//      6               LOW FREQUENCY SWEEP, 800-1600 Hz
//
//				ANALOG GAGE TYPES
//		GAGE TYPE			DESCRIPTION
//		85				EXTERNAL TEMPERATURE (C)
//		86				INTERNAL TEMPERATURE (C)
//		87				MAIN BATTERY - 12V (V)
//		95				LITHIUM BATTERY (V)
//		97				MAIN BATTERY - 3V (V)

//				THERMISTOR TYPES												
//      0               DISABLE CHANNEL
//		1				STANDARD 3K @ 25C NTC	
//		2				BR55KA822J 8.2K @ 25C HIGH TEMP			
//		3				103JL1A 10K @ 25C HIGH TEMP				
							

//Resources:

//  TMR1:   Frequency Capture
//  TMR2:   Frequency Capture - used with OC1
//  TMR3:   Swept Frequency Pluck
//  TMR4:   Frequency Capture - Gate
//  TMR5:   Frequency Capture - Gate
//  TMR6:   Not Committed
//  TMR7:   Frequency Capture
//  TMR8:   Sleep Timer
//  TMR9:   Sleep Timer


//	The following files are included in the MPLAB project:
//	Z:\8004\LC8004main_x_x.c (main source: x is revision level)
//	Z:\8004\LC8004delay_b.c
//	Z:\8004\LC8004extFRAM_i.c
//  Z:\8004\AD5241a.c


//	Header Files:
//#include "p33FJ256GP710A.h"
//#include "LC8004extFRAM_i.h"                              
//#include "LC8004main_1_11.h"
//#include "LC8004delay_b.h"
//#include "AD5241b.h"
//#include "FRAM_ADDRESSh.h                                                     
//#include <outcompare.h>
//#include <ports.h>
//#include <timer.h>
//#include <reset.h>
//#include <stdlib.h>
//#include <InCap.h>
//#include <uart.h>
//#include <i2c.h>
//#include <string.h>
//#include <stdio.h>
//#include <adc.h>
//#include <math.h>
//#include <p33Fxxxx.h>
//#include <xc.h>
//#include <stdbool.h>                                                          


//--------------------------------------------------------------
//						Includes
//--------------------------------------------------------------
#include "p33FJ256GP710A.h"
#include "LC8004extFRAM_i.h"                                                    
#include "LC8004main_1_11.h"
#include "LC8004delay_b.h"                                                      
#include "AD5241b.h"                                                            
#include "FRAM_ADDRESSh.h"                                                      
#include <outcompare.h>
#include <ports.h>
#include <timer.h>
#include <reset.h>
#include <stdlib.h>
#include <InCap.h>
#include <uart.h>
#include <i2c.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>                                                            
#include <adc.h>
#include <math.h>
#include <p33Fxxxx.h>
#include <stdbool.h>                                                            
#include <xc.h>



//***************************************************************************
//						Main Program
//***************************************************************************

int main(void) 
{

    Nop();
    PLLFBDbits.PLLDIV=14;                                                       //PLL Feedback Divisor M value (x16)    
    Nop();
    CLKDIVbits.PLLPOST=0;                                                       //PLL Postcaler N2 value (DIV/2)    
    Nop();
    CLKDIVbits.PLLPRE=0;                                                        //PLL Prescaler N1 value (DIV/2)    
    Nop();
    while(!OSCCONbits.LOCK){};                                                  //Wait for PLL to lock  
    setup();                                 
    
    
    restoreSettings();                                                          //reload the settings from FRAM    
    
    //LC2CONTROL.flags.Logging=0;                                               REM REV 1.10  
    //write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                   REM REV 1.10  
    
    //LC2CONTROL2.flags2.scheduled=0;                                           REM REV 1.10  
    //write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);                  //store flag in FRAM REM REV 1.10
    //stopLogging();                                                              
    
    SLEEP12V = 0;                                                               //Set 12V regulator into switchmode
    wait2S();                                                                   //provide a 2S delay to allow DS3231 to stabilize

//***************************TEST FOR 3V or 12V BATTERY*****************************************************************
    //Determine whether 3V or 12V battery is connected
    mainBatreading=take_analog_reading(87);                                     //test the 12V_SENSE input
                   
    if (mainBatreading > V0_5)                                                  //12V_SENSE >0.5V, so connected 
    {
        S_2.status2flags._BAT=1;                                                //Set the _BAT flag           
        batteryReading=(((Vref*mainBatreading)/4096)*mul12V);
        if(batteryReading>=bat_12LOW)                                           
            Blink(12);                                                          //Battery good
        else
            Blink(1);                                                           //Battery low
    } 
    else 
    {
        S_2.status2flags._BAT=0;                                                //Clear the _BAT flag     
        mainBatreading=take_analog_reading(97);                                 //take the 3V battery reading
        batteryReading=(((Vref*mainBatreading)/4096)*mul3V);
        if (batteryReading>=bat_3LOW)                                              
            Blink(3);                                                           //Battery good
        else
            Blink(1);                                                           //Battery low

    }
    write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);                          
    
    loadSV();                                                                   //Load the firmware revision into Modbus register 7D84 (Firmware Revision)    REV 1.11    
    
    baudrate = read_Int_FRAM(baudrateaddress);
    if ((baudrate != brg9600) && (baudrate != brg115200) && (baudrate != brg230400) && (baudrate != brg460800)) //FRAM does not contain valid baud rate value  
    {
        baudrate = brg115200;                                                   //set initial baud rate to 115200 bps           
        write_Int_FRAM(baudrateaddress, baudrate);                              //store baudrate in FRAM   
    }
    
    MODBUSaddressvalue=read_Int_FRAM(MODBUSaddress);
    if(MODBUSaddressvalue<1 | MODBUSaddressvalue>247)
    {
        MODBUSaddressvalue=1;                                                   //Initialize MODBUS address to 1
        write_Int_FRAM(MODBUSaddress,MODBUSaddressvalue);                       //store in FRAM
    }
    
    configUARTnormal();                                                         
    IFS0bits.U1RXIF = 0;                                                        //clear the UART1 interrupt flag
    
    if(LC2CONTROL2.flags2.Modbus)                                               //Test if need to switch back to Command Line interface 
    {
       shutdownTimer(2);                                                        //start 2 second timer
       for(ModbusTestIdx=0;ModbusTestIdx<10;ModbusTestIdx++)                    //Powerup with ';' being entered while rebooting
       {
           while(!DataRdyUART1() && !IFS3bits.T9IF);
           if(IFS3bits.T9IF)                                                    //timed out
           {
               IFS3bits.T9IF=0;                                                 //clear the T9 interrupt flag
               break;                                                           //breakout & continue with MODBUS communications
           }
           IFS0bits.U1RXIF = 0;                                                 //clear the UART1 interrupt flag
           RxData=ReadUART1();                                                  //Test for ';' being entered
           if(U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
               handleCOMError();

           if(RxData==semicolon)
           {
               CMD_LINE();                                                      //Reboot into command line interface    
           }
       }
    }
    
    
    configShutdownTimer();                                                      
  
    
    if(!LC2CONTROL2.flags2.Modbus)                                              
        testReset();
    
    if (isWDTTO())                                                              //Did WDT timeout during VW read?		
    {
        MUX_CLOCK = 0;                                                          //set the MUX_CLOCK line low
        MUX_RESET = 0;                                                          //set the MUX_RESET line low
        trap = read_Int_FRAM(TrapRegisteraddress);
        trap += 1;                                                              //increment trap counter if it did
        write_Int_FRAM(TrapRegisteraddress, trap);                              //store trap value in FRAM  
        if(!LC2CONTROL2.flags2.Modbus)                                          
        {
            putsUART1(VWTimeout);                                               //"VW Timeout Occurred!"
            while (BusyUART1());
            crlf();
        }
    }

    if(DISPLAY_CONTROL.flags.BT)                                                //Enable Bluetooth if previously enabled    
        enableBT();
    
    if(PORT_CONTROL.flags.ControlPortON)                                        //Turn on Control Port if previously on     
    {
        CONTROL = 1; //turn on control port
        _READ=0;                                                                //LED ON            
    }
    
    if (LC2CONTROL.flags.Reset && !LC2CONTROL2.flags2.Modbus)                   
    {
        putsUART1(ResetComplete);                                               //Display "Reset Complete" if RESET command was previously issued
        while (BusyUART1());
        prompt();
        LC2CONTROL.flags.Reset = 0;                                             //Clear the Reset flag
        write_Int_FRAM(LC2CONTROLflagsaddress, LC2CONTROL.full);                //store flag in FRAM  
    }


    if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting)                //was logger previously logging?
    {
        if (DISPLAY_CONTROL.flags.Synch)
            VWflagsbits.synch = 1;                                              //set the synch flag
        LC2CONTROL2.flags2.FirstReading = 1;                                    //clear the first reading flag
        write_Int_FRAM(LC2CONTROL2flagsaddress, LC2CONTROL2.full2);             //store flags in FRAM 
        startLogging();                                                         //yes                                                 
    }
    
    DISPLAY_CONTROL.flags.TakingReading = 0;
    DISPLAY_CONTROL.flags.Shutdown = 0;
    shutdownTimer(TimeOut);                                                     //Reset 15S timer       
    
    ///*TEST REM
    if(LC2CONTROL2.flags2.Modbus)                                               //if MODBUS 
        _RS485RX_EN=0;                                                          //enable RS485 Rx   
    else                                                                        
        _RS485RX_EN=1;                                                          //disable RS485 Rx if not   
    
    
    DISPLAY_CONTROL.flags.Shutdown=1;                                           
    IFS3bits.T9IF=1;                                                            
    shutdown();                                                                 

    while (1)                                                                   //infinite loop
    {
        Nop();
        while (!IFS3bits.T9IF) 
        {
            if(LC2CONTROL2.flags2.Modbus)                                       
            {
               MODBUScomm();                                                         
            }
            else                                                                
            {
               CMDcomm();
            }
        }

        IFS0bits.U1RXIF = 0;                                                    //clear the UART1 interrupt flag
        if(DISPLAY_CONTROL.flags.Shutdown | IFS3bits.T9IF)                                      
        {
            IFS3bits.T9IF=0;                                                    //clear the T5 interrupt flag
            shutdown();
        }

        LC2CONTROL2.flags2.ON = 0;
        write_Int_FRAM(LC2CONTROL2flagsaddress, LC2CONTROL2.full2);             //store flag in FRAM  
    }                                                                           //end of while(1)
}                                                                               //end of main()



//******************************************************************************************************
//			Functions
//******************************************************************************************************

//***************************************************************************
//	void _3VX_on(void)
//
//	turns on the +3.3VX analog circuitry
//
//	Parameters received: none
//	Returns: nothing
//
//	Called from: main()
//
//***************************************************************************

void _3VX_on(void) 
{
    PMD3bits.I2C2MD=0;                                                          //Enable the I2C2 module for AD5241 communications  
    V3_X_CONTROL = 1;                                                           //turn on +3VX
    delay(294920);                                                              //10mS delay    
    SCL_VW=1;                                                                   //drive high    
    SDA_VW=1;                                                                   //drive high    
}

//***************************************************************************
//	void _3VX_off(void)
//
//	turns off the +3.3VX analog circuitry
//
//	Parameters received: none
//	Returns: nothing
//
//	Called from: main()
//
//***************************************************************************

void _3VX_off(void) 
{
    write_AD5241(0);                                                            //reset the gain setting to 1   
    PMD3bits.I2C2MD=1;                                                          //Disable the I2C2 module
    TRISAbits.TRISA2=0;                                                         //Configure RA2 (SCL_VW) as digital output    
    LATAbits.LATA2=0;                                                           //drive low       
    TRISAbits.TRISA3=0;                                                         //Configure RA3 (SCL_VW) as digital output    
    LATAbits.LATA3=0;                                                           //drive low   
    V3_X_CONTROL = 0;                                                           //shut off +5VX
}


//***************************************************************************
//	unsigned long bcd2d(unsigned char)
//
//	converts 2 digit bcd to decimal
//
//	Parameters received: 8 bit bcd char to be converted
//	Returns: 32 bit decimal
//
//	Called from:
//
//***************************************************************************

unsigned long bcd2d(unsigned char bcd) {
    unsigned long x;

    x = bcd;

    if (bcd >= 0 && bcd <= 0x09)                                                //convert 2 digit bcd to decimal
        return x;

    if (bcd >= 0x10 && bcd <= 0x19)
        return x - 6;

    if (bcd >= 0x20 && bcd <= 0x29)
        return x - 12;

    if (bcd >= 0x30 && bcd <= 0x39)
        return x - 18;

    if (bcd >= 0x40 && bcd <= 0x49)
        return x - 24;

    if (bcd >= 0x50 && bcd <= 0x59)
        return x - 30;

    if (bcd >= 0x60 && bcd <= 0x69)
        return x - 36;

    if (bcd >= 0x70 && bcd <= 0x79)
        return x - 42;

    if (bcd >= 0x80 && bcd <= 0x89)
        return x - 48;

    if (bcd >= 0x90 && bcd <= 0x99)
        return x - 54;

    return 0;
}

void Blink(unsigned char times) 
{
    unsigned char t = 0;                                                        //cycle loop index

    for (t=0; t < times; t++) 
    {
        if(_READ)                                                               
        {
            _READ = 0;                                                          //Light the LED
            delay(80000);                                                        
            _READ=1;                                                            //Off the LED
            delay(320000);                                                       
        }
        else
        {   
            _READ=1;                                                            //Off the LED
            delay(80000);                                                       
            _READ = 0;                                                          //Light the LED
            delay(320000);                                                      
        }
    }

}

void BT_D(void)                                                                 
{
    PORT_CONTROL.flags.BluetoothTimerEN = 0;                                    //clear Bluetooth Timer enable flag
    write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);              //store flag in FRAM`
    S_1.status1flags._BT_Timer=0;                                                //clear the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
    disableAlarm(Alarm2);                                                       //disable the Alarm2 interrupt		
}

void BT_E(void)                                                                  
{
    PORT_CONTROL.flags.BluetoothTimerEN = 1;                                    //set Bluetooth Timer enable flag
    PORT_CONTROL.flags.PortTimerEN=0;                                           //clear the Control Port Timer enable flag
    PORT_CONTROL.flags.BTTime = 0;                                              //clear the BTtime flag
    write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);              //store flag in FRAM 
    S_1.status1flags._BT_Timer=1;                                                //set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
    delay(4000);                                                                //delay required? 
    enableAlarm(Alarm2);                                                        //enable the Alarm2 interrupt    
}

void BT_R(void)                                                                 
{
    unsigned char b = 0;
    
    crlf();
    putsUART1(Resetting);
    while (BusyUART1());    
    
    _BT_FR=1;                                                                   //raise BT_FR line high
    delay(4000);    
    enableBT();                                                                 //Turn on BT module
    for(b=0;b<40;b++)
    {
    __delay32(mS100);                                                           //1S delay while _BT_RESET is high   
    }
    
    _BT_FR=0;                                                                   //bring BT_FR low
    for(b=0;b<40;b++)
    {
    __delay32(mS100);                                                           //1S delay while _BT_RESET is low   
    } 
    
    _BT_FR=1;                                                                   //raise BT_FR line high again
    for(b=0;b<40;b++)
    {
    __delay32(mS100);                                                           //1S delay while _BT_RESET is high   
    }    
    
    _BT_FR=0;                                                                   //bring BT_FR low
    
    crlf();
    putsUART1(ResetComplete);
    while(BusyUART1());
    
    disableBT();                                                                //Turn off BT module
    
}

void BTStatus(void)                                                             
{
    char bbBUF[6];                                                              //temporary storage for BB values
   
    
    DISPLAY_CONTROL.display=read_Int_FRAM(DISPLAY_CONTROLflagsaddress);         //Read the BT flag
    PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress); 
    crlf();
    if(DISPLAY_CONTROL.flags.BT)
    {
        putsUART1(BTON);                                                        //Display BT Enabled
    }
    else
    {
        putsUART1(BTOFF);                                                       //Display BT Disabled
    }
    while(BusyUART1());
    
    crlf();

    if (PORT_CONTROL.flags.BluetoothTimerEN)                                    //Bluetooth Timer Enabled
    {
        putsUART1(BTTIMEREN);                                                   //"Bluetooth Timer Enabled."
    } else {
        putsUART1(BTTIMERDIS);                                                  //"Bluetooth Timer Disabled."
    }
    while (BusyUART1()); 
    
    crlf();

    putsUART1(CPTIMERACT);                                                      //"Timer activate at "
    while (BusyUART1());

    PortOnHours=read_Int_FRAM(PortOnHoursaddress);   
    if (PortOnHours < 10)                                                       //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(bbBUF, "%d", PortOnHours);                                          //format the hour data
    putsUART1(bbBUF);                                                           //display it
    while (BusyUART1());
    putcUART1(colon);                                                           // :
    while (BusyUART1());
    PortOnMinutes=read_Int_FRAM(PortOnMinutesaddress);   
    if (PortOnMinutes < 10)                                                     //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(bbBUF, "%d", PortOnMinutes);                                        //format the minute data
    putsUART1(bbBUF);                                                           //display it
    while (BusyUART1());

    crlf();

    putsUART1(CPTIMERDEACT);                                                    //"Timer deactivate at "
    while (BusyUART1());
    PortOffHours=read_Int_FRAM(PortOffHoursaddress); 
    if (PortOffHours < 10)                                                      //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(bbBUF, "%d", PortOffHours);                                         //format the hour data
    putsUART1(bbBUF);                                                           //display it
    while (BusyUART1());
    putcUART1(colon);                                                           // :
    while (BusyUART1());
    PortOffMinutes=read_Int_FRAM(PortOffMinutesaddress); 
    if (PortOffMinutes < 10)                                                    //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(bbBUF, "%d", PortOffMinutes);                                       //format the minute data
    putsUART1(bbBUF);                                                           //display it
    while (BusyUART1());
    crlf();   

}

void BTONE(void)                                                                
{
    enableBT();  
    PORT_CONTROL.flags.BluetoothON = 1;
    PORT_CONTROL.flags.B0issued = 0;                                            //clear B0issued flag
    write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);              //store flag in FRAM  
    S_1.status1flags._BT=1;                                                     //set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                             
}


void BTZERO(void)                                                               
{
    disableBT();                                                                //Turn off bluetooth
    if (!PORT_CONTROL.flags.BTTime)                         
        PORT_CONTROL.flags.BluetoothON = 0;                                     //clear flag if not in scheduled ON time
    PORT_CONTROL.flags.B0issued = 1;                                            //set B0issued flag
    write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);              //store flag in FRAM  
    S_1.status1flags._BT=0;                                                     //clear the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                              
}

void Buf2DateTime(char buffer[]) {

    unsigned char SelectionLoop;
    unsigned char temp = 0;
    unsigned char tens = 0;
    unsigned char ones = 0;
    unsigned char index = 2;

    struct RTCFlag {
        int monthsChanged : 1;
        int daysChanged : 1;
        int yearsChanged : 1;
        int hoursChanged : 1;
        int minutesChanged : 1;
        int secondsChanged : 1;
        int firstpass : 1;
    } RTCFlagBits;

    RTCFlagBits.monthsChanged = 0;                                              //initialize flags
    RTCFlagBits.daysChanged = 0;
    RTCFlagBits.yearsChanged = 0;
    RTCFlagBits.hoursChanged = 0;
    RTCFlagBits.minutesChanged = 0;
    RTCFlagBits.secondsChanged = 0;
    RTCFlagBits.firstpass = 1;

    RTCseconds = 0;                                                             //initialize RTCseconds to 0
    LC2CONTROL.flags.ERROR = 0;                                                 //clear the ERROR flag

    for (SelectionLoop = 1; SelectionLoop < 7; SelectionLoop++) {
        for (index; buffer[index] != cr; index++)                               //parse the buffer for data
        {
            temp = buffer[index];                                               //get the char at buffer[index]

            if (isalpha(temp))                                                  //don't allow any letters
            {
                LC2CONTROL.flags.ERROR = 1;                                     //set the ERROR flag
                return;
            }


            if (LC2CONTROL2.flags2.SetStartTime | LC2CONTROL2.flags2.SetStopTime | PORT_CONTROL.flags.SetAlarm2Time | PORT_CONTROL.flags.SetAlarm2StopTime) {
                if (index == 2 && (temp > two | temp < zero))                   //don't allow hour(tens) > 2
                {
                    LC2CONTROL.flags.ERROR = 1;                                 //set the ERROR flag
                    return;
                }

                if (index == 3 && (((buffer[2] == two) && (temp > three | temp < zero))//don't allow hour(ones) > 3 
                        | ((buffer[2] == one) && (temp > nine | temp < zero)))) {
                    LC2CONTROL.flags.ERROR = 1;                                 //set the ERROR flag
                    return;
                }

                if (index == 4 && temp != colon)                                //don't allow char other than colon 
                {
                    LC2CONTROL.flags.ERROR = 1;                                 //set the ERROR flag
                    return;
                }

                if (index == 5 && (temp > five | temp < zero))                  //don't allow minute(tens) > 5
                {
                    LC2CONTROL.flags.ERROR = 1;                                 //set the ERROR flag
                    return;
                }

                if (index == 6 && (temp > nine | temp < zero))                  //don't allow minute(ones) > 9
                {
                    LC2CONTROL.flags.ERROR = 1;                                 //set the ERROR flag
                    return;
                }

                if (buffer[7] != cr) {
                    if (index == 7 && (temp != colon))                          //don't allow char other than colon
                    {
                        LC2CONTROL.flags.ERROR = 1;                             //set the ERROR flag
                        return;
                    }

                    if (index == 8 && (temp > five | temp < zero)) {
                        LC2CONTROL.flags.ERROR = 1;                             //set the ERROR flag
                        return;
                    }

                    if (index == 9 && (temp > nine | temp < zero)) {
                        LC2CONTROL.flags.ERROR = 1;                             //set the ERROR flag
                        return;
                    }

                    if (index == 10 && (temp != cr)) {
                        LC2CONTROL.flags.ERROR = 1;                             //set the ERROR flag
                        return;
                    }
                }
            }

            switch (SelectionLoop)                                              //get the value from the array
            {
                case 1:
                case 2:
                case 3:
                    if (!isdigit(temp) && temp != slash && temp != colon && temp != nul) 
                        return;                                                 //not valid digit,slash or colon, so exit
                    break;
                case 4:
                case 5:
                    if (!isdigit(temp) && temp != colon) 
                        return;                                                 //not valid digit or colon, so exit
                    break;
                case 6:
                    if (!isdigit(temp) && temp != colon) 
                        return;
                    break;
                default:
                    return;
            }



            if (temp == nul)                                                    //convert nul char to ascii zero
                temp = zero;

            if ((SelectionLoop < 4 && temp == slash) | (SelectionLoop >= 4 && temp == colon) |
                    (LC2CONTROL.flags.LoggingStartTime && temp == colon) |
                    (LC2CONTROL2.flags2.SetStopTime && temp == colon) |
                    (PORT_CONTROL.flags.SetAlarm2Time && temp == colon) |
                    (PORT_CONTROL.flags.SetAlarm2StopTime && temp == colon) |
                    (LC2CONTROL2.flags2.SetStopTime && SelectionLoop == 3 && temp == zero && RTCFlagBits.firstpass == 1) |
                    (LC2CONTROL.flags.LoggingStartTime && SelectionLoop == 3 && temp == zero && RTCFlagBits.firstpass == 1) |
                    (PORT_CONTROL.flags.SetAlarm2Time && SelectionLoop == 3 && temp == zero && RTCFlagBits.firstpass == 1) |
                    (PORT_CONTROL.flags.SetAlarm2StopTime && SelectionLoop == 3 && temp == zero && RTCFlagBits.firstpass == 1)) break;

            temp = temp - 0x30;                                                 //convert to decimal

            if (RTCFlagBits.firstpass)
                ones = temp;
            else {
                tens = 10 * (ones);                                             //buffer[2] was tens
                ones = temp;                                                    //buffer[3] is ones 
            }

            switch (SelectionLoop) {
                case 1:

                    if (!LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL2.flags2.SetStopTime && !PORT_CONTROL.flags.SetAlarm2Time && !PORT_CONTROL.flags.SetAlarm2StopTime) {
                        RTCmonths = tens + ones;
                        if ((RTCmonths > 12 | RTCmonths == 0) && !RTCFlagBits.firstpass) return; //invalid	
                        RTCFlagBits.monthsChanged = 1;                          //set the months changed flag
                        break;
                    } else {
                        if (PORT_CONTROL.flags.SetAlarm2StopTime) {
                            PortOffHours = tens + ones;
                            if ((PortOffHours > 23) && !RTCFlagBits.firstpass) {
                                return;                                         //invalid
                            }
                            break;
                        }

                        if (LC2CONTROL2.flags2.SetStopTime) {
                            LoggingStopHours = tens + ones;
                            if ((LoggingStopHours > 23) && !RTCFlagBits.firstpass) return; //invalid
                        } else {
                            RTChours = tens + ones;
                            if ((RTChours > 23) && !RTCFlagBits.firstpass) return; //invalid
                        }

                        if (!LC2CONTROL2.flags2.SetStopTime && !PORT_CONTROL.flags.SetAlarm2Time && !PORT_CONTROL.flags.SetAlarm2StopTime) //if logging stop time is not being extracted from buffer						
                        {
                            setClock(RTCAlarm1HoursAddress, RTChours);          //load the Start Time Hours into the RTC
                            if(DISPLAY_CONTROL.flags.firstTime)                 
                                write_Int_FRAM(startHoursaddress,RTChours);     //store first time in FRAM 
                        }

                        if (PORT_CONTROL.flags.SetAlarm2Time) {
                            setClock(RTCAlarm2HoursAddress, RTChours);          //load the Start Time Hours into the RTC
                            write_Int_FRAM(PortOnHoursaddress,RTChours);        //store Port ON hours in FRAM 
                        }
                        break;
                    }

                case 2:

                    if (!LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL2.flags2.SetStopTime && !PORT_CONTROL.flags.SetAlarm2Time && !PORT_CONTROL.flags.SetAlarm2StopTime) {
                        RTCdays = tens + ones;
                        if ((RTCdays > 31 | RTCdays == 0) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.daysChanged = 1;                            //set the days changed flag
                        break;
                    } else {
                        if (PORT_CONTROL.flags.SetAlarm2StopTime) {
                            PortOffMinutes = tens + ones;
                            if ((PortOffMinutes > 59) && !RTCFlagBits.firstpass) {
                                return;                                         //invalid
                            }
                            break;
                        }

                        if (LC2CONTROL2.flags2.SetStopTime) {
                            LoggingStopMinutes = tens + ones;
                            if ((LoggingStopMinutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        } else {
                            RTCminutes = tens + ones;
                            if ((RTCminutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        }

                        if (!LC2CONTROL2.flags2.SetStopTime && !PORT_CONTROL.flags.SetAlarm2Time)//if logging stop time is not being extracted from buffer
                        {
                            setClock(RTCAlarm1MinutesAddress, RTCminutes);      //load the Start Time Minutes into the RTC
                            RTCseconds = 0;
                            setClock(RTCAlarm1SecondsAddress, RTCseconds);
                            if(DISPLAY_CONTROL.flags.firstTime)                 
                            {
                                write_Int_FRAM(startMinutesaddress,RTCminutes); //store in FRAM     
                                write_Int_FRAM(startSecondsaddress,RTCseconds); //store in FRAM     
                            }
                        }

                        if (PORT_CONTROL.flags.SetAlarm2Time) {
                            RTCminutes = tens + ones;
                            if ((PortOffMinutes > 59) && !RTCFlagBits.firstpass) {
                                return;                                         //invalid
                            }
                            setClock(RTCAlarm2MinutesAddress, RTCminutes);      //load the Control Port RTC Alarm2 minutes into the RTC
                            write_Int_FRAM(PortOnMinutesaddress,RTCminutes);	//store Port ON minutes in FRAM   
                        }
                        break;
                    }

                case 3:

                    if (!LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL2.flags2.SetStopTime && !PORT_CONTROL.flags.SetAlarm2Time && !PORT_CONTROL.flags.SetAlarm2StopTime) {
                        RTCyears = tens + ones;
                        if ((RTCyears > 99) && !RTCFlagBits.firstpass) return;  //invalid
                        RTCFlagBits.yearsChanged = 1;                           //set the years changed flag
                        break;
                    } else {
                        if (LC2CONTROL2.flags2.SetStopTime) {
                            LoggingStopSeconds = tens + ones;
                            if ((LoggingStopSeconds > 59) && !RTCFlagBits.firstpass) return; //invalid
                        } else {
                            RTCseconds = tens + ones;
                            if ((RTCseconds > 59) && !RTCFlagBits.firstpass) {
                                return;                                         //invalid
                            }
                        }

                        if (!LC2CONTROL2.flags2.SetStopTime)                    //if logging stop time is not being extracted from buffer
                        {
                            setClock(RTCAlarm1SecondsAddress, RTCseconds);      //load the Start Time Seconds into the RTC
                            if(DISPLAY_CONTROL.flags.firstTime)                 
                                write_Int_FRAM(startSecondsaddress,RTCseconds); //store in FRAM 
                        }

                        if (temp == nul && !RTCFlagBits.firstpass) return;
                        break;
                    }

                case 4:

                    if (!LC2CONTROL2.flags2.SetStopTime) {
                        RTChours = tens + ones;
                        if ((RTChours > 23) && !RTCFlagBits.firstpass) return;  //invalid
                        RTCFlagBits.hoursChanged = 1;                           //set the hours changed flag
                        break;
                    }

                    if (PORT_CONTROL.flags.SetAlarm2StopTime) {
                        PortOffHours = tens + ones;
                        if ((PortOffHours > 23) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.hoursChanged = 1;                           //set the hours changed flag
                        break;
                    } else {
                        LoggingStopHours = tens + ones;
                        if ((LoggingStopHours > 23) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.hoursChanged = 1;                           //set the hours changed flag
                        break;
                    }

                case 5:

                    if (!LC2CONTROL2.flags2.SetStopTime) {
                        RTCminutes = tens + ones;
                        if ((RTCminutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.minutesChanged = 1;                         //set the minutes changed flag
                        break;
                    }

                    if (PORT_CONTROL.flags.SetAlarm2StopTime) {
                        PortOffMinutes = tens + ones;
                        if ((PortOffMinutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.minutesChanged = 1;                         //set the hours changed flag
                        break;
                    } else {
                        LoggingStopMinutes = tens + ones;
                        if ((LoggingStopMinutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.minutesChanged = 1;                         //set the minutes changed flag
                        break;
                    }

                case 6:

                    if (!LC2CONTROL2.flags2.SetStopTime) {
                        RTCseconds = tens + ones;
                        if ((RTCseconds > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.secondsChanged = 1;                         //set the seconds changed flag
                        break;
                    } else {
                        LoggingStopSeconds = tens + ones;
                        if ((LoggingStopSeconds > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.secondsChanged = 1;                         //set the seconds changed flag
                        break;
                    }

                default:
                    return;
            }

            RTCFlagBits.firstpass = 0;
        }                                                                       //end of for(index)

        index++;
        RTCFlagBits.firstpass = 1;
        tens = 0;
        ones = 0;
    }                                                                           //end of for(SelectionLoop)

    if (PORT_CONTROL.flags.SetAlarm2Time) {
        setClock(RTCAlarm2HoursAddress, RTChours);                              //load the Control Port RTC Alarm2 Hours into the RTC
        write_Int_FRAM(PortOnHoursaddress,RTChours);                            //store Port ON hours in FRAM		
    }

    if (!LC2CONTROL2.flags2.SetStopTime)                                        //don't load RTC if logging stop time is being extracted from buffer
    {


        if (RTCFlagBits.monthsChanged)                                          //if the user entered new month info
            setClock(RTCMonthsAddress, RTCmonths);                              //load it into the RTC

        if (RTCFlagBits.daysChanged)                                            //if the user entered new day info
            setClock(RTCDaysAddress, RTCdays);                                  //load it into the RTC

        if (RTCFlagBits.yearsChanged)                                           //if the user entered new year info
            setClock(RTCYearsAddress, RTCyears);                                //load it into the RTC

        if (RTCFlagBits.hoursChanged && !LC2CONTROL.flags.LoggingStartTime)     //if the user entered new hour info for current time
            setClock(RTCHoursAddress, RTChours);                                //load it into the RTC

        if (RTCFlagBits.hoursChanged && LC2CONTROL.flags.LoggingStartTime)      //if the user entered new hour info for Start Logging
        {
            setClock(RTCAlarm1HoursAddress, RTChours);                          //load it into the RTC
            enableAlarm(Alarm1);
        }

        if (RTCFlagBits.minutesChanged && !LC2CONTROL.flags.LoggingStartTime)   //if the user entered new minute info
            setClock(RTCMinutesAddress, RTCminutes);                            //load it into the RTC

        if (RTCFlagBits.minutesChanged && LC2CONTROL.flags.LoggingStartTime) {
            setClock(RTCAlarm1MinutesAddress, RTCminutes);
            enableAlarm(Alarm1);
        }

        if (RTCFlagBits.secondsChanged)                                         //if the user entered new seconds info
            setClock(RTCSecondsAddress, RTCseconds);                            //load it into the RTC

        crlf();

        putsUART1(Date);                                                        //Date:
        while (BusyUART1());

        RTCdata = readClock(RTCMonthsAddress);                                  //get the month from the RTC
        displayClock(RTCdata);                                                  //display it

        putcUART1(slash);                                                       // '/'
        while (BusyUART1());

        RTCdata = readClock(RTCDaysAddress);                                    //get the day from the RTC
        displayClock(RTCdata);                                                  //display it

        putcUART1(slash);                                                       // '/'
        while (BusyUART1());

        RTCdata = readClock(RTCYearsAddress);                                   //get the year from the RTC
        displayClock(RTCdata);                                                  //display it

        putsUART1(Time);                                                        //Time:
        while (BusyUART1());

        RTCdata = readClock(RTCHoursAddress);                                   //get the hour from the RTC
        displayClock(RTCdata);                                                  //display it

        putcUART1(colon);                                                       // ':'
        while (BusyUART1());

        RTCdata = readClock(RTCMinutesAddress);                                 //get the minute from the RTC
        displayClock(RTCdata);                                                  //display it

        putcUART1(colon);                                                       // ':'
        while (BusyUART1());

        RTCdata = readClock(RTCSecondsAddress);                                 //get the second from the RTC
        displayClock(RTCdata);                                                  //display it
    }
}

unsigned long Buffer2Decimal(char buffer[], unsigned int i, unsigned int x) {
    unsigned long num = 0;
    unsigned int index = 0;
    unsigned long temp;
    unsigned int loopcount = 0;
    unsigned int comp = 0;                                                      //compensation for 2 letter command "SC" and "NA"

    //unsigned int i is # of char in buffer
    //unsigned int x is switch: x=0 for pointer value
    //							x=1 for Scan Interval value
    //							x=2 for Network Address value


    if (buffer[index] == capB && i == 2)                                        //just 'B' was entered
        return 1;                                                               //so backup user position by 1

    if (x == 1 | x == 2)                                                        //adjust for "SC" or "NA" in buffer
        comp = 1;

    for (index = i - 2; index > comp; index--) {
        if (x == 0)                                                             //pointer information being entered
        {
            if (index > 5)
                return;                                                         //99999 maximum!
        }

        if (x == 1)                                                             //scan interval being entered
        {
            if (index > 6) {
                return 86400;                                                   //86400 maximum!
            }
        }

        temp = buffer[index];                                                   //parse the buffer

        if (!isdigit(temp)) {
            LC2CONTROL.flags.ScanError = 1;                                     //set the error flag
            return;                                                             //return if not valid digit
        }

        if (index == 2 && (x == 1 | x == 2))                                    //SC0, NA0 is invalid
        {
            if (temp == 0x30)
                return 0;                                                       //ERROR
        }

        switch (loopcount) {
            case 0:                                                             //ones
                num = temp - 0x30;
                break;

            case 1:
                num += 10 * (temp - 0x30);                                      //tens
                break;

            case 2:
                num += 100 * (temp - 0x30);                                     //hundreds
                break;

            case 3:
                num += 1000 * (temp - 0x30);                                    //thousands
                break;

            case 4:
                num += 10000 * (temp - 0x30);                                   //ten-thousands
                break;

            default:
                break;
        }

        loopcount++;
    }

    if (buffer[0] == capP && (num > (maxSingleVW - 1) | num == 0))              //invalid pointer positions    
        return;                                                                 //so ignore

    if (num > 86400)                                                            //Scan Interval greater than 1 day?
        return 86400;                                                           //limit it to 1 day

    return num;                                                                 //return the decimal value
}

void Buf2GageData(char buffer[]) {
    unsigned char SelectionLoop;
    unsigned char temp;
    unsigned char tens = 0;
    unsigned char ones = 0;
    unsigned char index = 2;

    struct RTCFlag {
        int monthsChanged : 1;
        int daysChanged : 1;
        int yearsChanged : 1;
        int hoursChanged : 1;
        int minutesChanged : 1;
        int secondsChanged : 1;
        int firstpass : 1;
    } RTCFlagBits;

    RTCFlagBits.monthsChanged = 0;                                              //initialize flags
    RTCFlagBits.daysChanged = 0;
    RTCFlagBits.yearsChanged = 0;
    RTCFlagBits.hoursChanged = 0;
    RTCFlagBits.minutesChanged = 0;
    RTCFlagBits.secondsChanged = 0;
    RTCFlagBits.firstpass = 1;

    for (SelectionLoop = 1; SelectionLoop < 7; SelectionLoop++) {
        for (index; buffer[index] != cr; index++)                               //parse the buffer for data
        {
            temp = buffer[index];                                               //get the char at buffer[index]

            switch (SelectionLoop)                                              //get the value from the array
            {
                case 1:
                case 2:
                case 3:
                    if (!isdigit(temp) && temp != slash && temp != colon) return; //not valid digit,slash or colon, so exit
                    break;
                case 4:
                case 5:
                    if (!isdigit(temp) && temp != colon) return;                //not valid digit or colon, so exit
                    break;
                case 6:
                    if (!isdigit(temp) && temp != colon) return;
                    break;
                default:
                    return;
            }

            if ((SelectionLoop < 4 && temp == slash) | (SelectionLoop >= 4 && temp == colon) | (LC2CONTROL.flags.LoggingStartTime && temp == colon)) break;

            temp = temp - 0x30;                                                 //convert to decimal

            if (RTCFlagBits.firstpass)
                ones = temp;
            else {
                tens = 10 * (ones);                                             //buffer[2] was tens
                ones = temp;                                                    //buffer[3] is ones 
            }

            switch (SelectionLoop) {
                case 1:
                    if (!LC2CONTROL.flags.LoggingStartTime) {
                        RTCmonths = tens + ones;
                        if ((RTCmonths > 12 | RTCmonths == 0) && !RTCFlagBits.firstpass) return; //invalid	
                        RTCFlagBits.monthsChanged = 1;                          //set the months changed flag
                        break;
                    } else {
                        RTChours = tens + ones;
                        if ((RTChours > 23) && !RTCFlagBits.firstpass) return;  //invalid
                        if (!LC2CONTROL2.flags2.SetStopTime)                    //if logging stop time is not being extracted from buffer
                            setClock(RTCAlarm1HoursAddress, RTChours);          //load the Start Time Hours into the RTC
                        break;
                    }

                case 2:
                    if (!LC2CONTROL.flags.LoggingStartTime) {
                        RTCdays = tens + ones;
                        if ((RTCdays > 31 | RTCdays == 0) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.daysChanged = 1;                            //set the days changed flag
                        break;
                    } else {
                        RTCminutes = tens + ones;
                        if ((RTCminutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        if (!LC2CONTROL2.flags2.SetStopTime)                    //if logging stop time is not being extracted from buffer
                            setClock(RTCAlarm1MinutesAddress, RTCminutes);      //load the Start Time Minutes into the RTC
                        break;
                    }

                case 3:
                    if (!LC2CONTROL.flags.LoggingStartTime) {
                        RTCyears = tens + ones;
                        if ((RTCyears > 99) && !RTCFlagBits.firstpass) return;  //invalid
                        RTCFlagBits.yearsChanged = 1;                           //set the years changed flag
                        break;
                    } else {
                        RTCseconds = tens + ones;
                        if ((RTCseconds > 59) && !RTCFlagBits.firstpass) return; //invalid
                        if (!LC2CONTROL2.flags2.SetStopTime)                    //if logging stop time is not being extracted from buffer
                            setClock(RTCAlarm1SecondsAddress, RTCseconds);      //load the Start Time Seconds into the RTC
                        break;
                    }

                case 4:
                    RTChours = tens + ones;
                    if ((RTChours > 23) && !RTCFlagBits.firstpass) return;      //invalid
                    RTCFlagBits.hoursChanged = 1;                               //set the hours changed flag
                    break;

                case 5:
                    RTCminutes = tens + ones;
                    if ((RTCminutes > 59) && !RTCFlagBits.firstpass) return;    //invalid
                    RTCFlagBits.minutesChanged = 1;                             //set the minutes changed flag
                    break;

                case 6:
                    RTCseconds = tens + ones;
                    if ((RTCseconds > 59) && !RTCFlagBits.firstpass) return;    //invalid
                    RTCFlagBits.secondsChanged = 1;                             //set the seconds changed flag
                    break;

                default:
                    return;
            }

            RTCFlagBits.firstpass = 0;

        }                                                                       //end of for(index)

        index++;
        RTCFlagBits.firstpass = 1;
        tens = 0;
        ones = 0;
    }                                                                           //end of for(SelectionLoop)

    if (!LC2CONTROL2.flags2.SetStopTime)                                        //don't load RTC if logging stop time is being extracted from buffer
    {
        if (RTCFlagBits.monthsChanged)                                          //if the user entered new month info
            setClock(RTCMonthsAddress, RTCmonths);                              //load it into the RTC

        if (RTCFlagBits.daysChanged)                                            //if the user entered new day info
            setClock(RTCDaysAddress, RTCdays);                                  //load it into the RTC

        if (RTCFlagBits.yearsChanged)                                           //if the user entered new year info
            setClock(RTCYearsAddress, RTCyears);                                //load it into the RTC

        if (RTCFlagBits.hoursChanged && !LC2CONTROL.flags.LoggingStartTime)     //if the user entered new hour info for current time
            setClock(RTCHoursAddress, RTChours);                                //load it into the RTC

        if (RTCFlagBits.hoursChanged && LC2CONTROL.flags.LoggingStartTime)      //if the user entered new hour info for Start Logging
        {
            setClock(RTCAlarm1HoursAddress, RTChours);                          //load it into the RTC
            enableAlarm(Alarm1);
        }

        if (RTCFlagBits.minutesChanged && !LC2CONTROL.flags.LoggingStartTime)   //if the user entered new minute info
            setClock(RTCMinutesAddress, RTCminutes);                            //load it into the RTC

        if (RTCFlagBits.minutesChanged && LC2CONTROL.flags.LoggingStartTime) {
            setClock(RTCAlarm1MinutesAddress, RTCminutes);
            enableAlarm(Alarm1);
        }

        if (RTCFlagBits.secondsChanged)                                         //if the user entered new seconds info
            setClock(RTCSecondsAddress, RTCseconds);                            //load it into the RTC
    }

    crlf();

    putsUART1(Date);                                                            //Date:
    while (BusyUART1());

    RTCdata = readClock(RTCMonthsAddress);                                      //get the month from the RTC
    displayClock(RTCdata);                                                      //display it

    putcUART1(slash);                                                           // '/'
    while (BusyUART1());

    RTCdata = readClock(RTCDaysAddress);                                        //get the day from the RTC
    displayClock(RTCdata);                                                      //display it

    putcUART1(slash);                                                           // '/'
    while (BusyUART1());

    RTCdata = readClock(RTCYearsAddress);                                       //get the year from the RTC
    displayClock(RTCdata);                                                      //display it

    putsUART1(Time);                                                            //Time:
    while (BusyUART1());

    RTCdata = readClock(RTCHoursAddress);                                       //get the hour from the RTC
    displayClock(RTCdata);                                                      //display it

    putcUART1(colon);                                                           // ':'
    while (BusyUART1());

    RTCdata = readClock(RTCMinutesAddress);                                     //get the minute from the RTC
    displayClock(RTCdata);                                                      //display it

    putcUART1(colon);                                                           // ':'
    while (BusyUART1());

    RTCdata = readClock(RTCSecondsAddress);                                     //get the second from the RTC
    displayClock(RTCdata);                                                      //display it
}

int checkScanInterval(void) 
{
    MUX4_ENABLE.mux=read_Int_FRAM(MUX4_ENABLEflagsaddress);  

    if (MUX4_ENABLE.mflags.mux16_4 == Single) {
        //if (ScanInterval < minScanSingleVW)                                   //REM REV 1.10
        if ((ScanInterval < minScanSingleVW) | (LogIntLength<minScanSingleVW))  //REV 1.10
            return minScanSingleVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) {
        //if (ScanInterval < minScanFourVW)                                     //REM REV 1.10
        if ((ScanInterval < minScanFourVW) | (LogIntLength<minScanFourVW))      //REV 1.10
            return minScanFourVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) {
        //if (ScanInterval < minScanEightVW)                                    //REM REV 1.10
        if ((ScanInterval < minScanEightVW) | (LogIntLength<minScanEightVW))    //REV 1.10
            return minScanEightVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) {
        //if (ScanInterval < minScanSixteenVW)                                  //REM REV 1.10
        if ((ScanInterval < minScanSixteenVW) | (LogIntLength<minScanSixteenVW))//REV 1.10
            return minScanSixteenVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) {
        //if (ScanInterval < minScanThirtytwoVW)                                //REM REV 1.10
        if ((ScanInterval < minScanThirtytwoVW) | (LogIntLength<minScanThirtytwoVW))  //REV 1.10
            return minScanThirtytwoVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) {
        //if (ScanInterval < minScanEightTH)                                    //REM REV 1.10
        if ((ScanInterval < minScanEightTH) | (LogIntLength<minScanEightTH))    //REV 1.10
            return minScanEightTH;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) {
        //if (ScanInterval < minScanThirtytwoTH)                                //REM REV 1.10
        if ((ScanInterval < minScanThirtytwoTH) | (LogIntLength<minScanThirtytwoTH))  //REV 1.10
            return minScanThirtytwoTH;
        return 0;
    }
}


void checkSynch(unsigned long ReadingTimeSeconds) {
    unsigned char CurrentDay = 0;
    unsigned long CurrentTimeSeconds = 0;
    unsigned long ScanInterval = 0;
    unsigned long NextTimeSeconds = 0;

    CurrentTimeSeconds = RTChms2s(1);                                           //get the current time from the RTC
    NextTimeSeconds = RTChms2s(0);                                              //get the next time to read from the RTC
    ScanInterval = hms2s();                                                     //get the scan interval
    CurrentDay = readClock(RTCDaysAddress);                                     //get the current day

    if ((((CurrentTimeSeconds >= NextTimeSeconds) && NextTimeSeconds != 0) |
            ((RTCdays != CurrentDay) && NextTimeSeconds == 0)) &&
            ((ReadingTimeSeconds + ScanInterval) - NextTimeSeconds != 86400))   //will the next reading get missed?
    {
        if (LC2CONTROL2.flags2.FirstReading)                                    //is it the first reading?
        {
            NextTimeSeconds += ScanInterval;                                    //adjust the next time to read by +1 ScanInterval unit
            hms(NextTimeSeconds, 1);                                            //update the RTC Alarm1 register accordingly
        }
    }
}

void clockMux(unsigned int delayValue) {
    MUX_CLOCK = 1;                                                              //set MUX_CLOCK high (1st pulse)
    delay(delayValue);                                                          //delay
    MUX_CLOCK = 0;                                                              //set MUX_CLOCK low
    delay(delayValue);                                                          //delay
    MUX_CLOCK = 1;                                                              //set MUX_CLOCK high (2nd pulse)
    delay(delayValue);                                                          //delay
    MUX_CLOCK = 0;                                                              //set MUX_CLOCK low
}

void clockThMux(unsigned int delayValue) {
    MUX_RESET = 1;                                                              //set MUX_RESET high
    delay(delayValue);                                                          //delay
    MUX_RESET = 0;                                                              //set MUX_RESET low
}

//***************************************************************************
//				CMDcomm()
//
//	Command line interface communications loop
//
//	Parameters received: none
//	Returns: nothing
//
//
//***************************************************************************
void CMDcomm(void) 
{
    unsigned char MonitorWasEnabled = 0;
    unsigned char value;
    unsigned char Alarm1SecondsValue;
    unsigned char Alarm1MinutesValue;
    unsigned char Alarm1HoursValue;
    char RxDataTemp = 0;
    char trapBUF[6];                                                            //temporary storage for trap count, lithium cell reading and minimum allowable scan interval
    char testmenuBUF[2];                                                        //stores test menu selection
    int channel = 0;                                                            //current channel #
    int conversion = 0;                                                         //current conversion type:
                                                                                //0=linear
                                                                                //1=polynomial
                                                                                //-1=channel disabled
    int ch;
    long i;                                                                     
    int LogInt = 0;
    int LogIntIt = 0;
    int SR_SAVE;
    int TBLPAG_SAVE;
    int tensScanInterval = 0;
    unsigned int DisplayArrayPointer = 0;
    unsigned int response;
    unsigned int data;                                                          //FOR FRAM TEST
    unsigned int testData;                                                      //for testInternalFRAM
    unsigned int id;                                                            //display index
    unsigned long Alarm1SecondsLong = 0;
    unsigned long Alarm1MinutesLong = 0;
    unsigned long Alarm1HoursLong = 0;
    volatile unsigned long memoryStatus;
    unsigned long tempID = 0;                                                   //VER 6.0.12

    for(i=0;i<5;i++)                                                            //empty the Rx buffer
    {
        RxData = ReadUART1();
    }

    if (!_232)                                                                  //is RS232?
        _232SHDN = 1;                                                           //Enable RS232

    while (!IFS3bits.T9IF)                                                      //enable COM for Timeout period
    {
        shutdownTimer(TimeOut);                                                 //start 15S shutdown timer	
        //wait till first char is received
        while(!DataRdyUART1() && !IFS3bits.T9IF && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR);    

        if(U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)  
            handleCOMError();                                                   //if communications error   

        RxData = ReadUART1();

        if (!DISPLAY_CONTROL.flags.Shutup && !IFS3bits.T9IF)                    
        {
            putsUART1(Hello);
            while (BusyUART1());
            prompt();
        }

        DISPLAY_CONTROL.flags.Shutup = 0;                                       //Clear Shutup flag if set 

        for (i = 0; i < 30; i++)                                                //clear the buffer
        {
            buffer[i] = 0;
        }

        i = 0;                                                                  //initialize char buffer index

        RxData = 0;                                                             //initialize receive buffer

        while (!IFS3bits.T9IF)
        {
            while (!IFS3bits.T9IF)
            {
                shutdownTimer(TimeOut);                                         //start 15S shutdown timer	

                while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS3bits.T9IF); //read the keyboard input - timeout if 15 seconds of inactivity
                if(U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)            
                    handleCOMError();                                           //if communications error   

                if (IFS3bits.T9IF)                                              //break out of loop if time to shutdown	
                {
                    prompt();                                                   
                    return;                                                     
                }

                RxData = ReadUART1();                                           //get the char from the USART buffer
                if (RxData != cr && RxData != escape) {
                    putcUART1(RxData);                                          //echo char (except <CR>) to terminal
                    while (BusyUART1());
                }

                if (RxData == 0x08)                                             //decrement buffer index if backspace is received
                {
                    i--;
                    putcUART1(space);                                           //clear the previous character
                    while (BusyUART1());
                    putcUART1(0x08);
                    while (BusyUART1());
                }

                if (i < 0)                                                      //don't let buffer index go below zero
                    i = 0;

                if (RxData != 0x08 && i < 52)                                   //as long as char is not backspace and index is <52	
                {
                    buffer[i] = RxData;                                         //store the received char in the buffer
                    i++;
                    if (i > 51)
                        i = 0;
                }

                if (RxData == cr)                                               //CR received - buffer is ready
                {
                    RxData = 0;                                                 //clear the receive buffer
                    break;
                }
            }

            LC2CONTROL2.flags2.X = 0;                                           //make sure 'X' flag is clear 

            switch (buffer[0]) 
            {

                case question:                                                  //Help Screen
                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    crlf();
                    putsUART1(CommandDescription);                              //Command				Description
                    while (BusyUART1());

                    crlf();
                    putsUART1(Dashes);                                          //----------------------------------------
                    while (BusyUART1());
                    
                    crlf();
                    putsUART1(BT);                                              //BT 					Bluetooth sTatus
                    while (BusyUART1());

                    crlf();
                    putsUART1(BT0);                                             //BT0					Disable Bluetooth
                    while (BusyUART1());

                    crlf();
                    putsUART1(BT1);                                             //BT1					Enable Bluetooth
                    while (BusyUART1());     
                    
                    crlf();
                    putsUART1(BTD);                                             //BTD					Disable Bluetooth Timer 
                    while (BusyUART1());  
                    
                    crlf();
                    putsUART1(BTE);                                             //BTE					Enable Bluetooth Timer 
                    while (BusyUART1());    
                    
                    crlf();                                     
                    putsUART1(BTR);                                             //BTR                   Bluetooth Reset     
                    while(BusyUART1());

                    crlf();
                    putsUART1(Viewclock);                                       //C						view current Clock
                    while (BusyUART1());

                    crlf();
                    putsUART1(Clockset);                                        //CSmm/dd/yy/hh:mm:ss	Clock Set
                    while (BusyUART1());

                    crlf();
                    putsUART1(Default);                                         //DEFAULT				load factory DEFAULT gage settings
                    while (BusyUART1());

                    crlf();
                    putsUART1(DF);                                              //DF					Date Format(0=Julian,1=Month,Day)
                    while (BusyUART1());

                    crlf();
                    putsUART1(DL);                                              //DL					Datalogger Type
                    while (BusyUART1());

                    crlf();
                    putsUART1(Displaynnnn);                                     //Dnnnnn				Display nnnn arrays (formatted) from pointer
                    while (BusyUART1());

                    crlf();
                    putsUART1(End);                                             //E						End communications and go to sleep
                    while (BusyUART1());

                    crlf();
                    putsUART1(GageinformationZ);                                //Gnn/L/tt/szzzzzz/sffffff/soooooo
                    while (BusyUART1());

                    crlf();
                    putsUART1(or);                                              //or
                    while (BusyUART1());

                    crlf();
                    putsUART1(GageinformationA);                                //Gnn/L/tt/saaaaaa/sbbbbbb/scccccc
                    while (BusyUART1());

                    crlf();

                    crlf();
                    putsUART1(GageinfoWhere);                                   //Gage Information where;
                    while (BusyUART1());

                    crlf();
                    putsUART1(nn);                                              //						nn = Channel #
                    while (BusyUART1());

                    crlf();
                    putsUART1(c);                                               //						c = Conversion Type(L/C)
                    while (BusyUART1());

                    crlf();
                    putsUART1(tt);                                              //						tt = Gage Type
                    while (BusyUART1());

                    crlf();

                    crlf();
                    putsUART1(forLinear);                                       //						For Linear Conversion:
                    while (BusyUART1());

                    crlf();
                    putsUART1(szzzz);                                           //						szzzzzz = zero reading with sign
                    while (BusyUART1());

                    crlf();
                    putsUART1(sffff);                                           //						sffffff = gage factor with sign
                    while (BusyUART1());

                    crlf();
                    putsUART1(soooo);                                           //						soooooo = offset with sign
                    while (BusyUART1());

                    crlf();

                    crlf();
                    putsUART1(forPoly);                                         //						For Polynomial Conversion:
                    while (BusyUART1());

                    crlf();
                    putsUART1(saaaa);                                           //						saaaaaa = Polynomial coefficient A
                    while (BusyUART1());

                    crlf();
                    putsUART1(sbbbb);                                           //						sbbbbbb = Polynomial Coefficient B
                    while (BusyUART1());

                    crlf();
                    putsUART1(scccc);                                           //						scccccc = Polynomial coefficient C
                    while (BusyUART1());

                    crlf();

                    crlf();
                    putsUART1(ID);                                              //IDdddddddddddddddd	view current ID, set to dddddddddddddddd
                    while (BusyUART1());

                    crlf();
                    putsUART1(Log);                                             //LN/lllll/iii			view Log intervals/change n interval
                    while (BusyUART1());

                    crlf();
                    putsUART1(length);                                          //						lllll = length
                    while (BusyUART1());

                    crlf();
                    putsUART1(iterations);                                      //						iii = iterations of interval
                    while (BusyUART1());

                    crlf();
                    putsUART1(Logdisenable);                                    //LD,LE					Log intervals Disable, Enable
                    while (BusyUART1());
                    
                    crlf();                                                     //MAddd                 MODBUS Address (1-247)  
                    putsUART1(Modbusaddress);                                   
                    while(BusyUART1());                                         

                    crlf();
                    putsUART1(Monitorstatus);                                   //M,MD,ME				Monitor status, Disable, Enable
                    while (BusyUART1());
                    
                    crlf();                                                     
					putsUART1(MEMSTAT);                                         //MS					Memory Status   
					while(BusyUART1());                                         
					

                    crlf();
                    putsUART1(MX);                                              //MXS					Multiplexer Setup
                    while (BusyUART1());

                    crlf();
                    putsUART1(MXselect);                                        //MX#					Select Multiplexer Configuration(1,4,8,8T,16,32,32T)
                    while (BusyUART1());

                    crlf();
                    putsUART1(Displaynext);                                     //N						display Next time to read
                    while (BusyUART1());

                    crlf();
                    putsUART1(O);                                               //O 					pOrt status
                    while (BusyUART1());

                    crlf();
                    putsUART1(O0);                                              //O0					pOrt OFF (0)
                    while (BusyUART1());

                    crlf();
                    putsUART1(O1);                                              //O1					pOrt ON (1)
                    while (BusyUART1());

                    crlf();
                    putsUART1(OD);                                              //OD					pOrt timer Disabled
                    while (BusyUART1());

                    crlf();
                    putsUART1(OE);                                              //OE					pOrt timer Enabled
                    while (BusyUART1());

                    crlf();
                    putsUART1(OTT);                                             //OThh:mm				pOrt sTart time
                    while (BusyUART1());

                    crlf();
                    putsUART1(OPT);                                             //OPhh:mm				pOrt stoP time
                    while (BusyUART1());

                    crlf();
                    putsUART1(Position);                                        //Pnnnn					Position array pointer to nnnn
                    while (BusyUART1());

                    crlf();
                    putsUART1(Reset);                                           //R						Reset memory
                    while (BusyUART1());

                    crlf();
                    putsUART1(RESET);                                           //RESET					RESET processor
                    while (BusyUART1());
                    
                    crlf();                                                     
                    putsUART1(RSN);                                             //RSN                   Read transducer Serial Number    
                    while (BusyUART1());                                        


                    crlf();
                    putsUART1(SystemStatus);                                    //S,SS					datalogger Status, System Status
                    while (BusyUART1());

                    crlf();
                    putsUART1(Scanint);                                         //SCnnnnn				view SCan interval/enter nnnnn interval
                    while (BusyUART1());

                    crlf();
                    putsUART1(Stoplog);                                         //SPhh:mm				StoP logging, hh:mm = stop time
                    while (BusyUART1());

                    crlf();
                    putsUART1(sR);                                              //SR					Synchronize Readings
                    while (BusyUART1());

                    crlf();
                    putsUART1(Startlog);                                        //SThh:mm				STart logging, hh:mm = start time
                    while (BusyUART1());

                    crlf();
                    putsUART1(SV);                                              //SV					Software Version
                    while (BusyUART1());
                    
					crlf();														
					putsUART1(Tnn);                                             //Tnn/t						
					while(BusyUART1());											

					crlf();														
					putsUART1(nn);                                              //						nn = Channel #	
					while(BusyUART1());											
					
                    crlf();														
					putsUART1(t);                                               //						t = Thermistor Type	
					while(BusyUART1());											

                    crlf();
                    putsUART1(TF);                                              //TF					Time Format(0=hhmm, 1=hh,mm)
                    while (BusyUART1());
				
                    crlf();
                    putsUART1(TR);                                              //TR					display TRap count, zero TRap count
                    while (BusyUART1());

                    crlf();
                    putsUART1(VL);                                              //VL					display Lithium cell Voltage
                    while (BusyUART1());

                    crlf();
                    putsUART1(VTHREE);                                          //V3					display 3V Battery Voltage
                    while (BusyUART1());

                    crlf();
                    putsUART1(VTWELVE);                                         //V12					display 12V Battery Voltage
                    while (BusyUART1());

                    crlf();
                    putsUART1(WF);                                              //WF					Wrap Format
                    while (BusyUART1());

                    crlf();
                    putsUART1(XCMD);                                            //X						Single Reading - NOT stored
                    while (BusyUART1());
                    
                    crlf();
                    putsUART1(Modbus);                                          //:::					Switch to MODBUS communications     
                    while (BusyUART1());                    

                    break;


                case capB:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    while (BusyUART1());

                    if (buffer[1] == cr)
                        break;
                    
                    if(buffer[1]==capT && buffer[2]==cr)                        //BT received     
                    {
                        BTStatus();                                             
                        break;
                    }
                    
                    if (buffer[1] == capT && buffer[2] == zero && buffer[3] == cr)  //BT0 received  Deactivate Bluetooth      
                    {
                        BTZERO();                                               
                        BTStatus();                                             //display Bluetooth status                        
                        break;
                    }
                    
                    if (buffer[1] == capT && buffer[2] == one && buffer[3] == cr)   //BT1 received  Activate Bluetooth    
                    {
                        BTONE();                                                
                        BTStatus();                                             //display Bluetooth status
                        break;
                    }


                    if (buffer[1]==capT && buffer[2] == capD && buffer[3] == cr) //BTD<CR> received 
                    {
                        BT_D();                                                 
                        BTStatus();                                             //display Bluetooth status
                        break;
                    }

                    if (buffer[1]==capT &&buffer[2] == capE && buffer[3] == cr) //BTE<CR> received  
                    {
                        BT_E();                                                  
                        BTStatus();                                             //display Bluetooth status
                        break;
                    }

                    if (buffer[1]==capT &&buffer[2] == capR && buffer[3] == cr) //BTR<CR> received  
                    {
                        BT_R();                                                  
                        break;
                    }                    

                    if (buffer[2] == nine && buffer[3] == cr)                   //setup for 9600 baud
                        value = brg9600;
                    else
                    if (buffer[2] == one && buffer[3] == one && buffer[4] == five && buffer[5] == cr) //setup for 115200 baud
                        value = brg115200;
                    else                                                        
                    if (buffer[2] == two && buffer[3] == three && buffer[4] == zero && buffer[5] == cr) //setup for 230400 baud 
                        value = brg230400;                                          
                    else                                                        
                    if (buffer[2] == four && buffer[3] == six && buffer[4] == zero && buffer[5] == cr) //setup for 460800 baud 
                        value = brg460800;                                                              
           
                    baudrate = value;
                    write_Int_FRAM(baudrateaddress,baudrate);                   //store baudrate in FRAM		
                    LC2CONTROL.flags.Reset = 1; //Set the Reset flag
                    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);     //store flag in FRAM  
                    asm("RESET");
                    break;

                
                    break;


                case capC:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    while (BusyUART1());

                    if (buffer[1] == cr)                                        //C<CR> received
                    {
                        READ_TIME();                                            
                        break;
                    }

                    if (buffer[1] == capS)                                      //CS received
                    {
                        SR_SAVE = SR;
                        TBLPAG_SAVE = TBLPAG;
                        SRbits.IPL = 0x7;                                       //Disable interrupts 

                        if (LC2CONTROL.flags.LoggingStartTime)                  //save flags
                            LC2CONTROL2.flags2.LoggingStartTimeTemp = 1;

                        if (LC2CONTROL2.flags2.SetStartTime)
                            LC2CONTROL2.flags2.SetStartTimeTemp = 1;

                        LC2CONTROL.flags.LoggingStartTime = 0;
                        LC2CONTROL2.flags2.SetStartTime = 0;
                        LC2CONTROL2.flags2.SetStopTime = 0; 
                        PORT_CONTROL.flags.SetAlarm2Time = 0; 
                        PORT_CONTROL.flags.SetAlarm2StopTime = 0; 

                        Buf2DateTime(buffer);

                        if (LC2CONTROL.flags.ERROR) 
                        {
                            LC2CONTROL.flags.ERROR = 0;                         //clear the ERROR flag
                            break;
                        }

                        if (LC2CONTROL2.flags2.LoggingStartTimeTemp)            //restore flags
                            LC2CONTROL.flags.LoggingStartTime = 1;
                        if (LC2CONTROL2.flags2.SetStartTimeTemp)
                            LC2CONTROL2.flags2.SetStartTime = 1;
                        if (DISPLAY_CONTROL.flags.Synch)
                            VWflagsbits.synch = 1;                              //set the synch flag
                        if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) // update only if logging
                            upD8RTCAlarm1();

                        TBLPAG = TBLPAG_SAVE;
                        SR = SR_SAVE;   
                        break;
                    }

                    break;

                case capD:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    crlf();
                    while (BusyUART1());

                    memoryStatus=read_Int_FRAM(MemoryStatusaddress);            //get the memory status 

                    if (buffer[1] == capE && buffer[2] == capF && buffer[3] == capA && buffer[4] == capU //load DEFAULTS
                            && buffer[5] == capL && buffer[6] == capT && buffer[7] == cr) 
                    {
                        if (LC2CONTROL.flags.Logging) 
                        {
                            putsUART1(DnotAllowed);                             //"DEFAULT not allowed while logging"
                            while (BusyUART1());
                            break;
                        }

                        putsUART1(ThisWillLoadDefaults);                        //"This will load the factory default settings!"    
                        while (BusyUART1());
                        
                        crlf();
                        putsUART1(RUsure);                                      //Are you sure(Y/N)?
                        while (BusyUART1());
                        
                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS3bits.T9IF); //wait for user response	
                        if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                            handleCOMError();                                   //if communications error

                        response = ReadUART1();                                 //get the char from the USART buffer
                        putcUART1(response);                                    //echo char to terminal
                        while (BusyUART1());

                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS3bits.T9IF); //wait for <CR>	

                        RxData = ReadUART1(); //RxData contains user response

                        if (response == capY && RxData == cr)                   //yes, so load defaults
                        {
                            loadDefaults();
                            crlf();
                            putsUART1(AllChannelsLoaded);                       //"Restored to factory default settings."  
                            while (BusyUART1());
                            break;
                        } else {
                            crlf();
                            putsUART1(DefaultsNotLoaded);                       //"Settings not restored to factory defaults."  
                            while (BusyUART1());
                            break;
                        }
                    }

                    if (buffer[1] == cr)                                        //D<CR> received
                    {
                        if (memoryStatus == 0 && FRAM_MEMORY.flags.memEmpty)    //if memory is empty
                        {
                            crlf();
                            putsUART1(Noarrays2display);                        //"There are no arrays to display"
                            while (BusyUART1());
                            break;
                        }

                        if (!DISPLAY_CONTROL.flags.BPD) {
                            displayMemoryStatus();                              //display memory status from FRAM
                            break;
                        } else {
                            if (tempUserPosition <= memoryStatus && (!DISPLAY_CONTROL.flags.newPointer &&
                                    DISPLAY_CONTROL.flags.BPD && !DISPLAY_CONTROL.flags.Backup && DISPLAY_CONTROL.flags.Scan))
                                tempUserPosition = id;

                            if (tempUserPosition <= memoryStatus) {
                                LC2CONTROL2.flags2.ID = 1;                      //set the ID flag   
                                displayReading(ch, tempUserPosition);           //display the reading at the current user position
                            }

                            tempUserPosition++;
                            if (tempUserPosition > memoryStatus + 1)
                                tempUserPosition = memoryStatus + 1;
                            DISPLAY_CONTROL.flags.Backup = 0; //clear the Backup flag
                            break;
                        }

                    }

                    if (buffer[1] == capL && buffer[2] == cr)                   //DL<CR> received
                    {
                        if (MUX4_ENABLE.mflags.mux16_4 == Single)               //Single Channel    
                            putsUART1(LCfour);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW4)                  //4 channel mux 
                            putsUART1(LCfourbyfour);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW8)                  //8 channel VW mux 
                            putsUART1(LCfourbyeightVW);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW16)                 //16 channel mux
                            putsUART1(LCfourbysixteen);                          
                        if (MUX4_ENABLE.mflags.mux16_4 == VW32)                 //32 channel VW mux 
                            putsUART1(LCfourbythirtytwoVW);
                        if (MUX4_ENABLE.mflags.mux16_4 == TH8)                  //8 channel Therm    
                            putsUART1(LCfourbyeightTH);
                        if (MUX4_ENABLE.mflags.mux16_4 == TH32)                 //32 channel Therm    
                            putsUART1(LCfourbythirtytwoTH);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[2] == cr)                   //DF<CR> received
                    {
                        if (!LC2CONTROL.flags.DateFormat)                       //0=julian date format
                            putsUART1(Datejulian);
                        else
                            putsUART1(Datemonthday);                            //1=month/day format
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[3] == cr)                   //DF#<CR> received
                    {
                        if (buffer[2] == zero)                                  //julian date format chosen
                        {
                            LC2CONTROL.flags.DateFormat = 0;
                            putsUART1(Datejulian);
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        if (buffer[2] == one)                                   //month/day format chosen
                        {
                            LC2CONTROL.flags.DateFormat = 1;
                            putsUART1(Datemonthday);
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        while (BusyUART1());
                        break;
                    }

                    if (isdigit(buffer[1]) && !FRAM_MEMORY.flags.memEmpty)      //Enter # of arrays to display from PositionArrayPointer value
                    {
                        if (LC2CONTROL.flags.Monitor) {
                            MonitorWasEnabled = 1;
                            LC2CONTROL.flags.Monitor = 0;                       //clear the Monitor Flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        DISPLAY_CONTROL.flags.BPD = 1;                          //set the P_D mode flag						
                        DisplayArrayPointer = Buffer2Decimal(buffer, i, 0);     //Get the requested # of arrays to display

                        if (!DISPLAY_CONTROL.flags.Display)                     //Display from where previous display left off
                            id = tempUserPosition;
                        else
                            tempUserPosition = id;

                        restoreSettings();                                      //load the flags from FRAM   
                        LC2CONTROL2.flags2.ID = 1;                              //set the ID flag   
                        DISPLAY_CONTROL.flags.bail = 0;                         
                        IFS0bits.U1RXIF = 0;                                    //clear the UART1 interrupt flag

                        for (id; id < (tempUserPosition + DisplayArrayPointer); id++) {
                            IEC0bits.U1RXIE = 1; 
                            if (DISPLAY_CONTROL.flags.bail)                     //break out of data download
                                break;

                            if (id == (maxSingleVW + 1) && MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel  VW 
                            {
                                id = 1;                                         //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxSingleVW + 1); 
                                tempUserPosition = 1;
                            }

                            if (id == (maxFourVW + 1) && MUX4_ENABLE.mflags.mux16_4 == VW4) //if 4 channel VW MUX  
                            {
                                id = 1;                                         //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxFourVW + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxEightVW + 1) && MUX4_ENABLE.mflags.mux16_4 == VW8) //if 8 channel VW MUX  
                            {
                                id = 1;                                         //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxEightVW + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxSixteenVW + 1) && MUX4_ENABLE.mflags.mux16_4 == VW16) //if 16 channel VW MUX     
                            {
                                id = 1;                                         //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxSixteenVW + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxThirtytwoVW + 1) && MUX4_ENABLE.mflags.mux16_4 == VW32) //if 32 channel VW MUX     
                            {
                                id = 1;                                         //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxThirtytwoVW + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxEightTH + 1) && MUX4_ENABLE.mflags.mux16_4 == TH8) //if 8 channel TH MUX  
                            {
                                id = 1;                                         //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxEightTH + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxThirtytwoTH + 1) && MUX4_ENABLE.mflags.mux16_4 == TH32) //if 32 channel MUX  
                            {
                                id = 1;                                         //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxThirtytwoTH + 1);
                                tempUserPosition = 1;
                            }

                            displayReading(ch, id);                             //display the array at address id
                            
                        }                                                       //end of For loop
                        displayTemporaryStatus(id);                             //display memory status with current User Position    
                        
                        DISPLAY_CONTROL.flags.newPointer = 0;                   //clear the pointer updated flag

                        if (MonitorWasEnabled) {
                            LC2CONTROL.flags.Monitor = 1;                       //reset the monitor flag if previously set
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        DISPLAY_CONTROL.flags.BPD = 0;                          //clear the P_D mode flag	
                        DISPLAY_CONTROL.flags.Backup = 0;                       //clear the Backup flag 
                        DISPLAY_CONTROL.flags.Display = 1;                      //set the Display flag
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 
                        shutdownTimer(TimeOut);                                 
                        break;
                    }

                    if (FRAM_MEMORY.flags.memEmpty) 
                    {
                        crlf();
                        putsUART1(Noarrays2display);                            //"There are no arrays to display"
                        while (BusyUART1());
                        break;
                    }

                    DISPLAY_CONTROL.flags.Backup = 0;                           //clear the back pointer up flag
                    break;

                case capE:                                                      //Shutdown                       
                    
                    crlf();
                    write_Int_FRAM(UserPositionaddress,0x00);                   //Zero the User Position pointer    
                    write_Int_FRAM((UserPositionaddress+1),0x00);               //UserPositionaddress+1                       
                    DISPLAY_CONTROL.flags.Shutdown = 1;
                    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 
                    IFS3bits.T9IF = 1;                                          //set the T9IF to break out on CMDcomm() in main()

                    return;

                case capG:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   

                    for (i = 1; buffer[i] != cr && !LC2CONTROL.flags.ERROR; i++) //parse buffer for illegal characters
                    {                                                           //exit if one is encountered
                        if (buffer[i] != minus && buffer[i] != decimal && buffer[i] != slash && !isdigit(buffer[i]) && buffer[i] != capD
                                && buffer[i] != capL && buffer[i] != capP)
                            LC2CONTROL.flags.ERROR = 1;                         //set the error flag
                    }

                    if (LC2CONTROL.flags.ERROR)                                 //Test for error
                    {
                        LC2CONTROL.flags.ERROR = 0;                             //reset the error flag
                        break;                                                  //return to '*' prompt
                    }

                    if (!isdigit(buffer[1]))                                    //if buffer[1] is not valid channel #
                        break;                                                  //return to '*' prompt

                    if ((isdigit(buffer[1]) && buffer[2] == slash) | (isdigit(buffer[1]) && isdigit(buffer[2]) && buffer[3] == slash))
                        channel = getChannel();                                 //get the current channel #

                    //TEST MUX CONFIGURATION FOR INVALID CHANNELS:
                    if ((channel >= 5 && (MUX4_ENABLE.mflags.mux16_4 == VW4)) | //4 channel VW/TH
                            (channel >= 9 && (MUX4_ENABLE.mflags.mux16_4 == TH8)) | //8 channel TH
                            (channel >= 9 && (MUX4_ENABLE.mflags.mux16_4 == VW8)) | //8 channel VW
                            (channel >= 17 && (MUX4_ENABLE.mflags.mux16_4 == VW16)) | //16 channel VW/TH
                            (channel >= 33 && (MUX4_ENABLE.mflags.mux16_4 == TH32)) | //32 channel TH
                            (channel >= 33 && (MUX4_ENABLE.mflags.mux16_4 == VW32)) |
                            (channel != 1 && (MUX4_ENABLE.mflags.mux16_4 == Single))) //single channel
                        break;                                                  //return to '*' prompt

                    if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) 
                    {
                        conversion = getConversion();                           //get the type of conversion:
                                                                                //0=linear
                                                                                //1=polynomial
                                                                                //-1=channel disabled
                                                                                //2=invalid
                        //TEST MUX CONFIGURATION FOR INVALID CONVERSION:
                        if (conversion == 2)                                    //invalid conversion
                            break;                                              //return to the '*' prompt
                        setChannelConversion(channel, conversion);              //setup the channel conversion
                    }

                    //Extract gage type from buffer and save to FRAM:
                    gageType = getGageType(channel);                            //get the user entered gage type
                    if (gageType == 0)                                          //is the channel being disabled?
                    {
                        disableChannel(channel);
                        displayMUX(channel);                                    //display the channel #
                        break;
                    }

                    if ((gageType == -1 | gageType == -2) && (buffer[1] != slash))
                        break;                                                  //gage type hasn't changed or illegal entry

                    if (gageType != -1)                                         //if valid gage type is entered, store it
                        storeGageType(channel, gageType);

                    enableChannel(channel);                                     //enable the selected channel

                    //Extract gage info or polynomial coefficients from buffer and save to FRAM:

                    if (conversion)                                             //Polynomial Conversion
                    {
                        getGageInfo(1, channel);                                //get coefficient A
                        getGageInfo(2, channel);                                //get coefficient B
                        getGageInfo(3, channel);                                //get coefficient C
                    } else                                                      //Linear Conversion
                    {
                        getGageInfo(1, channel);                                //get zero reading
                        getGageInfo(2, channel);                                //get gage factor
                        getGageInfo(3, channel);                                //get gage offset
                    }

                    displayMUX(channel);                                        //display the channel #
                    break;


                case capI:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    crlf();

                    if (buffer[1] == capD && buffer[2] == cr && LC2CONTROL.flags.ID) //Display ID
                    {
                        putsUART1(DataloggerID);                                //Datalogger ID:
                        while (BusyUART1());

                        for (i = IDaddress; i < CH1GTaddress; i += 2)           //parse the buffer and extract the ID character   
                        {
                            data=read_Int_FRAM(i);                              //read the ID starting FRAM location  
                            unpack(data);                                       //unpack into (2) bytes
                            if (Hbyte == cr)                                    //exit if MSB = <CR>
                                break;
                            putcUART1(Hbyte);
                            while (BusyUART1());
                            if (Lbyte == cr)                                    //exit if LSB = <CR>
                                break;
                            putcUART1(Lbyte);
                            while (BusyUART1());
                        }

                        break;
                    }


                    if (buffer[1] == capD && buffer[2] == cr && !LC2CONTROL.flags.ID) //No ID to display
                    {
                        putsUART1(DataloggerID);                                //Datalogger ID:
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capD && buffer[2] == space)                //clear the current ID
                    {
                        putsUART1(DataloggerID);                                //Datalogger ID:
                        while (BusyUART1());

                        for (i = IDaddress; i < CH1GTaddress; i++)              //load the ID buffer with <cr>   
                        {
                            write_Int_FRAM(IDaddress,cr);      
                        }

                        LC2CONTROL.flags.ID = 0;                                //reset the ID flag
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM	
                        break;
                    }

                    if (buffer[1] == capD && buffer[2] != cr && buffer[2] != space) //Enter Datalogger ID and store in FRAM
                    {
                        tempID = IDaddress; 
                        LC2CONTROL.flags.ID = 1;                                //set the ID flag
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM	
                        putsUART1(DataloggerID);                                //Datalogger ID:
                        while (BusyUART1());

                        for (i = 2; i < 17; i++) {
                            if (!isdigit(buffer[i]) && !isalpha(buffer[i]) && buffer[i] != cr && !isgraph(buffer[i]) && buffer[i] != space)//invalid
                            {                                                   //so display the stored ID
                                for (i = tempID; i < CH1GTaddress; i += 2)      //parse the buffer and extract the ID character 
                                {
                                    data=read_Int_FRAM(i);                      //read the ID starting FRAM location  
                                    unpack(data);                               //unpack into (2) bytes
                                    if (Hbyte == cr)                            //exit if MSB = <CR>
                                        break;
                                    putcUART1(Hbyte);
                                    while (BusyUART1());
                                    if (Lbyte == cr)                            //exit if LSB = <CR>
                                        break;
                                    putcUART1(Lbyte);
                                    while (BusyUART1());
                                }
                                break;
                            }

                            Hbyte = buffer[i];                                  //load Hbyte
                            if (Hbyte == cr) {
                                data = pack(Hbyte, 0x00);
                                write_Int_FRAM(tempID,data);   
                                break;
                            }

                            i += 1;                                             //increment array pointer
                            Lbyte = buffer[i];                                  //load Lbyte
                            putcUART1(Hbyte);                                   //display ID
                            while (BusyUART1());
                            if (Lbyte != cr) {
                                putcUART1(Lbyte);
                                while (BusyUART1());
                            }
                            data = pack(Hbyte, Lbyte);
                            write_Int_FRAM(tempID,data);                        //store in FRAM   

                            if (Lbyte == cr)
                                break;
                            tempID += 2;                                        //increment FRAM pointer
                        }

                        break;
                    }

                    break;

                case capL:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    while (BusyUART1());

                    if (buffer[1] == cr)                                        //L<CR> received, so display Log Interval Table
                    {
                        displayLogTable();                                      //Display Log Interval Table
                        crlf();
                        break;
                    }

                    if (buffer[1] == capD)                                      //LD received
                    {
                        if (LC2CONTROL.flags.LogInterval) {
                            crlf();
                            putsUART1(Logdisabled);                             //Log intervals disabled
                            while (BusyUART1());
                            DISLOGINT();
                            break;
                        }
                        crlf();
                        putsUART1(Logintalreadydisabled);                       //Log intervals already disabled!
                        while (BusyUART1());
                    }

                    if (buffer[1] == capE)                                      //LE received
                    {
                        if (LC2CONTROL.flags.LoggingStopTime) {
                            crlf();
                            putsUART1(LogIntStopTime);
                            while (BusyUART1());
                            break;
                        }

                        if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) {
                            crlf();
                            putsUART1(LogIntWhileLogging);
                            while (BusyUART1());
                            break;
                        }

                        if (!LC2CONTROL.flags.LogInterval)                      //is Log interval flag clear?
                        {
                            crlf();
                            putsUART1(Logenabled);                              //Log intervals enabled
                            while (BusyUART1());
                            ENLOGINT();                                         
                            if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) //is datalogger logging?
                            {
                                DISPLAY_CONTROL.flags.Shutup = 1;               //don't allow message display
                                stopLogging();                                  //stop and restart logging if it is	
                                if (DISPLAY_CONTROL.flags.Synch)
                                    VWflagsbits.synch = 1;                      //set the synch flag
                                crlf();
                                startLogging();
                                DISPLAY_CONTROL.flags.Shutup = 0;
                            }

                            break;
                        }

                        crlf();
                        putsUART1(Logintalreadyenabled);                        //Log intervals already enabled!
                        while (BusyUART1());
                    }


                    for (i = 1; buffer[i] != cr && !LC2CONTROL.flags.ERROR; i++) //parse buffer for illegal characters
                    {                                                           //exit if one is encountered
                        if (buffer[i] != slash && !isdigit(buffer[i]))
                            LC2CONTROL.flags.ERROR = 1;                         //set the error flag
                    }

                    if (LC2CONTROL.flags.ERROR)                                 //Test for error
                    {
                        LC2CONTROL.flags.ERROR = 0;                             //reset the error flag
                        break;                                                  //return to '*' prompt
                    }

                    LogInt = getLogInterval();                                  //Get the Log Interval

                    if (LogInt == -2)                                           //error
                        break;                                                  //return to "*" prompt

                    if (LogInt != -1)                                           //"L//" received?
                        getLogIntLength();                                      //no, so get interval length

                    if (LogIntLength <= 0 | LogIntLength > 86400)               //error or illegal
                        break;

                    testScanInterval = checkScanInterval();

                    if (testScanInterval)                                       //if error
                    {
                        crlf();
                        putsUART1(MinInterror);                                 //ERROR: Minumum Scan Interval for this configuration is :
                        while (BusyUART1());
                        sprintf(trapBUF, "%d", testScanInterval);               //minimum scan interval in seconds
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        putsUART1(Seconds);
                        while (BusyUART1());
                        break;
                    }

                    LogIntIt = getLogIterations();

                    if (LogIntIt == -1)                                         //error
                        break;

                    storeLogInterval(LogInt, LogIntIt);                         //store the Log Interval information
                    displayLogInterval(LogInt);
                    break;


                case capM:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    while (BusyUART1());

                    if (buffer[1] == cr && !LC2CONTROL.flags.Monitor)           //Monitor mode disabled?
                    {
                        crlf();
                        putsUART1(Monitordisabled);                             //display Monitor mode disabled
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == cr && LC2CONTROL.flags.Monitor)            //Monitor mode enabled?
                    {
                        crlf();
                        putsUART1(Monitorenabled);                              //display Monitor mode enabled
                        while (BusyUART1());
                        break;
                    }
                    
                    
                    if (buffer[1] == capA && buffer[2]==cr)                     //MA received
                    {
                        crlf();
                        NAdata=read_Int_FRAM(MODBUSaddress);					//read MODBUS Address from FRAM 
                        putsUART1(ModbusaddressIS);                             //MODBUS address:
                        while (BusyUART1());
                        sprintf(NABUF, "%d", NAdata);                           //format MODBUS address
                        putsUART1(NABUF);                                       //display MODBUS address
                        while (BusyUART1());
                        break;
                    }
                    
                    if (buffer[1] == capA && ((isdigit(buffer[2]) && buffer[3] == cr) |
                            (isdigit(buffer[2]) && isdigit(buffer[3]) && buffer[4] == cr) |
                            (isdigit(buffer[2]) && isdigit(buffer[3]) && isdigit(buffer[4]) && buffer[5] == cr))) //Enter MODBUS Address
                    {
                        crlf();
                        while (BusyUART1());
                        MODBUSaddressvalue = Buffer2Decimal(buffer, i, 2);      //extract MODBUS address from buffer
                        if (MODBUSaddressvalue == 0 | MODBUSaddressvalue>247)   // ERROR
                            break;
                        write_Int_FRAM(MODBUSaddress,MODBUSaddressvalue);		//store MODBUS address in FRAM   
                        NAdata=read_Int_FRAM(MODBUSaddress);					//read MODBUS address from FRAM  
                        putsUART1(ModbusaddressIS);                             //"MODBUS address:"
                        while (BusyUART1());
                        sprintf(NABUF, "%d", NAdata);                           //format MODBUS address
                        putsUART1(NABUF);                                       //display MODBUS address
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capD)                                      //MD received
                    {
                        crlf();
                        if (LC2CONTROL.flags.Monitor)                           //is Monitor flag set?
                        {
                            putsUART1(Monitordisabled);                         //Monitor mode disabled
                            while (BusyUART1());
                            LC2CONTROL.flags.Monitor = 0;                       //Clear Monitor flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                            break;
                        }

                        putsUART1(Monitoralreadydisabled);                      //Monitor already disabled!
                        while (BusyUART1());
                    }

                    if (buffer[1] == capE)                                      //ME received
                    {
                        crlf();
                        if (!LC2CONTROL.flags.Monitor)                          //is Monitor flag clear?
                        {
                            putsUART1(Monitorenabled);                          //Monitor mode enabled
                            while (BusyUART1());
                            LC2CONTROL.flags.Monitor = 1;                       //Set Monitor flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                            break;
                        }

                        putsUART1(Monitoralreadyenabled);                       //Monitor already enabled!
                        while (BusyUART1());
                    }
                    
                    if(buffer[1]==capS)                                         //MS received
					{
						crlf();
                        switch(MUX4_ENABLE.mflags.mux16_4)
                        {
                            case Single:                                        
                                putsUART1(Readings29K);                         //Display 29000 readings max
                                break;
                            case VW4:
                                putsUART1(Readings14_5K);                       //Display 14500 readings max
                                break;
                            case VW8:
                                putsUART1(Readings11_86K);                      //Display 11860 readings max
                                break;
                            case VW16:
                                putsUART1(Readings4_83K);                       //Display 4830 readings max
                                break;
                            case VW32:
                                putsUART1(Readings3_72K);                       //Display 3720 readings max
                                break;
                            case TH8:
                                putsUART1(Readings18_64K);                      //Display 18640 readings max
                                break;
                            case TH32:
                                putsUART1(Readings6_87K);                       //Display 6870 readings max
                                break;
                            default:
                                break;
                        }
						while(BusyUART1());								

						crlf();
						if(DISPLAY_CONTROL.flags.WrapMemory)                    //is Memory set to wrap?
						{
							putsUART1(Loggingwillnotstopmemfull);               //Display Logging will not stop when
						}                                                       //memory is full
						else
						{
							putsUART1(Loggingwillstopmemfull);                  //Display Logging will stop when 
						}                                                       //memory is full
						while(BusyUART1());		

						crlf();
						if(DISPLAY_CONTROL.flags.Synch)                         //Will readings synchronize to the top of the hour? 
						{
							putsUART1(Synch);                                   //Display "Readings are synchronized to the top of the hour."
						}												
						else
						{
							putsUART1(Synchnot);                                //Display "Readings are not synchronized to the top of the hour." 
						}												
						while(BusyUART1());	
                        break;
					}


                    //case MXS, MX1, MX4, MX8, MX8T, MX16, MX32, MX32T:

                    if (buffer[1] == capX && buffer[2] == capS && buffer[3] == cr) //Display multiplexer setup
                    {
                        crlf();
                        displayMUX(0);
                        break;
                    }

                    if (buffer[1] == capX && buffer[2] == one && buffer[3] == cr) //Select single channel VW Datalogger
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MX1();                                                     
                        putsUART1(MUX1);                                        //Display Single Channel Datalogger Selected.    
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capX && buffer[2] == four && buffer[3] == cr) //Select 4 channel mux
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MX4();
                        putsUART1(MUX4);                                        //Display 4 Channel Mux Selected.
                        while (BusyUART1());
                        break;
                    }

                    //VER 6.0.6:
                    if (buffer[1] == capX && buffer[2] == eight && buffer[3] == capT && buffer[4] == cr) //Select 8 channel thermistor mux
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MX8T();                                                 
                        putsUART1(MUX8);                                        //Display 8 Channel Mux Selected.
                        while (BusyUART1());
                        break;
                    }

                    //VER 6.0.9:
                    if (buffer[1] == capX && buffer[2] == three && buffer[3] == two && buffer[4] == capT && buffer[5] == cr) //Select 32 channel thermistor mux
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MX32T();                                                
                        putsUART1(MUX32);                                       //Display 32 Channel Mux Selected.
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capX && buffer[2] == one && buffer[3] == six && buffer[4] == cr) //Select 16 channel mux
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MX16();                                                 
                        putsUART1(MUX16);                                       //Display 16 Channel Mux Selected.
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capX && buffer[2] == eight && buffer[3] == cr) //Select 8 channel VW mux
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MX8();                                                  
                        putsUART1(MUX8VW);                                      //Display 8 Channel VW Mux Selected.
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capX && buffer[2] == three && buffer[3] == two && buffer[4] == cr) //Select 32 channel VW mux
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MX32();                                                 
                        putsUART1(MUX32VW); //Display 32 Channel VW Mux Selected.
                        while (BusyUART1());
                        break;
                    }

                    break;


                case capN:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    while (BusyUART1());

                    if (buffer[1] == cr)                                        //N received
                    {
                        crlf();
                        restoreSettings();

                        if (!LC2CONTROL.flags.Logging && !LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL.flags.LoggingStopTime) //Logging stopped? 
                        {
                            putsUART1(Loggingstopped);                          //Logging Stopped
                            while (BusyUART1());
                            break;
                        } else {
                            putsUART1(Nexttime2read);                           //Next Time to Read:
                            while (BusyUART1());
                            RTCdata = readClock(RTCAlarm1HoursAddress);         //Display Alarm1 hours
                            displayClock(RTCdata);
                            putcUART1(colon); // :
                            while (BusyUART1());
                            RTCdata = readClock(RTCAlarm1MinutesAddress);       //Display Alarm1 minutes
                            displayClock(RTCdata);
                            putcUART1(colon);
                            while (BusyUART1());
                            RTCdata = readClock(RTCAlarm1SecondsAddress);       //Display Alarm1 seconds
                            displayClock(RTCdata);
                        }
                        break;
                    }

                    break;

                case capO:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    while (BusyUART1());

                    if (buffer[1] == cr)                                        //O<CR> received
                    {
                        controlPortStatus(1);                                   //display control port status     
                        break;
                    }

                    if (buffer[1] == zero && buffer[2] == cr)                   //O0<CR> received
                    {
                        disableOP();                                            
                        break;
                    }

                    if (buffer[1] == one && buffer[2] == cr)                    //O1<CR> received
                    {
                        enableOP();                                             
                        break;
                    }

                    if (buffer[1] == capD && buffer[2] == cr)                   //OD<CR> received
                    {
                        O_D();                                                  
                        controlPortStatus(1);                                   //display control port status   
                        break;
                    }

                    if (buffer[1] == capE && buffer[2] == cr)                   //OE<CR> received
                    {
                        O_E();                                                  
                        controlPortStatus(1);                                   //display control port status   
                        break;
                    }

                    if (buffer[1] == capT && isdigit(buffer[2]))                //Control Port Start time being entered
                    {
                        crlf();
                        PORT_CONTROL.flags.SetAlarm2StopTime=0;                 //make sure stop time flag is clear 
                        PORT_CONTROL.flags.SetAlarm2Time = 1;                   //set the flag
                        Buf2DateTime(buffer);                                   //set the RTC Alarm2 time
                        PORT_CONTROL.flags.SetAlarm2Time = 0;                   //clear the flag
                        controlPortStatus(0);                                   //display Timer status  
                        break;
                    }

                    if (buffer[1] == capP && isdigit(buffer[2]))                //Control Port Stop time being entered
                    {
                        crlf();
                        PORT_CONTROL.flags.SetAlarm2Time=0;                     //make sure on time flag is clear   
                        PORT_CONTROL.flags.SetAlarm2StopTime = 1;               //set the SetAlarm2StopTime flag		
                        Buf2DateTime(buffer);                                   //get Logging Stop Time from buffer
                        PORT_CONTROL.flags.SetAlarm2StopTime = 0;               //clear the SetAlarm2StopTime flag
                        if (LC2CONTROL.flags.ERROR) 
                        {
                            LC2CONTROL.flags.ERROR = 0;                         //clear the ERROR flag
                            PORT_CONTROL.flags.SetAlarm2StopTime = 0;           //clear the Alarm2StopTime flag
                            write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in FRAM  
                            break;
                        }
                        LC2CONTROL2.flags2.SetStopTime = 0;                     //clear the SetStopTime flag

                        write_Int_FRAM(PortOffHoursaddress,PortOffHours);       //store Port OFF time in FRAM 
                        write_Int_FRAM(PortOffMinutesaddress,PortOffMinutes);  
                        controlPortStatus(0);                                   //display Timer status  
                        break;
                    }

                    break;


                case capP:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    while (BusyUART1());

                    if (buffer[1] == cr)                                        //P<CR> received
                    {
                        crlf();
                        if (!DISPLAY_CONTROL.flags.BPD)
                            displayMemoryStatus();                              //display memory status from FRAM
                        else
                            displayTemporaryStatus(tempUserPosition);           //manipulating the pointers

                        break;
                    }

                    if (isdigit(buffer[1])) {
                        memoryStatus=read_Int_FRAM(MemoryStatusaddress);        //get Memory Status (MS) pointer 
                        tempUserPosition = Buffer2Decimal(buffer, i, 0);        //Get the requested pointer position
                        if (tempUserPosition > memoryStatus)                    //invalid input
                            break;
                        DISPLAY_CONTROL.flags.BPD = 1;                          //set the P_D mode flag
                        DISPLAY_CONTROL.flags.newPointer = 1;                   //set the pointer updated flag
                        DISPLAY_CONTROL.flags.Scan = 1;                         //set the Scan flag
                        DISPLAY_CONTROL.flags.Display = 0;                      //clear Display flag if previously enabled
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 
                        displayTemporaryStatus(tempUserPosition);               //display temporary memory status
                        break;
                    }

                    break;


                case capR:                                                      //R received

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    while (BusyUART1());

                    if (buffer[1] == cr)                                        //CR received
                    {
                        crlf();
                        putsUART1(RUsure);                                      //Are you sure(Y/N)?

                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS3bits.T9IF);
                        if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                            handleCOMError();                                   //if communications error

                        response = ReadUART1();                                 //get the char from the USART buffer
                        putcUART1(response);                                    //echo char to terminal					
                        while (BusyUART1());

                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS3bits.T9IF);
                        if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                            handleCOMError();                                   //if communications error

                        RxData = ReadUART1();                                   //RxData contains user response

                        if (response == capY && RxData == cr)                   //yes, so clear memory
                        {
                            R();
                            crlf();
                            putsUART1(MEMcleared);                              //Memory cleared
                            while (BusyUART1());
                            break;
                        } else {
                            crlf();
                            putsUART1(MEMnotcleared);                           //Memory not cleared
                            while (BusyUART1());
                            break;
                        }
                    }

                    if (buffer[1] == capE && buffer[2] == capS && buffer[3] == capE && buffer[4] == capT && buffer[5] == cr) //"RESET" issued
                    {
                        crlf();
                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(RnotAllowed);
                            while (BusyUART1());
                            crlf();
                        } else {
                            putsUART1(Resetting);
                            while (BusyUART1());
                            crlf();                                             
                            RST();                                            
                        }
                    }
                    
                    if (buffer[1] == capS && buffer[2] == capN && buffer[3] == cr) //"RSN" received 
                        ReadSN();                                               //get the gage serial number 
                    //Restore Register values:                                  
                    T6CONbits.T32=0;                                            //Reset TMR6/7 for 16bit mode   
                    PR6=0;                                                      //Reset PR6/7/8   
                    PR7=0;  
                    PR8=0;
                    IEC3bits.T7IE=0;                                            //Disable TMR7 interrupt    
                    IPC12bits.T7IP=4;                                           //Reset TMR7 interrupt priority to back to 4     
                    TMR6=0;                                                     //clear the TMR6 register   
                    TMR7=0;                                                     //clear the TMR7 register  
                    TMR8=0;                                                                 //clear the TMR8 register                    
                    INTCON2bits.ALTIVT=0;                                       //restore to primary interrupt vectors
                    PMD3bits.T6MD=1;                                            //Disable TMR6  
                    PMD3bits.T7MD=1;                                            //Disable TMR7 
                    PMD3bits.T8MD=1;                                            //Enable TMR8
                    break;

                case capS:
                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    while (BusyUART1());

                    if (buffer[1] == cr) //S received
                    {
                        restoreSettings();

                        if ((!DISPLAY_CONTROL.flags.newPointer && !DISPLAY_CONTROL.flags.BPD &&
                                !DISPLAY_CONTROL.flags.Backup && DISPLAY_CONTROL.flags.Scan)
                                ||
                                (!DISPLAY_CONTROL.flags.newPointer && !DISPLAY_CONTROL.flags.BPD &&
                                !DISPLAY_CONTROL.flags.Backup && !DISPLAY_CONTROL.flags.Scan))
                            displayMemoryStatus();                              //Display the memory status from FRAM

                        if ((DISPLAY_CONTROL.flags.newPointer && DISPLAY_CONTROL.flags.BPD &&
                                !DISPLAY_CONTROL.flags.Backup && DISPLAY_CONTROL.flags.Scan)
                                ||
                                (!DISPLAY_CONTROL.flags.newPointer && DISPLAY_CONTROL.flags.BPD &&
                                DISPLAY_CONTROL.flags.Backup && DISPLAY_CONTROL.flags.Scan)) {
                            if (tempUserPosition <= 0)
                                tempUserPosition = 1;
                            displayTemporaryStatus(tempUserPosition);
                        }

                        if (!DISPLAY_CONTROL.flags.newPointer && DISPLAY_CONTROL.flags.BPD &&
                                !DISPLAY_CONTROL.flags.Backup && DISPLAY_CONTROL.flags.Scan)
                            displayTemporaryStatus(id);                         //working with P,B and D commands

                        crlf();

                        
                        if (MUX4_ENABLE.mflags.mux16_4 == Single)
                            putsUART1(MUX1);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW4)                  //Display MUX type  
                            putsUART1(MUX4);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW8) 
                            putsUART1(MUX8VW);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW16)
                            putsUART1(MUX16);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW32) 
                            putsUART1(MUX32VW);
                        if (MUX4_ENABLE.mflags.mux16_4 == TH8) 
                            putsUART1(MUX8);
                        if (MUX4_ENABLE.mflags.mux16_4 == TH32) 
                            putsUART1(MUX32);


                        while (BusyUART1());
                        crlf();

                        putsUART1(Scaninterval);                                //Scan interval:
                        while (BusyUART1());
                        ScanInterval = hms2s();                                 //convert h,m,s to s
                        displayScanInterval(ScanInterval, 1);                   //display scan interval in seconds


                        if (LC2CONTROL.flags.Logging && !LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL2.flags2.Waiting) 
                        {
                            crlf();                                             //display logging status
                            putsUART1(Loggingstarted);                          //Logging Started
                        }

                        if (!LC2CONTROL.flags.Logging && !LC2CONTROL.flags.LoggingStopTime &&!LC2CONTROL.flags.LoggingStartTime) {
                            crlf();                                             //display logging status
                            putsUART1(Loggingstopped);                          //Logging Stopped
                        }
                        while (BusyUART1());


                        if (LC2CONTROL2.flags2.Waiting)                         //display logging start time	
                        {
                            crlf();
                            displayLoggingWillStart();
                            while (BusyUART1());                                    
                        }

                        if (LC2CONTROL.flags.LoggingStopTime)                   //display logging stop time
                        {
                            LC2CONTROL2.flags2.SetStopTime = 1;                 //set the flag to format the stop time
                            crlf();
                            displayLoggingWillStop();
                            while (BusyUART1());                                    
                            LC2CONTROL2.flags2.SetStopTime = 0;                 //reset the flag
                        }
                        

                        crlf();                                                 //display log interval status

                        if (LC2CONTROL.flags.LogInterval)
                            putsUART1(Logenabled);
                        else
                            putsUART1(Logdisabled);
                        while (BusyUART1());

                        crlf();                                                 //display monitor status

                        if (LC2CONTROL.flags.Monitor)
                            putsUART1(Monitorenabled);
                        else
                            putsUART1(Monitordisabled);
                        while (BusyUART1());

                        break;
                    }

                    if (buffer[1] == capC && buffer[2] == cr)                   //SC received
                    {
                        crlf();
                        putsUART1(Scaninterval);                                //Scan Interval:
                        while (BusyUART1());
                        ScanInterval = hms2s();                                 //convert h,m,s to s
                        displayScanInterval(ScanInterval, 1);                   //display scan interval in seconds
                        break;
                    }

                    if (buffer[1] == capC && buffer[2] != cr)                   //Enter Scan Interval
                    {
                        crlf();
                        testScanInterval = 0;                                   //set testScanInterval to 0
                        LC2CONTROL.flags.ScanError = 0;                         //clear the flag	

                        if ((buffer[2] >= zero && buffer[2] <= nine) && buffer[3] == cr) //SC#<CR>?
                        {
                            ScanInterval = buffer[2] - 0x30;                    //convert to integer
                            testScanInterval = checkScanInterval();             //test for minimum allowable Scan Interval
                        }

                        if ((buffer[2] >= zero && buffer[2] <= nine) && (buffer[3] >= zero && buffer[3] <= nine) && buffer[4] == cr) //SC##<CR>?
                        {
                            tensScanInterval = (buffer[2] - 0x30)*10;           //scan interval 10's convert to integer
                            ScanInterval = buffer[3] - 0x30;                    //scan interval 1's convert to integer
                            ScanInterval += tensScanInterval;                   //add 10's to 1's
                            testScanInterval = checkScanInterval();             //test for minimum allowable Scan Interval
                        }

                        if (testScanInterval)                                   //if error
                        {
                            putsUART1(Minscanerror);                            //ERROR: Minumum Scan Interval for this configuration is :
                            while (BusyUART1());
                            sprintf(trapBUF, "%d", testScanInterval);           //minimum scan interval in seconds
                            putsUART1(trapBUF);
                            while (BusyUART1());
                            putsUART1(Seconds);
                            while (BusyUART1());
                        } else                                                  //scan interval is ok
                        {
                            hms(Buffer2Decimal(buffer, i, 1), 0);               //convert to hours,minutes,seconds
                            ScanInterval = hms2s();                             //convert h,m,s to s
                            putsUART1(Scaninterval);                            //Scan Interval:
                            while (BusyUART1());
                            displayScanInterval(ScanInterval, 1);               //display scan interval in seconds

                            if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) //is datalogger logging?
                            {
                                DISPLAY_CONTROL.flags.Shutup = 1;               //don't allow message display
                                stopLogging();                                  //stop and restart logging if it is	
                                if (DISPLAY_CONTROL.flags.Synch)
                                    VWflagsbits.synch = 1;                      //set the synch flag
                                crlf();
                                startLogging();
                                DISPLAY_CONTROL.flags.Shutup = 0;
                            }
                        }

                        for (i = 0; i < 30; i++)                                //clear the buffer
                        {
                            buffer[i] = 0;
                        }

                        break;
                    }

                    //***************************************Stop Logging****************************************************

                    if (buffer[1] == capP && buffer[2] == cr) //SP received
                    {
                        if(STOP())                                              
                            crlf();
                        break;                                                  
                    }

                    if (buffer[1] == capP && isdigit(buffer[2]))                //Enter Stop Logging Time
                    {
                        if (LC2CONTROL.flags.LogInterval) 
                        {
                            crlf();
                            putsUART1(StopTimeNotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        crlf();

                        if (!LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL.flags.Logging) 
                        {
                            putsUART1(Loggingalreadystopped);                   //Logging already stopped!
                            while (BusyUART1());
                            break;
                        }
                        
                        STOPTIME();                                             
                        break;
                    }

                    //**********************************************System Status************************************************
                    if (buffer[1] == capS && buffer[2] == cr) {
                        crlf();
                        putsUART1(Trapcount);                                   //Display value in Trapcount 
                        while (BusyUART1());
                        trap=read_Int_FRAM(TrapRegisteraddress);                //TEST LC2MUX   
                        sprintf(trapBUF, "%d", trap);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        crlf();
                        
                        NAdata=read_Int_FRAM(MODBUSaddress);                    //read MODBUS Address from FRAM  
                        putsUART1(ModbusaddressIS);                             //"MODBUS Address:"
                        while (BusyUART1());
                        sprintf(NABUF, "%d", NAdata);                           //format MODBUS address
                        putsUART1(NABUF);                                       //display MODBUS address
                        while (BusyUART1());
                        crlf();

                        if (LC2CONTROL.flags.TimeFormat)                        //0=hhmm format
                        {                                                       //1=hh,mm format
                            putsUART1(Timeformatcomma);
                        } else {
                            putsUART1(Timeformat);
                        }
                        while (BusyUART1());
                        crlf();

                        if (LC2CONTROL.flags.DateFormat)                        //0=julian date format
                        {                                                       //1=month/day format
                            putsUART1(Datemonthday);
                        } else {
                            putsUART1(Datejulian);
                        }
                        while (BusyUART1());
                        
                        if(MUX4_ENABLE.mflags.mux16_4==Single)                  //if Single Channel   
                        {
                            crlf();												
                            Thermtype=read_Int_FRAM(CH1THaddress);              //read thermistor type	
                            if(Thermtype==3)                                    //3=High Temp 103JL1A thermistor	REV 1.10
                            {													
                                putsUART1(Hightemp3);                           //High temp 103JL1A thermistor selected REV 1.10						
                            }
                            else
                            if(Thermtype==2)                                    //2=High Temp BR55KA822J thermistor REV 1.10
                            {
                                putsUART1(Hightemp2);                           //High temp BR55KA822J thermistor selected
                            }
                            else
                            {
                                putsUART1(Hightemp1);                           //Standard themistor selected   REV 1.10
                            }                        

                            while(BusyUART1());                                     
                        }
                        break;
                    }


                    //**********************************************Start Logging************************************************
                    if (buffer[1] == capT && buffer[2] == cr)                   //ST received
                    {
                        if(START())                                             
                            return;                                             
                        break;                                                  
                    }


                    if (buffer[1] == capT && buffer[2] != cr)                   //Enter Start Logging Time
                    {
                        crlf();

                        testScanInterval = 0;

                        ScanInterval = hms2s();                                 //get the stored scan interval
                        testScanInterval = checkScanInterval();                 //test for minimum allowable Scan Interval

                        if (testScanInterval)                                   //if error
                        {
                            if (LC2CONTROL.flags.LogInterval)                   //Log Intervals?
                            {
                                putsUART1(MinInterror);                         //ERROR: Minimum Log Interval Length for this configuration is :
                            } else {
                                putsUART1(Minscanerror);                        //ERROR: Minimum Scan Interval for this configuration is :
                            }

                            while (BusyUART1());
                            sprintf(trapBUF, "%d", testScanInterval);           //minimum scan interval in seconds
                            putsUART1(trapBUF);
                            while (BusyUART1());
                            putsUART1(Seconds);
                            while (BusyUART1());
                            break;
                        }

                        LC2CONTROL.flags.LoggingStartTime = 1;
                        LC2CONTROL.flags.Logging = 0;
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM 
                        S_1.status1flags._ST=1;                                 //set the MODBUS status flag    
                        write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                          
                        LC2CONTROL2.flags2.SetStartTime = 1;                    //Going to set Start time	
                        DISPLAY_CONTROL.flags.Synch = 0;                        //make sure Synch flag is clear	
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM  
                        DISPLAY_CONTROL.flags.firstTime=1;                      
                        PORT_CONTROL.flags.SetAlarm2StopTime=0;                 
                        PORT_CONTROL.flags.SetAlarm2Time=0;                     
                        Buf2DateTime(buffer);                                   //set RTC Alarm time
                        DISPLAY_CONTROL.flags.firstTime=0;                      

                        if (LC2CONTROL.flags.ERROR) {
                            LC2CONTROL.flags.ERROR = 0;                         //clear the ERROR flag
                            LC2CONTROL.flags.LoggingStartTime = 0;              //clear the LoggingStartTime flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM 
                            S_1.status1flags._ST=0;                             //clear the MODBUS status flag    
                            write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                              
                            LC2CONTROL2.flags2.SetStartTime = 0;                //clear the SetStartTime flag
                            break;
                        }

                        LC2CONTROL2.flags2.SetStartTime = 0;                    //clear the set start time flag
                        putsUART1(Loggingwillstart);                            //Logging will start at:
                        while (BusyUART1());

                        Alarm1HoursValue = readClock(RTCAlarm1HoursAddress);    //Display Alarm1 hours MAY BE ABLE TO MAKE FUNCTION HERE
                        displayClock(Alarm1HoursValue);
                        putcUART1(colon); // :
                        while (BusyUART1());

                        Alarm1MinutesValue = readClock(RTCAlarm1MinutesAddress); //Display Alarm1 minutes
                        displayClock(Alarm1MinutesValue);
                        putcUART1(colon);
                        while (BusyUART1());

                        Alarm1SecondsValue = readClock(RTCAlarm1SecondsAddress); //Display Alarm1 seconds
                        displayClock(Alarm1SecondsValue);
                        Alarm1SecondsLong = bcd2d(Alarm1SecondsValue);          //convert to decimal from BCD
                        Alarm1MinutesLong = bcd2d(Alarm1MinutesValue);
                        Alarm1HoursLong = bcd2d(Alarm1HoursValue);

                        TotalStartSeconds = ((Alarm1HoursLong * 3600)+(Alarm1MinutesLong * 60) + Alarm1SecondsLong); //compute total start seconds
                        write_longFRAM(TotalStartSeconds,TotalStartSecondsaddress);	//store to FRAM   
                        TotalStopSeconds=read_longFRAM(TotalStopSecondsaddress);		//get the stored stop time  

                        if (TotalStopSeconds < TotalStartSeconds)               //midnight rollover?
                        {
                            TotalStopSeconds += 86400;                          //compensate
                            write_longFRAM(TotalStopSeconds,TotalStopSecondsaddress);	//store to FRAM   
                        }

                        LoggingStartDays = readClock(RTCDaysAddress);           //Get and store the current day 
                        LoggingStartDay = RTCtoDecimal(LoggingStartDays);
                        write_Int_FRAM(LoggingStartDayaddress,LoggingStartDay);    

                        LC2CONTROL.flags.Logging = 1;                           //set the Logging flag	
                        LC2CONTROL.flags.LoggingStartTime = 0;                  //clear the Logging Start Time flag	
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flags in FRAM     

                        VWflagsbits.firstReading=1;                             
                        LC2CONTROL2.flags2.Waiting = 1;                         //Set the Waiting flag	
                        LC2CONTROL2.flags2.scheduled=1;                         
                        write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flags in FRAM   
                        IFS1bits.INT1IF = 0;                                    //clear the interrupt flag  
                        IFS1bits.INT2IF = 0;                                    //clear the interrupt flag  
                        setClock(0x0F, 0);                                      //Clear the RTC Alarm flags     
                        enableAlarm(Alarm1);                                    
                        enableINT1();                                           //enable INT1 (take a reading on interrupt) 
                        INTCON1bits.NSTDIS = 0;                                 //reset nesting of interrupts   	                        
                        break;                                                  
                    }


                    //*******************************************Software Version*****************************************************

                    if (buffer[1] == capV && buffer[2] == cr)                   //display software revision
                    {
                        crlf();
                        putsUART1(Softwareversion);                             //Software version:
                        while (BusyUART1());
                        //putsUART1(Rev);                                       REM REV 1.11
                        //while (BusyUART1());                                  REM REV 1.11
                        putsUART1(RevMAJOR);                                    //REV 1.11
                        while(BusyUART1());                                     //REV 1.11
                        putcUART1(decimal);                                     //REV 1.11
                        while(BusyUART1());                                     //REV 1.11
                        putsUART1(RevMINOR);                                    //REV 1.11
                        while(BusyUART1());                                     //REV 1.11
                        break;
                    }

                    //**********************************************Synchronize Readings**********************************************
                    if (buffer[1] == capR && buffer[2] == cr)                   //SR received
                    {
                        crlf();

                        if (DISPLAY_CONTROL.flags.Synch)                        //If readings will be synchronized
                        {
                            putsUART1(Synch);                                   //"Readings are synchronized to the top of the hour"
                        } else {
                            putsUART1(Synchnot);                                //"Readings are not synchronized to the top of the hour"
                        }
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capR && buffer[2] == zero && buffer[3] == cr) //SR0 received
                    {
                        if(!LC2CONTROL2.flags2.Modbus)                          //if command line interface 
                            crlf();
                        synch_zero();                                                        
                        break;
                    }

                    if (buffer[1] == capR && buffer[2] == one && buffer[3] == cr) //SR1 received
                    {
                        if(!LC2CONTROL2.flags2.Modbus)                          //if command line interface 
                            crlf();
                        synch_one();                                            

                        break;
                    }


                case capT:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer   
                    crlf();

                    if (buffer[1] == capR && buffer[2] == cr)                   //TR<CR> received
                    {
                        putsUART1(Trapcount);                                   //Display value in Trapcount
                        while (BusyUART1());
                        trap=read_Int_FRAM(TrapRegisteraddress); 
                        sprintf(trapBUF, "%d", trap);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capR && buffer[2] == zero && buffer[3] == cr) //reset Trap Counter
                    {
                        trap = 0;                                               //reset trap counter
                        write_Int_FRAM(TrapRegisteraddress,trap);  
                        putsUART1(Trapcount);                                   //Display value in Trapcount
                        while (BusyUART1());
                        sprintf(trapBUF, "%d", trap);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[2] == cr)                   //TF<CR> received
                    {
                        if (!LC2CONTROL.flags.TimeFormat)                       //0=hhmm
                            putsUART1(Timeformat);
                        else
                            putsUART1(Timeformatcomma);                         //1=hh,mm
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[3] == cr)                   //TF#<CR> received
                    {
                        if (buffer[2] == zero)                                  //hhmm format chosen
                        {
                            LC2CONTROL.flags.TimeFormat = 0;
                            putsUART1(Timeformat);
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        if (buffer[2] == one)                                   //hh,mm format chosen
                        {
                            LC2CONTROL.flags.TimeFormat = 1;
                            putsUART1(Timeformatcomma);
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        while (BusyUART1());
                        break;
                    }

                    //******************************************TEST MENU*********************************************************

                    if (buffer[1] == capE && buffer[2] == capS && buffer[3] == capT) //TEST<CR> received
                    {
                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(TnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        while (testmenuBUF[0] != capX) 
                        {
                            for (i = 0; i < 2; i++)                             //clear the buffer
                            {
                                testmenuBUF[i] = 0;
                            }

                            for (i = 0; i < 22; i++)                            //clear the buffer
                            {
                                buffer[i] = 0;
                            }

                            i = 0;                                              //initialize char buffer index

                            RxData = 0;                                         //initialize receive buffer

                            displayTestMenu();                                  //display the test choices

                            while (RxData != cr)                                //put chars in buffer until <CR> is received
                                while (1)                                       //put chars in buffer until <CR> is received
                                {
                                    while (!DataRdyUART1() && !U1STAbits.FERR   //wait for choice
                                            && !U1STAbits.PERR && !U1STAbits.OERR);
                                    if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                                        handleCOMError();

                                    RxData = ReadUART1();                       //get the char from the USART buffer

                                    if (RxData != 0x08 && RxData != cr) {
                                        putcUART1(RxData);                      //echo char (except backspace & <CR>) to terminal
                                        while (BusyUART1());
                                        putcUART1(0x08);                        //restore cursor to original location
                                        while (BusyUART1());
                                        testmenuBUF[0] = RxData;                //store the received char in the buffer
                                    }

                                    if (testmenuBUF[0] != 0 && RxData == cr)    //break out of loop if selection made
                                        break;
                                }


                            switch (testmenuBUF[0])                             //perform the test
                            {

                                case one:                                       //test ALL FRAM DURATION: 1m 35s/FRAM
                                    
                                    crlf();
                                    testData = testFRAM(1); //test external FRAM bank 1

                                    crlf();
                                    testData = testFRAM(2); //test external FRAM bank 2

                                    crlf();
                                    testData = testFRAM(3); //test external FRAM bank 3

                                    crlf();
                                    testData = testFRAM(4); //test external FRAM bank 4

                                    crlf();
                                    RxData = 0;
                                    break;

                                case two:                                       //test +3V_X
                                    ADPCFG = 0;                                 //all analog channels

                                    pluckOFF();
                                    _EXC_EN=1;
                                    _3VX_on();                                  //power-up analog circuitry
                                    crlf();
                                    putsUART1(Poweron);                         //display "Analog power supplies on"
                                    while (BusyUART1());
                                    IFS0bits.U1RXIF = 0;                        //clear the UART1 interrupt flag
                                    while (!IFS0bits.U1RXIF);                   //wait until a key is pressed
                                    
                                    if (IFS0bits.U1RXIF)                        
                                        IFS0bits.U1RXIF = 0;                    //clear the UART1 interrupt flag
                                    RxDataTemp = ReadUART1();                   //get the char from the UART buffer to clear it
                                    _3VX_off();                                 //power-down the analog circuitry
                                    setADCsleep();                              
                                    putcUART1(0x0D);                            //<CR>
                                    while (BusyUART1());
                                    putsUART1(Poweroff);                        //display "Analog power supplies off"
                                    while (BusyUART1());
                                    RxData = 0;
                                    break;

                                case three:                                     //test RTC 32KHz
                                    crlf();
                                    putsUART1(RTC32KHZON);                      //display "RTC 32KHz output on."
                                    while (BusyUART1());
                                    start32KHz();                               //start the RTC 32KHz output
                                    IFS0bits.U1RXIF = 0;                        //clear the UART1 interrupt flag
                                    while (!IFS0bits.U1RXIF);                   //wait until a key is pressed
                                    
                                    IFS0bits.U1RXIF = 0;                        //clear the UART1 interrupt flag
                                    RxDataTemp = ReadUART1();                   //get the char from the UART buffer to clear it
                                    stop32KHz();                                //stop the RTC 32KHz output
                                    putcUART1(0x0D);                            //<CR>
                                    while (BusyUART1());
                                    putsUART1(RTC32KHZOFF);                     //display "RTC 32KHz output off."
                                    while (BusyUART1());
                                    RxData = 0;
                                    break;
                                    
                                case four:                                      //Test Excitation    
                                    crlf();
                                    putsUART1(_5VEXCON);                        //display "Excitation on."
                                    while (BusyUART1());   
                                    
                                    pluckON();                                   
                                    _3VX_on();                                  //power-up analog circuitry  
                                    IFS0bits.U1RXIF = 0;                        //clear the UART1 interrupt flag
                                    
                                    while (!IFS0bits.U1RXIF)                    //wait until a key is pressed
                                    {
                                        pluckPOS();
                                        delay(1220);                            
                                        pluckNEG();
                                        delay(1220);                               
                                    }
                                    IFS0bits.U1RXIF = 0;                        //clear the UART1 interrupt flag
                                    RxDataTemp = ReadUART1();                   //get the char from the UART buffer to clear it
                                                                        
                                    pluckOFF();                                 //turn off the pluck drivers
                                    _3VX_off();                                 //power-down the analog circuitry
                                    putcUART1(0x0D);                            //<CR>
                                    while (BusyUART1());
                                    putsUART1(_5VEXCOFF);                       //display "Excitation off."
                                    while (BusyUART1());
                                    RxData = 0;
                                    break;
                                    
                                case five:                                      
                                    ADPCFG = 0;                                 //all analog channels
                                    pluckOFF();
                                 
                                    while(testmenuBUF[0]!=capX)
                                    {
                                        crlf();                                     
                                        putsUART1(_selectGT);                       
                                        while(BusyUART1());                         
                                    
                                        for (i = 0; i < 2; i++)                 //clear the buffer  
                                        {
                                            testmenuBUF[i] = 0;
                                        }
                                        i=0;
                                        
                                        while (1)                               //put chars in buffer until <CR> is received  
                                        {
                                            while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR); 

                                            RxData = ReadUART1();               //get the char from the USART buffer
                                            if (RxData != cr && RxData != escape) 
                                            {
                                                putcUART1(RxData);              //echo char (except <CR>) to terminal
                                                while (BusyUART1());
                                            }

                                            if (RxData == 0x08)                 //decrement buffer index if backspace is received
                                            {
                                                i--;
                                                putcUART1(space);               //clear the previous character
                                                while (BusyUART1());
                                                putcUART1(0x08);
                                                while (BusyUART1());
                                            }

                                            if (i < 0)                          //don't let buffer index go below zero
                                                i = 0;

                                            if (RxData != 0x08 && i < 3)        //as long as char is not backspace and index is <2	
                                            {
                                                testmenuBUF[i] = RxData;        //store the received char in the buffer
                                                i++;
                                                if (i > 2)
                                                    i = 0;
                                            }

                                            if (RxData == cr)                   //CR received - buffer is ready
                                            {
                                                RxData = 0;                     //clear the receive buffer
                                                break;
                                            }
                                        }
                                            if ((testmenuBUF[0] == one | testmenuBUF[0]==two | testmenuBUF[0]==three |
                                                    testmenuBUF[0]== four | testmenuBUF[0]==five | testmenuBUF[0]==six |
                                                    testmenuBUF[0]==capX) &&
                                                    testmenuBUF[1]==cr)
                                            {
                                                break;
                                            }
                                            else
                                            {
                                                crlf();
                                                putsUART1(_wrong);
                                                while(BusyUART1());
                                                for (i = 0; i < 2; i++)         //clear the buffer  
                                                {
                                                    testmenuBUF[i] = 0;
                                                }
                                            }
                                                                            
                                    }
                          
                                    if(testmenuBUF[0]==capX)                    //exit back to main test menu
                                    {
                                        testmenuBUF[0]=0;
                                        break;
                                    }
                                    
                                    crlf();
                                    switch(testmenuBUF[0])
                                    {
                                        case one:
                                            putsUART1(_VWON1);                  //display "VW Channel on: GageType 1 (1400-3500Hz) selected."
                                            break;
                                        case two:
                                            putsUART1(_VWON2);                  //display "VW Channel on: GageType 2 (2800-4500Hz) selected."
                                            break;
                                        case three:
                                            putsUART1(_VWON3);                  //display "VW Channel on: GageType 3 (400-1200Hz) selected."
                                            break;
                                        case four:
                                            putsUART1(_VWON4);                  //display "VW Channel on: GageType 4 (1200-2800Hz) selected."
                                            break;
                                        case five:
                                            putsUART1(_VWON5);                  //display "VW Channel on: GageType 5 (2500-4500Hz) selected."
                                            break;
                                        case six:
                                            putsUART1(_VWON6);                  //display "VW Channel on: GageType 6 (800-1600Hz) selected."
                                            break;
                                        default:
                                            break;
                                    }
                                    while (BusyUART1());
                                    crlf();
                                    _3VX_on();                                  //power-up analog circuitry 
                                    LC2CONTROL2.flags2.uCclock=1;               //set flag for HSPLL osc    
                                    enableVWchannel(testmenuBUF[0]-0x30);                      
                                    IFS0bits.U1RXIF = 0;                        //clear the UART1 interrupt flag
                                    while (!IFS0bits.U1RXIF);                    //wait until a key is pressed
                                    IFS0bits.U1RXIF = 0;                        //KEY PRESSED - clear the UART1 interrupt flag
                                    RxDataTemp = ReadUART1();                   //get the char from the UART buffer to clear it
                                    disableVWchannel();                         //turn off the VW channel
                                    LC2CONTROL2.flags2.uCclock=0;               //Reset the flag    
                                    _3VX_off();                                 //power-down the analog circuitry 
                                    putcUART1(0x0D);                            //<CR>
                                    while (BusyUART1());
                                    putsUART1(_VWOFF);                          //display "VW Channel off"
                                    while (BusyUART1());
                                    RxData = 0;
                                    crlf();
 
                                    break;
                                    
                                case capX:
                                    RxData = 0;
                                    break;                                      //exit Test Menu

                                default:
                                    RxData = 0;
                                    break;
                            }                                                   //end of switch(buffer[0])

                            if (RxData == 0) {
                                delay(1280000);                                 
                                crlf();                                         //put space between tests
                                crlf();
                                crlf();
                            }
                        }                                                       //end of while(testmenuBUF[0]!=capX)
                        testmenuBUF[0] = 0;                                     //clear the buffer
                        _3VX_off();                                                                       
                        break;
                    }                                                           //end of if(buffer[1]==capE && buffer[2]==capS && buffer[3]==capT)
                    
					if((isdigit(buffer[1]) && buffer[2]==slash) | (isdigit(buffer[1]) && isdigit(buffer[2]) && 	buffer[3]==slash))
							channel=getChannel();                               //get the current channel #		

                    if((channel>1&&MUX4_ENABLE.mflags.mux16_4==Single) |
                            (channel>4&&MUX4_ENABLE.mflags.mux16_4==VW4) |
                            (channel>8&&MUX4_ENABLE.mflags.mux16_4==VW8) |
                            (channel>8&&MUX4_ENABLE.mflags.mux16_4==TH8) |
                            (channel>16&&MUX4_ENABLE.mflags.mux16_4==VW16) |
                            (channel>32&&MUX4_ENABLE.mflags.mux16_4==VW32) |
                            (channel>32&&MUX4_ENABLE.mflags.mux16_4==TH32))
                        break;                                                  //return to '*' prompt
                    
					Thermtype=getThermtype();                                   //Get the thermistor type

                    if(Thermtype==0 | Thermtype==1 | Thermtype==2 | Thermtype==3)       
                        setChannelThermtype(channel,Thermtype);                 //Assign the Thermistor type to the channel		

					displayMUX(channel);

                    break;                                                    

                    //***********************************************************************************************************


                case capV:                                                      //Lithium Cell Voltage

                    if (buffer[1] == capL && buffer[2] == cr)                   //VL<CR>
                    {
                        crlf();
                        lithBatreading = take_analog_reading(95);               //get the lithium coin cell voltage
                        write_Int_FRAM(BatteryReading,lithBatreading);          
                        putsUART1(LithiumVoltage);                              //display "Lithium Cell Voltage ="
                        Lithiumreading = (((Vref * lithBatreading) / 4096.0) * mullith3V); //convert to voltage	lithBatreading; 
                        while (BusyUART1());
                        sprintf(trapBUF, "%.2f", Lithiumreading);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        putcUART1(capV);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == three && buffer[2] == cr)                  //V3<CR>
                    {
                        crlf();
                        mainBatreading = take_analog_reading(97);               //get the 3V Battery Voltage
                        write_Int_FRAM(BatteryReading,mainBatreading);          
                        putsUART1(THREEV);                                      //display "3V Battery Voltage ="
                        batteryReading = (((Vref * mainBatreading) / 4096.0) * mul3V); //convert to voltage	lithBatreading; 
                        while (BusyUART1());
                        sprintf(trapBUF, "%.2f", batteryReading);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        putcUART1(capV);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == one && buffer[2] == two && buffer[3] == cr) //V12<CR>
                    {
                        crlf();
                        mainBatreading = take_analog_reading(87);               //get the 12V Battery Voltage
                        write_Int_FRAM(BatteryReading,mainBatreading);          
                        putsUART1(TWELVEV);                                     //display "12V Battery Voltage ="
                        batteryReading = (((Vref * mainBatreading) / 4096.0) * mul12V); //convert to voltage    V
                        while (BusyUART1());
                        sprintf(trapBUF, "%.2f", batteryReading);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        putcUART1(capV);
                        while (BusyUART1());
                    }

                    break;


                case capW:                                                      //Wrap Format

                    if (buffer[1] == capF && buffer[2] == zero && buffer[3] == cr) //stop logging when memory full
                    {
                        crlf();
                        wrap_zero();                                            
                        putsUART1(Loggingwillstopmemfull);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[2] == one && buffer[3] == cr) //wrap memory when full
                    {
                        crlf();
                        wrap_one();                                             
                        putsUART1(Loggingwillnotstopmemfull);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[2] == cr)                   //WF<CR> received
                    {
                        crlf();
                        shutdownTimer(TimeOut);                                 //Reset 15S timer   
                        if (DISPLAY_CONTROL.flags.WrapMemory)
                            putsUART1(Loggingwillnotstopmemfull);
                        else
                            putsUART1(Loggingwillstopmemfull);
                        while (BusyUART1());
                        break;
                    }

                    break;


                case capX:                                                      //take and display, but don't store a reading

                    if (buffer[1] == cr) 
                    {
                        crlf();
                        X();                                                    
                    }

                    break;

                case colon:                                                     //Enter MODBUS communications :::<cr>  
                    
                    if (buffer[1] == colon && buffer[2] == colon && buffer[3]==cr)
                    {
                        crlf();
                        putsUART1(SwitchtoModbus);                              //"Switch to MODBUS communications."    
                        while (BusyUART1());
                        
                        crlf();
                        putsUART1(RUsure);                                      //Are you sure(Y/N)?
                        while (BusyUART1());
                        
                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS3bits.T9IF); //wait for user response	
                        if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                            handleCOMError();                                   //if communications error

                        response = ReadUART1();                                 //get the char from the USART buffer
                        putcUART1(response);                                    //echo char to terminal
                        while (BusyUART1());

                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS3bits.T9IF); //wait for <CR>	
 
                        RxData = ReadUART1();                                   //RxData contains user response

                        if (response == capY && RxData == cr)                   //yes, so reboot into MODBUS
                        {
                            LC2CONTROL2.flags2.Modbus=1;                        //set the Modbus flag
                            write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flag in FRAM`
                            asm("RESET");                                       //reboot
                        } 
                        else 
                        {
                            crlf();
                            break;
                        }

                    }
                    
                    break;
                
            }                                                                   //end of switch

            prompt();                                                       
            
            for (i = 0; i < 22; i++)                                            //clear the buffer
            {
                buffer[i] = 0;
            }

            i = 0;                                                              //re-initialize buffer pointer


        }                                                                       //end of 2nd while(!IFS3bits.T9IF)

    }                                                                           //end of 1st while(!IFS3bits.T9IF)
    
}                                                                               //end of CMDcomm()


void CMD_LINE(void)
{
    LC2CONTROL2.flags2.Modbus=0;                                                //clear the Modbus flag
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);                  //store flag in FRAM`
    _RS485RX_EN=1;                                                              //Disable RS485 Rx  
    asm("RESET");    
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


void configShutdownTimer(void)
{
    //Configure Timer8/9 as 32 bit timer                                        
	T8CONbits.TON=0;                                                            //make sure Timer 8 is off
	T9CONbits.TON=0;                                                            //make sure Timer 9 is off
	T8CONbits.T32=1;                                                            //Setup for 32 bit timer
	TMR9HLD=0;                                                                  //clear Timer 9 holding register
	TMR9=0;                                                                     //clear Timer 9 register
	TMR8=0;                                                                     //clear Timer 8 register    
}



void configTimers(void)                                                         
{
    //Enable Timers:
    PMD1bits.T1MD=0;                                                            //Timer 1
    PMD1bits.T4MD=0;                                                            //Timer 4
    PMD1bits.T5MD=0;                                                            //Timer 5
    PMD3bits.T7MD=0;                                                            //Timer 7
    PMD3bits.T8MD=0;                                                            //Timer 8   
    PMD3bits.T9MD=0;                                                            //Timer 9   
    
    //Configure Timer7 as 16 bit counter with no prescaler and T7CK as input
    T7CONbits.TON=0;                                                            //make sure Timer 7 is off
    T7CONbits.TCKPS=0;                                                          //setup for no prescale  
    T7CONbits.TSIDL=0;                                                          //continue operation in idle
    T7CONbits.TGATE=0;                                                          //no timer gate operation
    T7CONbits.TCS=0;                                                            //count internal Tcy clocks
    TMR7=0;                                                                     //clear Timer 7 register

	//Configure Timer4/5 as 32 bit timer
	T4CONbits.TON=0;                                                            //make sure Timer 4 is off
	T5CONbits.TON=0;                                                            //make sure Timer 5 is off
	T4CONbits.T32=1;                                                            //Setup for 32 bit timer
	TMR5HLD=0;                                                                  //clear Timer 5 holding register
	TMR5=0;                                                                     //clear Timer 5 register
	TMR4=0;                                                                     //clear Timer 4 register
    IEC1bits.T5IE=1;                                                            //enable Timer 5 interrupt    
}



void configUARTnormal(void) 
{
    unsigned int U1MODEvalue;
    unsigned int U1STAvalue;

    U1MODEvalue = UART_EN &
            UART_IDLE_CON &
            UART_IrDA_DISABLE &
            UART_MODE_SIMPLEX &
            UART_UEN_00 &
            UART_EN_WAKE &
            UART_DIS_LOOPBACK &
            UART_DIS_ABAUD &
            UART_BRGH_FOUR &
            UART_NO_PAR_8BIT &
            UART_1STOPBIT;

    U1STAvalue = UART_INT_TX_BUF_EMPTY &
            UART_IrDA_POL_INV_ZERO &
            UART_SYNC_BREAK_DISABLED &
            UART_TX_ENABLE &
            UART_INT_RX_CHAR &
            UART_ADR_DETECT_DIS &
            UART_RX_OVERRUN_CLEAR;
    
    ConfigIntUART1(UART_RX_INT_EN &                                             //configure the UART interrupt                          
            UART_RX_INT_PR2 &                                                   //priority 1 higher than CPU priority
            UART_TX_INT_DIS &
            UART_TX_INT_PR5);

    OpenUART1(U1MODEvalue, U1STAvalue, baudrate);
    U1MODEbits.URXINV=0;                                                        //idle high
    if(baudrate==brg9600)                                                       
        U1MODEbits.BRGH=0;                                                      
    else                                                                        
        U1MODEbits.BRGH=1;                                                      //low speed mode

    SRbits.IPL=4;                                                               //Set CPU Interrupt priority to 4                           
    U1MODEbits.WAKE=1;                                                          //Set the WAKE bit  
    U1STAbits.FERR = 0;                                                         //clear UART error flags
    U1STAbits.PERR = 0;
    U1STAbits.OERR = 0;
}

void controlPortStatus(unsigned char x)                                         
{
    char eeBUF[6];                                                              //temporary storage for EE values

    crlf();
    PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress);   

    if(x)                                                                       
    {
        if (CONTROL)                                                            //Port is currently ON
        {
            putsUART1(CPON);                                                    //"Control Port ON"
        } else {
            putsUART1(CPOFF);                                                   //"Control Port OFF"
        }
        while (BusyUART1());
        crlf();
    }

    if(x)
    {
        if (PORT_CONTROL.flags.PortTimerEN)                                     //Port Timer Enabled
        {
            putsUART1(CPTIMEREN);                                               //"Control Port Timer Enabled."
        } else {
            putsUART1(CPTIMERDIS);                                              //"Control Port Timer Disabled."
        }
        while (BusyUART1());
        crlf();
    }

    putsUART1(CPTIMERACT);                                                      //"Timer activate at "
    while (BusyUART1());

    PortOnHours=read_Int_FRAM(PortOnHoursaddress);   
    if (PortOnHours < 10)                                                       //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(eeBUF, "%d", PortOnHours);                                          //format the hour data
    putsUART1(eeBUF);                                                           //display it
    while (BusyUART1());
    putcUART1(colon);                                                           // :
    while (BusyUART1());
    PortOnMinutes=read_Int_FRAM(PortOnMinutesaddress);   
    if (PortOnMinutes < 10)                                                     //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(eeBUF, "%d", PortOnMinutes);                                        //format the minute data
    putsUART1(eeBUF);                                                           //display it
    while (BusyUART1());

    crlf();

    putsUART1(CPTIMERDEACT);                                                    //"Control Port Timer deactivate at "
    while (BusyUART1());
    PortOffHours=read_Int_FRAM(PortOffHoursaddress); 
    if (PortOffHours < 10)                                                      //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(eeBUF, "%d", PortOffHours);                                         //format the hour data
    putsUART1(eeBUF);                                                           //display it
    while (BusyUART1());
    putcUART1(colon);                                                           // :
    while (BusyUART1());
    PortOffMinutes=read_Int_FRAM(PortOffMinutesaddress); 
    if (PortOffMinutes < 10)                                                    //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(eeBUF, "%d", PortOffMinutes);                                       //format the minute data
    putsUART1(eeBUF);                                                           //display it
    while (BusyUART1());
    crlf();
}


unsigned int CRC(_Bool test, unsigned char size)                                
{
    unsigned int crcREG=0xFFFF;
    unsigned int i=0;
    unsigned int x=0;
        
    for(i=0;i<size-2;i++)                                                       //CRC calculation
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
    
    Nop();
    Nop();
    return crcREG;
}

unsigned int CRC_SN(void)                                                       
{
    unsigned int crcREG=0xFFFF;
    unsigned int i=0;
    unsigned int x=0;
        
    for(i=0;i<4;i++)                                                            //CRC calculation
    {
        crcREG^=_SNbuf[i];                                               

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

void crlf(void)                                                                 //transmit <CR><LF>
{
    putcUART1(cr);
    while (BusyUART1());
    putcUART1(lf);
    while (BusyUART1());
}

unsigned int debounce(void) {
    delay(29492);                                                               
    if (!_CLK_INT) //RTC INTERRUPT valid
        return 1;
    return 0; //RTC INTERRUPT not valid
}


void disableAlarm(unsigned char alarm)                                         
{
    unsigned char temp;

    if(alarm==1)                                                                //RTC Alarm 1 (Reading)
    {
        temp=readClock(RTCControlAddress);                                      //get the value in the RTC Control Register
        temp&=0xFE;                                                             //AND it with 11111110 to set A1IE to 0
        setClock(RTCControlAddress,temp);                                       //write updated Control register to RTC
		
        temp=readClock(RTCStatusAddress);                                       //get the value in the RTC Status Register
        temp&=0xFE;                                                             //AND it with 11111110 to clear A1F
        setClock(RTCStatusAddress,temp);                                        //write updated Status register to RTC	
        return;	
    }

    if(alarm==2)                                                                //RTC Alarm 2 (Control Port)
    {
        temp=readClock(RTCControlAddress);                                      //get the value in the RTC Control Register
        temp&=0xFD;                                                             //AND it with 11111101 to set A2IE to 0
        setClock(RTCControlAddress,temp);                                       //write updated Control register to RTC
		
        temp=readClock(RTCStatusAddress);                                       //get the value in the RTC Status Register
        temp&=0xFD;                                                             //AND it with 11111101 to clear A2F
        setClock(RTCStatusAddress,temp);                                        //write updated Status register to RTC		
    }

    setClock(0x0F,0);                                                           //Clear the RTC Alarm flags
}


void disableBT(void)                                                            
{
    PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress);               
    crlf();

    if(BT_CONNECT && !PORT_CONTROL.flags.BluetoothTimerEN)                      
    {
        putsUART1(BTDisablenotAllowed);
    }
    else
    {
        DISPLAY_CONTROL.flags.BT=0;                                             //clear the BT flag
        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);    //store flags in FRAM   
        S_1.status1flags._BT=0;                                                 //Clear the MODBUS status flag    
        write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
        _BT_RESET=0;                                                            //make sure RESET is low    
        BT_BAUD=0;                                                              //make sure BT_BAUD is low          
        BT_ENABLE=0;                                                            //Powerdown Bluetooth
    }
}

void disableChannel(int channel) {
    MUX_ENABLE1_16.MUXen1_16=read_Int_FRAM(MUX_ENABLE1_16flagsaddress);            
    MUX_ENABLE17_32.MUXen17_32=read_Int_FRAM(MUX_ENABLE17_32flagsaddress);          

    switch (channel) {
        case 1:
            MUX_ENABLE1_16.e1flags.CH1 = 0;
            break;

        case 2:
            MUX_ENABLE1_16.e1flags.CH2 = 0;
            break;

        case 3:
            MUX_ENABLE1_16.e1flags.CH3 = 0;
            break;

        case 4:
            MUX_ENABLE1_16.e1flags.CH4 = 0;
            break;

        case 5:
            MUX_ENABLE1_16.e1flags.CH5 = 0;
            break;

        case 6:
            MUX_ENABLE1_16.e1flags.CH6 = 0;
            break;

        case 7:
            MUX_ENABLE1_16.e1flags.CH7 = 0;
            break;

        case 8:
            MUX_ENABLE1_16.e1flags.CH8 = 0;
            break;

        case 9:
            MUX_ENABLE1_16.e1flags.CH9 = 0;
            break;

        case 10:
            MUX_ENABLE1_16.e1flags.CH10 = 0;
            break;

        case 11:
            MUX_ENABLE1_16.e1flags.CH11 = 0;
            break;

        case 12:
            MUX_ENABLE1_16.e1flags.CH12 = 0;
            break;

        case 13:
            MUX_ENABLE1_16.e1flags.CH13 = 0;
            break;

        case 14:
            MUX_ENABLE1_16.e1flags.CH14 = 0;
            break;

        case 15:
            MUX_ENABLE1_16.e1flags.CH15 = 0;
            break;

        case 16:
            MUX_ENABLE1_16.e1flags.CH16 = 0;
            break;

        case 17:
            MUX_ENABLE17_32.e2flags.CH17 = 0;
            break;

        case 18:
            MUX_ENABLE17_32.e2flags.CH18 = 0;
            break;

        case 19:
            MUX_ENABLE17_32.e2flags.CH19 = 0;
            break;

        case 20:
            MUX_ENABLE17_32.e2flags.CH20 = 0;
            break;

        case 21:
            MUX_ENABLE17_32.e2flags.CH21 = 0;
            break;

        case 22:
            MUX_ENABLE17_32.e2flags.CH22 = 0;
            break;

        case 23:
            MUX_ENABLE17_32.e2flags.CH23 = 0;
            break;

        case 24:
            MUX_ENABLE17_32.e2flags.CH24 = 0;
            break;

        case 25:
            MUX_ENABLE17_32.e2flags.CH25 = 0;
            break;

        case 26:
            MUX_ENABLE17_32.e2flags.CH26 = 0;
            break;

        case 27:
            MUX_ENABLE17_32.e2flags.CH27 = 0;
            break;

        case 28:
            MUX_ENABLE17_32.e2flags.CH28 = 0;
            break;

        case 29:
            MUX_ENABLE17_32.e2flags.CH29 = 0;
            break;

        case 30:
            MUX_ENABLE17_32.e2flags.CH30 = 0;
            break;

        case 31:
            MUX_ENABLE17_32.e2flags.CH31 = 0;
            break;

        case 32:
            MUX_ENABLE17_32.e2flags.CH32 = 0;
            break;
    }

    write_Int_FRAM(MUX_ENABLE1_16flagsaddress,MUX_ENABLE1_16.MUXen1_16);        //store flag in FRAM  
    write_Int_FRAM(MUX_ENABLE17_32flagsaddress,MUX_ENABLE17_32.MUXen17_32);     //store flag in FRAM  
}


void disableTHChannel(int channel)                                              
{
    THMUX_ENABLE1_16.THMUXen1_16=read_Int_FRAM(THMUX_ENABLE1_16flagsaddress);            
    THMUX_ENABLE17_32.THMUXen17_32=read_Int_FRAM(THMUX_ENABLE17_32flagsaddress);          

    switch (channel) {
        case 1:
            THMUX_ENABLE1_16.t1flags.CH1 = 0;
            break;

        case 2:
            THMUX_ENABLE1_16.t1flags.CH2 = 0;
            break;

        case 3:
            THMUX_ENABLE1_16.t1flags.CH3 = 0;
            break;

        case 4:
            THMUX_ENABLE1_16.t1flags.CH4 = 0;
            break;

        case 5:
            THMUX_ENABLE1_16.t1flags.CH5 = 0;
            break;

        case 6:
            THMUX_ENABLE1_16.t1flags.CH6 = 0;
            break;

        case 7:
            THMUX_ENABLE1_16.t1flags.CH7 = 0;
            break;

        case 8:
            THMUX_ENABLE1_16.t1flags.CH8 = 0;
            break;

        case 9:
            THMUX_ENABLE1_16.t1flags.CH9 = 0;
            break;

        case 10:
            THMUX_ENABLE1_16.t1flags.CH10 = 0;
            break;

        case 11:
            THMUX_ENABLE1_16.t1flags.CH11 = 0;
            break;

        case 12:
            THMUX_ENABLE1_16.t1flags.CH12 = 0;
            break;

        case 13:
            THMUX_ENABLE1_16.t1flags.CH13 = 0;
            break;

        case 14:
            THMUX_ENABLE1_16.t1flags.CH14 = 0;
            break;

        case 15:
            THMUX_ENABLE1_16.t1flags.CH15 = 0;
            break;

        case 16:
            THMUX_ENABLE1_16.t1flags.CH16 = 0;
            break;

        case 17:
            THMUX_ENABLE17_32.t2flags.CH17 = 0;
            break;

        case 18:
            THMUX_ENABLE17_32.t2flags.CH18 = 0;
            break;

        case 19:
            THMUX_ENABLE17_32.t2flags.CH19 = 0;
            break;

        case 20:
            THMUX_ENABLE17_32.t2flags.CH20 = 0;
            break;

        case 21:
            THMUX_ENABLE17_32.t2flags.CH21 = 0;
            break;

        case 22:
            THMUX_ENABLE17_32.t2flags.CH22 = 0;
            break;

        case 23:
            THMUX_ENABLE17_32.t2flags.CH23 = 0;
            break;

        case 24:
            THMUX_ENABLE17_32.t2flags.CH24 = 0;
            break;

        case 25:
            THMUX_ENABLE17_32.t2flags.CH25 = 0;
            break;

        case 26:
            THMUX_ENABLE17_32.t2flags.CH26 = 0;
            break;

        case 27:
            THMUX_ENABLE17_32.t2flags.CH27 = 0;
            break;

        case 28:
            THMUX_ENABLE17_32.t2flags.CH28 = 0;
            break;

        case 29:
            THMUX_ENABLE17_32.t2flags.CH29 = 0;
            break;

        case 30:
            THMUX_ENABLE17_32.t2flags.CH30 = 0;
            break;

        case 31:
            THMUX_ENABLE17_32.t2flags.CH31 = 0;
            break;

        case 32:
            THMUX_ENABLE17_32.t2flags.CH32 = 0;
            break;
    }

    write_Int_FRAM(THMUX_ENABLE1_16flagsaddress,THMUX_ENABLE1_16.THMUXen1_16);  //store flag in FRAM  
    write_Int_FRAM(THMUX_ENABLE17_32flagsaddress,THMUX_ENABLE17_32.THMUXen17_32); //store flag in FRAM  
}


void disableINT1(void) 
{
    IFS1bits.INT1IF = 0;                                                        //clear the interrupt flag
    IEC1bits.INT1IE = 0;                                                        //enable the interrupt
}


void disableOP(void)
{
    CONTROL = 0;
    _READ=1;                                                                    //OFF the LED   
    S_1.status1flags._OP=0;                                                     //clear the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
    if (!PORT_CONTROL.flags.CPTime)                                             //turn off control port
        PORT_CONTROL.flags.ControlPortON = 0;                                   //clear flag if not in scheduled ON time
    PORT_CONTROL.flags.O0issued = 1;                                            //set O0issued flag
    write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);              //store flag in FRAM  
    if(!LC2CONTROL2.flags2.Modbus)                                              //Only if command line interface       
        controlPortStatus(1);                                                   //display control port status     
}


void DISLOGINT(void)                                                            
{
    LC2CONTROL.flags.LogInterval = 0;                                           //clear the Log interval flag
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //store flag in FRAM  
    S_1.status1flags._Logint=0;                                                 //Clear the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
}


void disableTimers(void)
{
    //Disable Interrupts:
    IEC1bits.T5IE=0;                                                            //disable Timer 5 interrupt
    //Disable Timers:
    PMD1bits.T1MD=1;                                                            //Timer 1
    PMD1bits.T4MD=1;                                                            //Timer 4
    PMD3bits.T7MD=1;                                                            //Timer 7
}

void disableVWchannel(void)                                                     
{
    A=0;                                                                        //Turn off PLL VCO control signals
    B=0;
    C=0;                                                                
    D=0;                                                                    
    CloseOC1();                                                                 //Close the Output Compare module
    CloseTimer23();                                                             //Close the timer module
    PMD1bits.T3MD=1;                                                            //Disable Timer3 module
    PMD1bits.T2MD=1;                                                            //Disable Timer2 module
    PMD2bits.OC1MD=1;                                                           //Disable Output Compare1 module
    _AMP_SHDN=0;                                                                //Disable the AGC amp    
}

void displayBCD(void)                                                           //convert BCDtens and BCDones to ascii
{                                                                               //and transmit out UART1
    if (LC2CONTROL2.flags2.SetStopTime | (LC2CONTROL2.flags2.GageDisplay && BCDtens >= 1)) {
        putcUART1(BCDtens + 0x30);                                              //format leading zero in stop time
        while (BusyUART1());
    }
    putcUART1(BCDones + 0x30);
    while (BusyUART1());
}

void displayClock(unsigned char data)
{
    BCDone=0;                                                                   //initialize
    BCDten=0;
    BCDtwenty=0;
    BCDthirty=0;
    BCDforty=0;
    BCDfifty=0;
    BCDsixty=0;                                                                 
    BCDseventy=0;                                                               
    BCDeighty=0;                                                                
    BCDninety=0;                                                                

    if(data>=0x90) BCDninety=1;                                                 //extract BCDninety                         
    if(data>=0x80) BCDeighty=1;                                                 //extract BCDeighty                         
    if(data>=0x70) BCDseventy=1;                                                //extract BCDseventy                        
    if(data>=0x60) BCDsixty=1;                                                  //extract BCDsixty                          
    if(data>=0x50) BCDfifty=1;                                                  //extract BCDfifty
    if(data>=0x40) BCDforty=1;                                                  //extract BCDforty
    if(data>=0x30) BCDthirty=1;                                                 //extract BCDthirty
    if(data>=0x20) BCDtwenty=1;                                                 //extract BCDtwenty
    if(data>=0x10) BCDten=1;                                                    //extract BCDten
    BCDten=BCDten+BCDtwenty+BCDthirty+BCDforty+BCDfifty+BCDsixty+BCDseventy+BCDeighty+BCDninety;	//sum them in BCDten    

    BCDten+=0x30;                                                               //convert BCDten to ascii
    putcUART1(BCDten);                                                          //display tens
    while(BusyUART1());

    BCDone=data&0x0F;                                                           //mask off upper nybble of RTCdata
    BCDone+=0x30;                                                               //convert ones to ascii
    putcUART1(BCDone);                                                          //display ones
    while(BusyUART1());
}

void displayFRAMError(int errorAddress) 
{
    char ERRORBUF[4];

    crlf();
    putsUART1(Errors);                                                          //"Errors detected at 0x"
    while (BusyUART1());
    sprintf(ERRORBUF, "%d", errorAddress);                                      //format the address of the FRAM error
    putsUART1(ERRORBUF);                                                        //display it
    while (BusyUART1());
    crlf();
}

void displayGageInfo(int channel)                                               //display the gage information
{
    unsigned long address;                                                      
    unsigned long ZeroReadingaddress;                                           
    unsigned long GageFactoraddress;                                            
    unsigned long GageOffsetaddress;                                            
    unsigned long PolyCoAaddress;                                               
    unsigned long PolyCoBaddress;                                               
    unsigned long PolyCoCaddress;                                               
	unsigned long Thermaddress;                                                 
	unsigned int TEMPTHERM;                                                     
	char BUF[10];                                                                  
    float TEMPVAL;

    if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32)     
    {
        crlf();                                                                 
        putsUART1(GT);                                                          //GT:
        while (BusyUART1());                                                    //Display gage info from FRAM here
    }

	//calculate indexed address for gage type                                   
    if(MUX4_ENABLE.mflags.mux16_4==Single)                                      //if Single Channel                                      
    {
        address=CH1GTaddress;
        Thermaddress=CH1THaddress;                                              
    }
    
    address=CH1GTaddress+(2*(channel-1));                                   
    Thermaddress=CH1THaddress+(2*(channel-1));                              
    
    if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32)     
    {
        LC2CONTROL2.flags2.GageDisplay = 1;                                     //set the gage display flag
        toBCD(read_Int_FRAM(address));                                           
        displayBCD();
        LC2CONTROL2.flags2.GageDisplay = 0;                                     //clear the gage display flag
        LC2CONTROL.flags.Conversion = 0;                                        //clear the conversion flag
    }

    switch (channel)                                                            //get the gage information
    {
        case 1:
			if(MUX4_ENABLE.mflags.mux16_4==Single | MUX4_ENABLE.mflags.mux16_4==VW4)//Single channel or 4 channel MUX?    
			{            
                if (MUX_CONVERSION1_16.c1flags.CH1)                             //Polynomial Conversion?
                {
					PolyCoAaddress=CH1PolyCoAaddress;                           //set the PolyCoAaddress
					PolyCoBaddress=CH1PolyCoBaddress;                           //set the PolyCoBaddress
					PolyCoCaddress=CH1PolyCoCaddress;                           //set the PolyCoCaddress
					LC2CONTROL.flags.Conversion=1;                              //set the conversion flag
                } else 
                {
					ZeroReadingaddress=CH1ZRaddress;                            //set the ZeroReadingaddress
					GageFactoraddress=CH1GFaddress;                             //set the GageFactoraddress
					GageOffsetaddress=CH1GOaddress;                             //set the GageOffsetaddress
                }
            }
            else
			{
                if(MUX4_ENABLE.mflags.mux16_4==VW8 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)   
                {
                    if(MUX_CONVERSION1_16.c1flags.CH1)                          //Ch 1 of 16 Polynomial Conversion?
                    {
                        PolyCoAaddress=CH1PolyCoAaddress;                       //set the PolyCoAaddress
                        PolyCoBaddress=CH1PolyCoBaddress;                       //set the PolyCoBaddress
                        PolyCoCaddress=CH1PolyCoCaddress;                       //set the PolyCoCaddress
                        LC2CONTROL.flags.Conversion=1;                          //set the conversion flag
                    }
                    else
                    {
                        ZeroReadingaddress=CH1ZRaddress;                        //set the ZeroReadingaddress
                        GageFactoraddress=CH1GFaddress;                         //set the GageFactoraddress
                        GageOffsetaddress=CH1GOaddress;                         //set the GageOffsetaddress
                    }
                }
			}
                
            break;


        case 2:
			if(!MUX4_ENABLE.mflags.mux16_4)                                     //4 channel MUX?
			{
				if(MUX_CONVERSION1_16.c1flags.CH2)                              //Ch 2 of 4 Polynomial Conversion?
				{
					PolyCoAaddress=CH2PolyCoAaddress;                           //set the PolyCoAaddress
					PolyCoBaddress=CH2PolyCoBaddress;                           //set the PolyCoBaddress
					PolyCoCaddress=CH2PolyCoCaddress;                           //set the PolyCoCaddress
					LC2CONTROL.flags.Conversion=1;                              //set the conversion flag
					
				}
				else
				{
					ZeroReadingaddress=CH2ZRaddress;                            //set the ZeroReadingaddress
					GageFactoraddress=CH2GFaddress;                             //set the GageFactoraddress
					GageOffsetaddress=CH2GOaddress;                             //set the GageOffsetaddress
				}
			}
			else
			{
				if(MUX_CONVERSION1_16.c1flags.CH2)                              //Ch 2 of 16 Polynomial Conversion?
				{
					PolyCoAaddress=CH2PolyCoAaddress;                           //set the PolyCoAaddress
					PolyCoBaddress=CH2PolyCoBaddress;                           //set the PolyCoBaddress
					PolyCoCaddress=CH2PolyCoCaddress;                           //set the PolyCoCaddress
					LC2CONTROL.flags.Conversion=1;                              //set the conversion flag
				}
				else
				{
					ZeroReadingaddress=CH2ZRaddress;                            //set the ZeroReadingaddress
					GageFactoraddress=CH2GFaddress;                             //set the GageFactoraddress
					GageOffsetaddress=CH2GOaddress;                             //set the GageOffsetaddress
				}
			}

			break;

        case 3:
 			if(!MUX4_ENABLE.mflags.mux16_4)                                     //4 channel MUX?
			{
				if(MUX_CONVERSION1_16.c1flags.CH3)                              //Ch 3 of 4 Polynomial Conversion?
				{
					PolyCoAaddress=CH3PolyCoAaddress;                           //set the PolyCoAaddress
					PolyCoBaddress=CH3PolyCoBaddress;                           //set the PolyCoBaddress
					PolyCoCaddress=CH3PolyCoCaddress;                           //set the PolyCoCaddress
					LC2CONTROL.flags.Conversion=1;                              //set the conversion flag
					
				}
				else
				{
					ZeroReadingaddress=CH3ZRaddress;                            //set the ZeroReadingaddress
					GageFactoraddress=CH3GFaddress;                             //set the GageFactoraddress
					GageOffsetaddress=CH3GOaddress;                             //set the GageOffsetaddress
				}
			}
			else
			{
				if(MUX_CONVERSION1_16.c1flags.CH3)                              //Ch 3 of 16 Polynomial Conversion?
				{
					PolyCoAaddress=CH3PolyCoAaddress;                           //set the PolyCoAaddress
					PolyCoBaddress=CH3PolyCoBaddress;                           //set the PolyCoBaddress
					PolyCoCaddress=CH3PolyCoCaddress;                           //set the PolyCoCaddress
					LC2CONTROL.flags.Conversion=1;                              //set the conversion flag
				}
				else
				{
					ZeroReadingaddress=CH3ZRaddress;                            //set the ZeroReadingaddress
					GageFactoraddress=CH3GFaddress;                             //set the GageFactoraddress
					GageOffsetaddress=CH3GOaddress;                             //set the GageOffsetaddress
				}
			}


			break;

        case 4:
			if(!MUX4_ENABLE.mflags.mux16_4)                                     //4 channel MUX?
			{
				if(MUX_CONVERSION1_16.c1flags.CH4)                              //Ch 4 of 4 Polynomial Conversion?
				{
					PolyCoAaddress=CH4PolyCoAaddress;                           //set the PolyCoAaddress
					PolyCoBaddress=CH4PolyCoBaddress;                           //set the PolyCoBaddress
					PolyCoCaddress=CH4PolyCoCaddress;                           //set the PolyCoCaddress
					LC2CONTROL.flags.Conversion=1;                              //set the conversion flag
					
				}
				else
				{
					ZeroReadingaddress=CH4ZRaddress;                            //set the ZeroReadingaddress
					GageFactoraddress=CH4GFaddress;                             //set the GageFactoraddress
					GageOffsetaddress=CH4GOaddress;                             //set the GageOffsetaddress
				}
			}
			else
			{
				if(MUX_CONVERSION1_16.c1flags.CH4)                              //Ch 4 of 16 Polynomial Conversion?
				{
					PolyCoAaddress=CH4PolyCoAaddress;                           //set the PolyCoAaddress
					PolyCoBaddress=CH4PolyCoBaddress;                           //set the PolyCoBaddress
					PolyCoCaddress=CH4PolyCoCaddress;                           //set the PolyCoCaddress
					LC2CONTROL.flags.Conversion=1;                              //set the conversion flag
				}
				else
				{
					ZeroReadingaddress=CH4ZRaddress;                            //set the ZeroReadingaddress
					GageFactoraddress=CH4GFaddress;                             //set the GageFactoraddress
					GageOffsetaddress=CH4GOaddress;                             //set the GageOffsetaddress
				}
			}

			break;

        case 5:
			if(MUX_CONVERSION1_16.c1flags.CH5)                                  //Ch 5 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH5PolyCoAaddress;                               //set the PolyCoAaddress
				PolyCoBaddress=CH5PolyCoBaddress;                               //set the PolyCoBaddress
				PolyCoCaddress=CH5PolyCoCaddress;                               //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH5ZRaddress;                                //set the ZeroReadingaddress
				GageFactoraddress=CH5GFaddress;                                 //set the GageFactoraddress
				GageOffsetaddress=CH5GOaddress;                                 //set the GageOffsetaddress
			}			
			
			break;


        case 6:
			if(MUX_CONVERSION1_16.c1flags.CH6)                                  //Ch 6 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH6PolyCoAaddress;                               //set the PolyCoAaddress
				PolyCoBaddress=CH6PolyCoBaddress;                               //set the PolyCoBaddress
				PolyCoCaddress=CH6PolyCoCaddress;                               //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH6ZRaddress;                                //set the ZeroReadingaddress
				GageFactoraddress=CH6GFaddress;                                 //set the GageFactoraddress
				GageOffsetaddress=CH6GOaddress;                                 //set the GageOffsetaddress
			}			
			
			break;


        case 7:
                if(MUX_CONVERSION1_16.c1flags.CH7)                              //Ch 7 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH7PolyCoAaddress;                               //set the PolyCoAaddress
				PolyCoBaddress=CH7PolyCoBaddress;                               //set the PolyCoBaddress
				PolyCoCaddress=CH7PolyCoCaddress;                               //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH7ZRaddress;                                //set the ZeroReadingaddress
				GageFactoraddress=CH7GFaddress;                                 //set the GageFactoraddress
				GageOffsetaddress=CH7GOaddress;                                 //set the GageOffsetaddress
			}			
			
			break;


        case 8:
			if(MUX_CONVERSION1_16.c1flags.CH8)                                  //Ch 8 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH8PolyCoAaddress;                               //set the PolyCoAaddress
				PolyCoBaddress=CH8PolyCoBaddress;                               //set the PolyCoBaddress
				PolyCoCaddress=CH8PolyCoCaddress;                               //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH8ZRaddress;                                //set the ZeroReadingaddress
				GageFactoraddress=CH8GFaddress;                                 //set the GageFactoraddress
				GageOffsetaddress=CH8GOaddress;                                 //set the GageOffsetaddress
			}			
			
			break;


        case 9:

			if(MUX_CONVERSION1_16.c1flags.CH9)                                  //Ch 9 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH9PolyCoAaddress;                               //set the PolyCoAaddress
				PolyCoBaddress=CH9PolyCoBaddress;                               //set the PolyCoBaddress
				PolyCoCaddress=CH9PolyCoCaddress;                               //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH9ZRaddress;                                //set the ZeroReadingaddress
				GageFactoraddress=CH9GFaddress;                                 //set the GageFactoraddress
				GageOffsetaddress=CH9GOaddress;                                 //set the GageOffsetaddress
			}			
			
			break;
            
            
        case 10:

			if(MUX_CONVERSION1_16.c1flags.CH10)                                 //Ch 10 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH10PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH10PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH10PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH10ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH10GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH10GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 11:

			if(MUX_CONVERSION1_16.c1flags.CH11)                                 //Ch 11 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH11PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH11PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH11PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH11ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH11GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH11GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;


        case 12:

			if(MUX_CONVERSION1_16.c1flags.CH12)                                 //Ch 12 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH12PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH12PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH12PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH12ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH12GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH12GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 13:

			if(MUX_CONVERSION1_16.c1flags.CH13)                                 //Ch 13 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH13PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH13PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH13PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH13ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH13GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH13GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;



        case 14:

			if(MUX_CONVERSION1_16.c1flags.CH14)                                 //Ch 14 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH14PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH14PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH14PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH14ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH14GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH14GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 15:

			if(MUX_CONVERSION1_16.c1flags.CH15)                                 //Ch 15 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH15PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH15PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH15PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH15ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH15GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH15GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;


        case 16:

			if(MUX_CONVERSION1_16.c1flags.CH16)                                 //Ch 16 of 16 Polynomial Conversion?
			{
				PolyCoAaddress=CH16PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH16PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH16PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH16ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH16GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH16GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        
        case 17:

			if(MUX_CONVERSION17_32.c2flags.CH17)                                //Ch 17 Polynomial Conversion?
			{
				PolyCoAaddress=CH17PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH17PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH17PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH17ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH17GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH17GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;


        case 18:
			if(MUX_CONVERSION17_32.c2flags.CH18)                                //Ch 18 Polynomial Conversion?
			{
				PolyCoAaddress=CH18PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH18PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH18PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH18ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH18GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH18GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;
            
        case 19:
			if(MUX_CONVERSION17_32.c2flags.CH19)                                //Ch 19 Polynomial Conversion?
			{
				PolyCoAaddress=CH19PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH19PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH19PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH19ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH19GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH19GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 20:
			if(MUX_CONVERSION17_32.c2flags.CH20)                                //Ch 20 Polynomial Conversion?
			{
				PolyCoAaddress=CH20PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH20PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH20PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH20ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH20GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH20GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 21:
			if(MUX_CONVERSION17_32.c2flags.CH21)                                //Ch 21 Polynomial Conversion?
			{
				PolyCoAaddress=CH21PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH21PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH21PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH21ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH21GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH21GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;


        case 22:
			if(MUX_CONVERSION17_32.c2flags.CH22)                                //Ch 22 Polynomial Conversion?
			{
				PolyCoAaddress=CH22PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH22PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH22PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH22ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH22GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH22GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 23:
			if(MUX_CONVERSION17_32.c2flags.CH23)                                //Ch 23 Polynomial Conversion?
			{
				PolyCoAaddress=CH23PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH23PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH23PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH23ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH23GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH23GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 24:
			if(MUX_CONVERSION17_32.c2flags.CH24)                                //Ch 24 Polynomial Conversion?
			{
				PolyCoAaddress=CH24PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH24PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH24PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH24ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH24GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH24GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 25:

			if(MUX_CONVERSION17_32.c2flags.CH25)                                //Ch 25 Polynomial Conversion?
			{
				PolyCoAaddress=CH25PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH25PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH25PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH25ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH25GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH25GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 26:

			if(MUX_CONVERSION17_32.c2flags.CH26)                                //Ch 26 Polynomial Conversion?
			{
				PolyCoAaddress=CH26PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH26PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH26PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH26ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH26GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH26GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 27:

			if(MUX_CONVERSION17_32.c2flags.CH27)                                //Ch 27 Polynomial Conversion?
			{
				PolyCoAaddress=CH27PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH27PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH27PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH27ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH27GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH27GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 28:

			if(MUX_CONVERSION17_32.c2flags.CH28)                                //Ch 28 Polynomial Conversion?
			{
				PolyCoAaddress=CH28PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH28PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH28PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH28ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH28GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH28GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 29:

			if(MUX_CONVERSION17_32.c2flags.CH29)                                //Ch 29 Polynomial Conversion?
			{
				PolyCoAaddress=CH29PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH29PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH29PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH29ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH29GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH29GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;


        case 30:

			if(MUX_CONVERSION17_32.c2flags.CH30)                                //Ch 30 Polynomial Conversion?
			{
				PolyCoAaddress=CH30PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH30PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH30PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH30ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH30GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH30GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;

        case 31:

			if(MUX_CONVERSION17_32.c2flags.CH31)                                //Ch 31 Polynomial Conversion?
			{
				PolyCoAaddress=CH31PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH31PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH31PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH31ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH31GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH31GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;
            
        case 32:

			if(MUX_CONVERSION17_32.c2flags.CH32)                                //Ch 32 Polynomial Conversion?
			{
				PolyCoAaddress=CH32PolyCoAaddress;                              //set the PolyCoAaddress
				PolyCoBaddress=CH32PolyCoBaddress;                              //set the PolyCoBaddress
				PolyCoCaddress=CH32PolyCoCaddress;                              //set the PolyCoCaddress
				LC2CONTROL.flags.Conversion=1;                                  //set the conversion flag
			}
			else
			{
				ZeroReadingaddress=CH32ZRaddress;                               //set the ZeroReadingaddress
				GageFactoraddress=CH32GFaddress;                                //set the GageFactoraddress
				GageOffsetaddress=CH32GOaddress;                                //set the GageOffsetaddress
			}			
			
			break;
            
        default:
            break;
    }

    if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32)     
    {
        if (LC2CONTROL.flags.Conversion)                                        //Display polynomial coefficients
        {
            putsUART1(PA);
            while (BusyUART1());
            TEMPVAL=read_float(PolyCoAaddress);                                 //extract coefficient A from FRAM   
            formatandDisplayGageInfo(TEMPVAL);

            putsUART1(PB);
            while (BusyUART1());
            TEMPVAL=read_float(PolyCoAaddress+0x0080);                          //extract coefficient B from FRAM   
            formatandDisplayGageInfo(TEMPVAL);

            putsUART1(PC);
            while (BusyUART1());
            TEMPVAL=read_float(PolyCoAaddress+0x0100);                          //extract coefficient C from FRAM  
            formatandDisplayGageInfo(TEMPVAL);
        } else 
        {
            putsUART1(ZR);
            while (BusyUART1());
            TEMPVAL=read_float(ZeroReadingaddress);                             //extract Zero Reading from FRAM    
            formatandDisplayGageInfo(TEMPVAL);  

            putsUART1(GF);
            while (BusyUART1());
            TEMPVAL=read_float(ZeroReadingaddress+0x0080);                      //extract Gage Factor from FRAM 
            formatandDisplayGageInfo(TEMPVAL);

            putsUART1(GO);
            while (BusyUART1());
            TEMPVAL=read_float(ZeroReadingaddress+0x0100);                      //extract Gage Offset from FRAM 
            formatandDisplayGageInfo(TEMPVAL);
        }
    }
    
    if(MUX4_ENABLE.mflags.mux16_4!=VW8 && MUX4_ENABLE.mflags.mux16_4!=VW32)     //if thermistor is included in reading  
    {
        //DISPLAY THERMISTOR TYPE:                                              
		putsUART1(Tab);
		while(BusyUART1());
		putsUART1(TH);
		while(BusyUART1());
		TEMPTHERM=read_Int_FRAM(Thermaddress);
		sprintf(BUF,"%d",TEMPTHERM);
		putsUART1(BUF);
		while(BusyUART1());    
    }
}

void displayLoggingWillStart(void) {
    putsUART1(Loggingwillstart);                                                //Logging will start at:
    while (BusyUART1());
    RTCdata = readClock(RTCAlarm1HoursAddress);                                 //Display Alarm1 hours 
    displayClock(RTCdata);
    putcUART1(colon); // :
    while (BusyUART1());
    RTCdata = readClock(RTCAlarm1MinutesAddress);                               //Display Alarm1 minutes
    displayClock(RTCdata);
    putcUART1(colon);
    while (BusyUART1());
    RTCdata = readClock(RTCAlarm1SecondsAddress);                               //Display Alarm1 seconds
    displayClock(RTCdata);
}

void displayLoggingWillStop(void) {
    LC2CONTROL2.flags2.SetStopTime = 1;                                         //set the flag for formatting display

    putsUART1(Loggingwillstop);                                                 //Logging will stop at:
    while (BusyUART1());                                                        //get and display stop time from FRAM

    toBCD(read_Int_FRAM(LoggingStopHoursaddress));   
    displayBCD();

    putcUART1(colon);
    while (BusyUART1());

    toBCD(read_Int_FRAM(LoggingStopMinutesaddress)); 
    displayBCD();

    putcUART1(colon);
    while (BusyUART1())

        toBCD(read_Int_FRAM(LoggingStopSecondsaddress)); 
        displayBCD();

    LC2CONTROL2.flags2.SetStopTime = 0;                                         //reset the flag
}

void displayLogInterval(int interval) {
    char BUF[6];
    int value;
    unsigned long length;

    crlf();
    putsUART1(Interval);                                                        //"Interval #"
    while (BusyUART1());

    sprintf(BUF, "%d", interval);                                               //format it
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());

    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());

    interval -= 1;                                                              //change interval from 1-6 to 0-5

    putsUART1(Length);
    while (BusyUART1());
    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //Single VW Channel 
        length=read_longFRAM(SingleLogIntLength1address+(interval*4));          //get Interval value  
    else                                                                        //multichannel  
        length=read_longFRAM(LogIntLength1address+(interval*4));                //get Interval value  
    displayScanInterval(length, 0);

    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());

    putsUART1(Iterations);
    while (BusyUART1());
    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //Single VW Channel 
        value=read_Int_FRAM(SingleLogIt1address+(interval*2));                  //get Interval iterations 
    else                                                                        //multichannel  
        value=read_Int_FRAM(LogIt1address+(interval*2));                        //get Interval value    
    sprintf(BUF, "%d", value);                                                  //format it
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());

    if (LC2CONTROL.flags.Logging && LC2CONTROL.flags.LogInterval)               //if logging and log intervals enabled
    {
        putcUART1(slash);                                                       //foreslash
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 == Single)                               //Single VW Channel 
            value=read_Int_FRAM(SingleLogItRemain1address+(interval*2));        //get the remaining iterations at this interval   
        else                                                                    //multichannel
            value=read_Int_FRAM(LogItRemain1address+(interval*2));              //get the remaining iterations at this interval   

        if (interval == 0)
            value += 1;

        if (value <= 0)
            value = 0;

        sprintf(BUF, "%d", value);
        putsUART1(BUF);
        while (BusyUART1());
    }

}

void displayLogTable(void) 
{
    int i;                                                                      //index for Log Interval loop

    crlf();

    putsUART1(Loglist);                                                         //"Log Intervals List"
    while (BusyUART1());

    crlf();

    putsUART1(Dashes);                                                          //"----------------------"
    while (BusyUART1());

    crlf();

    for (i = 1; i < 7; i++) {
        displayLogInterval(i);                                                  //display the Log Interval information
        crlf();
    }

}

void displayMemoryStatus(void) {
    char BUF[6];                                                                //temporary storage for display
    unsigned int pointer = 0;

    crlf();
    putsUART1(MS);                                                              //MS:
    while (BusyUART1());                                                        //Display Memory Status
    pointer=read_Int_FRAM(MemoryStatusaddress);                                 //get Memory Status (MS) pointer  
    sprintf(BUF, "%d", pointer);                                                //format it                                  
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());

    putsUART1(OP);                                                              //OP:
    while (BusyUART1());                                                        //Display Output Position
    pointer=read_Int_FRAM(OutputPositionaddress);                               //get Output Position (OP) pointer  
    sprintf(BUF, "%d", pointer);                                                //format it
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());

    putsUART1(UP);                                                              //UP:
    while (BusyUART1());                                                        //Display User Position
    pointer=read_Int_FRAM(UserPositionaddress);                                 //get User Position (UP) pointer from FRAM  
    sprintf(BUF, "%d", pointer);
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());

}


void displayMUX(int displayChannel) 
{
    MUX_ENABLE1_16.MUXen1_16=read_Int_FRAM(MUX_ENABLE1_16flagsaddress);       
    MUX_ENABLE17_32.MUXen17_32=read_Int_FRAM(MUX_ENABLE17_32flagsaddress);          
    THMUX_ENABLE1_16.THMUXen1_16=read_Int_FRAM(THMUX_ENABLE1_16flagsaddress);       
    THMUX_ENABLE17_32.THMUXen17_32=read_Int_FRAM(THMUX_ENABLE17_32flagsaddress);          
    //DISPLAY MUX SETUP TABLE:
    crlf();

    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //Single Channel    
    {
        putsUART1(MUX1setupmenu);                                               //"LC-4 Single Channel VW Setup:"
        while (BusyUART1());
        crlf();
        displayGageInfo(1);                                                     //Display gage information
        crlf();
        crlf();
        return;                                                                 //just return
    }


    if (MUX4_ENABLE.mflags.mux16_4 == VW4)
        putsUART1(MUX4setupmenu);                                               //"LC-4MUX 4-Channel VW Multiplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == VW8)
        putsUART1(MUX8setupmenu);                                               //"LC-4MUX 8-Channel VW Multiplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == VW16)
        putsUART1(MUX16setupmenu);                                              //"LC-4MUX 16-Channel VW Mulitplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == VW32)
        putsUART1(MUX32setupmenu);                                              //"LC-4MUX 32-Channel VW Multiplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == TH8)
        putsUART1(MUX8Tsetupmenu);                                              //LC-4MUX 8-Channel Thermistor Multiplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == TH32)
        putsUART1(MUX32Tsetupmenu);                                             //LC-4MUX 32-Channel Thermistor Multiplexer Setup:"
    while (BusyUART1());
    crlf();
    crlf();


    if (displayChannel == 1 | displayChannel == 0) 
    {
        putsUART1(CH1);                                                         //CH1 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW8 |      
                MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH1)                                     //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH1)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }        
        while (BusyUART1());

        displayGageInfo(1);                                                     //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 2 | displayChannel == 0) 
    {
        putsUART1(CH2);                                                         //CH2 setup
        while (BusyUART1());

        if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW8 |      
                MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH2)                                     //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH2)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(2);                                                     //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 3 | displayChannel == 0) 
    {
        putsUART1(CH3);                                                         //CH3 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW8 |      
                MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH3)                                     //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH3)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(3);                                                     //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 4 | displayChannel == 0) 
    {
        putsUART1(CH4);                                                         //CH4 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW8 |      
                MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH4)                                     //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH4)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(4);                                                     //Display gage information
        crlf();
        crlf();
    }


    if (MUX4_ENABLE.mflags.mux16_4 == VW4)
        return;

    if (displayChannel == 5 | displayChannel == 0) 
    {
        putsUART1(CH5);                                                         //CH5 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW8 |      
                MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH5)                                     //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH5)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(5);                                                     //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 6 | displayChannel == 0) 
    {
        putsUART1(CH6);                                                         //CH6 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW8 |      
                MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH6)                                     //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH6)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(6);                                                     //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 7 | displayChannel == 0) 
    {
        putsUART1(CH7);                                                         //CH7 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW8 |      
                MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH7)                                     //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH7)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(7);                                                     //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 8 | displayChannel == 0) 
    {
        putsUART1(CH8);                                                         //CH8 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW8 |      
                MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH8)                                     //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH8)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(8);                                                     //Display gage information
        crlf();
        crlf();
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == TH8)
        return;


    if (displayChannel == 9 | displayChannel == 0) 
    {
        putsUART1(CH9);                                                         //CH9 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH9)                                     //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH9)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(9);                                                     //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 10 | displayChannel == 0) 
    {
        putsUART1(CH10);                                                        //CH10 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH10)                                    //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH10)                                  //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(10);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 11 | displayChannel == 0) 
    {
        putsUART1(CH11);                                                        //CH11 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH11)                                    //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH11)                                  //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(11);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 12 | displayChannel == 0) 
    {
        putsUART1(CH12);                                                        //CH12 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH12)                                    //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH12)                                  //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(12);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 13 | displayChannel == 0) 
    {
        putsUART1(CH13);                                                        //CH13 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH13)                                    //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH13)                                  //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(13);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 14 | displayChannel == 0) 
    {
        putsUART1(CH14);                                                        //CH14 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH14)                                    //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH14)                                  //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(14);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 15 | displayChannel == 0) 
    {
        putsUART1(CH15);                                                        //CH15 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH15)                                    //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH15)                                  //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(15);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 16 | displayChannel == 0) 
    {
        putsUART1(CH16);                                                        //CH16 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE1_16.e1flags.CH16)                                    //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE1_16.t1flags.CH16)                                  //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(16);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16)
        return;

    if (displayChannel == 17 | displayChannel == 0) 
    {
        putsUART1(CH17);                                                        //CH17 setup
        while (BusyUART1());
    
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH17)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH17)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(17);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 18 | displayChannel == 0) 
    {
        putsUART1(CH18);                                                        //CH18 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH18)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH18)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(18);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 19 | displayChannel == 0) 
    {
        putsUART1(CH19);                                                        //CH19 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH19)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH19)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(19);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 20 | displayChannel == 0) 
    {
        putsUART1(CH20);                                                        //CH20 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH20)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH20)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(20);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 21 | displayChannel == 0) 
    {
        putsUART1(CH21);                                                        //CH21 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH21)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH21)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(21);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 22 | displayChannel == 0) 
    {
        putsUART1(CH22);                                                        //CH22 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH22)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH22)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(22);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 23 | displayChannel == 0) 
    {
        putsUART1(CH23);                                                        //CH23 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH23)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH23)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(23);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 24 | displayChannel == 0) 
    {
        putsUART1(CH24);                                                        //CH24 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH24)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH24)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(24);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 25 | displayChannel == 0) 
    {
        putsUART1(CH25);                                                        //CH25 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH25)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH25)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(25);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 26 | displayChannel == 0) 
    {
        putsUART1(CH26);                                                        //CH26 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH26)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH26)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(26);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 27 | displayChannel == 0) 
    {
        putsUART1(CH27);                                                        //CH27 setup
        while (BusyUART1());
    
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH27)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH27)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(27);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 28 | displayChannel == 0) 
    {
        putsUART1(CH28);                                                        //CH28 setup
        while (BusyUART1());
        
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH28)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH28)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(28);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 29 | displayChannel == 0) 
    {
        putsUART1(CH29);                                                        //CH29 setup
        while (BusyUART1());
    
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH29)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH29)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(29);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 30 | displayChannel == 0) 
    {
        putsUART1(CH30);                                                        //CH30 setup
        while (BusyUART1());
    
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH30)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH30)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(30);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 31 | displayChannel == 0) 
    {
        putsUART1(CH31);                                                        //CH31 setup
        while (BusyUART1());
    
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH31)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH31)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(31);                                                    //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 32 | displayChannel == 0) 
    {
        putsUART1(CH32);                                                        //CH32 setup
        while (BusyUART1());
    
        if(MUX4_ENABLE.mflags.mux16_4==VW32)    
        {
            if (MUX_ENABLE17_32.e2flags.CH32)                                   //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }
        
        if(MUX4_ENABLE.mflags.mux16_4==TH32)    
        {
            if (THMUX_ENABLE17_32.t2flags.CH32)                                 //Display ENABLED or DISABLED
                putsUART1(Enabled);
            else
                putsUART1(Disabled);
        }                
        while (BusyUART1());

        displayGageInfo(32);                                                    //Display gage information
        crlf();
        crlf();
    }
}

void displayReading(int ch, unsigned long outputPosition)                       //display readings stored in external FRAM at FRAMaddress
{
    typedef struct                                                              
    {
	unsigned char lsb;                                                          //array LSB for binary Tx
    unsigned char msb;                                                          //array MSB for binary Tx
    }ArrayBytes;
    typedef union{unsigned int array;
    ArrayBytes arrays;
    }uarrays;
    uarrays TXARRAY;                                                            //TXARRAY.ArrayBytes.msb, TXARRAY.array.lsb
    
    char BUF[10];                                                               //temporary storage for display data                          
    unsigned char NOB;                                                          //Number Of Bytes 
    volatile unsigned int arrayIDX;                                                      
    unsigned char month;
    unsigned char day;
    unsigned int julian;
    unsigned int year;
    int intThermreading=0;                                                      
    int mainBatreading = 0;
    unsigned int displayChannel = 0;
    unsigned long i;                                                            
    unsigned int maxchannelplusone;
    unsigned long FRAMaddress;
    unsigned long tempoutputPosition = 0;                                       
    float mainBat = 0.0;
    float extThermProcessed = 0.0;

    for(arrayIDX=0;arrayIDX<152;arrayIDX++)
    {
        TxBinaryBUF[arrayIDX]=0;                                                //clear the buffer  
    }
    arrayIDX=0;                                                                 //reset the index   

    year = read_Int_FRAM(0);                                                    //Read the year from the external FRAM to initialize I2C    
        
    if ((outputPosition > maxSingleVW && (MUX4_ENABLE.mflags.mux16_4 == Single)) |
            (outputPosition > maxFourVW && (MUX4_ENABLE.mflags.mux16_4 == VW4)) |
            (outputPosition > maxEightVW && (MUX4_ENABLE.mflags.mux16_4 == VW8)) |
            (outputPosition > maxSixteenVW && (MUX4_ENABLE.mflags.mux16_4 == VW16)) |
            (outputPosition > maxThirtytwoVW && (MUX4_ENABLE.mflags.mux16_4 == VW32)) | //memory full:Return memory pointer to 1 for circular memory
            (outputPosition > maxEightTH && (MUX4_ENABLE.mflags.mux16_4 == TH8)) |
            (outputPosition > maxThirtytwoTH && (MUX4_ENABLE.mflags.mux16_4 == TH32))) {
        outputPosition = 1;
        write_Int_FRAM(OutputPositionaddress,outputPosition);                   //store outputPosition pointer in internal FRAM   
    }

    TXARRAY.array=outputPosition;                                               

    //calculate the external FRAM base address:
    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //Single Channel VW
    {
        FRAMaddress = SingleVWPosition;
        NOB=28;                                                                 //28 bytes for binary download  
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //4 Channel VW
    {
        FRAMaddress = VW4Position;
        NOB=52;                                                                 
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //8 Channel VW
    {
        FRAMaddress = VW8Position;
        NOB=52;                                                                 
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                     //16 Channel VW
    {
        FRAMaddress = VW16Position;
        NOB=148;                                                                          
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                     //32 Channel VW
    {
        FRAMaddress = VW32Position;
        NOB=148;                                                                
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                      //8 Channel thermistor
    {
        FRAMaddress = TH8Position;
        NOB=52;                                                                 
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                     //32 Channel thermistor
    {
        FRAMaddress = TH32Position;
        NOB=148;                                                                
    }

    if (!_232) 
        _232SHDN=1;                                                             

    if (LC2CONTROL.flags.ID)                                                    //display ID    
    {

        for (i = IDaddress; i < CH1GTaddress; i += 2)                           //parse the buffer and extract the ID character    
        {
            data=read_Int_FRAM(i);                                              //read the ID starting FRAM location    
            unpack(data);                                                       //unpack into (2) bytes

            if (Hbyte == cr)                                                    
                break;
            putcUART1(Hbyte);

            if (Lbyte == cr)                                                    //exit if LSB = <CR>    
                break;
            putcUART1(Lbyte);
        }
        putcUART1(comma);                                                       // , DELIMITER
    }
    
    //TIME STAMP
    //YEAR
    IEC1bits.INT1IE = 0;                                                        //Disable INT1
    year = read_Int_FRAM(FRAMaddress);                                          //Read the year from the external FRAM
    IEC1bits.INT1IE = 1;                                                        //Enable INT1


    putcUART1(two);                                                             //year
    while(BusyUART1());
    putcUART1(zero);
    while(BusyUART1());

    if (year < 10)
    {
        putcUART1(zero);                                                        //add leading zero if required
        while(BusyUART1());
    }
    sprintf(BUF, "%d", year);                                                   //format it
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(comma);                                                           // , DELIMITER
    while(BusyUART1());

    //DAY OF YEAR
    IEC1bits.INT1IE = 0;                                                        //Disable INT1
    julian = read_Int_FRAM(FRAMaddress + 2);                                    //Read the decimal date from external FRAM
    IEC1bits.INT1IE = 1;                                                        //Enable INT1

    if(LC2CONTROL.flags.DateFormat)                                             //month,day
    {
        month = toMonthDay(julian, year, 1);                                    //get the month from the julian date
        day = toMonthDay(julian, year, 0);                                      //get the day from the julian date
        sprintf(BUF, "%d", month);                                              //format the month data 
        putsUART1(BUF);                                                         //display it
        while (BusyUART1());
        putcUART1(comma);                                                       // , DELIMITER
        while(BusyUART1());
        sprintf(BUF, "%d", day);                                                //format the day data   
        putsUART1(BUF);                                                         //display it
        while (BusyUART1());
        putcUART1(comma);                                                       // , DELIMITER
        while(BusyUART1());
    } 
    else                                                                        //decimal day
    {
        sprintf(BUF, "%d", julian);                                         
        putsUART1(BUF);
        while (BusyUART1());
        putcUART1(comma);                                                       // , DELIMITER
        while(BusyUART1());
    } 

    IEC1bits.INT1IE = 0;                                                        //Disable INT1
    TESTSECONDS=read_longFRAM(FRAMaddress + 4);                                 
    IEC1bits.INT1IE = 1;                                                        //Enable INT1
    seconds2hms(TESTSECONDS);

    //HOUR
    if (hour < 10)                                                              //add leading zero
    {
        putcUART1(zero);
        while(BusyUART1());
    }

    sprintf(BUF, "%d", hour);                                                   //format the hour data  
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    if (LC2CONTROL.flags.TimeFormat)                                            //hh,mm format
    {
        putcUART1(comma); // , DELIMITER
        while(BusyUART1());
    }

    //MINUTE
    if (minute < 10)                                                            //add leading zero
    {
        putcUART1(zero);
        while(BusyUART1());
    }

    sprintf(BUF, "%d", minute);                                                 //format the minute data    
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(comma);                                                           // , DELIMITER
    while(BusyUART1());

    //SECOND
    sprintf(BUF, "%d", second);                                                 //format the second data    
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(comma);                                                           // , DELIMITER
    while(BusyUART1());

    //****************Get and format the readings from FRAM****************************************************
    //MAIN BATTERY
    IEC1bits.INT1IE = 0;                                                        //Disable INT1
    mainBatreading = read_Int_FRAM(FRAMaddress + 8);                            //Read the 12bit main Bat reading from external FRAM
    IEC1bits.INT1IE = 1;                                                        //Enable INT2

    if (mainBatreading >= 0) 
    {
        mainBat = (((Vref * mainBatreading) / 4096) * mul3V);                   //format the 3V battery data    
        if (mainBat < 0.3)
            mainBat = 0.0;
    } 
    else 
    {
        mainBat = (((Vref * mainBatreading) / 4096) * mul12V);                  //format the 12V battery data   
        mainBat *= -1.0;                                                        //convert to positive
        if (mainBat < 0.3)
            mainBat = 0.0;
    }

    //*************************************Display the readings**************************************************************

    //MAIN BATTERY
    sprintf(BUF, "%.2f", mainBat);
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(comma);                                                           // , DELIMITER
    while(BusyUART1());

    //****************Get and format the readings from FRAM****************************************************
    //INTERNAL THERMISTOR
    IEC1bits.INT1IE = 0;                                                        //Disable INT1
    intThermreading = read_Int_FRAM(FRAMaddress + 10);                          //Read the 12bit internal thermistor reading from external FRAM
    IEC1bits.INT1IE = 1;                                                        //Enable INT1

    IEC1bits.INT1IE = 0;                                                        //Disable INT1
    intThermreading=read_Int_FRAM(FRAMaddress+10);                              //Read the 12bit internal thermistor reading from internal FRAM 
    IEC1bits.INT1IE = 1;                                                        //Enable INT1
    processDS3231Temperature(intThermreading);                                  
    DS3231Fraction*=25;                                                         //Format the DS3231 fractional portion
    
    sprintf(BUF, "%d", DS3231Integer);                                          //format the DS3231 Temperature Integer portion
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(decimal);                                                         //decimal point
    while(BusyUART1());
    sprintf(BUF, "%d", DS3231Fraction);                                     
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());    
    putcUART1(comma);                                                           // , DELIMITER
    while (BusyUART1());

    tempoutputPosition = outputPosition;

    //GAGE READING
    if (MUX4_ENABLE.mflags.mux16_4 == Single) 
        maxchannelplusone = 2;                                                  //single channel VW

    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      
        maxchannelplusone = 5;                                                  //4 channel VW mux

    if (MUX4_ENABLE.mflags.mux16_4 == VW8)
        maxchannelplusone = 9;                                                  //8 channel VW mux

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) 
        maxchannelplusone = 17;                                                 //16 channel VW mux

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) 
        maxchannelplusone = 33;                                                 //32 channel VW mux

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) 
        maxchannelplusone = 9;                                                  //8 channel mux

    if (MUX4_ENABLE.mflags.mux16_4 == TH32)
        maxchannelplusone = 33;                                                 //32 channel mux

    for (displayChannel = 1; displayChannel < maxchannelplusone; displayChannel++) 
    {
        IEC1bits.INT1IE = 0;                                                    //Disable INT1

        if (MUX4_ENABLE.mflags.mux16_4 == Single)                               //Single Channel VW
            FRAMaddress = SingleVWBytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                  //4 channel VW mux
            FRAMaddress = VW4Bytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                  //8 channel VW mux
            FRAMaddress = VW8Bytes * (tempoutputPosition - 1)+(4 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                 //16 channel mux    
            FRAMaddress = VW16Bytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                 //32 channel VW mux
            FRAMaddress = VW32Bytes * (tempoutputPosition - 1)+(4 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                  //8 channel TH mux
            FRAMaddress = TH8Bytes * (tempoutputPosition - 1)+(2 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                 //32 channel TH mux
            FRAMaddress = TH32Bytes * (tempoutputPosition - 1)+(2 * (displayChannel - 1)) + 12; 



        //GAGE READING

        if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32) //8 channel or 32 channel Thermistor mux 
        {
            DEC_TEMP.decimaltemp=read_Int_FRAM(FRAMaddress);                    //Read the 16bit external thermistor reading from internal FRAM 
            extThermProcessed=INT16tof32();                                     //convert 16 bit reading to 32 bit float	

            if (extThermProcessed <= -10.0 | extThermProcessed >= 10.0) 
            {
                sprintf(BUF, "%.1f", extThermProcessed);                        //format the external thermistor reading 1 decimal place
            }

            if (extThermProcessed>-10.0 && extThermProcessed < 10.0)
            {
                sprintf(BUF, "%.2f", extThermProcessed);                        //format the external thermistor reading 2 decimal places
            }
            
            write_Flt_FRAM(scratchaddress,extThermProcessed);                   //write to scratch address  
            read_Flt_FRAM(scratchaddress,3);                                    //read back to load MSB,MMSB,MMMSB & LSB    
        } 
        else                                                                    //VW
        {
            read_Flt_FRAM(FRAMaddress, 0);                                      //Read the transducer reading from external FRAM
            IEC1bits.INT1IE = 1;                                                //Enable INT1
            sprintf(BUF, "%.3f", gageReading);                                  //format the transducer reading

            if (gageReading == -999999.0)
            {
                sprintf(BUF, "%.1f", gageReading);                              //format the error message (-999999.0)
            }
            gageReading = 0;                                                    //reset the gageReading
        }


        switch (displayChannel)                                                 //VW or 8 or 32 channel Thermistor Display
        {

            case 1:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 1 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH1)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH1))
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else 
                {
                    putsUART1(BUF);                                             //Display reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 2:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 2 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH2)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH2))                
                
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 3:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 3 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH3)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH3))                           
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 4:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 4 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH4)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH4))                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 5:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 5 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH5)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH5))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 6:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 6 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH6)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH6))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 7:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 7 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH7)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH7))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 8:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 8 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH8)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH8))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 9:

                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 9 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH9)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH9))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 10:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 10 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH10)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH10))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 11:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 11 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH11)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH11))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

            case 12:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 12 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH12)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH12))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 13:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 13 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH13)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH13))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 14:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 14 disabled   
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH14)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH14))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 15:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 15 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH15)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH15))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 16:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 16 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH16)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH16))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 17:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 17 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH17)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH17))                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 18:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 18 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH18)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH18))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 19:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 4 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH19)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH19))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 20:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 20 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH20)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH20))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                         //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                       // , DELIMITER
                }
                break;


            case 21:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 21 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH21)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH21))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 22:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 22 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH22)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH22))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 23:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 23 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH23)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH23))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 24:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 24 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH24)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH24))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 25:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 25 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH25)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH25))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 26:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 26 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH26)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH26))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 27:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 27 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH27)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH27))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 28:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 28 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH28)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH28))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 29:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 29 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH29)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH29))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 30:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 30 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH30)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH30))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 31:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 31 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH31)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH31))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;


            case 32:
                if (((MUX4_ENABLE.mflags.mux16_4==Single  |                     //Channel 32 disabled    
                    MUX4_ENABLE.mflags.mux16_4==VW4     |
                    MUX4_ENABLE.mflags.mux16_4==VW8     | 
                    MUX4_ENABLE.mflags.mux16_4==VW16    |  
                    MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH32)
                |  
                ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                  MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH32))                                                          
                {
                    putsUART1(DisabledDisplay);                                 //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                } 
                else
                {
                    putsUART1(BUF);                                             //Display VW reading
                    while (BusyUART1());
                    putcUART1(comma);                                           // , DELIMITER
                }
                break;

        }
        while (BusyUART1());
    }                                                                           //end of 1st for(displayChannel) loop


    if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32 &&
            MUX4_ENABLE.mflags.mux16_4 != VW8 && MUX4_ENABLE.mflags.mux16_4 != VW32) //Thermistor display for VW 
    {
        for (displayChannel = 1; displayChannel < maxchannelplusone; displayChannel++) 
        {
            if (displayChannel != 1)                                                        
            {
                putcUART1(comma);                                               // , DELIMITER
                while (BusyUART1());
            }


            IEC1bits.INT1IE = 0;                                                //Disable INT1

            if (MUX4_ENABLE.mflags.mux16_4 == Single)                           //Single Channel VW
                FRAMaddress = SingleVWBytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

            if (MUX4_ENABLE.mflags.mux16_4 == VW4)                              //4 channel VW mux
                FRAMaddress = VW4Bytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

            if (MUX4_ENABLE.mflags.mux16_4 == VW8)                              //8 channel VW mux
                FRAMaddress = VW8Bytes * (tempoutputPosition - 1)+(4 * (displayChannel - 1)) + 12;

            if (MUX4_ENABLE.mflags.mux16_4 == VW16)                             //16 channel VW mux
                FRAMaddress = VW16Bytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

            if (MUX4_ENABLE.mflags.mux16_4 == VW32)                             //32 channel VW mux
                FRAMaddress = VW32Bytes * (tempoutputPosition - 1)+(4 * (displayChannel - 1)) + 12;

            //****************Get and format the readings from FRAM****************************************************
            //EXTERNAL THERMISTOR
            IEC1bits.INT1IE = 0;                                                //Disable INT1
            DEC_TEMP.decimaltemp=read_Int_FRAM(FRAMaddress+4);                  //Read the 12bit external thermistor reading from external EEPROM	
            IEC1bits.INT1IE = 1;                                                //Enable INT1

            extThermProcessed=INT16tof32();                                     //convert 16 bit reading to 32 bit float	

            if (extThermProcessed <= -10.0 | extThermProcessed >= 10.0)                         
                sprintf(BUF, "%.1f", extThermProcessed);                        //format the external thermistor reading 1 decimal place

            if (extThermProcessed>-10.0 && extThermProcessed < 10.0)
                sprintf(BUF, "%.2f", extThermProcessed);                        //format the external thermistor reading 2 decimal places
                
            write_Flt_FRAM(scratchaddress,extThermProcessed);                   //write to scratch address  
            read_Flt_FRAM(scratchaddress,3);                                    //read back to load MSB,MMSB,MMMSB & LSB    
            
            switch (displayChannel) 
            {

                case 1:

                    if (!MUX_ENABLE1_16.e1flags.CH1)                            //Channel 1 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }
                    break;

                case 2:

                    if (!MUX_ENABLE1_16.e1flags.CH2)                            //Channel 2 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }
                    break;

                case 3:

                    if (!MUX_ENABLE1_16.e1flags.CH3)                            //Channel 3 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }
                    break;

                case 4:

                    if (!MUX_ENABLE1_16.e1flags.CH4)                            //Channel 4 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }
                    break;

                case 5:

                    if (!MUX_ENABLE1_16.e1flags.CH5)                            //Channel 5 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }
                    break;

                case 6:

                    if (!MUX_ENABLE1_16.e1flags.CH6)                            //Channel 6 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }
                    break;

                case 7:

                    if (!MUX_ENABLE1_16.e1flags.CH7)                            //Channel 7 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 8:

                    if (!MUX_ENABLE1_16.e1flags.CH8)                            //Channel 8 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    }
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 9:

                    if (!MUX_ENABLE1_16.e1flags.CH9)                            //Channel 9 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 10:

                    if (!MUX_ENABLE1_16.e1flags.CH10)                           //Channel 10 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 11:

                    if (!MUX_ENABLE1_16.e1flags.CH11)                           //Channel 11 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 12:

                    if (!MUX_ENABLE1_16.e1flags.CH12)                           //Channel 12 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 13:

                    if (!MUX_ENABLE1_16.e1flags.CH13)                           //Channel 13 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 14:

                    if (!MUX_ENABLE1_16.e1flags.CH14)                           //Channel 14 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 15:

                    if (!MUX_ENABLE1_16.e1flags.CH15)                           //Channel 15 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 16:

                    if (!MUX_ENABLE1_16.e1flags.CH16)                           //Channel 16 disabled?
                    {
                        putsUART1(DisabledDisplay);                             //Yes - display "---"
                    } 
                    else
                    {
                        putsUART1(BUF);                                         //display reading
                    }

                    break;

                case 17:
                    
                    putsUART1(BUF);                                             //display reading
                    break;

                case 18:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 19:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 20:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 21:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 22:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 23:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 24:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 25:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 26:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 27:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 28:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 29:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 30:

                    putsUART1(BUF);                                             //display reading
                    break;

                case 31:
    
                    putsUART1(BUF);                                             //display reading
                    break;

                case 32:

                    putsUART1(BUF);                                             //display reading
                    break;


                default:

                    putsUART1(Error);                                           //"ERROR" (somethings wrong)
                    break;



            }

            while (BusyUART1());

        }                                                                       //end of 2nd for(displayChannel)

    }                                                                           // end of if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32)   


    if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32 &&
           MUX4_ENABLE.mflags.mux16_4 != VW8 && MUX4_ENABLE.mflags.mux16_4 != VW32)
        putcUART1(comma);                                                       // , DELIMITER
    sprintf(BUF, "%d", TXARRAY.array);                                       
    putsUART1(BUF);
    while (BusyUART1());
    crlf();
}

void displayScanInterval(unsigned long ScanInterval, unsigned int text) 
{
    char siBUF[8];
    
    sprintf(siBUF, "%ld", ScanInterval);                                        //format the scan interval data
    putsUART1(siBUF);                                                           //display it
    while (BusyUART1());    
    
    if (text)                                                                   //display "second(s)."
    {
        putsUART1(Seconds);
        while (BusyUART1());
    }
}



void displayTempReading(void) {
    char BUF[8];                                                                //temporary storage for display data
    unsigned char month;
    unsigned char day;
    unsigned int julian;
    unsigned int year;
    int intThermreading=0;                                                      
    int mainBatreading = 0;
    unsigned long i;                                                            
    unsigned int displayChannel = 0;
    unsigned int maxchannelplusone;
    unsigned long TempFRAMaddress = XmemStart;                                  //internal FRAM address for beginning of data    
    float mainBat = 0.0;
    float extThermProcessed = 0.0;
    unsigned long TESTSECONDS = 0;                                              

    restoreSettings();                                                          //load the flags from internal FRAM
    _232SHDN=1;                                                                

    if (LC2CONTROL.flags.ID)                                                    //Display ID if flag is set
    {

        for (i = IDaddress; i < CH1GTaddress; i += 2)                           //parse the buffer and extract the ID character   
        {
            data=read_Int_FRAM(i);                                              //read the ID starting FRAM location    
            unpack(data);                                                       //unpack into (2) bytes
            if (Hbyte == cr)                                                    //exit if MSB = <CR>
                break;
            putcUART1(Hbyte);
            while (BusyUART1());
            if (Lbyte == cr)                                                    //exit if LSB = <CR>
                break;
            putcUART1(Lbyte);
            while (BusyUART1());
        }
        putcUART1(comma);                                                       // , DELIMITER
        while (BusyUART1());
    }

    //TIME STAMP
    //YEAR
    IEC1bits.INT1IE = 0;                                                        //Disable INT2
    year=read_Int_FRAM(XmemStart);                                              //Read the year from the internal FRAM  
    IEC1bits.INT1IE = 1;                                                        //Enable INT2
    putcUART1(two);                                                             //year
    while (BusyUART1());
    putcUART1(zero);
    while (BusyUART1());
    if (year < 10) {                                                            //add leading zero if required
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(BUF, "%d", year);                                                   //format it
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());

    putcUART1(comma);                                                           // , DELIMITER
    while (BusyUART1());

    //DAY OF YEAR
    IEC1bits.INT1IE = 0;                                                        //Disable INT2
    julian=read_Int_FRAM(XmemStart+2);                                          //Read the decimal date from internal FRAM  
    IEC1bits.INT1IE = 1;                                                        //Enable INT2
    if (LC2CONTROL.flags.DateFormat)                                            //month,day
    {
        month = toMonthDay(julian, year, 1);                                    //get the month from the julian date
        day = toMonthDay(julian, year, 0);                                      //get the day from the julian date

        sprintf(BUF, "%d", month);                                              //format the month data
        putsUART1(BUF);                                                         //display it
        while (BusyUART1());

        putcUART1(comma);                                                       // , DELIMITER
        while (BusyUART1());

        sprintf(BUF, "%d", day);                                                //format the day data
        putsUART1(BUF);                                                         //display it
        while (BusyUART1());
    } else                                                                      //decimal day
    {
        sprintf(BUF, "%d", julian);
        putsUART1(BUF);
        while (BusyUART1());
    }

    putcUART1(comma);                                                           // , DELIMITER
    while (BusyUART1());

    IEC1bits.INT1IE = 0;                                                        //Disable INT2
    TESTSECONDS=read_longFRAM(XmemStart+4); 
    IEC1bits.INT1IE = 1;                                                        //Enable INT2
    seconds2hms(TESTSECONDS);

    //HOUR
    if (hour < 10)                                                              //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }

    sprintf(BUF, "%d", hour);                                                   //format the hour data
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());

    if (LC2CONTROL.flags.TimeFormat)                                            //hh,mm format
    {
        putcUART1(comma);                                                       // , DELIMITER
        while (BusyUART1());
    }

    //MINUTE
    if (minute < 10)                                                            //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(BUF, "%d", minute);                                                 //format the minute data
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());

    putcUART1(comma);                                                           // , DELIMITER
    while (BusyUART1());

    //SECOND
    sprintf(BUF, "%d", second);                                                 //format the second data
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());

    putcUART1(comma);                                                           // , DELIMITER
    while (BusyUART1());

    //****************Get and format the readings from FRAM****************************************************
    //MAIN BATTERY
    IEC1bits.INT1IE = 0;                                                        //Disable INT2
    mainBatreading=read_Int_FRAM(XmemStart+8);                                  //Read the 12bit main Bat reading from internal FRAM    
    IEC1bits.INT1IE = 1;                                                        //Enable INT2

    if (mainBatreading >= 0) {
        mainBat = (((Vref * mainBatreading) / 4096) * mul3V);                   //format the 3V battery data 
        if (mainBat < 0.3)
            mainBat = 0.0;
    } else {
        mainBat = (((Vref * mainBatreading) / 4096) * mul12V);                  //format the 12V battery data 	
        mainBat *= -1.0;                                                        //convert to positive
        if (mainBat < 0.3)
            mainBat = 0.0;
    }

    //INTERNAL THERMISTOR
    IEC1bits.INT1IE = 0;                                                        //Disable INT2
    intThermreading=read_Int_FRAM(XmemStart+10);                                //Read the 12bit internal thermistor reading from internal FRAM 
    IEC1bits.INT1IE = 1; //Enable INT2
    processDS3231Temperature(intThermreading);                                  




    //*************************************Display the readings**************************************************************

    //MAIN BATTERY
    sprintf(BUF, "%.2f", mainBat);
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());

    putcUART1(comma);                                                           // , DELIMITER
    while (BusyUART1());

    //INTERNAL TEMPERATURE:
    sprintf(BUF, "%d", DS3231Integer);                                          //format the DS3231 Temperature Integer portion
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(decimal);                                                         //decimal point
    while(BusyUART1());
    DS3231Fraction*=25;                                                         //Format the DS3231 fractional portion
    sprintf(BUF, "%d", DS3231Fraction);                                         
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());    
       
    //GAGE READING & THERMISTOR READINGS

    if (MUX4_ENABLE.mflags.mux16_4 == Single) {
        maxchannelplusone = 2;                                                  //Single Channel
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //determine # of channels in MUX    
    {
        maxchannelplusone = 5;                                                  //4 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //determine # of channels in MUX    
    {
        maxchannelplusone = 9;                                                  //8 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) 
    {
        maxchannelplusone = 17;                                                 //16 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) 
    {
        maxchannelplusone = 33;                                                 //32 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) 
    {
        maxchannelplusone = 9;                                                  //8 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) 
    {
        maxchannelplusone = 33;                                                 //8 CHANNEL
    }

    if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32)     
    {
        for (displayChannel = 1; displayChannel < maxchannelplusone; displayChannel++) 
        {
            putcUART1(comma);                                                   // , DELIMITER
            while (BusyUART1());

            //GAGE READING
            IEC1bits.INT1IE = 0;                                                //Disable INT2

            if(MUX4_ENABLE.mflags.mux16_4==Single)                              
                TempFRAMaddress=XmemStart+0xE;                                  
            else                                                                
            if(MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16)
                TempFRAMaddress = (XmemStart+0xE) + (6 * (displayChannel - 1)); //compute the reading address    
            else
            if (MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) 
                 TempFRAMaddress = (XmemStart+0xC) + (4 * (displayChannel - 1));   

            Nop();

            if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32) 
            {
                read_Flt_FRAM(TempFRAMaddress,0);                               //Read the transducer reading from internal FRAM    
                sprintf(BUF, "%.3f", gageReading);                              //format the transducer reading      
                if (gageReading==-999999.0)                                     
                    sprintf(BUF, "%.1f", gageReading);                          //format the error message (-999999.0)   
                gageReading = 0;                                                //reset the gage reading   
            }                                                                     
            IEC1bits.INT1IE = 1;                                                //Enable INT2


            if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32) 
            {
                switch (displayChannel) 
                {

                    case 1:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 1 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH1)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH1))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                    case 2:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 2 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH2)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH2))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        

                    case 3:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 3 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH3)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH3))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 4:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 4 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH4)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH4))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 5:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 5 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH5)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH5))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        

                    case 6:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 6 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH6)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH6))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        

                    case 7:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 7 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH7)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH7))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 8:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 8 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH8)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH8))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 9:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 9 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH9)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH9))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 10:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 10 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH10)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH10))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 11:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 11 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH11)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH11))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 12:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 12 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH12)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH12))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 13:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 13 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH13)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH13))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 14:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 14 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH14)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH14))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 15:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 15 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH15)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH15))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 16:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 16 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH16)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH16))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 17:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 17 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH17)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH17))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                        


                    case 18:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 18 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH18)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH18))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 19:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 19 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH19)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH19))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 20:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 20 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH20)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH20))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 21:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 21 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH21)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH21))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 22:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 22 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH22)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH22))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 23:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 23 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH23)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH23))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 24:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 24 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH24)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH24))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 25:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 25 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH25)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH25))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 26:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 26 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH26)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH26))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 27:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 27 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH27)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH27))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 28:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 28 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH28)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH28))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 29:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 29 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH29)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH29))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 30:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 30 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH30)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH30))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 31:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 31 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH31)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH31))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    case 32:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 32 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH32)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH32))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;                

                    default:

                        putsUART1(Error);                                       //"ERROR" (somethings wrong)
                        break;
                }
                while (BusyUART1());
            }
        }                                                                       //end of 1st for(displayChannel) loop
    }


    for (displayChannel = 1; displayChannel < maxchannelplusone; displayChannel++) {
        putcUART1(comma);                                                       // , DELIMITER
        while (BusyUART1());

        //EXTERNAL THERMISTOR
        IEC1bits.INT1IE = 0;                                                    //Disable INT2
        if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32)  
            TempFRAMaddress = (XmemStart+0xC) + (6 * (displayChannel - 1));     //compute memory location for external thermistor   
        else
            TempFRAMaddress = (XmemStart+0xC) + (2 * (displayChannel - 1));     //compute memory location for external thermistor   

        DEC_TEMP.decimaltemp=read_Int_FRAM(TempFRAMaddress);                    //Read the 16bit external thermistor reading from internal FRAM 
        IEC1bits.INT1IE = 1;                                                    //Enable INT2
        extThermProcessed=INT16tof32();                                         //convert 16 bit reading to 32 bit float	
            
        if (extThermProcessed <= -10.0 | extThermProcessed >= 10.0)         
            sprintf(BUF, "%.1f", extThermProcessed);                            //format the external thermistor reading 1 decimal place

        if (extThermProcessed>-10.0 && extThermProcessed < 10.0)
            sprintf(BUF, "%.2f", extThermProcessed);                            //format the external thermistor reading 2 decimal places


        switch (displayChannel) {

                case 1:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 1 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH1)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH1))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 2:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 2 disabled   
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH2)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH2))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 3:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 3 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH3)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH3))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 4:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 4 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH4)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH4))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 5:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 5 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH5)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH5))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 6:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 6 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH6)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH6))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 7:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 7 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH7)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH7))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 8:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 8 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH8)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH8))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 9:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 9 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH9)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH9))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 10:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 10 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH10)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH10))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 11:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 11 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH11)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH11))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 12:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 12 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH12)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH12))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 13:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 13 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH13)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH13))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 14:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 14 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH14)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH14))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 15:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 15 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH15)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH15))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 16:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 16 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE1_16.e1flags.CH16)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE1_16.t1flags.CH16))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 17:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 17 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH17)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH17))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;


                case 18:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 18 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH18)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH18))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 19:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 19 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH19)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH19))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 20:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 20 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH20)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH20))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 21:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 21 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH21)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH21))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 22:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 22 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH22)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH22))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 23:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 23 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH23)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH23))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 24:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 24 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH24)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH24))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 25:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 25 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH25)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH25))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 26:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 26 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH26)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH26))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 27:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 27 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH27)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH27))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 28:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 28 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH28)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH28))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 29:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 29 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH29)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH29))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 30:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 30 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH30)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH30))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 31:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 31 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH31)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH31))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;

                case 32:

                        if (((MUX4_ENABLE.mflags.mux16_4==Single  |             //Channel 32 disabled    
                            MUX4_ENABLE.mflags.mux16_4==VW4     |
                            MUX4_ENABLE.mflags.mux16_4==VW8     | 
                            MUX4_ENABLE.mflags.mux16_4==VW16    |  
                            MUX4_ENABLE.mflags.mux16_4==VW32) && !MUX_ENABLE17_32.e2flags.CH32)
                            |  
                            ((MUX4_ENABLE.mflags.mux16_4==TH8     |
                            MUX4_ENABLE.mflags.mux16_4==TH32)  && !THMUX_ENABLE17_32.t2flags.CH32))
                            {
                                putsUART1(DisabledDisplay);                     //Yes - display "---"
                            }
                            else
                            {
                                putsUART1(BUF);                                 //No - display reading
                            }
                            break;



                default:

                    putsUART1(Error);                                           //"ERROR" (somethings wrong)
                    break;

            }

            while (BusyUART1());
        }
                                                                 
    crlf();

}

void displayTemporaryStatus(int tempUserPosition) {
    char BUF[6];                                                                //temporary storage for display
    unsigned int pointer = 0;
    crlf();

    putsUART1(MS);                                                              //MS:
    while (BusyUART1());                                                        //Display Memory Status
    pointer=read_Int_FRAM(MemoryStatusaddress);                                 //get Memory Status (MS) pointer  
    sprintf(BUF, "%d", pointer);                                                //format it
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());

    putsUART1(OP);                                                              //OP:
    while (BusyUART1());                                                        //Display Output Position
    pointer=read_Int_FRAM(OutputPositionaddress);                               //get Output Position (OP) pointer  
    sprintf(BUF, "%d", pointer);                                                //format it
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());

    putsUART1(UP);                                                              //UP:
    while (BusyUART1());                                                        //Display Temporary User Position
    pointer = tempUserPosition;

    if (pointer == (maxSingleVW + 1))                                           //restore pointer to 1  
        pointer = 1;

    sprintf(BUF, "%d", pointer);
    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
}

void displayTestMenu(void) {
    crlf();
    crlf();

    putsUART1(TM1);                                                             //LC-2x4 TEST MENU
    while (BusyUART1());
    crlf();
    crlf();

    putsUART1(TM2);                                                             //SELECTION         TEST
    while (BusyUART1());
    crlf();
    crlf();

    putsUART1(TM9);                                                             //    1         ALL FRAM
    while (BusyUART1());
    crlf();

    putsUART1(TM10);                                                            //    2         +3V_X
    while (BusyUART1());
    crlf();

    putsUART1(TM11);                                                            //    3         RTC 32KHz
    while (BusyUART1());
    crlf();
    
    putsUART1(TM4);                                                             //      4         EXCITATION
    while (BusyUART1());
    crlf();
    
    putsUART1(TM5);                                                             //      5         TEST VW CHANNEL
    while (BusyUART1());
    crlf();

    putsUART1(TM13);                                                            //    <ESC>     EXIT TEST MENU
    while (BusyUART1());
    crlf();

    crlf();
    crlf();

    putsUART1(TM12);                                                            //ENTER SELECTION:
    while (BusyUART1());
}


void enableAlarm(unsigned char alarm)
{
    unsigned char temp;
	
    if(alarm==1)                                                                //RTC Alarm 1 (Reading)
    {
        setClock(0x0A,0x80);                                                    //Alarm1 when hours,minutes & seconds match
        PORT_CONTROL.flags.Alarm1Enabled=1;                                     //Set the Alarm1 enabled flag   
        write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);          //store flag in EEPROM  
		
        temp=readClock(RTCControlAddress);                                      //get the value in the RTC Control Register
        temp|=0x05;                                                             //OR it with 00000101 to set A1E=1,INTCN=1
        setClock(RTCControlAddress,temp);                                       //write updated Control register to RTC
		
        temp=readClock(RTCStatusAddress);                                       //get the value in the RTC Status Register
        temp&=0xFE;                                                             //AND it with 11111110 to clear A1F
        setClock(RTCStatusAddress,temp);                                        //write updated Status register to RTC	
    }

    if(alarm==2)                                                                //RTC Alarm 2 (Control Port)
    {
        setClock(0x0D,0x80);                                                    //Alarm2 when hours & minutes match
        IEC1bits.INT1IE=1;                                                      //enable the INT1 interrupt 
        PORT_CONTROL.flags.Alarm2Enabled=1;                                     //Set the Alarm2 enabled flag
        write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);          //store flag in EEPROM
        temp=readClock(RTCControlAddress);                                      //get the value in the RTC Control Register
        temp|=0x06;                                                             //OR it with 00000110 to set A2E=1,INTCN=1
        setClock(RTCControlAddress,temp);                                       //write updated Control register to RTC
        temp=readClock(RTCStatusAddress);                                       //get the value in the RTC Status Register
        temp&=0xFD;                                                             //AND it with 11111101 to clear A2F
        setClock(RTCStatusAddress,temp);                                        //write updated Status register to RTC
        enableINT1();                                                           //enable INT1 (respond to RTC interrupt) 
    }

    setClock(0x0F,0);                                                           //Clear the RTC Alarm flags

}


void enableBT(void)                                                      
{
   //TURN ON BLUETOOTH:
    BT_ENABLE=1;                                                                //Powerup Bluetooth     
    _BT_RESET=1;                                                                //Release from RESET    
    BT_BAUD=0;                                                                  //Set baudrate to 115.2k    
    DISPLAY_CONTROL.flags.BT=1;                                                 //set the BT flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM  
    S_1.status1flags._BT=1;                                                     //set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
    PMD3bits.T6MD=0;                                                            //Enable Timer 6   
}

void enableChannel(int channel) 
{

    switch (channel) 
    {
        case 1:
            MUX_ENABLE1_16.e1flags.CH1 = 1;
            break;

        case 2:
            MUX_ENABLE1_16.e1flags.CH2 = 1;
            break;

        case 3:
            MUX_ENABLE1_16.e1flags.CH3 = 1;
            break;

        case 4:
            MUX_ENABLE1_16.e1flags.CH4 = 1;
            break;

        case 5:
            MUX_ENABLE1_16.e1flags.CH5 = 1;
            break;

        case 6:
            MUX_ENABLE1_16.e1flags.CH6 = 1;
            break;

        case 7:
            MUX_ENABLE1_16.e1flags.CH7 = 1;
            break;

        case 8:
            MUX_ENABLE1_16.e1flags.CH8 = 1;
            break;

        case 9:
            MUX_ENABLE1_16.e1flags.CH9 = 1;
            break;

        case 10:
            MUX_ENABLE1_16.e1flags.CH10 = 1;
            break;

        case 11:
            MUX_ENABLE1_16.e1flags.CH11 = 1;
            break;

        case 12:
            MUX_ENABLE1_16.e1flags.CH12 = 1;
            break;

        case 13:
            MUX_ENABLE1_16.e1flags.CH13 = 1;
            break;

        case 14:
            MUX_ENABLE1_16.e1flags.CH14 = 1;
            break;

        case 15:
            MUX_ENABLE1_16.e1flags.CH15 = 1;
            break;

        case 16:
            MUX_ENABLE1_16.e1flags.CH16 = 1;
            break;

        case 17:
            MUX_ENABLE17_32.e2flags.CH17 = 1;
            break;

        case 18:
            MUX_ENABLE17_32.e2flags.CH18 = 1;
            break;

        case 19:
            MUX_ENABLE17_32.e2flags.CH19 = 1;
            break;

        case 20:
            MUX_ENABLE17_32.e2flags.CH20 = 1;
            break;

        case 21:
            MUX_ENABLE17_32.e2flags.CH21 = 1;
            break;

        case 22:
            MUX_ENABLE17_32.e2flags.CH22 = 1;
            break;

        case 23:
            MUX_ENABLE17_32.e2flags.CH23 = 1;
            break;

        case 24:
            MUX_ENABLE17_32.e2flags.CH24 = 1;
            break;

        case 25:
            MUX_ENABLE17_32.e2flags.CH25 = 1;
            break;

        case 26:
            MUX_ENABLE17_32.e2flags.CH26 = 1;
            break;

        case 27:
            MUX_ENABLE17_32.e2flags.CH27 = 1;
            break;

        case 28:
            MUX_ENABLE17_32.e2flags.CH28 = 1;
            break;

        case 29:
            MUX_ENABLE17_32.e2flags.CH29 = 1;
            break;

        case 30:
            MUX_ENABLE17_32.e2flags.CH30 = 1;
            break;

        case 31:
            MUX_ENABLE17_32.e2flags.CH31 = 1;
            break;

        case 32:
            MUX_ENABLE17_32.e2flags.CH32 = 1;
            break;

        default:
            return;
    }

    if(channel<17)                                                              
    {
        write_Int_FRAM(MUX_ENABLE1_16flagsaddress,MUX_ENABLE1_16.MUXen1_16);    //store flag in FRAM 
    }
    else                                                                        
    {
        write_Int_FRAM(MUX_ENABLE17_32flagsaddress,MUX_ENABLE17_32.MUXen17_32); //store flag in FRAM  
    }
}


void enableTHChannel(int channel)                                                  
{
    switch (channel) 
    {
        case 1:
            THMUX_ENABLE1_16.t1flags.CH1 = 1;
            break;

        case 2:
            THMUX_ENABLE1_16.t1flags.CH2 = 1;
            break;

        case 3:
            THMUX_ENABLE1_16.t1flags.CH3 = 1;
            break;

        case 4:
            THMUX_ENABLE1_16.t1flags.CH4 = 1;
            break;

        case 5:
            THMUX_ENABLE1_16.t1flags.CH5 = 1;
            break;

        case 6:
            THMUX_ENABLE1_16.t1flags.CH6 = 1;
            break;

        case 7:
            THMUX_ENABLE1_16.t1flags.CH7 = 1;
            break;

        case 8:
            THMUX_ENABLE1_16.t1flags.CH8 = 1;
            break;

        case 9:
            THMUX_ENABLE1_16.t1flags.CH9 = 1;
            break;

        case 10:
            THMUX_ENABLE1_16.t1flags.CH10 = 1;
            break;

        case 11:
            THMUX_ENABLE1_16.t1flags.CH11 = 1;
            break;

        case 12:
            THMUX_ENABLE1_16.t1flags.CH12 = 1;
            break;

        case 13:
            THMUX_ENABLE1_16.t1flags.CH13 = 1;
            break;

        case 14:
            THMUX_ENABLE1_16.t1flags.CH14 = 1;
            break;

        case 15:
            THMUX_ENABLE1_16.t1flags.CH15 = 1;
            break;

        case 16:
            THMUX_ENABLE1_16.t1flags.CH16 = 1;
            break;

        case 17:
            THMUX_ENABLE17_32.t2flags.CH17 = 1;
            break;

        case 18:
            THMUX_ENABLE17_32.t2flags.CH18 = 1;
            break;

        case 19:
            THMUX_ENABLE17_32.t2flags.CH19 = 1;
            break;

        case 20:
            THMUX_ENABLE17_32.t2flags.CH20 = 1;
            break;

        case 21:
            THMUX_ENABLE17_32.t2flags.CH21 = 1;
            break;

        case 22:
            THMUX_ENABLE17_32.t2flags.CH22 = 1;
            break;

        case 23:
            THMUX_ENABLE17_32.t2flags.CH23 = 1;
            break;

        case 24:
            THMUX_ENABLE17_32.t2flags.CH24 = 1;
            break;

        case 25:
            THMUX_ENABLE17_32.t2flags.CH25 = 1;
            break;

        case 26:
            THMUX_ENABLE17_32.t2flags.CH26 = 1;
            break;

        case 27:
            THMUX_ENABLE17_32.t2flags.CH27 = 1;
            break;

        case 28:
            THMUX_ENABLE17_32.t2flags.CH28 = 1;
            break;

        case 29:
            THMUX_ENABLE17_32.t2flags.CH29 = 1;
            break;

        case 30:
            THMUX_ENABLE17_32.t2flags.CH30 = 1;
            break;

        case 31:
            THMUX_ENABLE17_32.t2flags.CH31 = 1;
            break;

        case 32:
            THMUX_ENABLE17_32.t2flags.CH32 = 1;
            break;

        default:
            return;
    }

    if(channel<17)                                                              
    {
        write_Int_FRAM(THMUX_ENABLE1_16flagsaddress,THMUX_ENABLE1_16.THMUXen1_16);    //store flag in FRAM 
    }
    else                                                                        
    {
        write_Int_FRAM(THMUX_ENABLE17_32flagsaddress,THMUX_ENABLE17_32.THMUXen17_32); //store flag in FRAM  
    }
}

void enableINT1(void)
{
    ConfigINT1(FALLING_EDGE_INT & EXT_INT_PRI_5 & EXT_INT_ENABLE & GLOBAL_INT_ENABLE);  //Set IP to 5   
}


void ENLOGINT(void)                                                             
{
    LC2CONTROL.flags.LogInterval = 1;                                           //Set Log intervals flag
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //store flag in FRAM    
                            
    S_1.status1flags._Logint=1;                                                 //Set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);

    DISPLAY_CONTROL.flags.Synch = 0;                                            //clear the Synch flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM     
}


void enableOP(void)
{
    CONTROL = 1;                                                                //turn on control port
    _READ=0;                                                                    //LED ON    
    S_1.status1flags._OP=1;                                                     //set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
    PORT_CONTROL.flags.ControlPortON = 1;
    PORT_CONTROL.flags.O0issued = 0;                                            //clear O0issued flag
    write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);              //store flag in FRAM  
    if(!LC2CONTROL2.flags2.Modbus)                                              //Only if command line interface    
        controlPortStatus(1);                                                   //display control port status      
}

unsigned int f32toINT16(float value)                                            
{
	int chars=0;
	char decBUF[6];                                                           
	unsigned char ones=0;
	unsigned char tens=0;
	unsigned char hundreds=0;

	DEC_TEMP.temp.sign=0;
	DEC_TEMP.temp.whole=0;
	DEC_TEMP.temp.tenths=0;

	chars=sprintf(decBUF,"%.1f",value);                                         //convert float value to ascii string	
	if(decBUF[0]==minus)                                                        //if negative
		DEC_TEMP.temp.sign=1;                                                   //set the sign bit and store in DEC_TEMP.temp.sign
	DEC_TEMP.temp.tenths=(decBUF[chars-1]-0x30);                                //extract the tenths value & store in DEC_TEMP.temp.tenths
	if(chars-3>=0 && decBUF[chars-3]!=minus)
		ones=decBUF[chars-3]-0x30;                                              //extract ones
	if(chars-4>=0 && decBUF[chars-4]!=minus)
		tens=decBUF[chars-4]-0x30;                                              //extract tens
	if(chars-5>=0 && decBUF[chars-5]!=minus)
		hundreds=decBUF[chars-5]-0x30;                                          //extract hundreds
    
	DEC_TEMP.temp.whole=100*hundreds+10*tens+ones;                              //store whole number value in DEC_TEMP  

	return DEC_TEMP.decimaltemp;                                                //return the 16 bit value
}



void enableVWchannel(unsigned char gageType)                                    
{
    unsigned int timeHigh=0;
    unsigned char gainGT1=0x80;                                               
    unsigned char gainGT2=0x80;                                               
    unsigned char gainGT3=0x80;                                                 
    unsigned char gainGT4=0x80;
    unsigned char gainGT5=0x80;                                                 
    unsigned char gainGT6=0x80;
    
    _AMP_SHDN=1;                                                                //Enable the AGC amp    
    switch(gageType)
    {
        case 1:                                                                 //Set Fco for 4.770 KHz
            write_AD5241(gainGT1);                                              //Set initial gain

            if(LC2CONTROL2.flags2.uCclock)                                      //if HSPLL osc  
                timeHigh=31;                                                    //Fco=4.770KHz
            else
                timeHigh=7;                                                     //HS osc  

            A=0;                                                                //Setup PLL VCO
            B=0;
            C=0;                                                                //R=23.7K
            D=0;                                                                //C=470pF
            break;
            
        case 2:                                                                 //Set Fco for 6.370 KHz
            write_AD5241(gainGT2);                                              //Set initial gain
            
            if(LC2CONTROL2.flags2.uCclock)                                      //if HSPLL  
                timeHigh=23;                                                    //Fco=6.370KHz
            else
                timeHigh=5;                                                       
  
            A=1;                                                                //Setup PLL VCO
            B=1;
            C=0;                                                                //R=15.8K
            D=0;                                                                //C=470pF
            break;
            
        case 3:                                                                 //Set Fco for 1.603 KHz
            write_AD5241(gainGT3);                                              //Set initial gain
            
            if(LC2CONTROL2.flags2.uCclock)                                      //if HSPLL  
                timeHigh=92;                                                    //Fco=1.603KHZ
            else
                timeHigh=23;                                                    //Fco=1.603 KHz  

            A=1;                                                                //Setup PLL VCO
            B=0;
            C=0;                                                                //R=18K
            D=1;                                                                //C=1800pF
            break;
            
        case 4:                                                                 //Set Fco for 3.820 KHz
            write_AD5241(gainGT4);                                              //Set initial gain
            
            if(LC2CONTROL2.flags2.uCclock)                                      //if HSPLL  
                timeHigh=38;                                                    //Fco=3.820KHz
            else
                timeHigh=9;                                                     //Fco=3.820KHz  

            A=0;                                                                //Setup PLL VCO
            B=0;
            C=0;                                                                //R=23.7K
            D=0;                                                                //C=470pF
            break;
            
        case 5:                                                                 //Set Fco for 6.370 KHz
            write_AD5241(gainGT5);                                              //Set initial gain
            
            if(LC2CONTROL2.flags2.uCclock)                                      //if HSPLL  
                timeHigh=23;                                                    //Fco=6.370KHz
            else
                timeHigh=5;                                                     //Fco=6.370KHz     

            A=1;                                                                //Setup PLL VCO
            B=1;
            C=0;                                                                //R=15.8K
            D=0;                                                                //C=470pF
            break;
            
        case 6:                                                                 //Set Fco for 2.168 KHz
            write_AD5241(gainGT6);                                              //Set initial gain
            
            if(LC2CONTROL2.flags2.uCclock)                                      //if HSPLL  
                timeHigh=68;                                                    //2.168KHz
            else
                timeHigh=17;                                                    //2.168KHz  

            A=1;                                                                //Setup PLL VCO
            B=1;
            C=1;                                                                //R=9.98K
            D=1;                                                                //C=1800pF
            break;
            
        default:
            write_AD5241(gainGT5);                                              //Set initial gain
            
            if(LC2CONTROL2.flags2.uCclock)                                      //if HSPLL  
                timeHigh=23;                                                    //Fco=6.370KHz
            else
                timeHigh=5;                                                     //Fco=6.370KHz  
            
            A=1;                                                                //Setup PLL VCO
            B=1;
            C=0;                                                                //R=15.8K
            D=0;                                                                //C=470pF
    }

    
    PMD2bits.OC1MD=0;                                                           //Enable Output Compare1 module
    OC1CON=0x0000;                                                              //Turn off Output Compare 1 module
    OC1CON=0x0001;                                                              //Enable OC1 module with pin initially low    
    OC1CONbits.OCM1=1;                                                          //setup OC1 module for compare event toggles OC1 pin
    OC1CONbits.OCTSEL=0;                                                        //Timer2 is the clock source
    OC1R=timeHigh;                                                              //setup compare register

    PMD1bits.T2MD=0;                                                            //Enable Timer2 module    
    T2CONbits.TCKPS=0;                                                          //1:1 Timer2 Prescaler
    T2CONbits.T32=0;                                                            //Timer2 is 16 bit
    T2CONbits.TCS=0;                                                            //Clock is Fcy
    PR2=timeHigh;                                                               //set the Timer2 Period Register 
    T2CONbits.TON=1;                                                            //Start Timer2
}


unsigned int filterArray(unsigned int unfiltered[])                             
{
    unsigned int filtered[16];                                                  //filtered array
    unsigned int result=0;                                                      //final filtered value
    unsigned char i=0;                                                          //index
    
    for(i=0;i<16;i++)                                                           //perform 5 point moving average  
    {
        filtered[i]=(unfiltered[i]+unfiltered[i+1]+unfiltered[i+2]+unfiltered[i+3]+unfiltered[i+4])/5;
    }
    
    for(i=0;i<16;i++)                                                           //totalize the results
    {
        result+=filtered[i];
    }
    
    result = result >> 4;
    return result;                                                              //return the final filtered value
}


void formatandDisplayGageInfo(float TEMPVAL) {
    char BUF[8];                                                                //temporary storage for gage info

    if ((TEMPVAL >= 0.0 && TEMPVAL < 1.0) | (TEMPVAL >= -1.0 && TEMPVAL < 0.0))
        sprintf(BUF, "%.5f", TEMPVAL);                                          //format it

    if ((TEMPVAL >= 1.0 && TEMPVAL < 10.0) | (TEMPVAL >= -10.0 && TEMPVAL<-1.0))
        sprintf(BUF, "%.5f", TEMPVAL);                                          //format it

    if ((TEMPVAL >= 10.0 && TEMPVAL < 100.0) | (TEMPVAL >= -100.0 && TEMPVAL<-10.0))
        sprintf(BUF, "%.5f", TEMPVAL);                                          //format it

    if ((TEMPVAL >= 100.0 && TEMPVAL < 1000.0) | (TEMPVAL >= -1000.0 && TEMPVAL<-100.0))
        sprintf(BUF, "%.4f", TEMPVAL);                                          //format it

    if ((TEMPVAL >= 1000.0 && TEMPVAL < 10000.0) | (TEMPVAL >= -10000.0 && TEMPVAL<-1000.0))
        sprintf(BUF, "%.3f", TEMPVAL);                                          //format it

    if ((TEMPVAL >= 10000.0 && TEMPVAL < 100000.0) | (TEMPVAL >= -100000.0 && TEMPVAL<-10000.0))
        sprintf(BUF, "%.2f", TEMPVAL);                                          //format it

    if ((TEMPVAL >= 100000.0 && TEMPVAL < 1000000.0) | (TEMPVAL>-1000000.0 && TEMPVAL<-100000.0))
        sprintf(BUF, "%.1f", TEMPVAL);                                          //format it

    if (TEMPVAL >= 1000000.0 | TEMPVAL<-1000000.0)
        sprintf(BUF, "%.0f", TEMPVAL);                                          //format it


    putsUART1(BUF);                                                             //display it
    while (BusyUART1());
}


int getChannel(void) {
    int channelOnes = 0;
    int channelTens = 0;

    if (isdigit(buffer[1]))                                                     //examine the first channel char
        channelOnes = buffer[1] - 0x30;

    if (buffer[2] == '/')
        return channelOnes;                                                     //return the single digit channel #

    channelTens = channelOnes;                                                  //shift first digit into 10's place

    if (isdigit(buffer[2]))
        channelOnes = buffer[2] - 0x30;                                         //get the 1's digit

    channelTens *= 10;                                                          //multiply 10's by 10
    channelTens = channelTens + channelOnes;                                    //add the ones
    return channelTens;                                                         //return the 2 digit channel #
}


unsigned char getSNbytes(unsigned int logicthreshold)                           //using the ADC as a halfassed UART    
{                                                                               
    unsigned int databit=0;                                                     //value of bit received
    unsigned char b=0;                                                          //loop index
    unsigned char s=0;                                                          //loop index

    //Configure Timer 6/7 as 32-bit timer:
    INTCON2bits.ALTIVT=1;                                                       //set to use alternate interrupt vectors
    PMD3bits.T6MD=0;                                                            //Enable TMR6
    PMD3bits.T7MD=0;                                                            //Enable TMR7
    T6CONbits.T32=1;                                                            //Set TMR6/7 for 32bit mode
    T6CONbits.TCKPS=0;                                                          //Set TMR6 prescaler to 1:1
    T7CONbits.TCKPS=0;                                                          //Set TMR7 prescaler to 1:0
    T6CONbits.TCS=0;                                                            //set to count internal (Fosc/2) clocks
    T7CONbits.TCS=0;                                                            //set to count internal (Fosc/2) clocks 
    T6CONbits.TGATE=0;                                                          //Disable gated time accumulation
    
    for(b=0;b<6;b++)                                                            //will be receiving 6 bytes
    {
        PR6=mS1_5LSW;                                                           //Load TMR6 period register with 1.5mS least significant word   
        PR7=mS1_5MSW;                                                           //Load TMR7 period register with 1.5mS most significant word    
        IFS3bits.T7IF=0;                                                        //clear the TMR7 interrupt flag
        IEC3bits.T7IE=1;                                                        //Enable TMR7 interrupt  
        IPC12bits.T7IP=6;                                                       //Set TMR7 interrupt priority to 6    
        TMR6=0;                                                                 //clear the TMR6 register
        TMR7=0;                                                                 //clear the TMR7 register   
        

        
        //wait for start bit:    
        while((databit<logicthreshold) && !IFS3bits.T8IF)                       //wait for the start bit    
        {
            databit=take_fast_analog_reading();                                 
        } 
        
        if(IFS3bits.T8IF)                                                       //0.5S timeout?
        {
            IFS3bits.T8IF=0;    
            INTCON2bits.ALTIVT=0;                                               //reset to use standard interrupt vectors

            T6CONbits.T32=0;                                                    //Set TMR6/7 for 16bit mode
            PMD3bits.T6MD=1;                                                    //Disable TMR6
            PMD3bits.T7MD=1;                                                    //Disable TMR7
            return 0;
        }
    
        VWflagsbits.timeout=0;                                                  //make sure timeout flag is clear
        T6CONbits.TON=1;                                                        //Start TMR 6/7
    
        while(!VWflagsbits.timeout);                                            //wait for 1.65mS
        T6CONbits.TON=0;                                                        //Turn off timer
        VWflagsbits.timeout=0;                                                  //reset the timeout flag
        
        //Sample start bit:
        databit=take_fast_analog_reading();                                     
        
        if(databit<logicthreshold)
        {
            INTCON2bits.ALTIVT=0;                                               //reset to use standard interrupt vectors
            T6CONbits.T32=0;                                                    //Set TMR6/7 for 16bit mode
            PMD3bits.T6MD=1;                                                    //Disable TMR6
            PMD3bits.T7MD=1;                                                    //Disable TMR7
            return 0;                                                           //Error - return 0
        }
    
        for(s=0;s<9;s++)                                                        //get the bits of the byte
        {
            databit=0;
            PR6=mS3_15LSW;                                                      //Load TMR6 period register with 3.15mS least significant word  
            PR7=mS3_15MSW;                                                      //Load TMR7 period register with 3.15mS most significant word   
            TMR6=0;                                                             //clear the TMR6 register
            TMR7=0;                                                             //clear the TMR7 register       
            T6CONbits.TON=1;                                                    //Start the 3.33mS timer        
            while(!VWflagsbits.timeout);                                        //sample in the middle of the bit period
            T6CONbits.TON=0;                                                    //Turn off timer
            VWflagsbits.timeout=0;                                              //reset the timeout flag 

            databit=take_fast_analog_reading();                                 
            
            if(s==8 && (databit>logicthreshold))                                //stop bit not detected
            {
                INTCON2bits.ALTIVT=0;                                           //reset to use standard interrupt vectors
                T6CONbits.T32=0;                                                //Set TMR6/7 for 16bit mode
                PMD3bits.T6MD=1;                                                //Disable TMR6
                PMD3bits.T7MD=1;                                                //Disable TMR7
                return 0;                                                       //Error - return 0
            }

            
            if(s==8 && (databit<logicthreshold))
                break;                                                          //byte received so exit
        
            switch(s)
            {
                case 0:
                if(databit>logicthreshold)
                    SN_BITS.temp.bit0=0;                                        //databit>threshold so logic 0
                else
                    SN_BITS.temp.bit0=1;                                        //databit<threshold so logic 1
                break;
                
                case 1:
                if(databit>logicthreshold)
                    SN_BITS.temp.bit1=0;                                        //databit>threshold so logic 0
                else
                    SN_BITS.temp.bit1=1;                                        //databit<threshold so logic 1
                break;
                
                case 2:
                if(databit>logicthreshold)
                    SN_BITS.temp.bit2=0;                                        //databit>threshold so logic 0
                else
                    SN_BITS.temp.bit2=1;                                        //databit<threshold so logic 1
                break;               
                
                case 3:
                if(databit>logicthreshold)
                    SN_BITS.temp.bit3=0;                                        //databit>threshold so logic 0
                else
                    SN_BITS.temp.bit3=1;                                        //databit<threshold so logic 1
                break;                
                
                case 4:
                if(databit>logicthreshold)
                    SN_BITS.temp.bit4=0;                                        //databit>threshold so logic 0
                else
                    SN_BITS.temp.bit4=1;                                        //databit<threshold so logic 1
                break;               
                
                case 5:
                if(databit>logicthreshold)
                    SN_BITS.temp.bit5=0;                                        //databit>threshold so logic 0
                else
                    SN_BITS.temp.bit5=1;                                        //databit<threshold so logic 1
                break;               
                
                case 6:
                if(databit>logicthreshold)
                    SN_BITS.temp.bit6=0;                                        //databit>threshold so logic 0
                else
                    SN_BITS.temp.bit6=1;                                        //databit<threshold so logic 1
                break;               
                
                case 7:
                if(databit>logicthreshold)
                    SN_BITS.temp.bit7=0;                                        //databit>threshold so logic 0
                else
                    SN_BITS.temp.bit7=1;                                        //databit<threshold so logic 1
                break;                
            }
        }
    
        _SNbuf[b]=SN_BITS.sn;                                                   

    }

    INTCON2bits.ALTIVT=0;                                                       //reset to use standard interrupt vectors
    T6CONbits.T32=0;                                                            //Set TMR6/7 for 16bit mode
    PMD3bits.T6MD=1;                                                            //Disable TMR6
    PMD3bits.T7MD=1;                                                            //Disable TMR7
    return 1;                                                                   //6 bytes received ok - return 1
}

int getConversion(void) {
    if ((buffer[2] == slash && buffer[3] == capL && buffer[4] == slash)
            | (buffer[3] == slash && buffer[4] == capL && buffer[5] == slash))  //linear conversion?
        return 0;

    if ((buffer[2] == slash && buffer[3] == capP && buffer[4] == slash)
            | (buffer[3] == slash && buffer[4] == capP && buffer[5] == slash))  //polynomial conversion?
        return 1;

    return 2;                                                                   //illegal entry
}

void getGageInfo(unsigned char info, int channel) {
    unsigned char i = 0;                                                        //buffer index
    unsigned char dp = 0;                                                       //decimal point location
    unsigned char x;                                                            //location in buffer of beginning of ZR slash
    unsigned char y;                                                            //location in buffer of beginning of GF slash
    unsigned char z;                                                            //location in buffer of beginning of GO slash
    unsigned char negative = 0;                                                 //switch for negative number
    float whole = 0.0;                                                          //whole number portion of floating point value
    float fractional = 0.0;                                                     //fractional portion of floating point value
    float mul = 1.0;                                                            //decimal weight multiplier
    float div = 0.1;                                                            //decimal weight multiplier


    for (i; buffer[i] != slash; i++)                                            //parse buffer for location of first '/'
    {
        if (buffer[i] == cr)
            return;
    }

    ++i;

    for (i; buffer[i] != slash; i++)                                            //parse buffer for location of second '/'
    {
        if (buffer[i] == cr)
            return;
    }

    ++i;

    for (i; buffer[i] != slash; i++)                                            //parse buffer for location of third '/'
    {
        if (buffer[i] == cr)
            return;
    }

    for (i; buffer[i] != slash && buffer[i] != cr; i++)                         //parse buffer for location of 4th '/'
    {
        if (buffer[i] == cr)
            return;
    }

    for (i; buffer[i] != slash && buffer[i] != cr; i++)                         //parse buffer for location of 5th '/'
    {
        if (buffer[i] == cr)
            return;
    }

    x = i;                                                                      //x=index of this location
    i++;                                                                        //increment to next buffer location


    for (i; buffer[i] != slash && buffer[i] != cr; i++)                         //parse buffer for location of 6th '/'
    {
        if (buffer[i - 1] == cr)
            return;                                                             //break out of loop if 6th '/' is not found
    }

    y = i;                                                                      //y=index of this location
    i++;                                                                        //increment to next buffer location

    if (info != 1)                                                              //don't go here if zero reading
    {
        for (i; buffer[i] != slash && buffer[i] != cr; i++)                     //parse buffer for location of 7th '/'
        {
            if (buffer[i - 1] == cr)
                return;                                                         //break out of loop if 7th '/' is not found
        }

        z = i;                                                                  //z=index of this location
    }


    switch (info)                                                               //determine information to extract
    {
        case 1:                                                                 //zero reading or polynomial coefficient A
            break;                                                              //x&y already determined

        case 2:                                                                 //gage factor or polynomial coefficient B
            x = y;
            y = z;
            break;

        case 3:                                                                 //gage offset or polynomial coefficient C
            x = z;
            y = cr;                                                             //<CR> = end of buffer
            break;

        default:
            break;
    }

    if (info == 3)                                                              //<CR> indicates end of buffer
    {
        for (i = x + 1; buffer[i] != cr; i++)                                   //locate decimal point
        {
            if (buffer[i] == decimal)
                dp = i;                                                         //store index of decimal point
        }
    } else {
        for (i = x + 1; i < y; i++)                                             //locate decimal point
        {
            if (buffer[i] == decimal)
                dp = i;                                                         //store index of decimal point
        }
    }

    if (dp)                                                                     //if a decimal point was found
    {
        while (buffer[dp - 1] != slash) {
            if (buffer[dp - 1] == minus)                                        //determine if value is negative
            {
                negative = 1;                                                   //turn on negative switch
                break;
            }

            whole += (buffer[dp - 1] - 0x30) * mul;                             //extract the whole number
            dp -= 1;                                                            //decrement the pointer
            mul *= 10;                                                          //increment the power of ten
        }

        for (dp; buffer[dp] != decimal; dp++) {
        }                                                                       //go back to the decimal point

        while (buffer[dp + 1] != slash && buffer[dp + 1] != cr)                 //get the fractional part
        {
            fractional += (buffer[dp + 1] - 0x30) * div;                        //extract the fractional part
            dp += 1;                                                            //increment the pointer
            div /= 10;                                                          //decrement the power of ten
        }

        whole += fractional;                                                    //return the final floating point number	
        if (negative)
            whole *= -1.0;
    } else                                                                      //no decimal point in value
    {
        while (buffer[i - 1] != slash) {
            if (buffer[i - 1] == minus)                                         //determine if value is negative
            {
                whole *= -1.00;                                                 //negate
                break;
            }

            whole += (buffer[i - 1] - 0x30) * mul;                              //extract the whole number
            i -= 1;                                                             //decrement the pointer
            mul *= 10;                                                          //increment the power of ten
        }

    }

    switch (info)                                                               //determine information to extract
    {
        case 1:                                                                 //zero reading or polynomial coefficient A
            if (y != x + 1)                                                     //make sure user entered a value here
            {
                zeroReading = whole;                                            //zero reading was entered
                polyCoA = whole;                                                //polynomial coefficient A was entered
            }
            break;

        case 2:                                                                 //gage factor or polynomial coefficient B
            if (y != x + 1)                                                     //make sure user entered a value here
            {
                gageFactor = whole;                                             //gage factor was entered
                polyCoB = whole;                                                //polynomial coefficient B was entered
            }
            break;

        case 3:                                                                 //gage offset or polynomial coefficient C 
            if (buffer[z] == slash && buffer[z + 1] != cr) {
                gageOffset = whole;                                             //gage offset was entered
                polyCoC = whole;                                                //polynomial coefficient C was entered
            }
            break;

        default:
            break;
    }

    //Write to internal FRAM

    switch (channel) {
        case 1:                                                                 //CHANNEL 1

            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH1) {
                        write_Flt_FRAM(CH1ZRaddress,whole);                     //store the zero reading    
                    } else {
                        write_Flt_FRAM(CH1PolyCoAaddress,whole);                //store the Polynomial coefficient A    
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH1) {
                        write_Flt_FRAM(CH1GFaddress,whole);                     //store the gage factor 
                    } else {
                        write_Flt_FRAM(CH1PolyCoBaddress,whole);                //store the Polynomial coefficient B    
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH1) {
                        write_Flt_FRAM(CH1GOaddress,whole);                     //store the gage offset 
                    } else {
                        write_Flt_FRAM(CH1PolyCoCaddress,whole);                //store the Polynomial coefficient C    
                    }
                    break;
            }

            return;


        case 2:                                                                 //CHANNEL 2
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH2) {
                        write_Flt_FRAM(CH2ZRaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH2PolyCoAaddress,whole);                
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH2) {
                        write_Flt_FRAM(CH2GFaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH2PolyCoBaddress,whole);                
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH2) {
                        write_Flt_FRAM(CH2GOaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH2PolyCoCaddress,whole);                
                    }
                    break;
            }

            return;


        case 3:                                                                 //CHANNEL 3
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH3) {
                        write_Flt_FRAM(CH3ZRaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH3PolyCoAaddress,whole);                
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH3) {
                        write_Flt_FRAM(CH3GFaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH3PolyCoBaddress,whole);                
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH3) {
                        write_Flt_FRAM(CH3GOaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH3PolyCoCaddress,whole);                
                    }
                    break;
            }

            return;

        case 4:                                                                 //CHANNEL 4
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH4) {
                        write_Flt_FRAM(CH4ZRaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH4PolyCoAaddress,whole);                
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH4) {
                        write_Flt_FRAM(CH4GFaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH4PolyCoBaddress,whole);                
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH4) {
                        write_Flt_FRAM(CH4GOaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH4PolyCoCaddress,whole);                
                    }
                    break;
            }

            return;

        case 5:                                                                 //CHANNEL 5
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH5) {
                        write_Flt_FRAM(CH5ZRaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH5PolyCoAaddress,whole);                
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH5) {
                        write_Flt_FRAM(CH5GFaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH5PolyCoBaddress,whole);                
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH5) {
                        write_Flt_FRAM(CH5GOaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH5PolyCoCaddress,whole);                
                    }
                    break;
            }

            return;

        case 6:                                                                 //CHANNEL 6
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH6) {
                        write_Flt_FRAM(CH6ZRaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH6PolyCoAaddress,whole);                
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH6) {
                        write_Flt_FRAM(CH6GFaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH6PolyCoBaddress,whole);                
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH6) {
                        write_Flt_FRAM(CH6GOaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH6PolyCoCaddress,whole);                
                    }
                    break;
            }

            return;

        case 7:                                                                 //CHANNEL 7
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH7) {
                        write_Flt_FRAM(CH7ZRaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH7PolyCoAaddress,whole);                
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH7) {
                        write_Flt_FRAM(CH7GFaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH7PolyCoBaddress,whole);                
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH7) {
                        write_Flt_FRAM(CH7GOaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH7PolyCoCaddress,whole);                
                    }
                    break;
            }

            return;


        case 8:                                                                 //CHANNEL 8
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH8) {
                        write_Flt_FRAM(CH8ZRaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH8PolyCoAaddress,whole);                
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH8) {
                        write_Flt_FRAM(CH8GFaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH8PolyCoBaddress,whole);                
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH8) {
                        write_Flt_FRAM(CH8GOaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH8PolyCoCaddress,whole);                
                    }
                    break;
            }

            return;

        case 9:                                                                 //CHANNEL 9
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH9) {
                        write_Flt_FRAM(CH9ZRaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH9PolyCoAaddress,whole);                
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH9) {
                        write_Flt_FRAM(CH9GFaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH9PolyCoBaddress,whole);                
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH9) {
                        write_Flt_FRAM(CH9GOaddress,whole);                     
                    } else {
                        write_Flt_FRAM(CH9PolyCoCaddress,whole);                
                    }
                    break;
            }

            return;


        case 10:                                                                //CHANNEL 10
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH10) {
                        write_Flt_FRAM(CH10ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH10PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH10) {
                        write_Flt_FRAM(CH10GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH10PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH10) {
                        write_Flt_FRAM(CH10GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH10PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;


        case 11:                                                                //CHANNEL 11
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH11) {
                        write_Flt_FRAM(CH11ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH11PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH11) {
                        write_Flt_FRAM(CH11GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH11PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH11) {
                        write_Flt_FRAM(CH11GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH11PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 12:                                                                //CHANNEL 12
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH12) {
                        write_Flt_FRAM(CH12ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH12PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH12) {
                        write_Flt_FRAM(CH12GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH12PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH12) {
                        write_Flt_FRAM(CH12GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH12PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 13:                                                                //CHANNEL 13
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH13) {
                        write_Flt_FRAM(CH13ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH13PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH13) {
                        write_Flt_FRAM(CH13GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH13PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH13) {
                        write_Flt_FRAM(CH13GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH13PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 14:                                                                //CHANNEL 14
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH14) {
                        write_Flt_FRAM(CH14ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH14PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH14) {
                        write_Flt_FRAM(CH14GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH14PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH14) {
                        write_Flt_FRAM(CH14GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH14PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 15:                                                                //CHANNEL 15
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH15) {
                        write_Flt_FRAM(CH15ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH15PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH15) {
                        write_Flt_FRAM(CH15GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH15PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH15) {
                        write_Flt_FRAM(CH15GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH15PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;


        case 16:                                                                //CHANNEL 16
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH16) {
                        write_Flt_FRAM(CH16ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH16PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH16) {
                        write_Flt_FRAM(CH16GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH16PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH16) {
                        write_Flt_FRAM(CH16GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH16PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 17:                                                                //CHANNEL 17
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH17) {
                        write_Flt_FRAM(CH17ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH17PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH17) {
                        write_Flt_FRAM(CH17GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH17PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH17) {
                        write_Flt_FRAM(CH17GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH17PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 18:                                                                //CHANNEL 18
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH18) {
                        write_Flt_FRAM(CH18ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH18PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH18) {
                        write_Flt_FRAM(CH18GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH18PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH18) {
                        write_Flt_FRAM(CH18GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH18PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 19:                                                                //CHANNEL 19
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH19) {
                        write_Flt_FRAM(CH19ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH19PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH19) {
                        write_Flt_FRAM(CH19GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH19PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH19) {
                        write_Flt_FRAM(CH19GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH19PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 20:                                                                //CHANNEL 20
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH20) {
                        write_Flt_FRAM(CH20ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH20PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH20) {
                        write_Flt_FRAM(CH20GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH20PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH20) {
                        write_Flt_FRAM(CH20GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH20PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 21:                                                                //CHANNEL 21
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH21) {
                        write_Flt_FRAM(CH21ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH21PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH21) {
                        write_Flt_FRAM(CH21GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH21PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH21) {
                        write_Flt_FRAM(CH21GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH21PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 22:                                                                //CHANNEL 22
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH22) {
                        write_Flt_FRAM(CH22ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH22PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH22) {
                        write_Flt_FRAM(CH22GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH22PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH22) {
                        write_Flt_FRAM(CH22GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH22PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 23:                                                                //CHANNEL 23
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH23) {
                        write_Flt_FRAM(CH23ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH23PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH23) {
                        write_Flt_FRAM(CH23GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH23PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH23) {
                        write_Flt_FRAM(CH23GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH23PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 24:                                                                //CHANNEL 24
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH24) {
                        write_Flt_FRAM(CH24ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH24PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH24) {
                        write_Flt_FRAM(CH24GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH24PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH24) {
                        write_Flt_FRAM(CH24GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH24PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 25:                                                                //CHANNEL 25
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH25) {
                        write_Flt_FRAM(CH25ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH25PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH25) {
                        write_Flt_FRAM(CH25GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH25PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH25) {
                        write_Flt_FRAM(CH25GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH25PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 26:                                                                //CHANNEL 26
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH26) {
                        write_Flt_FRAM(CH26ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH26PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH26) {
                        write_Flt_FRAM(CH26GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH26PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH26) {
                        write_Flt_FRAM(CH26GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH26PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 27:                                                                //CHANNEL 27
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH27) {
                        write_Flt_FRAM(CH27ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH27PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH27) {
                        write_Flt_FRAM(CH27GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH27PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH27) {
                        write_Flt_FRAM(CH27GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH27PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 28:                                                                //CHANNEL 28
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH28) {
                        write_Flt_FRAM(CH28ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH28PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH28) {
                        write_Flt_FRAM(CH28GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH28PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH28) {
                        write_Flt_FRAM(CH28GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH28PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 29:                                                                //CHANNEL 29
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH29) {
                        write_Flt_FRAM(CH29ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH29PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH29) {
                        write_Flt_FRAM(CH29GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH29PolyCoBaddress,whole);                                      
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH29) {
                        write_Flt_FRAM(CH29GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH29PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

        case 30:                                                                //CHANNEL 30
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH30) {
                        write_Flt_FRAM(CH30ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH30PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH30) {
                        write_Flt_FRAM(CH30GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH30PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH30) {
                        write_Flt_FRAM(CH30GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH30PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;


        case 31:                                                                //CHANNEL 31
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH31) {
                        write_Flt_FRAM(CH31ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH31PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH31) {
                        write_Flt_FRAM(CH31GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH31PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH31) {
                        write_Flt_FRAM(CH31GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH31PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;


        case 32:                                                                //CHANNEL 32
            switch (info) {
                case 1:                                                         //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH32) {
                        write_Flt_FRAM(CH32ZRaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH32PolyCoAaddress,whole);               
                    }
                    break;

                case 2:                                                         //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH32) {
                        write_Flt_FRAM(CH32GFaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH32PolyCoBaddress,whole);               
                    }
                    break;

                case 3:                                                         //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH32) {
                        write_Flt_FRAM(CH32GOaddress,whole);                    
                    } else {
                        write_Flt_FRAM(CH32PolyCoCaddress,whole);               
                    }
                    break;
            }

            return;

    }
}

int getGageType(int channel) 
{
    int gageOnes = 0;

    if (channel < 10)                                                           //channels 1-9
    {
        if (buffer[5] == '/')                                                   //forces skipping the gage type
            return -1;

        if (!isdigit(buffer[5]))                                                //illegal entry
            return -2;

        if (isdigit(buffer[5]))                                                 //examine the first gage type char
            gageOnes = buffer[5];                                               //store to gageOnes if digit

        if (buffer[6] == '/') {
            gageOnes -= 0x30;                                                   //convert to int
            if (gageOnes >= 0 && gageOnes <= 6)                                 
            {
                return gageOnes;                                                //return the single digit gage type from 0 to 5
            } else {
                return -2;                                                      //illegal entry
            }
        } else {
            return -2;                                                          //illegal entry
        }
    }

    if (channel >= 10 && channel <= 32)                                         //channels 10-32                
    {
        if (buffer[6] == '/')                                                   //forces skipping the gage type
            return -1;

        if (!isdigit(buffer[6]))                                                //illegal entry
            return -2;

        if (isdigit(buffer[6]))                                                 //examine the first gage type char
            gageOnes = buffer[6];                                               //store to gageOnes if digit

        if (buffer[7] == '/') {
            gageOnes -= 0x30;                                                   //convert to int
            if (gageOnes >= 0 && gageOnes <= 6)                                 
            {
                return gageOnes;                                                //return the single digit gage type from 0 to 5
            } else {
                return -2;                                                      //illegal entry
            }
        } else {
            return -2;                                                          //illegal entry
        }
    }

}

int getLogInterval(void) {
    int LogInt;


    LogInt = buffer[1];                                                         //examine the first Interval char

    if (!isdigit(buffer[1]))                                                    //illegal entry
        return -2;

    LogInt -= 0x30;                                                             //convert ascii to decimal

    if (LogInt < 1 | LogInt > 6)                                                //illegal Log Interval
        return -2;


    if (buffer[2] == cr)                                                        //display Log Interval value and iterations
    {
        displayLogInterval(LogInt);
        return -2;                                                              //go back to * prompt after displaying Log Interval value and iterations
    }


    if (buffer[2] == '/')
        return LogInt;                                                          //return the single digit Interval
}

void getLogIntLength(void) {
    int i = 0;
    int Length = 0;
    int slashcounter = 0;
    int startindex = 0;
    int stopindex = 0;


    if (buffer[2] == '/' && buffer[3] == '/')                                   //forces skipping to Log Iterations
    {
        LogIntLength = -1;
        return;
    }

    for (i = 0; i < 13; i++)                                                    //locate the first and last digit of the interval length
    {
        if (buffer[i] == slash && slashcounter == 0) {
            startindex = i + 1;                                                 //location in buffer of first interval length digit
            slashcounter = 1;
            continue;                                                           //go to next buffer index
        }

        if (buffer[i] == slash && slashcounter == 1) {
            stopindex = i - 1;                                                  //location in buffer of last interval length digit
            break;                                                              //break out of for loop
        }
    }

    Length = stopindex - startindex;

    if (Length > 4) 
    {
        LogIntLength = -1;                                                      //error
        return;
    }

    switch (Length)                                                             //compute the integer value of the interval length
    {
        case 0:
            if (!isdigit(buffer[startindex]))                                   //not a digit
            {
                LogIntLength = -1;                                              //error
                return;
            }

            LogIntLength = buffer[startindex] - 0x30;                           //ones

            if (LogIntLength < 1) {
                LogIntLength = -2;                                              //error
                return;
            }

            break;

        case 1:                                                                 //return the value that's between 10 and 99
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]))                           //not a digit
            {
                LogIntLength = -1;                                              //error
                return;
            }


            LogIntLength = ((buffer[startindex] - 0x30)*10.0)+
                    (buffer[startindex + 1] - 0x30);                            //tens + ones
            break;

        case 2:                                                                 //return the value that's between 100 and 999
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]) |
                    !isdigit(buffer[startindex + 2]))                           //not a digit
            {
                LogIntLength = -1;                                              //error
                return;
            }

            LogIntLength = ((buffer[startindex] - 0x30)*100.0)+
                    ((buffer[startindex + 1] - 0x30)*10.0)+
                    (buffer[startindex + 2] - 0x30);                            //hundreds + tens + ones
            break;

        case 3:                                                                 //return the value that's between 1000 and 9999
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]) |
                    !isdigit(buffer[startindex + 2]) |
                    !isdigit(buffer[startindex + 3]))                           //not a digit
            {
                LogIntLength = -1;                                              //error
                return;
            }

            LogIntLength = ((buffer[startindex] - 0x30)*1000.0)+
                    ((buffer[startindex + 1] - 0x30)*100.0)+
                    ((buffer[startindex + 2] - 0x30)*10.0)+
                    (buffer[startindex + 3] - 0x30);                            //thousands + hundreds + tens + ones
            break;

        case 4:                                                                 //return the value that's between 10000 and 99999
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]) |
                    !isdigit(buffer[startindex + 2]) |
                    !isdigit(buffer[startindex + 3]) |
                    !isdigit(buffer[startindex + 4]))                           //not a digit
            {
                LogIntLength = -1;                                              //error
                return;
            }

            LogIntLength = ((buffer[startindex] - 0x30)*10000.0)+
                    ((buffer[startindex + 1] - 0x30)*1000.0)+
                    ((buffer[startindex + 2] - 0x30)*100.0)+
                    ((buffer[startindex + 3] - 0x30)*10.0)+
                    (buffer[startindex + 4] - 0x30);                            //ten-thousands + thousands + hundreds + tens + ones

            if (LogIntLength > 86400)                                           //error
            {
                LogIntLength = -1;
                return;
            }

            break;

        default:
            LogIntLength = -1;                                                  //error
    }

}

int getLogIterations(void) {
    int i = 0;
    int iterations = 0;
    int Length = 0;
    int slashcounter = 0;
    int startindex = 0;
    int stopindex = 0;


    for (i = 0; i < 13; i++)                                                    //locate the first and last digit of the iterations
    {
        if (buffer[i] == slash && slashcounter == 0) {
            slashcounter = 1;                                                   //first '/'
            continue;
        }

        if (buffer[i] == slash && slashcounter == 1) {
            startindex = i + 1;                                                 //location in buffer of first iterations digit
            slashcounter = 2;
            continue;                                                           //go to next buffer index
        }

        if (buffer[i] == cr && slashcounter == 2) {
            stopindex = i - 1;                                                  //location in buffer of last iterations digit
            break;                                                              //break out of for loop
        }
    }

    Length = stopindex - startindex;

    if (Length > 2)                                                             //>999
        return -1;                                                              //error


    switch (Length)                                                             //compute the integer value of the iterations
    {
        case 0:
            if (!isdigit(buffer[startindex]))                                   //not a digit
            {
                iterations = -1;                                                //error
                break;
            }

            iterations = buffer[startindex] - 0x30;                             //ones
            break;

        case 1:                                                                 //return the value that's between 10 and 99
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]))                           //not a digit
            {
                iterations = -1;                                                //error
                break;
            }


            iterations = ((buffer[startindex] - 0x30)*10.0)+
                    (buffer[startindex + 1] - 0x30);                            //tens + ones
            break;

        case 2:                                                                 //return the value that's between 100 and 999
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]) |
                    !isdigit(buffer[startindex + 2]))                           //not a digit
            {
                iterations = -1;                                                //error
                break;
            }

            iterations = ((buffer[startindex] - 0x30)*100.0)+
                    ((buffer[startindex + 1] - 0x30)*10.0)+
                    (buffer[startindex + 2] - 0x30);                            //hundreds + tens + ones

            if (iterations >= 256)                                              //error
                return -1;

            break;

        default:
            iterations = -1;                                                    //error
    }

    return iterations;

}


//***************************************************************************
//						getFrequency()
//
//	
//	Parameters received: none
//	Returns: float
//
//	Called from: main()
//
//***************************************************************************
float getFrequency(void)
{
	unsigned long VWcount=0;
	float frequency=0.0;
	float frequencyTotal=0.0;                                                   
	unsigned int i=0;                                                           
	unsigned int loopsTotal=1;                                                  
    unsigned int AGC=0;                                                          
	
    enableVWchannel(gageType);                                                  //configure the PLL and LPF 
    configTimers(); 
    
    TRISB=0x033D;                                                               //Configure PORTB   
    LATB=0;                                                                     

    __delay32(mS100);                                                           //100mS delay for Signal Level Measurement   
    AGC=take_analog_reading(98);                                                //Check the VW signal level      
    
    if(AGC<VAGC_MIN)                                                            //if less than 2.25V threshold  
    {
        disableTimers();
        return 0.0;                                                             
    }
   
	for(i=0;i<loopsTotal;i++)
	{
        
		IFS0bits.T1IF=0;                                                        //clear TMR1,TMR5,TMR7 interrupt flags	
		IFS1bits.T5IF=0;
        IFS3bits.T7IF=0;
        
        TMR7=0;                                                                 
        VWcount=0;                                                              
		VWcountMSW=0;                                                           //clear the counter MSW register  
		VWcountLSW=0;                                                           //clear the counter LSW register	
        PR7=0xFFFF;                                                             //load PR7 with max count  

        if(VWflagsbits.retry)                                                   
        {
            PR4=mS64LSW;                                                           
            PR5=mS64MSW;                                                                       
        }
        else
        {
            PR4=mS512LSW;                                                                                                                 
            PR5=mS512MSW;                                                       
        }

        T7CONbits.TCS=1;                                                        //set to count external T7CKI clocks
		IEC1bits.T5IE=1;                                                        //enable 512mS interrupt
        INTCON1bits.NSTDIS=0;                                                   //enable interrupt nesting  
        IPC7bits.T5IP=6;                                                        //set INT5 priority to 6  
        IPC12bits.T7IP=7;                                                       //Set TMR7 interrupt priority to 7  
        IEC3bits.T7IE=1;                                                        //Enable TMR7 interrupt         

		//Synchronize to VW:
		while(VW100 && !IFS0bits.T1IF);                                         //wait while VW(100) is high
		while(!VW100 && !IFS0bits.T1IF);                                        //wait while VW(100) is low

		T7CONbits.TON=1;                                                        //start VW100 counter	
		if(!IFS0bits.T1IF)                                                      //Capture frequency				
		{
			T4CONbits.TON=1;                                                    //start 256mS timer	
            
            while(!IFS0bits.T1IF && CaptureFlag==0);                            //wait for 256mS gate to terminate or 300mS time out to occur    

			if(IFS0bits.T1IF)
            {
                disableTimers();                                                
                return 0;                                                       //timeout waiting for VW_100
            }

            T7CONbits.TON=0;                                                    //shut off counter
			T4CONbits.TON=0;                                                    //shut off timer
			CaptureFlag=0;                                                      //Reset captureFlag		
			VWcount=(VWcountMSW*65536)+VWcountLSW;                              //Totalize counter  
            VWcount-=1;                                                         

            if(VWflagsbits.retry)                                               
                frequency=(VWcount/mS64)*10.0;                                  //convert to frequency	
            else
                frequency=(VWcount/mS512)*10.0;                                 //convert to frequency	
			frequencyTotal=frequencyTotal+frequency;                            

		}
	}
    
    AGC=take_analog_reading(98);                                                //Check the VW signal level again     
    
    if(AGC<VAGC_MIN)                                                            //if less than 2.25V threshold  
    {
        disableTimers();
        return 0.0;                                                             
    }
    
	frequency=frequencyTotal/(loopsTotal*1.0);                                  
    disableTimers();
	return frequency;
}


unsigned char getSerialNumber(void)
{
    csum crc;
    unsigned char result=0;
    unsigned int logicthreshold=0;
    unsigned int therm=0;
    unsigned int Vmax=0;                                                        //value of max V at TH (Rth = 0)       
   
    //Configure PORTB:
    TRISB=0x033D;                                                               //Configure PORTB
    LATB=0;                                                                     //Set PORTB outputs low
    
    //Setup TMR8 as 0.5S background timer                                       
    PMD3bits.T8MD=0;                                                            //Enable TMR8
    T8CONbits.TON=0;                                                            //Make sure timer is off
    T8CONbits.T32=0;                                                            //set to 16bit
    T8CONbits.TCS=0;                                                            //Internal Tcy
    T8CONbits.TGATE=0;                                                          //Disable gated timer mode
    T8CONbits.TCKPS=3;                                                          //1:256 prescaler 
    TMR8=0;                                                                     //clear the TMR8 register
    PR8=mS500;                                                                  //load the period register with 0.5S timeout value
    IPC12bits.T8IP=1;                                                           //set TMR8 interrupt priority to 1 (lowest)
    IFS3bits.T8IF=0;                                                            //Clear the interrupt flag
    IEC3bits.T8IE=0;                                                            //Disable TMR8 interrupt

    //Start the 0.5S background timer                                           
    T8CONbits.TON=1;
    
    //Turn on +3VX:
    _3VX_on();
    
    //Get the logic threshold:
    therm=take_fast_analog_reading();                                           //get the baseline thermistor reading   

    //Apply DC excitation to the VW:
    pluckON();      
    pluckPOS();
    
    __delay32(mS20);                                                            //20mS delay    
    Vmax=take_fast_analog_reading();                                            //get the max therm voltage 
    logicthreshold=therm+((Vmax-therm)/2);                                      //determine the logic threshold (varies with temperature)
    __delay32(mS10);                                                            //10mS delay    
 
    //Get the 4 byte serial number and 2 byte checksum. 
    result=getSNbytes(logicthreshold);                                        
    pluckOFF();
    _3VX_off();
    
    if(result==0)                                                               //error        
        return 0;
    
    //Check CRC:
    crc.c=CRC_SN();                                                             
    
    if((_SNbuf[5] != crc.z[1]) | (_SNbuf[4] != crc.z[0]))                       //received crc does not agree with computed crc_    
    {
        return 0;                                                               
    }
    else   
    {
        //Store Serial Number in FRAM:                                          
        crc.z[1]=_SNbuf[0];                                                     //Serial Number MSB
        crc.z[0]=_SNbuf[1];
        write_Int_FRAM(GAGE_SERIAL_MSWaddress,crc.c);                           //store Serial Number MSW in FRAM`
        crc.z[1]=_SNbuf[2];                                               
        crc.z[0]=_SNbuf[3];                                                     //Serial Number LSB
        write_Int_FRAM(GAGE_SERIAL_LSWaddress,crc.c);                           //store Serial Number LSW in FRAM`        
        return 1;                                                               //Serial Number reading good
    }
}


char getThermtype(void)
{

	if(buffer[2]==slash && buffer[4]==cr)
	{
        if(buffer[3]-0x30>=0 && buffer[3]-0x30<=3)                              
			return buffer[3]-0x30;                                              //0,1,2,3
	}
	
	if(buffer[3]==slash && buffer[5]==cr)
	{
        if(buffer[4]-0x30>=0 && buffer[4]-0x30<=3)                              
		return buffer[4]-0x30;                                                  //0,1,2,3
	}

	return -1;                                                                  //illegal entry
}


unsigned long getTxChecksum(unsigned int arraySize)                             
{
    unsigned long accumulator=0;                                                //sum of array elements to calculate checksum
    unsigned int acc=0;                                                         //loop index

    for(acc=1;acc<arraySize;acc++)   
    {
        accumulator+=TxBinaryBUF[acc];                                          
    }

    accumulator=(~(accumulator)+1);                                             //two's compliment
    return accumulator;                                                         //checksum
}




void handleCOMError(void) {
    U1STAbits.OERR = 0;                                                         //clear flag if overrun error
    U1MODEbits.UARTEN = 0;                                                      //disable the UART to clear its registers
    U1MODEbits.UARTEN = 1;                                                      //re-enable the UART
}

void handleFRAMResults(unsigned int testData) 
{
    if (testData==0) 
    {
        crlf();
        putsUART1(TP);                                                          //"TEST PASSED"
        while (BusyUART1());
    } 
    else 
    {
        crlf();
        putsUART1(TFAIL);                                                       //"TEST FAILED @"
        while (BusyUART1());
    }
}

void hms(unsigned long interval, unsigned char destination) {
    char BUFFER[11];
    unsigned int ScanHours = 0;                                                 //initialize
    unsigned int ScanMinutes = 0;
    unsigned int ScanSeconds = 0;

    if (LC2CONTROL.flags.ScanError)                                             //is error flag set?
    {
        LC2CONTROL.flags.ScanError = 0;                                         //clear the flag
        return;                                                                 //and exit
    }

    while (1) {
        if (interval < 60) {
            ScanSeconds = interval;                                             //extract seconds and return
            break;
        }

        if (interval > 3599) {
            ScanHours += 1;                                                     //increment hours
            interval -= 3600;
        } else {
            ScanMinutes += 1;                                                   //increment minutes
            interval -= 60;
        }
    }

    if ((destination == 0) && !LC2CONTROL2.flags2.Modbus)                       
    {
        write_Int_FRAM(ScanHoursaddress,ScanHours);                             //store HMS in FRAM`
        write_Int_FRAM(ScanMinutesaddress,ScanMinutes);        
        write_Int_FRAM(ScanSecondsaddress,ScanSeconds);        
    }

    if (destination == 1)                                                       //store HMS in RTC Alarm1
    {
        BUFFER[0] = 'S';
        BUFFER[1] = 'T';

        toBCD(ScanHours);

        BUFFER[2] = BCDtens + 0x30;                                             //convert hours to ascii & store in BUFFER
        BUFFER[3] = BCDones + 0x30;

        BUFFER[4] = colon;                                                      // : delimiter

        toBCD(ScanMinutes);

        BUFFER[5] = BCDtens + 0x30;                                             //convert minutes to ascii & store in BUFFER
        BUFFER[6] = BCDones + 0x30;

        BUFFER[7] = colon;                                                      // : delimiter

        toBCD(ScanSeconds);

        BUFFER[8] = BCDtens + 0x30;                                             //convert seconds to ascii & store in BUFFER
        BUFFER[9] = BCDones + 0x30;

        BUFFER[10] = cr;                                                        //end of string

        PORT_CONTROL.flags.SetAlarm2StopTime=0;                                 //clear flag    
        PORT_CONTROL.flags.SetAlarm2Time=0;                                     //clear flag    
        LC2CONTROL.flags.LoggingStartTime = 1;                                  //set flag 
        LC2CONTROL2.flags2.SetStartTime = 1;
        Buf2DateTime(BUFFER);                                                   //Load the RTC Alarm1 register
        LC2CONTROL.flags.LoggingStartTime = 0;                                  //reset flag 
        LC2CONTROL2.flags2.SetStartTime = 0;
    }
}

unsigned long hms2s(void) 
{
    unsigned long total = 0;
   
    total=(read_Int_FRAM(ScanHoursaddress)*3600.0)+(read_Int_FRAM(ScanMinutesaddress)*60.0)+(read_Int_FRAM(ScanSecondsaddress));   

    return total;
}


float INT16tof32(void)                                                          
{
	float recoveredvalue=0.0;

	recoveredvalue=DEC_TEMP.temp.whole+(DEC_TEMP.temp.tenths/10.0);             //convert 16 bit stored value to 32 bit float
	if(DEC_TEMP.temp.sign)
		recoveredvalue*=-1.0;                                                   //negate if negative
	return recoveredvalue;
}

void loadDefaults(void) 
{
    unsigned long i;                                                            //loop index

    //	Clear the SetStopTime flag:
    LC2CONTROL2.flags2.SetStopTime = 0;                                         //clear the set stop time flag
    LC2CONTROL2.flags2.scheduled=0;                                             
    S_1.status1flags._SP=0;                                                     //clear the MODBUS Stop Time flag
    S_1.status1flags._ST=0;                                                     //clear the MODBUS Start Time flag  
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);                  //store flag in FRAM  

    // Linear Conversions:
    MUX_CONVERSION1_16.MUXconv1_16 = 0;                                         //All Linear Conversion
    MUX_CONVERSION17_32.MUXconv17_32 = 0;                                       //All Linear Conversion
    write_Int_FRAM(MUX_CONV1_16flagsaddress,MUX_CONVERSION1_16.MUXconv1_16); 
    write_Int_FRAM(MUX_CONV17_32flagsaddress,MUX_CONVERSION17_32.MUXconv17_32);  

    //Enable all channels:
    for(i=1;i<33;i++)                                                           
    {                                                                           
        enableChannel(i);                                                       
    }                                                                           

    //initialize gage types to 1:
    for (i = CH1GTaddress; i < CH32GTaddress + 1; i += 2)                       //channel gage type selection loop 
    {
        write_Int_FRAM(i,1);                                                    //write 1 to channel gage type  
    }

    //initialize zero reading to 0:
    for (i = CH1ZRaddress; i < CH32ZRaddress + 1; i += 4)                       //channel zero reading selection loop   
    {
        write_Flt_FRAM(i,0.0);                                                  
    }


    //initialize gage factor to 1:
    for (i = CH1GFaddress; i < CH32GFaddress + 1; i += 4)                       //channel gage factor selection loop 
    {
        write_Flt_FRAM(i,1.0);                                                  
    }

    //initialize gage offset to 0:
    for (i = CH1GOaddress; i < CH32GOaddress + 1; i += 4)                       //channel gage offset selection loop 
    {
        write_Flt_FRAM(i,0.0);                                                  
    }

    //initialize polynomial coefficient A to 0:
    for (i = CH1PolyCoAaddress; i < CH32PolyCoAaddress + 1; i += 4)             //channel polynomial coefficient A selection loop  
    {
        write_Flt_FRAM(i,0.0);                                                  
    }

    //initialize polynomial coefficient B to 1:
    for (i = CH1PolyCoBaddress; i < CH32PolyCoBaddress + 1; i += 4)             //channel polynomial coefficient B selection loop  
    {
        write_Flt_FRAM(i,1.0);                                                  
    }

    //initialize polynomial coefficient C to 0:
    for (i = CH1PolyCoCaddress; i < CH32PolyCoCaddress + 1; i += 4)             //channel polynomial coefficient C selection loop  
    {
        write_Flt_FRAM(i,0.0);                                                  
    }
    
    for(i=CH1THaddress;i<CH32THaddress+1;i+=0x0002)                             //all thermistors type 1    
	{
		write_Int_FRAM(i,1);                                                    //write 1 to channel thermistor type    
	}
    
    //Enable all thermistor channels:
   write_Int_FRAM(THMUX_ENABLE1_16flagsaddress, 0xFFFF);                        
   write_Int_FRAM(THMUX_ENABLE17_32flagsaddress, 0xFFFF);                       

    //initialize scan intervals:
    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //Single VW Channel
    {
        S_1.status1flags._CFG=Single;                                           
        LogIntLength = minScanSingleVW;
        hms(minScanSingleVW, 0);
        
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //4 channel VW MUX
    {
        S_1.status1flags._CFG=VW4;                                              
        LogIntLength = minScanFourVW;
        hms(minScanFourVW, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //8 channel VW MUX
    {
        S_1.status1flags._CFG=VW8;                                              
        LogIntLength = minScanEightVW;
        hms(minScanEightVW, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                     //16 channel VW MUX
    {
        S_1.status1flags._CFG=VW16;                                             
        LogIntLength = minScanSixteenVW;
        hms(minScanSixteenVW, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                     //32 channel VW MUX
    {
        S_1.status1flags._CFG=VW32;                                             
        LogIntLength = minScanThirtytwoVW;
        hms(minScanThirtytwoVW, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                      //8 channel TH MUX
    {
        S_1.status1flags._CFG=TH8;                                              
        LogIntLength = minScanEightTH;
        hms(minScanEightTH, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                     //32 channel TH MUX
    {
        S_1.status1flags._CFG=TH32;                                             
        LogIntLength = minScanThirtytwoTH;
        hms(minScanThirtytwoTH, 0);
    }

    //initialize log intervals:
    //Log interval #1:
    storeLogInterval(1,100);                                                    //100 iterations
    write_Int_FRAM(LogItRemain1address,100);                                    //100 left to go
    write_Int_FRAM(LogItRemain1MASTERaddress,100);                              
    
    //Log interval #2:
    //LogIntLength=40;                                                            //40 second scan interval REM REV 1.10
    LogIntLength+=10;                                                           //second scan interval  REV 1.10
    storeLogInterval(2,90);                                                     //90 iterations
    write_Int_FRAM(LogItRemain2address,90);                                     //90 left to go
    write_Int_FRAM(LogItRemain2MASTERaddress,90);                           

    //Log interval #3:
    //LogIntLength=50;                                                            //50 second scan interval REM REV 1.10
    LogIntLength+=10;                                                           //third scan interval  REV 1.10
    storeLogInterval(3,80);                                                     //80 iterations
    write_Int_FRAM(LogItRemain3address,80);                                     //80 left to go
    write_Int_FRAM(LogItRemain3MASTERaddress,80);                           

    //Log interval #4:
    //LogIntLength=60;                                                            //60 second scan interval REM REV 1.10
    LogIntLength+=10;                                                           //fourth scan interval  REV 1.10
    storeLogInterval(4,70);                                                     //70 iterations
    write_Int_FRAM(LogItRemain4address,70);                                     //70 left to go
    write_Int_FRAM(LogItRemain4MASTERaddress,70);                           

    //Log interval #5:
    //LogIntLength=90;                                                            //90 second scan interval REM REV 1.10
    LogIntLength+=10;                                                           //fifth scan interval  REV 1.10
    storeLogInterval(5,60);                                                     //60 iterations
    write_Int_FRAM(LogItRemain5address,60);                                     //60 left to go
    write_Int_FRAM(LogItRemain5MASTERaddress,60);                           
 
    //Log interval #6:
    //LogIntLength=120;                                                           //120 second scan interval    REM REV 1.10
    LogIntLength+=10;                                                           //sixth scan interval  REV 1.10
    storeLogInterval(6,0);                                                      //0 iterations
    write_Int_FRAM(LogItRemain6address,0);                                      //0 left to go
    write_Int_FRAM(LogItRemain6MASTERaddress,0);                            

    if(MUX4_ENABLE.mflags.mux16_4==Single)                                      //Single Channel selected    
    {
        //Log interval #1:
        LogIntLength = 3;                                                       //3 second scan interval
        storeLogInterval(1, 3);                                                 //3 iterations
        write_Int_FRAM(SingleLogItRemain1address,3);                            //3 left to go 
        write_Int_FRAM(SingleLogItRemain1MASTERaddress,3);                      

        //Log interval #2:
        LogIntLength = 6;                                                       //6 second scan interval
        storeLogInterval(2, 9);                                                 //9 iterations
        write_Int_FRAM(SingleLogItRemain2address,9);                            //9 left to go`
        write_Int_FRAM(SingleLogItRemain2MASTERaddress,9);                      

        //Log interval #3:
        LogIntLength = 10;                                                      //10 second scan interval
        storeLogInterval(3, 54);                                                //54 iterations
        write_Int_FRAM(SingleLogItRemain3address,54);                           //54 left to go 
        write_Int_FRAM(SingleLogItRemain3MASTERaddress,54);                     

        //Log interval #4:
        LogIntLength = 30;                                                      //30 second scan interval
        storeLogInterval(4, 180);                                               //180 iterations
        write_Int_FRAM(SingleLogItRemain4address,180);                          //180 left to go  
        write_Int_FRAM(SingleLogItRemain4MASTERaddress,180);                    

        //Log interval #5:
        LogIntLength = 240;                                                     //240 second scan interval
        storeLogInterval(5, 225);                                               //225 iterations
        write_Int_FRAM(SingleLogItRemain5address,225);                          //225 left to go  
        write_Int_FRAM(SingleLogItRemain5MASTERaddress,225);                    

        //Log interval #6:
        LogIntLength = 3600;                                                    //3600 second scan interval
        storeLogInterval(6, 0);                                                 //0 iterations
        write_Int_FRAM(SingleLogItRemain6address,0);                            //0 left to go 
        write_Int_FRAM(SingleLogItRemain6MASTERaddress,0);                      
    }


    //initialize wrap format
    DISPLAY_CONTROL.flags.WrapMemory = 1;                                       //set memory to wrap
    S_1.status1flags._Wrap=1;                                                    

    //initialize reading synchronization
    DISPLAY_CONTROL.flags.Synch = 1;                                            //readings synchronized to top of hour
    S_1.status1flags._Sync=1;                                                    

    //save the flags
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM 

    //initialize monitor mode
    LC2CONTROL.flags.Monitor = 1;                                               //Turn on the monitor

    //initialize log intervals
    LC2CONTROL.flags.LogInterval = 0;                                           //Turn off log intervals
    S_1.status1flags._Logint=0;                                                   

    //initialize logging
    LC2CONTROL.flags.Logging = 0;                                               //Logging stopped
    S_1.status1flags._Logging=0;                                                 

    //initialize time format
    LC2CONTROL.flags.TimeFormat = 0;                                            //hhmm

    //initialize date format
    LC2CONTROL.flags.DateFormat = 0;                                            //julian
    
    //clear the stop time flag                                                  
    LC2CONTROL.flags.LoggingStopTime=0;
    
    //Lock the MODBUS protected registers                                       
    LC2CONTROL.flags.Unlock=0;                                  

    //save the flags
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //store flags in FRAM  
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                          
    write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);                          
    
}


void loadSV(void)                                                               //REV 1.11
{
    int value=0;
    
    value=atoi(RevMAJOR);                                                       //get the major revision
    value<<=8;                                                                  //Shift 8 bits to the left
    value+=atoi(RevMINOR);                                                      //get the minor revision
    write_Int_FRAM(FirmwareRevision,value);                                     //store it in FRAM
}

int MODBUScheckScanInterval(unsigned int x)                                     //Returns 0 if scan interval is ok  
{
    MUX4_ENABLE.mux=read_Int_FRAM(MUX4_ENABLEflagsaddress);  

    if (MUX4_ENABLE.mflags.mux16_4 == Single) 
    {
        if (x < minScanSingleVW)
            return minScanSingleVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) 
    {
        if (x < minScanFourVW)
            return minScanFourVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) 
    {
        if (x < minScanEightVW)
            return minScanEightVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) 
    {
        if (x < minScanSixteenVW)
            return minScanSixteenVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) 
    {
        if (x < minScanThirtytwoVW)
            return minScanThirtytwoVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) 
    {
        if (x < minScanEightTH)
            return minScanEightTH;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) 
    {
        if (x < minScanThirtytwoTH)
            return minScanThirtytwoTH;
        return 0;
    }    
}


void MODBUScomm(void)                                                           
{
    unsigned char arraysize=0;
    unsigned long memaddressStart=0;  
    unsigned long pageadd=0;                                                    
    unsigned int a=0;                                                           
    unsigned long b=0;                                                          
    unsigned int modbusaddressvalue;
    unsigned int testvalue=0;                                                   
    unsigned int i=0;                                                           
    unsigned int ECHO=0;
    unsigned int testHours=0;                                                   
    unsigned int testMinutes=0;                                                 
    csum       csumdata;                                                        //csumdata[1] is MSB, csumdata[0] is LSB
    csum       value;                                                           //value[1] is MSB, value[0] is LSB]
    csum       registers;                                                       //registers[1] is MSB, registers [0] is LSB
    xFRAMul    ulregister;                                                      //32 bit register for unsigned ints 
    xFRAMflt   fltregister;                                                     //32 bit register for floats     
    s1flags   tempStatusValue;                                                  //temporary register for comparison 
    s1flags   tempValueValue;                                                   //convert value into bitfield register   
    s2flags   tempStatus2Value;                                                 //temporary register for comparison 
    s2flags   tempValue2Value;                                                  //convert value into bitfield register 
    
    
    tempStatusValue.status1=0;                                                  //initialize        
    tempStatus2Value.status2=0;                                                 //initialize    
    
    tempStatusValue.status1=read_Int_FRAM(MODBUS_STATUS1address);               
    
    for(a=0;a<125;a++)                                                        
    {
        MODBUS_TXbuf[a]=0;                                                      //Clear the MODBUS_TXbuf[]                 
    }                                                                              
    
    //Determine if crc value is correct:
    arraysize=MODBUS_RX();                                                      //Receive the incoming MODBUS packet
    if(arraysize==0)                                                            
        return;                                                                 
    
    csumdata.c=CRC(TEST,arraysize);                                             //compute the crc checksum
    
    if((MODBUS_RXbuf[(arraysize-1)] != csumdata.z[1]) | (MODBUS_RXbuf[(arraysize-2)] != csumdata.z[0]))  //received crc does not agree with computed crc
    {                                                                         
        IFS3bits.T9IF=1;                                                        //exit if crc error  
        return;                                                               
    }
    
    
    //crc value is correct so test if address is correct:
    modbusaddressvalue=read_Int_FRAM(MODBUSaddress);
    if(MODBUS_RXbuf[ADDRESS]!=modbusaddressvalue)                                  
    {
        IFS3bits.T9IF=1;                                                        //exit if not an address match
        return;
    }      
    
    
    //address is correct so get the starting memory address that the function will work with:
    if(MODBUS_RXbuf[COMMAND]==READ_HOLDING)
    {
        //get the memory page                                                   
        memaddressStart|=MODBUS_RXbuf[REGISTER_MSB];                            //Add the MSB
        memaddressStart<<=8;                                                    //Shift 8 bits to the left
        memaddressStart|=MODBUS_RXbuf[REGISTER_LSB];                            //Add the LSB
        memaddressStart*=2;                                                     //Multiply incoming register address x2 to get absolute memory address  
        pageadd=read_Int_FRAM(MODBUS_PAGEaddress);                              
        pageadd<<=16;                                                           
        memaddressStart+=pageadd;                                               
    }
    
    if(MODBUS_RXbuf[COMMAND]==WRITE_HOLDING | MODBUS_RXbuf[COMMAND]==WRITE_MULTIPLE)    
    {
        memaddressStart|=MODBUS_RXbuf[REGISTER_MSB];                            //Add the MSB
        memaddressStart<<=8;                                                    //Shift 8 bits to the left
        memaddressStart|=MODBUS_RXbuf[REGISTER_LSB];                            //Add the LSB
        memaddressStart*=2;                                                     //Multiply incoming register address x2 to get absolute memory address      
        pageadd=7;                                                              
        pageadd<<=16;                                                           
        memaddressStart+=pageadd;                                                
    }
    
    //get & perform the function:
    switch(MODBUS_RXbuf[COMMAND])
    {
        case READ_HOLDING:                                                      
            //get the number of registers to be read:
            registers.z[1]=MODBUS_RXbuf[4];
            registers.z[0]=MODBUS_RXbuf[5];

            for(a=0;a<((registers.c)*2);a+=2)
            {
                value.c=read_Int_FRAM((memaddressStart)+a);
                MODBUS_TXbuf[a+3]=value.z[1];                                   //load the MSB of the value into MODBUS_TXbuf[] odd registers starting at 0x03
                MODBUS_TXbuf[a+4]=value.z[0];                                   //load the LSB of the value into MODBUX_TXbuf[] even registers starting at 0x04
            }

            MODBUS_TXbuf[BYTE_COUNT]=2*(registers.c);                           
            LC2CONTROL.flags.Unlock=0;                                          //lock the password protected registers 
            break;

        case WRITE_HOLDING:                                                     
            //16 Bit:
            //if((memaddressStart>=baudrateaddress) && (memaddressStart<=RESERVED2))   //MODBUS 16 bit Registers    REM REV 1.10
            if(((memaddressStart>=baudrateaddress) && (memaddressStart<=RESERVED2))   //MODBUS 16 bit Registers     REV 1.10
                    |
                ((memaddressStart>=THMUX_ENABLE1_16flagsaddress) && (memaddressStart<=LastMemoryaddress)))    
            {
                MODBUS_TXbuf[REGISTER_MSB]=MODBUS_RXbuf[REGISTER_MSB];          //Load the TXbuf[] with Register address MSB
                MODBUS_TXbuf[REGISTER_LSB]=MODBUS_RXbuf[REGISTER_LSB];          //Load the TXbuf[] with Register address LSB
                value.z[1]=MODBUS_RXbuf[WRITE_DATA_MSB];                        //get the write data MSB
                MODBUS_TXbuf[WRITE_DATA_MSB]=MODBUS_RXbuf[WRITE_DATA_MSB];      //Load the TXbuf[] with data MSB
                value.z[0]=MODBUS_RXbuf[WRITE_DATA_LSB];                        //get the write data LSB
                MODBUS_TXbuf[WRITE_DATA_LSB]=MODBUS_RXbuf[WRITE_DATA_LSB];      //Load the TXbuf[] with data LSB
                
                if(memaddressStart>=baudrateaddress && memaddressStart<=ProtectedRESERVED2 && LC2CONTROL.flags.Unlock)  //PASSWORD PROTECTED 
                {
                    write_Int_FRAM(memaddressStart, value.c);                   //Write to password protected FRAM Registers (single)   
                    LC2CONTROL.flags.Unlock=0;                                  //lock the password protected registers 
                    break;
                }
                
            
                //if((memaddressStart>=passwordaddressHIGH) && (memaddressStart<=RESERVED2))   //MODBUS R/W Registers   REM REV 1.10
                if(((memaddressStart>=passwordaddressHIGH) && (memaddressStart<=RESERVED2))   //MODBUS R/W Registers REV 1.10                
                        |
                    ((memaddressStart>=THMUX_ENABLE1_16flagsaddress) && (memaddressStart<=LastMemoryaddress)))       
                {
                    if(memaddressStart==ScanSecondsaddress)
                    {
                        testHours=read_Int_FRAM(ScanHoursaddress);
                        testMinutes=read_Int_FRAM(ScanMinutesaddress);
                        testHours=(testHours*3600)+(testMinutes*60)+value.c;
                        if(MODBUScheckScanInterval(testHours))                  //scan interval too short
                            return;
                    }
                    write_Int_FRAM(memaddressStart, value.c);                   //Write to FRAM Registers (single)       
                }
                else
                {
                    return;                
                }
            }
            break;
            
            
        case WRITE_MULTIPLE:                                                    
            
            if(memaddressStart==SerialNumberHIGH && LC2CONTROL.flags.Unlock)    //Password protected Serial Number 
            {
                registers.z[1]=MODBUS_RXbuf[7];                                 //Get the Serial Number MSW
                registers.z[0]=MODBUS_RXbuf[8];
                write_Int_FRAM(SerialNumberHIGH,registers.c);                   //Write to FRAM
                registers.z[1]=MODBUS_RXbuf[9];                                 //Get the Serial Number LSW
                registers.z[0]=MODBUS_RXbuf[10];
                write_Int_FRAM(SerialNumberLOW,registers.c);                    //Write to FRAM    
                LC2CONTROL.flags.Unlock=0;                                      //clear the unlock flag 
                MODBUS_TXbuf[REGISTER_MSB]=MODBUS_RXbuf[REGISTER_MSB];          //Load the TXbuf[] with Register address MSB  
                MODBUS_TXbuf[REGISTER_LSB]=MODBUS_RXbuf[REGISTER_LSB];          //Load the TXbuf[] with Register address LSB  
                MODBUS_TXbuf[4]=MODBUS_RXbuf[4];                                //Load the TXbuf[] with # of Registers MSB    
                MODBUS_TXbuf[5]=MODBUS_RXbuf[5];                                //Load the TXbuf[] with # of Registers LSB                    
            }            
            else
            //if((memaddressStart>=passwordaddressHIGH) && (memaddressStart<=RESERVED2)) //Registers are 16 bit - not password protected    REM REV 1.10
            if(((memaddressStart>=passwordaddressHIGH) && (memaddressStart<=RESERVED2))   //MODBUS R/W Registers REV 1.10                
                    |
                ((memaddressStart>=THMUX_ENABLE1_16flagsaddress) && (memaddressStart<=LastMemoryaddress)))                       
            {
                if(memaddressStart==passwordaddressHIGH && MODBUS_RXbuf[7]==passwordbyte3 && MODBUS_RXbuf[8]==passwordbyte2
                        && MODBUS_RXbuf[9]==passwordbyte1 && MODBUS_RXbuf[10]==passwordbyte0)                        //Password being written?
                {                                                               //yes
                    LC2CONTROL.flags.Unlock=1;                                  //set the unlock flag
                    MODBUS_TXbuf[REGISTER_MSB]=MODBUS_RXbuf[REGISTER_MSB];      //Load the TXbuf[] with Register address MSB  
                    MODBUS_TXbuf[REGISTER_LSB]=MODBUS_RXbuf[REGISTER_LSB];      //Load the TXbuf[] with Register address LSB  
                    MODBUS_TXbuf[4]=MODBUS_RXbuf[4];                            //Load the TXbuf[] with # of Registers MSB    
                    MODBUS_TXbuf[5]=MODBUS_RXbuf[5];                            //Load the TXbuf[] with # of Registers LSB                    
                }
                else                                                            //everything else   
                {
                    //get the number of bytes to be written:
                    a=7;                                                        //load the start index for values to be written
                    b=memaddressStart;
                    
                    //2 Byte single register writes:
                    
                    for(a,b;a<MODBUS_RXbuf[6]+7;a++,b+=2)                       
                    {
                        value.z[1]=MODBUS_RXbuf[a];
                        a++;
                        value.z[0]=MODBUS_RXbuf[a];

                        if(b==ScanSecondsaddress)
                        {
                            testHours=read_Int_FRAM(ScanHoursaddress);
                            testMinutes=read_Int_FRAM(ScanMinutesaddress);
                            testHours=(testHours*3600)+(testMinutes*60)+value.c;
                            if(MODBUScheckScanInterval(testHours))              //if scan interval too short
                                return;                                         //return                  
                        }
                        write_Int_FRAM(b,value.c);                          
                    }
                    LC2CONTROL.flags.Unlock=0;                                  //clear the unlock flag 
                    MODBUS_TXbuf[REGISTER_MSB]=MODBUS_RXbuf[REGISTER_MSB];      //Load the TXbuf[] with Register address MSB    
                    MODBUS_TXbuf[REGISTER_LSB]=MODBUS_RXbuf[REGISTER_LSB];      //Load the TXbuf[] with Register address LSB    
                    MODBUS_TXbuf[4]=MODBUS_RXbuf[4];                            //Load the TXbuf[] with # of Registers MSB      
                    MODBUS_TXbuf[5]=MODBUS_RXbuf[5];                            //Load the TXbuf[] with # of Registers LSB                              
                }
                break;
            }
            else    
            if((memaddressStart>=LogIntLength1address) && (memaddressStart<=SingleLogIntLength6address))        //32 bit int:
            {
                a=7;
                b=memaddressStart;
                
                //4 byte unsigned long int register writes:
                
                for(a,b;a<MODBUS_RXbuf[6]+7;a+=4,b+=4)
                {
                    ulregister.y[3]=MODBUS_RXbuf[a];                            //MSB of 32 bit int
                    ulregister.y[2]=MODBUS_RXbuf[a+1];
                    ulregister.y[1]=MODBUS_RXbuf[a+2];
                    ulregister.y[0]=MODBUS_RXbuf[a+3];                          //LSB of 32 bit int
                    write_longFRAM(ulregister.f, b);                            //write the value to FRAM     
                }
                     
                LC2CONTROL.flags.Unlock=0;                                      //clear the unlock flag 
                MODBUS_TXbuf[REGISTER_MSB]=MODBUS_RXbuf[REGISTER_MSB];          //Load the TXbuf[] with Register address MSB    
                MODBUS_TXbuf[REGISTER_LSB]=MODBUS_RXbuf[REGISTER_LSB];          //Load the TXbuf[] with Register address LSB    
                MODBUS_TXbuf[4]=MODBUS_RXbuf[4];                                //Load the TXbuf[] with # of Registers MSB      
                MODBUS_TXbuf[5]=MODBUS_RXbuf[5];                                //Load the TXbuf[] with # of Registers LSB           
                break;                                                          
            }
            else
            if((memaddressStart>=CH1ZRaddress) && (memaddressStart<=CH32PolyCoCaddress))        //32 bit float:
            {
                a=7;
                b=memaddressStart;
                
                //4 byte float register writes:
                
                for(a,b;a<MODBUS_RXbuf[6]+7;a+=4,b+=4)
                {
                    fltregister.x[3]=MODBUS_RXbuf[a];                           //MSB of 32 bit float
                    fltregister.x[2]=MODBUS_RXbuf[a+1];
                    fltregister.x[1]=MODBUS_RXbuf[a+2];
                    fltregister.x[0]=MODBUS_RXbuf[a+3];                         //LSB of 32 bit float
                    write_Flt_FRAM(b,fltregister.g);                            //write the value to FRAM
                }
                     
                LC2CONTROL.flags.Unlock=0;                                      //clear the unlock flag 
                MODBUS_TXbuf[REGISTER_MSB]=MODBUS_RXbuf[REGISTER_MSB];          //Load the TXbuf[] with Register address MSB    
                MODBUS_TXbuf[REGISTER_LSB]=MODBUS_RXbuf[REGISTER_LSB];          //Load the TXbuf[] with Register address LSB    
                MODBUS_TXbuf[4]=MODBUS_RXbuf[4];                                //Load the TXbuf[] with # of Registers MSB      
                MODBUS_TXbuf[5]=MODBUS_RXbuf[5];                                //Load the TXbuf[] with # of Registers LSB           
                break;                                                          
            }
            else
            {
                return;                
            }            

            
            break;

        default:
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
        ECHO=0;                                                                 
    }
    else
    {
        csumdata.c=CRC(GENERATE,8);                                             
        MODBUS_TXbuf[6]= csumdata.z[0];                                         //attach crc LSB
        MODBUS_TXbuf[7]= csumdata.z[1];                                         //attach crc MSB
        ECHO=1;                                                                 
    }


    //Perform the requested function if Status register was written             
    if(memaddressStart==MODBUS_STATUS1address && MODBUS_RXbuf[COMMAND]==WRITE_HOLDING)        //write to STATUS1 Register          
    {
        if(tempStatusValue.status1 != value.c)                                  //if new value is different than what's present
        {
            tempValueValue.status1 = value.c;                                   //store received flags

            switch(tempValueValue.status1flags._CFG)                             
            {
                if (tempStatusValue.status1flags._CFG == tempValueValue.status1flags._CFG)    //no difference between received and stored value
                    break;  

                if(tempStatusValue.status1flags._Logging)                        //if logging    
                    break;                                                      //Don't allow CFG change    

                case 0:                                                         //MX4   
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

            for(i=3;i<16;i++)                                                   
            {

                switch(i)
                {

                    case 3:                                                     //unused
                        if (tempStatusValue.status1flags._Setrtc == tempValueValue.status1flags._Setrtc)    //no difference between received and stored value
                        {
                            tempValueValue.status1flags._Setrtc=0;              
                            tempStatusValue.status1flags._Setrtc=0;             
                            break;                        
                        }

                        if(tempValueValue.status1flags._Setrtc)                 //load the RTC current time registers from FRAM
                        {                                                       
                            WRITE_TIME();
                            tempValueValue.status1flags._Setrtc=0;              //clear this bit on exit
                            tempStatusValue.status1flags._Setrtc=0;             
                            S_1.status1flags._Setrtc=0;                         //clear the MODBUS status flag      
                            write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);      
                            break;
                        }                                                       

                    case 4:
                        if (tempStatusValue.status1flags._Logint == tempValueValue.status1flags._Logint)    //no difference between received and stored value
                            break;                        

                        if(tempValueValue.status1flags._Logint)                 //enable Log Intervals
                            ENLOGINT();
                        else
                            DISLOGINT();                                        //disable Log Intervals
                        break;

                    case 5:
                        if (tempStatusValue.status1flags._Wrap == tempValueValue.status1flags._Wrap)    //no difference between received and stored value
                            break;                        

                        if(tempValueValue.status1flags._Wrap)
                            wrap_one();                                         //Enable memory wrapping
                        else
                            wrap_zero();                                        //Disable memory wrapping    
                        break;

                    case 6:
                        if (tempStatusValue.status1flags._BT == tempValueValue.status1flags._BT)    //no difference between received and stored value
                            break;                        

                        if(tempValueValue.status1flags._BT)
                            BTONE();                                            //Turn Bluetooth module ON          
                        else
                            BTZERO();                                           //Turn Bluetooth module OFF         
                        break;

                    case 7:
                        if (tempStatusValue.status1flags._BT_Timer == tempValueValue.status1flags._BT_Timer)    //no difference between received and stored value
                            break;                        

                        if(tempValueValue.status1flags._BT_Timer)
                            BT_E();                                             //Enable Bluetooth Timer            
                        else
                            BT_D();                                             //Disable Bluetooth Timer           
                        break;

                    case 8:
                        if (tempStatusValue.status1flags._OP == tempValueValue.status1flags._OP)    //no difference between received and stored value
                            break;                        

                        if(tempValueValue.status1flags._OP)
                            enableOP();                                         //Activate Output Port              
                        else
                            disableOP();                                        //Deactivate Output Port            
                        break;

                    case 9:
                        if (tempStatusValue.status1flags._OP_Timer == tempValueValue.status1flags._OP_Timer)    //no difference between received and stored value
                            break;                        

                        if(tempValueValue.status1flags._OP_Timer)
                            O_E();                                              //Enable Output Port Timer          
                        else
                            O_D();                                              //Disable Output Port Timer         
                        break;

                    case 10:
                        if (tempStatusValue.status1flags._Sync == tempValueValue.status1flags._Sync)    //no difference between received and stored value
                            break;                        

                        if(tempValueValue.status1flags._Sync)
                            synch_one();                                        //Enable synchronized readings      
                        else
                            synch_zero();                                       //Disable synchronized readings     
                        break;

                    case 11:
                        if (tempStatusValue.status1flags._ST == tempValueValue.status1flags._ST)    //no difference between received and stored value
                            break;                        

                        if(tempValueValue.status1flags._ST)
                            MODBUS_EnableStartTime();                           
                        else
                            MODBUS_DisableStartTime();
                        break;

                    case 12:
                        if (tempStatusValue.status1flags._SP == tempValueValue.status1flags._SP)    //no difference between received and stored value
                            break;      

                        if(tempValueValue.status1flags._SP)
                            MODBUS_EnableStopTime();                            
                        else
                            MODBUS_DisableStopTime();
                        break;

                    case 13:                                                    
                        if (tempStatusValue.status1flags._Readrtc == tempValueValue.status1flags._Readrtc)    //no difference between received and stored value
                        {
                            tempStatusValue.status1flags._Readrtc=0;            
                            tempValueValue.status1flags._Readrtc=0;             
                            break;   
                        }

                        if(tempValueValue.status1flags._Readrtc)
                            READ_TIME();                                        //get the RTC current time registers & store in FRAM    

                        tempValueValue.status1flags._Readrtc=0;                 //clear this bit on exit       
                        S_1.status1flags._Setrtc=0;                             //clear the MODBUS status flag      
                        write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);          
                        break;

                    case 14:                                                    
                        if (tempStatusValue.status1flags._X == tempValueValue.status1flags._X)    //no difference between received and stored value
                        {
                            tempStatusValue.status1flags._X=0;                  
                            tempValueValue.status1flags._X=0;                    
                            break;      
                        }

                        X();                                                    //take 'X' Reading  
                        tempValueValue.status1flags._X=0;                       //clear this bit on exit
                        S_1.status1flags._X=0;                                  //clear the MODBUS status flag      
                        write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);          
                        break;   

                    case 15:    
                        if (tempStatusValue.status1flags._Logging == tempValueValue.status1flags._Logging)    //no difference between received and stored value
                            break;

                        if(tempValueValue.status1flags._Logging)
                        {
                            testvalue=START();                                  //Start Logging
                            if(!testvalue)                                      //min scan error    
                                return;
                        }
                        else
                        {    
                            testvalue=STOP();                                   //Stop Logging
                        }    
                        break;                        

                    default:
                        break;
                }
            }
        }
    }

    if(memaddressStart==MODBUS_STATUS2address && MODBUS_RXbuf[COMMAND]==WRITE_HOLDING)        //write to STATUS2 Register  
    {
        if(tempStatus2Value.status2 != value.c)                                 //if new value is different than what's present
        {
            tempValue2Value.status2 = value.c;                                  //store received flags

            for(i=0;i<16;i++)                                                   
            {

                switch(i)
                {

                    case 0:                                                     
                        if (tempStatus2Value.status2flags._R == tempValue2Value.status2flags._R)    //no difference between received and stored value
                        {
                            tempStatus2Value.status2flags._R=0;                 
                            tempValue2Value.status2flags._R=0;                  
                            break;                        
                        }

                        if(tempValue2Value.status2flags._R)                     //Reset memory pointers 
                            R();

                        tempValue2Value.status2flags._R=0;                      //clear this bit on exit
                        S_2.status2flags._R=0;                                  //clear the MODBUS status flag      
                        write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);          
                        break;

                    case 1:
                        if (tempStatus2Value.status2flags._RST == tempValue2Value.status2flags._RST)    //no difference between received and stored value
                        {
                            tempStatus2Value.status2flags._RST=0;               
                            tempValue2Value.status2flags._RST=0;                
                            break;                        
                        }

                        if(tempValue2Value.status2flags._RST)
                            RST();                                              //Reset uC  

                        tempValue2Value.status2flags._RST=0;                    //clear this bit on exit
                        S_2.status2flags._RST=0;                                //clear the MODBUS status flag      
                        write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);                                  
                        break;

                    case 2:
                        if (tempStatus2Value.status2flags._CMD == tempValue2Value.status2flags._CMD)    //no difference between received and stored value
                        {
                            tempStatus2Value.status2flags._CMD=0;               
                            tempValue2Value.status2flags._CMD=0;                
                            break;                        
                        }

                        if(tempValue2Value.status2flags._CMD)
                        {
                            tempValue2Value.status2flags._CMD=0;                //clear this bit on exit 
                            S_2.status2flags._CMD=0;                            //clear the MODBUS status flag      
                            write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);      
                            CMD_LINE();
                        }

                        break;

                    case 3:
                        if (tempStatus2Value.status2flags._LD == tempValue2Value.status2flags._LD)    //no difference between received and stored value
                        {
                            tempStatus2Value.status2flags._LD=0;                
                            tempValue2Value.status2flags._LD=0;                 
                            break;  
                        }

                        if(tempValue2Value.status2flags._LD)
                            loadDefaults();                                     //load default settings

                        tempValue2Value.status2flags._LD=0;                     //clear this bit on exit   
                        S_2.status2flags._LD=0;                                 //clear the MODBUS status flag      
                        write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);          
                        break;

                    case 4:                                                     
                        if (tempStatus2Value.status2flags._CCV == tempValue2Value.status2flags._CCV)    //no difference between received and stored value
                        {
                            tempStatus2Value.status2flags._CCV=0;               
                            tempValue2Value.status2flags._CCV=0;                
                            break;      
                        }

                        if(tempValue2Value.status2flags._CCV)                   //Read the coin cell voltage
                        {
                            lithBatreading = take_analog_reading(95);           //get the lithium coin cell voltage
                            write_Int_FRAM(BatteryReading,lithBatreading);       
                        }                            

                        tempValue2Value.status2flags._CCV=0;                    //clear this bit on exit   
                        S_2.status2flags._CCV=0;                                //clear the MODBUS status flag      
                        write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);                                  
                        break;

                    case 5:                                                     
                        if (tempStatus2Value.status2flags._BV3 == tempValue2Value.status2flags._BV3)    //no difference between received and stored value
                        {
                            tempStatus2Value.status2flags._BV3=0;               
                            tempValue2Value.status2flags._BV3=0;                
                            break;                        
                        }

                        if(tempValue2Value.status2flags._BV3)                   //Read the battery voltage
                        {
                            mainBatreading = take_analog_reading(97);           //get the 3V Battery Voltage
                            write_Int_FRAM(BatteryReading,mainBatreading);      
                        }                            

                        tempValue2Value.status2flags._BV3=0;                    //clear this bit on exit   
                        S_2.status2flags._BV3=0;                                //clear the MODBUS status flag      
                        write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);                                  
                        break;

                    case 6:                                                     
                        if (tempStatus2Value.status2flags._BV12 == tempValue2Value.status2flags._BV12)    //no difference between received and stored value
                        {
                            tempStatus2Value.status2flags._BV12=0;               
                            tempValue2Value.status2flags._BV12=0;                
                            break; 
                        }

                        if(tempValue2Value.status2flags._BV12)                  //Read the battery voltage
                        {
                            mainBatreading = take_analog_reading(87);           //get the 12V Battery Voltage
                            write_Int_FRAM(BatteryReading,mainBatreading);      
                        }                            

                        tempValue2Value.status2flags._BV12=0;                   //clear this bit on exit  
                        S_2.status2flags._BV12=0;                               //clear the MODBUS status flag      
                        write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);                                  
                        break;                        

                    case 7:                                                     
                        break;

                    case 8:                                                     //Trigger a gage serial number read 
                        if (tempStatus2Value.status2flags._SN == tempValue2Value.status2flags._SN)    //no difference between received and stored value
                        {
                            tempStatus2Value.status2flags._SN=0;               
                            tempValue2Value.status2flags._SN=0;                
                            break; 
                        }

                        if(tempValue2Value.status2flags._SN)                    //Read the gage serial number
                        {
                            ReadSN();                                           //get the gage serial number
                        }                            

                        tempValue2Value.status2flags._SN=0;                     //clear this bit on exit  
                        S_2.status2flags._SN=0;                                 //clear the MODBUS status flag      
                        write_Int_FRAM(MODBUS_STATUS2address,S_2.status2);  
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

    //Transmit the array:
    MODBUS_TX(ECHO);                                                            
    IFS3bits.T9IF=0;                                                            
    shutdownTimer(TimeOut);                                                     //start 15S shutdown timer	
    return;
}


void MODBUS_DisableStartTime(void)                                              
{
    LC2CONTROL.flags.Logging = 0;                                               //set the Logging flag    
    LC2CONTROL.flags.LoggingStartTime=0;                                        //clear the logging start time flag
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //save flag
    LC2CONTROL2.flags2.Waiting=0;                                               //set the waiting flag
    LC2CONTROL2.flags2.scheduled=0;                                             
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);
    S_1.status1flags._ST=0;                                                     //set the MODBUS Start Time Enabled flag
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);   
}

void MODBUS_EnableStartTime(void)                                               
{
    unsigned int    value=0;
    
    value=read_Int_FRAM(startHoursaddress);                                     //get the start hours from FRAM
    RTChours=value;
    setClock(RTCAlarm1HoursAddress, RTChours);                                  //load the Start Time Hours into the RTC

    value=read_Int_FRAM(startMinutesaddress);                                   //get the start minutes from FRAM
    RTCminutes=value;
    setClock(RTCAlarm1MinutesAddress, RTCminutes);                              //load the Start Time Minutes into the RTC
    
    value=read_Int_FRAM(startSecondsaddress);                                   //get the start seconds from FRAM
    RTCseconds=value;
    setClock(RTCAlarm1SecondsAddress, RTCseconds);                              //load the Start Time Seconds into the RTC   
    
    LC2CONTROL.flags.Logging = 1;                                               //set the Logging flag    
    LC2CONTROL.flags.LoggingStartTime=0;                                        //clear the logging start time flag
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //save flag
    LC2CONTROL2.flags2.Waiting=1;                                               //set the waiting flag
    LC2CONTROL2.flags2.scheduled=1;                                             
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);
    S_1.status1flags._ST=1;                                                     //set the MODBUS Start Time Enabled flag
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
    
    enableAlarm(Alarm1);                                      
    enableINT1();                                                               //enable INT1 (take a reading on interrupt) 
    INTCON2bits.INT1EP=1;                                                       //interrupt on negative edge
    INTCON1bits.NSTDIS = 0;                                                     //reset nesting of interrupts   	  
}


void MODBUS_DisableStopTime(void)                                               
{
    LC2CONTROL.flags.LoggingStopTime = 0;                                       //set the LoggingStopTime flag		
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //store flag in FRAM 
    S_1.status1flags._SP=0;                                                     //clear the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                              
    LC2CONTROL2.flags2.SetStopTime = 0;                                         //clear the SetStopTime flag    
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);
}

void MODBUS_EnableStopTime(void)                                                
{
    STOPTIME();
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

    while (!IFS3bits.T9IF)                                                      //enable COM for Timeout period
    {
        configMODBUStimers();                                                   //Setup TMR2 & TMR6 for packet timing TEST REM

        for (i = 0; i < 125; i++)                                               //clear the buffer
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
                
                MODBUS_RXbuf[i] = MODBUSdata;                                   //store the received char in the buffer(i)
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


void MODBUS_TX(unsigned int echo)                                               
{
    unsigned char i; 
    
    _RS485RX_EN=1;                                                              //Disable the RS485 Rx  
    RS485TX_EN=1;                                                               //Enable the RS485 Tx   
    U1STAbits.UTXEN=1;                                                          //Enable TX if disabled 
    
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
        for(i=0;i<8;i++)                                                        
        {
            putcUART1(MODBUS_TXbuf[i]);
            while(BusyUART1());
        }        
    }
    
    RS485TX_EN=0;                                                               //Disable the RS485 Tx   
    _RS485RX_EN=0;                                                              //Enable the RS485 Rx  
}




void MX1(void)                                                                  
{
    DISPLAY_CONTROL.flags.TH=0;                                                 //VW Configuration  
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flag in FRAM  
    MUX4_ENABLE.mflags.mux16_4=0;                                               //clear the mux flags   
    MUX4_ENABLE.mflags.mux16_4 = Single;                                        //Single Channel Datalogger selected
    S_1.status1flags._CFG=Single;                                               //set the MODBUS status flags    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);        
    write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);                    //store flag in FRAM  
    MUX_ENABLE1_16.e1flags.CH1=1;                           
    write_Int_FRAM(MUX_ENABLE1_16flagsaddress,MUX_ENABLE1_16.MUXen1_16); 
    LogIntLength = minScanSingleVW;
    hms(minScanSingleVW, 0);                                                    //load minimum scan interval for Single Channel   
    loadDefaults();                                                             //REV 1.10
}


void MX4(void)                                                                  
{
    MUX4_ENABLE.mflags.mux16_4 = VW4;                                           //4 channel VW/TH mux selected  
    write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);                    //store flag in FRAM  
    S_1.status1flags._CFG=VW4;                                                   //set the MODBUS status flags    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);      
    LogIntLength = minScanFourVW;
    hms(minScanFourVW, 0);                                                      //load minimum scan interval for 4 channel mode    
    loadDefaults();                                                             //REV 1.10
}


void MX8(void)                                                                  
{
    MUX4_ENABLE.mflags.mux16_4 = VW8;                                           //8 channel VW mux selected
    write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);                    //store flag in FRAM 
    S_1.status1flags._CFG=VW8;                                                   //set the MODBUS status flags    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                              
    LogIntLength = minScanEightVW;
    hms(minScanEightVW, 0);                                                     //load minimum scan interval for 8 channel VW mode  
    loadDefaults();                                                             //REV 1.10
}


void MX8T(void)                                                                 
{
    MUX4_ENABLE.mflags.mux16_4 = TH8;                                           //8 channel thermistor mux selected 
    write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);                    //store flag in FRAM 
    S_1.status1flags._CFG=TH8;                                                   //set the MODBUS status flags   
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                              
    LogIntLength = minScanEightTH;
    hms(minScanEightTH, 0);                                                     //load minimum scan interval for 8 channel thermistor mode  
    loadDefaults();                                                             //REV 1.10
}


void MX16(void)                                                                 
{
    MUX4_ENABLE.mflags.mux16_4 = VW16;                                          //16 channel VW/TH mux selected 
    write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);                    //store flag in FRAM 
    S_1.status1flags._CFG=VW16;                                                  //set the MODBUS status flags    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                              
    LogIntLength = minScanSixteenVW;
    hms(minScanSixteenVW, 0);                                                   //load minimum scan interval if 16 channel mode 
    loadDefaults();                                                             //REV 1.10
}

void MX32(void)                                                                 
{
    MUX4_ENABLE.mflags.mux16_4 = VW32;                                          //32 channel VW mux selected
    write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);                    //store flag in FRAM 
    S_1.status1flags._CFG=VW32;                                                  //set the MODBUS status flags    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                              
    LogIntLength = minScanThirtytwoVW;
    hms(minScanThirtytwoVW, 0);                                                 //load minimum scan interval for 32 channel VW mode
    loadDefaults();                                                             //REV 1.10    
}


void MX32T(void)                                                                
{
    MUX4_ENABLE.mflags.mux16_4 = TH32;                                          //32 channel thermistor mux selected
    write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);                    //store flag in FRAM
    S_1.status1flags._CFG=TH32;                                                  //set the MODBUS status flags    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                              
    LogIntLength = minScanThirtytwoTH;
    hms(minScanThirtytwoTH, 0);                                                 //load minimum scan interval for 32 channel thermistor mode 
    loadDefaults();                                                             //REV 1.10
}

void O_D(void)                                                                  
{
    PORT_CONTROL.flags.PortTimerEN = 0;                                         //clear Port Timer enable flag
    write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);              //store flag in FRAM`
    S_1.status1flags._OP_Timer=0;                                                //clear the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
    disableAlarm(Alarm2);                                                       //disable the Alarm2 interrupt	
}

void O_E(void)                                                                  
{
    PORT_CONTROL.flags.PortTimerEN = 1;                                         //set Port Timer enable flag
    PORT_CONTROL.flags.BluetoothTimerEN = 0;                                    //clear Bluetooth Timer enable flag 
    PORT_CONTROL.flags.CPTime = 0;                                              //clear the CPtime flag
    write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);              //store flag in FRAM  
    S_1.status1flags._OP_Timer=1;                                                //set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                        
    delay(4000);                                                                //delay required? 
    enableAlarm(Alarm2);                                                        //enable the Alarm2 interrupt    
}


unsigned int pack(unsigned char Hbyte, unsigned char Lbyte) {
    unsigned int packedword;

    packedword = Hbyte;                                                         //load Hbyte
    packedword *= 256;                                                          //left shift 8
    packedword += Lbyte;                                                        //add Lbyte

    return packedword;                                                          //return packed 16 bit word
}


//***************************************************************************
//								pluck()
//
//	Utilizing Timer3, generates swept frequency pluck based on start frequency,
//  stop frequency and # of excitation cycles
//
//	Parameters received: _Fstart: start frequency (unsigned int)
//						 _Fstop: stop frequency (unsigned int)
//						 _cycles: # of excitation cycles (unsigned int)
//											 
//	Returns: nothing
//
//	Called from: main()
//
//***************************************************************************

void pluck(unsigned int _Fstart, unsigned int _Fstop, unsigned int _cycles) {
    unsigned int i = 0;
    unsigned int maxloop = _cycles / 2;                                         //temporary register for for/next loop


    float Fstart_tcy_halfperiod = ((1.0 / _Fstart) / tcy) / 2.0;                //compute # of tcy for start frequency
    float Fstop_tcy_halfperiod = ((1.0 / _Fstop) / tcy) / 2.0;                  //compute # of tcy for stop frequency
    float step = (Fstart_tcy_halfperiod - Fstop_tcy_halfperiod) / (_cycles / 2);//compute step
    unsigned int timer_value = 65536 - Fstart_tcy_halfperiod;
    
    pluckON();                                                                  
    PMD1bits.T3MD=0;                                                            //enable TMR3   

    T2CONbits.T32 = 0;                                                          //make sure 32 bit mode is off
    OpenTimer3(T3_ON & T3_GATE_OFF & T3_PS_1_1 & T3_SOURCE_INT, 0xFFFF);

    for (i = 0; i < maxloop; ++i)                                               //generate swept frequency pluck
    {
        IFS0bits.T3IF = 0;                                                      //clear TMR3 interrupt flag
        VWflagsbits.pluckflag = 1;                                              //set computation flag
        WriteTimer3(timer_value);                                               //setup TMR3
        pluckPOS();                                                             //set pluck high    
        while (!IFS0bits.T3IF)                                                  //wait till TMR3 overflows
        {
            if (VWflagsbits.pluckflag)                                          //compute timer value once
            {
                timer_value = 65536 - Fstart_tcy_halfperiod;
            }
            VWflagsbits.pluckflag = 0;                                          //clear computation flag & wait for TMR3IF
        }
        IFS0bits.T3IF = 0;                                                      //clear TMR3 interrupt flag		

        VWflagsbits.pluckflag = 1;                                              //set computation flag
        WriteTimer3(timer_value);                                               //setup TMR3
        pluckNEG();                                                             //set pluck low 
        while (!IFS0bits.T3IF)                                                  //wait till TMR3 overflows
        {
            if (VWflagsbits.pluckflag)                                          //compute new timer value once
            {
                Fstart_tcy_halfperiod = Fstart_tcy_halfperiod - step;
                timer_value = 65536 - Fstart_tcy_halfperiod;
            }
            VWflagsbits.pluckflag = 0;                                          //clear computation flag and wait for TMR3IF
        }

    }                                                                           //end of for loop

    IFS0bits.T3IF = 0;                                                          //clear TMR3 interrupt flag
    CloseTimer3();                                                              //close the TMR3
    TMR3 = 0;                                                                   //clear TMR3
    pluckOFF();                                                                 //disable the pluck driver  
    PMD1bits.T3MD=1;                                                            //disable TMR3   
}

void pluckOFF(void)                                                             
{
    IN1=0;
    IN2=0;
    EXC_EN=0;                                                                   //Disable H-Bridge
    V9_EXC=0;                                                                   //9V TEST 
    _EXC_EN=1;                                                                  //Turn off Excitation supply
}

void pluckON(void)                                                              
{
    IN1=0;                                                                      //Make sure H-Bridge outputs are off
    IN2=0;
    _EXC_EN=0;                                                                  //Turn on Excitation supply
    delay(4000);                                                                //delay for stabilization 
    EXC_EN=1;                                                                   //Enable H-Bridge   
    delay(4000);                                                                //delay for stabilization
}

void pluckPOS(void)                                                             
{
    IN1=1;                                                                      //DRV8839 OUT1 = 1
    IN2=0;                                                                      //DRV8839 OUT2 = 0
}


void pluckNEG(void)                                                             
{
    IN1=0;                                                                      //DRV8839 OUT1 = 0
    IN2=1;                                                                      //DRV8839 OUT2 = 1
}


void processDS3231Temperature(int x)
{
    DS3231Integer=x>>8;                                                         //Store MSB in DS3231Integer
    x&=0x00C0;                                                                  //mask off all but the fraction bits
    DS3231Fraction=x>>6;                                                        //Store the DS3231 fraction in DS3231Fraction
}

void processReading(float VWreading, int channel) {
    float VWreading_1000;                                                       //VWreading/1000

    switch (channel) 
    {
        case 1:

            if (!MUX_CONVERSION1_16.c1flags.CH1) {
                LC2CONTROL.flags.Conversion = 0;                                //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1;                                //Polynomial Conversion
            }
            break;


        case 2:

            if (!MUX_CONVERSION1_16.c1flags.CH2) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;


        case 3:

            if (!MUX_CONVERSION1_16.c1flags.CH3) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;


        case 4:

            if (!MUX_CONVERSION1_16.c1flags.CH4) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;


        case 5:

            if (!MUX_CONVERSION1_16.c1flags.CH5) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 6:

            if (!MUX_CONVERSION1_16.c1flags.CH6) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 7:

            if (!MUX_CONVERSION1_16.c1flags.CH7) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 8:

            if (!MUX_CONVERSION1_16.c1flags.CH8) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 9:

            if (!MUX_CONVERSION1_16.c1flags.CH9) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 10:

            if (!MUX_CONVERSION1_16.c1flags.CH10) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 11:

            if (!MUX_CONVERSION1_16.c1flags.CH11) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 12:

            if (!MUX_CONVERSION1_16.c1flags.CH12) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 13:

            if (!MUX_CONVERSION1_16.c1flags.CH13) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 14:

            if (!MUX_CONVERSION1_16.c1flags.CH14) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 15:

            if (!MUX_CONVERSION1_16.c1flags.CH15) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 16:

            if (!MUX_CONVERSION1_16.c1flags.CH16) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 17:

            if (!MUX_CONVERSION17_32.c2flags.CH17) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;


        case 18:

            if (!MUX_CONVERSION17_32.c2flags.CH18) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;


        case 19:

            if (!MUX_CONVERSION17_32.c2flags.CH19) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;


        case 20:

            if (!MUX_CONVERSION17_32.c2flags.CH20) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;


        case 21:

            if (!MUX_CONVERSION17_32.c2flags.CH21) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 22:

            if (!MUX_CONVERSION17_32.c2flags.CH22) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 23:

            if (!MUX_CONVERSION17_32.c2flags.CH23) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 24:

            if (!MUX_CONVERSION17_32.c2flags.CH24) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 25:

            if (!MUX_CONVERSION17_32.c2flags.CH25) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 26:

            if (!MUX_CONVERSION17_32.c2flags.CH26) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 27:

            if (!MUX_CONVERSION17_32.c2flags.CH27) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 28:

            if (!MUX_CONVERSION17_32.c2flags.CH28) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 29:

            if (!MUX_CONVERSION17_32.c2flags.CH29) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 30:

            if (!MUX_CONVERSION17_32.c2flags.CH30) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 31:

            if (!MUX_CONVERSION17_32.c2flags.CH31) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;

        case 32:

            if (!MUX_CONVERSION17_32.c2flags.CH32) {
                LC2CONTROL.flags.Conversion = 0; 
            } else {
                LC2CONTROL.flags.Conversion = 1; 
            }
            break;
    }

    if (LC2CONTROL.flags.Conversion)                                            //polynomial conversion
    {
        VWreading_1000 = VWreading * 0.001;                                     //multiply digits by .001 for polynomial conversions
        VWreadingProcessed = (VWreading_1000 * VWreading_1000 * polyCoA)+(VWreading_1000 * polyCoB) + polyCoC;
        return;
    }

    VWreadingProcessed = ((VWreading - zeroReading) * gageFactor) + gageOffset;   
    //VWreadingProcessed=VWreading;                                               
    return;
}

void prompt(void)                                                               //transmit <CR><LF>
{
    putcUART1(cr);
    while (BusyUART1());
    putcUART1(lf);
    while (BusyUART1());
    putcUART1(asterisk);
    while (BusyUART1());
}


unsigned char readClock(unsigned char address)                                  //Receive timekeeping data from the DS3231 via I2C1 interface
{
    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                            I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);

    unsigned int config2=0x44;                                                  //d64 value for I2CBRG at 400KHz    
    unsigned char clkdata=0;

    OpenI2C1(config1, config2);                                                 //open the I2C1 module
    IdleI2C1();                                                                 //make sure bus is idle
    StartI2C1();
    while(I2C1CONbits.SEN);                                                     //wait till start sequence is complete

    delay(8);                                                                   

    MasterWriteI2C1(0xD0);                                                      //DS3231 RTC Slave address write byte
    while(I2C1STATbits.TBF);                                                    //wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(8);                                                                   

    MasterWriteI2C1(address);                                                   //select the appropriate DS3231 register
    while(I2C1STATbits.TBF);                                                    //wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(8);                                                                   

    StopI2C1();

    delay(8);                                                                   

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    StartI2C1();
    while(I2C1CONbits.SEN);                                                     //wait till start sequence is complete

    delay(8);                                                                   

    MasterWriteI2C1(0xD1);                                                      //DS3231 RTC Slave address read byte
    while(I2C1STATbits.TBF);                                                    //wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    delay(8);                                                                   

    clkdata = MasterReadI2C1();                                                 //read the data from the DS3231
    while(I2C1STATbits.TBF);                                                    //wait till data is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    delay(8);                                                                   

    NotAckI2C1();                                                               //NACK the DS3231

    delay(8);                                                                   

    StopI2C1();
    while(I2C1CONbits.PEN);                                                     //wait till stop sequence is complete


    CloseI2C1();                                                                //close the I2C1 module
    
    return clkdata;                                                             //return the integer DS3231 data    
}


//***************************************************************************
//			readDS3231temperature()
//
//	Read the DS3231 temperature registors (0x11 & 0x12)
//
//	Parameters received: none
//	Returns: 12 bit value as a 16 bit int in the format:
//  S   d   d   d   d   d   d   d | d   d   x   x   x   x   x   x
// sign         integer(C)          fraction (0.25C)
//
//  example:
//  0001 1001 01000 0000    = +25.25C  
//
//	Called from: main()
//
//***************************************************************************
int readDS3231temperature(void)                                                 
{
    unsigned char data=0;
    int T=0;
    
    data=readClock(0x11);                                                       //Get the MSB
    T=data;                                                                     //load into T LSB
    T=T<<8;                                                                     //shift it into the MSB
    data=readClock(0x12);                                                       //Get the LSB
    T+=data;                                                                    //add it to T
    return T;                                                                   //return the final temperature value
}


void R(void)                                                                    //Reset memory pointers 
{
    FRAM_MEMORY.flags.memEmpty = 1;                                             //set the memory empty flag
    write_Int_FRAM(FRAM_MEMORYflagsaddress,FRAM_MEMORY.memory);                 //store flag in FRAM  
    resetMemory();                                                              //clear FRAM data memory and reset pointers
    PORT_CONTROL.flags.O0issued = 0;                                            //clear O0 issued flag
    PORT_CONTROL.flags.CPTime = 0;                                              //clear CPTime flag
    write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);              //store flag in FRAM  
    setClock(0x0F, 0);                                                          //Clear the RTC Alarm flags    
}

void READ_TIME(void)                                                            
{
    unsigned char clockdata=0;                                                  
    unsigned int RTCYEAR=0;                                                     
    unsigned int RTCMONTH=0;                                                    
    unsigned int RTCDAY=0;                                                      
    unsigned int RTCHOUR=0;                                                     
    unsigned int RTCMINUTE=0;                                                   
    unsigned int RTCSECOND=0;                                                   
    unsigned long decimalValue=0;                                               
    
    if(!LC2CONTROL2.flags2.Modbus)                          
    {
        crlf();
        putsUART1(Date);                                                        //Date:
        while (BusyUART1());
    }
    else
    {
        __delay32(uS300);                                                       //300uS delay for RTC   
    }    

    clockdata = readClock(RTCMonthsAddress);                                    //get the month from the RTC
    decimalValue=bcd2d(clockdata);                                              //convert BCD to decimal for MODBUS 
    RTCMONTH=decimalValue;    
    if(!LC2CONTROL2.flags2.Modbus)
    {
        displayClock(clockdata);                                                //display it
        putcUART1(slash);                                                       // '/'
        while (BusyUART1());
    }
    else
    {
        __delay32(uS300);                                                       //300uS delay for RTC   
    }

    clockdata = readClock(RTCDaysAddress);                                      //get the day from the RTC
    decimalValue=bcd2d(clockdata);                                              //convert BCD to decimal for MODBUS  
    RTCDAY=decimalValue;    
    
    if(!LC2CONTROL2.flags2.Modbus)
    {
        displayClock(clockdata);                                                //display it
        putcUART1(slash);                                                       // '/'
        while (BusyUART1());
    }
    else
    {
        __delay32(uS300);                                                       //300uS delay for RTC   
    }


    clockdata = readClock(RTCYearsAddress);                                     //get the year from the RTC
    decimalValue=bcd2d(clockdata);                                              //convert BCD to decimal for MODBUS     
    RTCYEAR=decimalValue;    

    if(!LC2CONTROL2.flags2.Modbus)
    {
        displayClock(clockdata);                                                //display it
        putcUART1(tab);
        while(BusyUART1());
    }
    else
    {
        __delay32(uS300);                                                       //300uS delay for RTC   
    }    

    if(!LC2CONTROL2.flags2.Modbus)                          
    {
        putsUART1(Time);                                                        //Time:
        while (BusyUART1());
    }
    else
    {
        __delay32(uS300);                                                       //300uS delay for RTC   
    }

    clockdata = readClock(RTCHoursAddress);                                     //get the hours from the RTC
    decimalValue=bcd2d(clockdata);                                              //convert BCD to decimal for MODBUS     
    RTCHOUR=decimalValue;    
 
    if(!LC2CONTROL2.flags2.Modbus)
    {
        displayClock(clockdata);                                                //display it
        putcUART1(colon);                                                       // ':'
        while (BusyUART1());
    }
    else
    {
        __delay32(uS300);                                                       //300uS delay for RTC   
    }    

    clockdata = readClock(RTCMinutesAddress);                                   //get the minutes from the RTC
    decimalValue=bcd2d(clockdata);                                              //convert BCD to decimal for MODBUS     
    RTCMINUTE=decimalValue;    
    
    if(!LC2CONTROL2.flags2.Modbus)
    {
        displayClock(clockdata);                                                //display it
        putcUART1(colon);                                                       // ':'
        while (BusyUART1());
    }
    else
    {
        __delay32(uS300);                                                       //300uS delay for RTC   
    }    

    clockdata = readClock(RTCSecondsAddress);                                   //get the seconds from the RTC
    decimalValue=bcd2d(clockdata);                                              //convert BCD to decimal for MODBUS     
    RTCSECOND=decimalValue;    
 
    if(!LC2CONTROL2.flags2.Modbus)
    {
        displayClock(clockdata);                                                //display it   
    }
    else
    {
        __delay32(uS300);                                                       //300uS delay for RTC   
    }    
    
    write_Int_FRAM(RTCYearsaddress,RTCYEAR);
    write_Int_FRAM(RTCMonthsaddress,RTCMONTH);
    write_Int_FRAM(RTCDaysaddress,RTCDAY);
    write_Int_FRAM(RTCHoursaddress,RTCHOUR);
    write_Int_FRAM(RTCMinutesaddress,RTCMINUTE);
    write_Int_FRAM(RTCSecondsaddress,RTCSECOND);  
    
}

void ReadSN(void)                                                               
{
    char SNBUF[8];                                                              
    unsigned long   SerialNumber;
    unsigned char   i;
    
    
    PORT_CONTROL.flags.temp=0;
    if(IEC1bits.INT1IE)
    {
        PORT_CONTROL.flags.temp=1;
        IEC1bits.INT1IE=0;
        IFS1bits.INT1IF=0;
    }
    
    if(!LC2CONTROL2.flags2.Modbus)                          
    {
        crlf();          
    }
    
    SerialNumber=0;
    if(getSerialNumber())                                                       //display the probe serial number
    {
        for(i=0;i<4;i++)                                                        //get the serial number from the buffer    
        {
            SerialNumber+=_SNbuf[i];
            if(i==3)
                break;
            SerialNumber<<=8;
        }
        
        
        
        if(!LC2CONTROL2.flags2.Modbus)                          
        {
            sprintf(SNBUF, "%ld", SerialNumber);                                //format the serial number
            putsUART1(SNBUF);                                                   //display it
            while (BusyUART1());            
        }
    }
    else                                                                        //display ERROR
    {
        if(!LC2CONTROL2.flags2.Modbus)                          
        {
            putsUART1(Error);                                                   //"ERROR"  
            while(BusyUART1());
        }
    }    
    
    if(PORT_CONTROL.flags.temp)
    {
        IEC1bits.INT1IE=1;
    }    
}



void RST(void)                                                                  
{
    LC2CONTROL.flags.Reset = 1;                                                 //Set the Reset flag
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //store flag in FRAM  
    asm("RESET");    
}

//***************************************************************************
//								read_vw()
//
//	capture and process VW frequency
//
//	Parameters received: none
//	Returns: floating-point digits
//
//	Called from: main()
//
//***************************************************************************
float read_vw(void)
{
	float frequency=0.0;
	float digits=0.0;

	frequency=getFrequency();
    
    F=vwf32toINT16(frequency);
    
	if(frequency==0.0)
		VWflagsbits.VWerror=1;                                                  //set reading error flag if timeout occurs

	IFS0bits.T1IF=0;                                                            //reset TMR1IF

	if(LC2CONTROL.flags.Conversion)                                             //convert to digits
	{
		digits=(frequency*frequency)*.000001;                                   //digits = F^2x10E-6 if polynomial conversion
	}
	else
	{
		digits=(frequency*frequency)*.001;                                      //digits = F^2x10E-3 if linear conversion
	}

	return digits;                                                              //return the digits	
}


void reloadLogTable(void)                                                       
{
   unsigned long    index;
   unsigned int     val;
   
   for(index=LogIt1MASTERaddress;index<MODBUS_STATUS1address;index+=2)
   {
       val=read_Int_FRAM(index);                                                //Read the master value    
       write_Int_FRAM(index-0x3e2,val);                                         //reload the log table with the master value
   }
}

void resetMemory(void) 
{
    unsigned int data;


    data = 0;                                                                   //reset memory pointers                                         
    write_Int_FRAM(MemoryStatusaddress,data);                                   

    data = 1;                                                                   
    write_Int_FRAM(OutputPositionaddress,data);    
    write_Int_FRAM(UserPositionaddress,data);  

    DISPLAY_CONTROL.flags.monitorWasEnabled = 0;                                //clear the BPD flags
    DISPLAY_CONTROL.flags.newPointer = 0;
    DISPLAY_CONTROL.flags.BPD = 0;
    DISPLAY_CONTROL.flags.Backup = 0;
    DISPLAY_CONTROL.flags.Scan = 0;
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM 
}


void restoreSettings(void) 
{
    
    LC2CONTROL.full=read_Int_FRAM(LC2CONTROLflagsaddress);                      //restore flags from FRAM
    LC2CONTROL2.full2=read_Int_FRAM(LC2CONTROL2flagsaddress);
    DISPLAY_CONTROL.display=read_Int_FRAM(DISPLAY_CONTROLflagsaddress);
    PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress);
    FRAM_MEMORY.memory=read_Int_FRAM(FRAM_MEMORYflagsaddress);
    MUX4_ENABLE.mux=read_Int_FRAM(MUX4_ENABLEflagsaddress);
    MUX_CONVERSION1_16.MUXconv1_16=read_Int_FRAM(MUX_CONV1_16flagsaddress);
    MUX_CONVERSION17_32.MUXconv17_32=read_Int_FRAM(MUX_CONV17_32flagsaddress);
    MUX_ENABLE1_16.MUXen1_16=read_Int_FRAM(MUX_ENABLE1_16flagsaddress);            
    MUX_ENABLE17_32.MUXen17_32=read_Int_FRAM(MUX_ENABLE17_32flagsaddress);          
    memoryStatus=read_Int_FRAM(MemoryStatusaddress);                            //get the memory status
    outputPosition=read_Int_FRAM(OutputPositionaddress);                        //get the memory pointer
    userPosition=read_Int_FRAM(UserPositionaddress);                            //get the user position
    TotalStopSeconds=read_longFRAM(TotalStopSecondsaddress);                    //get the stored stop time
    S_1.status1=read_Int_FRAM(MODBUS_STATUS1address);                           
}

unsigned long RTChms2s(unsigned char x)                                         //returns total seconds in RTC register
{                                                                               //x=0:return total in Alarm1 registers
    unsigned long total = 0;                                                    //x=1:return total in RTC registers
    unsigned char hours = 0;
    unsigned char minutes = 0;
    unsigned char seconds = 0;
    unsigned long decimalhours = 0;
    unsigned long decimalminutes = 0;
    unsigned long decimalseconds = 0;

    if (x)
        hours = readClock(RTCHoursAddress);
    else
        hours = readClock(RTCAlarm1HoursAddress);
    decimalhours = bcd2d(hours);                                                //convert RTC 2 digit BCD hours to decimal
    decimalhours *= 3600;                                                       //convert result to seconds

    if (x)
        minutes = readClock(RTCMinutesAddress);
    else
        minutes = readClock(RTCAlarm1MinutesAddress);
    decimalminutes = bcd2d(minutes);
    decimalminutes *= 60;

    if (x)
        seconds = readClock(RTCSecondsAddress);
    else
        seconds = readClock(RTCAlarm1SecondsAddress);
    decimalseconds = bcd2d(seconds);

    total = decimalhours + decimalminutes + decimalseconds;

    return total;
}


unsigned int RTCtoDecimal(unsigned char data)
{
    BCDone=0;                                                                   //initialize
    BCDten=0;
    BCDtwenty=0;
    BCDthirty=0;
    BCDforty=0;
    BCDfifty=0;

    if(data>=0x50) BCDfifty=1;                                                  //extract BCDfifty
    if(data>=0x40) BCDforty=1;                                                  //extract BCDforty
    if(data>=0x30) BCDthirty=1;                                                 //extract BCDthirty
    if(data>=0x20) BCDtwenty=1;                                                 //extract BCDtwenty
    if(data>=0x10) BCDten=1;                                                    //extract BCDten
    BCDten=BCDten+BCDtwenty+BCDthirty+BCDforty+BCDfifty;                        //sum them in BCDten

    BCDten*=10;                                                                 //multiply BCDten by ten
    BCDone=data&0x0F;                                                           //mask off upper nybble of RTCdata
	
    decimalRTC=BCDten+BCDone;                                                   //add tens and ones
    return decimalRTC;                                                          //return value
}


void seconds2hms(unsigned long s) {
    second = 0;
    hour = 0;
    minute = 0;

    while (1) {
        if (s < 60) {
            second = s;                                                         //extract seconds and return
            break;
        }

        if (s > 3599) {
            hour += 1;                                                          //increment hours
            s -= 3600;
        } else {
            minute += 1;                                                        //increment minutes
            s -= 60;
        }
    }
}

void setADCsleep()                                                              //Configure ADC inputs as digital I/O 
{
    //CONFIGURE PORTB:   
    AD1PCFGH=0xFFFF;                                                            
    TRISB=0x0000;                                                               //PORTB all outputs
    LATB=0x0000;                                                                //All low
    
    //Configure PORTC:
    AD1PCFGL=0xFFFF;
    AD2PCFGL=0xFFFF;
    TRISC=0x9000;                                                               //Configure PORTC   
    LATC=0x0000;
}


void setADCwake(void)
{
     //Configure ADC for Digital I/O & Analog inputs:                            
    AD2PCFGL=0xFCC2;                                                            //AN0,2,3,4,5,8,9 are analog  
    AD1PCFGL=0xFCC2;                                                            //all else digital  
    AD1PCFGH=0xFFFF;   
}

void setChannelConversion(int channel, int conversion) {
    switch (channel)                                                            //assign the proper conversion type per channel
    {
        case 1:
            MUX_CONVERSION1_16.c1flags.CH1 = 0;                                 //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH1 = 1;                             //polynomial conversion
            break;

        case 2:
            MUX_CONVERSION1_16.c1flags.CH2 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH2 = 1; 
            break;

        case 3:
            MUX_CONVERSION1_16.c1flags.CH3 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH3 = 1; 
            break;

        case 4:
            MUX_CONVERSION1_16.c1flags.CH4 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH4 = 1; 
            break;

        case 5:
            MUX_CONVERSION1_16.c1flags.CH5 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH5 = 1; 
            break;

        case 6:
            MUX_CONVERSION1_16.c1flags.CH6 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH6 = 1; 
            break;

        case 7:
            MUX_CONVERSION1_16.c1flags.CH7 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH7 = 1; 
            break;

        case 8:
            MUX_CONVERSION1_16.c1flags.CH8 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH8 = 1; 
            break;

        case 9:
            MUX_CONVERSION1_16.c1flags.CH9 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH9 = 1; 
            break;

        case 10:
            MUX_CONVERSION1_16.c1flags.CH10 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH10 = 1; 
            break;

        case 11:
            MUX_CONVERSION1_16.c1flags.CH11 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH11 = 1; 
            break;

        case 12:
            MUX_CONVERSION1_16.c1flags.CH12 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH12 = 1; 
            break;

        case 13:
            MUX_CONVERSION1_16.c1flags.CH13 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH13 = 1; 
            break;

        case 14:
            MUX_CONVERSION1_16.c1flags.CH14 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH14 = 1; 
            break;

        case 15:
            MUX_CONVERSION1_16.c1flags.CH15 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH15 = 1; 
            break;

        case 16:
            MUX_CONVERSION1_16.c1flags.CH16 = 0; 
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH16 = 1; 
            break;

        case 17:
            MUX_CONVERSION17_32.c2flags.CH17 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH17 = 1; 
            break;

        case 18:
            MUX_CONVERSION17_32.c2flags.CH18 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH18 = 1; 
            break;

        case 19:
            MUX_CONVERSION17_32.c2flags.CH19 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH19 = 1; 
            break;

        case 20:
            MUX_CONVERSION17_32.c2flags.CH20 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH20 = 1; 
            break;

        case 21:
            MUX_CONVERSION17_32.c2flags.CH21 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH21 = 1; 
            break;

        case 22:
            MUX_CONVERSION17_32.c2flags.CH22 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH22 = 1; 
            break;

        case 23:
            MUX_CONVERSION17_32.c2flags.CH23 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH23 = 1; 
            break;

        case 24:
            MUX_CONVERSION17_32.c2flags.CH24 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH24 = 1; 
            break;

        case 25:
            MUX_CONVERSION17_32.c2flags.CH25 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH25 = 1; 
            break;

        case 26:
            MUX_CONVERSION17_32.c2flags.CH26 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH26 = 1; 
            break;

        case 27:
            MUX_CONVERSION17_32.c2flags.CH27 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH27 = 1; 
            break;

        case 28:
            MUX_CONVERSION17_32.c2flags.CH28 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH28 = 1; 
            break;

        case 29:
            MUX_CONVERSION17_32.c2flags.CH29 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH29 = 1; 
            break;

        case 30:
            MUX_CONVERSION17_32.c2flags.CH30 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH30 = 1; 
            break;

        case 31:
            MUX_CONVERSION17_32.c2flags.CH31 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH31 = 1; 
            break;

        case 32:
            MUX_CONVERSION17_32.c2flags.CH32 = 0; 
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH32 = 1; 
            break;

        default:
            return;

    }
    write_Int_FRAM(MUX_CONV1_16flagsaddress,MUX_CONVERSION1_16.MUXconv1_16);    //store flag in FRAM 
    write_Int_FRAM(MUX_CONV17_32flagsaddress,MUX_CONVERSION17_32.MUXconv17_32); //store flag in FRAM  
}


void setChannelThermtype(int channel,char Thermtype)                            
{
	switch(channel)                                                             //assign the proper thermistor type per channel
	{
		case 1:
            write_Int_FRAM(CH1THaddress,Thermtype);                             //store Thermistor type in EEPROM
            break;                                                              

		case 2:
			write_Int_FRAM(CH2THaddress,Thermtype);                             
            break;                                                              

		case 3:
			write_Int_FRAM(CH3THaddress,Thermtype);                             
            break;                                                              

		case 4:
			write_Int_FRAM(CH4THaddress,Thermtype);                             
            break;                                                              

		case 5:
			write_Int_FRAM(CH5THaddress,Thermtype);	
            break;                                                              

		case 6:
			write_Int_FRAM(CH6THaddress,Thermtype);	
            break;                                                              

		case 7:
			write_Int_FRAM(CH7THaddress,Thermtype);	
            break;                                                              

		case 8:		
			write_Int_FRAM(CH8THaddress,Thermtype);	
            break;                                                              

		case 9:
			write_Int_FRAM(CH9THaddress,Thermtype);	
            break;                                                              

		case 10:
			write_Int_FRAM(CH10THaddress,Thermtype);	
            break;                                                              

		case 11:
			write_Int_FRAM(CH11THaddress,Thermtype);	
            break;                                                              

		case 12:
			write_Int_FRAM(CH12THaddress,Thermtype);	
            break;                                                              

		case 13:
			write_Int_FRAM(CH13THaddress,Thermtype);	
            break;                                                              

		case 14:
			write_Int_FRAM(CH14THaddress,Thermtype);	
            break;                                                              

		case 15:
			write_Int_FRAM(CH15THaddress,Thermtype);	
            break;                                                              

		case 16:
			write_Int_FRAM(CH16THaddress,Thermtype);	
            break;                                                              
            
        case 17:
			write_Int_FRAM(CH17THaddress,Thermtype);	
            break;                                                              

		case 18:
			write_Int_FRAM(CH18THaddress,Thermtype);	
            break;                                                              

		case 19:
			write_Int_FRAM(CH19THaddress,Thermtype);	
            break;                                                              

		case 20:
			write_Int_FRAM(CH20THaddress,Thermtype);	
            break;                                                               
            
        case 21:
			write_Int_FRAM(CH21THaddress,Thermtype);	
            break;                                                              

		case 22:
			write_Int_FRAM(CH22THaddress,Thermtype);	
            break;                                                              

		case 23:
			write_Int_FRAM(CH23THaddress,Thermtype);	
            break;                                                              

		case 24:
			write_Int_FRAM(CH24THaddress,Thermtype);	
            break;                                                              
            
        case 25:
			write_Int_FRAM(CH25THaddress,Thermtype);	
            break;                                                              

		case 26:
			write_Int_FRAM(CH26THaddress,Thermtype);	
            break;                                                              

		case 27:
			write_Int_FRAM(CH27THaddress,Thermtype);	
            break;                                                              

		case 28:
			write_Int_FRAM(CH28THaddress,Thermtype);	
            break;                                                                 
            
        case 29:
			write_Int_FRAM(CH29THaddress,Thermtype);	
            break;                                                              

		case 30:
			write_Int_FRAM(CH30THaddress,Thermtype);	
            break;                                                              

		case 31:
			write_Int_FRAM(CH31THaddress,Thermtype);	
            break;                                                              

		case 32:
			write_Int_FRAM(CH32THaddress,Thermtype);	
            break;                                                                 

		default:
            break;                                                              
	}
    if(Thermtype==0)                                                            
        disableTHChannel(channel);                                              
    else                                                                        
        enableTHChannel(channel);                                               
}


void setClock(unsigned char address,unsigned char data)                         //Transmit timekeeping data to the DS3231 via I2C1 interface
{
    unsigned int config1 = (I2C1_ON & I2C1_IDLE_CON & I2C1_CLK_HLD &
                            I2C1_IPMI_DIS & I2C1_7BIT_ADD &
                            I2C1_SLW_EN & I2C1_SM_DIS &
                            I2C1_GCALL_DIS & I2C1_STR_DIS &
                            I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS &
                            I2C1_STOP_DIS & I2C1_RESTART_DIS &
                            I2C1_START_DIS);

    unsigned int config3=0x44;                                                  //d64 value for I2CBRG at 400KHz    
 
    OpenI2C1(config1, config3);                                                 //open the I2C1 module
    IdleI2C1();                                                                 //make sure bus is idle
    StartI2C1();
    while(I2C1CONbits.SEN);                                                     //wait till start sequence is complete

    MasterWriteI2C1(0xD0);                                                      //DS3231 RTC Slave address write byte
    while(I2C1STATbits.TBF);                                                    //wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(8);                                                                   

    MasterWriteI2C1(address);                                                   //select the appropriate DS3231 register
    while(I2C1STATbits.TBF);                                                    //wait till address is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(8);                                                                   

    if(address!=RTCControlAddress && address!=RTCStatusAddress)                  
    {
        toBCD(data);                                                            //convert to BCD
        data=0;                                                                 //clear the data

        switch(BCDtens)                                                         //setup DS3231 register bits
        {
            case 9:                                                             
                data=0x90;
                break;  
            case 8:                                                             
                data=0x80;
                break;  
            case 7:                                                             
                data=0x70;
                break;  
            case 6:                                                             
                data=0x60;
                break;                  
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

        data+=BCDones;                                                          //add the lower nybble
	
        if((address==0x0A)|(address==0x0D))                                     //Set A1M4/A2M4 in RTC address 0A by OR'ing data with 10000000	
            data|=0x80;
    }

    MasterWriteI2C1(data);                                                      //write the data to the DS3231
    while(I2C1STATbits.TBF);                                                    //wait till data is transmitted and ACK'd
    while(I2C1STATbits.ACKSTAT);

    IdleI2C1();
    while(I2C1STATbits.ACKSTAT);

    delay(8);                                                                   
	
    StopI2C1();
    while(I2C1CONbits.PEN);                                                     //wait till stop sequence is complete

    CloseI2C1();                                                                //close the I2C1 module

}


void setup(void)                                                                
{
    //Indicate uC clock set for HSPLL:                                          
    LC2CONTROL2.flags2.uCclock=1;                                               
    
    //Configure ADC for Digital I/O & Analog inputs:
    AD2PCFGLbits.PCFG15=1;                                                      //AN15 cfg as digital pin   
    AD2PCFGLbits.PCFG14=1;                                                      //AN14 cfg as digital pin
    AD2PCFGLbits.PCFG13=1;                                                      //AN13 cfg as digital pin
    AD2PCFGLbits.PCFG12=1;                                                      //AN12 cfg as digital pin
    AD2PCFGLbits.PCFG11=1;                                                      //AN11 cfg as digital pin
    AD2PCFGLbits.PCFG10=1;                                                      //AN10 cfg as digital pin
    AD2PCFGLbits.PCFG9=0;                                                       //AN9 cfg as analog input   (V_AGC)
    AD2PCFGLbits.PCFG8=0;                                                       //AN8 cfg as analog input   (VW_LPF)
    AD2PCFGLbits.PCFG7=1;                                                       //AN7 cfg as digital pin
    AD2PCFGLbits.PCFG6=1;                                                       //AN6 cfg as digital pin
    AD2PCFGLbits.PCFG5=0;                                                       //AN5 cfg as analog input   (V_LITH)
    AD2PCFGLbits.PCFG4=0;                                                       //AN4 cfg as analog input   (V_TH_EXT)
    AD2PCFGLbits.PCFG3=0;                                                       //AN3 cfg as analog input   (12V_SENSE)
    AD2PCFGLbits.PCFG2=0;                                                       //AN2 cfg as analog input   (3V_SENSE)
    AD2PCFGLbits.PCFG1=1;                                                       //AN1 cfg as digital pin
    AD2PCFGLbits.PCFG0=0;                                                       //AN0 cfg as analog input   (VW)
    
    AD1PCFGLbits.PCFG15=1;                                                      //AN15 cfg as digital pin   
    AD1PCFGLbits.PCFG14=1;                                                      //AN14 cfg as digital pin
    AD1PCFGLbits.PCFG13=1;                                                      //AN13 cfg as digital pin
    AD1PCFGLbits.PCFG12=1;                                                      //AN12 cfg as digital pin
    AD1PCFGLbits.PCFG11=1;                                                      //AN11 cfg as digital pin
    AD1PCFGLbits.PCFG10=1;                                                      //AN10 cfg as digital pin
    AD1PCFGLbits.PCFG9=0;                                                       //AN9 cfg as analog input   (V_AGC)
    AD1PCFGLbits.PCFG8=0;                                                       //AN8 cfg as analog input   (VW_LPF)
    AD1PCFGLbits.PCFG7=1;                                                       //AN7 cfg as digital pin
    AD1PCFGLbits.PCFG6=1;                                                       //AN6 cfg as digital pin
    AD1PCFGLbits.PCFG5=0;                                                       //AN5 cfg as analog input   (V_LITH)
    AD1PCFGLbits.PCFG4=0;                                                       //AN4 cfg as analog input   (V_TH_EXT)
    AD1PCFGLbits.PCFG3=0;                                                       //AN3 cfg as analog input   (12V_SENSE)
    AD1PCFGLbits.PCFG2=0;                                                       //AN2 cfg as analog input   (3V_SENSE)
    AD1PCFGLbits.PCFG1=1;                                                       //AN1 cfg as digital pin
    AD1PCFGLbits.PCFG0=0;                                                       //AN0 cfg as analog input   (VW)    

    AD1PCFGHbits.PCFG16=1;                                                      //Set AN16-32 as digital pins    
    AD1PCFGHbits.PCFG17=1;                                                      
    AD1PCFGHbits.PCFG18=1;
    AD1PCFGHbits.PCFG19=1;
    AD1PCFGHbits.PCFG20=1;
    IFS1bits.INT1IF=0;                                                          //Clear the interrupt flag  
    AD1PCFGHbits.PCFG21=1;
    IFS1bits.INT2IF=0;                                                          //Clear the interrupt flag
    AD1PCFGHbits.PCFG22=1;
    AD1PCFGHbits.PCFG23=1;  
    AD1PCFGHbits.PCFG24=1;
    AD1PCFGHbits.PCFG25=1;
    AD1PCFGHbits.PCFG26=1;
    AD1PCFGHbits.PCFG27=1;  
    AD1PCFGHbits.PCFG28=1;
    AD1PCFGHbits.PCFG29=1;
    AD1PCFGHbits.PCFG30=1;
    AD1PCFGHbits.PCFG31=1;    
    IFS1bits.INT1IF=0;                                                            

    //Configure PORTA:
    TRISA=0;                                                                    //PORTA configured as outputs                                                   
    //Inputs:
    TRISAbits.TRISA13=1;                                                        //analog input
    TRISAbits.TRISA12=1;                                                        //analog input
    TRISAbits.TRISA10=1;                                                        //analog Vref
    //Outputs:
    LATAbits.LATA0=0;                                                           //Unused = 0
    LATAbits.LATA1=0;                                                           //Unused = 0
    LATAbits.LATA2=0;                                                           //SCL_VW = 0
    LATAbits.LATA3=0;                                                           //SDA_VW = 0
    LATAbits.LATA4=0;                                                           //Unused = 0
    LATAbits.LATA5=0;                                                           //Unused = 0
    LATAbits.LATA6=0;                                                           //Unused = 0
    LATAbits.LATA7=0;                                                           //Unused = 0
    LATAbits.LATA9=0;                                                           //Unused = 0
    LATAbits.LATA14=0;                                                          //Unused = 0
    LATAbits.LATA15-0;                                                          //Unused = 0
    
    //Configure PORTB:
    TRISB=0;
    //Inputs:
    TRISBbits.TRISB9=1;                                                         //analog input  V_AGC
    TRISBbits.TRISB8=1;                                                         //analog input  VW_LPF
    TRISBbits.TRISB5=1;                                                         //analog input  V_LITH
    TRISBbits.TRISB4=1;                                                         //analog input  V_TH_EXT
    TRISBbits.TRISB3=1;                                                         //analog input  12V_SENSE
    TRISBbits.TRISB2=1;                                                         //analog input  3V_SENSE
    TRISBbits.TRISB0=1;                                                         //analog input  VW
    //Outputs:
    LATBbits.LATB1=0;                                                           //Unused = 0
    LATBbits.LATB6=0;                                                           //Unused = 0
    LATBbits.LATB7=0;                                                           //Unused = 0
    LATBbits.LATB10=0;                                                          //Unused = 0
    LATBbits.LATB11=0;                                                          //Unused = 0
    LATBbits.LATB12=0;                                                          //Unused = 0
    LATBbits.LATB13=0;                                                          //Unused = 0
    LATBbits.LATB14=0;                                                          //Unused = 0
    LATBbits.LATB15=0;                                                          //Unused = 0
    
    //Configure PORTC:
    TRISC=0;
    //Inputs:
    TRISCbits.TRISC15=1;                                                        //Osc
    TRISCbits.TRISC12=1;                                                        //Osc
    //Outputs:
    LATCbits.LATC1=0;                                                           //VW100 (T2CK input) = 0
    LATCbits.LATC2=0;                                                           //_AMP_SHDN = 0
    LATCbits.LATC3=0;                                                           //IN1 = 0
    LATCbits.LATC4=0;                                                           //EXC_EN = 0

    //Configure PORTD:
    TRISD=0;
    //Inputs:
    TRISDbits.TRISD5=1;                                                         //input BT_CONNECT
    //Outputs:
    LATDbits.LATD0=0;                                                           //FLTCLK = 0
    LATDbits.LATD1=1;                                                           //_CLK_RST = 1
    LATDbits.LATD2=0;                                                           //+3.3V_X_CONTROL = 0
    LATDbits.LATD3=0;                                                           //SAMPLE_LITHIUM = 0
    LATDbits.LATD4=0;                                                           //_BT_RESET = 0
    LATDbits.LATD6=0;                                                           //BT_ENABLE = 0
    LATDbits.LATD7=0;                                                           //SLEEP = 0
    LATDbits.LATD8=0;                                                           //TEST2 = 0
    LATDbits.LATD9=0;                                                           //TEST1 = 0
    LATDbits.LATD10=0;                                                          //Unused = 0
    LATDbits.LATD11=0;                                                          //Unused = 0
    LATDbits.LATD12=0;                                                          //BT_FR = 0
    LATDbits.LATD13=0;                                                          //_BT_RESET = 0
    LATDbits.LATD14=0;                                                          //Unused = 0
    LATDbits.LATD15=0;                                                          //RS485TX_EN = 0
    
    //Configure PORTE:
    TRISE=0;
    //Outputs:
    LATEbits.LATE0=0;                                                           //Unused = 0
    LATEbits.LATE1=0;                                                           //Unused = 0
    LATEbits.LATE2=0;                                                           //Unused = 0
    LATEbits.LATE3=0;                                                           //Unused = 0
    LATEbits.LATE4=0;                                                           //Unused = 0
    LATEbits.LATE5=1;                                                           //_EXC_EN = 1
    LATEbits.LATE6=0;                                                           //9V_EXC = 0
    LATEbits.LATE7=0;                                                           //Unused = 0

    //Configure PORTF:
    TRISF=0;
    //Inputs:
    TRISFbits.TRISF2=1;                                                         //Rx input
    TRISFbits.TRISF7=1;                                                         //_232 input
    TRISFbits.TRISF8=1;                                                         //USB_PWR input
    //Outputs:
    LATFbits.LATF0=0;                                                           //CONTROL=0
    LATFbits.LATF1=1;                                                           //_READ = 1
    LATFbits.LATF3=1;                                                           //U1TX = 1
    LATFbits.LATF4=1;                                                           //_RS485RX_EN = 1
    LATFbits.LATF5=0;                                                           //_232SHDN = 0
    LATFbits.LATF6=1;                                                           //WP = 1
    LATFbits.LATF12=0;                                                          //MUX_CLOCK = 0
    LATFbits.LATF13=0;                                                          //MUX_RESET = 0
    
    //Configure PORTG:
    TRISG=0;
    //Outputs:
    LATGbits.LATG0=0;                                                           //Unused
    LATGbits.LATG1=0;                                                           //Unused
    LATGbits.LATG2=1;                                                           //I2C SCL
    LATGbits.LATG3=1;                                                           //I2C SDA
    LATGbits.LATG6=0;                                                           //IN2
    LATGbits.LATG7=0;                                                           //Unused    
    LATGbits.LATG8=0;                                                           //Unused
    LATGbits.LATG9=0;                                                           //Unused
    LATGbits.LATG12=0;                                                          //A
    LATGbits.LATG13=0;                                                          //B
    LATGbits.LATG14=0;                                                          //C
    LATGbits.LATG15=0;                                                          //D
    
}



void shutdown(void) 
{
    _3VX_off();                                                                 //make sure analog +3VX is off
    INTCON1bits.NSTDIS = 1;                                                     //disable interrupt nesting                         
    CORCONbits.IPL3 = 0;                                                        //make sure IPL3 is not set                             

    _232SHDN=0;                                                                 //shut down the RS-232 Port 
    sleepFRAM();                                                                
    setADCsleep();                                                                 
    TRISGbits.TRISG2=0;                                                         //Make sure RG2 is configured as an output  
    TRISGbits.TRISG3=0;                                                         //Make sure RG3 is configured as an output  
    LATGbits.LATG2=1;                                                           //Set RG2 high  
    LATGbits.LATG3=1;                                                           //Set RG3 high  

    //shutdown modules except UART1 & I2C1  (and TMR6 if BT enabled)                                           
    PMD1=0xFF5F;                                                                //1111 1111 0101 1111   
    PMD2=0xFFFF;                                                                //1111 1111 1111 1111
    PMD3=0xFFFF;                                                                //1111 1111 1111 1111
    U1MODEbits.WAKE=1;     

    IFS1bits.INT1IF=0;                                                          //Make sure INT1 flag is clear  
    if(LC2CONTROL2.flags2.Modbus)                                               
        IFS0bits.U1RXIF=0;                                                      //Clear the UART1 interrupt flag if set 
    LC2CONTROL.flags.Unlock=0;                                                  //Write protect MODBUS registers    
    Sleep();                                                                    //SLEEP & continue with next command after wakeup - no vector 
        
    SLEEP12V = 0;                                                               //set regulator into switchmode when wake from sleep
    _232SHDN=1;                                                                 //enable the RS-232 Port 
    setADCwake();
    PMD1=0x7F5F;                                                                //re-enable TMR5
    PMD3=0x3FFF;                                                                //re-enable TMR8 & TMR9

    if(LC2CONTROL2.flags2.Modbus)                                               //15mS delay if MODBUS comms    
    {
        __delay32(mS15);                                                        //15mS delay
        if(U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR) 
            handleCOMError();                                                   //if communications error
    }
}


void start32KHz(void)
{
    unsigned char temp;

    temp=readClock(RTCStatusAddress);                                           //read the value of the status register
    temp=(temp|0x08);                                                           //OR it with 0x08 to set the EN32kHz bit
    setClock(RTCStatusAddress,temp);                                            //load it to the RTC
}


void startLogging(void) {
    //WDTSWEnable;                                                                

    if (LC2CONTROL.flags.LogInterval) 
    {
        LogItRemain1=read_Int_FRAM(LogIt1address);                              //Set LogItRemains in Log Interval table
        LogItRemain2=read_Int_FRAM(LogIt2address);	
        LogItRemain3=read_Int_FRAM(LogIt3address);	
        LogItRemain4=read_Int_FRAM(LogIt4address);	
        LogItRemain5=read_Int_FRAM(LogIt5address);	
        LogItRemain6=read_Int_FRAM(LogIt6address);	

        write_Int_FRAM(LogItRemain1address,LogItRemain1);                       //store to FRAM    
        write_Int_FRAM(LogItRemain2address,LogItRemain2);  
        write_Int_FRAM(LogItRemain3address,LogItRemain3);  
        write_Int_FRAM(LogItRemain4address,LogItRemain4);  
        write_Int_FRAM(LogItRemain5address,LogItRemain5);  
        write_Int_FRAM(LogItRemain6address,LogItRemain6);  
    }

    if (!DISPLAY_CONTROL.flags.Shutup && !LC2CONTROL2.flags2.Modbus)            
    {
        putsUART1(Loggingstarted);                                              //Logging started
        while (BusyUART1());
        putcUART1(cr);
        while (BusyUART1());
        putcUART1(nul);
        while (BusyUART1());
        putcUART1(lf);
        while (BusyUART1());
    }

    if (USB_PWR | !_232 | BT_CONNECT)                                           
        T4CONbits.TON = 1;                                                      //Turn timer back on
    LC2CONTROL.flags.Logging = 1;                                               //Set Logging flag
    LC2CONTROL.flags.LoggingStartTime = 0;
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //store flags in FRAM 
    
    S_1.status1flags._Logging=1;                                                //Set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);

    DISPLAY_CONTROL.flags.monitorWasEnabled = 0;                                //clear the BPD flags
    DISPLAY_CONTROL.flags.newPointer = 0;
    DISPLAY_CONTROL.flags.BPD = 0;
    DISPLAY_CONTROL.flags.Backup = 0;
    DISPLAY_CONTROL.flags.Scan = 0;
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM`

    if(!LC2CONTROL2.flags2.Modbus)                                              
        crlf();

    LC2CONTROL2.flags2.Waiting = 0;                                             //Clear the Waiting flag
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);                  //store flag in FRAM`

    VWflagsbits.firstReading=1;                                                 
    take_One_Complete_Reading(STORE);                                           //take a reading    
    VWflagsbits.firstReading=0;                                                 

    enableAlarm(Alarm1);
    enableINT1();                                                               //enable INT1 (take a reading on interrupt)
    INTCON1bits.NSTDIS = 0;                                                     //Reset nesting of interrupts                     
    IFS3bits.T9IF=1;                                                            //set the T9 interrupt flag to force shutdown   
    //ClrWdt();                                                                 
    //WDTSWDisable;                                                             
}


unsigned int START(void)                                                        
{

    if (USB_PWR)
        LC2CONTROL.flags.USBpower = 1;                                          //set flag if powered by USB
    else
        LC2CONTROL.flags.USBpower = 0;

    IEC1bits.INT1IE = 0;                                                        //temporarily disable the INT2 interrupt	
    LC2CONTROL2.full2=read_Int_FRAM(LC2CONTROL2flagsaddress);                   //restore flags from FRAM     
    IEC1bits.INT1IE = 1;                                                        //re-enable INT2 interrupt	

 
    delay(400);  
    if(!LC2CONTROL2.flags2.Modbus)                                              //only if command line
        crlf();
    
    testScanInterval = 0;                                                       //set ScanInterval to 0
    ScanInterval = hms2s();                                                     //get the stored scan interval
    testScanInterval = checkScanInterval();                                     //test for minimum allowable Scan Interval

    if (testScanInterval)                                                       //if error
    {
        if(!LC2CONTROL2.flags2.Modbus)                                          //if command line
        {
            if (LC2CONTROL.flags.LogInterval)                                   //Log Intervals?
            {
                putsUART1(MinInterror);                                         //ERROR: Minumum Log Interval Length for this configuration is :
            } else 
            {
                putsUART1(Minscanerror);                                        //ERROR: Minumum Scan Interval for this configuration is :
            }
            while (BusyUART1());
            sprintf(trapBUF, "%d", testScanInterval);                           //minimum scan interval in seconds
            putsUART1(trapBUF);
            while (BusyUART1());
            putsUART1(Seconds);
            while (BusyUART1());
            return 0;                                                           //return error
        }
        else
        {
            return 0;                                                           //return error in MODBUS
        }
    }
    
    if (!LC2CONTROL.flags.Logging)                                              //is Logging flag clear?
    {
        if (DISPLAY_CONTROL.flags.Synch)
            VWflagsbits.synch = 1;
        LC2CONTROL2.flags2.FirstReading = 1;                                    //set the first reading flag
        LC2CONTROL2.flags2.Interrupt = 0;                                       //clear the INT2 interrupt flag
        IEC1bits.INT1IE = 0;                                                    //temporarily disable the INT2 interrupt
        write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);              //store flags in FRAM 
        IEC1bits.INT1IE = 1;                                                    //re-enable INT2 interrupt
        startLogging();
        return 1;                                                               //logging started
    }

    if(!LC2CONTROL2.flags2.Modbus)
    {
        putsUART1(Loggingalreadystarted); //Logging already started!
        while (BusyUART1());
        return 0;
    }
    else
    {
        return 0;
    }
}
   

void stop32KHz(void)
{
    unsigned char temp;

    temp=readClock(RTCStatusAddress);                                           //read the value of the status register
    temp=(temp&&0xFE);                                                          //AND it with 0xFE to clear the EN32kHz bit
    setClock(RTCStatusAddress,temp);                                            //load it to the RTC
}



void stopLogging(void) 
{
    LC2CONTROL2.full2=read_Int_FRAM(LC2CONTROL2flagsaddress);                   //restore flags from FRAM 
    
    PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress);    
    if (!PORT_CONTROL.flags.Alarm2Enabled)
        disableINT1();
    disableAlarm(Alarm1);
    LC2CONTROL.flags.Logging = 0;                                               //Clear Logging flag
    LC2CONTROL.flags.LoggingStartTime = 0;                                      //Clear Start Logging flag
    LC2CONTROL.flags.LoggingStopTime = 0;                                       //Clear Stop Logging flag
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //store flag in FRAM 
    
    S_1.status1flags._Logging=0;                                                //Clear the MODBUS status flag    
    S_1.status1flags._ST=0;                                                     
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);


    LC2CONTROL2.flags2.Waiting = 0;                                             //Clear the Waiting flag
    LC2CONTROL2.flags2.SetStopTime = 0;                                         //clear the set stop time flag
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);                  //store flag in FRAM  

    DISPLAY_CONTROL.flags.Synch = 1;                                            //set the Synch flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM
    
    
    if(LC2CONTROL2.flags2.scheduled)                                            
    {                                                                           
        disableINT1();                                                          
        disableAlarm(Alarm1);                                                   
        //************Reload the start logging times************************    
        RTChours=read_Int_FRAM(startHoursaddress);                              //Get the Start Time Hours from FRAM            
        RTCminutes=read_Int_FRAM(startMinutesaddress);                          //Get the Start Time Minutes from FRAM          
        RTCseconds=read_Int_FRAM(startSecondsaddress);                          //Get the Start Time Seconds from FRAM          
        setClock(RTCAlarm1HoursAddress,RTChours);                               //reload the Start Time hours into the RTC      
        setClock(RTCAlarm1MinutesAddress, RTCminutes);                          //reload the Start Time Minutes into the RTC    
        setClock(RTCAlarm1SecondsAddress, RTCseconds);                          //reload the Start Time Seconds into the RTC    
        LC2CONTROL.flags.Logging = 1;                                           //set the Logging flag	
        LC2CONTROL.flags.LoggingStartTime = 0;                                  //clear the Logging Start Time flag	
        LC2CONTROL.flags.LoggingStopTime=1;                                   
        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                 //store flags in FRAM     

        LC2CONTROL2.flags2.Waiting = 1;                                         //Set the Waiting flag	
        LC2CONTROL2.flags2.scheduled=1;                                         
        write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);              //store flags in FRAM   
                        
        enableAlarm(Alarm1);                                                        
        enableINT1();                                                           //enable INT1 (take a reading on interrupt) 
        INTCON1bits.NSTDIS = 0;                                                 //reset nesting of interrupts   	   
    }                                                                           
    
    
    //RESET the Log Interval Table: 
    LogIt1=read_Int_FRAM(LogIt1address);
    LogIt2=read_Int_FRAM(LogIt2address);
    LogIt3=read_Int_FRAM(LogIt3address);
    LogIt4=read_Int_FRAM(LogIt4address);
    LogIt5=read_Int_FRAM(LogIt5address);
    LogIt6=read_Int_FRAM(LogIt6address);
    
    LogItRemain1 = LogIt1;
    write_Int_FRAM(LogItRemain1address,LogItRemain1);  

    LogItRemain2 = LogIt2;
    write_Int_FRAM(LogItRemain2address,LogItRemain2);  

    LogItRemain3 = LogIt3;
    write_Int_FRAM(LogItRemain3address,LogItRemain3);  

    LogItRemain4 = LogIt4;
    write_Int_FRAM(LogItRemain4address,LogItRemain4);  

    LogItRemain5 = LogIt5;
    write_Int_FRAM(LogItRemain5address,LogItRemain5);  

    LogItRemain6 = LogIt6;
    write_Int_FRAM(LogItRemain6address,LogItRemain6);  
}

unsigned int STOP(void)                                                         
{
    if(!LC2CONTROL2.flags2.Modbus)
        crlf();

    if (!LC2CONTROL.flags.Logging && !LC2CONTROL.flags.LoggingStopTime &&!LC2CONTROL.flags.LoggingStartTime && !S_1.status1flags._Logging)       
    {
        if(!LC2CONTROL2.flags2.Modbus)
        {
            putsUART1(Loggingalreadystopped);                                   //Logging already stopped!
            while (BusyUART1());
        }
        return 0;
    }

    LC2CONTROL.flags.Logging=0;                                             
    LC2CONTROL.flags.LoggingStopTime=0;                                     
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                 
    LC2CONTROL2.flags2.scheduled=0;                                             //clear the scheduled flag  
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);                  //store in FRAM 
    S_1.status1flags._Logging=0;                                                //clear the MODBUS lOGGING flag     
    S_1.status1flags._SP=0;                                                     //clear the MODBUS Enable Stop Logging Time flag    
    S_1.status1flags._ST=0;                                                     //clear the MODBUS Enable Start Logging Time flag   
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);
    stopLogging();
    if(!LC2CONTROL2.flags2.Modbus)
    {
        if (!DISPLAY_CONTROL.flags.Shutup) 
        {
            putsUART1(Loggingstopped);                                          //Logging stopped
            while (BusyUART1());
        }
    }
    LC2CONTROL2.flags2.Waiting=0;
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);              
    return 1;
}


void STOPTIME(void)                                                             
{
    LC2CONTROL.flags.LoggingStopTime = 1;                                       //set the LoggingStopTime flag		
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //store flag in FRAM 
    S_1.status1flags._SP=1;                                                     //set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                              
    LC2CONTROL2.flags2.SetStopTime = 1;                                         //set the SetStopTime flag

    if(!LC2CONTROL2.flags2.Modbus)
    {
        Buf2DateTime(buffer);                                                   //get Logging Stop Time from buffer

        if (LC2CONTROL.flags.ERROR) 
        {
            LC2CONTROL.flags.ERROR = 0;                                         //clear the ERROR flag
            LC2CONTROL.flags.LoggingStopTime = 0;                               //clear the LoggingStopTime flag
            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);             //store flag in FRAM  
            S_1.status1flags._SP=0;                                             //clear the MODBUS status flag    
            write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);      
            LC2CONTROL2.flags2.SetStopTime = 0;                                 //clear the SetStopTime flag
            return;
        }
    }

    LC2CONTROL2.flags2.SetStopTime = 0;                                         //clear the SetStopTime flag
    
    if(!LC2CONTROL2.flags2.Modbus)
    {
        write_Int_FRAM(LoggingStopHoursaddress,LoggingStopHours);               //store logging stop time in FRAM 
        write_Int_FRAM(LoggingStopMinutesaddress,LoggingStopMinutes);  
        write_Int_FRAM(LoggingStopSecondsaddress,LoggingStopSeconds);  
    }
    else                                                                        //get logging stop time from FRAM
    {
        LoggingStopHours=read_Int_FRAM(LoggingStopHoursaddress);
        LoggingStopMinutes=read_Int_FRAM(LoggingStopMinutesaddress);
        LoggingStopSeconds=read_Int_FRAM(LoggingStopSecondsaddress);
    }

    TotalStopSeconds = ((LoggingStopHours * 3600)+(LoggingStopMinutes * 60) + LoggingStopSeconds);
    TotalStartSeconds=read_longFRAM(TotalStartSecondsaddress); 

    if (TotalStopSeconds < TotalStartSeconds)                                   //midnight rollover?	
        TotalStopSeconds += 86400;                                              //compensate
    write_longFRAM(TotalStopSeconds,TotalStopSecondsaddress);                   //store to FRAM   

    if(!LC2CONTROL2.flags2.Modbus)
    {
        displayLoggingWillStart();                                              //display logging start time
        crlf();
        if(LC2CONTROL.flags.LoggingStopTime)                    
            displayLoggingWillStop();                                           //display logging stop time    
    }
}


void storeGageType(int channel, int gageType) {
    unsigned long address;                                                      

    //calculate indexed address for gage type                                   
    address = CH1GTaddress + (2 * (channel - 1));                               

    //write the gage type to the calculated address
    write_Int_FRAM(address,gageType);                                           //store gage type in FRAM 
}


void storeLogInterval(int interval, int iterations) 
{
    interval -= 1;                                                              //change interval from 1-6 to 0-5
    
    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //if Single Channel  
    {
        write_longFRAM(LogIntLength,(SingleLogIntLength1address+(interval*4)));	//store the interval  
        write_longFRAM(LogIntLength,(SingleLogIntLength1MASTERaddress+(interval*4)));   //store as the MASTER   
        write_Int_FRAM((SingleLogIt1address+(interval*2)), iterations);         //store the iterations  
        write_Int_FRAM((SingleLogIt1MASTERaddress+(interval*2)),iterations);    //store as the MASTER   
        write_Int_FRAM((SingleLogItRemain1address+(interval*2)), iterations);	//store the remaining iterations   
        write_Int_FRAM((SingleLogItRemain1MASTERaddress+(interval*2)), iterations); //store as the MASTER   
    }
    else
    {
        write_longFRAM(LogIntLength,(LogIntLength1address+(interval*4)));       //store the interval  
        write_longFRAM(LogIntLength,(LogIntLength1MASTERaddress+(interval*4))); //store as the MASTER   
        write_Int_FRAM((LogIt1address+(interval*2)), iterations);               //store the iterations  
        write_Int_FRAM((LogIt1MASTERaddress+(interval*2)),iterations);          //store as the MASTER   
        write_Int_FRAM((LogItRemain1address+(interval*2)), iterations);         //store the remaining iterations           
        write_Int_FRAM((LogItRemain1MASTERaddress+(interval*2)),iterations);    //store as the MASTER   
    }
    
}

void storeChannelReading(int ch) {
    unsigned long FRAMaddress;
    unsigned char baseAddress = 0; 

    outputPosition=read_Int_FRAM(OutputPositionaddress);                        //get the memory pointer  

    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //if Single Channel  
        baseAddress = SingleVWBytes;
    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //if 4 channel MUX  
        baseAddress = VW4Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //if 8 channel MUX  
        baseAddress = VW8Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                     //if 16 channel MUX  
        baseAddress = VW16Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                     //if 32 channel MUX  
        baseAddress = VW32Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                      //if 8 channel Thermistor MUX  
        baseAddress = TH8Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                     //if 32 channel Thermistor MUX  
        baseAddress = TH32Bytes;

    if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32) //8 or 32 Channel Thermistor
    {
        FRAMaddress = (baseAddress * (outputPosition - 1))+(2 * (ch - 1) + 12);
        Nop();
    }
    else
        if (MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //8 or 32 Channel VW    
        FRAMaddress = (baseAddress * (outputPosition - 1))+(4 * (ch - 1) + 12);
    else
        FRAMaddress = (baseAddress * (outputPosition - 1))+(6 * (ch - 1) + 12); //calculate the external FRAM address

    if (MUX4_ENABLE.mflags.mux16_4 == Single | MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) 
    {
        write_Flt_FRAM(FRAMaddress, VWreadingProcessed);                        //store the processed VW reading
        write_Int_FRAM(FRAMaddress + 4, extThermreading);                       //store the external thermistor reading 12bit ADC value
    } 
    else
    if (MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) 
    {
        write_Flt_FRAM(FRAMaddress, VWreadingProcessed);                        //store the processed VW reading
    } 
    else 
    {
        write_Int_FRAM(FRAMaddress, extThermreading);                           //8 or 32 channel therm mux: store the external thermistor reading 12bit ADC value
    }

}

unsigned long storeReading(int ch) {
    unsigned long FRAMaddress;
    unsigned char baseAddress = 0; 


    
    memoryStatus=read_Int_FRAM(MemoryStatusaddress);                            //get the memory status
    outputPosition=read_Int_FRAM(OutputPositionaddress);                        //get the memory pointer
    userPosition=read_Int_FRAM(UserPositionaddress);                            //get the user position
    FRAM_MEMORY.memory=read_Int_FRAM(FRAM_MEMORYflagsaddress);                  //get max # of arrays
    
    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //if Single Channel VW
        baseAddress = SingleVWBytes;                                            //external FRAM base address (Single channel)
    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //if 4 channel VW MUX
        baseAddress = VW4Bytes;                                                 //external FRAM base address (4 channel)
    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //if 8 channel VW MUX
        baseAddress = VW8Bytes;                                                 //external FRAM base address (8 channel)
    if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                     //if 16 channel VW MUX
        baseAddress = VW16Bytes;                                                //external FRAM base address (16 channel)
    if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                     //if 32 channel VW MUX
        baseAddress = VW32Bytes;                                                //external FRAM base address (4 channel)
    if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                      //if 8 channel Thermistor MUX
        baseAddress = TH8Bytes;                                                 //external FRAM base address (8 channel Thermistor)
    if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                     //if 32 channel Thermistor MUX
        baseAddress = TH32Bytes;                                                //external FRAM base address (32 channel Thermistor)
    FRAMaddress = (baseAddress * (outputPosition - 1));                         //calculate the external FRAM address

    write_Int_FRAM(FRAMaddress, year);                                          //store the year data
    write_Int_FRAM(FRAMaddress + 2, julian);                                    //store the decimal date


    write_longFRAM(seconds_since_midnight,FRAMaddress + 4);                     //store the current time in seconds 
    write_Int_FRAM(FRAMaddress + 8, mainBatreading);                            //store the 3V battery voltage 12bit ADC value
    write_Int_FRAM(FRAMaddress + 10, intThermreading);                          //store the internal thermistor reading 12bit ADC value

    //VER 6.0.3:
    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //single channel VW selected   
    {
        if (gageType == 95)
            write_Flt_FRAM(FRAMaddress + 12, Lithiumreading + 0.19);
        else
            write_Flt_FRAM(FRAMaddress + 12, VWreadingProcessed);
        write_Int_FRAM(FRAMaddress + 16, extThermreading);                      //store the external thermistor reading 12bit ADC value

        if (memoryStatus < maxSingleVW)                                         //if memory has not become full 
            memoryStatus += 1;                                                  //add 1 to the memory status register
            write_Int_FRAM(MemoryStatusaddress,memoryStatus);                   //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //if 4 channel VW MUX  
    {
        if (memoryStatus < maxFourVW)                                           //if memory has not become full
            memoryStatus += 1;                                                  //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);                       //store memoryStatus and userPosition in internal FRAM    
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //if 8 channel VW MUX  
    {
        if (memoryStatus < maxEightVW)                                          //if memory has not become full
            memoryStatus += 1;                                                  //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);                       //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                     //if 16 channel VW MUX 
    {
        if (memoryStatus < maxSixteenVW)                                        //if memory has not become full
            memoryStatus += 1;                                                  //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);                       //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                     //if 32 channel VW MUX  
    {
        if (memoryStatus < maxThirtytwoVW)                                      //if memory has not become full
            memoryStatus += 1;                                                  //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);                       //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                      //if 8 channel MUX 
    {
        if (memoryStatus < maxEightTH)                                          //if memory has not become full
            memoryStatus += 1;                                                  //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);                       //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                     //if 32 channel MUX 
    {
        if (memoryStatus < maxThirtytwoTH)                                      //if memory has not become full
            memoryStatus += 1;                                                  //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);                       //store memoryStatus and userPosition in internal FRAM   
    }

    outputPosition += 1;                                                        //increment the output position pointer


    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //Single Channel VW 
    {
        if (outputPosition > maxSingleVW)                                       //if memory is full 
            wrap_stop(); 
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //if 4 channel VW MUX  
    {
        if (outputPosition > maxFourVW)                                         //if memory is full
            wrap_stop(); 
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //if 8 channel VW MUX  
    {
        if (outputPosition > maxEightVW)                                        //if memory is full
            wrap_stop(); 
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                     //if 16 channel VW MUX 
    {
        if (outputPosition > maxSixteenVW)                                      //if memory is full
            wrap_stop(); 
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                     //if 32 channel VW MUX 
    {
        if (outputPosition > maxThirtytwoVW)                                    //if memory is full
            wrap_stop(); 
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                      //if 8 channel TH MUX 
    {
        if (outputPosition > maxEightTH)                                        //if memory is full
            wrap_stop(); 
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                     //if 32 channel TH MUX 
    {
        if (outputPosition > maxThirtytwoTH)                                    //if memory is full
            wrap_stop(); 
    }

    write_Int_FRAM(OutputPositionaddress,outputPosition);                       //store outputPosition pointer in internal FRAM  

    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //Single Channel VW  
    {                                                                           //User Position = Memory Status until Memory is full
        if (memoryStatus < maxSingleVW) 
            userPosition = memoryStatus;

        if (memoryStatus >= maxSingleVW && outputPosition == 1) 
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1;                                  //after that User Position equals Output Position - 1

        if (userPosition == (maxSingleVW + 1)) 
            userPosition = 1;
    }


    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //if 4 channel VW MUX  
    {                                                                           //User Position = Memory Status until Memory is full
        if (memoryStatus < maxFourVW)
            userPosition = memoryStatus;
                                                                                //User Position = Output Position for first reading
        if (memoryStatus >= maxFourVW && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1;                                  //after that User Position equals Output Position - 1

        if (userPosition == (maxFourVW + 1))
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //if 8 channel VW MUX  
    {                                                                           //User Position = Memory Status until Memory is full
        if (memoryStatus < maxEightVW)
            userPosition = memoryStatus;
                                                                                //User Position = Output Position for first reading
        if (memoryStatus >= maxEightVW && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1;                                  //after that User Position equals Output Position - 1

        if (userPosition == (maxEightVW + 1))
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                     //do same for 16 channel VW MUX    
    {                                                                           //User Position = Memory Status until Memory is full
        if (memoryStatus < maxSixteenVW)
            userPosition = memoryStatus;
                                                                                //User Position = Output Position for first reading
        if (memoryStatus >= maxSixteenVW && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1;                                  //after that User Position equals Output Position - 1

        if (userPosition == (maxSixteenVW + 1))
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                     //do same for 32 channel VW MUX    
    {                                                                           //User Position = Memory Status until Memory is full
        if (memoryStatus < maxThirtytwoVW)
            userPosition = memoryStatus;
                                                                                //User Position = Output Position for first reading
        if (memoryStatus >= maxThirtytwoVW && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1;                                  //after that User Position equals Output Position - 1

        if (userPosition == (maxThirtytwoVW + 1))
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                      //do same for 8 channel TH MUX    
    {                                                                           //User Position = Memory Status until Memory is full
        if (memoryStatus < maxEightTH)
            userPosition = memoryStatus;
                                                                                //User Position = Output Position for first reading
        if (memoryStatus >= maxEightTH && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1;                                  //after that User Position equals Output Position - 1

        if (userPosition == 12801)
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                     //do same for 32 channel TH MUX    
    {                                                                           //User Position = Memory Status until Memory is full
        if (memoryStatus < maxThirtytwoTH)
            userPosition = memoryStatus;
                                                                                //User Position = Output Position for first reading
        if (memoryStatus >= maxThirtytwoTH && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1;                                  //after that User Position equals Output Position - 1

        if (userPosition == 4801)
            userPosition = 1;
    }

    write_Int_FRAM(UserPositionaddress,userPosition);  

    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //Single Channel VW  
    {
        if (outputPosition == 1)                                                //memory has rolled over
            outputPosition = maxSingleVW;                                       //point to last array for displayReading() 
        else
            outputPosition -= 1;                                                //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //if 4 channel VW MUX  
    {
        if (outputPosition == 1)                                                //memory has rolled over
            outputPosition = maxFourVW;                                         //point to last array for displayReading()
        else
            outputPosition -= 1;                                                //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //if 8 channel VW MUX  
    {
        if (outputPosition == 1)                                                //memory has rolled over
            outputPosition = maxEightVW;                                        //point to last array for displayReading()
        else
            outputPosition -= 1;                                                //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                     //if 16 channel VW MUX 
    {
        if (outputPosition == 1)                                                //memory has rolled over
            outputPosition = maxSixteenVW;                                      //point to last array for displayReading()
        else
            outputPosition -= 1;                                                //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                     //if 32 channel VW MUX 
    {
        if (outputPosition == 1)                                                //memory has rolled over
            outputPosition = maxThirtytwoVW;                                    //point to last array for displayReading()
        else
            outputPosition -= 1;                                                //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                      //if 8 channel TH MUX 
    {
        if (outputPosition == 1)                                                //memory has rolled over
            outputPosition = maxEightTH;                                        //point to last array for displayReading()
        else
            outputPosition -= 1;                                                //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                     //if 32 channel TH MUX 
    {
        if (outputPosition == 1)                                                //memory has rolled over
            outputPosition = maxThirtytwoTH;                                    //point to last array for displayReading()
        else
            outputPosition -= 1;                                                //otherwise point to the reading just stored
    }


    FRAM_MEMORY.flags.memEmpty = 0;                                             //clear the memory empty flag
    write_Int_FRAM(FRAM_MEMORYflagsaddress,FRAM_MEMORY.memory);                 //store flag in FRAM 

    return outputPosition;                                                      //return the pointer for use in displayReading()
}

void storeTempChannelReading(int ch)                                            //Store 'X' command readings to FRAM
{
    unsigned long TempFRAMaddress;
    unsigned int DEC_Thermreading=0;                                            

    if (MUX4_ENABLE.mflags.mux16_4 == Single | 
            MUX4_ENABLE.mflags.mux16_4 == VW4 |
            MUX4_ENABLE.mflags.mux16_4 == VW16) 
    {
        TempFRAMaddress = (XmemStart+0xC) + (6 * (ch - 1));                     //compute memory location for external thermistor   
        DEC_Thermreading=DEC_TEMP.decimaltemp;                                          
        write_Int_FRAM(TempFRAMaddress,DEC_Thermreading);                       //store the float converted to int value    
        TempFRAMaddress = (XmemStart+0xE) + (6 * (ch - 1));                     //compute memory location for VW reading  
        write_Flt_FRAM(TempFRAMaddress,VWreadingProcessed);                     //store the processed VW reading  
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8 | 
            MUX4_ENABLE.mflags.mux16_4 == VW32) 
    {
        TempFRAMaddress = (XmemStart+0xC) + (4 * (ch - 1));                     //compute memory location for VW reading            
        write_Flt_FRAM(TempFRAMaddress,VWreadingProcessed);                     //store the processed VW reading     
    }

}

void storeTempChannelTHReading(int ch) 
{
    unsigned long TempFRAMaddress;

    TempFRAMaddress = (XmemStart+0xC) + (2 * (ch - 1));                         //compute memory location for external thermistor    
    write_Int_FRAM(TempFRAMaddress,extThermreading);                            //store the external thermistor reading 12bit ADC value 
}

void storeTempReading(int ch) 
{
    write_Int_FRAM(XmemStart,year);                                             //store the year data 
    write_Int_FRAM(XmemStart+2,julian);                                         //store the decimal date 
    write_longFRAM(seconds_since_midnight,XmemStart+4);                         //store the current time (absolute) 
    write_Int_FRAM(XmemStart+8,mainBatreading);                                 //store the 3V battery voltage 12bit ADC value 
    write_Int_FRAM(XmemStart+10,intThermreading);                               //store the internal thermistor reading 12bit ADC value 
}

unsigned long synch(unsigned long currenttimeseconds, unsigned long scaninterval) 
{
    unsigned int scanintmul = 0;                                                //scan interval multiplier

    for (scanintmul = 1; scaninterval * scanintmul < currenttimeseconds; scanintmul++) 
    {
    }                                                                           //determine next evenly divisible reading time based on
                                                                                //the scan interval
                                                                                //determine value to add to current clock (syncvalue) to
                                                                                // make the next reading synchronous
    VWflagsbits.synch = 0;                                                      //clear the synch flag
    return (scaninterval * scanintmul);
}                                                                               //close synch()

void synch_one(void)                                                            
{
    DISPLAY_CONTROL.flags.Synch = 1;                                            //set the Synch flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM 
    S_1.status1flags._Sync=1;                                                   //set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1); 
    
    if(!LC2CONTROL2.flags2.Modbus)                                              //If command line interface
    {
        putsUART1(Synch);                                                       //"Readings are synchronized to the top of the hour"
        while (BusyUART1());
    }

    if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting)                //is datalogger logging?	
    {
        DISPLAY_CONTROL.flags.Shutup = 1;                                       //don't allow message display
        stopLogging();                                                          //stop and restart logging if it is
        if (DISPLAY_CONTROL.flags.Synch)
            VWflagsbits.synch = 1;                                              //set the synch flag
        if(!LC2CONTROL2.flags2.Modbus)                                          //If command line interface
            crlf();
        startLogging();
        DISPLAY_CONTROL.flags.Shutup = 0;
    }    
}


void synch_zero(void)                                                           
{
    DISPLAY_CONTROL.flags.Synch = 0;                                            //clear the Synch flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM 
    S_1.status1flags._Sync=0;                                                   //clear the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1); 
    
    if(!LC2CONTROL2.flags2.Modbus)                                              //If command line interface
    {
        putsUART1(Synchnot);                                                    //"Readings are not synchronized to the top of the hour"
        while (BusyUART1());
    }

    if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting)                //is datalogger logging?	
    {
        DISPLAY_CONTROL.flags.Shutup = 1;                                       //don't allow message display
        stopLogging();                                                          //stop and restart logging if it is
        if (DISPLAY_CONTROL.flags.Synch)
            VWflagsbits.synch = 1;                                              //clear the synch flag
        if(!LC2CONTROL2.flags2.Modbus)                                          //If command line interface
            crlf();
        startLogging();
        DISPLAY_CONTROL.flags.Shutup = 0;
    }
}

unsigned int take_analog_reading(unsigned char gt)                              
{
    unsigned int analog;                                                        
    unsigned int *ADC16Ptr;
    unsigned int count;
    int internalTemperature=0;                                                  
        
    TRISB=0x033D;                                                               //Configure PORTB   
    LATB=0;

    if (gt >= 85 && gt <= 98)                                                           
    {
        if (gt == 86)                                                           //read the internal temperature 
        {
            internalTemperature=readDS3231temperature();                        //get the temp reading from the DS3231 RTC
            return internalTemperature;
        }

        IPC5bits.INT1IP=0;                                                      //disable the INT1 interrupt    
        //Enable ADC:
        AD1CON1bits.AD12B=1;                                                    //configure ADC for 12 bit operation    
        IFS1bits.INT1IF=0;                                                      
        PMD1bits.AD1MD=0;                                                       //Enable the ADC1 module    

        if(gt!=98)                                                              //3VX already on    
            _3VX_on();                                                          //power-up analog circuitry 

        //Setup 12 bit DAC:
        AD1PCFGH = 0xFFFF;                                                      //AN31..AN16 configured as digital I/O
        AD1PCFGL = 0xFCC2;                                                      //AN9,8,5,4,3,2,0 configured as analog inputs 

        
        AD1CON1bits.ADSIDL=1;                                                   //Discontinue in Idle
        AD1CON1bits.AD12B=1;                                                    //12bit
        AD1CON1bits.FORM=0;                                                     //integer format
        AD1CON1bits.SSRC=7;                                                      //Autoconvert
        AD1CON1bits.ASAM=0;                                                      //manual sampling  
        AD1CON2bits.VCFG=1;                                                     //External VREF+,AVss
        AD1CON2bits.SMPI=0xF;                                                   //interrupt after every 16th sample
        AD1CON3bits.ADRC=0;                                                     
        AD1CON3bits.SAMC=0x1F;                                                  //Autosample time = 1 Tad   
        AD1CON3bits.ADCS=0x0F;                                                  //Conversion clock select   
        
                                                                                //integer output
        if (gt == 85)                                                           //read the external thermistor 
            AD1CHS0 = 0x0004;                                                   //connect AN4 as CH0 positive input
        if (gt == 87)                                                           //read the main 12V battery 
            AD1CHS0 = 0x0003;                                                   //connect AN3 as CH0 input
        if (gt == 95)                                                           //read the 3V lithium cell 
        {
            AD1CHS0 = 0x0005;                                                   //connect AN5 as CH0 input
            SAMPLE_LITHIUM = 1;                                                 //sample the lithium battery
        }
        if (gt == 97)                                                           //read the main 3V battery 
            AD1CHS0 = 0x0002;                                                   //connect AN2 as CH0 input
        if (gt==98)                                                              //read V_AGC    
            AD1CHS0=0x0009;                                                     //connect AN9 as CH0 input

        analog = 0;                                                             //clear the value
        ADC16Ptr = &ADC1BUF0;                                                   //initialize ADCBUF pointer
        IFS0bits.AD1IF = 0;                                                     //clear ADC interrupt flag

        AD1CON1bits.ADON = 1;                                                   //turn ADC on     
        delay(32000);                                                           

        for(count=0;count<16;count++)
        {
           AD1CON1bits.DONE=0;                                                  
           AD1CON1bits.SAMP=1;                                                  //begin sampling    
           while(!IFS0bits.AD1IF);                                              
           AD1CON1bits.SAMP=0;                                                  //stop sampling 
           IFS0bits.AD1IF=0;                                                    
           analog=analog+*ADC16Ptr;                                           
        }
        AD1CON1bits.ADON = 0;                                                   //turn ADC off      
        SAMPLE_LITHIUM = 0;                                                     //turn off lithium battery sampling if on
        
        if(gt!=98)                                                              
            _3VX_off();                                                         //power-down analog circuitry   

        analog = analog >> 4;                                                   //average the result    
        if(gt==85)
            analog-=24;                                                         //CORRECT FOR READING ERROR
        IFS1bits.INT1IF=0;                                                      //clear the interrupt flag  
        IPC5bits.INT1IP=7;                                                      //re-enable INT1 at priority level 7 (HIGHEST)    
        return analog;                                                          //return the averaged 12 bit value	
    }

}

unsigned int take_fast_analog_reading(void)                                     
{
    volatile unsigned int dataBUF[20];                                                   
    unsigned char i=0;
    volatile unsigned int data=0;
    unsigned int *ADC16Ptr;
    
    //Configure ADC:
    PMD1bits.AD1MD=0;                                                           //Enable the ADC
    AD1CON1bits.ADSIDL=1;                                                       //Discontinue in Idle
    AD1CON1bits.AD12B=1;                                                        //12bit
    AD1CON1bits.FORM=0;                                                         //integer format
    AD1CON1bits.SSRC=7;                                                         //Autoconvert
    AD1CON1bits.ASAM=0;                                                         //manual sampling  
    AD1CON2bits.VCFG=1;                                                         //External VREF+,AVss
    AD1CON2bits.SMPI=0xF;                                                       //interrupt after every 16th sample
    AD1CON3bits.ADRC=0;                                                             
    AD1CON3bits.SAMC=0x03;                                                      //sampling clock = 3 Tad
    AD1CON3bits.ADCS=0x0D;                                                      //conversion clock = 14 Tad (ADCS value +1)
    AD1CHS0=0x0004;                                                             //AN4 is positive input
    IFS0bits.AD1IF=0;                                                           //Clear the ADC interrupt flag
    

    //Turn on the ADC:
    AD1CON1bits.ADON=1;      
    ADC16Ptr = &ADC1BUF0;                                                       //point to the ADC buffer0 address

    IFS0bits.AD1IF=0;                                                           //clear the interrupt flag

    for(i=0;i<20;i++)                                                              
    {
        AD1CON1bits.DONE=0;                                                  
        AD1CON1bits.SAMP=1;                                                     //begin sampling   
        while(!IFS0bits.AD1IF && !IFS3bits.T8IF);                                                                    
        AD1CON1bits.SAMP=0;                                                     //stop sampling 
        IFS0bits.AD1IF=0;                                                    
        dataBUF[i]=*ADC16Ptr;                                                   //store value in array  
    }
    //Turn off the ADC:
    AD1CON1bits.ADON=0;                                                         
    PMD1bits.AD1MD=1;                                                           //Disable the ADC
    
    //5 point Moving average filter the readings                                
    data=filterArray(dataBUF);                                                  
    return data;                                                                //return the data
}



void take_One_Complete_Reading(unsigned char store) 
{
    int ch;                                                                     //for loop index
    int ch_max;
    unsigned long outputPosition;
    unsigned long ReadingTimeSeconds;
    float batteryVoltage = 0.0;                                                 
    float TEST12V=0.0;                                                          
    float extThermRaw=0.0;                                                      
	float extThermProcessed=0.0;                                                

    
    
    _3VX_on();                                                                  //power-up analog circuitry 
    U1MODEbits.UARTEN=0;                                                        //Disable the COM PORT  
  
    if (store) 
    {
        SLEEP12V = 0;                                                           //set regulator into switchmode when wake from sleep
        LC2CONTROL.flags.ScanError = 0;                                         //clear the flag
        setClock(0x0F, 0);                                                      //Clear the RTC Alarm flags
        ReadingTimeSeconds = RTChms2s(1);                                       //get the current time from the RTC
        DISPLAY_CONTROL.flags.TakingReading = 1;                                //set the TakingReading flag
    }

    IEC1bits.INT1IE = 0;                                                        //temporarily disable the INT1 interrupt
    seconds_since_midnight = RTChms2s(1);                                       //get the total seconds from RTC
    RTCdays = readClock(RTCDaysAddress);                                        //get the day from the RTC
    RTCmonths = readClock(RTCMonthsAddress);                                    //get the month from the RTC
    RTCyears = readClock(RTCYearsAddress);                                      //get the year from the RTC
    IEC1bits.INT1IE = 1;                                                        //re-enable the INT2 interrupt

    //Need to test if current time = Logging Stop Time and Stop Time is enabled
    //If it is then need to set Next Time to Read to Logging Start Time
    
    if (store) 
    {
        upD8RTCAlarm1();                                                        //determine and load next reading time
    } else 
    {
        if (LC2CONTROL2.flags2.Interrupt)                                       //did INT2 interrupt occur during this period?
        {
            U1MODEbits.UARTEN=1;                                                //Re-enable the COM PORT  
            U1STAbits.UTXEN=1;                                                  
            return;                                                             //abort if it did
        }
    }

    year = RTCtoDecimal(RTCyears);                                              //convert BCD RTC data to decimal
    month = RTCtoDecimal(RTCmonths);
    day = RTCtoDecimal(RTCdays);
    julian = toJulian();                                                        //convert date to decimal date

    if (store)
        restoreSettings();


    IEC1bits.INT1IE = 0;                                                        //temporarily disable the INT2 interrupt
    TotalStopSeconds=read_longFRAM(TotalStopSecondsaddress);                    //get the stored stop time    
    LoggingStartDay=read_Int_FRAM(LoggingStartDayaddress);                      //Get the Logging Start day from FRAM   
    IEC1bits.INT1IE = 1;                                                        //re-enable the INT2 interrupt
    Nop();
    Nop();

    if (!store && LC2CONTROL2.flags2.Interrupt)
    {
        U1MODEbits.UARTEN=1;                                                    //Re-enable the COM PORT  
        U1STAbits.UTXEN=1;                                                      
        return;
    }

    if (store) 
    {
        if (LoggingStartDay != day && TotalStopSeconds >= 86400)                //Did midnight rollover occur?
        {
            TotalStopSeconds -= 86400;                                          //compensate TotalStopSeconds
            write_longFRAM(TotalStopSeconds,TotalStopSecondsaddress);           //update FRAM   
        }

        if (LC2CONTROL.flags.LoggingStopTime && (TotalStopSeconds < seconds_since_midnight)) //if scheduled Stop Logging is enabled
        {   
            DISPLAY_CONTROL.flags.TakingReading = 0;                            //Reset the Taking Reading flag  
            write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);//store flags in FRAM VER 6.0.13  
            stopLogging(); //and it's past the Stop Logging time
            U1MODEbits.UARTEN=1;                                                //Re-enable the COM PORT  
            U1STAbits.UTXEN=1;                                                  
            return;
        }
    } else 
    {
        if (LoggingStartDay != day) {
            TotalStopSeconds -= 86400;
            LoggingStartDay = day;
        }
    }

    //Retrieve the settings from FRAM:                                          
    MUX4_ENABLE.mux=read_Int_FRAM(MUX4_ENABLEflagsaddress);                     
    MUX_ENABLE1_16.MUXen1_16=read_Int_FRAM(MUX_ENABLE1_16flagsaddress);         
    MUX_ENABLE17_32.MUXen17_32=read_Int_FRAM(MUX_ENABLE17_32flagsaddress);      
    MUX_CONVERSION1_16.MUXconv1_16=read_Int_FRAM(MUX_CONV1_16flagsaddress);     
    MUX_CONVERSION17_32.MUXconv17_32=read_Int_FRAM(MUX_CONV17_32flagsaddress);  
    
    
    
    // TAKE VW READINGS:
    if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) //activate mux if multichannel  
        MUX_RESET = 1;                                                          //set the MUX_RESET line high
    if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32 |
            MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //if 8 or 32 channel mux  
        MUX_CLOCK = 1;                                                          //set the MUX_CLOCK line High   
    
    delay(11000);                                                               //4mS delay between ENABLE & 1st CLOCK  

    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //single channel selected   
        ch_max = 1;
    if (MUX4_ENABLE.mflags.mux16_4 == VW4)                                      //4 channel mux selected    
        ch_max = 4;
    if (MUX4_ENABLE.mflags.mux16_4 == VW8)                                      //8 channel mux selected    
        ch_max = 8;
    if (MUX4_ENABLE.mflags.mux16_4 == VW16)                                     //16 channel mux selected   
        ch_max = 16;
    if (MUX4_ENABLE.mflags.mux16_4 == VW32)                                     //32 channel mux selected    
        ch_max = 32;
    if (MUX4_ENABLE.mflags.mux16_4 == TH8)                                      //8 channel mux selected    
        ch_max = 8;
    if (MUX4_ENABLE.mflags.mux16_4 == TH32)                                     //32 channel mux selected    
        ch_max = 32;

    for (ch = 1; ch <= ch_max; ch++)                                            //mux loop
    {
        if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) //activate mux if multichannel  
            clockMux(10000);                                                    //double pulse clock the MUX
        if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32 |
                MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //if 8 or 32 channel mux  
            clockThMux(40000); 
        MUX4_ENABLE.mflags.skip = 0;                                            //clear channel skip flag

        if (!store)
            IEC1bits.INT1IE = 0;                                                //temporarily disable the INT2 interrupt
        
        VWreadingProcessed=-0.0;                                                //Store -0.0 to indicate channel is disabled    
        extThermreading=0x0001;                                                 //Store -0.0 to indicate channel is disabled    
        storeChannelReading(ch);                                                //store the reading 

        switch (ch)                                                             //load the channel parameters
        {
            case 1:                                                             //Channel 1
                
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                               
                        if(!THMUX_ENABLE1_16.t1flags.CH1)                       //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                               

                    if (!MUX_ENABLE1_16.e1flags.CH1)                            //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH1GTaddress);                       //load the gage type from internal FRAM
                    zeroReading=read_float(CH1ZRaddress);                       //load the zero reading from internal FRAM
                    gageFactor=read_float(CH1GFaddress);                        //load the gage factor from internal FRAM
                    gageOffset=read_float(CH1GOaddress);                        //load the gage offset from internal FRAM
                    polyCoA=read_float(CH1PolyCoAaddress);                      //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH1PolyCoBaddress);                      //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH1PolyCoCaddress);                      //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


            case 2:                                                             //Channel 2
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH2)                       //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE1_16.e1flags.CH2)                            //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                     gageType=read_Int_FRAM(CH2GTaddress);                      //load the gage type from internal FRAM
                     zeroReading=read_float(CH2ZRaddress);                      //load the zero reading from internal FRAM
                     gageFactor=read_float(CH2GFaddress);                       //load the gage factor from internal FRAM
                     gageOffset=read_float(CH2GOaddress);                       //load the gage offset from internal FRAM
                     polyCoA=read_float(CH2PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                     polyCoB=read_float(CH2PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                     polyCoC=read_float(CH2PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 3:                                                         //Channel 3
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH3)                       //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                 

                    if (!MUX_ENABLE1_16.e1flags.CH3)                            //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                     gageType=read_Int_FRAM(CH3GTaddress);                      //load the gage type from internal FRAM
                     zeroReading=read_float(CH3ZRaddress);                      //load the zero reading from internal FRAM
                     gageFactor=read_float(CH3GFaddress);                       //load the gage factor from internal FRAM
                     gageOffset=read_float(CH3GOaddress);                       //load the gage offset from internal FRAM
                     polyCoA=read_float(CH3PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                     polyCoB=read_float(CH3PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                     polyCoC=read_float(CH3PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 4:                                                         //Channel 4
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH4)                       //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH4)                            //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH4GTaddress);                       //load the gage type from internal FRAM
                    zeroReading=read_float(CH4ZRaddress);                       //load the zero reading from internal FRAM
                    gageFactor=read_float(CH4GFaddress);                        //load the gage factor from internal FRAM
                    gageOffset=read_float(CH4GOaddress);                        //load the gage offset from internal FRAM
                    polyCoA=read_float(CH4PolyCoAaddress);                      //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH4PolyCoBaddress);                      //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH4PolyCoCaddress);                      //load the polynomial coefficient C from internal FRAM
                     
                    break;                                                      //break out of switch(ch)


                case 5:                                                         //Channel 5
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH5)                       //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                 

                    if (!MUX_ENABLE1_16.e1flags.CH5)                            //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH5GTaddress);                       //load the gage type from internal FRAM
                    zeroReading=read_float(CH5ZRaddress);                       //load the zero reading from internal FRAM
                    gageFactor=read_float(CH5GFaddress);                        //load the gage factor from internal FRAM
                    gageOffset=read_float(CH5GOaddress);                        //load the gage offset from internal FRAM
                    polyCoA=read_float(CH5PolyCoAaddress);                      //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH5PolyCoBaddress);                      //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH5PolyCoCaddress);                      //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 6:                                                         //Channel 6
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH6)                       //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH6)                            //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                     gageType=read_Int_FRAM(CH6GTaddress);                      //load the gage type from internal FRAM
                     zeroReading=read_float(CH6ZRaddress);                      //load the zero reading from internal FRAM
                     gageFactor=read_float(CH6GFaddress);                       //load the gage factor from internal FRAM
                     gageOffset=read_float(CH6GOaddress);                       //load the gage offset from internal FRAM
                     polyCoA=read_float(CH6PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                     polyCoB=read_float(CH6PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                     polyCoC=read_float(CH6PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                     
                    break;                                                      //break out of switch(ch)


                case 7:                                                         //Channel 7
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH7)                       //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH7)                            //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                   
                    gageType=read_Int_FRAM(CH7GTaddress);                       //load the gage type from internal FRAM
                    zeroReading=read_float(CH7ZRaddress);                       //load the zero reading from internal FRAM
                    gageFactor=read_float(CH7GFaddress);                        //load the gage factor from internal FRAM
                    gageOffset=read_float(CH7GOaddress);                        //load the gage offset from internal FRAM
                    polyCoA=read_float(CH7PolyCoAaddress);                      //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH7PolyCoBaddress);                      //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH7PolyCoCaddress);                      //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 8:                                                         //Channel 8
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH8)                       //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH8)                            //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH8GTaddress);                       //load the gage type from internal FRAM
                    zeroReading=read_float(CH8ZRaddress);                       //load the zero reading from internal FRAM
                    gageFactor=read_float(CH8GFaddress);                        //load the gage factor from internal FRAM
                    gageOffset=read_float(CH8GOaddress);                        //load the gage offset from internal FRAM
                    polyCoA=read_float(CH8PolyCoAaddress);                      //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH8PolyCoBaddress);                      //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH8PolyCoCaddress);                      //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 9:                                                         //Channel 9
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH9)                       //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                 

                    if (!MUX_ENABLE1_16.e1flags.CH9)                            //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH9GTaddress);                       //load the gage type from internal FRAM
                    zeroReading=read_float(CH9ZRaddress);                       //load the zero reading from internal FRAM
                    gageFactor=read_float(CH9GFaddress);                        //load the gage factor from internal FRAM
                    gageOffset=read_float(CH9GOaddress);                        //load the gage offset from internal FRAM
                    polyCoA=read_float(CH9PolyCoAaddress);                      //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH9PolyCoBaddress);                      //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH9PolyCoCaddress);                      //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 10:                                                        //Channel 10
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH10)                      //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH10)                           //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH10GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH10ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH10GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH10GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH10PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH10PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH10PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 11:                                                        //Channel 11
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH11)                      //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH11)                           //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH11GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH11ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH11GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH11GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH11PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH11PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH11PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 12:                                                        //Channel 12
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH12)                      //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH12)                           //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH12GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH12ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH12GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH12GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH12PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH12PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH12PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 13:                                                        //Channel 13
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH13)                      //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                              
                    }                                                                                 

                    if (!MUX_ENABLE1_16.e1flags.CH13)                           //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH13GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH13ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH13GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH13GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH13PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH13PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH13PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 14:                                                        //Channel 14
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH14)                      //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH14)                           //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH14GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH14ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH14GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH14GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH14PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH14PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH14PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 15:                                                        //Channel 15
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH15)                      //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH15)                           //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH15GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH15ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH15GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH15GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH15PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH15PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH15PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 16:                                                        //Channel 16
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE1_16.t1flags.CH16)                      //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE1_16.e1flags.CH16)                           //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH16GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH16ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH16GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH16GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH16PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH16PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH16PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 17:                                                        //Channel 17
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH17)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                                  

                    if (!MUX_ENABLE17_32.e2flags.CH17)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH17GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH17ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH17GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH17GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH17PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH17PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH17PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 18:                                                        //Channel 18
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH18)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                              

                    if (!MUX_ENABLE17_32.e2flags.CH18)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH18GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH18ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH18GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH18GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH18PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH18PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH18PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 19:                                                        //Channel 19
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH19)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH19)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH19GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH19ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH19GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH19GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH19PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH19PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH19PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 20:                                                        //Channel 20
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH20)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH20)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH20GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH20ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH20GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH20GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH20PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH20PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH20PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 21:                                                        //Channel 21
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH21)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH21)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                   
                    gageType=read_Int_FRAM(CH21GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH21ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH21GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH21GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH21PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH21PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH21PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 22:                                                        //Channel 22
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH22)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH22)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH22GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH22ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH22GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH22GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH22PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH22PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH22PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                     
                    break;                                                      //break out of switch(ch)


                case 23:                                                        //Channel 23
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH23)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH23)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH23GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH23ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH23GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH23GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH23PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH23PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH23PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)


                case 24:                                                        //Channel 24
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH24)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH24)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH24GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH24ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH24GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH24GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH24PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH24PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH24PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                   
                    break;                                                      //break out of switch(ch)


                case 25:                                                        //Channel 25
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH25)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH25)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH25GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH25ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH25GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH25GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH25PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH25PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH25PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 26:                                                        //Channel 26
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH26)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                              

                    if (!MUX_ENABLE17_32.e2flags.CH26)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH26GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH26ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH26GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH26GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH26PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH26PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH26PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 27:                                                        //Channel 27
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH27)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH27)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH27GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH27ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH27GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH27GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH27PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH27PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH27PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 28:                                                        //Channel 28
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH28)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH28)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH28GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH28ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH28GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH28GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH28PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH28PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH28PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 29:                                                        //Channel 29
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH29)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH29)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH29GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH29ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH29GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH29GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH29PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH29PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH29PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 30:                                                        //Channel 30
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH30)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                              

                    if (!MUX_ENABLE17_32.e2flags.CH30)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH30GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH30ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH30GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH30GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH30PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH30PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH30PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 31:                                                        //Channel 31
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH31)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH31)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH31GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH31ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH31GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH31GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH31PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH31PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH31PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                case 32:                                                        //Channel 32
                    
                    if(MUX4_ENABLE.mflags.mux16_4==TH8 | MUX4_ENABLE.mflags.mux16_4==TH32)      
                    {                                                           
                        if(!THMUX_ENABLE17_32.t2flags.CH32)                     //Therm channel disabled    
                            MUX4_ENABLE.mflags.skip = 1;                        //set the skip flag 
                        break;                                                  //break out of switch(ch)                               
                    }                                                                               

                    if (!MUX_ENABLE17_32.e2flags.CH32)                          //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1;                            //set the skip flag
                        break;                                                  //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH32GTaddress);                      //load the gage type from internal FRAM
                    zeroReading=read_float(CH32ZRaddress);                      //load the zero reading from internal FRAM
                    gageFactor=read_float(CH32GFaddress);                       //load the gage factor from internal FRAM
                    gageOffset=read_float(CH32GOaddress);                       //load the gage offset from internal FRAM
                    polyCoA=read_float(CH32PolyCoAaddress);                     //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_float(CH32PolyCoBaddress);                     //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_float(CH32PolyCoCaddress);                     //load the polynomial coefficient C from internal FRAM
                    
                    break;                                                      //break out of switch(ch)

                default:
                    break;

            }                                                                   //end of switch(ch)


        if (!store) 
        {
            IEC1bits.INT1IE = 1;                                                //re-enable the INT2 interrupt
            Nop();
            Nop();
            if (LC2CONTROL2.flags2.Interrupt)
            {
                U1MODEbits.UARTEN=1;                                            //Re-enable the COM PORT  
                U1STAbits.UTXEN=1;                                              
                return;
            }
        }

        if (MUX4_ENABLE.mflags.skip)                                            //skip channel
        {
            if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) 
                clockMux(10);
            if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32 |
                    MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //if 8 or 32 channel mux  
                clockThMux(10); 
            continue;                                                           //return to beginning of for(ch)
        }

        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) //VW logger
        {
            if (gageType > 0 && gageType <= 6)                                  //VW                             
            {
                Blink(1);

                if (!store)
                    IEC1bits.INT1IE = 0;                                        //temporarily disable the INT2 interrupt
                //WDTSWEnable;                                                    //Start WDT                           
                VWreading = take_reading(gageType,ch);                          //take VW reading (or other gage type) 
                if(!VWreading)                                                  //retry if no response  
                {
                    VWflagsbits.retry=1;                                        
                    VWreading=take_reading(gageType,ch);                        
                    VWflagsbits.retry=0;                                        
                }
                disableVWchannel();                                             

                //ClrWdt();                                                       //clear the WDT 
                //WDTSWDisable;                                                   //Stop WDT 
                if (!store) {
                    IEC1bits.INT1IE = 1;                                        //re-enable the INT2 interrupt
                    Nop();
                    Nop();
                    if (LC2CONTROL2.flags2.Interrupt)
                    {
                        U1MODEbits.UARTEN=1;                                    //Re-enable the COM PORT     
                        U1STAbits.UTXEN=1;                                                          
                        return;
                    }
                }
                
                if (VWreading == 0.0)
                    VWreadingProcessed = -999999;                               //error message
                else
                {                                                               
                    processReading(VWreading, ch);                              //apply linear or polynomial conversion
                    write_Int_FRAM(CH1Reading+(2*(ch-1)),F);                    //store the whole # portion of VWreading in FRAM    
                }

                if (!store)
                    IEC1bits.INT1IE = 0;                                        //temporarily disable the INT2 interrupt
                
                if (MUX4_ENABLE.mflags.mux16_4 == Single |
                        MUX4_ENABLE.mflags.mux16_4 == VW4 |
                        MUX4_ENABLE.mflags.mux16_4 == VW16)
                {

                    extThermreading = take_analog_reading(85);                  //take external thermistor reading
                    if(extThermreading>5000)
                    {
                        extThermProcessed=-99.0;
                    }
                    else
                    {
                        extThermRaw=((Vref*extThermreading)/4096);              //convert to float voltage	 
                        extThermProcessed=V_HT2C(extThermRaw,ch);               //convert to float degrees C	
                    }
                    extThermreading=f32toINT16(extThermProcessed);              //convert float to 16 bit	
                }

                if (store && MUX4_ENABLE.mflags.mux16_4 != Single)
                    storeChannelReading(ch);                                    //store the reading   
                if (!store)
                    storeTempChannelReading(ch);
            }
        } 
        else                                                                    //Thermistor logger
        {
            Blink(1);

            extThermreading = take_analog_reading(85);                          //take external thermistor reading
            if(extThermreading>5000)
            {
                extThermProcessed=-99.0;
            }
            else
            {
                extThermRaw=((Vref*extThermreading)/4096);                      //convert to float voltage	 
                extThermProcessed=V_HT2C(extThermRaw,ch);                       //convert to float degrees C	
            }            
            extThermreading=f32toINT16(extThermProcessed);                      //convert float to 16 bit	
            
            if (store)
                storeChannelReading(ch);
            else
                storeTempChannelTHReading(ch);
        }

        if (!store) {
            IEC1bits.INT1IE = 1;                                                //re-enable the INT1 interrupt
            Nop();
            Nop();
            if (LC2CONTROL2.flags2.Interrupt)
            {
                U1MODEbits.UARTEN=1;                                            //Re-enable the COM PORT  
                U1STAbits.UTXEN=1;                                              
                return;
            }
        }
    }                                                                           //end of MUX loop for(ch)

    if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) //deactivate mux if multichannel  
        MUX_RESET = 0;                                                          //bring the MUX_RESET line low
    if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32 |
            MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //if 8 or 32 channel mux        
        MUX_CLOCK = 0;


    // TAKE ANALOG READINGS:

    if (store) {
        intThermreading = take_analog_reading(86);                              //take internal thermistor reading
    } else {
        IEC1bits.INT1IE = 0;                                                    //temporarily disable the INT1 interrupt
        intThermreading = take_analog_reading(86);                              //take internal thermistor reading
        IEC1bits.INT1IE = 1;                                                    //re-enable the INT1 interrupt
        Nop();
        Nop();
        if (LC2CONTROL2.flags2.Interrupt)
        {
            U1MODEbits.UARTEN=1;                                                //Re-enable the COM PORT  
            U1STAbits.UTXEN=1;                                                  
            return;
        }
    }

    //Determine whether 3V or 12V battery is connected

    if (store) {
        mainBatreading = take_analog_reading(87);                               //test the 12V_SENSE input
    } else {
        IEC1bits.INT1IE = 0;                                                    //temporarily disable the INT1 interrupt
        mainBatreading = take_analog_reading(87);
        IEC1bits.INT1IE = 1;                                                    //re-enable the INT1 interrupt
        Nop();
        Nop();
        if (LC2CONTROL2.flags2.Interrupt)
        {
            U1MODEbits.UARTEN=1;                                                //Re-enable the COM PORT  
            U1STAbits.UTXEN=1;                                                  
            return;
        }
    }


    if (mainBatreading < 820)                                                   //12V_SENSE <0.5V, so not connected
    {
        DISPLAY_CONTROL.flags.PS12V = 0;                                        //clear the 12V battery flag
        if (store) 
        {
            mainBatreading = take_analog_reading(97);                           //take the 3V battery reading
        }
        else 
        {
            IEC1bits.INT1IE = 0;                                                //temporarily disable the INT1 interrupt
            mainBatreading = take_analog_reading(97);                           //take internal thermistor reading
            IEC1bits.INT1IE = 1;                                                //re-enable the INT1 interrupt
            Nop();
            Nop();
            if (LC2CONTROL2.flags2.Interrupt)
            {
                U1MODEbits.UARTEN=1;                                            //Re-enable the COM PORT  
                U1STAbits.UTXEN=1;                                                          
                return;
            }
        }
        batteryVoltage = (((Vref * mainBatreading) / 4096) * mul3V);            //convert to voltage    
    } 
    else 
    {
        DISPLAY_CONTROL.flags.PS12V = 1;                                        //set the 12V battery flag
        TEST12V = (((Vref * mainBatreading) / 4096) * mul12V);                  //convert to voltage for test  
        mainBatreading *= -1;                                                   //convert to negative number
        batteryVoltage = (((Vref * mainBatreading) / 4096) * mul12V);           //convert to voltage   
        batteryVoltage = batteryVoltage*-1.0;                                   //convert batteryVoltage to positive for display  
    }

    if (!store) 
    {
        storeTempReading(ch);                                                   //temporary storage for display only ('X' command)
        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//save display flags`
        IEC1bits.INT1IE = 1;                                                    //re-enable the INT1 interrupt
        Nop();
        Nop();
        if (LC2CONTROL2.flags2.Interrupt)                                       //did INT2 interrupt occur during this period?
        {
            U1MODEbits.UARTEN=1;                                                //Re-enable the COM PORT  
            U1STAbits.UTXEN=1;                                                          
            return;                                                             //abort reading if it did
        }

        _3VX_off();                                                             //power-down the analog circuitry

        U1MODEbits.UARTEN=1;                                                    //Re-enable the COM PORT  
        U1STAbits.UTXEN=1;                                                          
        if(!LC2CONTROL2.flags2.Modbus)                                          //If command line interface 
        {
            IEC1bits.INT1IE = 0;                                                //temporarily disable the INT1 interrupt
            displayTempReading();                                               //Display the reading
            IEC1bits.INT1IE = 1;                                                //re-enable the INT1 interrupt
        }
        return;
    }

    if (((!USB_PWR | _232 | !BT_CONNECT) && DISPLAY_CONTROL.flags.PS12V && TEST12V < bat_12MIN) |   //if 12V battery < minimum V or 3V battery < minimum V  
            ((!USB_PWR | _232 | !BT_CONNECT) && !DISPLAY_CONTROL.flags.PS12V && batteryVoltage < bat_3MIN))  
    {
        disableINT1();                                                          //stop logging and shutdown
        disableAlarm(Alarm1);
        shutdown();
    }    
    
    
    IEC1bits.INT1IE = 0;                                                        //temporarily disable the INT1 interrupt
    outputPosition = storeReading(ch);                                          //store the reading and return the outputPosition pointer
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //save display flags    

    IEC1bits.INT1IE = 1;                                                        //re-enable the INT1 interrupt
    Nop();

    _3VX_off();                                                                 //power-down the analog circuitry
 
    U1MODEbits.UARTEN=1;                                                        //Re-enable the COM PORT    
    U1STAbits.UTXEN=1;                                                          

    if (!LC2CONTROL2.flags2.Modbus && LC2CONTROL.flags.Monitor && (USB_PWR | !_232 | BT_CONNECT))  
    {
        IEC1bits.INT1IE = 0;                                                    //temporarily disable the INT1 interrupt
        LC2CONTROL2.flags2.ID = 1;                                              //set the ID flag                            
        displayReading(ch, outputPosition);                                     //display the values stored in external FRAM
        IEC1bits.INT1IE = 1;                                                    //re-enable the INT1 interrupt
    }

    DISPLAY_CONTROL.flags.TakingReading = 0;                                    //clear the TakingReading flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM 
    VWflagsbits.firstReading=0;                                                 
    
    if ((LC2CONTROL.flags.LoggingStopTime && (TotalStopSeconds == seconds_since_midnight)) | //logging scheduled to stop?
            (memoryStatus >= maxSingleVW && outputPosition == 1 && !DISPLAY_CONTROL.flags.WrapMemory)) //stop logging when memory full?
        stopLogging();                                                          //yes

    if(store)                                                                   
        checkSynch(ReadingTimeSeconds);                                         //adjust time of next scheduled reading if necessary
    
    if(store)                                                                   //force shutdown after reading if logging   
        IFS3bits.T9IF=1;
}

float take_reading(unsigned char gageType,int ch)                               //take a reading,ch is channel    
{
    unsigned int *ADC16Ptr;
    unsigned int count=0;                                                       
    unsigned int lithtemp=0;                                                    
    unsigned int F_start=0;                                                     
    unsigned int F_stop=0;                                                      

    float digits = 0.0;
    float lithium = 0.0;

    _3VX_on();                                                                  //power-up analog circuitry 

    if (gageType == 95)                                                         //lithium coin cell reading
    {
        ADPCFG = 0;                                                             //ALL INPUTS are analog channels
        SAMPLE_LITHIUM = 1;                                                     //sample the lithium battery		
        delay(2000);                                                            
        ADC16Ptr = &ADCBUF0;                                                    //intialize ADCBUF pointer
        SAMPLE_LITHIUM = 0;                                                     //turn off lithium battery sampling if on

        for (count = 0; count < 16; count++)                                    //totalize the values from the buffer
        {
            lithtemp = lithtemp + *ADC16Ptr++;
        }

        lithtemp /= 16;                                                         //average the result
        lithium = ((5.0 * lithtemp) / 4096.0);                                  //convert to voltage	
        return lithium;                                                         //return the averaged 12 bit value		
    }

	//Determine and initiate Swept Frequency VW pluck
    if(VWflagsbits.retry)                                                       //wideband sweep
    {
        if (gageType == 1)
            pluck(1400, 3500, 384);                                             //1400-3500 Hz Sweep  

        if (gageType == 2)
            pluck(2800, 4500, 384);                                             //2800-4500 Hz Sweep

        if (gageType == 3)
            pluck(400, 1200, 384);                                              //400-1200 Hz Sweep

        if (gageType == 4)
            pluck(1200, 2800, 384);                                             //1200-2800 Hz Sweep

        if (gageType == 5)
            pluck(2500, 4500, 384);                                             //2500-4500 Hz Sweep
    
        if (gageType == 6)                                                          
            pluck(800, 1600, 384);                                              //800-1600 Hz Sweep    
    }
    else                                                                        //Narrowband sweep
    {
        F=read_Int_FRAM(CH1Reading+(2*(ch-1)));                                 //get the previous whole # reading from FRAM    
        F_start=0.9*F;                                                        
        F_stop=1.1*F;                                                         
        pluck(F_start,F_stop,384);                                              //+/- 10% 
    }
     
    delay(80000);                                                               //30mS delay for PLL settling 
    digits = read_vw();                                                         //get the VW digits
    _3VX_off();                                                                 //power-down analog circuitry   
    return digits;                                                              //give it to take_One_Complete_Reading()
}


void testPoint(unsigned char tp, unsigned char cycles)
{
    int i=0;                                                                    //delay loop index
    int c=0;                                                                    //cycle loop index

    for(c;c<cycles;c++)
    {
        switch(tp)                                                              //bring test point high
        {
            case 1:
                TEST1=1;
                break;
            case 2:
                TEST2=1;
                break;
            default:
                break;
        }
	

        for(i=0;i<10;i++){}                                                     //short delay

        switch(tp)                                                              //bring test point low
        {
            case 1:
                TEST1=0;
                break;
            case 2:
                TEST2=0;
                break;
            default:
                break;
        }

        for(i=0;i<10;i++){}                                                     //short delay
    }

    for(i=0;i<5;i++){}                                                          //spacing between test points

}


void testPoint2(unsigned char cycles) {

    int c = 0; //cycle loop index

    for (c; c < cycles; c++) {
        _READ = 0;                                                              //bring testpoint low
        delay(400);                                                             
        _READ = 1;                                                              //bring testpoint high
        delay(400);                                                             
    }

}

void testReset(void) 
{
    _232SHDN=1;
    
    delay(4000);                                                                
    
    if (RCONbits.TRAPR) 
    {
        RCONbits.TRAPR = 0;                                                     //clear the flag if set
        if(OSCCONbits.CF)                                                       //Clock failure 
        {
            OSCCONbits.CF=0;
            putsUART1(oscfail);                                                 //    "Oscillator Failure has occurred."
        }
        else
        {    
            putsUART1(trapr);                                                   //    "A Trap Conflict Reset has occurred."
        }
        while (BusyUART1());
        crlf();
    }

    if (RCONbits.IOPUWR) 
    {
        RCONbits.IOPUWR = 0;                                                    //clear the flag if set
        putsUART1(iopuwr);                                                      //    "An Illegal Opcode, Illegal Address or Uninitialized W has occurred."
        while (BusyUART1());
        crlf();
    }

    if (RCONbits.EXTR) 
    {
        RCONbits.EXTR = 0;                                                      //clear the flag if set
        putsUART1(extr);                                                        //    "A Master Clear (pin) Reset has occurred."
        while (BusyUART1());
        crlf();
    }

    if (RCONbits.SWR) 
    {
        RCONbits.SWR = 0;                                                       //clear the flag if set
        putsUART1(swr);                                                         //    "A RESET Instruction has been executed."
        while (BusyUART1());
        crlf();
    }

    if (RCONbits.WDTO) 
    {
        RCONbits.WDTO = 0;                                                      //clear the flag if set
        putsUART1(wdto);                                                        //    "WDT Timeout has occurred."
        while (BusyUART1());
        crlf();
    }

    if (RCONbits.SLEEP) 
    {
        RCONbits.SLEEP = 0;                                                     //clear the flag if set        
        putsUART1(sleep);                                                       //    "Device has been in Sleep mode."
        while (BusyUART1());
        crlf();
    }

    if (RCONbits.BOR) 
    {
        RCONbits.BOR = 0;                                                       //clear the flag if set        
        putsUART1(bor);                                                         //    "A Brown-out Reset has occurred."
        while (BusyUART1());
        crlf();
    }

    if (RCONbits.POR) 
    {
        RCONbits.POR = 0;                                                       //clear the flag if set        
        putsUART1(por);                                                         //    "A Power-up Reset has occurred."
        while (BusyUART1());
        crlf();
    }
    
    _232SHDN=0;
}

void toBCD(unsigned char value) {
    if (value >= 0 && value <= 9)                                               //convert values 0-9 to 2 digit BCD
    {
        BCDtens = 0;
        BCDones = value;
        return;
    }

    if (value >= 10 && value <= 19)                                             //convert values 10-19 to 2 digit BCD
    {
        BCDtens = 1;
        BCDones = value - 10;
        return;
    }

    if (value >= 20 && value <= 29)                                             //convert values 20-29 to 2 digit BCD
    {
        BCDtens = 2;
        BCDones = value - 20;
        return;
    }

    if (value >= 30 && value <= 39)                                             //convert values 30-39 to 2 digit BCD
    {
        BCDtens = 3;
        BCDones = value - 30;
        return;
    }

    if (value >= 40 && value <= 49)                                             //convert values 40-49 to 2 digit BCD
    {
        BCDtens = 4;
        BCDones = value - 40;
        return;
    }

    if (value >= 50 && value <= 59)                                             //convert values 50-59 to 2 digit BCD
    {
        BCDtens = 5;
        BCDones = value - 50;
        return;
    }

    if (value >= 60 && value <= 69)                                             //convert values 60-69 to 2 digit BCD
    {
        BCDtens = 6;
        BCDones = value - 60;
        return;
    }

    if (value >= 70 && value <= 79)                                             //convert values 70-79 to 2 digit BCD
    {
        BCDtens = 7;
        BCDones = value - 70;
        return;
    }

    if (value >= 80 && value <= 89)                                             //convert values 80-89 to 2 digit BCD
    {
        BCDtens = 8;
        BCDones = value - 80;
        return;
    }

    if (value >= 90 && value <= 99)                                             //convert values 90-99 to 2 digit BCD
    {
        BCDtens = 9;
        BCDones = value - 90;
        return;
    }


}

unsigned int toJulian(void)                                                     //convert mm/dd to day of year (julian) with 
{                                                                               //leap year correction
    unsigned int DayOfYear;
    unsigned int offset;


    LC2CONTROL.flags.LeapYear = 0;                                              //clear the leap year flag

    if (year % 4 == 0)                                                          //is It a leap year?
        LC2CONTROL.flags.LeapYear = 1;                                          //set flag if it is

    if (!LC2CONTROL.flags.LeapYear) {
        switch (month)                                                          //determine day of year offset - not a leap year
        {
            case 1:
                offset = 0;
                break;
            case 2:
                offset = 31;
                break;
            case 3:
                offset = 59;
                break;
            case 4:
                offset = 90;
                break;
            case 5:
                offset = 120;
                break;
            case 6:
                offset = 151;
                break;
            case 7:
                offset = 181;
                break;
            case 8:
                offset = 212;
                break;
            case 9:
                offset = 243;
                break;
            case 10:
                offset = 273;
                break;
            case 11:
                offset = 304;
                break;
            case 12:
                offset = 334;
                break;
            default:
                return;
                break;
        }                                                                       //end of switch(RTCmonths)
    } else {
        switch (month)                                                          //determine day of year offset - leap year
        {
            case 1:
                offset = 0;
                break;
            case 2:
                offset = 31;
                break;
            case 3:
                offset = 60;
                break;
            case 4:
                offset = 91;
                break;
            case 5:
                offset = 121;
                break;
            case 6:
                offset = 152;
                break;
            case 7:
                offset = 182;
                break;
            case 8:
                offset = 213;
                break;
            case 9:
                offset = 244;
                break;
            case 10:
                offset = 274;
                break;
            case 11:
                offset = 305;
                break;
            case 12:
                offset = 335;
                break;
            default:
                return;
                break;
        }                                                                       //end of switch(RTCmonths)
    }

    DayOfYear = day + offset;
    return DayOfYear;                                                           //exit and return day of year
}

unsigned char toMonthDay(unsigned int julian, unsigned int year, unsigned char x) //x:0=day,1=month
{

    if (year % 4 == 0)                                                          //leap year?
        LC2CONTROL.flags.LeapYear = 1;                                          //set the leap year flag
    else
        LC2CONTROL.flags.LeapYear = 0;                                          //if not, clear the leap year flag

    if (julian >= 1 && julian <= 31)                                            //January
    {
        if (x)
            return 1;                                                           //return month
        else
            return julian;                                                      //return day
    }

    if (!LC2CONTROL.flags.LeapYear)                                             //Not a Leap Year
    {
        if (julian >= 32 && julian <= 59)                                       //February
        {
            if (x)
                return 2;                                                       //return month
            else
                return julian - 31;                                             //return day
        }

        if (julian >= 60 && julian <= 90)                                       //March
        {
            if (x)
                return 3;                                                       //return month
            else
                return julian - 59;                                             //return day
        }

        if (julian >= 91 && julian <= 120)                                      //April
        {
            if (x)
                return 4;                                                       //return month
            else
                return julian - 90;                                             //return day
        }

        if (julian >= 121 && julian <= 151)                                     //May
        {
            if (x)
                return 5;                                                       //return month
            else
                return julian - 120;                                            //return day
        }

        if (julian >= 152 && julian <= 181)                                     //June
        {
            if (x)
                return 6;                                                       //return month
            else
                return julian - 151;                                            //return day
        }

        if (julian >= 182 && julian <= 212)                                     //July
        {
            if (x)
                return 7;                                                       //return month
            else
                return julian - 181;                                            //return day
        }

        if (julian >= 213 && julian <= 243)                                     //August
        {
            if (x)
                return 8;                                                       //return month
            else
                return julian - 212;                                            //return day
        }

        if (julian >= 244 && julian <= 273)                                     //September
        {
            if (x)
                return 9;                                                       //return month
            else
                return julian - 243;                                            //return day
        }

        if (julian >= 274 && julian <= 304)                                     //October
        {
            if (x)
                return 10;                                                      //return month
            else
                return julian - 273;                                            //return day
        }

        if (julian >= 305 && julian <= 334)                                     //November
        {
            if (x)
                return 11;                                                      //return month
            else
                return julian - 304;                                            //return day
        }

        if (julian >= 335 && julian <= 365)                                     //December
        {
            if (x)
                return 12;                                                      //return month
            else
                return julian - 334;                                            //return day
        }

    } else                                                                      //Leap Year
    {
        if (julian >= 32 && julian <= 60)                                       //February
        {
            if (x)
                return 2;                                                       //return month
            else
                return julian - 31;                                             //return day
        }

        if (julian >= 61 && julian <= 91)                                       //March
        {
            if (x)
                return 3;                                                       //return month
            else
                return julian - 60;                                             //return day
        }

        if (julian >= 92 && julian <= 121)                                      //April
        {
            if (x)
                return 4;                                                       //return month
            else
                return julian - 91;                                             //return day
        }

        if (julian >= 122 && julian <= 152)                                     //May
        {
            if (x)
                return 5;                                                       //return month
            else
                return julian - 121;                                            //return day
        }

        if (julian >= 153 && julian <= 182)                                     //June
        {
            if (x)
                return 6;                                                       //return month
            else
                return julian - 152;                                            //return day
        }

        if (julian >= 183 && julian <= 213)                                     //July
        {
            if (x)
                return 7;                                                       //return month
            else
                return julian - 182;                                            //return day
        }

        if (julian >= 214 && julian <= 244)                                     //August
        {
            if (x)
                return 8;                                                       //return month
            else
                return julian - 213;                                            //return day
        }

        if (julian >= 245 && julian <= 274)                                     //September
        {
            if (x)
                return 9;                                                       //return month
            else
                return julian - 244;                                            //return day
        }

        if (julian >= 275 && julian <= 305)                                     //October
        {
            if (x)
                return 10;                                                      //return month
            else
                return julian - 274;                                            //return day
        }

        if (julian >= 306 && julian <= 335)                                     //November
        {
            if (x)
                return 11;                                                      //return month
            else
                return julian - 305;                                            //return day
        }

        if (julian >= 336 && julian <= 366)                                     //December
        {
            if (x)
                return 12;                                                      //return month
            else
                return julian - 335;                                            //return day
        }
    }
}

void unpack(unsigned int data) {
    Lbyte = data;                                                               //get LSB
    data &= 0xFF00;                                                             //mask LSB
    data /= 256;                                                                //right shift data 8 bits
    Hbyte = data;                                                               //get MSB
}

void upD8RTCAlarm1(void) 
{
    unsigned long CurrentTimeSeconds = 0;
    unsigned long ScanInterval = 0;
    unsigned long NewTime = 0;

    CurrentTimeSeconds = RTChms2s(1);                                           //get the current time from the RTC

    if (LC2CONTROL.flags.LogInterval)                                           //Log Intervals enabled?
    {                                                                           //yes

        if (MUX4_ENABLE.mflags.mux16_4 != Single)                               //if multichannel   
        {
            LogIt1=read_Int_FRAM(LogIt1address);                                //check if Log iterations=0
            LogIt2=read_Int_FRAM(LogIt2address);                                //check if Log iterations=0
            LogIt3=read_Int_FRAM(LogIt3address);                                //check if Log iterations=0
            LogIt4=read_Int_FRAM(LogIt4address);                                //check if Log iterations=0
            LogIt5=read_Int_FRAM(LogIt5address);                                //check if Log iterations=0
            LogIt6=read_Int_FRAM(LogIt6address);                                //check if Log iterations=0

            LogItRemain1=read_Int_FRAM(LogItRemain1address);                    //get the remaining intervals in 1
            LogItRemain2=read_Int_FRAM(LogItRemain2address);                    //get the remaining intervals in 2
            LogItRemain3=read_Int_FRAM(LogItRemain3address);                    //get the remaining intervals in 3
            LogItRemain4=read_Int_FRAM(LogItRemain4address);                    //get the remaining intervals in 4
            LogItRemain5=read_Int_FRAM(LogItRemain5address);                    //get the remaining intervals in 5
            LogItRemain6=read_Int_FRAM(LogItRemain6address);                    //get the remaining intervals in 6
         } 
        else                                                                    //if Single Channel
        {
            LogIt1=read_Int_FRAM(SingleLogIt1address);                          //check if Log iterations=0
            LogIt2=read_Int_FRAM(SingleLogIt2address);                          //check if Log iterations=0
            LogIt3=read_Int_FRAM(SingleLogIt3address);                          //check if Log iterations=0
            LogIt4=read_Int_FRAM(SingleLogIt4address);                          //check if Log iterations=0
            LogIt5=read_Int_FRAM(SingleLogIt5address);                          //check if Log iterations=0
            LogIt6=read_Int_FRAM(SingleLogIt6address);                          //check if Log iterations=0

            LogItRemain1=read_Int_FRAM(SingleLogItRemain1address);              //get the remaining intervals in 1
            LogItRemain2=read_Int_FRAM(SingleLogItRemain2address);              //get the remaining intervals in 2
            LogItRemain3=read_Int_FRAM(SingleLogItRemain3address);              //get the remaining intervals in 3
            LogItRemain4=read_Int_FRAM(SingleLogItRemain4address);              //get the remaining intervals in 4
            LogItRemain5=read_Int_FRAM(SingleLogItRemain5address);              //get the remaining intervals in 5
            LogItRemain6=read_Int_FRAM(SingleLogItRemain6address);              //get the remaining intervals in 6
         }


        //********************************LOG INTERVAL 1*****************************************
        while (LogIt1 == 0)                                                     //if 0, log indefinately at this Scan Interval	
        {
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength1address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single)                           //if single channel    
                ScanInterval=read_longFRAM(SingleLogIntLength1address);       
            break;
        }

        while (LogItRemain1>-1)                                                 //in interval 1?
        {
            LogItRemain1 -= 1;                                                  //yes, so decrement the remaining intervals

            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain1address,LogItRemain1);               //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) //VER 6.0.7
                write_Int_FRAM(SingleLogItRemain1address,LogItRemain1);         //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength1address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) //VER 6.0.7
                ScanInterval=read_longFRAM(SingleLogIntLength1address);       
            break;
        }

        //********************************LOG INTERVAL 2*****************************************
        while (LogIt2 == 0 && LogItRemain1 == -1 && LogIt1 != 0)                //if 0, log indefinately at this Scan Interval 
        {
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength2address);    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) //VER 6.0.7
                ScanInterval=read_longFRAM(SingleLogIntLength2address);       
            break;
        }

        while (LogItRemain2>-1 && LogItRemain1 == -1 && LogIt1 != 0)            //in interval 2?	
        {
            LogItRemain2 -= 1;                                                  //yes, so decrement the remaining intervals

            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain2address,LogItRemain2);               //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                write_Int_FRAM(SingleLogItRemain2address,LogItRemain2);         //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength2address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength2address);       
            break;
        }

        //********************************LOG INTERVAL 3*****************************************
        while (LogIt3 == 0 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0) //if 0, log indefinately at this Scan Interval	
        {
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength3address);    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength3address);       
            break;
        }

        while (LogItRemain3>-1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0) //in interval 3?	
        {
            LogItRemain3 -= 1;                                                  //yes, so decrement the remaining intervals
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain3address,LogItRemain3);               //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                write_Int_FRAM(SingleLogItRemain3address,LogItRemain3);          //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength3address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength3address);       
            break;
        }

        //********************************LOG INTERVAL 4*****************************************
        while (LogIt4 == 0 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0) //if 0, log indefinitely at this Scan Interval	
        {
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength4address);    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength4address);       
            break;
        }

        while (LogItRemain4>-1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0)//in interval 4?	
        {
            LogItRemain4 -= 1;                                                  //yes, so decrement the remaining intervals
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain4address,LogItRemain4);               //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                write_Int_FRAM(SingleLogItRemain4address,LogItRemain4);         //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength4address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength4address);       
            break;
        }

        //********************************LOG INTERVAL 5*****************************************
        while (LogIt5 == 0 && LogItRemain4 == -1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0 && LogIt4 != 0) //if 0, log indefinately at this Scan Interval	
        {
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength5address);    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength5address);       
            break;
        }

        while (LogItRemain5>-1 && LogItRemain4 == -1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0 && LogIt4 != 0) //in interval 5?	
        {
            LogItRemain5 -= 1;                                                  //yes, so decrement the remaining intervals
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain5address,LogItRemain5);               //store it to FRAM`
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                write_Int_FRAM(SingleLogItRemain5address,LogItRemain5);          //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength5address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength5address);       
            break;
        }

        //********************************LOG INTERVAL 6*****************************************
        while (LogIt6 == 0 && LogItRemain5 == -1 && LogItRemain4 == -1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0 && LogIt4 != 0 && LogIt5 != 0) //if 0, log indefinately at this Scan Interval	 
        {
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength6address);    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength6address);        
            break;
        }

        if (MUX4_ENABLE.mflags.mux16_4 != Single)                               //if multichannel   
            LogItRemain6=read_Int_FRAM(LogItRemain6address);                    //get the remaining intervals
        if (MUX4_ENABLE.mflags.mux16_4 == Single) 
            LogItRemain6=read_Int_FRAM(SingleLogItRemain6address);              //get the remaining intervals   

        while (LogItRemain6>-1 && LogItRemain5 == -1 && LogItRemain4 == -1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0 && LogIt4 != 0 && LogIt5 != 0)//in interval 6?	
        {
            LogItRemain6 -= 1;                                                  //yes, so decrement the remaining intervals
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain6address,LogItRemain6);               //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                write_Int_FRAM(SingleLogItRemain6address,LogItRemain6);         //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength6address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength6address);       
            break;
        }

        //*******************************LOG INTERVALS COMPLETE**********************************	
        if (LogItRemain6 == -1 && LogItRemain5 == -1 && LogItRemain4 == -1 && LogItRemain3 == -1 && 
                LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && 
                LogIt3 != 0 && LogIt4 != 0 && LogIt5 != 0 && LogIt6 != 0) 
        {
            stopLogging();                                                      //Stop Logging
            reloadLogTable();                                                   
            return;
        }

    } 
    else 
    {                                                                           //no
        ScanInterval = hms2s();
    }

    if (!(86400 % ScanInterval) && VWflagsbits.synch && DISPLAY_CONTROL.flags.Synch && !LC2CONTROL.flags.LoggingStartTime) //if scan interval is evenly divisible into
    {                                                                           //86400 and the first reading is being taken and the Synch flag is set
        NewTime = synch(CurrentTimeSeconds, ScanInterval);
    } else {
        NewTime = CurrentTimeSeconds + ScanInterval;
    }

    if (NewTime <= CurrentTimeSeconds)
        NewTime = CurrentTimeSeconds + ScanInterval;                            //In case synch'd time is current time or before current time

    if (LC2CONTROL2.flags2.FirstReading)                                        //is it the first reading?
    {
        if (MUX4_ENABLE.mflags.mux16_4 == VW32 | MUX4_ENABLE.mflags.mux16_4 == VW16) //32 or 16 channel mux    
        {
            if (NewTime <= (CurrentTimeSeconds + 20))
                NewTime = NewTime + ScanInterval;                               //to make sure reading isn't skipped due to processing time
        }

        if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == TH8) //4 or 8 channel mux     
        {
            if (NewTime <= (CurrentTimeSeconds + 6))
                NewTime = NewTime + ScanInterval;                               //to make sure reading isn't skipped due to processing time
        }

        if (MUX4_ENABLE.mflags.mux16_4 == TH32 | MUX4_ENABLE.mflags.mux16_4 == VW8) //8VW or 32TH channel mux    
        {
            if (NewTime <= (CurrentTimeSeconds + 10))
                NewTime = NewTime + ScanInterval;                               //to make sure reading isn't skipped due to processing time
        }

        if (MUX4_ENABLE.mflags.mux16_4 == Single)                               //single channel     
        {
            if (NewTime == CurrentTimeSeconds)
                NewTime = CurrentTimeSeconds + ScanInterval;                    //In case synch'd time is current time

            if (NewTime == (CurrentTimeSeconds + 1))                            //To make sure reading isn't skipped due
                NewTime = NewTime + ScanInterval;                               //to processing time
        }

        LC2CONTROL2.flags2.FirstReading = 0;                                    //clear the first reading flag 
        write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);              //store flags in FRAM 
    }


    if (NewTime >= 86400)                                                       //midnight rollover?
        NewTime -= 86400;                                                       //compensate

    hms(NewTime, 1);                                                            //update the RTC Alarm1 register
}

//***************************************************************************
//							V_HT2C()
//
//	Converts high temp thermistor voltage to degrees C via Steinhart-Hart Log Equation
//
//	Parameters received: thermistor voltage, channel
//	Returns: temperature in degrees C
//
//
//***************************************************************************
float V_HT2C(float V, unsigned int ch)                                          
{
	double a=0.0;						
	double b=0.0;
	double c=0.0;
	double d=0.0;
	const double R5k=5000.0;
	const double R1k=1000.0;
	double I=0.0;
	double T=0.0;
	double V1k=0.0;
	double Vht=0.0;
	double V5k=0.0;
	double Rht=0.0;
	double LogRht=0.0;
	float T32bit=0.0;
    unsigned long Thermtypeaddress=0;                                           

    Thermtypeaddress=CH1THaddress + (2*(ch-1));                                 
	Thermtype=read_Int_FRAM(Thermtypeaddress);

    if(Thermtype==1)                                                            
	{
		a=1.4051E-3;                                                            //coefficients for standard therm
		b=2.369E-4;
		c=1.019E-7;
	}
	else
    if(Thermtype==2)                                                            
	{
		a=1.02569E-3;                                                           //coefficients for high temp BR55KA822J therm
		b=2.478265E-4;
		c=1.289498E-7;
	}
	else
    if(Thermtype==3)                                                                
	{
		a=1.12766979300187E-3;                                                  //coefficients for high temp 103JL1A therm
		b=2.34444184128213E-4;
		c=8.47692130592308E-8;
		d=1.17512193579615E-11;	
	}

    
    
//	Convert 32 bit float V to 64 bit double V5k:                                
	V5k=V;

//	Determine thermistor loop current (I):
	I=V5k/R5k;															

//	Determine voltage dropped across internal 1K .1% resistor (V1k):
	V1k=R1k*I;

//	Determine voltage across external high temp thermistor (Vht):
	Vht=Vref-V1k-V5k;

//	Determine resistance of external high temp thermistor (Rht):
	Rht=Vht/I;

//	Get the natural log of the Rht:
	LogRht=log(Rht);

    if(Thermtype==1 | Thermtype==2)                                             
	{
	//	Plug LogRht into Steinhart-Hart Log Equation and return temperature(C):
		T=(1/(a+(b*LogRht)+(c*(LogRht*LogRht*LogRht))))-273.2;
	}

    if(Thermtype==3)                                                            
	{
	//	Plug LogRht into Steinhart-Hart Log Equation and return temperature(C):
		T=(1/(a+(b*LogRht)+(c*(LogRht*LogRht*LogRht)+(d*(LogRht*LogRht*LogRht*LogRht*LogRht)))))-273.2;
	}
	//	Convert T to 32 bit float:
		T32bit=T;

	//	Return the 32bit temperature value (C):
		return T32bit;
}

unsigned int vwf32toINT16(float value)                                          
{
	int chars=0;
	char decBUF[8];
	unsigned char ones=0;
	unsigned char tens=0;
	unsigned char hundreds=0;
    unsigned char thousands=0;
    unsigned char tenthousands=0;
	DEC_VWF.vwf.whole=0;

    for(chars=0;chars<8;chars++)
    {
        decBUF[chars]=0;                                                        //initialize buffer
    }
    
	chars=sprintf(decBUF,"%.0f",value);                                         //convert float value to ascii string	

    if(chars==3)
    {
        ones=decBUF[chars-1]-0x30;                                              //extract ones
		tens=decBUF[chars-2]-0x30;                                              //extract tens
		hundreds=decBUF[chars-3]-0x30;                                          //extract hundreds
    }
    if(chars==4)
    {
        ones=decBUF[chars-1]-0x30;                                              //extract ones
		tens=decBUF[chars-2]-0x30;                                              //extract tens
		hundreds=decBUF[chars-3]-0x30;                                          //extract hundreds    
		thousands=decBUF[chars-4]-0x30;                                         //extract thousands
    }
    if(chars==5)
    {
        ones=decBUF[chars-1]-0x30;                                              //extract ones
		tens=decBUF[chars-2]-0x30;                                              //extract tens
		hundreds=decBUF[chars-3]-0x30;                                          //extract hundreds    
		thousands=decBUF[chars-4]-0x30;                                         //extract thousands
        tenthousands=decBUF[chars-5]-0x30;                                      //extract tenthousands
    }    

	DEC_VWF.vwf.whole=10000*tenthousands+1000*thousands+100*hundreds+10*tens+ones;	//store whole number value in DEC_TEMP
	return DEC_VWF.decimalvw;                                                   //return the 16 bit value
}

void wait(void) 
{
    unsigned char t = 0;

    ClrWdt();                                                                 
    WDTSWEnable;                                                                //Start WDT    

    while (!t) {
        RxData = ReadUART1();

        if (RxData == xon)
            t = 1;
    }
    WDTSWDisable;                                                               //Stop WDT    
}

void wait2S(void) 
{
    unsigned long point1S = 294912;                                             //delay Tcy value for 0.1S                    
    unsigned int i = 0;

    Blink(1);                                                                   
    for (i = 0; i < 20; i++)                                                    //delay 2S and blink LED
    {
        delay(point1S);
    }


}

void wrap_one(void)                                                             
{
    shutdownTimer(TimeOut);                                                     //Reset 15S timer   
    DISPLAY_CONTROL.flags.WrapMemory = 1;                                       //set the wrap memory flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM
    S_1.status1flags._Wrap=1;                                                   //Set the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);          
}

void wrap_stop(void) 
{                                                                               //memory full
    outputPosition = 1;                                                         //reset outputPosition pointer
    if (!DISPLAY_CONTROL.flags.WrapMemory)                                      //is logging to stop when memory is full
    {
        stopLogging();                                                          //YES
    }
}

void wrap_zero(void)                                                            
{
    shutdownTimer(TimeOut);                                                     //Reset 15S timer   
    DISPLAY_CONTROL.flags.WrapMemory = 0;                                       //clear the wrap memory flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM 
    S_1.status1flags._Wrap=0;                                                   //Clear the MODBUS status flag    
    write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);    
}

void WRITE_TIME(void)                                                           
{
    unsigned int    clockdata=0;
    
    clockdata=read_Int_FRAM(RTCYearsaddress);                                   //get the years
    RTCyears=clockdata;
    setClock(RTCYearsAddress, RTCyears);                                        //load it into the RTC
    
    clockdata=read_Int_FRAM(RTCMonthsaddress);                                  //get the months
    RTCmonths=clockdata;
    setClock(RTCMonthsAddress, RTCmonths);                                      //load it into the RTC
    
    clockdata=read_Int_FRAM(RTCDaysaddress);                                    //get the days
    RTCdays=clockdata;
    setClock(RTCDaysAddress, RTCdays);                                          //load it into the RTC

    clockdata=read_Int_FRAM(RTCHoursaddress);                                   //get the hours
    RTChours=clockdata;
    setClock(RTCHoursAddress, RTChours);                                        //load it into the RTC
    
    clockdata=read_Int_FRAM(RTCMinutesaddress);                                 //get the minutes
    RTCminutes=clockdata;
    setClock(RTCMinutesAddress, RTCminutes);                                    //load it into the RTC
    
    clockdata=read_Int_FRAM(RTCSecondsaddress);                                 //get the seconds
    RTCseconds=clockdata;
    setClock(RTCSecondsAddress, RTCseconds);                                    //load it into the RTC
}

void X(void)                                                                    
{
    LC2CONTROL2.flags2.X = 1;                                                   //set the 'X' flag
    LC2CONTROL2.flags2.Interrupt = 0;                                           //clear the INT2 interrupt flag	
    VWflagsbits.firstReading=1;                                                 
    take_One_Complete_Reading(NOSTORE);                                         //take a reading    
    VWflagsbits.firstReading=0;                                                 
    LC2CONTROL2.flags2.X = 0;                                                   //clear the 'X' flag	

    INTCON1bits.NSTDIS = 0;                                                     //Reset nesting of interrupts 

    RxData = ReadUART1();                                                       //clear the UART buffer	
    RxData = ReadUART1();                                                       //clear the UART buffer	
    RxData = ReadUART1();                                                       //clear the UART buffer	
    RxData = ReadUART1();                                                       //clear the UART buffer
    RxData = cr;                                                                //load <CR> into RxData	
}


//----------------------------INTERRUPTS----------------------------------------

//***************************UNUSED PRIMARY INTERRUPT VECTORS***********************************************
void __attribute__((__interrupt__)) _INT0Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _IC1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _OC1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _T1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DMA0Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _IC2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _OC2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _T2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _T3Interrupt(void) 
{
    asm("RESET");
}

//void __attribute__((__interrupt__)) _SPI1EInterrupt(void) 
//{
//    asm("RESET");
//}

void __attribute__((__interrupt__)) _SPI1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _U1TXInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _ADC1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DMA1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _SI2C1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _MI2C1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _CNInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _ADC2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _IC7Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _IC8Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DMA2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _OC3Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _OC4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _T4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _INT2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _U2RXInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _U2TXInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _SPI2ErrInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _SPI2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _C1RxRdyInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _C1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DMA3Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _IC3Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _IC4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _IC5Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _IC6Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _OC5Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _OC6Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _OC7Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _OC8Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DMA4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _T6Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _SI2C2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _MI2C2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _T8Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _T9Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _INT3Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _INT4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _C2RxRdyInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _C2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DCIErrInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DCIInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DMA5Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _U1ErrInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _U2ErrInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DMA6Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _DMA7Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _C1TxReqInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _C2TxReqInterrupt(void) 
{
    asm("RESET");
}


//***************************UNUSED ALTERNATE INTERRUPT VECTORS***********************************************
void __attribute__((__interrupt__)) _AltINT0Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltIC1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltOC1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltT1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDMA0Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltIC2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltOC2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltT2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltT3Interrupt(void) 
{
    asm("RESET");
}

//void __attribute__((__interrupt__)) _AltSPI1EInterrupt(void) 
//{
//    asm("RESET");
//}

void __attribute__((__interrupt__)) _AltSPI1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltU1TXInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltADC1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDMA1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltSI2C1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltMI2C1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltCNInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltADC2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltIC7Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltIC8Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDMA2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltOC3Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltOC4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltT4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltINT2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltU2RXInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltU2TXInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltSPI2ErrInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltSPI2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltC1RxRdyInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltC1Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDMA3Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltIC3Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltIC4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltIC5Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltIC6Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltOC5Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltOC6Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltOC7Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltOC8Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDMA4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltT6Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltSI2C2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltMI2C2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltT8Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltT9Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltINT3Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltINT4Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltC2RxRdyInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltC2Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDCIErrInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDCIInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDMA5Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltU1ErrInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltU2ErrInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDMA6Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltDMA7Interrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltC1TxReqInterrupt(void) 
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltC2TxReqInterrupt(void) 
{
    asm("RESET");
}




//***************************INTERRUPT SERVICE ROUTINES***********************************************

void __attribute__((__interrupt__)) _U1RXInterrupt(void)                        //This is the UART1 Receive ISR 
{
    IFS0bits.U1RXIF = 0;                                                        //clear the UART1 interrupt flag
    U1STAbits.OERR = 0;
    RxData = ReadUART1();                                                       //get the char

    
    if (RxData == xoff)                                                         //Xoff received
    {
        wait();
    }
    if (RxData == escape)                                                       //does the user want to stop the data download?
    {
        DISPLAY_CONTROL.flags.bail = 1;                                         //Set the bail flag
    }

}

void __attribute__((__interrupt__)) _AltU1RXInterrupt(void)                     //This is the UART1 ALTERNATE Receive ISR 
{
    IFS0bits.U1RXIF = 0;                                                        //clear the UART1 interrupt flag
    U1STAbits.OERR = 0;
    RxData = ReadUART1();                                                       //get the char

    
    if (RxData == xoff)                                                         //Xoff received
    {
        wait();
    }
    if (RxData == escape)                                                       //does the user want to stop the data download?
    {
        DISPLAY_CONTROL.flags.bail = 1;                                         //Set the bail flag
    }

}

void __attribute__((__interrupt__)) _INT1Interrupt(void)                        //This is the RTC ISR when time to read occurs  
{
    unsigned char   tempRTC;                                                    

    IFS1bits.INT1IF = 0;                                                        //clear the interrupt flag                             

    INTCON1bits.NSTDIS = 0;
    if(SRbits.IPL==7)
        SRbits.IPL=5;
    
    SLEEP12V = 0;
    validRTC = debounce();
    
    if (validRTC)                                                               //if valid interrupt
    {
        PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress);           //get the PORT_CONTROL flags  
        intSource = readClock(0x0F);                                            //get the RTC Alarm flags		
        intSource &= 0x03;                                                      //strip off bits 7-2    
        
        if(intSource == 0x01 | intSource == 0x03)                               //Alarm 1 
        {
            tempRTC=readClock(0x0f);                                            //clear the RTC A1F 
            tempRTC&=0xfe;                                                      
            setClock(0x0f,tempRTC);                                             
            LC2CONTROL2.flags2.Interrupt = 1;                                   //set the INT2 interrupt flag
            MUX_CLOCK = 0;                                                      //set the MUX_CLOCK line low
            MUX_RESET = 0;                                                      //set the MUX_RESET line low
            disableAlarm(Alarm1);
            LC2CONTROL2.flags2.Waiting = 0;                                     //clear the Waiting flag
            write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);          //store flags in FRAM 
            T4CONbits.TON = 0;
            enableAlarm(Alarm1);
            tempTMR4 = TMR4;                                                    //save TMR4/5 registers
            tempTMR5 = TMR5HLD;
            take_One_Complete_Reading(STORE);
            TMR5HLD = tempTMR5;                                                 //restore TMR4/5 registers
            TMR4 = tempTMR4;
            if (LC2CONTROL.flags.USBpower)                                      
                T4CONbits.TON = 1;                                              //turn Timer4/5 back on
            U1STAbits.OERR = 0;                                                 //clear flag if overrun error
            PMD3bits.T9MD=0;                                                    //Make sure TMR9 module is enabled  
            IFS3bits.T9IF = 1;
        }        

        if(intSource == 0x02 | intSource == 0x03)                               //Alarm 2 
        {
            tempRTC=readClock(0x0f);                                            //clear the RTC A2F 
            tempRTC&=0xfd;                                                      
            setClock(0x0f,tempRTC);                                             
            disableAlarm(Alarm2);

            if (PORT_CONTROL.flags.PortTimerEN && (!PORT_CONTROL.flags.ControlPortON | !PORT_CONTROL.flags.CPTime)) //PORT was OFF or O1 was previously issued    
            {
                CONTROL = 1;                                                    //Turn PORT ON
                _READ=0;                                                        //light LED 
                Nop();
                PORT_CONTROL.flags.ControlPortON = 1;                           //set flag
                PORT_CONTROL.flags.CPTime = 1;                                  //set flag
                S_1.status1flags._OP=1;                                         //set the MODBUS status flag    
                S_1.status1flags._OP_Timer=1;                                   //set the MODBUS status flag    
                write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);

                PortOffHours=read_Int_FRAM(PortOffHoursaddress);                //write Port OFF hours to RTC 
                setClock(RTCAlarm2HoursAddress, PortOffHours);
                PortOffMinutes=read_Int_FRAM(PortOffMinutesaddress);            //write Port OFF minutes to RTC   
                setClock(RTCAlarm2MinutesAddress, PortOffMinutes);
            } else
            if (PORT_CONTROL.flags.PortTimerEN &&(PORT_CONTROL.flags.ControlPortON | PORT_CONTROL.flags.O0issued | PORT_CONTROL.flags.CPTime)) //PORT was ON,O0 was previously issued or CPTime in process 
            {
                CONTROL = 0;                                                    //Turn PORT OFF
                _READ=1;                                                        //OFF LED 
                PORT_CONTROL.flags.ControlPortON = 0;                           //clear flag
                PORT_CONTROL.flags.O0issued = 0;                                //clear flag
                PORT_CONTROL.flags.CPTime = 0;                                  //clear flag
                S_1.status1flags._OP=0;                                         //clear the MODBUS status flag    
                S_1.status1flags._OP_Timer=0;                                   //clear the MODBUS status flag    
                write_Int_FRAM(MODBUS_STATUS1address,S_1.status1);                
                PortOnHours=read_Int_FRAM(PortOnHoursaddress);                  //write Port ON hours to RTC  
                setClock(RTCAlarm2HoursAddress, PortOnHours);
                PortOnMinutes=read_Int_FRAM(PortOnMinutesaddress);              //write Port ON minutes to RTC    
                setClock(RTCAlarm2MinutesAddress, PortOnMinutes);
            }
            else                                                                
            if (PORT_CONTROL.flags.BluetoothTimerEN && (!PORT_CONTROL.flags.BluetoothON | !PORT_CONTROL.flags.BTTime))   //Bluetooth was OFF or BT1 was previously issued
            {
                enableBT();                                                     //Turn Bluetooth ON
                PORT_CONTROL.flags.BluetoothON = 1;                             //set flag
                PORT_CONTROL.flags.BTTime = 1;                                  //set flag

                PortOffHours=read_Int_FRAM(PortOffHoursaddress);                //write Port OFF hours to RTC 
                setClock(RTCAlarm2HoursAddress, PortOffHours);
                PortOffMinutes=read_Int_FRAM(PortOffMinutesaddress);            //write Port OFF minutes to RTC   
                setClock(RTCAlarm2MinutesAddress, PortOffMinutes);
            } else
            if (PORT_CONTROL.flags.BluetoothTimerEN && (PORT_CONTROL.flags.BluetoothON | PORT_CONTROL.flags.B0issued | PORT_CONTROL.flags.BTTime)) //Bluetooth was ON,B0 was previously issued or BTTime in process
            {
                disableBT();                                                    //Turn Bluetooth OFF    
                PORT_CONTROL.flags.BluetoothON = 0;                             //clear flag
                PORT_CONTROL.flags.B0issued = 0;                                //clear flag
                PORT_CONTROL.flags.BTTime = 0;                                  //clear flag
                PortOnHours=read_Int_FRAM(PortOnHoursaddress);                  //write Port ON hours to RTC  
                setClock(RTCAlarm2HoursAddress, PortOnHours);
                PortOnMinutes=read_Int_FRAM(PortOnMinutesaddress);              //write Port ON minutes to RTC    
                setClock(RTCAlarm2MinutesAddress, PortOnMinutes);
            }                

            write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);      //store flag in FRAM  
            enableAlarm(Alarm2);
            PMD3bits.T9MD=0;                                                    //Make sure TMR9 module is enabled  
            IFS3bits.T9IF = 1;
        }

    }

}

void __attribute__((__interrupt__)) _AltINT1Interrupt(void)                     //This is the RTC ALTERNATE ISR when time to read occurs
{
    unsigned char   tempRTC;                                                    

    IFS1bits.INT1IF = 0;                                                        //clear the interrupt flag                             

    INTCON1bits.NSTDIS = 0;
    if(SRbits.IPL==7)
        SRbits.IPL=5;
    
    SLEEP12V = 0;
    validRTC = debounce();
    
    if (validRTC)                                                               //if valid interrupt
    {
        PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress);           //get the PORT_CONTROL flags  
        intSource = readClock(0x0F);                                            //get the RTC Alarm flags		
        intSource &= 0x03;                                                      //strip off bits 7-2    
        
        if(intSource == 0x01 | intSource == 0x03)                               //Alarm 1 
        {
            tempRTC=readClock(0x0f);                                            //clear the RTC A1F 
            tempRTC&=0xfe;                                                      
            setClock(0x0f,tempRTC);                                             
            LC2CONTROL2.flags2.Interrupt = 1;                                   //set the INT2 interrupt flag
            MUX_CLOCK = 0;                                                      //set the MUX_CLOCK line low
            MUX_RESET = 0;                                                      //set the MUX_RESET line low
            disableAlarm(Alarm1);
            LC2CONTROL2.flags2.Waiting = 0;                                     //clear the Waiting flag
            write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);          //store flags in FRAM 
            T4CONbits.TON = 0;
            enableAlarm(Alarm1);
            tempTMR4 = TMR4;                                                    //save TMR4/5 registers
            tempTMR5 = TMR5HLD;
            take_One_Complete_Reading(STORE);
            TMR5HLD = tempTMR5;                                                 //restore TMR4/5 registers
            TMR4 = tempTMR4;
            if (LC2CONTROL.flags.USBpower)                                      
                T4CONbits.TON = 1;                                              //turn Timer4/5 back on
            U1STAbits.OERR = 0;                                                 //clear flag if overrun error
            PMD3bits.T9MD=0;                                                    //Make sure TMR9 module is enabled  
            IFS3bits.T9IF = 1;
        }        

        if(intSource == 0x02 | intSource == 0x03)                               //Alarm 2 
        {
            tempRTC=readClock(0x0f);                                            //clear the RTC A2F 
            tempRTC&=0xfd;                                                      
            setClock(0x0f,tempRTC);                                             
            disableAlarm(Alarm2);

            if (PORT_CONTROL.flags.PortTimerEN && (!PORT_CONTROL.flags.ControlPortON | !PORT_CONTROL.flags.CPTime)) //PORT was OFF or O1 was previously issued    
            {
                CONTROL = 1;                                                    //Turn PORT ON
                _READ=0;                                                        //light LED 
                Nop();
                PORT_CONTROL.flags.ControlPortON = 1;                           //set flag
                PORT_CONTROL.flags.CPTime = 1;                                  //set flag

                PortOffHours=read_Int_FRAM(PortOffHoursaddress);                //write Port OFF hours to RTC 
                setClock(RTCAlarm2HoursAddress, PortOffHours);
                PortOffMinutes=read_Int_FRAM(PortOffMinutesaddress);            //write Port OFF minutes to RTC   
                setClock(RTCAlarm2MinutesAddress, PortOffMinutes);
            } else
            if (PORT_CONTROL.flags.PortTimerEN &&(PORT_CONTROL.flags.ControlPortON | PORT_CONTROL.flags.O0issued | PORT_CONTROL.flags.CPTime)) //PORT was ON,O0 was previously issued or CPTime in process 
            {
                CONTROL = 0;                                                    //Turn PORT OFF
                _READ=1;                                                        //OFF LED 
                PORT_CONTROL.flags.ControlPortON = 0;                           //clear flag
                PORT_CONTROL.flags.O0issued = 0;                                //clear flag
                PORT_CONTROL.flags.CPTime = 0;                                  //clear flag
                PortOnHours=read_Int_FRAM(PortOnHoursaddress);                  //write Port ON hours to RTC  
                setClock(RTCAlarm2HoursAddress, PortOnHours);
                PortOnMinutes=read_Int_FRAM(PortOnMinutesaddress);              //write Port ON minutes to RTC    
                setClock(RTCAlarm2MinutesAddress, PortOnMinutes);
            }
            else                                                                
            if (PORT_CONTROL.flags.BluetoothTimerEN && (!PORT_CONTROL.flags.BluetoothON | !PORT_CONTROL.flags.BTTime))   //Bluetooth was OFF or BT1 was previously issued
            {
                enableBT();                                                     //Turn Bluetooth ON
                PORT_CONTROL.flags.BluetoothON = 1;                             //set flag
                PORT_CONTROL.flags.BTTime = 1;                                  //set flag

                PortOffHours=read_Int_FRAM(PortOffHoursaddress);                //write Port OFF hours to RTC 
                setClock(RTCAlarm2HoursAddress, PortOffHours);
                PortOffMinutes=read_Int_FRAM(PortOffMinutesaddress);            //write Port OFF minutes to RTC   
                setClock(RTCAlarm2MinutesAddress, PortOffMinutes);
            } else
            if (PORT_CONTROL.flags.BluetoothTimerEN && (PORT_CONTROL.flags.BluetoothON | PORT_CONTROL.flags.B0issued | PORT_CONTROL.flags.BTTime)) //Bluetooth was ON,B0 was previously issued or BTTime in process
            {
                disableBT();                                                    //Turn Bluetooth OFF    
                PORT_CONTROL.flags.BluetoothON = 0;                             //clear flag
                PORT_CONTROL.flags.B0issued = 0;                                //clear flag
                PORT_CONTROL.flags.BTTime = 0;                                  //clear flag
                PortOnHours=read_Int_FRAM(PortOnHoursaddress);                  //write Port ON hours to RTC  
                setClock(RTCAlarm2HoursAddress, PortOnHours);
                PortOnMinutes=read_Int_FRAM(PortOnMinutesaddress);              //write Port ON minutes to RTC    
                setClock(RTCAlarm2MinutesAddress, PortOnMinutes);
            }                

            write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);      //store flag in FRAM  
            enableAlarm(Alarm2);
            IFS3bits.T9IF = 1;
        }

    }

}

void __attribute__((__interrupt__)) _OscillatorFail(void)
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _AltOscillatorFail(void)
{
    asm("RESET");
}

void __attribute__((__interrupt__)) _T5Interrupt(void)                          //This is the Timer 5 256mS interrupt
{
    VWcountLSW=TMR7+1;                                                          //read the counter value (/8)   
    IFS1bits.T5IF=0;                                                            //clear the interrupt flag
    CaptureFlag=1;                                                              //set the capture flag	
}

void __attribute__((__interrupt__)) _AltT5Interrupt(void)                       //This is the ALTERNATE Timer 5 256mS interrupt
{
	VWcountLSW=TMR7;                                                            //read the counter value (/8)   
    IFS1bits.T5IF=0;                                                            //clear the interrupt flag
    CaptureFlag=1;                                                              //set the capture flag	
}


void __attribute__((__interrupt__)) _T7Interrupt(void)                          //This is the Timer 7 overflow interrupt
{
	VWcountMSW+=1;                                                              //increment the VWcountMSB register 
    IFS3bits.T7IF=0;                                                            //clear the interrupt flag
}

void __attribute__((__interrupt__)) _AltT7Interrupt(void)                       //This is the ALTERNATE Timer 6/7 overflow interrupt
{
	VWflagsbits.timeout=1;                                                      //set the timeout flag  
    IFS3bits.T7IF=0;                                                            //clear the interrupt flag
}