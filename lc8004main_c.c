//-------------------------------------------------------------
//
//				LC8004
//		Single/4-Channel/8-Channel/16-Channel/32-Channel VW
//              8-Channel/32 Channel Thermistor
//              VW/Thermistor Datalogger
//		with USB/RS-232/RS-485/BT interface
//
//-------------------------------------------------------------


//-------------------------------------------------------------
//
//	COMPANY:	GEOKON, INC
//	DATE:		08/26/2016
//	DESIGNER: 	GEORGE MOORE
//	REVISION:   c
//	CHECKSUM:	0x5867 (MPLABX ver 3.15 and XC16 ver 1.26)
//	DATA(RAM)MEM:	7274/30720   24%
//	PGM(FLASH)MEM:  133983/261888 51%

//  Target device is Microchip Technology DsPIC33FJ256GP710A
//  clock is crystal type HS @ 14.7456 MHz
//  Fcy=7.3728 MHz
//	Tcy=135.633680556 nS

//	Watchdog Timer is ENABLED during Take_One_Complete_Reading() via software control.
//	WDT timeout period is 4S
//	WDT is cleared with the ClrWdt() function and DISABLED after each successful VW read operation.


//	CONFIGURATION SETTINGS:                             (Set in LC8004main_x.h)
//
//      Oscillator                                          HS
//      Clock Switching and Monitor                         Sw Disabled, Mon Disabled
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


//				VW GAGE TYPES				
//		GAGE TYPE			DESCRIPTION
//		0				DISABLE CHANNEL
//		1				MID FREQUENCY SWEEP, 1400-3500 Hz
//		2				HIGH FREQUENCY SWEEP, 2800-4500 Hz
//		3				VERY LOW FREQUENCY SWEEP, 400-1200 Hz
//		4				LOW FREQUENCY SWEEP, 1200-2800 Hz
//		5				VERY HIGH FREQUENCY SWEEP, 2500-4500 Hz
//
//				ANALOG GAGE TYPES
//		GAGE TYPE			DESCRIPTION
//		85				EXTERNAL THERMISTOR (C)
//		86				INTERNAL THERMISTOR (C)
//		87				MAIN BATTERY - 12V (V)
//		95				LITHIUM BATTERY (V)
//		97				MAIN BATTERY - 3V (V)




//	The following files are included in the MPLAB project:
//	Z:\8004\LC8004main_x.c (main source: x is revision level)
//	Z:\8004\LC8004delay_a.c
//	Z:\8004\LC8004rtc_a.c
//	Z:\8004\LC8004intFRAM_a.c	
//	Z:\8004\LC8004extFRAM_a.c

//	Header Files:
//  <p33FJ256GP710A.h>
//  Z:\8004\LC8004delay_a.h
//	Z:\8004\LC8004rtc_a.h
//	Z:\8004\LC8004extFRAM_b.h
//	Z:\8004\LC8004main_x.h (main header: x is revision level)
//	<InCap.h>
//	<timer.h>
//	<reset.h>
//	<stdlib.h>
//	<InCap.h>
//	<i2c.h>
//	<uart.h>
//	<stdio.h>
//	<adc12.h>
//  <p33Fxxxx.h>
//  <xc.h>


//--------------------------------------------------------------
//						Includes
//--------------------------------------------------------------
#include "p33FJ256GP710A.h"
#include "LC8004extFRAM_b.h"                              
#include "LC8004main_c.h"
#include "LC8004delay_a.h"
#include "LC8004rtc_a.h"
#include <ports.h>
#include <timer.h>
#include <reset.h>
#include <stdlib.h>
#include <InCap.h>
#include <uart.h>
#include <i2c.h>
#include <string.h>
#include <stdio.h>
#include <adc.h>
#include <math.h>
#include <p33Fxxxx.h>
#include <xc.h>



//***************************************************************************
//						Main Program
//***************************************************************************

int main(void) 
{
    int gageType;
    unsigned char n = 0;
    unsigned int data;
    unsigned int date = 150; //decimal date
    unsigned int LoggingFlags; //flags register for FRAM
    float digits = 0.0;
    unsigned char controlreg = 0;


    
    setup();
    SLEEP12V = 0; //Set 12V regulator into switchmode
    wait2S(); //provide a 2S delay to allow DS3231 to stabilize
    

    //***************************VER 6.0.0:TEST FOR 3V or 12V BATTERY*****************************************************************
    //Determine whether 3V or 12V battery is connected
    //mainBatreading=take_analog_reading(87);                                     //test the 12V_SENSE input
    //config_Ports_Low_Power();                       
    //if (mainBatreading > 137) //12V_SENSE >0.5V, so connected
    //{
    //    Blink(12);
    //} else {
        //mainBatreading=take_analog_reading(97);                                 //take the 3V battery reading
        //config_Ports_Low_Power();                       
    //    if (mainBatreading > 273) //3VSENSE>0.5V, so connected
    //    {
    //        Blink(3);
    //    }
    //}

    //********************************************************************************************************************************

    baudrate = read_Int_FRAM(baudrateaddress);
    if ((baudrate != brg9600) && (baudrate != brg115200)) //FRAM does not contain valid baud rate value
    {
        baudrate = brg115200; //set initial baud rate to 115200 bps
        write_Int_FRAM(baudrateaddress, baudrate); //store baudrate in FRAM   TEST REM
    }

    configUARTnormal();                                                         //TEST

    if (isWDTTO()) //Did WDT timeout during VW read?		
    {
        MUX_CLOCK = 0; //set the MUX_CLOCK line low
        MUX_RESET = 0; //set the MUX_RESET line low
        trap = read_Int_FRAM(TrapRegisteraddress);
        trap += 1; //increment trap counter if it did
        write_Int_FRAM(TrapRegisteraddress, trap); //store trap value in FRAM  
        putsUART1(VWTimeout); //"VW Timeout Occurred!"
        while (BusyUART1());
        crlf();
    }

    DISPLAY_CONTROL.flags.Synch = 1; //default Synch setting
    restoreSettings(); //reload the settings from FRAM
    testReset();

    /*********************REM VER 6.0.5
if(LC2CONTROL2.flags2.FU)					//reboot from firmware update?
{
        configUARTnormal();                                         //configure the UART1 for normal communications
        crlf();
        putsUART1(UpdateComplete);                                  //Display "Firmware Update Complete"
        while(BusyUART1());
        crlf();
        putsUART1(Softwareversion);                                 //Software version:
        while(BusyUART1());
        putsUART1(Rev);
        while(BusyUART1());
        crlf();
}								
LC2CONTROL2.flags2.FU=0;					//Clear the FU flag
FRAMTest=write_intFRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flag in FRAM
     */

    if (LC2CONTROL.flags.Reset) 
    {
        putsUART1(ResetComplete); //Display "Reset Complete" if RESET command was previously issued
        while (BusyUART1());
        prompt();
        LC2CONTROL.flags.Reset = 0; //Clear the Reset flag
        write_Int_FRAM(LC2CONTROLflagsaddress, LC2CONTROL.full); //store flag in FRAM  
    }


    if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) //was logger previously logging?
    {
        if (DISPLAY_CONTROL.flags.Synch)
            VWflagsbits.synch = 1; //set the synch flag
        LC2CONTROL2.flags2.FirstReading = 1; //clear the first reading flag
        write_Int_FRAM(LC2CONTROL2flagsaddress, LC2CONTROL2.full2); //store flags in FRAM 
        startLogging(); //yes
    }

    DISPLAY_CONTROL.flags.TakingReading = 0;
    DISPLAY_CONTROL.flags.Shutdown = 0;
    //shutdownTimer(TimeOut);                                                     //Reset 15S timer	

    //DISPLAY_CONTROL.flags.Shutdown=1;		
    //IFS1bits.T5IF=1;                                                      
    //shutdown();                                                                 //TEST REM

    while (1) //infinite loop
    {
        while (!IFS1bits.T5IF) 
        {
            comm(); //open communications channel
        }

        testPoint(1,2);
        
        IFS1bits.T5IF = 0; //clear the TMR5 interrupt flag
        IFS0bits.U1RXIF = 0; //clear the UART1 interrupt flag

        while ((!IFS0bits.U1RXIF && !LC2CONTROL.flags.NetEnabled && _CLK_INT) | (!IFS0bits.U1RXIF && LC2CONTROL.flags.NetEnabled && DISPLAY_CONTROL.flags.Shutdown && _CLK_INT)) //shutdown
        {
            if (LC2CONTROL.flags.NetEnabled && !DISPLAY_CONTROL.flags.Shutdown) 
            {
                shutdownNetworked();
                //_RS485TX_EN=0;					//enable the RS485 Tx
                break;
            } else 
            {
                testPoint(1,3);
                shutdown();
            }
            //shutdownTimer(TimeOut);				
        }

        while (LC2CONTROL.flags.NetEnabled && (!USB_PWR | _232) && DISPLAY_CONTROL.flags.Shutdown && IFS0bits.U1RXIF) 
        {
            testPoint(1,4);
            IFS0bits.U1RXIF = 0;
            //shutdownTimer(TimeOut);                                             //Reset 15S timer

            netTest = qualifyNetAddress();

            if (LC2CONTROL.flags.NetEnabled && netTest == 0 && (!USB_PWR | _232)) {
                IFS1bits.T5IF = 1;
                break;
            }

            NAdata = read_Int_FRAM(Netaddress); //read Network Address from FRAM    
            putsUART1(NetworkaddressIS); //"Network address:"
            while (BusyUART1());
            sprintf(NABUF, "%d", NAdata); //format network address
            putsUART1(NABUF); //display network address
            while (BusyUART1());
            prompt();
            DISPLAY_CONTROL.flags.Shutdown = 0;
            write_Int_FRAM(DISPLAY_CONTROLflagsaddress, DISPLAY_CONTROL.display); //store flags in FRAM     
        }

        if (!LC2CONTROL.flags.NetEnabled) 
        {
            testPoint(1,5);
            //shutdownTimer(TimeOut);                                             //Reset 15S timer
            LC2CONTROL2.flags2.ON = 0;
            write_Int_FRAM(LC2CONTROL2flagsaddress, LC2CONTROL2.full2); //store flag in FRAM  
        }
    } //end of while(1)
} //end of main()



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

void _3VX_on(void) {
    V3_X_CONTROL = 1; //turn on +5VX
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

void _3VX_off(void) {
    V3_X_CONTROL = 0; //shut off +5VX
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

    if (bcd >= 0 && bcd <= 0x09) //convert 2 digit bcd to decimal
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

    return;
}

void Blink(unsigned char times) //VER 6.0.0
{
    unsigned char t = 0; //cycle loop index

    for (t; t < times; t++) {
        _READ = 0; //Light the LED
        delay(800);
        _READ = 1; //Off the LED
        delay(800);
    }

}

void Buf2DateTime(char buffer[]) {

    unsigned char SelectionLoop;
    unsigned char temp = 0;
    unsigned char tens = 0;
    unsigned char ones = 0;
    unsigned char index = 2;
    unsigned char readback = 0;

    struct RTCFlag {
        int monthsChanged : 1;
        int daysChanged : 1;
        int yearsChanged : 1;
        int hoursChanged : 1;
        int minutesChanged : 1;
        int secondsChanged : 1;
        int firstpass : 1;
    } RTCFlagBits;

    RTCFlagBits.monthsChanged = 0; //initialize flags
    RTCFlagBits.daysChanged = 0;
    RTCFlagBits.yearsChanged = 0;
    RTCFlagBits.hoursChanged = 0;
    RTCFlagBits.minutesChanged = 0;
    RTCFlagBits.secondsChanged = 0;
    RTCFlagBits.firstpass = 1;

    RTCseconds = 0; //initialize RTCseconds to 0
    LC2CONTROL.flags.ERROR = 0; //clear the ERROR flag

    for (SelectionLoop = 1; SelectionLoop < 7; SelectionLoop++) {
        for (index; buffer[index] != cr; index++) //parse the buffer for data
        {
            temp = buffer[index]; //get the char at buffer[index]

            if (isalpha(temp)) //don't allow any letters
            {
                LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                return;
            }


            if (LC2CONTROL2.flags2.SetStartTime | LC2CONTROL2.flags2.SetStopTime | PORT_CONTROL.flags.SetAlarm2Time | PORT_CONTROL.flags.SetAlarm2StopTime) {
                if (index == 2 && (temp > two | temp < zero)) //don't allow hour(tens) > 2
                {
                    LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                    return;
                }

                if (index == 3 && (((buffer[2] == two) && (temp > three | temp < zero))//don't allow hour(ones) > 3 
                        | ((buffer[2] == one) && (temp > nine | temp < zero)))) {
                    LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                    return;
                }

                if (index == 4 && temp != colon) //don't allow char other than colon 
                {
                    LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                    return;
                }

                if (index == 5 && (temp > five | temp < zero)) //don't allow minute(tens) > 5
                {
                    LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                    return;
                }

                if (index == 6 && (temp > nine | temp < zero)) //don't allow minute(ones) > 9
                {
                    LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                    return;
                }

                if (buffer[7] != cr) {
                    if (index == 7 && (temp != colon)) //don't allow char other than colon
                    {
                        LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                        return;
                    }

                    if (index == 8 && (temp > five | temp < zero)) {
                        LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                        return;
                    }

                    if (index == 9 && (temp > nine | temp < zero)) {
                        LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                        return;
                    }

                    if (index == 10 && (temp != cr)) {
                        LC2CONTROL.flags.ERROR = 1; //set the ERROR flag
                        return;
                    }
                }
            }

            switch (SelectionLoop) //get the value from the array
            {
                case 1:
                case 2:
                case 3:
                    if (!isdigit(temp) && temp != slash && temp != colon && temp != nul) return; //not valid digit,slash or colon, so exit
                    break;
                case 4:
                case 5:
                    if (!isdigit(temp) && temp != colon) return; //not valid digit or colon, so exit
                    break;
                case 6:
                    if (!isdigit(temp) && temp != colon) return;
                    break;
                default:
                    return;
            }



            if (temp == nul) //convert nul char to ascii zero
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

            temp = temp - 0x30; //convert to decimal

            if (RTCFlagBits.firstpass)
                ones = temp;
            else {
                tens = 10 * (ones); //buffer[2] was tens
                ones = temp; //buffer[3] is ones 
            }

            switch (SelectionLoop) {
                case 1:

                    if (!LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL2.flags2.SetStopTime && !PORT_CONTROL.flags.SetAlarm2Time && !PORT_CONTROL.flags.SetAlarm2StopTime) {
                        RTCmonths = tens + ones;
                        if ((RTCmonths > 12 | RTCmonths == 0) && !RTCFlagBits.firstpass) return; //invalid	
                        RTCFlagBits.monthsChanged = 1; //set the months changed flag
                        break;
                    } else {
                        if (PORT_CONTROL.flags.SetAlarm2StopTime) {
                            PortOffHours = tens + ones;
                            if ((PortOffHours > 23) && !RTCFlagBits.firstpass) {
                                return; //invalid
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
                            setClock(RTCAlarm1HoursAddress, RTChours); //load the Start Time Hours into the RTC

                        if (PORT_CONTROL.flags.SetAlarm2Time) {
                            setClock(RTCAlarm2HoursAddress, RTChours); //load the Start Time Hours into the RTC
                            write_Int_FRAM(PortOnHoursaddress,RTChours);	//store Port ON hours in FRAM 
                        }
                        break;
                    }

                case 2:

                    if (!LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL2.flags2.SetStopTime && !PORT_CONTROL.flags.SetAlarm2Time && !PORT_CONTROL.flags.SetAlarm2StopTime) {
                        RTCdays = tens + ones;
                        if ((RTCdays > 31 | RTCdays == 0) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.daysChanged = 1; //set the days changed flag
                        break;
                    } else {
                        if (PORT_CONTROL.flags.SetAlarm2StopTime) {
                            PortOffMinutes = tens + ones;
                            if ((PortOffMinutes > 59) && !RTCFlagBits.firstpass) {
                                return; //invalid
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
                            setClock(RTCAlarm1MinutesAddress, RTCminutes); //load the Start Time Minutes into the RTC
                            RTCseconds = 0;
                            setClock(RTCAlarm1SecondsAddress, RTCseconds);
                        }

                        if (PORT_CONTROL.flags.SetAlarm2Time) {
                            RTCminutes = tens + ones;
                            if ((PortOffMinutes > 59) && !RTCFlagBits.firstpass) {
                                return; //invalid
                            }
                            setClock(RTCAlarm2MinutesAddress, RTCminutes); //load the Control Port RTC Alarm2 minutes into the RTC
                            write_Int_FRAM(PortOnMinutesaddress,RTCminutes);	//store Port ON minutes in FRAM   
                        }
                        break;
                    }

                case 3:

                    if (!LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL2.flags2.SetStopTime && !PORT_CONTROL.flags.SetAlarm2Time && !PORT_CONTROL.flags.SetAlarm2StopTime) {
                        RTCyears = tens + ones;
                        if ((RTCyears > 99) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.yearsChanged = 1; //set the years changed flag
                        break;
                    } else {
                        if (LC2CONTROL2.flags2.SetStopTime) {
                            LoggingStopSeconds = tens + ones;
                            if ((LoggingStopSeconds > 59) && !RTCFlagBits.firstpass) return; //invalid
                        } else {
                            RTCseconds = tens + ones;
                            if ((RTCseconds > 59) && !RTCFlagBits.firstpass) {
                                return; //invalid
                            }
                        }

                        if (!LC2CONTROL2.flags2.SetStopTime) //if logging stop time is not being extracted from buffer
                        {
                            setClock(RTCAlarm1SecondsAddress, RTCseconds); //load the Start Time Seconds into the RTC
                        }

                        if (temp == nul && !RTCFlagBits.firstpass) return;
                        break;
                    }

                case 4:

                    if (!LC2CONTROL2.flags2.SetStopTime) {
                        RTChours = tens + ones;
                        if ((RTChours > 23) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.hoursChanged = 1; //set the hours changed flag
                        break;
                    }

                    if (PORT_CONTROL.flags.SetAlarm2StopTime) {
                        PortOffHours = tens + ones;
                        if ((PortOffHours > 23) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.hoursChanged = 1; //set the hours changed flag
                        break;
                    } else {
                        LoggingStopHours = tens + ones;
                        if ((LoggingStopHours > 23) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.hoursChanged = 1; //set the hours changed flag
                        break;
                    }

                case 5:

                    if (!LC2CONTROL2.flags2.SetStopTime) {
                        RTCminutes = tens + ones;
                        if ((RTCminutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.minutesChanged = 1; //set the minutes changed flag
                        break;
                    }

                    if (PORT_CONTROL.flags.SetAlarm2StopTime) {
                        PortOffMinutes = tens + ones;
                        if ((PortOffMinutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.minutesChanged = 1; //set the hours changed flag
                        break;
                    } else {
                        LoggingStopMinutes = tens + ones;
                        if ((LoggingStopMinutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.minutesChanged = 1; //set the minutes changed flag
                        break;
                    }

                case 6:

                    if (!LC2CONTROL2.flags2.SetStopTime) {
                        RTCseconds = tens + ones;
                        if ((RTCseconds > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.secondsChanged = 1; //set the seconds changed flag
                        break;
                    } else {
                        LoggingStopSeconds = tens + ones;
                        if ((LoggingStopSeconds > 59) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.secondsChanged = 1; //set the seconds changed flag
                        break;
                    }

                default:
                    return;
            }

            RTCFlagBits.firstpass = 0;
        } //end of for(index)

        index++;
        RTCFlagBits.firstpass = 1;
        tens = 0;
        ones = 0;
    } //end of for(SelectionLoop)

    if (PORT_CONTROL.flags.SetAlarm2Time) {
        setClock(RTCAlarm2HoursAddress, RTChours); //load the Control Port RTC Alarm2 Hours into the RTC
        write_Int_FRAM(PortOnHoursaddress,RTChours);	//store Port ON hours in FRAM		
    }

    if (!LC2CONTROL2.flags2.SetStopTime) //don't load RTC if logging stop time is being extracted from buffer
    {


        if (RTCFlagBits.monthsChanged) //if the user entered new month info
            setClock(RTCMonthsAddress, RTCmonths); //load it into the RTC

        if (RTCFlagBits.daysChanged) //if the user entered new day info
            setClock(RTCDaysAddress, RTCdays); //load it into the RTC

        if (RTCFlagBits.yearsChanged) //if the user entered new year info
            setClock(RTCYearsAddress, RTCyears); //load it into the RTC

        if (RTCFlagBits.hoursChanged && !LC2CONTROL.flags.LoggingStartTime) //if the user entered new hour info for current time
            setClock(RTCHoursAddress, RTChours); //load it into the RTC

        if (RTCFlagBits.hoursChanged && LC2CONTROL.flags.LoggingStartTime) //if the user entered new hour info for Start Logging
        {
            setClock(RTCAlarm1HoursAddress, RTChours); //load it into the RTC
            enableAlarm(Alarm1);
        }

        if (RTCFlagBits.minutesChanged && !LC2CONTROL.flags.LoggingStartTime) //if the user entered new minute info
            setClock(RTCMinutesAddress, RTCminutes); //load it into the RTC

        if (RTCFlagBits.minutesChanged && LC2CONTROL.flags.LoggingStartTime) {
            setClock(RTCAlarm1MinutesAddress, RTCminutes);
            enableAlarm(Alarm1);
        }

        if (RTCFlagBits.secondsChanged) //if the user entered new seconds info
            setClock(RTCSecondsAddress, RTCseconds); //load it into the RTC

        crlf();

        putsUART1(Date); //Date:
        while (BusyUART1());

        RTCdata = readClock(RTCMonthsAddress); //get the month from the RTC
        displayClock(RTCdata); //display it

        putcUART1(slash); // '/'
        while (BusyUART1());

        RTCdata = readClock(RTCDaysAddress); //get the day from the RTC
        displayClock(RTCdata); //display it

        putcUART1(slash); // '/'
        while (BusyUART1());

        RTCdata = readClock(RTCYearsAddress); //get the year from the RTC
        displayClock(RTCdata); //display it

        putsUART1(Time); //Time:
        while (BusyUART1());

        RTCdata = readClock(RTCHoursAddress); //get the hour from the RTC
        displayClock(RTCdata); //display it

        putcUART1(colon); // ':'
        while (BusyUART1());

        RTCdata = readClock(RTCMinutesAddress); //get the minute from the RTC
        displayClock(RTCdata); //display it

        putcUART1(colon); // ':'
        while (BusyUART1());

        RTCdata = readClock(RTCSecondsAddress); //get the second from the RTC
        displayClock(RTCdata); //display it
    }
}

unsigned long Buffer2Decimal(char buffer[], unsigned int i, unsigned int x) {
    unsigned long num = 0;
    unsigned int index = 0;
    unsigned long temp;
    unsigned int loopcount = 0;
    unsigned int comp = 0; //compensation for 2 letter command "SC" and "NA"

    //unsigned int i is # of char in buffer
    //unsigned int x is switch: x=0 for pointer value
    //							x=1 for Scan Interval value
    //							x=2 for Network Address value


    if (buffer[index] == capB && i == 2) //just 'B' was entered
        return 1; //so backup user position by 1

    if (x == 1 | x == 2) //adjust for "SC" or "NA" in buffer
        comp = 1;

    for (index = i - 2; index > comp; index--) {
        if (x == 0) //pointer information being entered
        {
            if (index > 5)
                return; //99999 maximum!
        }

        if (x == 1) //scan interval being entered
        {
            if (index > 6) {
                return 86400; //86400 maximum!
            }
        }

        if (x == 2) //network address being entered
        {
            if (index > 4)
                return 0; //ERROR 256 maximum!
        }

        temp = buffer[index]; //parse the buffer

        if (!isdigit(temp)) {
            LC2CONTROL.flags.ScanError = 1; //set the error flag
            return; //return if not valid digit
        }

        if (index == 2 && (x == 1 | x == 2)) //SC0, NA0 is invalid
        {
            if (temp == 0x30)
                return 0; //ERROR
        }

        switch (loopcount) {
            case 0: //ones
                num = temp - 0x30;
                break;

            case 1:
                num += 10 * (temp - 0x30); //tens
                break;

            case 2:
                num += 100 * (temp - 0x30); //hundreds
                break;

            case 3:
                num += 1000 * (temp - 0x30); //thousands
                break;

            case 4:
                num += 10000 * (temp - 0x30); //ten-thousands
                break;

            default:
                break;
        }

        loopcount++;
    }

    //if(buffer[0]==capP && (num>15999|num==0))	//invalid pointer positions REM VER 6.0.3
    if (buffer[0] == capP && (num > (maxSingleVW - 1) | num == 0)) //invalid pointer positions    VER 6.0.3
        return; //so ignore

    if (num > 86400) //Scan Interval greater than 1 day?
        return 86400; //limit it to 1 day

    if (x == 2 && num > 256) //Network Address >256
        return 0; //ERROR

    return num; //return the decimal value

}

void Buf2GageData(char buffer[]) {
    unsigned char SelectionLoop;
    unsigned char temp;
    unsigned char tens = 0;
    unsigned char ones = 0;
    unsigned char index = 2;
    unsigned char readback = 0;

    struct RTCFlag {
        int monthsChanged : 1;
        int daysChanged : 1;
        int yearsChanged : 1;
        int hoursChanged : 1;
        int minutesChanged : 1;
        int secondsChanged : 1;
        int firstpass : 1;
    } RTCFlagBits;

    RTCFlagBits.monthsChanged = 0; //initialize flags
    RTCFlagBits.daysChanged = 0;
    RTCFlagBits.yearsChanged = 0;
    RTCFlagBits.hoursChanged = 0;
    RTCFlagBits.minutesChanged = 0;
    RTCFlagBits.secondsChanged = 0;
    RTCFlagBits.firstpass = 1;

    for (SelectionLoop = 1; SelectionLoop < 7; SelectionLoop++) {
        for (index; buffer[index] != cr; index++) //parse the buffer for data
        {
            temp = buffer[index]; //get the char at buffer[index]

            switch (SelectionLoop) //get the value from the array
            {
                case 1:
                case 2:
                case 3:
                    if (!isdigit(temp) && temp != slash && temp != colon) return; //not valid digit,slash or colon, so exit
                    break;
                case 4:
                case 5:
                    if (!isdigit(temp) && temp != colon) return; //not valid digit or colon, so exit
                    break;
                case 6:
                    if (!isdigit(temp) && temp != colon) return;
                    break;
                default:
                    return;
            }

            if ((SelectionLoop < 4 && temp == slash) | (SelectionLoop >= 4 && temp == colon) | (LC2CONTROL.flags.LoggingStartTime && temp == colon)) break;

            temp = temp - 0x30; //convert to decimal

            if (RTCFlagBits.firstpass)
                ones = temp;
            else {
                tens = 10 * (ones); //buffer[2] was tens
                ones = temp; //buffer[3] is ones 
            }

            switch (SelectionLoop) {
                case 1:
                    if (!LC2CONTROL.flags.LoggingStartTime) {
                        RTCmonths = tens + ones;
                        if ((RTCmonths > 12 | RTCmonths == 0) && !RTCFlagBits.firstpass) return; //invalid	
                        RTCFlagBits.monthsChanged = 1; //set the months changed flag
                        break;
                    } else {
                        RTChours = tens + ones;
                        if ((RTChours > 23) && !RTCFlagBits.firstpass) return; //invalid
                        if (!LC2CONTROL2.flags2.SetStopTime) //if logging stop time is not being extracted from buffer
                            setClock(RTCAlarm1HoursAddress, RTChours); //load the Start Time Hours into the RTC
                        break;
                    }

                case 2:
                    if (!LC2CONTROL.flags.LoggingStartTime) {
                        RTCdays = tens + ones;
                        if ((RTCdays > 31 | RTCdays == 0) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.daysChanged = 1; //set the days changed flag
                        break;
                    } else {
                        RTCminutes = tens + ones;
                        if ((RTCminutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                        if (!LC2CONTROL2.flags2.SetStopTime) //if logging stop time is not being extracted from buffer
                            setClock(RTCAlarm1MinutesAddress, RTCminutes); //load the Start Time Minutes into the RTC
                        break;
                    }

                case 3:
                    if (!LC2CONTROL.flags.LoggingStartTime) {
                        RTCyears = tens + ones;
                        if ((RTCyears > 99) && !RTCFlagBits.firstpass) return; //invalid
                        RTCFlagBits.yearsChanged = 1; //set the years changed flag
                        break;
                    } else {
                        RTCseconds = tens + ones;
                        if ((RTCseconds > 59) && !RTCFlagBits.firstpass) return; //invalid
                        if (!LC2CONTROL2.flags2.SetStopTime) //if logging stop time is not being extracted from buffer
                            setClock(RTCAlarm1SecondsAddress, RTCseconds); //load the Start Time Seconds into the RTC
                        break;
                    }

                case 4:
                    RTChours = tens + ones;
                    if ((RTChours > 23) && !RTCFlagBits.firstpass) return; //invalid
                    RTCFlagBits.hoursChanged = 1; //set the hours changed flag
                    break;

                case 5:
                    RTCminutes = tens + ones;
                    if ((RTCminutes > 59) && !RTCFlagBits.firstpass) return; //invalid
                    RTCFlagBits.minutesChanged = 1; //set the minutes changed flag
                    break;

                case 6:
                    RTCseconds = tens + ones;
                    if ((RTCseconds > 59) && !RTCFlagBits.firstpass) return; //invalid
                    RTCFlagBits.secondsChanged = 1; //set the seconds changed flag
                    break;

                default:
                    return;
            }

            RTCFlagBits.firstpass = 0;

        } //end of for(index)

        index++;
        RTCFlagBits.firstpass = 1;
        tens = 0;
        ones = 0;
    } //end of for(SelectionLoop)

    if (!LC2CONTROL2.flags2.SetStopTime) //don't load RTC if logging stop time is being extracted from buffer
    {
        if (RTCFlagBits.monthsChanged) //if the user entered new month info
            setClock(RTCMonthsAddress, RTCmonths); //load it into the RTC

        if (RTCFlagBits.daysChanged) //if the user entered new day info
            setClock(RTCDaysAddress, RTCdays); //load it into the RTC

        if (RTCFlagBits.yearsChanged) //if the user entered new year info
            setClock(RTCYearsAddress, RTCyears); //load it into the RTC

        if (RTCFlagBits.hoursChanged && !LC2CONTROL.flags.LoggingStartTime) //if the user entered new hour info for current time
            setClock(RTCHoursAddress, RTChours); //load it into the RTC

        if (RTCFlagBits.hoursChanged && LC2CONTROL.flags.LoggingStartTime) //if the user entered new hour info for Start Logging
        {
            setClock(RTCAlarm1HoursAddress, RTChours); //load it into the RTC
            enableAlarm(Alarm1);
        }

        if (RTCFlagBits.minutesChanged && !LC2CONTROL.flags.LoggingStartTime) //if the user entered new minute info
            setClock(RTCMinutesAddress, RTCminutes); //load it into the RTC

        if (RTCFlagBits.minutesChanged && LC2CONTROL.flags.LoggingStartTime) {
            setClock(RTCAlarm1MinutesAddress, RTCminutes);
            enableAlarm(Alarm1);
        }

        if (RTCFlagBits.secondsChanged) //if the user entered new seconds info
            setClock(RTCSecondsAddress, RTCseconds); //load it into the RTC
    }

    crlf();

    putsUART1(Date); //Date:
    while (BusyUART1());

    RTCdata = readClock(RTCMonthsAddress); //get the month from the RTC
    displayClock(RTCdata); //display it

    putcUART1(slash); // '/'
    while (BusyUART1());

    RTCdata = readClock(RTCDaysAddress); //get the day from the RTC
    displayClock(RTCdata); //display it

    putcUART1(slash); // '/'
    while (BusyUART1());

    RTCdata = readClock(RTCYearsAddress); //get the year from the RTC
    displayClock(RTCdata); //display it

    putsUART1(Time); //Time:
    while (BusyUART1());

    RTCdata = readClock(RTCHoursAddress); //get the hour from the RTC
    displayClock(RTCdata); //display it

    putcUART1(colon); // ':'
    while (BusyUART1());

    RTCdata = readClock(RTCMinutesAddress); //get the minute from the RTC
    displayClock(RTCdata); //display it

    putcUART1(colon); // ':'
    while (BusyUART1());

    RTCdata = readClock(RTCSecondsAddress); //get the second from the RTC
    displayClock(RTCdata); //display it
}

int checkScanInterval(void) //VER 6.0.13
{
    MUX4_ENABLE.mux=read_Int_FRAM(MUX4_ENABLEflagsaddress);  

    if (MUX4_ENABLE.mflags.mux16_4 == Single) {
        if (ScanInterval < minScanSingleVW)
            return minScanSingleVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) {
        if (ScanInterval < minScanFourVW)
            return minScanFourVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) {
        if (ScanInterval < minScanEightVW)
            return minScanEightVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) {
        if (ScanInterval < minScanSixteenVW)
            return minScanSixteenVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) {
        if (ScanInterval < minScanThirtytwoVW)
            return minScanThirtytwoVW;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) {
        if (ScanInterval < minScanEightTH)
            return minScanEightTH;
        return 0;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) {
        if (ScanInterval < minScanThirtytwoTH)
            return minScanThirtytwoTH;
        return 0;
    }
}


void checkSynch(unsigned long ReadingTimeSeconds) {
    unsigned char CurrentDay = 0;
    unsigned long CurrentTimeSeconds = 0;
    unsigned long ScanInterval = 0;
    unsigned long NextTimeSeconds = 0;

    CurrentTimeSeconds = RTChms2s(1); //get the current time from the RTC
    NextTimeSeconds = RTChms2s(0); //get the next time to read from the RTC
    ScanInterval = hms2s(); //get the scan interval
    CurrentDay = readClock(RTCDaysAddress); //get the current day

    if ((((CurrentTimeSeconds >= NextTimeSeconds) && NextTimeSeconds != 0) |
            ((RTCdays != CurrentDay) && NextTimeSeconds == 0)) &&
            ((ReadingTimeSeconds + ScanInterval) - NextTimeSeconds != 86400)) //will the next reading get missed?
    {
        if (LC2CONTROL2.flags2.FirstReading) //is it the first reading?
        {
            NextTimeSeconds += ScanInterval; //adjust the next time to read by +1 ScanInterval unit
            hms(NextTimeSeconds, 1); //update the RTC Alarm1 register accordingly
        }
    }
}

void clockMux(unsigned int delayValue) {
    MUX_CLOCK = 1; //set MUX_CLOCK high (1st pulse)
    delay(delayValue); //delay
    MUX_CLOCK = 0; //set MUX_CLOCK low
    delay(delayValue); //delay
    MUX_CLOCK = 1; //set MUX_CLOCK high (2nd pulse)
    delay(delayValue); //delay
    MUX_CLOCK = 0; //set MUX_CLOCK low
}

void clockThMux(unsigned int delayValue) {
    MUX_RESET = 1; //set MUX_RESET high
    delay(delayValue); //delay
    MUX_RESET = 0; //set MUX_RESET low
}

void comm(void) {
    unsigned char MonitorWasEnabled = 0;
    unsigned char value;
    unsigned char SecondsValue; //for RTC testing
    unsigned char MinutesValue;
    unsigned char HoursValue;
    unsigned char DayValue;
    unsigned char DateValue;
    unsigned char Month_CenturyValue;
    unsigned char YearValue;
    unsigned char Alarm1SecondsValue;
    unsigned char Alarm1MinutesValue;
    unsigned char Alarm1HoursValue;
    unsigned char Alarm1DayDateValue;
    unsigned char Alarm2DayDateValue;
    unsigned char ControlValue; //for RTC testing
    unsigned char StatusValue; //for RTC testing
    unsigned char AgingOffsetValue;
    unsigned char MSBofTempValue;
    unsigned char LSBofTempValue;
    char counter = 0;
    char RxDataTemp = 0;
    char temp;
    char trapBUF[6]; //temporary storage for trap count, lithium cell reading and minimum allowable scan interval
    char testmenuBUF[1]; //stores test menu selection
    int channel = 0; //current channel #
    int conversion = 0; //current conversion type:
    //0=linear
    //1=polynomial
    //-1=channel disabled
    int CH;
    int ch;
    long i;                                                                     //REV B
    int LogInt = 0;
    int LogIntIt = 0;
    int NetAddress = 0;
    int ones = 0;
    int tens = 0;
    int hundreds = 0;
    unsigned int StoredNetAddress = 0;
    int SR_SAVE;
    int TBLPAG_SAVE;
    int tensScanInterval = 0;
    int testScanInterval = 0;
    unsigned int BackupArrayPointer = 0;
    unsigned int PositionArrayPointer;
    unsigned int DisplayArrayPointer = 0;
    unsigned int response;
    unsigned int BadData; //holds UART1 garbage
    unsigned int data; //FOR FRAM TEST
    unsigned int testData; //for testInternalFRAM
    unsigned int ia;
    unsigned int id; //display index
    unsigned int test;
    int pointer;
    unsigned int userPosition;
    volatile unsigned int temp_year;
    volatile unsigned int temp_julian;
    volatile unsigned int temp_hour;
    volatile unsigned int temp_minute;
    volatile unsigned int temp_second;
    volatile unsigned int temp_extThermreading = 0;
    volatile unsigned int temp_intThermreading = 0;
    volatile unsigned int temp_lithBatreading = 0;
    volatile int temp_mainBatreading = 0;
    volatile float temp_reading = 0.0;
    unsigned long FRAMaddress;
    unsigned long Alarm1SecondsLong = 0;
    unsigned long Alarm1MinutesLong = 0;
    unsigned long Alarm1HoursLong = 0;
    unsigned long NetworkAddress = 0;
    volatile unsigned long memoryStatus;
    unsigned long tempID = 0; //VER 6.0.12
    float TEMPVAL; //temporary floating point value


    for(i=0;i<5;i++)                                                            //empty the Rx buffer
    {
        RxData = ReadUART1();
    }

    if (!_232) //is RS232?
        _232SHDN = 1; //Enable RS232

    while (!IFS1bits.T5IF) //enable COM for Timeout period
    {
        shutdownTimer(TimeOut);                                                 //start 15S shutdown timer	(network not enabled)
        //wait till first char is received
        while(!DataRdyUART1() && !IFS1bits.T5IF && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR);    

        if(U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)  
            handleCOMError();				//if communications error   TEST REM

        RxData = ReadUART1();

        if (!LC2CONTROL.flags.NetEnabled && !DISPLAY_CONTROL.flags.Shutup && !IFS1bits.T5IF)
        {
            testPoint(1,1);
            putsUART1(Hello);
            while (BusyUART1());
            prompt();
        }

        DISPLAY_CONTROL.flags.Shutup = 0; //Clear Shutup flag if set 

        for (i = 0; i < 30; i++) //clear the buffer
        {
            buffer[i] = 0;
        }

        i = 0; //initialize char buffer index

        RxData = 0; //initialize receive buffer

        while (!IFS1bits.T5IF)
        {
            while (!IFS1bits.T5IF)
            {
                shutdownTimer(TimeOut);				//start 15S shutdown timer	(network not enabled)

                while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF); //read the keyboard input - timeout if 15 seconds of inactivity
                    //handleCOMError(); //if communications error

                if (IFS1bits.T5IF) //break out of loop if time to shutdown	
                    break;

                RxData = ReadUART1(); //get the char from the USART buffer
                if (RxData != cr && RxData != escape) {
                    putcUART1(RxData); //echo char (except <CR>) to terminal
                    while (BusyUART1());
                }

                if (RxData == 0x08) //decrement buffer index if backspace is received
                {
                    i--;
                    putcUART1(space); //clear the previous character
                    while (BusyUART1());
                    putcUART1(0x08);
                    while (BusyUART1());
                }

                if (i < 0) //don't let buffer index go below zero
                    i = 0;

                if (RxData != 0x08 && i < 52) //as long as char is not backspace and index is <52	
                {
                    buffer[i] = RxData; //store the received char in the buffer
                    i++;
                    if (i > 51)
                        i = 0;
                }

                if (RxData == cr) //CR received - buffer is ready
                {
                    RxData = 0; //clear the receive buffer
                    break;
                }
            }

            LC2CONTROL2.flags2.X = 0; //make sure 'X' flag is clear VER 6.0.10

            switch (buffer[0]) {

                case question: //Help Screen
                    shutdownTimer(TimeOut);                                     //Reset 15S timer
                    crlf();
                    putsUART1(CommandDescription);                              //Command				Description
                    while (BusyUART1());

                    crlf();
                    putsUART1(Dashes);                                          //----------------------------------------
                    while (BusyUART1());

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
                    putsUART1(DisplayXnnnn);                                    //DXnnnnn				Display nnnn arrays (not formatted) from pointer
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

                    crlf();
                    putsUART1(Monitorstatus);                                   //M,MD,ME				Monitor status, Disable, Enable
                    while (BusyUART1());

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
                    putsUART1(Networkaddress);                                  //NAddd					Network Address (1-256)
                    while (BusyUART1());

                    crlf();
                    putsUART1(Networkstatus_enable_disable);                    //NAddd			Network Status, Disable, Enable
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
                    putsUART1(TF);                                              //TF					Time Format(0=hhmm, 1=hh,mm)
                    while (BusyUART1());

                    crlf();
                    putsUART1(TR);                                              //TR					display TRap count, zero TRap count
                    while (BusyUART1());

                    //crlf();                                                   REM VER 6.0.5
                    //putsUART1(UF);						//UF					Update Firmware REM VER 6.0.5
                    //while(BusyUART1());                                       REM VER 6.0.5

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

                    break;


                case capB:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer
                    while (BusyUART1());

                    if (buffer[1] == cr)
                        break;

                    if (buffer[2] == nine && buffer[3] == cr)                   //setup for 9600 baud
                        value = brg9600;
                    else
                        if (buffer[2] == one && buffer[3] == one && buffer[4] == five && buffer[5] == cr) //setup for 115200 baud
                        value = brg115200;
                    else {
                        break;
                    }
                    baudrate = value;
                    write_Int_FRAM(baudrateaddress,baudrate);                   //store baudrate in FRAM		
                    LC2CONTROL.flags.Reset = 1; //Set the Reset flag
                    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);     //store flag in FRAM  
                    asm("RESET");
                    break;


                case capC:

                    shutdownTimer(TimeOut);                                     //Reset 15S timer
                    while (BusyUART1());

                    if (buffer[1] == cr) //C<CR> received
                    {
                        crlf();
                        putsUART1(Date); //Date:
                        while (BusyUART1());

                        RTCdata = readClock(RTCMonthsAddress); //get the month from the RTC
                        displayClock(RTCdata); //display it

                        putcUART1(slash); // '/'
                        while (BusyUART1());

                        RTCdata = readClock(RTCDaysAddress); //get the day from the RTC
                        displayClock(RTCdata); //display it

                        putcUART1(slash); // '/'
                        while (BusyUART1());

                        RTCdata = readClock(RTCYearsAddress); //get the year from the RTC
                        displayClock(RTCdata); //display it

                        putsUART1(Time); //Time:
                        while (BusyUART1());

                        RTCdata = readClock(RTCHoursAddress); //get the hour from the RTC
                        displayClock(RTCdata); //display it

                        putcUART1(colon); // ':'
                        while (BusyUART1());

                        RTCdata = readClock(RTCMinutesAddress); //get the minute from the RTC
                        displayClock(RTCdata); //display it

                        putcUART1(colon); // ':'
                        while (BusyUART1());

                        RTCdata = readClock(RTCSecondsAddress); //get the second from the RTC
                        displayClock(RTCdata); //display it

                        break;
                    }

                    if (buffer[1] == capS) //CS received
                    {
                        SR_SAVE = SR;
                        TBLPAG_SAVE = TBLPAG;
                        SRbits.IPL = 0x7; //Disable interrupts 

                        if (LC2CONTROL.flags.LoggingStartTime) //save flags
                            LC2CONTROL2.flags2.LoggingStartTimeTemp = 1;

                        if (LC2CONTROL2.flags2.SetStartTime)
                            LC2CONTROL2.flags2.SetStartTimeTemp = 1;

                        LC2CONTROL.flags.LoggingStartTime = 0;
                        LC2CONTROL2.flags2.SetStartTime = 0;
                        LC2CONTROL2.flags2.SetStopTime = 0; //VER 6.0.0
                        PORT_CONTROL.flags.SetAlarm2Time = 0; //VER 6.0.0
                        PORT_CONTROL.flags.SetAlarm2StopTime = 0; //VER 6.0.0

                        Buf2DateTime(buffer);

                        if (LC2CONTROL.flags.ERROR) {
                            LC2CONTROL.flags.ERROR = 0; //clear the ERROR flag
                            break;
                        }

                        if (LC2CONTROL2.flags2.LoggingStartTimeTemp) //restore flags
                            LC2CONTROL.flags.LoggingStartTime = 1;
                        if (LC2CONTROL2.flags2.SetStartTimeTemp)
                            LC2CONTROL2.flags2.SetStartTime = 1;

                        if (DISPLAY_CONTROL.flags.Synch)
                            VWflagsbits.synch = 1; //set the synch flag

                        if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) // update only if logging
                            upD8RTCAlarm1();

                        TBLPAG = TBLPAG_SAVE;
                        SR = SR_SAVE;
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
                            putsUART1(DnotAllowed); //"DEFAULT not allowed while logging"
                            while (BusyUART1());
                            break;
                        }

                        putsUART1(ThisWillLoadDefaults); //"This will load the factory default settings!"    VER 6.0.0
                        while (BusyUART1());
                        
                        crlf();
                        putsUART1(RUsure); //Are you sure(Y/N)?
                        while (BusyUART1());
                        
                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF); //wait for user response	
                        if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                            handleCOMError(); //if communications error

                        response = ReadUART1(); //get the char from the USART buffer
                        putcUART1(response); //echo char to terminal
                        while (BusyUART1());

                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF); //wait for <CR>	
                        //if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR) TEST REM REV B
                          //  handleCOMError(); //if communications error   TEST REM REV B

                        RxData = ReadUART1(); //RxData contains user response

                        if (response == capY && RxData == cr) //yes, so load defaults
                        {
                            loadDefaults();
                            crlf();
                            putsUART1(AllChannelsLoaded); //"Restored to factory default settings."  VER 6.0.0
                            while (BusyUART1());
                            break;
                        } else {
                            crlf();
                            putsUART1(DefaultsNotLoaded); //"Settings not restored to factory defaults."  VER 6.0.0
                            while (BusyUART1());
                            break;
                        }
                    }

                    if (buffer[1] == cr) //D<CR> received
                    {
                        if (memoryStatus == 0 && FRAM_MEMORY.flags.memEmpty) //if memory is empty
                        {
                            crlf();
                            putsUART1(Noarrays2display); //"There are no arrays to display"
                            while (BusyUART1());
                            break;
                        }

                        if (!DISPLAY_CONTROL.flags.BPD) {
                            displayMemoryStatus(); //display memory status from FRAM
                            break;
                        } else {
                            if (tempUserPosition <= memoryStatus && (!DISPLAY_CONTROL.flags.newPointer &&
                                    DISPLAY_CONTROL.flags.BPD && !DISPLAY_CONTROL.flags.Backup && DISPLAY_CONTROL.flags.Scan))
                                tempUserPosition = id;

                            if (tempUserPosition <= memoryStatus) {
                                LC2CONTROL2.flags2.ID = 1; //set the ID flag   VER 6.0.2
                                displayReading(ch, tempUserPosition); //display the reading at the current user position
                            }

                            tempUserPosition++;
                            if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                                crlf();

                            if (tempUserPosition > memoryStatus + 1)
                                tempUserPosition = memoryStatus + 1;

                            if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                                displayTemporaryStatus(tempUserPosition); //display memory status with current User Position
                            DISPLAY_CONTROL.flags.Backup = 0; //clear the Backup flag
                            break;
                        }

                    }

                    if (buffer[1] == capL && buffer[2] == cr) //DL<CR> received
                    {
                        if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel    VER 6.0.7
                            putsUART1(LCtwo);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW4) //4 channel mux VER 6.0.7
                            putsUART1(LCtwobyfour);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW8) //8 channel VW mux VER 6.0.13
                            putsUART1(LCtwobyeightVW);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW16) //VER 6.0.7
                            putsUART1(LCtwobysixteen); //16 channel mux
                        if (MUX4_ENABLE.mflags.mux16_4 == VW32) //32 channel VW mux VER 6.0.13
                            putsUART1(LCtwobythirtytwoVW);
                        if (MUX4_ENABLE.mflags.mux16_4 == TH8) //8 channel Therm    VER 6.0.7
                            putsUART1(LCtwobyeightTH);
                        if (MUX4_ENABLE.mflags.mux16_4 == TH32) //32 channel Therm    VER 6.0.9
                            putsUART1(LCtwobythirtytwoTH);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[2] == cr) //DF<CR> received
                    {
                        if (!LC2CONTROL.flags.DateFormat) //0=julian date format
                            putsUART1(Datejulian);
                        else
                            putsUART1(Datemonthday); //1=month/day format
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[3] == cr) //DF#<CR> received
                    {
                        if (buffer[2] == zero) //julian date format chosen
                        {
                            LC2CONTROL.flags.DateFormat = 0;
                            putsUART1(Datejulian);
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        if (buffer[2] == one) //month/day format chosen
                        {
                            LC2CONTROL.flags.DateFormat = 1;
                            putsUART1(Datemonthday);
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        while (BusyUART1());
                        break;
                    }

                    //**********************************************************************************************************************
                    //VER 6.0.2:
                    if (buffer[1] == capX && buffer[2] == cr) //DX<CR> received
                    {
                        if (!LC2CONTROL2.flags2.d) //0=ASCII Data Download
                            putsUART1(DX0);
                        else
                            putsUART1(DX1); //1=Binary Data Download
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capX && buffer[3] == cr) //DX#<CR> received
                    {
                        if (buffer[2] == zero) //ASCII data download chosen
                        {
                            LC2CONTROL2.flags2.d = 0;
                            putsUART1(DX0);
                            write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flag in FRAM  
                        }

                        if (buffer[2] == one) //Binary data download chosen
                        {
                            LC2CONTROL2.flags2.d = 1;
                            putsUART1(DX1);
                            write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flag in FRAM  
                        }

                        while (BusyUART1());
                        break;
                    }
                    //*******************************************************************************************************************************************

                    if (isdigit(buffer[1]) && !FRAM_MEMORY.flags.memEmpty) //Enter # of arrays to display from PositionArrayPointer value
                    {
                        if (LC2CONTROL.flags.Monitor) {
                            MonitorWasEnabled = 1;
                            LC2CONTROL.flags.Monitor = 0; //clear the Monitor Flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        DISPLAY_CONTROL.flags.BPD = 1; //set the P_D mode flag						
                        DisplayArrayPointer = Buffer2Decimal(buffer, i, 0); //Get the requested # of arrays to display

                        if (!DISPLAY_CONTROL.flags.Display) //Display from where previous display left off
                            id = tempUserPosition;
                        else
                            tempUserPosition = id;

                        restoreSettings(); //load the flags from internal FRAM   VER 6.0.2
                        LC2CONTROL2.flags2.ID = 1; //set the ID flag   VER 6.0.2
                        DISPLAY_CONTROL.flags.bail = 0; //VER 6.0.2
                        IFS0bits.U1RXIF = 0; //clear the UART1 interrupt flag
                        //IEC0bits.U1RXIE=1;                              //Enable UART1 interrupt    VER 6.0.2 TEST REM
                        for (id; id < (tempUserPosition + DisplayArrayPointer); id++) {
                            IEC0bits.U1RXIE = 1; //TEST
                            if (DISPLAY_CONTROL.flags.bail) //break out of data download
                                break;

                            if (id == (maxSingleVW + 1) && MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel  VW VER 6.0.7
                            {
                                id = 1; //memory rolled over so reset display pointers
                                //DisplayArrayPointer=(DisplayArrayPointer+tempUserPosition)-16001; REM VER 6.0.3
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxSingleVW + 1); //VER 6.0.3
                                tempUserPosition = 1;
                            }

                            if (id == (maxFourVW + 1) && MUX4_ENABLE.mflags.mux16_4 == VW4) //if 4 channel VW MUX  VER 6.0.7
                            {
                                id = 1; //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxFourVW + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxEightVW + 1) && MUX4_ENABLE.mflags.mux16_4 == VW8) //if 8 channel VW MUX  VER 6.0.7
                            {
                                id = 1; //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxEightVW + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxSixteenVW + 1) && MUX4_ENABLE.mflags.mux16_4 == VW16) //if 16 channel VW MUX     VER 6.0.7
                            {
                                id = 1; //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxSixteenVW + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxThirtytwoVW + 1) && MUX4_ENABLE.mflags.mux16_4 == VW32) //if 32 channel VW MUX     VER 6.0.7
                            {
                                id = 1; //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxThirtytwoVW + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxEightTH + 1) && MUX4_ENABLE.mflags.mux16_4 == TH8) //if 8 channel TH MUX  VER 6.0.7
                            {
                                id = 1; //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxEightTH + 1);
                                tempUserPosition = 1;
                            }

                            if (id == (maxThirtytwoTH + 1) && MUX4_ENABLE.mflags.mux16_4 == TH32) //if 32 channel MUX  VER 6.0.9
                            {
                                id = 1; //memory rolled over so reset display pointers
                                DisplayArrayPointer = (DisplayArrayPointer + tempUserPosition)-(maxThirtytwoTH + 1);
                                tempUserPosition = 1;
                            }

                            displayReading(ch, id); //display the array at address id
                        } //end of For loop
                        IEC0bits.U1RXIE = 0; //Disable UART1 interrupt

                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                            displayTemporaryStatus(id); //display memory status with current User Position
                        DISPLAY_CONTROL.flags.newPointer = 0; //clear the pointer updated flag

                        if (MonitorWasEnabled) {
                            LC2CONTROL.flags.Monitor = 1; //reset the monitor flag if previously set
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        DISPLAY_CONTROL.flags.BPD = 0; //clear the P_D mode flag	
                        DISPLAY_CONTROL.flags.Backup = 0; //clear the Backup flag 
                        DISPLAY_CONTROL.flags.Display = 1; //set the Display flag
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 
                        shutdownTimer(TimeOut);	
                        break;
                    }

                    if (FRAM_MEMORY.flags.memEmpty) 
                    {
                        crlf();
                        putsUART1(Noarrays2display); //"There are no arrays to display"
                        while (BusyUART1());
                        break;
                    }

                    DISPLAY_CONTROL.flags.Backup = 0; //clear the back pointer up flag
                    break;

                case capE: //Shut-down

                    crlf();
                    write_Int_FRAM(UserPositionaddress,0x00);	//Zero the User Position pointer    
                    write_Int_FRAM(0xF02F,0x00);                    //UserPositionaddress+1    
                    DISPLAY_CONTROL.flags.Shutdown = 1;
                    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 
                    IFS1bits.T5IF = 1; //set the T5IF to break out on Comm() in main()
                    return;

                case capG:

                    shutdownTimer(TimeOut);					//Reset 15S timer

                    for (i = 1; buffer[i] != cr && !LC2CONTROL.flags.ERROR; i++) //parse buffer for illegal characters
                    { //exit if one is encountered
                        if (buffer[i] != minus && buffer[i] != decimal && buffer[i] != slash && !isdigit(buffer[i]) && buffer[i] != capD
                                && buffer[i] != capL && buffer[i] != capP)
                            LC2CONTROL.flags.ERROR = 1; //set the error flag
                    }

                    if (LC2CONTROL.flags.ERROR) //Test for error
                    {
                        LC2CONTROL.flags.ERROR = 0; //reset the error flag
                        break; //return to '*' prompt
                    }

                    if (!isdigit(buffer[1])) //if buffer[1] is not valid channel #
                        break; //return to '*' prompt

                    if ((isdigit(buffer[1]) && buffer[2] == slash) | (isdigit(buffer[1]) && isdigit(buffer[2]) && buffer[3] == slash))
                        channel = getChannel(); //get the current channel #

                    //REM VER 6.0.6:
                    /*
                    //TEST MUX CONFIGURATION FOR INVALID CHANNELS:
                    if((channel>=5&& (MUX4_ENABLE.mflags.mux16_4==0)) |         //4 channel //VER 6.0.0
            (channel>=17&&(MUX4_ENABLE.mflags.mux16_4==1)) |        //16 channel
                        (channel!=0 && (MUX4_ENABLE.mflags.mux16_4==2)))        //single channel
                        break;							//return to '*' prompt
                     */

                    //VER 6.0.6:
                    //TEST MUX CONFIGURATION FOR INVALID CHANNELS:
                    if ((channel >= 5 && (MUX4_ENABLE.mflags.mux16_4 == VW4)) | //4 channel VW/TH
                            (channel >= 9 && (MUX4_ENABLE.mflags.mux16_4 == TH8)) | //8 channel TH
                            (channel >= 9 && (MUX4_ENABLE.mflags.mux16_4 == VW8)) | //8 channel VW
                            (channel >= 17 && (MUX4_ENABLE.mflags.mux16_4 == VW16)) | //16 channel VW/TH
                            (channel >= 33 && (MUX4_ENABLE.mflags.mux16_4 == TH32)) | //32 channel TH
                            (channel >= 33 && (MUX4_ENABLE.mflags.mux16_4 == VW32)) |
                            (channel != 1 && (MUX4_ENABLE.mflags.mux16_4 == Single))) //single channel
                        break; //return to '*' prompt

                    if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) //VER 6.0.11
                    {
                        conversion = getConversion(); //get the type of conversion:
                        //0=linear
                        //1=polynomial
                        //-1=channel disabled
                        //2=invalid
                        //TEST MUX CONFIGURATION FOR INVALID CONVERSION:
                        if (conversion == 2) //invalid conversion
                            break; //return to the '*' prompt
                        setChannelConversion(channel, conversion); //setup the channel conversion
                    }

                    //Extract gage type from buffer and save to FRAM:
                    gageType = getGageType(channel); //get the user entered gage type
                    if (gageType == 0) //is the channel being disabled?
                    {
                        disableChannel(channel);
                        displayMUX(channel); //display the channel #
                        break;
                    }

                    if ((gageType == -1 | gageType == -2) && (buffer[1] != slash))
                        break; //gage type hasn't changed or illegal entry

                    if (gageType != -1) //if valid gage type is entered, store it
                        storeGageType(channel, gageType);

                    enableChannel(channel); //enable the selected channel

                    //Extract gage info or polynomial coefficients from buffer and save to FRAM:

                    if (conversion) //Polynomial Conversion
                    {
                        getGageInfo(1, channel); //get coefficient A
                        getGageInfo(2, channel); //get coefficient B
                        getGageInfo(3, channel); //get coefficient C
                    } else //Linear Conversion
                    {
                        getGageInfo(1, channel); //get zero reading
                        getGageInfo(2, channel); //get gage factor
                        getGageInfo(3, channel); //get gage offset
                    }

                    displayMUX(channel); //display the channel #
                    break;


                case capI:

                    shutdownTimer(TimeOut);				//Reset 15S timer
                    crlf();

                    if (buffer[1] == capD && buffer[2] == cr && LC2CONTROL.flags.ID) //Display ID
                    {
                        putsUART1(DataloggerID); //Datalogger ID:
                        while (BusyUART1());

                        for (i = IDaddress; i < FRAM_MEMORYflagsaddress; i += 2) //parse the buffer and extract the ID character
                        {
                            data=read_Int_FRAM(i);				//read the ID starting FRAM location  
                            unpack(data); //unpack into (2) bytes
                            if (Hbyte == cr) //exit if MSB = <CR>
                                break;
                            putcUART1(Hbyte);
                            while (BusyUART1());
                            if (Lbyte == cr) //exit if LSB = <CR>
                                break;
                            putcUART1(Lbyte);
                            while (BusyUART1());
                        }

                        break;
                    }


                    if (buffer[1] == capD && buffer[2] == cr && !LC2CONTROL.flags.ID) //No ID to display
                    {
                        putsUART1(DataloggerID); //Datalogger ID:
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capD && buffer[2] == space) //clear the current ID
                    {
                        putsUART1(DataloggerID); //Datalogger ID:
                        while (BusyUART1());

                        for (i = IDaddress; i < FRAM_MEMORYflagsaddress; i++) //load the ID buffer with <cr>   VER 6.0.2
                        {
                            write_Int_FRAM(IDaddress,cr);  //VER 6.0.2 VER 6.0.13    
                        }

                        LC2CONTROL.flags.ID = 0; //reset the ID flag
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM	
                        break;
                    }

                    if (buffer[1] == capD && buffer[2] != cr && buffer[2] != space) //Enter Datalogger ID and store in FRAM
                    {
                        tempID = IDaddress; //VER 6.0.12
                        LC2CONTROL.flags.ID = 1; //set the ID flag
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM	
                        putsUART1(DataloggerID); //Datalogger ID:
                        while (BusyUART1());

                        for (i = 2; i < 17; i++) {
                            if (!isdigit(buffer[i]) && !isalpha(buffer[i]) && buffer[i] != cr && !isgraph(buffer[i]) && buffer[i] != space)//invalid
                            { //so display the stored ID
                                for (i = tempID; i < FRAM_MEMORYflagsaddress; i += 2) //parse the buffer and extract the ID character
                                {
                                    data=read_Int_FRAM(i);				//read the ID starting FRAM location  
                                    unpack(data); //unpack into (2) bytes
                                    if (Hbyte == cr) //exit if MSB = <CR>
                                        break;
                                    putcUART1(Hbyte);
                                    while (BusyUART1());
                                    if (Lbyte == cr) //exit if LSB = <CR>
                                        break;
                                    putcUART1(Lbyte);
                                    while (BusyUART1());
                                }
                                break;
                            }

                            Hbyte = buffer[i]; //load Hbyte
                            if (Hbyte == cr) {
                                data = pack(Hbyte, 0x00);
                                write_Int_FRAM(tempID,data);   
                                break;
                            }

                            i += 1; //increment array pointer
                            Lbyte = buffer[i]; //load Lbyte
                            putcUART1(Hbyte); //display ID
                            while (BusyUART1());
                            if (Lbyte != cr) {
                                putcUART1(Lbyte);
                                while (BusyUART1());
                            }
                            data = pack(Hbyte, Lbyte);
                            write_Int_FRAM(tempID,data);			//store in FRAM   

                            if (Lbyte == cr)
                                break;
                            tempID += 2; //increment FRAM pointer
                        }

                        break;
                    }

                    break;

                case capL:

                    shutdownTimer(TimeOut);								//Reset 15S timer
                    while (BusyUART1());

                    if (buffer[1] == cr) //L<CR> received, so display Log Interval Table
                    {
                        displayLogTable(); //Display Log Interval Table
                        crlf();
                        break;
                    }

                    if (buffer[1] == capD) //LD received
                    {
                        if (LC2CONTROL.flags.LogInterval) {
                            crlf();
                            putsUART1(Logdisabled); //Log intervals disabled
                            while (BusyUART1());
                            LC2CONTROL.flags.LogInterval = 0; //clear the Log interval flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                            break;
                        }
                        crlf();
                        putsUART1(Logintalreadydisabled); //Log intervals already disabled!
                        while (BusyUART1());
                    }

                    if (buffer[1] == capE) //LE received
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

                        if (!LC2CONTROL.flags.LogInterval) //is Log interval flag clear?
                        {
                            crlf();
                            putsUART1(Logenabled); //Log intervals enabled
                            while (BusyUART1());
                            LC2CONTROL.flags.LogInterval = 1; //Set Log intervals flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  VER 6.0.13  
                            DISPLAY_CONTROL.flags.Synch = 0; //clear the Synch flag
                            write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 

                            if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) //is datalogger logging?
                            {
                                DISPLAY_CONTROL.flags.Shutup = 1; //don't allow message display
                                stopLogging(); //stop and restart logging if it is	
                                if (DISPLAY_CONTROL.flags.Synch)
                                    VWflagsbits.synch = 1; //set the synch flag
                                crlf();
                                startLogging();
                                DISPLAY_CONTROL.flags.Shutup = 0;
                            }

                            break;
                        }

                        crlf();
                        putsUART1(Logintalreadyenabled); //Log intervals already enabled!
                        while (BusyUART1());
                    }


                    for (i = 1; buffer[i] != cr && !LC2CONTROL.flags.ERROR; i++) //parse buffer for illegal characters
                    { //exit if one is encountered
                        if (buffer[i] != slash && !isdigit(buffer[i]))
                            LC2CONTROL.flags.ERROR = 1; //set the error flag
                    }

                    if (LC2CONTROL.flags.ERROR) //Test for error
                    {
                        LC2CONTROL.flags.ERROR = 0; //reset the error flag
                        break; //return to '*' prompt
                    }

                    LogInt = getLogInterval(); //Get the Log Interval

                    if (LogInt == -2) //error
                        break; //return to "*" prompt

                    if (LogInt != -1) //"L//" received?
                        getLogIntLength(); //no, so get interval length

                    if (LogIntLength <= 0 | LogIntLength > 86400) //error or illegal
                        break;

                    testScanInterval = checkScanInterval();

                    if (testScanInterval) //if error
                    {
                        crlf();
                        putsUART1(MinInterror); //ERROR: Minumum Scan Interval for this configuration is :
                        while (BusyUART1());
                        sprintf(trapBUF, "%d", testScanInterval); //minimum scan interval in seconds
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        putsUART1(Seconds);
                        while (BusyUART1());
                        break;
                    }

                    LogIntIt = getLogIterations();

                    if (LogIntIt == -1) //error
                        break;

                    storeLogInterval(LogInt, LogIntIt); //store the Log Interval information
                    displayLogInterval(LogInt);
                    break;


                case capM:

                    shutdownTimer(TimeOut);						//Reset 15S timer
                    while (BusyUART1());

                    if (buffer[1] == cr && !LC2CONTROL.flags.Monitor) //Monitor mode disabled?
                    {
                        crlf();
                        putsUART1(Monitordisabled); //display Monitor mode disabled
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == cr && LC2CONTROL.flags.Monitor) //Monitor mode enabled?
                    {
                        crlf();
                        putsUART1(Monitorenabled); //display Monitor mode enabled
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capD) //MD received
                    {
                        crlf();
                        if (LC2CONTROL.flags.Monitor) //is Monitor flag set?
                        {
                            putsUART1(Monitordisabled); //Monitor mode disabled
                            while (BusyUART1());
                            LC2CONTROL.flags.Monitor = 0; //Clear Monitor flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                            break;
                        }

                        putsUART1(Monitoralreadydisabled); //Monitor already disabled!
                        while (BusyUART1());
                    }

                    if (buffer[1] == capE) //ME received
                    {
                        crlf();
                        if (!LC2CONTROL.flags.Monitor) //is Monitor flag clear?
                        {
                            putsUART1(Monitorenabled); //Monitor mode enabled
                            while (BusyUART1());
                            LC2CONTROL.flags.Monitor = 1; //Set Monitor flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                            break;
                        }

                        putsUART1(Monitoralreadyenabled); //Monitor already enabled!
                        while (BusyUART1());
                    }


                    //case MXS, MX4, MX16:

                    if (buffer[1] == capX && buffer[2] == capS && buffer[3] == cr) //Display multiplexer setup
                    {
                        crlf();
                        displayMUX(0);
                        break;
                    }

                    //***************************************VER 6.0.0********************************************************************
                    if (buffer[1] == capX && buffer[2] == one && buffer[3] == cr) //Select single channel VW Datalogger
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }
                        DISPLAY_CONTROL.flags.TH=0;                           //VW Configuration  
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flag in FRAM   
                        MUX4_ENABLE.mflags.mux16_4 = Single; //Single Channel Datalogger selected
                        write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);	//store flag in FRAM  
                        LogIntLength = 3;
                        hms(minScanSingleVW, 0); //load minimum scan interval if Single Channel  VER 6.0.3
                        putsUART1(MUX1); //Display Single Channel Datalogger Selected.    VER 6.0.5
                        while (BusyUART1());
                        break;
                    }


                    //***************************************VER 6.0.5********************************************************************
                    //                   if(buffer[1]==capX && buffer[2]==one && buffer[3]==capT && buffer[4]==cr)          //Select single channel Thermistor Datalogger
                    //                   {
                    //                       crlf();

                    //                       if(LC2CONTROL.flags.Logging)
                    //			{
                    //                            putsUART1(CnotAllowed);
                    //                            while(BusyUART1());
                    //                            break;
                    //			}

                    //                        DISPLAY_CONTROL.flags.TH=1;
                    //                        FRAMTest=write_intFRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flag in FRAM
                    //			MUX4_ENABLE.mflags.mux16_4=2;                           //Single Channel Datalogger selected
                    //			FRAMTest=write_intFRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);	//store flag in FRAM
                    //			LogIntLength=3;
                    //                        hms(minScanSingleVW,0);                                   //load minimum scan interval if Single Channel  VER 6.0.3
                    //			putsUART1(MUX1T);                                       //Display Single Channel Thermistor Datalogger Selected.
                    //			while(BusyUART1());
                    //			break;
                    //                    }


                    //********************************************************************************************************************

                    if (buffer[1] == capX && buffer[2] == four && buffer[3] == cr) //Select 4 channel mux
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MUX4_ENABLE.mflags.mux16_4 = VW4; //4 channel VW/TH mux selected  VER 6.0.7
                        write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);	//store flag in FRAM  
                        LogIntLength = minScanFourVW;
                        hms(minScanFourVW, 0); //load 10S scan interval if 4 channel mode
                        putsUART1(MUX4); //Display 4 Channel Mux Selected.
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

                        MUX4_ENABLE.mflags.mux16_4 = TH8; //8 channel thermistor mux selected VER 6.0.7
                        write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);	//store flag in FRAM  
                        LogIntLength = minScanEightTH;
                        hms(5, 0); //load 5S scan interval if 8 channel thermistor mode
                        putsUART1(MUX8); //Display 8 Channel Mux Selected.
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

                        MUX4_ENABLE.mflags.mux16_4 = TH32; //32 channel thermistor mux selected
                        write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);	//store flag in FRAM  
                        LogIntLength = minScanThirtytwoTH;
                        hms(10, 0); //load 10S scan interval if 32 channel thermistor mode
                        putsUART1(MUX32); //Display 32 Channel Mux Selected.
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

                        MUX4_ENABLE.mflags.mux16_4 = VW16; //16 channel VW/TH mux selected VER 6.0.7
                        write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);	//store flag in FRAM  
                        LogIntLength = minScanSixteenVW;
                        hms(minScanSixteenVW, 0); //load 30S scan interval if 16 channel mode
                        putsUART1(MUX16); //Display 16 Channel Mux Selected.
                        while (BusyUART1());
                        break;
                    }

                    //VER 6.0.11:***********************************************
                    if (buffer[1] == capX && buffer[2] == eight && buffer[3] == cr) //Select 8 channel VW mux
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MUX4_ENABLE.mflags.mux16_4 = VW8; //8 channel VW mux selected
                        write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);	//store flag in FRAM  
                        LogIntLength = minScanEightVW;
                        hms(minScanEightVW, 0); //load min scan interval if 8 channel VW mode
                        putsUART1(MUX8VW); //Display 8 Channel VW Mux Selected.
                        while (BusyUART1());
                        break;
                    }

                    //VER 6.0.9:
                    if (buffer[1] == capX && buffer[2] == three && buffer[3] == two && buffer[4] == cr) //Select 32 channel VW mux
                    {
                        crlf();

                        if (LC2CONTROL.flags.Logging) {
                            putsUART1(CnotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        MUX4_ENABLE.mflags.mux16_4 = VW32; //32 channel VW mux selected
                        write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);	//store flag in FRAM  
                        LogIntLength = minScanThirtytwoVW;
                        hms(minScanThirtytwoVW, 0); //load min scan interval if 32 channel VW mode
                        putsUART1(MUX32VW); //Display 32 Channel VW Mux Selected.
                        while (BusyUART1());
                        break;
                    }
                    //**********************************************************
                    break;


                case capN:

                    shutdownTimer(TimeOut);				//Reset 15S timer
                    while (BusyUART1());

                    if (buffer[1] == cr) //N received
                    {
                        crlf();
                        restoreSettings();

                        if (!LC2CONTROL.flags.Logging && !LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL.flags.LoggingStopTime) //Logging stopped? 
                        {
                            putsUART1(Loggingstopped); //Logging Stopped
                            while (BusyUART1());
                            break;
                        } else {
                            putsUART1(Nexttime2read); //Next Time to Read:
                            while (BusyUART1());
                            RTCdata = readClock(RTCAlarm1HoursAddress); //Display Alarm1 hours
                            displayClock(RTCdata);
                            putcUART1(colon); // :
                            while (BusyUART1());
                            RTCdata = readClock(RTCAlarm1MinutesAddress); //Display Alarm1 minutes
                            displayClock(RTCdata);
                            putcUART1(colon);
                            while (BusyUART1());
                            RTCdata = readClock(RTCAlarm1SecondsAddress); //Display Alarm1 seconds
                            displayClock(RTCdata);
                        }
                        break;
                    }

                    if (buffer[1] == capE && buffer[2] == cr) //Enable Networking
                    {
                        crlf();
                        putsUART1(Networkenabled);
                        while (BusyUART1());
                        LC2CONTROL.flags.NetEnabled = 1; //Set the network enable flag
                        netTest = 1; //Set so readings can be displayed
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        break;
                    }

                    if (buffer[1] == capD && buffer[2] == cr && (USB_PWR | !_232)) //Disable Networking    VER 6.0.0
                    {
                        crlf();
                        putsUART1(Networkdisabled);
                        while (BusyUART1());
                        LC2CONTROL.flags.NetEnabled = 0; //Clear the network enable flag
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        break;
                    }

                    if (buffer[1] == capD && buffer[2] == cr && (!USB_PWR | _232) && LC2CONTROL.flags.NetEnabled) //Disable Networking not allowed    VER 6.0.0
                    { //while networked
                        crlf();
                        putsUART1(Changes); //"Changes not allowed while networked."
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capS && buffer[2] == cr) //Network Status
                    {
                        crlf();
                        if (LC2CONTROL.flags.NetEnabled) //Enabled
                            putsUART1(Networkenabled);
                        else
                            putsUART1(Networkdisabled); //Disabled
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capA && buffer[2] == cr) //NA received
                    {
                        crlf();
                        NAdata=read_Int_FRAM(Netaddress);					//read Network Address from FRAM  
                        putsUART1(NetworkaddressIS); //"Network address:"
                        while (BusyUART1());
                        sprintf(NABUF, "%d", NAdata); //format network address
                        putsUART1(NABUF); //display network address
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capA && (USB_PWR | !_232) && ((isdigit(buffer[2]) && buffer[3] == cr) |
                            (isdigit(buffer[2]) && isdigit(buffer[3]) && buffer[4] == cr) |
                            (isdigit(buffer[2]) && isdigit(buffer[3]) && isdigit(buffer[4]) && buffer[5] == cr))) //Enter Network Address VER 6.0.0
                    {
                        crlf();
                        while (BusyUART1());
                        NetworkAddress = Buffer2Decimal(buffer, i, 2); //extract network address from buffer
                        if (NetworkAddress == 0) // ERROR
                            break;
                        write_Int_FRAM(Netaddress,NetworkAddress);			//store Network Address in FRAM   
                        NAdata=read_Int_FRAM(Netaddress);					//read Network Address from FRAM  
                        putsUART1(NetworkaddressIS); //"Network address:"
                        while (BusyUART1());
                        sprintf(NABUF, "%d", NAdata); //format network address
                        putsUART1(NABUF); //display network address
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capA && (!USB_PWR | _232) && LC2CONTROL.flags.NetEnabled && ((isdigit(buffer[2]) && buffer[3] == cr) |
                            (isdigit(buffer[2]) && isdigit(buffer[3]) && buffer[4] == cr) |
                            (isdigit(buffer[2]) && isdigit(buffer[3]) && isdigit(buffer[4]) && buffer[5] == cr))) //Enter Network Address     VER 6.0.0
                    {
                        crlf();
                        putsUART1(Changes); //"Changes not allowed while networked."
                        while (BusyUART1());
                        break;
                    }
                    break;

                case capO:

                    shutdownTimer(TimeOut);									//Reset 15S timer
                    while (BusyUART1());

                    if (buffer[1] == cr) //O<CR> received
                    {
                        controlPortStatus(); //display control port status
                        break;
                    }

                    if (buffer[1] == zero && buffer[2] == cr) //O0<CR> received
                    {
                        CONTROL = 0;
                        _READ=1;                                                //OFF the LED   REV B
                        if (!PORT_CONTROL.flags.CPTime) //turn off control port
                            PORT_CONTROL.flags.ControlPortON = 0; //clear flag if not in scheduled ON time
                        PORT_CONTROL.flags.O0issued = 1; //set O0issued flag
                        write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in FRAM  
                        controlPortStatus(); //display control port status
                        break;
                    }

                    if (buffer[1] == one && buffer[2] == cr) //O1<CR> received
                    {
                        CONTROL = 1; //turn on control port
                        _READ=0;                                                //LED ON    REV B
                        PORT_CONTROL.flags.ControlPortON = 1;
                        PORT_CONTROL.flags.O0issued = 0; //clear O0issued flag
                        write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in FRAM  
                        controlPortStatus(); //display control port status
                        break;
                    }

                    if (buffer[1] == capD && buffer[2] == cr) //OD<CR> received
                    {
                        PORT_CONTROL.flags.PortTimerEN = 0; //clear Port Timer enable flag
                        write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in FRAM`
                        disableAlarm(Alarm2); //disable the Alarm2 interrupt		
                        controlPortStatus(); //display control port status
                        break;
                    }

                    if (buffer[1] == capE && buffer[2] == cr) //OE<CR> received
                    {
                        PORT_CONTROL.flags.PortTimerEN = 1; //set Port Timer enable flag
                        PORT_CONTROL.flags.CPTime = 0; //clear the CPtime flag
                        write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in FRAM  
                        delay(1000);                                            //TEST
                        enableAlarm(Alarm2); //enable the Alarm2 interrupt
                        controlPortStatus(); //display control port status
                        break;
                    }

                    if (buffer[1] == capT && isdigit(buffer[2])) //Control Port Start time being entered
                    {
                        crlf();
                        PORT_CONTROL.flags.SetAlarm2Time = 1; //set the flag
                        Buf2DateTime(buffer); //set the RTC Alarm2 time
                        PORT_CONTROL.flags.SetAlarm2Time = 0; //clear the flag
                        controlPortStatus(); //display control port status
                        break;
                    }

                    if (buffer[1] == capP && isdigit(buffer[2])) //Control Port Stop time being entered
                    {
                        crlf();
                        PORT_CONTROL.flags.SetAlarm2StopTime = 1; //set the SetAlarm2StopTime flag		
                        Buf2DateTime(buffer); //get Logging Stop Time from buffer
                        PORT_CONTROL.flags.SetAlarm2StopTime = 0; //clear the SetAlarm2StopTime flag
                        if (LC2CONTROL.flags.ERROR) {
                            LC2CONTROL.flags.ERROR = 0; //clear the ERROR flag
                            PORT_CONTROL.flags.SetAlarm2StopTime = 0; //clear the Alarm2StopTime flag
                            write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in FRAM  
                            break;
                        }
                        LC2CONTROL2.flags2.SetStopTime = 0; //clear the SetStopTime flag

                        write_Int_FRAM(PortOffHoursaddress,PortOffHours);	//store Port OFF time in FRAM 
                        write_Int_FRAM(PortOffMinutesaddress,PortOffMinutes);  
                        controlPortStatus(); //display control port status
                        break;
                    }

                    break;


                case capP:

                    shutdownTimer(TimeOut);									//Reset 15S timer
                    while (BusyUART1());

                    if (buffer[1] == cr) //P<CR> received
                    {
                        crlf();
                        if (!DISPLAY_CONTROL.flags.BPD)
                            displayMemoryStatus(); //display memory status from FRAM
                        else
                            displayTemporaryStatus(tempUserPosition); //manipulating the pointers

                        break;
                    }

                    if (isdigit(buffer[1])) {
                        memoryStatus=read_Int_FRAM(MemoryStatusaddress);//get Memory Status (MS) pointer 
                        tempUserPosition = Buffer2Decimal(buffer, i, 0); //Get the requested pointer position
                        if (tempUserPosition > memoryStatus) //invalid input
                            break;
                        DISPLAY_CONTROL.flags.BPD = 1; //set the P_D mode flag
                        DISPLAY_CONTROL.flags.newPointer = 1; //set the pointer updated flag
                        DISPLAY_CONTROL.flags.Scan = 1; //set the Scan flag
                        DISPLAY_CONTROL.flags.Display = 0; //clear Display flag if previously enabled
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 
                        displayTemporaryStatus(tempUserPosition); //display temporary memory status
                        break;
                    }

                    break;


                case capR: //R received

                    shutdownTimer(TimeOut);								//Reset 15S timer
                    while (BusyUART1());

                    if (buffer[1] == cr) //CR received
                    {
                        crlf();
                        putsUART1(RUsure); //Are you sure(Y/N)?

                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF);
                        if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                            handleCOMError(); //if communications error

                        response = ReadUART1(); //get the char from the USART buffer
                        putcUART1(response); //echo char to terminal					
                        while (BusyUART1());

                        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF);
                        if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                            handleCOMError(); //if communications error

                        RxData = ReadUART1(); //RxData contains user response

                        if (response == capY && RxData == cr) //yes, so clear memory
                        {
                            FRAM_MEMORY.flags.memEmpty = 1; //set the memory empty flag
                            write_Int_FRAM(FRAM_MEMORYflagsaddress,FRAM_MEMORY.memory);	//store flag in FRAM  
                            resetMemory(); //clear FRAM data memory and reset pointers
                            PORT_CONTROL.flags.O0issued = 0; //clear O0 issued flag
                            PORT_CONTROL.flags.CPTime = 0; //clear CPTime flag
                            write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in FRAM  
                            setClock(0x0F, 0); //Clear the RTC Alarm flags
                            crlf();
                            putsUART1(MEMcleared); //Memory cleared
                            while (BusyUART1());
                            break;
                        } else {
                            crlf();
                            putsUART1(MEMnotcleared); //Memory not cleared
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
                            LC2CONTROL.flags.Reset = 1; //Set the Reset flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                            crlf();
                            asm("RESET");
                        }
                    }

                    break;

                case capS:

                    shutdownTimer(TimeOut);								//Reset 15S timer
                    while (BusyUART1());

                    if (buffer[1] == cr) //S received
                    {
                        restoreSettings();

                        if ((!DISPLAY_CONTROL.flags.newPointer && !DISPLAY_CONTROL.flags.BPD &&
                                !DISPLAY_CONTROL.flags.Backup && DISPLAY_CONTROL.flags.Scan)
                                ||
                                (!DISPLAY_CONTROL.flags.newPointer && !DISPLAY_CONTROL.flags.BPD &&
                                !DISPLAY_CONTROL.flags.Backup && !DISPLAY_CONTROL.flags.Scan))
                            displayMemoryStatus(); //Display the memory status from FRAM

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
                            displayTemporaryStatus(id); //working with P,B and D commands

                        crlf();

                        //VER 6.0.0:
                        if (MUX4_ENABLE.mflags.mux16_4 == Single)
                            putsUART1(MUX1);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW4) //Display MUX type  VER 6.0.7
                            putsUART1(MUX4);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW8) //VER 6.0.11
                            putsUART1(MUX8VW);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW16)
                            putsUART1(MUX16);
                        if (MUX4_ENABLE.mflags.mux16_4 == VW32) //VER 6.0.11
                            putsUART1(MUX32VW);
                        if (MUX4_ENABLE.mflags.mux16_4 == TH8) //VER 6.0.7
                            putsUART1(MUX8);
                        if (MUX4_ENABLE.mflags.mux16_4 == TH32) //VER 6.0.9
                            putsUART1(MUX32);


                        while (BusyUART1());
                        crlf();

                        putsUART1(Scaninterval); //Scan interval:
                        while (BusyUART1());
                        ScanInterval = hms2s(); //convert h,m,s to s
                        displayScanInterval(ScanInterval, 1); //display scan interval in seconds


                        if (LC2CONTROL.flags.Logging && !LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL2.flags2.Waiting) {
                            crlf(); //display logging status
                            putsUART1(Loggingstarted); //Logging Started
                        }

                        if (!LC2CONTROL.flags.Logging && !LC2CONTROL.flags.LoggingStopTime &&!LC2CONTROL.flags.LoggingStartTime) {
                            crlf(); //display logging status
                            putsUART1(Loggingstopped); //Logging Stopped
                        }
                        while (BusyUART1());

                        if (LC2CONTROL2.flags2.Waiting) //display logging start time	
                        {
                            crlf();
                            displayLoggingWillStart();
                        }

                        if (LC2CONTROL.flags.LoggingStopTime) //display logging stop time
                        {
                            LC2CONTROL2.flags2.SetStopTime = 1; //set the flag to format the stop time
                            crlf();
                            displayLoggingWillStop();
                            LC2CONTROL2.flags2.SetStopTime = 0; //reset the flag
                        }

                        crlf(); //display log interval status

                        if (LC2CONTROL.flags.LogInterval)
                            putsUART1(Logenabled);
                        else
                            putsUART1(Logdisabled);
                        while (BusyUART1());

                        crlf(); //display monitor status

                        if (LC2CONTROL.flags.Monitor)
                            putsUART1(Monitorenabled);
                        else
                            putsUART1(Monitordisabled);
                        while (BusyUART1());

                        break;
                    }

                    if (buffer[1] == capC && buffer[2] == cr) //SC received
                    {
                        crlf();
                        putsUART1(Scaninterval); //Scan Interval:
                        while (BusyUART1());
                        ScanInterval = hms2s(); //convert h,m,s to s
                        displayScanInterval(ScanInterval, 1); //display scan interval in seconds
                        break;
                    }

                    if (buffer[1] == capC && buffer[2] != cr) //Enter Scan Interval
                    {
                        crlf();
                        testScanInterval = 0; //set testScanInterval to 0
                        LC2CONTROL.flags.ScanError = 0; //clear the flag	

                        if ((buffer[2] >= zero && buffer[2] <= nine) && buffer[3] == cr) //SC#<CR>?
                        {
                            ScanInterval = buffer[2] - 0x30; //convert to integer
                            testScanInterval = checkScanInterval(); //test for minimum allowable Scan Interval
                        }

                        if ((buffer[2] >= zero && buffer[2] <= nine) && (buffer[3] >= zero && buffer[3] <= nine) && buffer[4] == cr) //SC##<CR>?
                        {
                            tensScanInterval = (buffer[2] - 0x30)*10; //scan interval 10's convert to integer
                            ScanInterval = buffer[3] - 0x30; //scan interval 1's convert to integer
                            ScanInterval += tensScanInterval; //add 10's to 1's
                            testScanInterval = checkScanInterval(); //test for minimum allowable Scan Interval
                        }

                        if (testScanInterval) //if error
                        {
                            putsUART1(Minscanerror); //ERROR: Minumum Scan Interval for this configuration is :
                            while (BusyUART1());
                            sprintf(trapBUF, "%d", testScanInterval); //minimum scan interval in seconds
                            putsUART1(trapBUF);
                            while (BusyUART1());
                            putsUART1(Seconds);
                            while (BusyUART1());
                        } else //scan interval is ok
                        {
                            hms(Buffer2Decimal(buffer, i, 1), 0); //convert to hours,minutes,seconds
                            ScanInterval = hms2s(); //convert h,m,s to s
                            putsUART1(Scaninterval); //Scan Interval:
                            while (BusyUART1());
                            displayScanInterval(ScanInterval, 1); //display scan interval in seconds

                            if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) //is datalogger logging?
                            {
                                DISPLAY_CONTROL.flags.Shutup = 1; //don't allow message display
                                stopLogging(); //stop and restart logging if it is	
                                if (DISPLAY_CONTROL.flags.Synch)
                                    VWflagsbits.synch = 1; //set the synch flag
                                crlf();
                                startLogging();
                                DISPLAY_CONTROL.flags.Shutup = 0;
                            }
                        }

                        for (i = 0; i < 30; i++) //clear the buffer
                        {
                            buffer[i] = 0;
                        }

                        break;
                    }

                    //***************************************Stop Logging****************************************************

                    if (buffer[1] == capP && buffer[2] == cr) //SP received
                    {
                        crlf();

                        if (!LC2CONTROL.flags.Logging && !LC2CONTROL.flags.LoggingStopTime &&!LC2CONTROL.flags.LoggingStartTime) {
                            putsUART1(Loggingalreadystopped); //Logging already stopped!
                            while (BusyUART1());
                            break;
                        }

                        if (LC2CONTROL.flags.Logging || LC2CONTROL.flags.LoggingStartTime || LC2CONTROL.flags.LoggingStopTime) //is a logging flag set?
                        {
                            stopLogging();
                            if (!DISPLAY_CONTROL.flags.Shutup) {
                                putsUART1(Loggingstopped); //Logging stopped
                                while (BusyUART1());
                            }
                            break;
                        }

                        configUARTsleep();

                    }

                    if (buffer[1] == capP && isdigit(buffer[2])) //Enter Stop Logging Time
                    {
                        if (LC2CONTROL.flags.LogInterval) {
                            crlf();
                            putsUART1(StopTimeNotAllowed);
                            while (BusyUART1());
                            break;
                        }

                        crlf();

                        if (!LC2CONTROL.flags.LoggingStartTime && !LC2CONTROL.flags.Logging) {
                            putsUART1(Loggingalreadystopped); //Logging already stopped!
                            while (BusyUART1());
                            break;
                        }

                        LC2CONTROL.flags.LoggingStopTime = 1; //set the LoggingStopTime flag		
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        LC2CONTROL2.flags2.SetStopTime = 1; //set the SetStopTime flag

                        Buf2DateTime(buffer); //get Logging Stop Time from buffer

                        if (LC2CONTROL.flags.ERROR) {
                            LC2CONTROL.flags.ERROR = 0; //clear the ERROR flag
                            LC2CONTROL.flags.LoggingStopTime = 0; //clear the LoggingStopTime flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                            LC2CONTROL2.flags2.SetStopTime = 0; //clear the SetStopTime flag
                            break;
                        }

                        LC2CONTROL2.flags2.SetStopTime = 0; //clear the SetStopTime flag

                        write_Int_FRAM(LoggingStopHoursaddress,LoggingStopHours);	//store logging stop time in FRAM 
                        write_Int_FRAM(LoggingStopMinutesaddress,LoggingStopMinutes);  
                        write_Int_FRAM(LoggingStopSecondsaddress,LoggingStopSeconds);  

                        TotalStopSeconds = ((LoggingStopHours * 3600)+(LoggingStopMinutes * 60) + LoggingStopSeconds);
                        TotalStartSeconds=read_longFRAM(TotalStartSecondsaddress); 

                        if (TotalStopSeconds < TotalStartSeconds) //midnight rollover?	
                            TotalStopSeconds += 86400; //compensate
                        write_longFRAM(TotalStopSeconds,TotalStopSecondsaddress);	//store to FRAM   

                        displayLoggingWillStart(); //display logging start time
                        crlf();
                        displayLoggingWillStop(); //display logging stop time
                        break;
                    }

                    //**********************************************System Status************************************************
                    if (buffer[1] == capS && buffer[2] == cr) {
                        crlf();
                        putsUART1(Trapcount); //Display value in Trapcount 
                        while (BusyUART1());
                        trap=read_Int_FRAM(TrapRegisteraddress);		//TEST LC2MUX   
                        sprintf(trapBUF, "%d", trap);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        crlf();
                        NAdata=read_Int_FRAM(Netaddress);			//read Network Address from FRAM  
                        putsUART1(NetworkaddressIS); //"Network address:"
                        while (BusyUART1());
                        sprintf(NABUF, "%d", NAdata); //format network address
                        putsUART1(NABUF); //display network address
                        while (BusyUART1());
                        crlf();

                        if (LC2CONTROL.flags.NetEnabled) {
                            putsUART1(Networkenabled);
                        } else {
                            putsUART1(Networkdisabled);
                        }
                        while (BusyUART1());
                        crlf();

                        if (LC2CONTROL.flags.TimeFormat) //0=hhmm format
                        { //1=hh,mm format
                            putsUART1(Timeformatcomma);
                        } else {
                            putsUART1(Timeformat);
                        }
                        while (BusyUART1());
                        crlf();

                        if (LC2CONTROL.flags.DateFormat) //0=julian date format
                        { //1=month/day format
                            putsUART1(Datemonthday);
                        } else {
                            putsUART1(Datejulian);
                        }
                        while (BusyUART1());

                        break;
                    }


                    //**********************************************Start Logging************************************************
                    if (buffer[1] == capT && buffer[2] == cr) //ST received
                    {
                        if (USB_PWR)
                            LC2CONTROL.flags.USBpower = 1; //set flag if powered by USB
                        else
                            LC2CONTROL.flags.USBpower = 0;

                        IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt	
                        LC2CONTROL2.full2=read_Int_FRAM(LC2CONTROL2flagsaddress);	//restore flags from FRAM     
                        IEC1bits.INT1IE = 1; //re-enable INT2 interrupt	

                        delay(100);
                        crlf();
                        testScanInterval = 0; //set ScanInterval to 0

                        ScanInterval = hms2s(); //get the stored scan interval

                        testScanInterval = checkScanInterval(); //test for minimum allowable Scan Interval

                        if (testScanInterval) //if error
                        {
                            if (LC2CONTROL.flags.LogInterval) //Log Intervals?
                            {
                                putsUART1(MinInterror); //ERROR: Minumum Log Interval Length for this configuration is :
                            } else {
                                putsUART1(Minscanerror); //ERROR: Minumum Scan Interval for this configuration is :
                            }
                            while (BusyUART1());
                            sprintf(trapBUF, "%d", testScanInterval); //minimum scan interval in seconds
                            putsUART1(trapBUF);
                            while (BusyUART1());
                            putsUART1(Seconds);
                            while (BusyUART1());
                            break;
                        }

                        if (!LC2CONTROL.flags.Logging) //is Logging flag clear?
                        {
                            if (DISPLAY_CONTROL.flags.Synch)
                                VWflagsbits.synch = 1;
                            LC2CONTROL2.flags2.FirstReading = 1; //set the first reading flag
                            LC2CONTROL2.flags2.interrupt = 0; //clear the INT2 interrupt flag
                            IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
                            write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flags in FRAM 
                            IEC1bits.INT1IE = 1; //re-enable INT2 interrupt
                            startLogging();
                            return;
                        }

                        putsUART1(Loggingalreadystarted); //Logging already started!
                        while (BusyUART1());
                    }


                    if (buffer[1] == capT && buffer[2] != cr) //Enter Start Logging Time
                    {
                        crlf();

                        testScanInterval = 0;

                        ScanInterval = hms2s(); //get the stored scan interval
                        testScanInterval = checkScanInterval(); //test for minimum allowable Scan Interval

                        if (testScanInterval) //if error
                        {
                            if (LC2CONTROL.flags.LogInterval) //Log Intervals?
                            {
                                putsUART1(MinInterror); //ERROR: Minimum Log Interval Length for this configuration is :
                            } else {
                                putsUART1(Minscanerror); //ERROR: Minimum Scan Interval for this configuration is :
                            }

                            while (BusyUART1());
                            sprintf(trapBUF, "%d", testScanInterval); //minimum scan interval in seconds
                            putsUART1(trapBUF);
                            while (BusyUART1());
                            putsUART1(Seconds);
                            while (BusyUART1());
                            break;
                        }

                        LC2CONTROL.flags.LoggingStartTime = 1;
                        LC2CONTROL.flags.Logging = 0;
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        LC2CONTROL2.flags2.SetStartTime = 1; //Going to set Start time	
                        DISPLAY_CONTROL.flags.Synch = 0; //make sure Synch flag is clear	
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM     
                        Buf2DateTime(buffer); //set RTC Alarm time

                        if (LC2CONTROL.flags.ERROR) {
                            LC2CONTROL.flags.ERROR = 0; //clear the ERROR flag
                            LC2CONTROL.flags.LoggingStartTime = 0; //clear the LoggingStartTime flag
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                            LC2CONTROL2.flags2.SetStartTime = 0; //clear the SetStartTime flag
                            break;
                        }

                        LC2CONTROL2.flags2.SetStartTime = 0; //clear the set start time flag
                        putsUART1(Loggingwillstart); //Logging will start at:
                        while (BusyUART1());

                        Alarm1HoursValue = readClock(RTCAlarm1HoursAddress); //Display Alarm1 hours MAY BE ABLE TO MAKE FUNCTION HERE
                        displayClock(Alarm1HoursValue);
                        putcUART1(colon); // :
                        while (BusyUART1());

                        Alarm1MinutesValue = readClock(RTCAlarm1MinutesAddress); //Display Alarm1 minutes
                        displayClock(Alarm1MinutesValue);
                        putcUART1(colon);
                        while (BusyUART1());

                        Alarm1SecondsValue = readClock(RTCAlarm1SecondsAddress); //Display Alarm1 seconds
                        displayClock(Alarm1SecondsValue);
                        Alarm1SecondsLong = bcd2d(Alarm1SecondsValue); //convert to decimal from BCD
                        Alarm1MinutesLong = bcd2d(Alarm1MinutesValue);
                        Alarm1HoursLong = bcd2d(Alarm1HoursValue);

                        TotalStartSeconds = ((Alarm1HoursLong * 3600)+(Alarm1MinutesLong * 60) + Alarm1SecondsLong); //compute total start seconds
                        write_longFRAM(TotalStartSeconds,TotalStartSecondsaddress);	//store to FRAM   
                        TotalStopSeconds=read_longFRAM(TotalStopSecondsaddress);		//get the stored stop time  

                        if (TotalStopSeconds < TotalStartSeconds) //midnight rollover?
                        {
                            TotalStopSeconds += 86400; //compensate
                            write_longFRAM(TotalStopSeconds,TotalStopSecondsaddress);	//store to FRAM   
                        }

                        LoggingStartDays = readClock(RTCDaysAddress); //Get and store the current day 
                        LoggingStartDay = RTCtoDecimal(LoggingStartDays);
                        write_Int_FRAM(LoggingStartDayaddress,LoggingStartDay);    

                        ConfigIntCapture8(IC_INT_ON & IC_INT_PRIOR_6); //configure input capture interrupt	
                        enableAlarm(Alarm1);
                        enableINT1(); //enable INT2 (take a reading on interrupt) 
                        configUARTnormal();
                        INTCON1bits.NSTDIS = 0; //reset nesting of interrupts	

                        LC2CONTROL.flags.Logging = 1; //set the Logging flag	
                        LC2CONTROL.flags.LoggingStartTime = 0; //clear the Logging Start Time flag	
                        write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flags in FRAM     

                        LC2CONTROL2.flags2.Waiting = 1; //Set the Waiting flag	
                        write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flags in FRAM     
                    }


                    //*******************************************Software Version*****************************************************

                    if (buffer[1] == capV && buffer[2] == cr) //display software revision
                    {
                        crlf();
                        putsUART1(Softwareversion); //Software version:
                        while (BusyUART1());
                        putsUART1(Rev);
                        while (BusyUART1());
                        break;
                    }


                    //***************************************************************************************************************


                    //**********************************************Synchronize Readings**********************************************
                    if (buffer[1] == capR && buffer[2] == cr) //SR received
                    {
                        crlf();

                        if (DISPLAY_CONTROL.flags.Synch) //If readings will be synchronized
                        {
                            putsUART1(Synch); //"Readings are synchronized to the top of the hour"
                        } else {
                            putsUART1(Synchnot); //"Readings are not synchronized to the top of the hour"
                        }
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capR && buffer[2] == zero && buffer[3] == cr) //SR0 received
                    {
                        crlf();
                        DISPLAY_CONTROL.flags.Synch = 0; //clear the Synch flag
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 
                        putsUART1(Synchnot); //"Readings are not synchronized to the top of the hour"
                        while (BusyUART1());

                        if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) //is datalogger logging?	
                        {
                            DISPLAY_CONTROL.flags.Shutup = 1; //don't allow message display
                            stopLogging(); //stop and restart logging if it is
                            if (DISPLAY_CONTROL.flags.Synch)
                                VWflagsbits.synch = 1; //set the synch flag
                            crlf();
                            startLogging();
                            DISPLAY_CONTROL.flags.Shutup = 0;
                        }
                        break;
                    }

                    if (buffer[1] == capR && buffer[2] == one && buffer[3] == cr) //SR1 received
                    {
                        crlf();
                        DISPLAY_CONTROL.flags.Synch = 1; //set the Synch flag
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 
                        putsUART1(Synch); //"Readings are synchronized to the top of the hour"
                        while (BusyUART1());

                        if (LC2CONTROL.flags.Logging && !LC2CONTROL2.flags2.Waiting) //is datalogger logging?	
                        {
                            DISPLAY_CONTROL.flags.Shutup = 1; //don't allow message display
                            stopLogging(); //stop and restart logging if it is
                            if (DISPLAY_CONTROL.flags.Synch)
                                VWflagsbits.synch = 1; //set the synch flag
                            crlf();
                            startLogging();
                            DISPLAY_CONTROL.flags.Shutup = 0;
                        }
                        break;
                    }


                case capT:

                    shutdownTimer(TimeOut);						//Reset 15S timer
                    crlf();

                    if (buffer[1] == capR && buffer[2] == cr) //TR<CR> received
                    {
                        putsUART1(Trapcount); //Display value in Trapcount
                        while (BusyUART1());
                        trap=read_Int_FRAM(TrapRegisteraddress); 
                        sprintf(trapBUF, "%d", trap);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capR && buffer[2] == zero && buffer[3] == cr) //reset Trap Counter
                    {
                        trap = 0; //reset trap counter
                        write_Int_FRAM(TrapRegisteraddress,trap);  
                        putsUART1(Trapcount); //Display value in Trapcount
                        while (BusyUART1());
                        sprintf(trapBUF, "%d", trap);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[2] == cr) //TF<CR> received
                    {
                        if (!LC2CONTROL.flags.TimeFormat) //0=hhmm
                            putsUART1(Timeformat);
                        else
                            putsUART1(Timeformatcomma); //1=hh,mm
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[3] == cr) //TF#<CR> received
                    {
                        if (buffer[2] == zero) //hhmm format chosen
                        {
                            LC2CONTROL.flags.TimeFormat = 0;
                            putsUART1(Timeformat);
                            write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  
                        }

                        if (buffer[2] == one) //hh,mm format chosen
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

                        while (testmenuBUF[0] != capX) {
                            for (i = 0; i < 2; i++) //clear the buffer
                            {
                                testmenuBUF[i] = 0;
                            }

                            for (i = 0; i < 22; i++) //clear the buffer
                            {
                                buffer[i] = 0;
                            }

                            i = 0; //initialize char buffer index

                            RxData = 0; //initialize receive buffer

                            displayTestMenu(); //display the test choices

                            while (RxData != cr) //put chars in buffer until <CR> is received
                                while (1) //put chars in buffer until <CR> is received
                                {
                                    while (!DataRdyUART1() && !U1STAbits.FERR //wait for choice
                                            && !U1STAbits.PERR && !U1STAbits.OERR);
                                    if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                                        handleCOMError();

                                    RxData = ReadUART1(); //get the char from the USART buffer

                                    if (RxData != 0x08 && RxData != cr) {
                                        putcUART1(RxData); //echo char (except backspace & <CR>) to terminal
                                        while (BusyUART1());
                                        putcUART1(0x08); //restore cursor to original location
                                        while (BusyUART1());
                                        testmenuBUF[0] = RxData; //store the received char in the buffer
                                    }

                                    if (testmenuBUF[0] != 0 && RxData == cr) //break out of loop if selection made
                                        break;
                                }


                            switch (testmenuBUF[0]) //perform the test
                            {

                                case one: //test ALL FRAM
                                    crlf();
                                    testData = testFRAM(1); //test external FRAM bank 1
                                    handleFRAMResults(testData); //display the results
                                    crlf();
                                    testData = testFRAM(2); //test external FRAM bank 2
                                    handleFRAMResults(testData); //display the results
                                    crlf();
                                    testData = testFRAM(3); //test external FRAM bank 3
                                    handleFRAMResults(testData);
                                    crlf();
                                    testData = testFRAM(4); //test external FRAM bank 4
                                    handleFRAMResults(testData);
                                    crlf();
                                    RxData = 0;
                                    break;

                                case two: //test +3V_X
                                    ADPCFG = 0; //all analog channels
                                    //ADCON2=0;						//VDD & VSS are references
                                    _3VX_on(); //power-up analog circuitry
                                    crlf();
                                    putsUART1(Poweron); //display "Analog power supplies on"
                                    while (BusyUART1());
                                    IFS0bits.U1RXIF = 0; //clear the UART1 interrupt flag
                                    while (!IFS0bits.U1RXIF); //wait until a key is pressed
                                    if (IFS0bits.U1RXIF) //TEST POINT
                                        IFS0bits.U1RXIF = 0; //clear the UART1 interrupt flag
                                    RxDataTemp = ReadUART1(); //get the char from the UART buffer to clear it
                                    _3VX_off(); //power-down the analog circuitry
                                    putcUART1(0x0D); //<CR>
                                    while (BusyUART1());
                                    putsUART1(Poweroff); //display "Analog power supplies off"
                                    while (BusyUART1());
                                    RxData = 0;
                                    break;

                                case three: //test RTC 32KHz
                                    crlf();
                                    putsUART1(RTC32KHZON); //display "RTC 32KHz output on."
                                    while (BusyUART1());
                                    start32KHz(); //start the RTC 32KHz output
                                    IFS0bits.U1RXIF = 0; //clear the UART1 interrupt flag
                                    while (!IFS0bits.U1RXIF); //wait until a key is pressed
                                    IFS0bits.U1RXIF = 0; //clear the UART1 interrupt flag
                                    RxDataTemp = ReadUART1(); //get the char from the UART buffer to clear it
                                    stop32KHz(); //stop the RTC 32KHz output
                                    putcUART1(0x0D); //<CR>
                                    while (BusyUART1());
                                    putsUART1(RTC32KHZOFF); //display "RTC 32KHz output off."
                                    while (BusyUART1());
                                    RxData = 0;
                                    break;

                                case capX:
                                    RxData = 0;
                                    break; //exit Test Menu

                                default:
                                    RxData = 0;
                                    break;
                            } //end of switch(buffer[0])

                            if (RxData == 0) {
                                delay(80000); //a little aesthetic delay	VER 2.4.0
                                crlf(); //put space between tests
                                crlf();
                                crlf();
                            }
                        } //end of while(testmenuBUF[0]!=capX)
                        testmenuBUF[0] = 0; //clear the buffer
                        break;
                    } //end of if(buffer[1]==capE && buffer[2]==capS && buffer[3]==capT)


                    break;

                    //***********************************************************************************************************


                case capV: //Lithium Cell Voltage

                    if (buffer[1] == capL && buffer[2] == cr) //VL<CR>
                    {
                        crlf();
                        lithBatreading = take_analog_reading(95); //get the lithium coin cell voltage
                        putsUART1(LithiumVoltage); //display "Lithium Cell Voltage ="
                        Lithiumreading = (((Vref * lithBatreading) / 4096.0) * mullith3V); //convert to voltage	lithBatreading; VER 6.0.0
                        while (BusyUART1());
                        sprintf(trapBUF, "%.2f", Lithiumreading);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        putcUART1(capV);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == three && buffer[2] == cr) //V3<CR>
                    {
                        crlf();
                        mainBatreading = take_analog_reading(97); //get the 3V Battery Voltage
                        putsUART1(THREEV); //display "3V Battery Voltage ="
                        batteryReading = (((Vref * mainBatreading) / 4096.0) * mul3V); //convert to voltage	lithBatreading; VER 6.0.2
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
                        mainBatreading = take_analog_reading(87); //get the 12V Battery Voltage
                        putsUART1(TWELVEV); //display "12V Battery Voltage ="
                        batteryReading = (((Vref * mainBatreading) / 4096.0) * mul12V); //convert to voltage    VER 6.0.2
                        while (BusyUART1());
                        sprintf(trapBUF, "%.2f", batteryReading);
                        putsUART1(trapBUF);
                        while (BusyUART1());
                        putcUART1(capV);
                        while (BusyUART1());
                    }

                    break;

                    /*  REM VER 6.0.5:
            case capU:

                        if(buffer[1]==capF && buffer[2]==cr)				//Update Firmware
                        {
                            if(LC2CONTROL.flags.Logging | LC2CONTROL2.flags2.Waiting)
                            {
                                crlf();
                                putsUART1(FUWhileLogging);
                                while(BusyUART1());
                                break;
                }							

                crlf();
                putsUART1(UpdateFirmware);						//Update Firmware(Y/N)?
                while(BusyUART1());

                while(!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF);	//wait for response
                if(U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                                handleCOMError();							//if communications error

                response=ReadUART1();							//get the char from the USART buffer
                putcUART1(response);							//echo char to terminal
                while(BusyUART1());

                while(!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF);	
                if(U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                                handleCOMError();							//if communications error

                RxData=ReadUART1();								//RxData contains user response

                if(response==capY && RxData==cr)				//yes, so wait for 0xC1 from TinyBLD
                {
                                shutdownTimer(FUTimeOut);					//start 30S background timer
                                crlf();
                                putsUART1(WaitingForUpdate);				//"Waiting for Update..."
                                while(BusyUART1());

                                while(!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF);
                                if(U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
                                    handleCOMError();							//if communications error

                                if(IFS1bits.T5IF)							//timed-out
                                {
                                    crlf();
                    putsUART1(TimeOutFirmwareNotUpdated);
                    while(BusyUART1());
                    break;
                                }

                                RxData=ReadUART1();								//RxData should contain 0xC1

                                if(RxData==0xC1)								//received 0xC1
                                {
                                    LC2CONTROL.flags.Reset=0;					//make sure Reset flag is clear
                    LC2CONTROL2.flags2.FU=1;						//set the FU flag
                    FRAMTest=write_intFRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM
                    FRAMTest=write_intFRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flag in FRAM
                    asm("RESET");								//reset processor
                                }
                                else
                                {
                                    crlf();
                    putsUART1(FirmwareNotUpdated);					//"Firmware not Updated"
                    while(BusyUART1());
                    break;
                                }
                            }
                else
                {
                                crlf();
                                putsUART1(FirmwareNotUpdated);					//"Firmware not Updated"
                                while(BusyUART1());
                                break;
                }

                        }
                     */

                    //***********************************************************************************************************
                case capW: //Wrap Format

                    if (buffer[1] == capF && buffer[2] == zero && buffer[3] == cr) //stop logging when memory full
                    {
                        crlf();
                        shutdownTimer(TimeOut);						//Reset 15S timer
                        DISPLAY_CONTROL.flags.WrapMemory = 0; //clear the wrap memory flag
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 
                        putsUART1(Loggingwillstopmemfull);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[2] == one && buffer[3] == cr) //wrap memory when full
                    {
                        crlf();
                        shutdownTimer(TimeOut);						//Reset 15S timer
                        DISPLAY_CONTROL.flags.WrapMemory = 1; //set the wrap memory flag
                        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM`
                        putsUART1(Loggingwillnotstopmemfull);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == capF && buffer[2] == cr) //WF<CR> received
                    {
                        crlf();
                        shutdownTimer(TimeOut);						//Reset 15S timer
                        if (DISPLAY_CONTROL.flags.WrapMemory)
                            putsUART1(Loggingwillnotstopmemfull);
                        else
                            putsUART1(Loggingwillstopmemfull);
                        while (BusyUART1());
                        break;
                    }

                    break;


                    //***********************************************************************************************************	
                case capX: //take and display, but don't store a reading

                    if (buffer[1] == cr) {
                        LC2CONTROL2.flags2.X = 1; //set the 'X' flag

                        crlf();
                        ConfigIntCapture8(IC_INT_ON & IC_INT_PRIOR_6); //Configure input capture interrupt 
                        LC2CONTROL2.flags2.interrupt = 0; //clear the INT2 interrupt flag	
                        take_One_Complete_Reading(NOSTORE); //take a reading    VER 6.0.13
                        LC2CONTROL2.flags2.X = 0; //clear the 'X' flag	

                        INTCON1bits.NSTDIS = 0; //Reset nesting of interrupts 

                        RxData = ReadUART1(); //clear the UART buffer	
                        RxData = ReadUART1(); //clear the UART buffer	
                        RxData = ReadUART1(); //clear the UART buffer	
                        RxData = ReadUART1(); //clear the UART buffer
                        RxData = cr; //load <CR> into RxData	

                    }

                    break;


                case pound: //prevent bus collision if networked and not shut down before	

                    for (i = 1; buffer[i] != cr; i++) {
                        if (!isdigit(buffer[i]))
                            break; //ERROR - buffer[i] is not a digit
                    }

                    i--; //# of digits in network address

                    switch (i) {
                        case 1:
                            NetAddress = buffer[1] - 0x30; //address 1-9
                            break;
                        case 2:
                            tens = (buffer[1] - 0x30)*10; //address 10-99
                            ones = buffer[2] - 0x30;
                            NetAddress = tens + ones;
                            break;
                        case 3:
                            hundreds = (buffer[1] - 0x30)*100; //address 100-256
                            tens = (buffer[2] - 0x30)*10;
                            ones = buffer[3] - 0x30;
                            NetAddress = hundreds + tens + ones;
                            break;
                        default:
                            break;
                    }

                    if (NetAddress <= 0 | NetAddress > 256) //invalid addresses
                        break;

                    StoredNetAddress=read_Int_FRAM(Netaddress);  

                    if (NetAddress != StoredNetAddress && LC2CONTROL.flags.NetEnabled) //address doesn't match
                    {
                        crlf();
                        IFS1bits.T5IF = 1; //set the T5IF to break out on Comm() in main()
                        DISPLAY_CONTROL.flags.Shutdown = 1; //set the Shutdown flag	
                        return;
                    }

                    break;


                case tilde: //select R1-R0 or R0-R1 format

                    if (buffer[1] == tilde && buffer[2] == cr) //~~<CR> received
                    {
                        crlf();
                        shutdownTimer(TimeOut);						//Reset 15S timer

                        if (LC2CONTROL2.flags2.R)
                            putsUART1(R1);
                        else
                            putsUART1(R0);
                        while (BusyUART1());
                        break;
                    }

                    if (buffer[1] == tilde && buffer[2] == one && buffer[3] == cr) //~~1<CR> received
                    {
                        crlf();
                        shutdownTimer(TimeOut);						//Reset 15S timer

                        LC2CONTROL2.flags2.R = 1; //set the R flag
                        write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flag in FRAM`
                        break;
                    }

                    if (buffer[1] == tilde && buffer[2] == zero && buffer[3] == cr) //~~0<CR> received
                    {
                        crlf();
                        shutdownTimer(TimeOut);						//Reset 15S timer

                        LC2CONTROL2.flags2.R = 0; //clear the R flag
                        write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flag in FRAM  
                        break;
                    }

                    break;

            } //end of switch

            prompt();
            for (i = 0; i < 22; i++) //clear the buffer
            {
                buffer[i] = 0;
            }

            i = 0; //re-initialize buffer pointer


        } //end of 2nd while(!IFS1bits.T5IF)

    } //end of 1st while(!IFS1bits.T5IF)

} //end of comm()

void config_Ports_Low_Power(void) {
    TRISA = 0x1400; //RA12 & RA10 is input
    LATA = 0x0000;

    TRISC = 0x0000;
    LATC = 0x0000;

    TRISD = 0xF000; //configure port D:bits 15-12 as input, bits 11-0 as output

    if (!CONTROL) //PORT was OFF
        LATD = 0x0302; //All low except 3.3VXMEM, WP & READ LED    VER 6.0.4
    else
        LATD = 0x0342; //PORT was ON

    TRISF = 0x0004; //RF2 is input
    LATF = 0x0008; //RF3 is output high

    TRISG = 0x0001; //configure port G: all output
    if (netTest) {
        PORTG = 0x000C; //drive all outputs low except SDA and SCL   VER 6.0.4
        //LATG=0x000C;                                                             //drive all outputs low except SDA and SCL
    } else {
        PORTG = 0x004C; //drive all outputs low except _RS485TX_EN, SDA and SCL
        //LATG=0x004C;                                                           //drive all outputs low except _RS485TX_EN, SDA and SCL
    }

    WP = 1; //VER 6.0.4

    ADPCFG = 0; //all analog channels
    //ADCON2=0;                                                                   //VDD & VSS are references
    //ADCON1bits.ADON=0;                                                          //make sure ADC off
}

void configUARTnormal(void) {
    unsigned int U1MODEvalue;
    unsigned int U1STAvalue;

    //ConfigIntUART1(UART_RX_INT_DIS & //configure the UART
    //        UART_RX_INT_PR5 &
    //        UART_TX_INT_DIS &
    //        UART_TX_INT_PR5);

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

    OpenUART1(U1MODEvalue, U1STAvalue, baudrate);
    U1MODEbits.URXINV=0;                                                        //idle high
    U1MODEbits.BRGH=0;                                                          //low speed mode
    SRbits.IPL = 0; //Set CPU Interrupt priority to 0
    U1STAbits.FERR = 0; //clear UART error flags
    U1STAbits.PERR = 0;
    U1STAbits.OERR = 0;
}

void configUARTsleep(void) {
    unsigned int U1MODEvalue;
    unsigned int U1STAvalue;

    ConfigIntUART1(UART_RX_INT_EN & //configure the UART
            UART_RX_INT_PR1 &
            UART_TX_INT_DIS &
            UART_TX_INT_PR5);

    U1MODEvalue = UART_EN &
            UART_IDLE_CON &
            UART_EN_WAKE &
            UART_DIS_LOOPBACK &
            UART_DIS_ABAUD &
            UART_NO_PAR_8BIT &
            UART_1STOPBIT;

    U1STAvalue = UART_INT_TX_BUF_EMPTY &
            //UART_TX_PIN_NORMAL &
            UART_TX_ENABLE &
            UART_INT_RX_CHAR &
            UART_ADR_DETECT_DIS &
            UART_RX_OVERRUN_CLEAR;

    OpenUART1(U1MODEvalue, U1STAvalue, baudrate);

    SRbits.IPL = 2; //Set CPU Interrupt priority to 2 so
    //wakeup due to UART1 Rx will cause execution
    //to resume & not vector
}

void configVWchannel(unsigned char gageType) {
    Nop();
}

void controlPortStatus(void) {
    char eeBUF[6]; //temporary storage for EE values
    unsigned char Alarm2HoursValue;
    unsigned char Alarm2MinutesValue;


    crlf();
    PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress);   

    if (CONTROL) //Port is currently ON
    {
        putsUART1(CPON); //"Control Port ON"
    } else {
        putsUART1(CPOFF); //"Control Port OFF"
    }
    while (BusyUART1());
    crlf();

    if (PORT_CONTROL.flags.PortTimerEN) //Port Timer Enabled
    {
        putsUART1(CPTIMEREN); //"Control Port Timer Enabled."
    } else {
        putsUART1(CPTIMERDIS); //"Control Port Timer Disabled."
    }
    while (BusyUART1());
    crlf();

    putsUART1(CPTIMERACT); //"Control Port Timer activate at "
    while (BusyUART1());

    PortOnHours=read_Int_FRAM(PortOnHoursaddress);   
    if (PortOnHours < 10) //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(eeBUF, "%d", PortOnHours); //format the hour data
    putsUART1(eeBUF); //display it
    while (BusyUART1());
    putcUART1(colon); // :
    while (BusyUART1());
    PortOnMinutes=read_Int_FRAM(PortOnMinutesaddress);   
    if (PortOnMinutes < 10) //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(eeBUF, "%d", PortOnMinutes); //format the minute data
    putsUART1(eeBUF); //display it
    while (BusyUART1());

    crlf();

    putsUART1(CPTIMERDEACT); //"Control Port Timer deactivate at "
    while (BusyUART1());
    PortOffHours=read_Int_FRAM(PortOffHoursaddress); 
    if (PortOffHours < 10) //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(eeBUF, "%d", PortOffHours); //format the hour data
    putsUART1(eeBUF); //display it
    while (BusyUART1());
    putcUART1(colon); // :
    while (BusyUART1());
    PortOffMinutes=read_Int_FRAM(PortOffMinutesaddress); 
    if (PortOffMinutes < 10) //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(eeBUF, "%d", PortOffMinutes); //format the minute data
    putsUART1(eeBUF); //display it
    while (BusyUART1());
    crlf();
}

void crlf(void) //transmit <CR><LF>
{
    putcUART1(cr);
    while (BusyUART1());
    putcUART1(lf);
    while (BusyUART1());
}

unsigned int debounce(void) {
    delay(7373); //delay 1mS
    if (!_CLK_INT) //RTC INTERRUPT valid
        return 1;
    return 0; //RTC INTERRUPT not valid
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

    write_Int_FRAM(MUX_ENABLE1_16flagsaddress,MUX_ENABLE1_16.MUXen1_16);              //store flag in FRAM  
    write_Int_FRAM(MUX_ENABLE17_32flagsaddress,MUX_ENABLE17_32.MUXen17_32);             //store flag in FRAM  
}

//REM REV B:
//void disableINT2(void) {
//    IFS1bits.INT2IF = 0; //clear the interrupt flag
//    IEC1bits.INT2IE = 0; //enable the interrupt
//}

//REV B:
void disableINT1(void) 
{
    IFS1bits.INT1IF = 0;                                                        //clear the interrupt flag
    IEC1bits.INT1IE = 0;                                                        //enable the interrupt
}

void displayBCD(void) //convert BCDtens and BCDones to ascii
{ //and transmit out UART1
    if (LC2CONTROL2.flags2.SetStopTime | (LC2CONTROL2.flags2.GageDisplay && BCDtens >= 1)) {
        putcUART1(BCDtens + 0x30); //format leading zero in stop time
        while (BusyUART1());
    }
    putcUART1(BCDones + 0x30);
    while (BusyUART1());
}

void displayFRAMError(int errorAddress) //REM VER 6.0.0
{
    char ERRORBUF[4];

    crlf();
    putsUART1(Errors); //"Errors detected at 0x"
    while (BusyUART1());
    sprintf(ERRORBUF, "%d", errorAddress); //format the address of the FRAM error
    putsUART1(ERRORBUF); //display it
    while (BusyUART1());
    crlf();
}

/*
void displayExternalAddress(unsigned int x)	//FOR DEBUG
{
    char addressBUF[10];

    putcUART1(lessthan);					// < DELIMITER
    while(BusyUART1());

    sprintf(addressBUF,"%d",x);				//format external FRAM address
    putsUART1(addressBUF);					//display external FRAM address
    while(BusyUART1());

    putcUART1(greaterthan);					// > DELIMITER
    while(BusyUART1());
}
 */

void displayGageInfo(int channel) //display the gage information
{
    unsigned int address;
    unsigned int ZeroReadingaddress;
    unsigned int GageFactoraddress;
    unsigned int GageOffsetaddress;
    unsigned int PolyCoAaddress;
    unsigned int PolyCoBaddress;
    unsigned int PolyCoCaddress;
    float TEMPVAL;

    crlf();
    putsUART1(GT); //GT:
    while (BusyUART1()); //Display gage info from FRAM here

    address = CH1GTaddress + (0x1A * (channel - 1)); //get the channel's gage type address

    LC2CONTROL2.flags2.GageDisplay = 1; //set the gage display flag
    toBCD(read_Int_FRAM(address));                                           
    displayBCD();
    LC2CONTROL2.flags2.GageDisplay = 0; //clear the gage display flag
    LC2CONTROL.flags.Conversion = 0; //clear the conversion flag

    switch (channel) //get the gage information
    {
        case 1:
            if (MUX_CONVERSION1_16.c1flags.CH1) //Polynomial Conversion?
            {
                PolyCoAaddress = CH1PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH1ZRaddress; //set the ZeroReadingaddress
            }

            break;


        case 2:
            if (MUX_CONVERSION1_16.c1flags.CH2) //Polynomial Conversion?
            {
                PolyCoAaddress = CH2PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH2ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 3:
            if (MUX_CONVERSION1_16.c1flags.CH3) //Polynomial Conversion?
            {
                PolyCoAaddress = CH3PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH3ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 4:
            if (MUX_CONVERSION1_16.c1flags.CH4) //Polynomial Conversion?
            {
                PolyCoAaddress = CH4PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH4ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 5:
            if (MUX_CONVERSION1_16.c1flags.CH5) //Polynomial Conversion?
            {
                PolyCoAaddress = CH5PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH5ZRaddress; //set the ZeroReadingaddress
            }


            break;


        case 6:
            if (MUX_CONVERSION1_16.c1flags.CH6) //Polynomial Conversion?
            {
                PolyCoAaddress = CH6PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH6ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 7:
            if (MUX_CONVERSION1_16.c1flags.CH7) //Polynomial Conversion?
            {
                PolyCoAaddress = CH7PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH7ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 8:
            if (MUX_CONVERSION1_16.c1flags.CH8) //Polynomial Conversion?
            {
                PolyCoAaddress = CH8PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH8ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 9:

            if (MUX_CONVERSION1_16.c1flags.CH9) //Polynomial Conversion?
            {
                PolyCoAaddress = CH9PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH9ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 10:

            if (MUX_CONVERSION1_16.c1flags.CH10) //Polynomial Conversion?
            {
                PolyCoAaddress = CH10PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH10ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 11:

            if (MUX_CONVERSION1_16.c1flags.CH11) //Polynomial Conversion?
            {
                PolyCoAaddress = CH11PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH11ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 12:

            if (MUX_CONVERSION1_16.c1flags.CH12) //Polynomial Conversion?
            {
                PolyCoAaddress = CH12PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH12ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 13:

            if (MUX_CONVERSION1_16.c1flags.CH13) //Polynomial Conversion?
            {
                PolyCoAaddress = CH13PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH13ZRaddress; //set the ZeroReadingaddress
            }

            break;


        case 14:

            if (MUX_CONVERSION1_16.c1flags.CH14) //Polynomial Conversion?
            {
                PolyCoAaddress = CH14PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH14ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 15:

            if (MUX_CONVERSION1_16.c1flags.CH15) //Polynomial Conversion?
            {
                PolyCoAaddress = CH15PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH15ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 16:

            if (MUX_CONVERSION1_16.c1flags.CH16) //Polynomial Conversion?
            {
                PolyCoAaddress = CH16PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH16ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 17:

            if (MUX_CONVERSION17_32.c2flags.CH17) //Polynomial Conversion?
            {
                PolyCoAaddress = CH17PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH17ZRaddress; //set the ZeroReadingaddress
            }

            break;


        case 18:
            if (MUX_CONVERSION17_32.c2flags.CH18) //Polynomial Conversion?
            {
                PolyCoAaddress = CH18PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH18ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 19:
            if (MUX_CONVERSION17_32.c2flags.CH19) //Polynomial Conversion?
            {
                PolyCoAaddress = CH19PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH19ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 20:
            if (MUX_CONVERSION17_32.c2flags.CH20) //Polynomial Conversion?
            {
                PolyCoAaddress = CH20PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH20ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 21:
            if (MUX_CONVERSION17_32.c2flags.CH21) //Polynomial Conversion?
            {
                PolyCoAaddress = CH21PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH21ZRaddress; //set the ZeroReadingaddress
            }


            break;


        case 22:
            if (MUX_CONVERSION17_32.c2flags.CH22) //Polynomial Conversion?
            {
                PolyCoAaddress = CH22PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH22ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 23:
            if (MUX_CONVERSION17_32.c2flags.CH23) //Polynomial Conversion?
            {
                PolyCoAaddress = CH23PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH23ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 24:
            if (MUX_CONVERSION17_32.c2flags.CH24) //Polynomial Conversion?
            {
                PolyCoAaddress = CH24PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH24ZRaddress; //set the ZeroReadingaddress
            }


            break;

        case 25:

            if (MUX_CONVERSION17_32.c2flags.CH25) //Polynomial Conversion?
            {
                PolyCoAaddress = CH25PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH25ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 26:

            if (MUX_CONVERSION17_32.c2flags.CH26) //Polynomial Conversion?
            {
                PolyCoAaddress = CH26PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH26ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 27:

            if (MUX_CONVERSION17_32.c2flags.CH27) //Polynomial Conversion?
            {
                PolyCoAaddress = CH27PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH27ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 28:

            if (MUX_CONVERSION17_32.c2flags.CH28) //Polynomial Conversion?
            {
                PolyCoAaddress = CH28PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH28ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 29:

            if (MUX_CONVERSION17_32.c2flags.CH29) //Polynomial Conversion?
            {
                PolyCoAaddress = CH29PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH29ZRaddress; //set the ZeroReadingaddress
            }

            break;


        case 30:

            if (MUX_CONVERSION17_32.c2flags.CH30) //Polynomial Conversion?
            {
                PolyCoAaddress = CH30PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH30ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 31:

            if (MUX_CONVERSION17_32.c2flags.CH31) //Polynomial Conversion?
            {
                PolyCoAaddress = CH31PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH31ZRaddress; //set the ZeroReadingaddress
            }

            break;

        case 32:

            if (MUX_CONVERSION17_32.c2flags.CH32) //Polynomial Conversion?
            {
                PolyCoAaddress = CH32PolyCoAaddress; //set the PolyCoAaddress
                LC2CONTROL.flags.Conversion = 1; //set the conversion flag
            } else {
                ZeroReadingaddress = CH32ZRaddress; //set the ZeroReadingaddress
            }

            break;

        default:
            break;
    }


    if (LC2CONTROL.flags.Conversion) //Display polynomial coefficients
    {
        putsUART1(PA);
        while (BusyUART1());
        TEMPVAL=read_longFRAM(PolyCoAaddress);                               //extract coefficient A from FRAM   
        formatandDisplayGageInfo(TEMPVAL);

        putsUART1(PB);
        while (BusyUART1());
        TEMPVAL=read_longFRAM(PolyCoAaddress+0x0004);                        //extract coefficient B from FRAM   
        formatandDisplayGageInfo(TEMPVAL);

        putsUART1(PC);
        while (BusyUART1());
        TEMPVAL=read_longFRAM(PolyCoAaddress+0x0008);                        //extract coefficient C from FRAM   
        formatandDisplayGageInfo(TEMPVAL);
    } else {
        putsUART1(ZR);
        while (BusyUART1());
        TEMPVAL=read_longFRAM(ZeroReadingaddress);                           //extract Zero Reading from FRAM    
        formatandDisplayGageInfo(TEMPVAL);

        putsUART1(GF);
        while (BusyUART1());
        TEMPVAL=read_longFRAM(ZeroReadingaddress+0x0004);                    //extract Gage Factor from FRAM 
        formatandDisplayGageInfo(TEMPVAL);

        putsUART1(GO);
        while (BusyUART1());
        TEMPVAL=read_longFRAM(ZeroReadingaddress+0x0008);                    //extract Gage Offset from FRAM 
        formatandDisplayGageInfo(TEMPVAL);
    }
}

void displayLoggingWillStart(void) {
    putsUART1(Loggingwillstart); //Logging will start at:
    while (BusyUART1());
    RTCdata = readClock(RTCAlarm1HoursAddress); //Display Alarm1 hours MAY BE ABLE TO MAKE FUNCTION HERE
    displayClock(RTCdata);
    putcUART1(colon); // :
    while (BusyUART1());
    RTCdata = readClock(RTCAlarm1MinutesAddress); //Display Alarm1 minutes
    displayClock(RTCdata);
    putcUART1(colon);
    while (BusyUART1());
    RTCdata = readClock(RTCAlarm1SecondsAddress); //Display Alarm1 seconds
    displayClock(RTCdata);
}

void displayLoggingWillStop(void) {
    LC2CONTROL2.flags2.SetStopTime = 1; //set the flag for formatting display

    putsUART1(Loggingwillstop); //Logging will stop at:
    while (BusyUART1()); //get and display stop time from FRAM

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

    LC2CONTROL2.flags2.SetStopTime = 0; //reset the flag
}

void displayLogInterval(int interval) {
    char BUF[6];
    int value;
    unsigned long length;

    crlf();
    putsUART1(Interval); //"Interval #"
    while (BusyUART1());

    sprintf(BUF, "%d", interval); //format it
    putsUART1(BUF); //display it
    while (BusyUART1());

    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());

    interval -= 1; //change interval from 1-6 to 0-5

    putsUART1(Length);
    while (BusyUART1());
    length=read_longFRAM(LogIntLength1address+(interval*8));                  //get Interval value  
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
    value=read_Int_FRAM(LogIt1address+(interval*8));                           //get Interval iterations 
    sprintf(BUF, "%d", value); //format it
    putsUART1(BUF); //display it
    while (BusyUART1());

    if (LC2CONTROL.flags.Logging && LC2CONTROL.flags.LogInterval) //if logging and log intervals enabled
    {
        putcUART1(slash); //foreslash
        while (BusyUART1());
        value=read_Int_FRAM(LogItRemain1address+(interval*8));           //get the remaining iterations at this interval   

        if (interval == 0)
            value += 1;

        if (value <= 0)
            value = 0;

        sprintf(BUF, "%d", value);
        putsUART1(BUF);
        while (BusyUART1());
    }

}

void displayLogTable(void) {
    int i; //index for Log Interval loop

    crlf();

    putsUART1(Loglist); //"Log Intervals List"
    while (BusyUART1());

    crlf();

    putsUART1(Dashes); //"----------------------"
    while (BusyUART1());

    crlf();

    for (i = 1; i < 7; i++) {
        displayLogInterval(i); //display the Log Interval information
        crlf();
    }

}

void displayMemoryStatus(void) {
    char BUF[6]; //temporary storage for display
    unsigned int pointer = 0;

    crlf();
    putsUART1(MS); //MS:
    while (BusyUART1()); //Display Memory Status
    pointer=read_Int_FRAM(MemoryStatusaddress);//get Memory Status (MS) pointer  
    sprintf(BUF, "%d", pointer); //format it
    putsUART1(BUF); //display it
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());


    putsUART1(OP); //OP:
    while (BusyUART1()); //Display Output Position
    pointer=read_Int_FRAM(OutputPositionaddress);//get Output Position (OP) pointer  
    sprintf(BUF, "%d", pointer); //format it
    putsUART1(BUF); //display it
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());


    putsUART1(UP); //UP:
    while (BusyUART1()); //Display User Position
    pointer=read_Int_FRAM(UserPositionaddress);//get User Position (UP) pointer from FRAM  
    sprintf(BUF, "%d", pointer);
    putsUART1(BUF); //display it
    while (BusyUART1());
}


void displayMUX(int displayChannel) {

    MUX_ENABLE1_16.MUXen1_16=read_Int_FRAM(MUX_ENABLE1_16flagsaddress);             
    MUX_ENABLE17_32.MUXen17_32=read_Int_FRAM(MUX_ENABLE17_32flagsaddress);          
    //DISPLAY MUX SETUP TABLE:
    crlf();

    if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel    
    {
        putsUART1(MUX1setupmenu); //"LC-2 Single Channel VW Setup:"
        while (BusyUART1());
        crlf();
        displayGageInfo(1); //Display gage information
        crlf();
        crlf();
        return; //just return
    }


    if (MUX4_ENABLE.mflags.mux16_4 == VW4)
        putsUART1(MUX4setupmenu); //"LC-2MUX 4-Channel VW Multiplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == VW8)
        putsUART1(MUX8setupmenu); //"LC-2MUX 8-Channel VW Multiplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == VW16)
        putsUART1(MUX16setupmenu); //"LC-2MUX 16-Channel VW Mulitplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == VW32)
        putsUART1(MUX32setupmenu); //"LC-2MUX 32-Channel VW Multiplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == TH8)
        putsUART1(MUX8Tsetupmenu); //LC-2MUX 8-Channel Thermistor Multiplexer Setup:"
    if (MUX4_ENABLE.mflags.mux16_4 == TH32)
        putsUART1(MUX32Tsetupmenu); //LC-2MUX 32-Channel Thermistor Multiplexer Setup:"
    while (BusyUART1());
    crlf();
    crlf();


    if (displayChannel == 1 | displayChannel == 0) {
        putsUART1(CH1); //CH1 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH1) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(1); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 2 | displayChannel == 0) {
        putsUART1(CH2); //CH2 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH2) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(2); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 3 | displayChannel == 0) {
        putsUART1(CH3); //CH3 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH3) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(3); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 4 | displayChannel == 0) {
        putsUART1(CH4); //CH4 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH4) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(4); //Display gage information
        crlf();
        crlf();
    }


    if (MUX4_ENABLE.mflags.mux16_4 == VW4)
        return;

    if (displayChannel == 5 | displayChannel == 0) {
        putsUART1(CH5); //CH5 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH5) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(5); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 6 | displayChannel == 0) {
        putsUART1(CH6); //CH6 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH6) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(6); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 7 | displayChannel == 0) {
        putsUART1(CH7); //CH7 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH7) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(7); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 8 | displayChannel == 0) {
        putsUART1(CH8); //CH8 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH8) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(8); //Display gage information
        crlf();
        crlf();
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == TH8)
        return;


    if (displayChannel == 9 | displayChannel == 0) {
        putsUART1(CH9); //CH9 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH9) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(9); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 10 | displayChannel == 0) {
        putsUART1(CH10); //CH10 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH10) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(10); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 11 | displayChannel == 0) {
        putsUART1(CH11); //CH11 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH11) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(11); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 12 | displayChannel == 0) {
        putsUART1(CH12); //CH12 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH12) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(12); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 13 | displayChannel == 0) {
        putsUART1(CH13); //CH13 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH13) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(13); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 14 | displayChannel == 0) {
        putsUART1(CH14); //CH14 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH14) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(14); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 15 | displayChannel == 0) {
        putsUART1(CH15); //CH15 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH15) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(15); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 16 | displayChannel == 0) {
        putsUART1(CH16); //CH16 setup
        while (BusyUART1());
        if (MUX_ENABLE1_16.e1flags.CH16) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(16); //Display gage information
        crlf();
        crlf();
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16)
        return;

    if (displayChannel == 17 | displayChannel == 0) {
        putsUART1(CH17); //CH17 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH17) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(17); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 18 | displayChannel == 0) {
        putsUART1(CH18); //CH18 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH18) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(18); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 19 | displayChannel == 0) {
        putsUART1(CH19); //CH19 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH19) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(19); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 20 | displayChannel == 0) {
        putsUART1(CH20); //CH20 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH20) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(20); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 21 | displayChannel == 0) {
        putsUART1(CH21); //CH21 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH21) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(21); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 22 | displayChannel == 0) {
        putsUART1(CH22); //CH22 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH22) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(22); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 23 | displayChannel == 0) {
        putsUART1(CH23); //CH23 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH23) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(23); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 24 | displayChannel == 0) {
        putsUART1(CH24); //CH24 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH24) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(24); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 25 | displayChannel == 0) {
        putsUART1(CH25); //CH25 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH25) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(25); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 26 | displayChannel == 0) {
        putsUART1(CH26); //CH26 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH26) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(26); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 27 | displayChannel == 0) {
        putsUART1(CH27); //CH27 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH27) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(27); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 28 | displayChannel == 0) {
        putsUART1(CH28); //CH28 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH28) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(28); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 29 | displayChannel == 0) {
        putsUART1(CH29); //CH29 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH29) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(29); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 30 | displayChannel == 0) {
        putsUART1(CH30); //CH30 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH30) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(30); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 31 | displayChannel == 0) {
        putsUART1(CH31); //CH31 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH31) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(31); //Display gage information
        crlf();
        crlf();
    }

    if (displayChannel == 32 | displayChannel == 0) {
        putsUART1(CH32); //CH32 setup
        while (BusyUART1());
        if (MUX_ENABLE17_32.e2flags.CH32) //Display ENABLED or DISABLED
            putsUART1(Enabled);
        else
            putsUART1(Disabled);
        while (BusyUART1());
        if (MUX4_ENABLE.mflags.mux16_4 != TH32)
            displayGageInfo(32); //Display gage information
        crlf();
        crlf();
    }


}

void displayReading(int ch, unsigned long outputPosition) //display readings stored in external FRAM at FRAMaddress
{
    char BUF[8]; //temporary storage for display data    
    unsigned char month;
    unsigned char day;
    unsigned int julian;
    unsigned int year;
    unsigned int extThermreading = 0;
    unsigned int intThermreading = 0;
    int mainBatreading = 0;
    int displayArray;
    int tempdisplayArray = 0; //VER 6.0.2
    unsigned int displayChannel = 0;
    unsigned int x = 0; //VER 6.0.2
    unsigned long i;                                                            //REV B
    unsigned int maxchannelplusone;
    unsigned int pointer = 0;
    unsigned long FRAMaddress;
    unsigned long tempoutputPosition = 0; //for DEBUG
    float mainBat = 0.0;
    float intThermRaw = 0.0;
    float intThermProcessed = 0.0;
    float extThermRaw = 0.0;
    float extThermProcessed = 0.0;
    float percent = 0.0;


    if ((outputPosition > maxSingleVW && (MUX4_ENABLE.mflags.mux16_4 == Single)) |
            (outputPosition > maxFourVW && (MUX4_ENABLE.mflags.mux16_4 == VW4)) |
            (outputPosition > maxEightVW && (MUX4_ENABLE.mflags.mux16_4 == VW8)) |
            (outputPosition > maxSixteenVW && (MUX4_ENABLE.mflags.mux16_4 == VW16)) |
            (outputPosition > maxThirtytwoVW && (MUX4_ENABLE.mflags.mux16_4 == VW32)) | //memory full:Return memory pointer to 1 for circular memory
            (outputPosition > maxEightTH && (MUX4_ENABLE.mflags.mux16_4 == TH8)) |
            (outputPosition > maxThirtytwoTH && (MUX4_ENABLE.mflags.mux16_4 == TH32))) {
        outputPosition = 1;
        write_Int_FRAM(OutputPositionaddress,outputPosition);	//store outputPosition pointer in internal FRAM   
    }

    displayArray = outputPosition;

    //calculate the external FRAM address:
    if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel VW
        FRAMaddress = SingleVWPosition;

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //4 Channel VW
        FRAMaddress = VW4Position;

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //8 Channel VW
        FRAMaddress = VW8Position;

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //16 Channel VW
        FRAMaddress = VW16Position;

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //32 Channel VW
        FRAMaddress = VW32Position;

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //8 Channel thermistor
        FRAMaddress = TH8Position;

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //32 Channel thermistor
        FRAMaddress = TH32Position;

    if (!_232) //VER 6.0.1
        _232SHDN=1;                                                             

        if ((LC2CONTROL.flags.ID && !LC2CONTROL2.flags2.d) | (LC2CONTROL2.flags2.d && LC2CONTROL2.flags2.ID)) //Display ID if flag is set or if Binary download (1X)
        {
            //IDaddress=0xF030;                                                 //reset ID FRAM pointer REM REV B

            for (i = IDaddress; i < FRAM_MEMORYflagsaddress; i += 2)            //parse the buffer and extract the ID character
            {
                data=read_Int_FRAM(i);                                          //read the ID starting FRAM location    
                unpack(data); //unpack into (2) bytes

                if (Hbyte == cr && !LC2CONTROL2.flags2.d) //exit if MSB = <CR>    VER 6.0.2
                    break;
                putcUART1(Hbyte);

                if (Lbyte == cr && !LC2CONTROL2.flags2.d) //exit if LSB = <CR>    VER 6.0.2
                    break;
                putcUART1(Lbyte);
            }

            if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                putcUART1(comma); // , DELIMITER
            else
                LC2CONTROL2.flags2.ID = 0; //clear the ID flag

        }

    //TIME STAMP
    //YEAR
    IEC1bits.INT1IE = 0; //Disable INT2
    year = read_Int_FRAM(FRAMaddress); //Read the year from the external FRAM
    IEC1bits.INT1IE = 1; //Enable INT2


    if (!LC2CONTROL2.flags2.d) //VER 6.0.2
    {
        putcUART1(two); //year
        putcUART1(zero);

        if (year < 10)
            putcUART1(zero); //add leading zero if required

        sprintf(BUF, "%d", year); //format it
        putsUART1(BUF); //display it
        while (BusyUART1());
        putcUART1(comma); // , DELIMITER
    } else {
        putcUART1(year); //Binary
    }

    //DAY OF YEAR
    IEC1bits.INT1IE = 0; //Disable INT2
    julian = read_Int_FRAM(FRAMaddress + 2); //Read the decimal date from external FRAM
    IEC1bits.INT1IE = 1; //Enable INT2

    if (LC2CONTROL.flags.DateFormat) //month,day
    {
        month = toMonthDay(julian, year, 1); //get the month from the julian date
        day = toMonthDay(julian, year, 0); //get the day from the julian date

        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
        {
            sprintf(BUF, "%d", month); //format the month data ASCII
            putsUART1(BUF); //display it
            while (BusyUART1());
            putcUART1(comma); // , DELIMITER
        } else {
            putcUART1(month); //Binary
        }

        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
        {
            sprintf(BUF, "%d", day); //format the day data   ASCII
            putsUART1(BUF); //display it
            while (BusyUART1());
            putcUART1(comma); // , DELIMITER
        } else {
            putcUART1(day); //Binary
        }

    } else //decimal day
    {
        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
        {
            sprintf(BUF, "%d", julian); //ASCII
            putsUART1(BUF);
            while (BusyUART1());
            putcUART1(comma); // , DELIMITER
        } else {
            putcUART1(MSB); //Binary    
            putcUART1(LSB);
        }
    }


    IEC1bits.INT1IE = 0; //Disable INT2
    read_Flt_FRAM(FRAMaddress + 4, 1);
    IEC1bits.INT1IE = 1; //Enable INT2
    seconds2hms(TESTSECONDS);

    //HOUR
    if (hour < 10) //add leading zero
        putcUART1(zero);

    if (!LC2CONTROL2.flags2.d) //VER 6.0.2
    {
        sprintf(BUF, "%d", hour); //format the hour data  ASCII
        putsUART1(BUF); //display it
        while (BusyUART1());
        if (LC2CONTROL.flags.TimeFormat) //hh,mm format
            putcUART1(comma); // , DELIMITER
    } else {
        putcUART1(hour); //Binary
    }

    //MINUTE
    if (minute < 10) //add leading zero
        putcUART1(zero);

    if (!LC2CONTROL2.flags2.d) //VER 6.0.2
    {
        sprintf(BUF, "%d", minute); //format the minute data    ASCII
        putsUART1(BUF); //display it
        while (BusyUART1());
        putcUART1(comma); // , DELIMITER
    } else {
        putcUART1(minute); //Binary
    }

    //SECOND
    if (!LC2CONTROL2.flags2.d) //VER 6.0.2
    {
        sprintf(BUF, "%d", second); //format the second data    ASCII
        putsUART1(BUF); //display it
        while (BusyUART1());
        putcUART1(comma); // , DELIMITER
    } else {
        putcUART1(second); //Binary
    }



    //****************Get and format the readings from FRAM****************************************************
    //MAIN BATTERY
    IEC1bits.INT1IE = 0; //Disable INT2
    mainBatreading = read_Int_FRAM(FRAMaddress + 8); //Read the 12bit main Bat reading from external FRAM
    IEC1bits.INT1IE = 1; //Enable INT2

    if (mainBatreading >= 0) {
        mainBat = (((Vref * mainBatreading) / 4096) * mul3V); //format the 3V battery data    VER 6.0.2
        if (mainBat < 0.3)
            mainBat = 0.0;
    } else {
        mainBat = (((Vref * mainBatreading) / 4096) * mul12V); //format the 12V battery data   VER 6.0.2
        mainBat *= -1.0; //convert to positive
        if (mainBat < 0.3)
            mainBat = 0.0;
    }

    //*************************************Display the readings**************************************************************

    //MAIN BATTERY
    if (!LC2CONTROL2.flags2.d) //VER 6.0.2
    {
        sprintf(BUF, "%.2f", mainBat);
        putsUART1(BUF); //display it
        while (BusyUART1());
        putcUART1(comma); // , DELIMITER
    } else {
        putcUART1(MSB); //MSB Binary
        putcUART1(LSB); //LSB Binary
    }


    //****************Get and format the readings from FRAM****************************************************
    //INTERNAL THERMISTOR
    IEC1bits.INT1IE = 0; //Disable INT2
    intThermreading = read_Int_FRAM(FRAMaddress + 10); //Read the 12bit internal thermistor reading from external FRAM
    IEC1bits.INT1IE = 1; //Enable INT2

    if (!LC2CONTROL2.flags2.d) //VER 6.0.2
    {
        intThermRaw = ((Vref * intThermreading) / 4096); //convert to voltage    VER 6.0.0
        intThermProcessed = V_T2C(intThermRaw); //convert to degrees C

        //*************************************Display the readings**************************************************************
        //INTERNAL THERMISTOR
        if (intThermProcessed <= -10.0 | intThermProcessed >= 10.0) {
            if (intThermProcessed<-40.0) //no therm connected
                intThermProcessed = -99.0;
            sprintf(BUF, "%.1f", intThermProcessed); //format the internal thermistor reading 1 decimal place
        }

        if (intThermProcessed>-10.0 && intThermProcessed < 10.0) {
            sprintf(BUF, "%.2f", intThermProcessed); //format the internal thermistor reading 2 decimal places
        }
        putsUART1(BUF); //display it
        while (BusyUART1());

        putcUART1(comma); // , DELIMITER
        while (BusyUART1());
    } else {
        putcUART1(MSB); //MSB Binary
        putcUART1(LSB); //LSB Binary
    }

    tempoutputPosition = outputPosition;

    //GAGE READING
    if (MUX4_ENABLE.mflags.mux16_4 == Single) {
        maxchannelplusone = 2; //single channel VW
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //VER 6.0.7
    {
        maxchannelplusone = 5; //4 channel VW mux
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) {
        maxchannelplusone = 9; //8 channel VW mux
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) {
        maxchannelplusone = 17; //16 channel VW mux
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) {
        maxchannelplusone = 33; //32 channel VW mux
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) {
        maxchannelplusone = 9; //8 channel mux
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) {
        maxchannelplusone = 33; //32 channel mux
    }

    for (displayChannel = 1; displayChannel < maxchannelplusone; displayChannel++) {
        IEC1bits.INT1IE = 0; //Disable INT2

        if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel VW
            FRAMaddress = SingleVWBytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == VW4) //4 channel VW mux
            FRAMaddress = VW4Bytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == VW8) //8 channel VW mux
            FRAMaddress = VW8Bytes * (tempoutputPosition - 1)+(4 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == VW16) //16 channel mux    
            FRAMaddress = VW16Bytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == VW32) //32 channel VW mux
            FRAMaddress = VW32Bytes * (tempoutputPosition - 1)+(4 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == TH8) //8 channel TH mux
            FRAMaddress = TH8Bytes * (tempoutputPosition - 1)+(2 * (displayChannel - 1)) + 12;

        if (MUX4_ENABLE.mflags.mux16_4 == TH32) //32 channel TH mux
            FRAMaddress = TH32Bytes * (tempoutputPosition - 1)+(2 * (displayChannel - 1)) + 12; //VER 6.0.12



        //GAGE READING

        if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32) //8 channel or 32 channel Thermistor mux VER 6.0.9
        {
            extThermreading = read_Int_FRAM(FRAMaddress);
            extThermRaw = ((Vref * extThermreading) / 4096); //convert to voltage
            extThermProcessed = V_T2C(extThermRaw); //convert to degrees C

            if (extThermProcessed <= -10.0 | extThermProcessed >= 10.0) {
                if (extThermProcessed<-50.0) //no therm connected
                    extThermProcessed = -99.0;
                sprintf(BUF, "%.1f", extThermProcessed); //format the external thermistor reading 1 decimal place
            }

            if (extThermProcessed>-10.0 && extThermProcessed < 10.0)
                sprintf(BUF, "%.2f", extThermProcessed); //format the external thermistor reading 2 decimal places
        } else //VW
        {
            read_Flt_FRAM(FRAMaddress, 0); //Read the transducer reading from external FRAM
            if (!LC2CONTROL2.flags2.d) //VER 6.0.12    Format for display
            {
                IEC1bits.INT1IE = 1; //Enable INT2
                sprintf(BUF, "%.3f", gageReading); //format the transducer reading

                if (gageReading == -999999.0)
                    sprintf(BUF, "%.1f", gageReading); //format the error message (-999999.0)
                gageReading = 0; //reset the gageReading
            }

        }


        switch (displayChannel) //VW or 8 or 32 channel Thermistor Display
        {

            case 1:
                if (!MUX_ENABLE1_16.e1flags.CH1) //Channel 1 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 2:
                if (!MUX_ENABLE1_16.e1flags.CH2) //Channel 2 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 3:
                if (!MUX_ENABLE1_16.e1flags.CH3) //Channel 3 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 4:
                if (!MUX_ENABLE1_16.e1flags.CH4) //Channel 4 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 5:
                if (!MUX_ENABLE1_16.e1flags.CH5) //Channel 5 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 6:
                if (!MUX_ENABLE1_16.e1flags.CH6) //Channel 6 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 7:
                if (!MUX_ENABLE1_16.e1flags.CH7) //Channel 7 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 8:
                if (!MUX_ENABLE1_16.e1flags.CH8) //Channel 8 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 9:
                if (!MUX_ENABLE1_16.e1flags.CH9) //Channel 9 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 10:
                if (!MUX_ENABLE1_16.e1flags.CH10) //Channel 10 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 11:
                if (!MUX_ENABLE1_16.e1flags.CH11) //Channel 11 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

            case 12:
                if (!MUX_ENABLE1_16.e1flags.CH12) //Channel 12 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 13:
                if (!MUX_ENABLE1_16.e1flags.CH13) //Channel 13 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 14:
                if (!MUX_ENABLE1_16.e1flags.CH14) //Channel 14 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 15:
                if (!MUX_ENABLE1_16.e1flags.CH15) //Channel 15 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 16:
                if (!MUX_ENABLE1_16.e1flags.CH16) //Channel 16 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 17:
                if (!MUX_ENABLE17_32.e2flags.CH17) //Channel 17 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 18:
                if (!MUX_ENABLE17_32.e2flags.CH18) //Channel 18 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 19:
                if (!MUX_ENABLE17_32.e2flags.CH19) //Channel 19 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 20:
                if (!MUX_ENABLE17_32.e2flags.CH20) //Channel 20 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 21:
                if (!MUX_ENABLE17_32.e2flags.CH21) //Channel 21 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 22:
                if (!MUX_ENABLE17_32.e2flags.CH22) //Channel 22 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 23:
                if (!MUX_ENABLE17_32.e2flags.CH23) //Channel 23 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 24:
                if (!MUX_ENABLE17_32.e2flags.CH24) //Channel 24 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 25:
                if (!MUX_ENABLE17_32.e2flags.CH25) //Channel 25 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 26:
                if (!MUX_ENABLE17_32.e2flags.CH26) //Channel 26 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 27:
                if (!MUX_ENABLE17_32.e2flags.CH27) //Channel 27 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 28:
                if (!MUX_ENABLE17_32.e2flags.CH28) //Channel 28 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 29:
                if (!MUX_ENABLE17_32.e2flags.CH29) //Channel 29 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 30:
                if (!MUX_ENABLE17_32.e2flags.CH30) //Channel 30 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 31:
                if (!MUX_ENABLE17_32.e2flags.CH31) //Channel 31 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;


            case 32:
                if (!MUX_ENABLE17_32.e2flags.CH32) //Channel 32 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                    while (BusyUART1());
                    putcUART1(comma); // , DELIMITER
                    break;
                } else {
                    if (!LC2CONTROL2.flags2.d) //VER 6.0.12
                    {
                        putsUART1(BUF); //Display VW reading
                        while (BusyUART1());
                        putcUART1(comma); // , DELIMITER
                        break;
                    } else {
                        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) {
                            putcUART1(MSB); //VW MSB Binary
                            putcUART1(MMSB); //VW MMSB Binary
                            putcUART1(MMMSB); //VW MMMSB Binary
                            putcUART1(LSB); //VW LSB Binary
                        } else {
                            putcUART1(MSB); //Thermistor MSB Binary
                            putcUART1(LSB); //Thermistor LSB Binary
                        }
                    }
                }
                break;

        }
        while (BusyUART1());


    } //end of 1st for(displayChannel) loop


    if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32 &&
            MUX4_ENABLE.mflags.mux16_4 != VW8 && MUX4_ENABLE.mflags.mux16_4 != VW32) //Thermistor display for VW VER 6.0.13
    {
        for (displayChannel = 1; displayChannel < maxchannelplusone; displayChannel++) {
            if (!LC2CONTROL2.flags2.d && displayChannel != 1) {
                putcUART1(comma); // , DELIMITER
                while (BusyUART1());
            }


            IEC1bits.INT1IE = 0; //Disable INT2

            if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel VW
                FRAMaddress = SingleVWBytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

            if (MUX4_ENABLE.mflags.mux16_4 == VW4) //4 channel VW mux
                FRAMaddress = VW4Bytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

            if (MUX4_ENABLE.mflags.mux16_4 == VW8) //8 channel VW mux
                FRAMaddress = VW8Bytes * (tempoutputPosition - 1)+(4 * (displayChannel - 1)) + 12;

            if (MUX4_ENABLE.mflags.mux16_4 == VW16) //16 channel VW mux
                FRAMaddress = VW16Bytes * (tempoutputPosition - 1)+(6 * (displayChannel - 1)) + 12;

            if (MUX4_ENABLE.mflags.mux16_4 == VW32) //32 channel VW mux
                FRAMaddress = VW32Bytes * (tempoutputPosition - 1)+(4 * (displayChannel - 1)) + 12;

            //****************Get and format the readings from FRAM****************************************************
            //EXTERNAL THERMISTOR
            IEC1bits.INT1IE = 0; //Disable INT2
            extThermreading = read_Int_FRAM(FRAMaddress + 4); //Read the 12bit external thermistor reading from external FRAM
            IEC1bits.INT1IE = 1; //Enable INT2

            if (!LC2CONTROL2.flags2.d) //VER 6.0.2
            {
                extThermRaw = ((Vref * extThermreading) / 4096); //convert to voltage
                extThermProcessed = V_T2C(extThermRaw); //convert to degrees C

                if (extThermProcessed <= -10.0 | extThermProcessed >= 10.0) {
                    if (extThermProcessed<-50.0) //no therm connected
                        extThermProcessed = -99.0;
                    sprintf(BUF, "%.1f", extThermProcessed); //format the external thermistor reading 1 decimal place
                }

                if (extThermProcessed>-10.0 && extThermProcessed < 10.0)
                    sprintf(BUF, "%.2f", extThermProcessed); //format the external thermistor reading 2 decimal places
            }

            switch (displayChannel) {

                case 1:

                    if (!MUX_ENABLE1_16.e1flags.CH1) //Channel 1 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }
                    break;

                case 2:

                    if (!MUX_ENABLE1_16.e1flags.CH2) //Channel 2 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }
                    break;

                case 3:

                    if (!MUX_ENABLE1_16.e1flags.CH3) //Channel 3 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }
                    break;

                case 4:

                    if (!MUX_ENABLE1_16.e1flags.CH4) //Channel 4 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }
                    break;

                case 5:

                    if (!MUX_ENABLE1_16.e1flags.CH5) //Channel 5 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }
                    break;

                case 6:

                    if (!MUX_ENABLE1_16.e1flags.CH6) //Channel 6 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }
                    break;

                case 7:

                    if (!MUX_ENABLE1_16.e1flags.CH7) //Channel 7 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 8:

                    if (!MUX_ENABLE1_16.e1flags.CH8) //Channel 8 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 9:

                    if (!MUX_ENABLE1_16.e1flags.CH9) //Channel 9 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 10:

                    if (!MUX_ENABLE1_16.e1flags.CH10) //Channel 10 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 11:

                    if (!MUX_ENABLE1_16.e1flags.CH11) //Channel 11 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 12:

                    if (!MUX_ENABLE1_16.e1flags.CH12) //Channel 12 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 13:

                    if (!MUX_ENABLE1_16.e1flags.CH13) //Channel 13 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 14:

                    if (!MUX_ENABLE1_16.e1flags.CH14) //Channel 14 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 15:

                    if (!MUX_ENABLE1_16.e1flags.CH15) //Channel 15 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 16:

                    if (!MUX_ENABLE1_16.e1flags.CH16) //Channel 16 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        if (!LC2CONTROL2.flags2.d) //VER 6.0.2
                        {
                            putsUART1(BUF); //display reading
                        } else {
                            putcUART1(MSB); //MSB Binary
                            putcUART1(LSB); //LSB Binary
                        }
                    }

                    break;

                case 17:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 18:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 19:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 20:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 21:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 22:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 23:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 24:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 25:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 26:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 27:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 28:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 29:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 30:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 31:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;

                case 32:
                    if (!LC2CONTROL2.flags2.d) {
                        putsUART1(BUF); //display reading
                    } else {
                        putcUART1(MSB); //MSB Binary
                        putcUART1(LSB); //LSB Binary
                    }

                    break;


                default:

                    putsUART1(Error); //"ERROR" (somethings wrong)
                    break;



            }

            while (BusyUART1());

        } //end of 2nd for(displayChannel)

    } // end of if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32)   VER 6.0.9

    if (!LC2CONTROL2.flags2.d) //VER 6.0.2
    {
        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32 &&
                MUX4_ENABLE.mflags.mux16_4 != VW8 && MUX4_ENABLE.mflags.mux16_4 != VW32)
            putcUART1(comma); // , DELIMITER
        sprintf(BUF, "%d", displayArray); //Display the Array #
        putsUART1(BUF);
        while (BusyUART1());
        crlf();
    } else {
        putcUART1(nul); //transmit nul char as seperator
    }

}

void displayScanInterval(unsigned long ScanInterval, unsigned int text) {
    long interval = 0; //signed ScanInterval
    unsigned char z = 0; //loop index

    BCDones = 0; //initialize
    BCDtens = 0;
    BCDhundreds = 0;
    BCDthousands = 0;
    BCDtenthousands = 0;


    for (interval = ScanInterval - 10000; interval >= 0; interval -= 10000) {
        BCDtenthousands += 1; //increment ten thousands
    }

    interval += 10000;

    for (interval -= 1000; interval >= 0; interval -= 1000) {
        BCDthousands += 1; //increment thousands
    }

    interval += 1000;

    for (interval -= 100; interval >= 0; interval -= 100) {
        BCDhundreds += 1; //increment hundreds
    }

    interval += 100;

    for (interval -= 10; interval >= 0; interval -= 10) {
        BCDtens += 1; //increment tens
    }

    interval += 10;

    for (interval -= 1; interval >= 0; interval -= 1) {
        BCDones += 1; //increment ones
    }


    //remove leading zeros, convert to ascii and transmit out UART

    if (BCDtenthousands != 0) {
        putcUART1(BCDtenthousands + 0x30);
        while (BusyUART1());
    }


    if (BCDtenthousands != 0 | BCDthousands != 0) {
        putcUART1(BCDthousands + 0x30);
        while (BusyUART1());
    }


    if (BCDtenthousands != 0 | BCDthousands != 0 | BCDhundreds != 0) {
        putcUART1(BCDhundreds + 0x30);
        while (BusyUART1());
    }


    if (BCDtenthousands != 0 | BCDthousands != 0 | BCDhundreds != 0 | BCDtens != 0) {
        putcUART1(BCDtens + 0x30);
        while (BusyUART1());
    }


    putcUART1(BCDones + 0x30);
    while (BusyUART1());

    if (!text) {
        //determine index for formatting loop
        if (BCDtenthousands == 0 && BCDthousands == 0 && BCDhundreds == 0 && BCDtens == 0 && BCDones != 0)
            z = 4;

        if (BCDtenthousands == 0 && BCDthousands == 0 && BCDhundreds == 0 && BCDtens != 0)
            z = 3;

        if (BCDtenthousands == 0 && BCDthousands == 0 && BCDhundreds != 0)
            z = 2;

        if (BCDtenthousands == 0 && BCDthousands != 0)
            z = 1;

        for (z; z > 0; z--) //format the scan interval
        {
            putcUART1(' ');
            while (BusyUART1());
        }
    }

    if (text) //display "second(s)."
    {
        putsUART1(Seconds);
        while (BusyUART1());
    }
}

void displayTempReading(void) {
    char BUF[8]; //temporary storage for display data
    unsigned char month;
    unsigned char day;
    unsigned int julian;
    unsigned int year;
    unsigned int extThermreading = 0;
    unsigned int intThermreading = 0;
    unsigned int tempTherm = 0; //VER 6.0.6
    int mainBatreading = 0;
    unsigned long i;                                                            //REV B
    unsigned int pointer = 0;
    unsigned int displayChannel = 0;
    unsigned int maxchannelplusone;
    unsigned int TempFRAMaddress = 0xF500; //internal FRAM address for beginning of data
    float mainBat = 0.0;
    float intThermRaw = 0.0;
    float intThermProcessed = 0.0;
    float extThermRaw = 0.0;
    float extThermProcessed = 0.0;
    float percent = 0.0;
    float tempGage = 0.0;
    long TESTSECONDS = 0;

    restoreSettings(); //load the flags from internal FRAM
    //_232SHDN=1;                                                                //VER 6.0.0

    if (LC2CONTROL.flags.ID) //Display ID if flag is set
    {
        //IDaddress=0xF030;                                                       //reset ID FRAM pointer

        for (i = IDaddress; i < FRAM_MEMORYflagsaddress; i += 2) //parse the buffer and extract the ID character
        {
            data=read_Int_FRAM(i);                                             //read the ID starting FRAM location    
            unpack(data); //unpack into (2) bytes
            if (Hbyte == cr) //exit if MSB = <CR>
                break;
            putcUART1(Hbyte);
            while (BusyUART1());
            if (Lbyte == cr) //exit if LSB = <CR>
                break;
            putcUART1(Lbyte);
            while (BusyUART1());
        }
        putcUART1(comma); // , DELIMITER
        while (BusyUART1());
    }

    //TIME STAMP
    //YEAR
    IEC1bits.INT1IE = 0; //Disable INT2
    year=read_Int_FRAM(TempFRAMaddress);                                     //Read the year from the internal FRAM  
    IEC1bits.INT1IE = 1; //Enable INT2
    putcUART1(two); //year
    while (BusyUART1());
    putcUART1(zero);
    while (BusyUART1());
    if (year < 10) { //add leading zero if required
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(BUF, "%d", year); //format it
    putsUART1(BUF); //display it
    while (BusyUART1());

    putcUART1(comma); // , DELIMITER
    while (BusyUART1());

    //DAY OF YEAR
    IEC1bits.INT1IE = 0; //Disable INT2
    julian=read_Int_FRAM(TempFRAMaddress+2);                                 //Read the decimal date from internal FRAM  
    IEC1bits.INT1IE = 1; //Enable INT2
    if (LC2CONTROL.flags.DateFormat) //month,day
    {
        month = toMonthDay(julian, year, 1); //get the month from the julian date
        day = toMonthDay(julian, year, 0); //get the day from the julian date

        sprintf(BUF, "%d", month); //format the month data
        putsUART1(BUF); //display it
        while (BusyUART1());

        putcUART1(comma); // , DELIMITER
        while (BusyUART1());

        sprintf(BUF, "%d", day); //format the day data
        putsUART1(BUF); //display it
        while (BusyUART1());
    } else //decimal day
    {
        sprintf(BUF, "%d", julian);
        putsUART1(BUF);
        while (BusyUART1());
    }

    putcUART1(comma); // , DELIMITER
    while (BusyUART1());

    IEC1bits.INT1IE = 0; //Disable INT2
    TESTSECONDS=read_longFRAM(TempFRAMaddress+4); 
    IEC1bits.INT1IE = 1; //Enable INT2
    seconds2hms(TESTSECONDS);

    //HOUR
    if (hour < 10) //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }

    sprintf(BUF, "%d", hour); //format the hour data
    putsUART1(BUF); //display it
    while (BusyUART1());

    if (LC2CONTROL.flags.TimeFormat) //hh,mm format
    {
        putcUART1(comma); // , DELIMITER
        while (BusyUART1());
    }

    //MINUTE
    if (minute < 10) //add leading zero
    {
        putcUART1(zero);
        while (BusyUART1());
    }
    sprintf(BUF, "%d", minute); //format the minute data
    putsUART1(BUF); //display it
    while (BusyUART1());

    putcUART1(comma); // , DELIMITER
    while (BusyUART1());

    //SECOND
    sprintf(BUF, "%d", second); //format the second data
    putsUART1(BUF); //display it
    while (BusyUART1());

    putcUART1(comma); // , DELIMITER
    while (BusyUART1());

    //****************Get and format the readings from FRAM****************************************************
    //MAIN BATTERY
    IEC1bits.INT1IE = 0; //Disable INT2
    mainBatreading=read_Int_FRAM(TempFRAMaddress+8);                         //Read the 12bit main Bat reading from internal FRAM    
    IEC1bits.INT1IE = 1; //Enable INT2

    if (mainBatreading >= 0) {
        mainBat = (((Vref * mainBatreading) / 4096) * mul3V); //format the 3V battery data VER 6.0.2
        if (mainBat < 0.3)
            mainBat = 0.0;
    } else {
        mainBat = (((Vref * mainBatreading) / 4096) * mul12V); //format the 12V battery data 	VER 6.0.2
        mainBat *= -1.0; //convert to positive
        if (mainBat < 0.3)
            mainBat = 0.0;
    }

    //INTERNAL THERMISTOR
    IEC1bits.INT1IE = 0; //Disable INT2
    intThermreading=read_Int_FRAM(TempFRAMaddress+10);                       //Read the 12bit internal thermistor reading from internal FRAM 
    IEC1bits.INT1IE = 1; //Enable INT2
    intThermRaw = ((Vref * intThermreading) / 4096); //convert to voltage    VER 6.0.0
    intThermProcessed = V_T2C(intThermRaw); //convert to degrees C




    //*************************************Display the readings**************************************************************

    //MAIN BATTERY
    sprintf(BUF, "%.2f", mainBat);
    putsUART1(BUF); //display it
    while (BusyUART1());

    putcUART1(comma); // , DELIMITER
    while (BusyUART1());


    //INTERNAL THERMISTOR
    if (intThermProcessed <= -10.0 | intThermProcessed >= 10.0) {
        if (intThermProcessed<-40.0) //no therm connected
            intThermProcessed = -99.0;
        sprintf(BUF, "%.1f", intThermProcessed); //format the internal thermistor reading 1 decimal place
    }

    if (intThermProcessed>-10.0 && intThermProcessed < 10.0)
        sprintf(BUF, "%.2f", intThermProcessed); //format the internal thermistor reading 2 decimal places

    sprintf(BUF, "%.2f", intThermProcessed); //format the internal thermistor data
    putsUART1(BUF); //display it
    while (BusyUART1());

    //GAGE READING & THERMISTOR READINGS

    if (MUX4_ENABLE.mflags.mux16_4 == Single) {
        maxchannelplusone = 2; //Single Channel
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //determine # of channels in MUX    VER 6.0.7
    {
        maxchannelplusone = 5; //4 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //determine # of channels in MUX    VER 6.0.7
    {
        maxchannelplusone = 9; //8 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //VER 6.0.7
    {
        maxchannelplusone = 17; //16 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //VER 6.0.7
    {
        maxchannelplusone = 33; //32 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //VER 6.0.7
    {
        maxchannelplusone = 9; //8 CHANNEL
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //VER 6.0.9
    {
        maxchannelplusone = 33; //8 CHANNEL
    }

    for (displayChannel = 1; displayChannel < maxchannelplusone; displayChannel++) {
        putcUART1(comma); // , DELIMITER
        while (BusyUART1());

        //GAGE READING
        IEC1bits.INT1IE = 0; //Disable INT2
        if (MUX4_ENABLE.mflags.mux16_4 == Single | //VER 6.0.13
                MUX4_ENABLE.mflags.mux16_4 == VW4 |
                MUX4_ENABLE.mflags.mux16_4 == VW16)
            TempFRAMaddress = 0xF50E + (6 * (displayChannel - 1)); //compute the reading address

        if (MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //VER 6.0.13
            TempFRAMaddress = 0xF50C + (4 * (displayChannel - 1));
        //8 Channel Therm   REM VER 6.0.9
        if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32) //8 or 32 Channel Therm   VER 6.0.9
        {
            TempFRAMaddress = 0xF50C + (2 * (displayChannel - 1)); //compute the reading address   VER 6.0.7
            tempTherm=read_Int_FRAM(TempFRAMaddress);                      
            extThermRaw = ((Vref * tempTherm) / 4096); //convert to voltage
            extThermProcessed = V_T2C(extThermRaw); //convert to degrees C

            if (extThermProcessed <= -10.0 | extThermProcessed >= 10.0) {
                if (extThermProcessed<-50.0) //no therm connected
                    extThermProcessed = -99.0;
                sprintf(BUF, "%.1f", extThermProcessed); //format the external thermistor reading 1 decimal place
            }

            if (extThermProcessed>-10.0 && extThermProcessed < 10.0)
                sprintf(BUF, "%.2f", extThermProcessed); //format the external thermistor reading 2 decimal places

        } else {
            tempGage=read_longFRAM(TempFRAMaddress);                           //Read the transducer reading from internal FRAM    
            sprintf(BUF, "%.3f", tempGage); //format the transducer reading
            if (tempGage == -999999.0)
                sprintf(BUF, "%.1f", tempGage); //format the error message (-999999.0)
            tempGage = 0; //reset the gage reading
        }
        IEC1bits.INT1IE = 1; //Enable INT2



        switch (displayChannel) {

            case 1:

                if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel    VER 6.0.7
                {
                    putsUART1(BUF); //display reading
                    break;
                }


                if (!MUX_ENABLE1_16.e1flags.CH1) //Channel 1 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 2:

                if (!MUX_ENABLE1_16.e1flags.CH2) //Channel 2 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 3:

                if (!MUX_ENABLE1_16.e1flags.CH3) //Channel 3 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 4:

                if (!MUX_ENABLE1_16.e1flags.CH4) //Channel 4 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 5:

                if (!MUX_ENABLE1_16.e1flags.CH5) //Channel 5 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 6:

                if (!MUX_ENABLE1_16.e1flags.CH6) //Channel 6 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 7:

                if (!MUX_ENABLE1_16.e1flags.CH7) //Channel 7 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 8:

                if (!MUX_ENABLE1_16.e1flags.CH8) //Channel 8 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 9:

                if (!MUX_ENABLE1_16.e1flags.CH9) //Channel 9 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 10:

                if (!MUX_ENABLE1_16.e1flags.CH10) //Channel 10 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 11:

                if (!MUX_ENABLE1_16.e1flags.CH11) //Channel 11 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 12:

                if (!MUX_ENABLE1_16.e1flags.CH12) //Channel 12 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 13:

                if (!MUX_ENABLE1_16.e1flags.CH13) //Channel 13 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 14:

                if (!MUX_ENABLE1_16.e1flags.CH14) //Channel 14 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 15:

                if (!MUX_ENABLE1_16.e1flags.CH15) //Channel 15 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 16:

                if (!MUX_ENABLE1_16.e1flags.CH16) //Channel 16 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 17:

                if (!MUX_ENABLE17_32.e2flags.CH17) //Channel 17 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 18:

                if (!MUX_ENABLE17_32.e2flags.CH18) //Channel 18 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 19:

                if (!MUX_ENABLE17_32.e2flags.CH19) //Channel 19 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 20:

                if (!MUX_ENABLE17_32.e2flags.CH20) //Channel 20 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 21:

                if (!MUX_ENABLE17_32.e2flags.CH21) //Channel 21 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 22:

                if (!MUX_ENABLE17_32.e2flags.CH22) //Channel 22 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 23:

                if (!MUX_ENABLE17_32.e2flags.CH23) //Channel 23 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 24:

                if (!MUX_ENABLE17_32.e2flags.CH24) //Channel 24 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 25:

                if (!MUX_ENABLE17_32.e2flags.CH25) //Channel 25 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 26:

                if (!MUX_ENABLE17_32.e2flags.CH26) //Channel 26 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 27:

                if (!MUX_ENABLE17_32.e2flags.CH27) //Channel 27 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 28:

                if (!MUX_ENABLE17_32.e2flags.CH28) //Channel 28 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 29:

                if (!MUX_ENABLE17_32.e2flags.CH29) //Channel 29 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 30:

                if (!MUX_ENABLE17_32.e2flags.CH30) //Channel 30 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 31:

                if (!MUX_ENABLE17_32.e2flags.CH31) //Channel 31 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            case 32:

                if (!MUX_ENABLE17_32.e2flags.CH32) //Channel 32 disabled?
                {
                    putsUART1(DisabledDisplay); //Yes - display "---"
                } else {
                    putsUART1(BUF); //No - display reading
                }
                break;

            default:

                putsUART1(Error); //"ERROR" (somethings wrong)
                break;
        }
        while (BusyUART1());
    } //end of 1st for(displayChannel) loop

    if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32 &&
            MUX4_ENABLE.mflags.mux16_4 != VW8 && MUX4_ENABLE.mflags.mux16_4 != VW32) //VER 6.0.13
    {
        for (displayChannel = 1; displayChannel < maxchannelplusone; displayChannel++) {
            putcUART1(comma); // , DELIMITER
            while (BusyUART1());

            //EXTERNAL THERMISTOR
            IEC1bits.INT1IE = 0; //Disable INT2
            TempFRAMaddress = 0xF50C + (6 * (displayChannel - 1));
            extThermreading=read_Int_FRAM(TempFRAMaddress);                  //Read the 12bit external thermistor reading from internal FRAM 
            IEC1bits.INT1IE = 1; //Enable INT2

            extThermRaw = ((Vref * extThermreading) / 4096); //convert to voltage    VER 6.0.0
            extThermProcessed = V_T2C(extThermRaw); //convert to degrees C

            if (extThermProcessed <= -10.0 | extThermProcessed >= 10.0) {
                if (extThermProcessed<-50.0) //no therm connected
                    extThermProcessed = -99.0;
                sprintf(BUF, "%.1f", extThermProcessed); //format the external thermistor reading 1 decimal place
            }

            if (extThermProcessed>-10.0 && extThermProcessed < 10.0)
                sprintf(BUF, "%.2f", extThermProcessed); //format the external thermistor reading 2 decimal places


            switch (displayChannel) {

                case 1:

                    if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel    VER 6.0.7
                    {
                        putsUART1(BUF); //display reading
                        break;
                    }


                    if (!MUX_ENABLE1_16.e1flags.CH1) //Channel 1 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 2:

                    if (!MUX_ENABLE1_16.e1flags.CH2) //Channel 2 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 3:

                    if (!MUX_ENABLE1_16.e1flags.CH3) //Channel 3 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 4:

                    if (!MUX_ENABLE1_16.e1flags.CH4) //Channel 4 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 5:

                    if (!MUX_ENABLE1_16.e1flags.CH5) //Channel 5 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 6:

                    if (!MUX_ENABLE1_16.e1flags.CH6) //Channel 6 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 7:

                    if (!MUX_ENABLE1_16.e1flags.CH7) //Channel 7 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 8:

                    if (!MUX_ENABLE1_16.e1flags.CH8) //Channel 8 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 9:

                    if (!MUX_ENABLE1_16.e1flags.CH9) //Channel 9 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 10:

                    if (!MUX_ENABLE1_16.e1flags.CH10) //Channel 10 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 11:

                    if (!MUX_ENABLE1_16.e1flags.CH11) //Channel 11 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 12:

                    if (!MUX_ENABLE1_16.e1flags.CH12) //Channel 12 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 13:

                    if (!MUX_ENABLE1_16.e1flags.CH13) //Channel 13 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 14:

                    if (!MUX_ENABLE1_16.e1flags.CH14) //Channel 14 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 15:

                    if (!MUX_ENABLE1_16.e1flags.CH15) //Channel 15 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 16:

                    if (!MUX_ENABLE1_16.e1flags.CH16) //Channel 16 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 17:

                    if (!MUX_ENABLE17_32.e2flags.CH17) //Channel 17 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 18:

                    if (!MUX_ENABLE17_32.e2flags.CH18) //Channel 18 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 19:

                    if (!MUX_ENABLE17_32.e2flags.CH19) //Channel 19 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 20:

                    if (!MUX_ENABLE17_32.e2flags.CH20) //Channel 20 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 21:

                    if (!MUX_ENABLE17_32.e2flags.CH21) //Channel 21 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 22:

                    if (!MUX_ENABLE17_32.e2flags.CH22) //Channel 22 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 23:

                    if (!MUX_ENABLE17_32.e2flags.CH23) //Channel 23 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 24:

                    if (!MUX_ENABLE17_32.e2flags.CH24) //Channel 24 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 25:

                    if (!MUX_ENABLE17_32.e2flags.CH25) //Channel 25 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 26:

                    if (!MUX_ENABLE17_32.e2flags.CH26) //Channel 26 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 27:

                    if (!MUX_ENABLE17_32.e2flags.CH27) //Channel 27 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 28:

                    if (!MUX_ENABLE17_32.e2flags.CH28) //Channel 28 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 29:

                    if (!MUX_ENABLE17_32.e2flags.CH29) //Channel 29 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 30:

                    if (!MUX_ENABLE17_32.e2flags.CH30) //Channel 30 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 31:

                    if (!MUX_ENABLE17_32.e2flags.CH31) //Channel 31 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;

                case 32:

                    if (!MUX_ENABLE17_32.e2flags.CH32) //Channel 32 disabled?
                    {
                        putsUART1(DisabledDisplay); //Yes - display "---"
                    } else {
                        putsUART1(BUF); //No - display reading
                    }
                    break;




                default:

                    putsUART1(Error); //"ERROR" (somethings wrong)
                    break;

            }

            while (BusyUART1());

        }
    }
    crlf();

}

void displayTemporaryStatus(int tempUserPosition) {
    char BUF[6]; //temporary storage for display
    unsigned int pointer = 0;
    crlf();

    putsUART1(MS); //MS:
    while (BusyUART1()); //Display Memory Status
    pointer=read_Int_FRAM(MemoryStatusaddress);//get Memory Status (MS) pointer  
    sprintf(BUF, "%d", pointer); //format it
    putsUART1(BUF); //display it
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());

    putsUART1(OP); //OP:
    while (BusyUART1()); //Display Output Position
    pointer=read_Int_FRAM(OutputPositionaddress);//get Output Position (OP) pointer  
    sprintf(BUF, "%d", pointer); //format it
    putsUART1(BUF); //display it
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());
    putcUART1(space);
    while (BusyUART1());

    putsUART1(UP); //UP:
    while (BusyUART1()); //Display Temporary User Position
    pointer = tempUserPosition;

    //if(pointer==16001)							//restore pointer to 1  REM VER 6.0.3
    if (pointer == (maxSingleVW + 1)) //restore pointer to 1  VER 6.0.3
        pointer = 1;

    sprintf(BUF, "%d", pointer);
    putsUART1(BUF); //display it
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

    putsUART1(TM1); //LC-2x4 TEST MENU
    while (BusyUART1());
    crlf();
    crlf();

    putsUART1(TM2); //SELECTION         TEST
    while (BusyUART1());
    crlf();
    crlf();

    putsUART1(TM9); //    1         ALL FRAM
    while (BusyUART1());
    crlf();

    putsUART1(TM10); //    2         +3V_X
    while (BusyUART1());
    crlf();

    putsUART1(TM11); //    3         RTC 32KHz
    while (BusyUART1());
    crlf();

    putsUART1(TM13); //    <ESC>     EXIT TEST MENU
    while (BusyUART1());
    crlf();

    crlf();
    crlf();

    putsUART1(TM12); //ENTER SELECTION:
    while (BusyUART1());
}

void enableChannel(int channel) {
    MUX_ENABLE1_16.MUXen1_16=read_Int_FRAM(MUX_ENABLE1_16flagsaddress);            
    MUX_ENABLE17_32.MUXen17_32=read_Int_FRAM(MUX_ENABLE17_32flagsaddress);          

    switch (channel) {
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

    write_Int_FRAM(MUX_ENABLE1_16flagsaddress,MUX_ENABLE1_16.MUXen1_16);                  //store flag in FRAM 
    write_Int_FRAM(MUX_ENABLE17_32flagsaddress,MUX_ENABLE17_32.MUXen17_32);                  //store flag in FRAM  
}

//REM REV B:
//void enableINT2(void) {
//    INTCON1bits.NSTDIS = 1; //disable nested interrupts
//    //IPC5bits.INT2IP=5;						//set interrupt priority to 5
//    IFS1bits.INT2IF = 0; //clear the interrupt flag
//    INTCON2bits.INT2EP = 1; //falling edge interrupt
//    IEC1bits.INT2IE = 1; //enable the interrupt
//}

//REV B:
void enableINT1(void)
{
    //INTCON1bits.NSTDIS = 1;                                                     //disable nested interrupts
    //IFS1bits.INT1IF = 0;                                                        //clear the interrupt flag
    //INTCON2bits.INT1EP = 1;                                                     //falling edge interrupt
    //IEC1bits.INT1IE = 1;                                                        //enable the interrupt
    ConfigINT1(FALLING_EDGE_INT & EXT_INT_PRI_7 & EXT_INT_ENABLE & GLOBAL_INT_ENABLE);
    Nop();
}


void formatandDisplayGageInfo(float TEMPVAL) {
    char BUF[8]; //temporary storage for gage info

    if ((TEMPVAL >= 0.0 && TEMPVAL < 1.0) | (TEMPVAL >= -1.0 && TEMPVAL < 0.0))
        sprintf(BUF, "%.5f", TEMPVAL); //format it

    if ((TEMPVAL >= 1.0 && TEMPVAL < 10.0) | (TEMPVAL >= -10.0 && TEMPVAL<-1.0))
        sprintf(BUF, "%.5f", TEMPVAL); //format it

    if ((TEMPVAL >= 10.0 && TEMPVAL < 100.0) | (TEMPVAL >= -100.0 && TEMPVAL<-10.0))
        sprintf(BUF, "%.5f", TEMPVAL); //format it

    if ((TEMPVAL >= 100.0 && TEMPVAL < 1000.0) | (TEMPVAL >= -1000.0 && TEMPVAL<-100.0))
        sprintf(BUF, "%.4f", TEMPVAL); //format it

    if ((TEMPVAL >= 1000.0 && TEMPVAL < 10000.0) | (TEMPVAL >= -10000.0 && TEMPVAL<-1000.0))
        sprintf(BUF, "%.3f", TEMPVAL); //format it

    if ((TEMPVAL >= 10000.0 && TEMPVAL < 100000.0) | (TEMPVAL >= -100000.0 && TEMPVAL<-10000.0))
        sprintf(BUF, "%.2f", TEMPVAL); //format it

    if ((TEMPVAL >= 100000.0 && TEMPVAL < 1000000.0) | (TEMPVAL>-1000000.0 && TEMPVAL<-100000.0))
        sprintf(BUF, "%.1f", TEMPVAL); //format it

    if (TEMPVAL >= 1000000.0 | TEMPVAL<-1000000.0)
        sprintf(BUF, "%.0f", TEMPVAL); //format it


    putsUART1(BUF); //display it
    while (BusyUART1());
}

/*  REM VER 6.0.10
void Fout(unsigned char gageType)
{
    float frequency=0.0;
    float halfperiod=0.0;
    unsigned int timer5load=0;

    switch(gageType)				//determine frequency to output
    {
        case 30:
            frequency=400.0;
            break;
    case 31:
            frequency=1000.0;
            break;
    case 32:
            frequency=2000.0;
            break;
        case 33:
            frequency=3000.0;
            break;
    case 34:
            frequency=4000.0;
            break;
    case 35:
            frequency=5000.0;
            break;
    default:
            break;
    }

    halfperiod=((1.0/frequency)/tcy)/2.0;	//compute # of tcy for 1/2 cycle
    timer5load=65536-halfperiod;			//compute Timer5 load value

}
 */

int getChannel(void) {
    int channelOnes = 0;
    int channelTens = 0;

    if (isdigit(buffer[1])) //examine the first channel char
        channelOnes = buffer[1] - 0x30;

    if (buffer[2] == '/')
        return channelOnes; //return the single digit channel #

    channelTens = channelOnes; //shift first digit into 10's place

    if (isdigit(buffer[2]))
        channelOnes = buffer[2] - 0x30; //get the 1's digit

    channelTens *= 10; //multiply 10's by 10
    channelTens = channelTens + channelOnes; //add the ones
    return channelTens; //return the 2 digit channel #
}

int getConversion(void) {
    if ((buffer[2] == slash && buffer[3] == capL && buffer[4] == slash)
            | (buffer[3] == slash && buffer[4] == capL && buffer[5] == slash)) //linear conversion?
        return 0;

    if ((buffer[2] == slash && buffer[3] == capP && buffer[4] == slash)
            | (buffer[3] == slash && buffer[4] == capP && buffer[5] == slash)) //polynomial conversion?
        return 1;

    return 2; //illegal entry
}

void getGageInfo(unsigned char info, int channel) {
    unsigned char i = 0; //buffer index
    unsigned char dp = 0; //decimal point location
    unsigned char x; //location in buffer of beginning of ZR slash
    unsigned char y; //location in buffer of beginning of GF slash
    unsigned char z; //location in buffer of beginning of GO slash
    unsigned char negative = 0; //switch for negative number
    float whole = 0.0; //whole number portion of floating point value
    float fractional = 0.0; //fractional portion of floating point value
    float mul = 1.0; //decimal weight multiplier
    float div = 0.1; //decimal weight multiplier


    for (i; buffer[i] != slash; i++) //parse buffer for location of first '/'
    {
        if (buffer[i] == cr)
            return;
    }

    ++i;

    for (i; buffer[i] != slash; i++) //parse buffer for location of second '/'
    {
        if (buffer[i] == cr)
            return;
    }

    ++i;

    for (i; buffer[i] != slash; i++) //parse buffer for location of third '/'
    {
        if (buffer[i] == cr)
            return;
    }

    for (i; buffer[i] != slash && buffer[i] != cr; i++)//parse buffer for location of 4th '/'
    {
        if (buffer[i] == cr)
            return;
    }

    for (i; buffer[i] != slash && buffer[i] != cr; i++)//parse buffer for location of 5th '/'
    {
        if (buffer[i] == cr)
            return;
    }

    x = i; //x=index of this location
    i++; //increment to next buffer location


    for (i; buffer[i] != slash && buffer[i] != cr; i++) //parse buffer for location of 6th '/'
    {
        if (buffer[i - 1] == cr)
            return; //break out of loop if 6th '/' is not found
    }

    y = i; //y=index of this location
    i++; //increment to next buffer location

    if (info != 1) //don't go here if zero reading
    {
        for (i; buffer[i] != slash && buffer[i] != cr; i++) //parse buffer for location of 7th '/'
        {
            if (buffer[i - 1] == cr)
                return; //break out of loop if 7th '/' is not found
        }

        z = i; //z=index of this location
    }


    switch (info) //determine information to extract
    {
        case 1: //zero reading or polynomial coefficient A
            break; //x&y already determined

        case 2: //gage factor or polynomial coefficient B
            x = y;
            y = z;
            break;

        case 3: //gage offset or polynomial coefficient C
            x = z;
            y = cr; //<CR> = end of buffer
            break;

        default:
            break;
    }

    if (info == 3) //<CR> indicates end of buffer
    {
        for (i = x + 1; buffer[i] != cr; i++) //locate decimal point
        {
            if (buffer[i] == decimal)
                dp = i; //store index of decimal point
        }
    } else {
        for (i = x + 1; i < y; i++) //locate decimal point
        {
            if (buffer[i] == decimal)
                dp = i; //store index of decimal point
        }
    }

    if (dp) //if a decimal point was found
    {
        while (buffer[dp - 1] != slash) {
            if (buffer[dp - 1] == minus) //determine if value is negative
            {
                negative = 1; //turn on negative switch
                break;
            }

            whole += (buffer[dp - 1] - 0x30) * mul; //extract the whole number
            dp -= 1; //decrement the pointer
            mul *= 10; //increment the power of ten
        }

        for (dp; buffer[dp] != decimal; dp++) {
        } //go back to the decimal point

        while (buffer[dp + 1] != slash && buffer[dp + 1] != cr) //get the fractional part
        {
            fractional += (buffer[dp + 1] - 0x30) * div; //extract the fractional part
            dp += 1; //increment the pointer
            div /= 10; //decrement the power of ten
        }

        whole += fractional; //return the final floating point number	
        if (negative)
            whole *= -1.0;
    } else //no decimal point in value
    {
        while (buffer[i - 1] != slash) {
            if (buffer[i - 1] == minus) //determine if value is negative
            {
                whole *= -1.00; //negate
                break;
            }

            whole += (buffer[i - 1] - 0x30) * mul; //extract the whole number
            i -= 1; //decrement the pointer
            mul *= 10; //increment the power of ten
        }

    }

    Nop(); //breakpoint for debugging

    switch (info) //determine information to extract
    {
        case 1: //zero reading or polynomial coefficient A
            if (y != x + 1) //make sure user entered a value here
            {
                zeroReading = whole; //zero reading was entered
                polyCoA = whole; //polynomial coefficient A was entered
            }
            break;

        case 2: //gage factor or polynomial coefficient B
            if (y != x + 1) //make sure user entered a value here
            {
                gageFactor = whole; //gage factor was entered
                polyCoB = whole; //polynomial coefficient B was entered
            }
            break;

        case 3: //gage offset or polynomial coefficient C 
            if (buffer[z] == slash && buffer[z + 1] != cr) {
                gageOffset = whole; //gage offset was entered
                polyCoC = whole; //polynomial coefficient C was entered
            }
            break;

        default:
            break;
    }

    //Write to internal FRAM

    //VER 6.0.12:
    switch (channel) {
        case 1: //CHANNEL 1

            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH1) {
                        write_longFRAM(whole,CH1ZRaddress);                     //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH1PolyCoAaddress);                //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH1) {
                        write_longFRAM(whole,CH1GFaddress);                     //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH1PolyCoBaddress);                //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH1) {
                        write_longFRAM(whole,CH1GOaddress);                  //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH1PolyCoCaddress);             //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;


        case 2: //CHANNEL 2
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH2) {
                        write_longFRAM(whole,CH2ZRaddress);                  //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH2PolyCoAaddress);             //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH2) {
                        write_longFRAM(whole,CH2GFaddress);                  //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH2PolyCoBaddress);             //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH2) {
                        write_longFRAM(whole,CH2GOaddress);                  //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH2PolyCoCaddress);             //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;


        case 3: //CHANNEL 3
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH3) {
                        write_longFRAM(whole,CH3ZRaddress);                  //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH3PolyCoAaddress);             //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH3) {
                        write_longFRAM(whole,CH3GFaddress);                  //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH3PolyCoBaddress);             //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH3) {
                        write_longFRAM(whole,CH3GOaddress);                  //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH3PolyCoCaddress);             //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 4: //CHANNEL 4
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH4) {
                        write_longFRAM(whole,CH4ZRaddress);                  //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH4PolyCoAaddress);             //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH4) {
                        write_longFRAM(whole,CH4GFaddress);                  //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH4PolyCoBaddress);             //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH4) {
                        write_longFRAM(whole,CH4GOaddress);                  //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH4PolyCoCaddress);             //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 5: //CHANNEL 5
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH5) {
                        write_longFRAM(whole,CH5ZRaddress);                  //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH5PolyCoAaddress);             //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH5) {
                        write_longFRAM(whole,CH5GFaddress);                  //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH5PolyCoBaddress);             //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH5) {
                        write_longFRAM(whole,CH5GOaddress);                  //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH5PolyCoCaddress);             //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 6: //CHANNEL 6
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH6) {
                        write_longFRAM(whole,CH6ZRaddress);                  //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH6PolyCoAaddress);             //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH6) {
                        write_longFRAM(whole,CH6GFaddress);                  //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH6PolyCoBaddress);             //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH6) {
                        write_longFRAM(whole,CH6GOaddress);                  //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH6PolyCoCaddress);             //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 7: //CHANNEL 7
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH7) {
                        write_longFRAM(whole,CH7ZRaddress);                  //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH7PolyCoAaddress);             //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH7) {
                        write_longFRAM(whole,CH7GFaddress);                  //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH7PolyCoBaddress);             //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH7) {
                        write_longFRAM(whole,CH7GOaddress);                  //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH7PolyCoCaddress);             //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;


        case 8: //CHANNEL 8
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH8) {
                        write_longFRAM(whole,CH8ZRaddress);                  //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH8PolyCoAaddress);             //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH8) {
                        write_longFRAM(whole,CH8GFaddress);                  //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH8PolyCoBaddress);             //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH8) {
                        write_longFRAM(whole,CH8GOaddress);                  //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH8PolyCoCaddress);             //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 9: //CHANNEL 9
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH9) {
                        write_longFRAM(whole,CH9ZRaddress);                  //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH9PolyCoAaddress);             //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH9) {
                        write_longFRAM(whole,CH9GFaddress);                  //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH9PolyCoBaddress);             //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH9) {
                        write_longFRAM(whole,CH9GOaddress);                  //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH9PolyCoCaddress);             //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;


        case 10: //CHANNEL 10
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH10) {
                        write_longFRAM(whole,CH10ZRaddress);                  //store the zero reading 
                    } else {
                        write_longFRAM(whole,CH10PolyCoAaddress);             //store the Polynomial coefficient A 
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH10) {
                        write_longFRAM(whole,CH10GFaddress);                  //store the gage factor  
                    } else {
                        write_longFRAM(whole,CH10PolyCoBaddress);             //store the Polynomial coefficient B 
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH10) {
                        write_longFRAM(whole,CH10GOaddress);                  //store the gage offset  
                    } else {
                        write_longFRAM(whole,CH10PolyCoCaddress);             //store the Polynomial coefficient C 
                    }
                    break;
            }

            return;


        case 11: //CHANNEL 11
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH11) {
                        write_longFRAM(whole,CH11ZRaddress);                  //store the zero reading 
                    } else {
                        write_longFRAM(whole,CH11PolyCoAaddress);             //store the Polynomial coefficient A 
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH11) {
                        write_longFRAM(whole,CH11GFaddress);                  //store the gage factor  
                    } else {
                        write_longFRAM(whole,CH11PolyCoBaddress);             //store the Polynomial coefficient B 
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH11) {
                        write_longFRAM(whole,CH11GOaddress);                  //store the gage offset  
                    } else {
                        write_longFRAM(whole,CH11PolyCoCaddress);             //store the Polynomial coefficient C 
                    }
                    break;
            }

            return;

        case 12: //CHANNEL 12
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH12) {
                        write_longFRAM(whole,CH12ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH12PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH12) {
                        write_longFRAM(whole,CH12GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH12PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH12) {
                        write_longFRAM(whole,CH12GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH12PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 13: //CHANNEL 13
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH13) {
                        write_longFRAM(whole,CH13ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH13PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH13) {
                        write_longFRAM(whole,CH13GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH13PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH13) {
                        write_longFRAM(whole,CH13GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH13PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 14: //CHANNEL 14
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH14) {
                        write_longFRAM(whole,CH14ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH14PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH14) {
                        write_longFRAM(whole,CH14GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH14PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH14) {
                        write_longFRAM(whole,CH14GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH14PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 15: //CHANNEL 15
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH15) {
                        write_longFRAM(whole,CH15ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH15PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH15) {
                        write_longFRAM(whole,CH15GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH15PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH15) {
                        write_longFRAM(whole,CH15GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH15PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;


        case 16: //CHANNEL 16
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION1_16.c1flags.CH16) {
                        write_longFRAM(whole,CH16ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH16PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION1_16.c1flags.CH16) {
                        write_longFRAM(whole,CH16GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH16PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION1_16.c1flags.CH16) {
                        write_longFRAM(whole,CH16GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH16PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 17: //CHANNEL 17
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH17) {
                        write_longFRAM(whole,CH17ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH17PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH17) {
                        write_longFRAM(whole,CH17GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH17PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH17) {
                        write_longFRAM(whole,CH17GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH17PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 18: //CHANNEL 18
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH18) {
                        write_longFRAM(whole,CH18ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH18PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH18) {
                        write_longFRAM(whole,CH18GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH18PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH18) {
                        write_longFRAM(whole,CH18GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH18PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 19: //CHANNEL 19
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH19) {
                        write_longFRAM(whole,CH19ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH19PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH19) {
                        write_longFRAM(whole,CH19GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH19PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH19) {
                        write_longFRAM(whole,CH19GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH19PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 20: //CHANNEL 20
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH20) {
                        write_longFRAM(whole,CH20ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH20PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH20) {
                        write_longFRAM(whole,CH20GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH20PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH20) {
                        write_longFRAM(whole,CH20GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH20PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 21: //CHANNEL 21
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH21) {
                        write_longFRAM(whole,CH21ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH21PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH21) {
                        write_longFRAM(whole,CH21GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH21PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH21) {
                        write_longFRAM(whole,CH21GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH21PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 22: //CHANNEL 22
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH22) {
                        write_longFRAM(whole,CH22ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH22PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH22) {
                        write_longFRAM(whole,CH22GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH22PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH22) {
                        write_longFRAM(whole,CH22GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH22PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 23: //CHANNEL 23
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH23) {
                        write_longFRAM(whole,CH23ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH23PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH23) {
                        write_longFRAM(whole,CH23GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH23PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH23) {
                        write_longFRAM(whole,CH23GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH23PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 24: //CHANNEL 24
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH24) {
                        write_longFRAM(whole,CH24ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH24PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH24) {
                        write_longFRAM(whole,CH24GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH24PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH24) {
                        write_longFRAM(whole,CH24GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH24PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 25: //CHANNEL 25
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH25) {
                        write_longFRAM(whole,CH25ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH25PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH25) {
                        write_longFRAM(whole,CH25GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH25PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH25) {
                        write_longFRAM(whole,CH25GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH25PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 26: //CHANNEL 26
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH26) {
                        write_longFRAM(whole,CH26ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH26PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH26) {
                        write_longFRAM(whole,CH26GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH26PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH26) {
                        write_longFRAM(whole,CH26GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH26PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 27: //CHANNEL 27
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH27) {
                        write_longFRAM(whole,CH27ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH27PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH27) {
                        write_longFRAM(whole,CH27GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH27PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH27) {
                        write_longFRAM(whole,CH27GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH27PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 28: //CHANNEL 28
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH28) {
                        write_longFRAM(whole,CH28ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH28PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH28) {
                        write_longFRAM(whole,CH28GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH28PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH28) {
                        write_longFRAM(whole,CH28GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH28PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 29: //CHANNEL 29
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH29) {
                        write_longFRAM(whole,CH29ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH29PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH29) {
                        write_longFRAM(whole,CH29GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH29PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH29) {
                        write_longFRAM(whole,CH29GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH29PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

        case 30: //CHANNEL 30
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH30) {
                        write_longFRAM(whole,CH30ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH30PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH30) {
                        write_longFRAM(whole,CH30GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH30PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH30) {
                        write_longFRAM(whole,CH30GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH30PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;


        case 31: //CHANNEL 31
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH31) {
                        write_longFRAM(whole,CH31ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH31PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH31) {
                        write_longFRAM(whole,CH31GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH31PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH31) {
                        write_longFRAM(whole,CH31GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH31PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;


        case 32: //CHANNEL 32
            switch (info) {
                case 1: //ZERO READING

                    if (!MUX_CONVERSION17_32.c2flags.CH32) {
                        write_longFRAM(whole,CH32ZRaddress);                 //store the zero reading  
                    } else {
                        write_longFRAM(whole,CH32PolyCoAaddress);            //store the Polynomial coefficient A  
                    }
                    break;

                case 2: //GAGE FACTOR

                    if (!MUX_CONVERSION17_32.c2flags.CH32) {
                        write_longFRAM(whole,CH32GFaddress);                 //store the gage factor   
                    } else {
                        write_longFRAM(whole,CH32PolyCoBaddress);            //store the Polynomial coefficient B  
                    }
                    break;

                case 3: //GAGE OFFSET

                    if (!MUX_CONVERSION17_32.c2flags.CH32) {
                        write_longFRAM(whole,CH32GOaddress);                 //store the gage offset   
                    } else {
                        write_longFRAM(whole,CH32PolyCoCaddress);            //store the Polynomial coefficient C  
                    }
                    break;
            }

            return;

    }
}

int getGageType(int channel) {
    int gageOnes = 0;

    if (channel < 10) //channels 1-9
    {
        if (buffer[5] == '/') //forces skipping the gage type
            return -1;

        if (!isdigit(buffer[5])) //illegal entry
            return -2;

        if (isdigit(buffer[5])) //examine the first gage type char
            gageOnes = buffer[5]; //store to gageOnes if digit

        if (buffer[6] == '/') {
            gageOnes -= 0x30; //convert to int
            if (gageOnes >= 0 && gageOnes <= 5) {
                return gageOnes; //return the single digit gage type from 0 to 5
            } else {
                return -2; //illegal entry
            }
        } else {
            return -2; //illegal entry
        }
    }

    //if(channel>=10 && channel<=16)		//channels 10-16                REM VER 6.0.13
    if (channel >= 10 && channel <= 32) //channels 10-32                VER 6.0.13
    {
        if (buffer[6] == '/') //forces skipping the gage type
            return -1;

        if (!isdigit(buffer[6])) //illegal entry
            return -2;

        if (isdigit(buffer[6])) //examine the first gage type char
            gageOnes = buffer[6]; //store to gageOnes if digit

        if (buffer[7] == '/') {
            gageOnes -= 0x30; //convert to int
            if (gageOnes >= 0 && gageOnes <= 5) {
                return gageOnes; //return the single digit gage type from 0 to 5
            } else {
                return -2; //illegal entry
            }
        } else {
            return -2; //illegal entry
        }
    }

}

int getLogInterval(void) {
    int LogInt;


    LogInt = buffer[1]; //examine the first Interval char

    if (!isdigit(buffer[1])) //illegal entry
        return -2;

    LogInt -= 0x30; //convert ascii to decimal

    if (LogInt < 1 | LogInt > 6) //illegal Log Interval
        return -2;


    if (buffer[2] == cr) //display Log Interval value and iterations
    {
        displayLogInterval(LogInt);
        return -2; //go back to * prompt after displaying Log Interval value and iterations
    }


    if (buffer[2] == '/')
        return LogInt; //return the single digit Interval
}

void getLogIntLength(void) {
    int IntOnes = 0;
    int IntTens = 0;
    int IntHundreds = 0;
    int IntThousands = 0;
    int IntTenThousands = 0;
    int i = 0;
    int Length = 0;
    int slashcounter = 0;
    int startindex = 0;
    int stopindex = 0;


    if (buffer[2] == '/' && buffer[3] == '/') //forces skipping to Log Iterations
    {
        LogIntLength = -1;
        return;
    }

    for (i = 0; i < 13; i++) //locate the first and last digit of the interval length
    {
        if (buffer[i] == slash && slashcounter == 0) {
            startindex = i + 1; //location in buffer of first interval length digit
            slashcounter = 1;
            continue; //go to next buffer index
        }

        if (buffer[i] == slash && slashcounter == 1) {
            stopindex = i - 1; //location in buffer of last interval length digit
            break; //break out of for loop
        }
    }

    Length = stopindex - startindex;

    if (Length > 4) //>#####
    {
        LogIntLength = -1; //error
        return;
    }

    switch (Length) //compute the integer value of the interval length
    {
        case 0:
            if (!isdigit(buffer[startindex])) //not a digit
            {
                LogIntLength = -1; //error
                return;
            }

            LogIntLength = buffer[startindex] - 0x30; //ones

            if (LogIntLength < 1) {
                LogIntLength = -2; //error
                return;
            }

            break;

        case 1: //return the value that's between 10 and 99
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1])) //not a digit
            {
                LogIntLength = -1; //error
                return;
            }


            LogIntLength = ((buffer[startindex] - 0x30)*10.0)+
                    (buffer[startindex + 1] - 0x30); //tens + ones
            break;

        case 2: //return the value that's between 100 and 999
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]) |
                    !isdigit(buffer[startindex + 2])) //not a digit
            {
                LogIntLength = -1; //error
                return;
            }

            LogIntLength = ((buffer[startindex] - 0x30)*100.0)+
                    ((buffer[startindex + 1] - 0x30)*10.0)+
                    (buffer[startindex + 2] - 0x30); //hundreds + tens + ones
            break;

        case 3: //return the value that's between 1000 and 9999
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]) |
                    !isdigit(buffer[startindex + 2]) |
                    !isdigit(buffer[startindex + 3])) //not a digit
            {
                LogIntLength = -1; //error
                return;
            }

            LogIntLength = ((buffer[startindex] - 0x30)*1000.0)+
                    ((buffer[startindex + 1] - 0x30)*100.0)+
                    ((buffer[startindex + 2] - 0x30)*10.0)+
                    (buffer[startindex + 3] - 0x30); //thousands + hundreds + tens + ones
            break;

        case 4: //return the value that's between 10000 and 99999
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]) |
                    !isdigit(buffer[startindex + 2]) |
                    !isdigit(buffer[startindex + 3]) |
                    !isdigit(buffer[startindex + 4])) //not a digit
            {
                LogIntLength = -1; //error
                return;
            }

            LogIntLength = ((buffer[startindex] - 0x30)*10000.0)+
                    ((buffer[startindex + 1] - 0x30)*1000.0)+
                    ((buffer[startindex + 2] - 0x30)*100.0)+
                    ((buffer[startindex + 3] - 0x30)*10.0)+
                    (buffer[startindex + 4] - 0x30); //ten-thousands + thousands + hundreds + tens + ones

            if (LogIntLength > 86400) //error
            {
                LogIntLength = -1;
                return;
            }

            break;

        default:
            LogIntLength = -1; //error
    }

}

int getLogIterations(void) {
    int IntOnes = 0;
    int IntTens = 0;
    int IntHundreds = 0;
    int i = 0;
    int iterations = 0;
    int Length = 0;
    int slashcounter = 0;
    int startindex = 0;
    int stopindex = 0;


    for (i = 0; i < 13; i++) //locate the first and last digit of the iterations
    {
        if (buffer[i] == slash && slashcounter == 0) {
            slashcounter = 1; //first '/'
            continue;
        }

        if (buffer[i] == slash && slashcounter == 1) {
            startindex = i + 1; //location in buffer of first iterations digit
            slashcounter = 2;
            continue; //go to next buffer index
        }

        if (buffer[i] == cr && slashcounter == 2) {
            stopindex = i - 1; //location in buffer of last iterations digit
            break; //break out of for loop
        }
    }

    Length = stopindex - startindex;

    if (Length > 2) //>999
        return -1; //error


    switch (Length) //compute the integer value of the iterations
    {
        case 0:
            if (!isdigit(buffer[startindex])) //not a digit
            {
                iterations = -1; //error
                break;
            }

            iterations = buffer[startindex] - 0x30; //ones
            break;

        case 1: //return the value that's between 10 and 99
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1])) //not a digit
            {
                iterations = -1; //error
                break;
            }


            iterations = ((buffer[startindex] - 0x30)*10.0)+
                    (buffer[startindex + 1] - 0x30); //tens + ones
            break;

        case 2: //return the value that's between 100 and 999
            if (!isdigit(buffer[startindex]) |
                    !isdigit(buffer[startindex + 1]) |
                    !isdigit(buffer[startindex + 2])) //not a digit
            {
                iterations = -1; //error
                break;
            }

            iterations = ((buffer[startindex] - 0x30)*100.0)+
                    ((buffer[startindex + 1] - 0x30)*10.0)+
                    (buffer[startindex + 2] - 0x30); //hundreds + tens + ones

            if (iterations >= 256) //error
                return -1;

            break;

        default:
            iterations = -1; //error
    }

    return iterations;

}



//***************************************************************************
//						getPeriod()
//
//	Capture frequency input to IC8
//
//	Parameters received: none
//	Returns: float
//
//	Called from: main()
//
//***************************************************************************

float getPeriod(unsigned char gageType) {
    unsigned int VW_period_low_word = 0;
    float VW_period = 0.0; //computed and returned VW period value
    unsigned long VW_period_buffer_buffer = 0;

    IFS0bits.T1IF = 0; //clear TMR1 & TMR3 interrupt flag
    IFS0bits.T3IF = 0;
    timer23_value = 0;

    OpenTimer1(T1_ON & T1_GATE_OFF & T1_PS_1_8 & T1_SOURCE_INT, 0xFFFF); //setup Timer 1

    ConfigIntTimer23(T3_INT_PRIOR_1 & T3_INT_OFF);
    ConfigIntCapture8(IC_INT_ON & IC_INT_PRIOR_6); //Configure input capture interrupt

    //while(VW_SQ && !IFS0bits.T1IF);		//synch edge for F capture
    //while(!VW_SQ && !IFS0bits.T1IF);

    if (!IFS0bits.T1IF) {
        OpenTimer23(T2_ON & T2_GATE_OFF & T2_PS_1_1 & T2_32BIT_MODE_ON & T2_SOURCE_INT, 0xFFFFFFFF);

        if (gageType != 3)
            OpenCapture8(IC_IDLE_STOP & IC_TIMER2_SRC & IC_INT_1CAPTURE & IC_EVERY_16_RISE_EDGE);
        else
            OpenCapture8(IC_IDLE_STOP & IC_TIMER2_SRC & IC_INT_1CAPTURE & IC_EVERY_4_RISE_EDGE);

        while (!IFS0bits.T1IF && !CaptureFlag); //wait for capture interrupt
        CaptureFlag = 0; //clear the capture flag

        CloseCapture8(); //reset the capture and timer registers
        CloseTimer1(); //and disable their interrupts

        CloseTimer23();
        WriteTimer23(0); //clear Timer23
    }

    VW_period = timer23_value - fudgeFactor;
    return VW_period;

}

void handleCOMError(void) {
    U1STAbits.OERR = 0; //clear flag if overrun error
    U1MODEbits.UARTEN = 0; //disable the UART to clear its registers
    U1MODEbits.UARTEN = 1; //re-enable the UART
    trap=read_Int_FRAM(TrapRegisteraddress);     
    trap += 1; //increment the trap counter
    write_Int_FRAM(TrapRegisteraddress,trap);  

    //if (!LC2CONTROL.flags.NetEnabled)
      //  asm("RESET"); //reset processor if error
}

void handleFRAMResults(unsigned int testData) {
    char ERRBUF[7]; //temporary storage for memory error location

    if (testData == 0) {
        crlf();
        putsUART1(TP); //"TEST PASSED"
        while (BusyUART1());
    } else {
        crlf();
        putsUART1(TFAIL); //"TEST FAILED: "
        while (BusyUART1());
        putsUART1(MEMLOC); //"MEMORY LOCATION "
        while (BusyUART1());
        sprintf(ERRBUF, "%X", testData); //format memory error address in HEX
        putsUART1(ERRBUF); //display error address
        while (BusyUART1());
        putcUART1(0x68); //"h"
        while (BusyUART1());
    }
}

void hms(unsigned long interval, unsigned char destination) {
    char BUFFER[11];
    unsigned int ScanHours = 0; //initialize
    unsigned int ScanMinutes = 0;
    unsigned int ScanSeconds = 0;

    if (LC2CONTROL.flags.ScanError) //is error flag set?
    {
        LC2CONTROL.flags.ScanError = 0; //clear the flag
        return; //and exit
    }

    while (1) {
        if (interval < 60) {
            ScanSeconds = interval; //extract seconds and return
            break;
        }

        if (interval > 3599) {
            ScanHours += 1; //increment hours
            interval -= 3600;
        } else {
            ScanMinutes += 1; //increment minutes
            interval -= 60;
        }
    }

    if (destination == 0) {
        write_Int_FRAM(ScanHoursaddress,ScanHours);                             //store HMS in FRAM`
        write_Int_FRAM(ScanMinutesaddress,ScanMinutes);        
        write_Int_FRAM(ScanSecondsaddress,ScanSeconds);        
    }

    if (destination == 1) //store HMS in RTC Alarm1
    {
        BUFFER[0] = 'S';
        BUFFER[1] = 'T';

        toBCD(ScanHours);

        BUFFER[2] = BCDtens + 0x30; //convert hours to ascii & store in BUFFER
        BUFFER[3] = BCDones + 0x30;

        BUFFER[4] = colon; // : delimiter

        toBCD(ScanMinutes);

        BUFFER[5] = BCDtens + 0x30; //convert minutes to ascii & store in BUFFER
        BUFFER[6] = BCDones + 0x30;

        BUFFER[7] = colon; // : delimiter

        toBCD(ScanSeconds);

        BUFFER[8] = BCDtens + 0x30; //convert seconds to ascii & store in BUFFER
        BUFFER[9] = BCDones + 0x30;

        BUFFER[10] = cr; //end of string

        LC2CONTROL.flags.LoggingStartTime = 1; //set flag 
        LC2CONTROL2.flags2.SetStartTime = 1;
        Buf2DateTime(BUFFER); //Load the RTC Alarm1 register
        LC2CONTROL.flags.LoggingStartTime = 0; //reset flag 
        LC2CONTROL2.flags2.SetStartTime = 0;
    }
}

unsigned long hms2s(void) {
    unsigned long total = 0;

    total=(read_Int_FRAM(ScanHoursaddress)*3600.0)+(read_Int_FRAM(ScanMinutesaddress)*60.0)+(read_Int_FRAM(ScanSecondsaddress));   

    return total;
}

void loadDefaults(void) 
{
    unsigned long i; //loop index
    float defaultValue;



    //	Clear the SetStopTime flag:
    LC2CONTROL2.flags2.SetStopTime = 0; //clear the set stop time flag
    LC2CONTROL2.flags2.R = 1; //set the R flag
    LC2CONTROL2.flags2.d = 0; //ASCII data download
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);                  //store flag in FRAM  

    // Linear Conversions:
    MUX_CONVERSION1_16.MUXconv1_16 = 0; //All Linear Conversion
    MUX_CONVERSION17_32.MUXconv17_32 = 0; //All Linear Conversion
    write_Int_FRAM(MUX_CONV1_16flagsaddress,MUX_CONVERSION1_16.MUXconv1_16); 
    write_Int_FRAM(MUX_CONV17_32flagsaddress,MUX_CONVERSION17_32.MUXconv17_32);  

    //MUX_ENABLE.MUXen=0xFFFF;                                                    //Enable all channels REM VER 6.0.13
    write_Int_FRAM(MUX_ENABLE1_16flagsaddress,0xFFFF);                         
    write_Int_FRAM(MUX_ENABLE17_32flagsaddress,0xFFFF);                        
    MUX4_ENABLE.mflags.mux16_4 = Single;                                        //Default as Single Channel 
    write_Int_FRAM(MUX4_ENABLEflagsaddress,MUX4_ENABLE.mux);   


    //initialize gage types to 1:
    for (i = CH1GTaddress; i < CH32GTaddress + 1; i += 0x001A) //channel gage type selection loop
    {
        write_Int_FRAM(i,1);					//write 1 to channel gage type  
    }


    //initialize zero reading to 0:
    for (i = CH1ZRaddress; i < CH32ZRaddress + 1; i += 0x001A) //channel zero reading selection loop
    {
        write_longFRAM(0,i);                                                    //write 0 to channel zero reading 
    }


    //initialize gage factor to 1:
    for (i = CH1GFaddress; i < CH32GFaddress + 1; i += 0x001A) //channel gage factor selection loop
    {
        write_longFRAM(1,i);                                                    //write 1 to channel gage factor  
    }

    //initialize gage offset to 0:
    for (i = CH1GOaddress; i < CH32GOaddress + 1; i += 0x001A) //channel gage offset selection loop
    {
        write_longFRAM(0,i);                                                    //write 0 to channel gage offset  
    }

    //initialize polynomial coefficient A to 0:
    for (i = CH1PolyCoAaddress; i < CH32PolyCoAaddress + 1; i += 0x001A) //channel polynomial coefficient A selection loop
    {
        write_longFRAM(0,i);                                                    //write 0 to channel polynomial coefficient A 
    }

    //initialize polynomial coefficient B to 1:
    for (i = CH1PolyCoBaddress; i < CH32PolyCoBaddress + 1; i += 0x001A) //channel polynomial coefficient B selection loop
    {
        write_longFRAM(1,i);                                                    //write 1 to channel polynomial coefficient B 
    }

    //initialize polynomial coefficient C to 0:
    for (i = CH1PolyCoCaddress; i < CH32PolyCoCaddress + 1; i += 0x001A) //channel polynomial coefficient C selection loop
    {
        write_longFRAM(0,i);                                                    //write 0 to channel polynomial coefficient C 
    }

    //initialize scan intervals:
    if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single VW Channel
    {
        LogIntLength = minScanSingleVW;
        hms(minScanSingleVW, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //4 channel VW MUX
    {
        LogIntLength = minScanFourVW;
        hms(minScanFourVW, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //8 channel VW MUX
    {
        LogIntLength = minScanEightVW;
        hms(minScanEightVW, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //16 channel VW MUX
    {
        LogIntLength = minScanSixteenVW;
        hms(minScanSixteenVW, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //32 channel VW MUX
    {
        LogIntLength = minScanThirtytwoVW;
        hms(minScanThirtytwoVW, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //8 channel TH MUX
    {
        LogIntLength = minScanEightTH;
        hms(minScanEightTH, 0);
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //32 channel TH MUX
    {
        LogIntLength = minScanThirtytwoTH;
        hms(minScanThirtytwoTH, 0);
    }

    //initialize log intervals:
    /*
    if(MUX4_ENABLE.mflags.mux16_4==VW16)                                           //16 channel MUX selected?  VER 6.0.7
    {
        //Log interval #1:
    LogIntLength=minScanSixteenVW;							//minScanSixteenVW second scan interval
    storeLogInterval(1,100);						//100 iterations
    FRAMTest=write_intFRAM(LogItRemain1address,100);                    //100 left to go

        //Log interval #2:
    LogIntLength=40;							//40 second scan interval
    storeLogInterval(2,90);							//90 iterations
    FRAMTest=write_intFRAM(LogItRemain2address,90);                     //90 left to go

        //Log interval #3:
    LogIntLength=50;							//50 second scan interval
    storeLogInterval(3,80);							//80 iterations
    FRAMTest=write_intFRAM(LogItRemain3address,80);                     //80 left to go

        //Log interval #4:
    LogIntLength=60;							//60 second scan interval
    storeLogInterval(4,70);							//70 iterations
    FRAMTest=write_intFRAM(LogItRemain4address,70);                     //70 left to go

        //Log interval #5:
    LogIntLength=90;							//90 second scan interval
    storeLogInterval(5,60);							//60 iterations
    FRAMTest=write_intFRAM(LogItRemain5address,60);                     //60 left to go
 
        //Log interval #6:
    LogIntLength=120;							//120 second scan interval
    storeLogInterval(6,0);							//0 iterations
    FRAMTest=write_intFRAM(LogItRemain6address,0);                      //0 left to go
    }

    if(MUX4_ENABLE.mflags.mux16_4==VW4)   				//4 channel MUX selected    VER 6.0.7
    {
        //Log interval #1:
    LogIntLength=minScanFourVW;							//10 second scan interval
    storeLogInterval(1,100);						//100 iterations
    FRAMTest=write_intFRAM(LogItRemain1address,100);                    //100 left to go
 			
        //Log interval #2:
    LogIntLength=20;							//20 second scan interval
    storeLogInterval(2,90);							//90 iterations
    FRAMTest=write_intFRAM(LogItRemain2address,90);                     //90 left to go

        //Log interval #3:
    LogIntLength=30;							//10 second scan interval
    storeLogInterval(3,80);							//80 iterations
    FRAMTest=write_intFRAM(LogItRemain3address,80);                     //80 left to go

        //Log interval #4:
    LogIntLength=40;							//40 second scan interval
    storeLogInterval(4,70);							//70 iterations
    FRAMTest=write_intFRAM(LogItRemain4address,70);                     //70 left to go

        //Log interval #5:
    LogIntLength=50;							//50 second scan interval
    storeLogInterval(5,60);							//60 iterations
    FRAMTest=write_intFRAM(LogItRemain5address,60);                     //60 left to go

        //Log interval #6:
    LogIntLength=60;							//60 second scan interval
    storeLogInterval(6,0);							//0 iterations
    FRAMTest=write_intFRAM(LogItRemain6address,0);                      //0 left to go
    }
     */


    //if(MUX4_ENABLE.mflags.mux16_4==Single)                                           //Single Channel selected    VER 6.0.7
    //{
    //Log interval #1:
    LogIntLength = 3; //3 second scan interval
    storeLogInterval(1, 3); //3 iterations
    write_Int_FRAM(SingleLogItRemain1address,3);                                //3 left to go 

    //Log interval #2:
    LogIntLength = 6; //6 second scan interval
    storeLogInterval(2, 9); //9 iterations
    write_Int_FRAM(SingleLogItRemain2address,9);                                 //9 left to go`

    //Log interval #3:
    LogIntLength = 10; //10 second scan interval
    storeLogInterval(3, 54); //54 iterations
    write_Int_FRAM(SingleLogItRemain3address,54);                               //54 left to go    `

    //Log interval #4:
    LogIntLength = 30; //30 second scan interval
    storeLogInterval(4, 180); //180 iterations
    write_Int_FRAM(SingleLogItRemain4address,180);                              //180 left to go   

    //Log interval #5:
    LogIntLength = 240; //240 second scan interval
    storeLogInterval(5, 225); //225 iterations
    write_Int_FRAM(SingleLogItRemain5address,225);                              //225 left to go   

    //Log interval #6:
    LogIntLength = 3600; //3600 second scan interval
    storeLogInterval(6, 0); //0 iterations
    write_Int_FRAM(SingleLogItRemain6address,0);                                //0 left to go 
    //}


    //initialize wrap format
    DISPLAY_CONTROL.flags.WrapMemory = 1; //set memory to wrap

    //initialize reading synchronization
    DISPLAY_CONTROL.flags.Synch = 1; //readings synchronized to top of hour

    //save the flags
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM 

    //initialize monitor mode
    LC2CONTROL.flags.Monitor = 1; //Turn on the monitor

    //initialize log intervals
    LC2CONTROL.flags.LogInterval = 0; //Turn off log intervals

    //initialize logging
    LC2CONTROL.flags.Logging = 0; //Logging stopped

    //initialize time format
    LC2CONTROL.flags.TimeFormat = 0; //hhmm

    //initialize date format
    LC2CONTROL.flags.DateFormat = 0; //julian

    //save the flags
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);                     //store flags in FRAM    
}

unsigned int pack(unsigned char Hbyte, unsigned char Lbyte) {
    unsigned int packedword;

    packedword = Hbyte; //load Hbyte
    packedword *= 256; //left shift 8
    packedword += Lbyte; //add Lbyte

    return packedword; //return packed 16 bit word
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
    unsigned int maxloop = _cycles / 2; //temporary register for for/next loop


    float Fstart_tcy_halfperiod = ((1.0 / _Fstart) / tcy) / 2.0; //compute # of tcy for start frequency
    float Fstop_tcy_halfperiod = ((1.0 / _Fstop) / tcy) / 2.0; //compute # of tcy for stop frequency
    float step = (Fstart_tcy_halfperiod - Fstop_tcy_halfperiod) / (_cycles / 2); //compute step

    unsigned int timer_value = 65536 - Fstart_tcy_halfperiod;

    //ENABLE=1;															//enable the pluck driver

    T2CONbits.T32 = 0; //make sure 32 bit mode is off
    OpenTimer3(T3_ON & T3_GATE_OFF & T3_PS_1_1 & T3_SOURCE_INT, 0xFFFF);
    T3CONbits.TON = 0;
    T3CONbits.TON = 1;

    for (i = 0; i < maxloop; ++i) //generate swept frequency pluck
    {
        IFS0bits.T3IF = 0; //clear TMR3 interrupt flag
        VWflagsbits.pluckflag = 1; //set computation flag
        WriteTimer3(timer_value); //setup TMR3
        //EXCITE=1;														//set pluck high
        while (!IFS0bits.T3IF) //wait till TMR3 overflows
        {
            if (VWflagsbits.pluckflag) //compute timer value once
            {
                timer_value = 65536 - Fstart_tcy_halfperiod;
            }
            VWflagsbits.pluckflag = 0; //clear computation flag & wait for TMR3IF
        }
        IFS0bits.T3IF = 0; //clear TMR3 interrupt flag		

        VWflagsbits.pluckflag = 1; //set computation flag
        WriteTimer3(timer_value); //setup TMR3
        //EXCITE=0;														//set pluck low
        while (!IFS0bits.T3IF) //wait till TMR3 overflows
        {
            if (VWflagsbits.pluckflag) //compute new timer value once
            {
                Fstart_tcy_halfperiod = Fstart_tcy_halfperiod - step;
                timer_value = 65536 - Fstart_tcy_halfperiod;
            }
            VWflagsbits.pluckflag = 0; //clear computation flag and wait for TMR3IF
        }

    } //end of for loop

    IFS0bits.T3IF = 0; //clear TMR3 interrupt flag
    CloseTimer3(); //close the TMR3
    TMR3 = 0; //clear TMR3
    //ENABLE=0;															//disable the pluck driver
}

void processReading(float VWreading, int channel) {
    float VWreading_1000; //VWreading/1000

    switch (channel) {
        case 1:

            if (!MUX_CONVERSION1_16.c1flags.CH1) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;


        case 2:

            if (!MUX_CONVERSION1_16.c1flags.CH2) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;


        case 3:

            if (!MUX_CONVERSION1_16.c1flags.CH3) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;


        case 4:

            if (!MUX_CONVERSION1_16.c1flags.CH4) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;


        case 5:

            if (!MUX_CONVERSION1_16.c1flags.CH5) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 6:

            if (!MUX_CONVERSION1_16.c1flags.CH6) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 7:

            if (!MUX_CONVERSION1_16.c1flags.CH7) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 8:

            if (!MUX_CONVERSION1_16.c1flags.CH8) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 9:

            if (!MUX_CONVERSION1_16.c1flags.CH9) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 10:

            if (!MUX_CONVERSION1_16.c1flags.CH10) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 11:

            if (!MUX_CONVERSION1_16.c1flags.CH11) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 12:

            if (!MUX_CONVERSION1_16.c1flags.CH12) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 13:

            if (!MUX_CONVERSION1_16.c1flags.CH13) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 14:

            if (!MUX_CONVERSION1_16.c1flags.CH14) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 15:

            if (!MUX_CONVERSION1_16.c1flags.CH15) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 16:

            if (!MUX_CONVERSION1_16.c1flags.CH16) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 17:

            if (!MUX_CONVERSION17_32.c2flags.CH17) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;


        case 18:

            if (!MUX_CONVERSION17_32.c2flags.CH18) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;


        case 19:

            if (!MUX_CONVERSION17_32.c2flags.CH19) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;


        case 20:

            if (!MUX_CONVERSION17_32.c2flags.CH20) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;


        case 21:

            if (!MUX_CONVERSION17_32.c2flags.CH21) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 22:

            if (!MUX_CONVERSION17_32.c2flags.CH22) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 23:

            if (!MUX_CONVERSION17_32.c2flags.CH23) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 24:

            if (!MUX_CONVERSION17_32.c2flags.CH24) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 25:

            if (!MUX_CONVERSION17_32.c2flags.CH25) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 26:

            if (!MUX_CONVERSION17_32.c2flags.CH26) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 27:

            if (!MUX_CONVERSION17_32.c2flags.CH27) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 28:

            if (!MUX_CONVERSION17_32.c2flags.CH28) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 29:

            if (!MUX_CONVERSION17_32.c2flags.CH29) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 30:

            if (!MUX_CONVERSION17_32.c2flags.CH30) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 31:

            if (!MUX_CONVERSION17_32.c2flags.CH31) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;

        case 32:

            if (!MUX_CONVERSION17_32.c2flags.CH32) {
                LC2CONTROL.flags.Conversion = 0; //Linear Conversion
            } else {
                LC2CONTROL.flags.Conversion = 1; //Polynomial Conversion
            }
            break;
    }

    if (LC2CONTROL.flags.Conversion) //polynomial conversion
    {
        VWreading_1000 = VWreading * 0.001; //multiply digits by .001 for polynomial conversions
        VWreadingProcessed = (VWreading_1000 * VWreading_1000 * polyCoA)+(VWreading_1000 * polyCoB) + polyCoC;
        return;
    }

    if (LC2CONTROL2.flags2.R) {
        VWreadingProcessed = ((VWreading - zeroReading) * gageFactor) + gageOffset;
        return;
    }

    if (!LC2CONTROL2.flags2.R) {
        VWreadingProcessed = ((zeroReading - VWreading) * gageFactor) + gageOffset;
        return;
    }
}

void prompt(void) //transmit <CR><LF>
{
    putcUART1(cr);
    while (BusyUART1());
    putcUART1(lf);
    while (BusyUART1());
    putcUART1(asterisk);
    while (BusyUART1());
}

int qualifyNetAddress(void) {
    unsigned int NetAddress = 0;
    unsigned int StoredNetAddress = 0;
    char RxData = 0; //holds value of UART1 receive buffer
    int i = 0;
    int x = 0;
    int ones = 0;
    int tens = 0;
    int hundreds = 0;
    char buffer[4];

    for (i = 0; i < 4; i++) //clear the buffer
    {
        buffer[i] = 0;
    }

    i = 1; //set buffer index to 1

    shutdownTimer(3); //3S timeout if networked

    while (!IFS1bits.T5IF) //wait for "#" - timeout if 15 seconds of inactivity
    {
        shutdownTimer(3); //Reset 3S timer

        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF);

        if (IFS1bits.T5IF)
            return 0;

        if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
            handleCOMError();

        RxData = ReadUART1();
        if (RxData == pound)
            break; //"#" received
    }

    if (IFS1bits.T5IF)
        return 0;

    while (!IFS1bits.T5IF) {
        while (!DataRdyUART1() && !U1STAbits.FERR && !U1STAbits.PERR && !U1STAbits.OERR && !IFS1bits.T5IF);
        if (U1STAbits.FERR | U1STAbits.PERR | U1STAbits.OERR)
            handleCOMError();

        RxData = ReadUART1(); //get the char from the USART buffer
        if (RxData == 0x08) //decrement buffer index if backspace is received
            i--;

        if (i < 1) //don't let buffer index go below one
            i = 1;

        if (RxData != 0x08) //as long as char is not backspace
        {
            buffer[i] = RxData; //store the received char in the buffer
            i++;
        }

        if (RxData == cr) //CR received - buffer is ready
        {
            RxData = 0; //clear the receive buffer
            break;
        }
    }

    if (IFS1bits.T5IF)
        return 0;

    for (i = 1; buffer[i] != cr; i++) {
        if (!isdigit(buffer[i]))
            return 0; //ERROR - buffer[i] is not a digit
    }

    i--; //# of digits in network address

    switch (i) {
        case 1:
            NetAddress = buffer[1] - 0x30; //address 1-9
            break;
        case 2:
            tens = (buffer[1] - 0x30)*10; //address 10-99
            ones = buffer[2] - 0x30;
            NetAddress = tens + ones;
            break;
        case 3:
            hundreds = (buffer[1] - 0x30)*100; //address 100-256
            tens = (buffer[2] - 0x30)*10;
            ones = buffer[3] - 0x30;
            NetAddress = hundreds + tens + ones;
            break;
        default:
            break;
    }

    if (NetAddress <= 0 | NetAddress > 256) //invalid addresses
        return 0;

    StoredNetAddress=read_Int_FRAM(Netaddress);  

    if (NetAddress != StoredNetAddress) //address doesn't match
        return 0;

    return 1; //addresses match
}


//***************************************************************************
//			read_vw()
//
//	capture and process VW frequency
//
//	Parameters received: none
//	Returns: floating-point frequency
//
//	Called from: main()
//
//***************************************************************************

float read_vw(unsigned char gageType) {
    int samples; //VW reading loop iterations
    unsigned int j = 0;
    float period = 0.0;
    float period_total = 0;
    float frequency = 0.0;
    float avg_period = 0.0;
    float digits = 0.0;

    //**************Adaptive Multiple Period Averaging***********************
    period = getPeriod(gageType);
    if (period == -999999) //no gage connected
        return -999999; //return error code

    if (gageType != 3) {
        period = (period * tcy) / 16.0; //determine period with every 16 cycle capture
        samples = (0.6 / period) / 32; //determine maximum samples for 800mS reading window
    } else {
        period = (period * tcy) / 4.0; //determine period with every 4 cycle capture
        samples = (0.6 / period) / 16;
    }

    for (j = 0; j < samples; j++) {
        period = getPeriod(gageType);
        if (IFS0bits.T1IF) //exit loop if VW drops out
            break;
        period_total += period;
    }

    if (!IFS0bits.T1IF) //only if VW is valid
    {
        avg_period = (period_total / samples); //determine average period

        if (gageType != 3)
            period = (avg_period * tcy) / 16.0; //determine period with every 16 cycle capture
        else
            period = (avg_period * tcy) / 4.0; //determine period with every 4 cycle capture

        frequency = 1 / period; //test
    } else {
        VWflagsbits.VWerror = 1; //set reading error flag
    }

    IFS0bits.T1IF = 0; //reset TMR1IF
    digits = (frequency * frequency)*.001; //digits = F^2x10E-3 if linear conversion

    return digits; //return the digits
}

void resetMemory(void) {
    unsigned int data;


    data = 0; //reset memory pointers
    //data=4560;                                                                  //TEST VER 6.0.12
    //data=6390;                                                                  //TEST VER 6.0.4
    //data=9590;                                                                  //TEST VER 6.0.4
    //data=12790;                                                                 //TEST VER 6.0.4
    //data=15990;                                                                 //TEST VER 6.0.4
    write_Int_FRAM(MemoryStatusaddress,data);                                   
    write_Int_FRAM(Offsetaddress,data);    

    data = 1;
    //data=4561;                                                                  //TEST VER 6.0.4
    //data=6391;                                                                  //TEST VER 6.0.4
    //data=9591;                                                                  //TEST VER 6.0.4
    //data=12791;                                                                 //TEST VER 6.0.4
    //data=15991;                                                                 //TEST VER 6.0.4
    write_Int_FRAM(OutputPositionaddress,data);    
    write_Int_FRAM(UserPositionaddress,data);  

    DISPLAY_CONTROL.flags.monitorWasEnabled = 0;                                //clear the BPD flags
    DISPLAY_CONTROL.flags.newPointer = 0;
    DISPLAY_CONTROL.flags.BPD = 0;
    DISPLAY_CONTROL.flags.Backup = 0;
    DISPLAY_CONTROL.flags.Scan = 0;
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);        //store flags in FRAM 
}

void resetVWchannel(void) {
    Nop();
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

}

unsigned long RTChms2s(unsigned char x) //returns total seconds in RTC register
{ //x=0:return total in Alarm1 registers
    unsigned long total = 0; //x=1:return total in RTC registers
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
    decimalhours = bcd2d(hours); //convert RTC 2 digit BCD hours to decimal
    decimalhours *= 3600; //convert result to seconds

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

void seconds2hms(unsigned long s) {
    second = 0;
    hour = 0;
    minute = 0;

    while (1) {
        if (s < 60) {
            second = s; //extract seconds and return
            break;
        }

        if (s > 3599) {
            hour += 1; //increment hours
            s -= 3600;
        } else {
            minute += 1; //increment minutes
            s -= 60;
        }
    }
}

void setChannelConversion(int channel, int conversion) {
    switch (channel) //assign the proper conversion type per channel
    {
        case 1:
            MUX_CONVERSION1_16.c1flags.CH1 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH1 = 1; //polynomial conversion
            break;

        case 2:
            MUX_CONVERSION1_16.c1flags.CH2 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH2 = 1; //polynomial conversion
            break;

        case 3:
            MUX_CONVERSION1_16.c1flags.CH3 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH3 = 1; //polynomial conversion
            break;

        case 4:
            MUX_CONVERSION1_16.c1flags.CH4 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH4 = 1; //polynomial conversion
            break;

        case 5:
            MUX_CONVERSION1_16.c1flags.CH5 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH5 = 1; //polynomial conversion
            break;

        case 6:
            MUX_CONVERSION1_16.c1flags.CH6 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH6 = 1; //polynomial conversion
            break;

        case 7:
            MUX_CONVERSION1_16.c1flags.CH7 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH7 = 1; //polynomial conversion
            break;

        case 8:
            MUX_CONVERSION1_16.c1flags.CH8 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH8 = 1; //polynomial conversion
            break;

        case 9:
            MUX_CONVERSION1_16.c1flags.CH9 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH9 = 1; //polynomial conversion
            break;

        case 10:
            MUX_CONVERSION1_16.c1flags.CH10 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH10 = 1; //polynomial conversion
            break;

        case 11:
            MUX_CONVERSION1_16.c1flags.CH11 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH11 = 1; //polynomial conversion
            break;

        case 12:
            MUX_CONVERSION1_16.c1flags.CH12 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH12 = 1; //polynomial conversion
            break;

        case 13:
            MUX_CONVERSION1_16.c1flags.CH13 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH13 = 1; //polynomial conversion
            break;

        case 14:
            MUX_CONVERSION1_16.c1flags.CH14 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH14 = 1; //polynomial conversion
            break;

        case 15:
            MUX_CONVERSION1_16.c1flags.CH15 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH15 = 1; //polynomial conversion
            break;

        case 16:
            MUX_CONVERSION1_16.c1flags.CH16 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION1_16.c1flags.CH16 = 1; //polynomial conversion
            break;

        case 17:
            MUX_CONVERSION17_32.c2flags.CH17 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH17 = 1; //polynomial conversion
            break;

        case 18:
            MUX_CONVERSION17_32.c2flags.CH18 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH18 = 1; //polynomial conversion
            break;

        case 19:
            MUX_CONVERSION17_32.c2flags.CH19 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH19 = 1; //polynomial conversion
            break;

        case 20:
            MUX_CONVERSION17_32.c2flags.CH20 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH20 = 1; //polynomial conversion
            break;

        case 21:
            MUX_CONVERSION17_32.c2flags.CH21 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH21 = 1; //polynomial conversion
            break;

        case 22:
            MUX_CONVERSION17_32.c2flags.CH22 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH22 = 1; //polynomial conversion
            break;

        case 23:
            MUX_CONVERSION17_32.c2flags.CH23 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH23 = 1; //polynomial conversion
            break;

        case 24:
            MUX_CONVERSION17_32.c2flags.CH24 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH24 = 1; //polynomial conversion
            break;

        case 25:
            MUX_CONVERSION17_32.c2flags.CH25 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH25 = 1; //polynomial conversion
            break;

        case 26:
            MUX_CONVERSION17_32.c2flags.CH26 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH26 = 1; //polynomial conversion
            break;

        case 27:
            MUX_CONVERSION17_32.c2flags.CH27 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH27 = 1; //polynomial conversion
            break;

        case 28:
            MUX_CONVERSION17_32.c2flags.CH28 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH28 = 1; //polynomial conversion
            break;

        case 29:
            MUX_CONVERSION17_32.c2flags.CH29 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH29 = 1; //polynomial conversion
            break;

        case 30:
            MUX_CONVERSION17_32.c2flags.CH30 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH30 = 1; //polynomial conversion
            break;

        case 31:
            MUX_CONVERSION17_32.c2flags.CH31 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH31 = 1; //polynomial conversion
            break;

        case 32:
            MUX_CONVERSION17_32.c2flags.CH32 = 0; //linear conversion
            if (conversion == 1)
                MUX_CONVERSION17_32.c2flags.CH32 = 1; //polynomial conversion
            break;

        default:
            return;

    }
    write_Int_FRAM(MUX_CONV1_16flagsaddress,MUX_CONVERSION1_16.MUXconv1_16);              //store flag in FRAM 
    write_Int_FRAM(MUX_CONV17_32flagsaddress,MUX_CONVERSION17_32.MUXconv17_32);              //store flag in FRAM  
}

void setup(void)
{
    //Configure PORTA:
    TRISA=0x1400; 
    LATA=0x000C;   
    AD1PCFGHbits.PCFG16=1;                                                      //Setup RC1 for Digital input instead of analog
    AD1PCFGHbits.PCFG20=1;                                                      //Setup RA12 for Digital input instead of analog
    AD1PCFGHbits.PCFG28=1;                                                      //Setup RE4 for Digital input instead of analog
    
    //Configure PORTB:
    TRISB=0x013D;
    LATB=0;
    //Configure PORTC:
    TRISC=0x9002;
    LATC=0x0018;
    //Configure PORTD:
    TRISD=0x0020;
    LATD=0x0002;                                                                
    //Configure PORTE:
    TRISE=0x10;
    LATE=0;
    //Configure PORTF:
    TRISF=0x0184;
    LATF=0x005A;
    //Configure PORTG:
    TRISG=0x0000;
    LATG=0x000C;                                                                
}

void shutdown(void) {
    mainBatreading = take_analog_reading(87); //test the 12V_SENSE input
    if (mainBatreading < 820) //12V_SENSE <0.5V, so not connected
    {
        DISPLAY_CONTROL.flags.PS12V = 0; //clear the 12V battery flag
    } else {
        DISPLAY_CONTROL.flags.PS12V = 1; //set the 12V battery flag
    }

    _3VX_off(); //make sure analog +5VX is off
    configUARTsleep(); //config UART1 to wakeup uC on start bit
    INTCON1bits.NSTDIS = 0; //enable interrupt nesting
    CORCONbits.IPL3 = 0; //make sure IPL3 is not set
    netTest = 0; //clear the netTest register

    if (DISPLAY_CONTROL.flags.Shutdown)
        Nop();
    // _RS485TX_EN=1;                                                          //disable the RS485 Tx

    if (DISPLAY_CONTROL.flags.PS12V) //powered by 12V?
        SLEEP12V = 1; //set regulator into linear LDO mode

    //_232SHDN=0;                                                                 //shut down the RS-232 Port VER 6.0.4
    Sleep(); //night night & continue with next command after wakeup - no vector
    SLEEP12V = 0; //set regulator into switchmode when wake from sleep
}

void shutdownNetworked(void) {
    _3VX_off(); //make sure analog +3VX is off
    Sleep(); //continue with next command after wakeup - no vector
    SLEEP12V = 0; //set regulator into switchmode when wake from sleep
}

void startLogging(void) {
    WDTSWEnable;

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

    if (!DISPLAY_CONTROL.flags.Shutup) {
        putsUART1(Loggingstarted); //Logging started
        while (BusyUART1());
        putcUART1(cr);
        while (BusyUART1());
        putcUART1(nul);
        while (BusyUART1());
        putcUART1(lf);
        while (BusyUART1());
    }

    if (!LC2CONTROL.flags.NetEnabled && (USB_PWR | !_232)) //VER 6.0.0
        T4CONbits.TON = 1; //Turn timer back on
    LC2CONTROL.flags.Logging = 1; //Set Logging flag
    LC2CONTROL.flags.LoggingStartTime = 0;
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flags in FRAM 

    DISPLAY_CONTROL.flags.monitorWasEnabled = 0; //clear the BPD flags
    DISPLAY_CONTROL.flags.newPointer = 0;
    DISPLAY_CONTROL.flags.BPD = 0;
    DISPLAY_CONTROL.flags.Backup = 0;
    DISPLAY_CONTROL.flags.Scan = 0;
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM`

    crlf();

    LC2CONTROL2.flags2.Waiting = 0; //Clear the Waiting flag
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flag in FRAM`

    ConfigIntCapture8(IC_INT_ON & IC_INT_PRIOR_6); //Configure input capture interrupt

    take_One_Complete_Reading(STORE); //take a reading    VER 6.0.13

    enableAlarm(Alarm1);
    enableINT1(); //enable INT1 (take a reading on interrupt)
    configUARTnormal();
    INTCON1bits.NSTDIS = 0; //Reset nesting of interrupts
    ClrWdt();
    WDTSWDisable;
}

void stopLogging(void) {
    PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress);    
    if (!PORT_CONTROL.flags.Alarm2Enabled)
        disableINT1();
    disableAlarm(Alarm1);
    LC2CONTROL.flags.Logging = 0; //Clear Logging flag
    LC2CONTROL.flags.LoggingStartTime = 0; //Clear Start Logging flag
    LC2CONTROL.flags.LoggingStopTime = 0; //Clear Stop Logging flag
    write_Int_FRAM(LC2CONTROLflagsaddress,LC2CONTROL.full);	//store flag in FRAM  `

    LC2CONTROL2.flags2.Waiting = 0; //Clear the Waiting flag
    LC2CONTROL2.flags2.SetStopTime = 0; //clear the set stop time flag
    write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flag in FRAM  

    DISPLAY_CONTROL.flags.Synch = 1; //set the Synch flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM

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

void storeGageType(int channel, int gageType) {
    unsigned int address;

    //calculate indexed address for gage type                                   
    address = CH1GTaddress + (0x1A * (channel - 1));

    //write the gage type to the calculated address
    write_Int_FRAM(address,gageType);                                           //store gage type in FRAM 
}

//REV B - storeLogInterval() may need more work:
void storeLogInterval(int interval, int iterations) 
{
    interval -= 1;                                                              //change interval from 1-6 to 0-5

    if (MUX4_ENABLE.mflags.mux16_4 == Single)                                   //if Single Channel  
    {
        write_longFRAM(LogIntLength,(SingleLogIntLength1address+(interval*8)));	//store the interval    
        write_Int_FRAM((SingleLogIt1address+(interval*8)), iterations);         //store the iterations  
        write_Int_FRAM((SingleLogItRemain1address+(interval*8)), iterations);	//store the remaining iterations    
    }
}

void storeChannelReading(int ch) {
    unsigned long FRAMaddress;
    unsigned char baseAddress = 0; //VER 6.0.10

    outputPosition=read_Int_FRAM(OutputPositionaddress);                       //get the memory pointer  

    //VER 6.0.10:***************************************************************
    if (MUX4_ENABLE.mflags.mux16_4 == Single) //if Single Channel  VER 6.0.10
        baseAddress = SingleVWBytes;
    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //if 4 channel MUX  VER 6.0.10
        baseAddress = VW4Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //if 8 channel MUX  VER 6.0.10
        baseAddress = VW8Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //if 16 channel MUX  VER 6.0.10
        baseAddress = VW16Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //if 32 channel MUX  VER 6.0.10
        baseAddress = VW32Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //if 8 channel Thermistor MUX  VER 6.0.10
        baseAddress = TH8Bytes;
    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //if 32 channel Thermistor MUX  VER 6.0.10
        baseAddress = TH32Bytes;

    if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32) //8 or 32 Channel Thermistor
        FRAMaddress = (baseAddress * (outputPosition - 1))+(2 * (ch - 1) + 12);
    else
        if (MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //8 or 32 Channel VW    VER 6.0.13
        FRAMaddress = (baseAddress * (outputPosition - 1))+(4 * (ch - 1) + 12);
    else
        FRAMaddress = (baseAddress * (outputPosition - 1))+(6 * (ch - 1) + 12); //calculate the external FRAM address

    //if(MUX4_ENABLE.mflags.mux16_4!=TH8 && MUX4_ENABLE.mflags.mux16_4!=TH32)     //VER 6.0.9   REM VER 6.0.13
    if (MUX4_ENABLE.mflags.mux16_4 == Single | MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) //VER 6.0.13
    {
        write_Flt_FRAM(FRAMaddress, VWreadingProcessed); //store the processed VW reading
        write_Int_FRAM(FRAMaddress + 4, extThermreading); //store the external thermistor reading 12bit ADC value
    } else
        if (MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //VER 6.0.13
    {
        write_Flt_FRAM(FRAMaddress, VWreadingProcessed); //store the processed VW reading
    } else {
        write_Int_FRAM(FRAMaddress, extThermreading); //8 or 32 channel therm mux: store the external thermistor reading 12bit ADC value
    }

}

unsigned long storeReading(int ch) {
    unsigned long FRAMaddress;
    unsigned int Y = 0; //TEST VER 6.0.4
    unsigned char baseAddress = 0; //VER 6.0.10


    
    memoryStatus=read_Int_FRAM(MemoryStatusaddress);                           //get the memory status
    outputPosition=read_Int_FRAM(OutputPositionaddress);                       //get the memory pointer
    userPosition=read_Int_FRAM(UserPositionaddress);                           //get the user position
    FRAM_MEMORY.memory=read_Int_FRAM(FRAM_MEMORYflagsaddress);             //get max # of arrays
    

    /*REM VER 6.0.10:
    if(MUX4_ENABLE.mflags.mux16_4==VW4)                                           //if 4 channel MUX  VER 6.0.7
        FRAMaddress=(36*(outputPosition-1));                                  //calculate the external FRAM address (4 channel)
    if(MUX4_ENABLE.mflags.mux16_4==VW16)                                           //if 16 channel MUX VER 6.0.7
        FRAMaddress=(108*(outputPosition-1));                                 //calculate the external FRAM address (16 channel)
    if(MUX4_ENABLE.mflags.mux16_4==Single)                                           //Single Channel    VER 6.0.7
        FRAMaddress=(20*(outputPosition-1));                                  //calculate the external FRAM address (single channel);
    if(MUX4_ENABLE.mflags.mux16_4==TH8)                                           //8 Channel    VER 6.0.7
        FRAMaddress=(30*(outputPosition-1));                                  //calculate the external FRAM address (8 channel);
    if(MUX4_ENABLE.mflags.mux16_4==TH32)                                           //32 Channel    VER 6.0.9
        FRAMaddress=(80*(outputPosition-1));                                  //calculate the external FRAM address (32 channel);
     */

    //VER 6.0.10:
    if (MUX4_ENABLE.mflags.mux16_4 == Single) //if Single Channel VW
        baseAddress = SingleVWBytes; //external FRAM base address (Single channel)
    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //if 4 channel VW MUX
        baseAddress = VW4Bytes; //external FRAM base address (4 channel)
    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //if 8 channel VW MUX
        baseAddress = VW8Bytes; //external FRAM base address (8 channel)
    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //if 16 channel VW MUX
        baseAddress = VW16Bytes; //external FRAM base address (16 channel)
    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //if 32 channel VW MUX
        baseAddress = VW32Bytes; //external FRAM base address (4 channel)
    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //if 8 channel Thermistor MUX
        baseAddress = TH8Bytes; //external FRAM base address (8 channel Thermistor)
    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //if 32 channel Thermistor MUX
        baseAddress = TH32Bytes; //external FRAM base address (32 channel Thermistor)
    FRAMaddress = (baseAddress * (outputPosition - 1)); //calculate the external FRAM address


    write_Int_FRAM(FRAMaddress, year); //store the year data
    write_Int_FRAM(FRAMaddress + 2, julian); //store the decimal date
    write_Flt_FRAM(FRAMaddress + 4, seconds_since_midnight); //store the current time (absolute)
    write_Int_FRAM(FRAMaddress + 8, mainBatreading); //store the 3V battery voltage 12bit ADC value
    write_Int_FRAM(FRAMaddress + 10, intThermreading); //store the internal thermistor reading 12bit ADC value

    //VER 6.0.3:
    if (MUX4_ENABLE.mflags.mux16_4 == Single) //single channel VW selected   VER 6.0.7
    {
        if (gageType == 95)
            write_Flt_FRAM(FRAMaddress + 12, Lithiumreading + 0.19);
        else
            write_Flt_FRAM(FRAMaddress + 12, VWreadingProcessed);
        write_Int_FRAM(FRAMaddress + 16, extThermreading); //store the external thermistor reading 12bit ADC value

        if (memoryStatus < maxSingleVW) //if memory has not become full VER 6.0.3
            memoryStatus += 1; //add 1 to the memory status register
            write_Int_FRAM(MemoryStatusaddress,memoryStatus);           //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //if 4 channel VW MUX  VER 6.0.7
    {
        if (memoryStatus < maxFourVW) //if memory has not become full
            memoryStatus += 1; //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);           //store memoryStatus and userPosition in internal FRAM    
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //if 8 channel VW MUX  VER 6.0.7
    {
        if (memoryStatus < maxEightVW) //if memory has not become full
            memoryStatus += 1; //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);           //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //if 16 channel VW MUX VER 6.0.7
    {
        if (memoryStatus < maxSixteenVW) //if memory has not become full
            memoryStatus += 1; //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);           //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //if 32 channel VW MUX  VER 6.0.7
    {
        if (memoryStatus < maxThirtytwoVW) //if memory has not become full
            memoryStatus += 1; //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);           //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //if 8 channel MUX VER 6.0.7
    {
        if (memoryStatus < maxEightTH) //if memory has not become full
            memoryStatus += 1; //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);           //store memoryStatus and userPosition in internal FRAM   
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //if 32 channel MUX VER 6.0.9
    {
        if (memoryStatus < maxThirtytwoTH) //if memory has not become full
            memoryStatus += 1; //add 1 to the memory status register
        write_Int_FRAM(MemoryStatusaddress,memoryStatus);           //store memoryStatus and userPosition in internal FRAM   
    }

    outputPosition += 1; //increment the output position pointer


    if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel VW VER 6.0.7
    {
        if (outputPosition > maxSingleVW) //if memory is full VER 6.0.3
            wrap_stop(); //VER 6.0.12
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //if 4 channel VW MUX  VER 6.0.7
    {
        if (outputPosition > maxFourVW) //if memory is full
            wrap_stop(); //VER 6.0.12
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //if 8 channel VW MUX  VER 6.0.7
    {
        if (outputPosition > maxEightVW) //if memory is full
            wrap_stop(); //VER 6.0.12
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //if 16 channel VW MUX VER 6.0.7
    {
        if (outputPosition > maxSixteenVW) //if memory is full
            wrap_stop(); //VER 6.0.12
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //if 32 channel VW MUX VER 6.0.7
    {
        if (outputPosition > maxThirtytwoVW) //if memory is full
            wrap_stop(); //VER 6.0.12
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //if 8 channel MUX VER 6.0.7
    {
        if (outputPosition > maxEightTH) //if memory is full
            wrap_stop(); //VER 6.0.12
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //if 32 channel MUX VER 6.0.9
    {
        if (outputPosition > maxThirtytwoTH) //if memory is full
            wrap_stop(); //VER 6.0.12
    }

    if(!DISPLAY_CONTROL.flags.WrapMemory)                                       //is logging to stop when memory is full?   
      stopLogging();                                                            //YES   

    write_Int_FRAM(OutputPositionaddress,outputPosition);                       //store outputPosition pointer in internal FRAM  

    if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel VW  VER 6.0.7
    { //User Position = Memory Status until Memory is full
        if (memoryStatus < maxSingleVW) //VER 6.0.3
            userPosition = memoryStatus;

        if (memoryStatus >= maxSingleVW && outputPosition == 1) //VER 6.0.3
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1; //after that User Position equals Output Position - 1

        if (userPosition == (maxSingleVW + 1)) //VER 6.0.3
            userPosition = 1;
    }


    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //if 4 channel VW MUX  VER 6.0.7
    { //User Position = Memory Status until Memory is full
        if (memoryStatus < maxFourVW)
            userPosition = memoryStatus;
        //User Position = Output Position for first reading
        if (memoryStatus >= maxFourVW && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1; //after that User Position equals Output Position - 1

        if (userPosition == (maxFourVW + 1))
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //if 8 channel VW MUX  VER 6.0.7
    { //User Position = Memory Status until Memory is full
        if (memoryStatus < maxEightVW)
            userPosition = memoryStatus;
        //User Position = Output Position for first reading
        if (memoryStatus >= maxEightVW && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1; //after that User Position equals Output Position - 1

        if (userPosition == (maxEightVW + 1))
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //do same for 16 channel VW MUX    VER 6.0.7
    { //User Position = Memory Status until Memory is full
        if (memoryStatus < maxSixteenVW)
            userPosition = memoryStatus;
        //User Position = Output Position for first reading
        if (memoryStatus >= maxSixteenVW && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1; //after that User Position equals Output Position - 1

        if (userPosition == (maxSixteenVW + 1))
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //do same for 32 channel VW MUX    VER 6.0.7
    { //User Position = Memory Status until Memory is full
        if (memoryStatus < maxThirtytwoVW)
            userPosition = memoryStatus;
        //User Position = Output Position for first reading
        if (memoryStatus >= maxThirtytwoVW && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1; //after that User Position equals Output Position - 1

        if (userPosition == (maxThirtytwoVW + 1))
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //do same for 8 channel MUX    VER 6.0.7
    { //User Position = Memory Status until Memory is full
        if (memoryStatus < maxEightTH)
            userPosition = memoryStatus;
        //User Position = Output Position for first reading
        if (memoryStatus >= maxEightTH && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1; //after that User Position equals Output Position - 1

        if (userPosition == 12801)
            userPosition = 1;
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //do same for 32 channel MUX    VER 6.0.9
    { //User Position = Memory Status until Memory is full
        if (memoryStatus < maxThirtytwoTH)
            userPosition = memoryStatus;
        //User Position = Output Position for first reading
        if (memoryStatus >= maxThirtytwoTH && outputPosition == 1)
            userPosition = outputPosition;
        else
            userPosition = outputPosition - 1; //after that User Position equals Output Position - 1

        if (userPosition == 4801)
            userPosition = 1;
    }

    write_Int_FRAM(UserPositionaddress,userPosition);  

    if (MUX4_ENABLE.mflags.mux16_4 == Single) //Single Channel VW  VER 6.0.7
    {
        if (outputPosition == 1) //memory has rolled over
            outputPosition = maxSingleVW; //point to last array for displayReading() VER 6.0.3
        else
            outputPosition -= 1; //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //if 4 channel VW MUX  VER 6.0.7
    {
        if (outputPosition == 1) //memory has rolled over
            outputPosition = maxFourVW; //point to last array for displayReading()
        else
            outputPosition -= 1; //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //if 8 channel VW MUX  VER 6.0.7
    {
        if (outputPosition == 1) //memory has rolled over
            outputPosition = maxEightVW; //point to last array for displayReading()
        else
            outputPosition -= 1; //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //if 16 channel VW MUX VER 6.0.7
    {
        if (outputPosition == 1) //memory has rolled over
            outputPosition = maxSixteenVW; //point to last array for displayReading()
        else
            outputPosition -= 1; //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //if 32 channel VW MUX VER 6.0.7
    {
        if (outputPosition == 1) //memory has rolled over
            outputPosition = maxThirtytwoVW; //point to last array for displayReading()
        else
            outputPosition -= 1; //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //if 8 channel MUX VER 6.0.7
    {
        if (outputPosition == 1) //memory has rolled over
            outputPosition = maxEightTH; //point to last array for displayReading()
        else
            outputPosition -= 1; //otherwise point to the reading just stored
    }

    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //if 32 channel MUX VER 6.0.9
    {
        if (outputPosition == 1) //memory has rolled over
            outputPosition = maxThirtytwoTH; //point to last array for displayReading()
        else
            outputPosition -= 1; //otherwise point to the reading just stored
    }


    FRAM_MEMORY.flags.memEmpty = 0; //clear the memory empty flag
    write_Int_FRAM(FRAM_MEMORYflagsaddress,FRAM_MEMORY.memory);	//store flag in FRAM 

    return outputPosition; //return the pointer for use in displayReading()
}

void storeTempChannelReading(int ch)                                            //Store 'X' command readings to internal FRAM
{
    unsigned long TempFRAMaddress;

    if (MUX4_ENABLE.mflags.mux16_4 == Single | 
            MUX4_ENABLE.mflags.mux16_4 == VW4 |
            MUX4_ENABLE.mflags.mux16_4 == VW16) 
    {
        TempFRAMaddress = (XmemStart+0xC) + (6 * (ch - 1));                     //compute memory location for external thermistor   REV B
        write_Int_FRAM(TempFRAMaddress,extThermreading);                        //store the external thermistor reading 12bit ADC value 
        TempFRAMaddress = (XmemStart+0xE) + (6 * (ch - 1));                     //compute memory location for VW reading            REV B
        write_Flt_FRAM(TempFRAMaddress,VWreadingProcessed);                     //store the processed VW reading  REV B
    }

    if (MUX4_ENABLE.mflags.mux16_4 == VW8 | 
            MUX4_ENABLE.mflags.mux16_4 == VW32) 
    {
        TempFRAMaddress = (XmemStart+0xC) + (4 * (ch - 1));                     //compute memory location for VW reading            REV B
        write_Flt_FRAM(TempFRAMaddress,VWreadingProcessed);                     //store the processed VW reading     REV B
    }

}

void storeTempChannelTHReading(int ch) 
{
    unsigned long TempFRAMaddress;

    TempFRAMaddress = (XmemStart+0xC) + (2 * (ch - 1));                         //compute memory location for external thermistor    REV B
    write_Int_FRAM(TempFRAMaddress,extThermreading);                            //store the external thermistor reading 12bit ADC value 
}

void storeTempReading(int ch) 
{
    write_Int_FRAM(XmemStart,year);                                              //store the year data 
    write_Int_FRAM(XmemStart+2,julian);                                          //store the decimal date 
    write_longFRAM(seconds_since_midnight,XmemStart+4);                         //store the current time (absolute) 
    write_Int_FRAM(XmemStart+8,mainBatreading);                                 //store the 3V battery voltage 12bit ADC value 
    write_Int_FRAM(XmemStart+10,intThermreading);                               //store the internal thermistor reading 12bit ADC value 
}

unsigned long synch(unsigned long currenttimeseconds, unsigned long scaninterval) 
{
    unsigned int scanintmul = 0; //scan interval multiplier

    for (scanintmul = 1; scaninterval * scanintmul < currenttimeseconds; scanintmul++) 
    {
    }                                                                           //determine next evenly divisible reading time based on
                                                                                //the scan interval
                                                                                //determine value to add to current clock (syncvalue) to
                                                                                // make the next reading synchronous
    VWflagsbits.synch = 0;                                                      //clear the synch flag
    return (scaninterval * scanintmul);
}                                                                               //close synch()

int take_analog_reading(unsigned char gageType) {
    char analogBUF[10];
    unsigned int analog;
    unsigned int *ADC16Ptr;
    unsigned int count;



    if (gageType >= 85 && gageType <= 97) {
        if (gageType == 86) //read the internal temperature
        {
            Nop(); //get the temp reading from the RTC
            return analog;
        }

        _3VX_on(); //power-up analog circuitry 

        //Setup 12 bit DAC:
        AD1PCFGH = 0xFFFF; //AN31..AN16 configured as digital I/O
        AD1PCFGL = 0xFEC2; //AN8,5,4,3,2,0 configured as analog inputs
        AD1CON2 = 0x203C; //interrupt after every 16 samples,converts CH0 only,don't scan inputs and external Vref 
        AD1CON3 = 0x0807; //Tad=8*Tcy, Sample time = 8*Tad
        AD1CON1 = 0x04E4; //12 bit ADC,internal counter ends sampling/starts conversion, sampling auto-start enabled

        if (gageType == 85) //read the external thermistor
            AD1CHS0 = 0x0004; //connect AN4 as CH0 positive input
        //read DS3231 temperature output
        if (gageType == 87) //read the main 12V battery
            AD1CHS0 = 0x0003; //connect AN3 as CH0 input
        if (gageType == 95) //read the 3V lithium cell
        {
            AD1CHS0 = 0x0005; //connect AN5 as CH0 input
            SAMPLE_LITHIUM = 1; //sample the lithium battery
        }
        if (gageType == 97) //read the main 3V battery
            AD1CHS0 = 0x0002; //connect AN2 as CH0 input

        analog = 0; //clear the value
        ADC16Ptr = &ADC1BUF0; //intialize ADCBUF pointer
        IFS0bits.AD1IF = 0; //clear ADC interrupt flag

        AD1CON1bits.ADON = 1; //turn ADC on	
        delay(1000); //delay 136uS for settling	

        while (!IFS0bits.AD1IF); //conversion done?

        SAMPLE_LITHIUM = 0; //turn off lithium battery sampling if on
        AD1CON1bits.ADON = 0; //turn ADC off	
        _3VX_off(); //power-down analog circuitry   

        for (count = 0; count < 16; count++) //totalize the values from the buffer
        {
            analog = analog + *ADC16Ptr++;
        }

        analog = analog >> 4; //average the result
        return analog; //return the averaged 12 bit value	
    }

}



void take_One_Complete_Reading(unsigned char store) {
    char BUF[17]; //temporary storage for display
    int i;
    int ch; //for loop index
    int ch_max;
    int skip;
    unsigned long outputPosition;
    unsigned long ReadingTimeSeconds;
    float batteryVoltage = 0.0;

    if (store) //VER 6.0.13
    {
        SLEEP12V = 0; //set regulator into switchmode when wake from sleep
        LC2CONTROL.flags.ScanError = 0; //clear the flag
        setClock(0x0F, 0); //Clear the RTC Alarm flags
        ReadingTimeSeconds = RTChms2s(1); //get the current time from the RTC
        DISPLAY_CONTROL.flags.TakingReading = 1; //set the TakingReading flag
    }

    IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
    seconds_since_midnight = RTChms2s(1); //get the total seconds from RTC
    RTCdays = readClock(RTCDaysAddress); //get the day from the RTC
    RTCmonths = readClock(RTCMonthsAddress); //get the month from the RTC
    RTCyears = readClock(RTCYearsAddress); //get the year from the RTC
    IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
    Nop();
    Nop();

    if (store) //VER 6.0.13
    {
        upD8RTCAlarm1(); //determine and load next reading time
    } else {
        if (LC2CONTROL2.flags2.interrupt) //did INT2 interrupt occur during this period?
            return; //abort if it did
    }

    year = RTCtoDecimal(RTCyears); //convert BCD RTC data to decimal
    month = RTCtoDecimal(RTCmonths);
    day = RTCtoDecimal(RTCdays);
    julian = toJulian(); //convert date to decimal date

    if (store)
        restoreSettings();


    IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
    TotalStopSeconds=read_longFRAM(TotalStopSecondsaddress);                 //get the stored stop time    
    LoggingStartDay=read_Int_FRAM(LoggingStartDayaddress);                     //Get the Logging Start day from FRAM   
    IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
    Nop();
    Nop();

    if (!store && LC2CONTROL2.flags2.interrupt)
        return;

    if (store) {
        if (LoggingStartDay != day && TotalStopSeconds >= 86400) //Did midnight rollover occur?
        {
            TotalStopSeconds -= 86400; //compensate TotalStopSeconds
            write_longFRAM(TotalStopSeconds,TotalStopSecondsaddress);            //update FRAM   
        }

        if (LC2CONTROL.flags.LoggingStopTime && (TotalStopSeconds < seconds_since_midnight)) //if scheduled Stop Logging is enabled
        {
            DISPLAY_CONTROL.flags.TakingReading = 0; //Reset the Taking Reading flag
            write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM VER 6.0.13  
            stopLogging(); //and it's past the Stop Logging time
            return;
        }
    } else {
        if (LoggingStartDay != day) {
            TotalStopSeconds -= 86400;
            LoggingStartDay = day;
        }
    }


    // TAKE VW READINGS:
    if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) //activate mux if multichannel  VER 6.0.7
        MUX_RESET = 1; //set the MUX_RESET line high
    if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32 |
            MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //if 8 or 32 channel mux  VER 6.0.9
        MUX_CLOCK = 1; //set the MUX_CLOCK line High   VER 6.0.6
    delay(100000); //10mS delay for settling

    if (MUX4_ENABLE.mflags.mux16_4 == Single) //single channel selected   VER 6.0.7
        ch_max = 1;
    if (MUX4_ENABLE.mflags.mux16_4 == VW4) //4 channel mux selected    VER 6.0.7
        ch_max = 4;
    if (MUX4_ENABLE.mflags.mux16_4 == VW8) //8 channel mux selected    VER 6.0.7
        ch_max = 8;
    if (MUX4_ENABLE.mflags.mux16_4 == VW16) //16 channel mux selected   VER 6.0.7
        ch_max = 16;
    if (MUX4_ENABLE.mflags.mux16_4 == VW32) //32 channel mux selected    VER 6.0.7
        ch_max = 32;
    if (MUX4_ENABLE.mflags.mux16_4 == TH8) //8 channel mux selected    VER 6.0.7
        ch_max = 8;
    if (MUX4_ENABLE.mflags.mux16_4 == TH32) //32 channel mux selected    VER 6.0.9
        ch_max = 32;

    for (ch = 1; ch <= ch_max; ch++) //mux loop
    {
        if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) //activate mux if multichannel  VER 6.0.7
            clockMux(10000); //double pulse clock the MUX
        if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32 |
                MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //if 8 or 32 channel mux  VER 6.0.9)  //8 or 32 channel mux VER 6.0.9
            clockThMux(10000); //VER 6.0.6
        MUX4_ENABLE.mflags.skip = 0; //clear channel skip flag

        if (!store)
            IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt

        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) //VER 6.0.9
        {
            switch (ch) //load the channel parameters
            {
                case 1: //Channel 1

                    if (!MUX_ENABLE1_16.e1flags.CH1) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH1GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH1ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH1GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH1GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH1PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH1PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH1PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 2: //Channel 2

                    if (!MUX_ENABLE1_16.e1flags.CH2) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                     gageType=read_Int_FRAM(CH2GTaddress);                  //load the gage type from internal FRAM
                     zeroReading=read_longFRAM(CH2ZRaddress);             //load the zero reading from internal FRAM
                     gageFactor=read_longFRAM(CH2GFaddress);              //load the gage factor from internal FRAM
                     gageOffset=read_longFRAM(CH2GOaddress);              //load the gage offset from internal FRAM
                     polyCoA=read_longFRAM(CH2PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                     polyCoB=read_longFRAM(CH2PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                     polyCoC=read_longFRAM(CH2PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 3: //Channel 3

                    if (!MUX_ENABLE1_16.e1flags.CH3) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                     gageType=read_Int_FRAM(CH3GTaddress);                  //load the gage type from internal FRAM
                     zeroReading=read_longFRAM(CH3ZRaddress);             //load the zero reading from internal FRAM
                     gageFactor=read_longFRAM(CH3GFaddress);              //load the gage factor from internal FRAM
                     gageOffset=read_longFRAM(CH3GOaddress);              //load the gage offset from internal FRAM
                     polyCoA=read_longFRAM(CH3PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                     polyCoB=read_longFRAM(CH3PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                     polyCoC=read_longFRAM(CH3PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 4: //Channel 4

                    if (!MUX_ENABLE1_16.e1flags.CH4) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH4GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH4ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH4GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH4GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH4PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH4PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH4PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                     
                    break; //break out of switch(ch)


                case 5:

                    if (!MUX_ENABLE1_16.e1flags.CH5) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH5GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH5ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH5GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH5GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH5PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH5PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH5PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 6: //Channel 6

                    if (!MUX_ENABLE1_16.e1flags.CH6) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                     gageType=read_Int_FRAM(CH6GTaddress);                  //load the gage type from internal FRAM
                     zeroReading=read_longFRAM(CH6ZRaddress);             //load the zero reading from internal FRAM
                     gageFactor=read_longFRAM(CH6GFaddress);              //load the gage factor from internal FRAM
                     gageOffset=read_longFRAM(CH6GOaddress);              //load the gage offset from internal FRAM
                     polyCoA=read_longFRAM(CH6PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                     polyCoB=read_longFRAM(CH6PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                     polyCoC=read_longFRAM(CH6PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                     
                    break; //break out of switch(ch)


                case 7: //Channel 7

                    if (!MUX_ENABLE1_16.e1flags.CH7) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                   
                    gageType=read_Int_FRAM(CH7GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH7ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH7GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH7GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH7PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH7PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH7PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 8: //Channel 8

                    if (!MUX_ENABLE1_16.e1flags.CH8) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH8GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH8ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH8GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH8GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH8PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH8PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH8PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 9: //Channel 9

                    if (!MUX_ENABLE1_16.e1flags.CH9) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH9GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH9ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH9GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH9GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH9PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH9PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH9PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 10: //Channel 10

                    if (!MUX_ENABLE1_16.e1flags.CH10) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH10GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH10ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH10GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH10GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH10PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH10PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH10PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 11: //Channel 11

                    if (!MUX_ENABLE1_16.e1flags.CH11) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH11GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH11ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH11GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH11GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH11PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH11PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH11PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 12: //Channel 12

                    if (!MUX_ENABLE1_16.e1flags.CH12) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH12GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH12ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH12GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH12GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH12PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH12PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH12PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 13: //Channel 13

                    if (!MUX_ENABLE1_16.e1flags.CH13) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH13GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH13ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH13GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH13GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH13PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH13PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH13PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 14: //Channel 14

                    if (!MUX_ENABLE1_16.e1flags.CH14) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH14GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH14ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH14GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH14GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH14PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH14PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH14PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 15: //Channel 15

                    if (!MUX_ENABLE1_16.e1flags.CH15) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH15GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH15ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH15GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH15GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH15PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH15PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH15PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 16: //Channel 16

                    if (!MUX_ENABLE1_16.e1flags.CH16) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH16GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH16ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH16GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH16GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH16PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH16PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH16PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 17: //Channel 17

                    if (!MUX_ENABLE17_32.e2flags.CH17) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH17GTaddress);                 //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH17ZRaddress);            //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH17GFaddress);             //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH17GOaddress);             //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH17PolyCoAaddress);           //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH17PolyCoBaddress);           //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH17PolyCoCaddress);           //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 18: //Channel 18

                    if (!MUX_ENABLE17_32.e2flags.CH18) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH18GTaddress);                 //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH18ZRaddress);            //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH18GFaddress);             //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH18GOaddress);             //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH18PolyCoAaddress);           //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH18PolyCoBaddress);           //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH18PolyCoCaddress);           //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 19: //Channel 19

                    if (!MUX_ENABLE17_32.e2flags.CH19) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH19GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH19ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH19GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH19GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH19PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH19PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH19PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 20: //Channel 20

                    if (!MUX_ENABLE17_32.e2flags.CH20) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH20GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH20ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH20GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH20GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH20PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH20PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH20PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 21: //Channel 21

                    if (!MUX_ENABLE17_32.e2flags.CH21) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                   
                    gageType=read_Int_FRAM(CH21GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH21ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH21GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH21GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH21PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH21PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH21PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 22: //Channel 22

                    if (!MUX_ENABLE17_32.e2flags.CH22) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH22GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH22ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH22GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH22GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH22PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH22PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH22PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                     
                    break; //break out of switch(ch)


                case 23: //Channel 23

                    if (!MUX_ENABLE17_32.e2flags.CH23) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH23GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH23ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH23GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH23GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH23PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH23PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH23PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)


                case 24: //Channel 24

                    if (!MUX_ENABLE17_32.e2flags.CH24) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH24GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH24ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH24GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH24GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH24PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH24PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH24PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                   
                    break; //break out of switch(ch)


                case 25: //Channel 25

                    if (!MUX_ENABLE17_32.e2flags.CH25) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH25GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH25ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH25GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH25GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH25PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH25PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH25PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 26: //Channel 26

                    if (!MUX_ENABLE17_32.e2flags.CH26) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH26GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH26ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH26GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH26GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH26PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH26PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH26PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 27: //Channel 27

                    if (!MUX_ENABLE17_32.e2flags.CH27) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH27GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH27ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH27GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH27GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH27PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH27PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH27PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 28: //Channel 28

                    if (!MUX_ENABLE17_32.e2flags.CH28) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH28GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH28ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH28GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH28GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH28PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH28PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH28PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 29: //Channel 29

                    if (!MUX_ENABLE17_32.e2flags.CH29) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH29GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH29ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH29GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH29GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH29PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH29PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH29PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 30: //Channel 30

                    if (!MUX_ENABLE17_32.e2flags.CH30) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH30GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH30ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH30GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH30GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH30PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH30PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH30PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 31: //Channel 31

                    if (!MUX_ENABLE17_32.e2flags.CH31) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH31GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH31ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH31GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH31GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH31PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH31PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH31PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                case 32: //Channel 32

                    if (!MUX_ENABLE17_32.e2flags.CH32) //is channel disabled?
                    {
                        MUX4_ENABLE.mflags.skip = 1; //set the skip flag
                        break; //break out of switch(ch)
                    }
                    
                    gageType=read_Int_FRAM(CH32GTaddress);                  //load the gage type from internal FRAM
                    zeroReading=read_longFRAM(CH32ZRaddress);             //load the zero reading from internal FRAM
                    gageFactor=read_longFRAM(CH32GFaddress);              //load the gage factor from internal FRAM
                    gageOffset=read_longFRAM(CH32GOaddress);              //load the gage offset from internal FRAM
                    polyCoA=read_longFRAM(CH32PolyCoAaddress);            //load the polynomial coefficient A from internal FRAM
                    polyCoB=read_longFRAM(CH32PolyCoBaddress);            //load the polynomial coefficient B from internal FRAM
                    polyCoC=read_longFRAM(CH32PolyCoCaddress);            //load the polynomial coefficient C from internal FRAM
                    
                    break; //break out of switch(ch)

                default:
                    break;

            } //end of switch(ch)
        }

        if (!store) {
            IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
            Nop();
            Nop();
            if (LC2CONTROL2.flags2.interrupt)
                return;
        }

        if (MUX4_ENABLE.mflags.skip) //skip channel
        {
            if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) //VER 6.0.7
                clockMux(10);
            if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32 |
                    MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //if 8 or 32 channel mux  VER 6.0.9) //VER 6.0.9
                clockThMux(10); //VER 6.0.6
            continue; //return to beginning of for(ch)
        }

        if (MUX4_ENABLE.mflags.mux16_4 != TH8 && MUX4_ENABLE.mflags.mux16_4 != TH32) //VW logger
        {
            if (gageType > 0 && gageType <= 5) //VW
            {
                Blink(1);
                configVWchannel(gageType); //Setup VW channel for the VW gage type
                if (!store)
                    IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
                WDTSWEnable; //Start WDT TEST LC2MUX
                VWreading = take_reading(gageType); //take VW reading (or other gage type)
                ClrWdt(); //clear the WDT TEST LC2MUX
                WDTSWDisable; //Stop WDT TEST LC2MUX
                if (!store) {
                    IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
                    Nop();
                    Nop();
                    if (LC2CONTROL2.flags2.interrupt)
                        return;
                }
                if (VWreading == 0.0)
                    VWreadingProcessed = -999999; //error message
                else
                    processReading(VWreading, ch); //apply linear or polynomial conversion

                if (!store)
                    IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
                if (MUX4_ENABLE.mflags.mux16_4 == Single |
                        MUX4_ENABLE.mflags.mux16_4 == VW4 |
                        MUX4_ENABLE.mflags.mux16_4 == VW16)
                    extThermreading = take_analog_reading(85); //take external thermistor reading

                if (store && MUX4_ENABLE.mflags.mux16_4 != Single)
                    storeChannelReading(ch); //store the reading   
                if (!store)
                    storeTempChannelReading(ch);
            }
        } else //Thermistor logger
        {
            Blink(1);
            extThermreading = take_analog_reading(85); //take external thermistor reading
            if (store)
                storeChannelReading(ch);
            else
                storeTempChannelTHReading(ch);
        }

        if (!store) {
            IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
            Nop();
            Nop();
            if (LC2CONTROL2.flags2.interrupt)
                return;
        }
    } //end of MUX loop for(ch)

    if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == VW16) //deactivate mux if multichannel  VER 6.0.7
        MUX_RESET = 0; //bring the MUX_RESET line low
    if (MUX4_ENABLE.mflags.mux16_4 == TH8 | MUX4_ENABLE.mflags.mux16_4 == TH32 |
            MUX4_ENABLE.mflags.mux16_4 == VW8 | MUX4_ENABLE.mflags.mux16_4 == VW32) //if 8 or 32 channel mux  VER 6.0.9)      //deactivate 8 or 32 channel mux  VER 6.0.9
        MUX_CLOCK = 0;


    // TAKE ANALOG READINGS:

    if (store) {
        intThermreading = take_analog_reading(86); //take internal thermistor reading
    } else {
        IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
        intThermreading = take_analog_reading(86); //take internal thermistor reading
        IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
        Nop();
        Nop();
        if (LC2CONTROL2.flags2.interrupt)
            return;
    }

    //Determine whether 3V or 12V battery is connected

    if (store) {
        mainBatreading = take_analog_reading(87); //test the 12V_SENSE input
    } else {
        IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
        mainBatreading = take_analog_reading(87);
        IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
        Nop();
        Nop();
        if (LC2CONTROL2.flags2.interrupt)
            return;
    }


    if (mainBatreading < 820) //12V_SENSE <0.5V, so not connected
    {
        DISPLAY_CONTROL.flags.PS12V = 0; //clear the 12V battery flag
        if (store) {
            mainBatreading = take_analog_reading(97); //take the 3V battery reading
        } else {
            IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
            mainBatreading = take_analog_reading(97); //take internal thermistor reading
            IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
            Nop();
            Nop();
            if (LC2CONTROL2.flags2.interrupt)
                return;
        }
        batteryVoltage = (((Vref * mainBatreading) / 4096) * mul3V); //convert to voltage    
    } else {
        DISPLAY_CONTROL.flags.PS12V = 1; //set the 12V battery flag
        mainBatreading *= -1; //convert to negative number
        batteryVoltage = (((Vref * mainBatreading) / 4096) * mul12V); //convert to voltage   
        batteryVoltage *= -1.0; //convert batteryVoltage to positive for display
    }

    if (!store) {
        storeTempReading(ch); //temporary storage for display only ('X' command)
        write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//save display flags`
        IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
        Nop();
        Nop();
        if (LC2CONTROL2.flags2.interrupt) //did INT2 interrupt occur during this period?
            return; //abort reading if it did

        _3VX_off(); //power-down the analog circuitry
        config_Ports_Low_Power();

        IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
        displayTempReading(); //Display the reading
        IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
        return;
    }

    if (((!USB_PWR | _232) && DISPLAY_CONTROL.flags.PS12V && batteryVoltage < 5.0) | //if 12V battery < 5V or 3V battery < 1.7V  
            ((!USB_PWR | _232) && !DISPLAY_CONTROL.flags.PS12V && batteryVoltage < 1.7)) {
        disableINT1(); //stop logging and shutdown
        disableAlarm(Alarm1);
        shutdown();
    }

    IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
    outputPosition = storeReading(ch); //store the reading and return the outputPosition pointer
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//save display flags    

    IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
    Nop();
    Nop();

    _3VX_off(); //power-down the analog circuitry
    config_Ports_Low_Power();

    if ((LC2CONTROL.flags.Monitor && (USB_PWR | !_232)) |
            (LC2CONTROL.flags.Monitor && (!USB_PWR | _232) && LC2CONTROL.flags.NetEnabled && netTest == 1)) 
    {
        IEC1bits.INT1IE = 0; //temporarily disable the INT2 interrupt
        LC2CONTROL2.flags2.ID = 1; //set the ID flag
        displayReading(ch, outputPosition); //display the values stored in external FRAM
        IEC1bits.INT1IE = 1; //re-enable the INT2 interrupt
    }

    DISPLAY_CONTROL.flags.TakingReading = 0; //clear the TakingReading flag
    write_Int_FRAM(DISPLAY_CONTROLflagsaddress,DISPLAY_CONTROL.display);	//store flags in FRAM 

    //VER 6.0.3:
    if ((LC2CONTROL.flags.LoggingStopTime && (TotalStopSeconds == seconds_since_midnight)) | //logging scheduled to stop?
            (memoryStatus >= maxSingleVW && outputPosition == 1 && !DISPLAY_CONTROL.flags.WrapMemory)) //stop logging when memory full?
        stopLogging(); //yes

    checkSynch(ReadingTimeSeconds); //adjust time of next scheduled reading if necessary
}

float take_reading(unsigned char gageType) //take a reading, x is gage type
{
    char analogBUF[10];
    unsigned int *ADC16Ptr;
    unsigned int count;
    unsigned int lithtemp;

    float digits = 0.0;
    float lithium = 0.0;

    _3VX_on(); //power-up analog circuitry VER 6.0.0
    Blink(1);

    if (gageType == 95) //lithium coin cell reading
    {
        ADPCFG = 0; //ALL INPUTS are analog channels
        //ADCON1=0x00E0;							//internal counter ends sampling and starts conversion
        //ADCON2=0x003C;							//interrupt after every 16 samples and INTERNAL VDD as Vref

        //ADCHS=0x0007;							//connect AN7 as CH0 input
        SAMPLE_LITHIUM = 1; //sample the lithium battery		

        //ADCSSL=0;
        //ADCON3=0x1F1F;							//Sample time = 31Tad (30uS), Tad=internal Tcy/2

        //ADCON1bits.ADON=1;						//turn ADC on

        delay(500); //500uS delay for settling	

        ADC16Ptr = &ADCBUF0; //intialize ADCBUF pointer
        //IFS0bits.ADIF=0;						//clear ADC interrupt flag
        //ADCON1bits.ASAM=1;						//auto start sampling for 31Tad then go to conversion
        //while(!IFS0bits.ADIF);					//conversion done?
        //ADCON1bits.ASAM=0;						//yes then stop sample/convert
        SAMPLE_LITHIUM = 0; //turn off lithium battery sampling if on

        for (count = 0; count < 16; count++) //totalize the values from the buffer
        {
            lithtemp = lithtemp + *ADC16Ptr++;
        }

        lithtemp /= 16; //average the result
        //ADCON1bits.ADON=0;						//turn ADC off

        lithium = ((5.0 * lithtemp) / 4096.0); //convert to voltage	

        return lithium; //return the averaged 12 bit value		
    }


    if (gageType == 1)
        pluck(1400, 3500, 384); //1400-3500 Hz Sweep

    if (gageType == 2)
        pluck(2800, 4500, 384); //2800-4500 Hz Sweep

    if (gageType == 3)
        pluck(400, 1200, 384); //400-1200 Hz Sweep

    if (gageType == 4)
        pluck(1200, 2800, 384); //1200-2800 Hz Sweep

    if (gageType == 5)
        pluck(2500, 4500, 384); //2500-4500 Hz Sweep


    delay(20000); //20mS delay

    digits = read_vw(gageType); //get the VW digits
    resetVWchannel(); //put the VW channel in standby mode
    _3VX_off(); //power-down analog circuitry   VER 6.0.0
    return digits; //give it to take_One_Complete_Reading()
}


void testPoint(unsigned char tp, unsigned char cycles)
{
    int i=0;							//delay loop index
    int c=0;							//cycle loop index

    for(c;c<cycles;c++)
    {
        switch(tp)						//bring test point high
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
	

        for(i=0;i<10;i++){}					//short delay

        switch(tp)						//bring test point low
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

        for(i=0;i<10;i++){}					//short delay
    }

    for(i=0;i<100;i++){}					//spacing between test points

}


void testPoint2(unsigned char cycles) {
    //int i=0;							//delay loop index  REM VER 6.0.10
    int c = 0; //cycle loop index

    for (c; c < cycles; c++) {
        _READ = 0; //bring testpoint low
        //for(i=0;i<10;i++){}					//short delay   REM VER 6.0.10
        delay(100); //VER 6.0.10
        _READ = 1; //bring testpoint high
        //for(i=0;i<10;i++){}					//short delay   REM VER 6.0.10
        delay(100); //VER 6.0.10
    }

}

void testReset(void) {
    configUARTnormal();
    //_232SHDN=1;
    if (RCONbits.TRAPR) {
        putsUART1(trapr); //    "A Trap Conflict Reset has occurred."
        while (BusyUART1());
        crlf();
        RCONbits.TRAPR = 0; //clear the flag if set
    }

    if (RCONbits.IOPUWR) {
        putsUART1(iopuwr); //    "An Illegal Opcode, Illegal Address or Uninitialized W has occurred."
        while (BusyUART1());
        crlf();
        RCONbits.IOPUWR = 0; //clear the flag if set
    }

    if (RCONbits.EXTR) {
        putsUART1(extr); //    "A Master Clear (pin) Reset has occurred."
        while (BusyUART1());
        crlf();
        RCONbits.EXTR = 0; //clear the flag if set
    }

    if (RCONbits.SWR) {
        putsUART1(swr); //    "A RESET Instruction has been executed."
        while (BusyUART1());
        crlf();
        RCONbits.SWR = 0; //clear the flag if set
    }

    if (RCONbits.WDTO) {
        putsUART1(wdto); //    "WDT Timeout has occurred."
        while (BusyUART1());
        crlf();
        RCONbits.WDTO = 0; //clear the flag if set
    }

    if (RCONbits.SLEEP) {
        putsUART1(sleep); //    "Device has been in Sleep mode."
        while (BusyUART1());
        crlf();
        RCONbits.SLEEP = 0; //clear the flag if set
    }

    if (RCONbits.BOR) {
        putsUART1(bor); //    "A Brown-out Reset has occurred."
        while (BusyUART1());
        crlf();
        RCONbits.BOR = 0; //clear the flag if set
    }

    if (RCONbits.POR) {
        putsUART1(por); //    "A Power-up Reset has occurred."
        while (BusyUART1());
        crlf();
        RCONbits.POR = 0; //clear the flag if set
    }
    //_232SHDN=0;
}

void toBCD(unsigned char value) {
    if (value >= 0 && value <= 9) //convert values 0-9 to 2 digit BCD
    {
        BCDtens = 0;
        BCDones = value;
        return;
    }

    if (value >= 10 && value <= 19) //convert values 10-19 to 2 digit BCD
    {
        BCDtens = 1;
        BCDones = value - 10;
        return;
    }

    if (value >= 20 && value <= 29) //convert values 20-29 to 2 digit BCD
    {
        BCDtens = 2;
        BCDones = value - 20;
        return;
    }

    if (value >= 30 && value <= 39) //convert values 30-39 to 2 digit BCD
    {
        BCDtens = 3;
        BCDones = value - 30;
        return;
    }

    if (value >= 40 && value <= 49) //convert values 40-49 to 2 digit BCD
    {
        BCDtens = 4;
        BCDones = value - 40;
        return;
    }

    if (value >= 50 && value <= 59) //convert values 50-59 to 2 digit BCD
    {
        BCDtens = 5;
        BCDones = value - 50;
        return;
    }

    if (value >= 60 && value <= 69) //convert values 60-69 to 2 digit BCD
    {
        BCDtens = 6;
        BCDones = value - 60;
        return;
    }

    if (value >= 70 && value <= 79) //convert values 70-79 to 2 digit BCD
    {
        BCDtens = 7;
        BCDones = value - 70;
        return;
    }

    if (value >= 80 && value <= 89) //convert values 80-89 to 2 digit BCD
    {
        BCDtens = 8;
        BCDones = value - 80;
        return;
    }

    if (value >= 90 && value <= 99) //convert values 90-99 to 2 digit BCD
    {
        BCDtens = 9;
        BCDones = value - 90;
        return;
    }


}

unsigned int toJulian(void) //convert mm/dd to day of year (julian) with 
{ //leap year correction
    unsigned int DayOfYear;
    unsigned int offset;


    LC2CONTROL.flags.LeapYear = 0; //clear the leap year flag

    if (year % 4 == 0) //is It a leap year?
        LC2CONTROL.flags.LeapYear = 1; //set flag if it is

    if (!LC2CONTROL.flags.LeapYear) {
        switch (month) //determine day of year offset - not a leap year
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
        } //end of switch(RTCmonths)
    } else {
        switch (month) //determine day of year offset - leap year
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
        } //end of switch(RTCmonths)
    }

    DayOfYear = day + offset;
    return DayOfYear; //exit and return day of year
}

unsigned char toMonthDay(unsigned int julian, unsigned int year, unsigned char x) //x:0=day,1=month
{

    if (year % 4 == 0) //leap year?
        LC2CONTROL.flags.LeapYear = 1; //set the leap year flag
    else
        LC2CONTROL.flags.LeapYear = 0; //if not, clear the leap year flag

    if (julian >= 1 && julian <= 31) //January
    {
        if (x)
            return 1; //return month
        else
            return julian; //return day
    }

    if (!LC2CONTROL.flags.LeapYear) //Not a Leap Year
    {
        if (julian >= 32 && julian <= 59) //February
        {
            if (x)
                return 2; //return month
            else
                return julian - 31; //return day
        }

        if (julian >= 60 && julian <= 90) //March
        {
            if (x)
                return 3; //return month
            else
                return julian - 59; //return day
        }

        if (julian >= 91 && julian <= 120) //April
        {
            if (x)
                return 4; //return month
            else
                return julian - 90; //return day
        }

        if (julian >= 121 && julian <= 151) //May
        {
            if (x)
                return 5; //return month
            else
                return julian - 120; //return day
        }

        if (julian >= 152 && julian <= 181) //June
        {
            if (x)
                return 6; //return month
            else
                return julian - 151; //return day
        }

        if (julian >= 182 && julian <= 212) //July
        {
            if (x)
                return 7; //return month
            else
                return julian - 181; //return day
        }

        if (julian >= 213 && julian <= 243) //August
        {
            if (x)
                return 8; //return month
            else
                return julian - 212; //return day
        }

        if (julian >= 244 && julian <= 273) //September
        {
            if (x)
                return 9; //return month
            else
                return julian - 243; //return day
        }

        if (julian >= 274 && julian <= 304) //October
        {
            if (x)
                return 10; //return month
            else
                return julian - 273; //return day
        }

        if (julian >= 305 && julian <= 334) //November
        {
            if (x)
                return 11; //return month
            else
                return julian - 304; //return day
        }

        if (julian >= 335 && julian <= 365) //December
        {
            if (x)
                return 12; //return month
            else
                return julian - 334; //return day
        }

    } else //Leap Year
    {
        if (julian >= 32 && julian <= 60) //February
        {
            if (x)
                return 2; //return month
            else
                return julian - 31; //return day
        }

        if (julian >= 61 && julian <= 91) //March
        {
            if (x)
                return 3; //return month
            else
                return julian - 60; //return day
        }

        if (julian >= 92 && julian <= 121) //April
        {
            if (x)
                return 4; //return month
            else
                return julian - 91; //return day
        }

        if (julian >= 122 && julian <= 152) //May
        {
            if (x)
                return 5; //return month
            else
                return julian - 121; //return day
        }

        if (julian >= 153 && julian <= 182) //June
        {
            if (x)
                return 6; //return month
            else
                return julian - 152; //return day
        }

        if (julian >= 183 && julian <= 213) //July
        {
            if (x)
                return 7; //return month
            else
                return julian - 182; //return day
        }

        if (julian >= 214 && julian <= 244) //August
        {
            if (x)
                return 8; //return month
            else
                return julian - 213; //return day
        }

        if (julian >= 245 && julian <= 274) //September
        {
            if (x)
                return 9; //return month
            else
                return julian - 244; //return day
        }

        if (julian >= 275 && julian <= 305) //October
        {
            if (x)
                return 10; //return month
            else
                return julian - 274; //return day
        }

        if (julian >= 306 && julian <= 335) //November
        {
            if (x)
                return 11; //return month
            else
                return julian - 305; //return day
        }

        if (julian >= 336 && julian <= 366) //December
        {
            if (x)
                return 12; //return month
            else
                return julian - 335; //return day
        }
    }
}

void unpack(unsigned int data) {
    Lbyte = data; //get LSB
    data &= 0xFF00; //mask LSB
    data /= 256; //right shift data 8 bits
    Hbyte = data; //get MSB
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
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  
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
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength2address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength2address);       
            break;
        }

        //********************************LOG INTERVAL 3*****************************************
        while (LogIt3 == 0 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0) //if 0, log indefinately at this Scan Interval	
        {
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength3address);    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength3address);       
            break;
        }

        while (LogItRemain3>-1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0) //in interval 3?	
        {
            LogItRemain3 -= 1;                                                  //yes, so decrement the remaining intervals
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain3address,LogItRemain3);               //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                write_Int_FRAM(SingleLogItRemain3address,LogItRemain3);          //store it to FRAM    
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength3address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength3address);       
            break;
        }

        //********************************LOG INTERVAL 4*****************************************
        while (LogIt4 == 0 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0) //if 0, log indefinitely at this Scan Interval	
        {
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength4address);    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength4address);       
            break;
        }

        while (LogItRemain4>-1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0)//in interval 4?	
        {
            LogItRemain4 -= 1;                                                  //yes, so decrement the remaining intervals
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain4address,LogItRemain4);               //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                write_Int_FRAM(SingleLogItRemain4address,LogItRemain4);          //store it to FRAM    
                    //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength4address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength4address);       
            break;
        }

        //********************************LOG INTERVAL 5*****************************************
        while (LogIt5 == 0 && LogItRemain4 == -1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0 && LogIt4 != 0) //if 0, log indefinately at this Scan Interval	
        {
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength5address);    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength5address);       
            break;
        }

        while (LogItRemain5>-1 && LogItRemain4 == -1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0 && LogIt4 != 0) //in interval 5?	
        {
            LogItRemain5 -= 1;                                                  //yes, so decrement the remaining intervals
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain5address,LogItRemain5);               //store it to FRAM`
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                write_Int_FRAM(SingleLogItRemain5address,LogItRemain5);          //store it to FRAM    
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength5address);   
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength5address);       
            break;
        }

        //********************************LOG INTERVAL 6*****************************************
        while (LogIt6 == 0 && LogItRemain5 == -1 && LogItRemain4 == -1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0 && LogIt4 != 0 && LogIt5 != 0) //if 0, log indefinately at this Scan Interval	 
        {
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                ScanInterval=read_longFRAM(LogIntLength6address);    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                ScanInterval=read_longFRAM(SingleLogIntLength6address);        
            break;
        }

        //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
        if (MUX4_ENABLE.mflags.mux16_4 != Single)                               //if multichannel   
            LogItRemain6=read_Int_FRAM(LogItRemain6address);                    //get the remaining intervals
        if (MUX4_ENABLE.mflags.mux16_4 == Single) 
            LogItRemain6=read_Int_FRAM(SingleLogItRemain6address);              //get the remaining intervals   

        while (LogItRemain6>-1 && LogItRemain5 == -1 && LogItRemain4 == -1 && LogItRemain3 == -1 && LogItRemain2 == -1 && LogItRemain1 == -1 && LogIt1 != 0 && LogIt2 != 0 && LogIt3 != 0 && LogIt4 != 0 && LogIt5 != 0)//in interval 6?	
        {
            LogItRemain6 -= 1;                                                  //yes, so decrement the remaining intervals
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
            if (MUX4_ENABLE.mflags.mux16_4 != Single)                           //if multichannel   
                write_Int_FRAM(LogItRemain6address,LogItRemain6);               //store it to FRAM    
            if (MUX4_ENABLE.mflags.mux16_4 == Single) 
                write_Int_FRAM(SingleLogItRemain6address,LogItRemain6);         //store it to FRAM    
            //if(MUX4_ENABLE.mflags.mux16_4==VW4 | MUX4_ENABLE.mflags.mux16_4==VW16 | MUX4_ENABLE.mflags.mux16_4==TH8)       //if multichannel  REM VER 6.0.9
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
            stopLogging(); //Stop Logging
            return;
        }

    } 
    else 
    { //no
        ScanInterval = hms2s();
    }

    if (!(86400 % ScanInterval) && VWflagsbits.synch && DISPLAY_CONTROL.flags.Synch && !LC2CONTROL.flags.LoggingStartTime) //if scan interval is evenly divisible into
    { //86400 and the first reading is being taken and the Synch flag is set
        NewTime = synch(CurrentTimeSeconds, ScanInterval);
    } else {
        NewTime = CurrentTimeSeconds + ScanInterval;
    }

    if (NewTime <= CurrentTimeSeconds)
        NewTime = CurrentTimeSeconds + ScanInterval; //In case synch'd time is current time or before current time

    if (LC2CONTROL2.flags2.FirstReading) //is it the first reading?
    {
        if (MUX4_ENABLE.mflags.mux16_4 == VW32 | MUX4_ENABLE.mflags.mux16_4 == VW16) //32 or 16 channel mux    
        {
            if (NewTime <= (CurrentTimeSeconds + 20))
                NewTime = NewTime + ScanInterval; //to make sure reading isn't skipped due to processing time
        }

        if (MUX4_ENABLE.mflags.mux16_4 == VW4 | MUX4_ENABLE.mflags.mux16_4 == TH8) //4 or 8 channel mux     VER 6.0.7
        {
            if (NewTime <= (CurrentTimeSeconds + 6))
                NewTime = NewTime + ScanInterval; //to make sure reading isn't skipped due to processing time
        }

        if (MUX4_ENABLE.mflags.mux16_4 == TH32 | MUX4_ENABLE.mflags.mux16_4 == VW8) //8VW or 32TH channel mux    VER 6.0.9
        {
            if (NewTime <= (CurrentTimeSeconds + 10))
                NewTime = NewTime + ScanInterval; //to make sure reading isn't skipped due to processing time
        }

        if (MUX4_ENABLE.mflags.mux16_4 == Single) //single channel     VER 6.0.7
        {
            if (NewTime == CurrentTimeSeconds)
                NewTime = CurrentTimeSeconds + ScanInterval; //In case synch'd time is current time

            if (NewTime == (CurrentTimeSeconds + 1)) //To make sure reading isn't skipped due
                NewTime = NewTime + ScanInterval; //to processing time
        }

        LC2CONTROL2.flags2.FirstReading = 0; //clear the first reading flag 
        write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);	//store flags in FRAM 
    }


    if (NewTime >= 86400) //midnight rollover?
        NewTime -= 86400; //compensate

    hms(NewTime, 1); //update the RTC Alarm1 register
}




//***************************************************************************
//							V_T2C()
//
//	Converts thermistor voltage to degrees C via 5th order polynomial
//
//	Parameters received: thermistor voltage
//	Returns: temperature in degrees C
//
//	Called from: 
//
//***************************************************************************

float V_T2C(float V_T) {
    float tempfloat;

    tempfloat =
            (C0 //do 5th order polynomial
            + (C1 * V_T) //and return result
            + (C2 * V_T * V_T)
            + (C3 * V_T * V_T * V_T)
            + (C4 * V_T * V_T * V_T * V_T)
            + (C5 * V_T * V_T * V_T * V_T * V_T));

    return tempfloat;
}

void wait(void) //VER 6.0.2
{
    unsigned char t = 0;

    //ClrWdt();
    //WDTSWEnable;                                                                //Start WDT

    while (!t) {
        RxData = ReadUART1();

        if (RxData == xon)
            t = 1;
    }
    //WDTSWDisable;                                                               //Stop WDT
}

void wait2S(void) {
    unsigned long point1S = 73728; //delay Tcy value for 0.1S
    unsigned int i = 0;

    for (i = 0; i < 20; i++) //delay 2S and blink LED
    {
        Blink(1);
        delay(point1S);
    }


}

void wrap_stop(void) //VER 6.0.12
{ //memory full
    outputPosition = 1; //reset outputPosition pointer
    if (!DISPLAY_CONTROL.flags.WrapMemory) //is logging to stop when memory is full?
        stopLogging(); //YES
}

//***************************INTERRUPT SERVICE ROUTINES***********************************************

void __attribute__((__interrupt__)) _U1RXInterrupt(void) //This is the UART1 Receive ISR VER 6.0.2
{
    IFS0bits.U1RXIF = 0; //clear the UART1 interrupt flag
    U1STAbits.OERR = 0;
    RxData = ReadUART1(); //get the char

    if (RxData == xoff) //Xoff received
    {
        wait();
    }
    if (RxData == escape) //does the user want to stop the data download?
    {
        DISPLAY_CONTROL.flags.bail = 1; //Set the bail flag
    }

}

void __attribute__((__interrupt__)) _INT1Interrupt(void)                        //This is the RTC ISR when time to read occurs  REV B
{
    //WDTSWEnable; //TEST VER 6.0.10                                            //TEST REM REV B
    IFS1bits.INT1IF = 0; //clear the interrupt flag                             //REV B
    SLEEP12V = 0;

    validRTC = debounce();

    if (validRTC) //if valid interrupt
    {
        PORT_CONTROL.control=read_Int_FRAM(CONTROL_PORTflagsaddress);          //get the PORT_CONTROL flags  
        intSource = readClock(0x0F); //get the RTC Alarm flags		
        intSource &= 0x03; //strip off bits 7-2    VER 6.0.13

        if (PORT_CONTROL.flags.Alarm2Enabled && (intSource == 0x02 | intSource == 0x03)) //Alarm 2
        {
            disableAlarm(Alarm2);

            if (!PORT_CONTROL.flags.ControlPortON | !PORT_CONTROL.flags.CPTime) //PORT was OFF or O1 was previously issued
            {
                CONTROL = 1; //Turn PORT ON
                _READ=0;                                                         //light LED TEST REV B
                PORT_CONTROL.flags.ControlPortON = 1; //set flag
                PORT_CONTROL.flags.CPTime = 1; //set flag

                PortOffHours=read_Int_FRAM(PortOffHoursaddress);               //write Port OFF hours to RTC 
                setClock(RTCAlarm2HoursAddress, PortOffHours);
                PortOffMinutes=read_Int_FRAM(PortOffMinutesaddress);           //write Port OFF minutes to RTC   
                setClock(RTCAlarm2MinutesAddress, PortOffMinutes);
            } else
                if (PORT_CONTROL.flags.ControlPortON | PORT_CONTROL.flags.O0issued | PORT_CONTROL.flags.CPTime) //PORT was ON,O0 was previously issued or CPTime in process
            {
                CONTROL = 0; //Turn PORT OFF
                _READ=1;                                                         //OFF LED TEST REV B
                PORT_CONTROL.flags.ControlPortON = 0; //clear flag
                PORT_CONTROL.flags.O0issued = 0; //clear flag
                PORT_CONTROL.flags.CPTime = 0; //clear flag
                PortOnHours=read_Int_FRAM(PortOnHoursaddress);             //write Port ON hours to RTC  
                setClock(RTCAlarm2HoursAddress, PortOnHours);
                PortOnMinutes=read_Int_FRAM(PortOnMinutesaddress);         //write Port ON minutes to RTC    
                setClock(RTCAlarm2MinutesAddress, PortOnMinutes);
            }

            write_Int_FRAM(CONTROL_PORTflagsaddress,PORT_CONTROL.control);	//store flag in FRAM  

            enableAlarm(Alarm2);
            if (!PORT_CONTROL.flags.Alarm1Enabled)
                shutdown();

        }

        if (PORT_CONTROL.flags.Alarm1Enabled && (intSource == 0x01 | intSource == 0x03)) //Alarm 1						
        {
            LC2CONTROL2.flags2.interrupt = 1; //set the INT2 interrupt flag
            MUX_CLOCK = 0; //set the MUX_CLOCK line low
            MUX_RESET = 0; //set the MUX_RESET line low
            disableAlarm(Alarm1);
            //WDTSWEnable;                                                      TEST REM VER 6.0.10
            LC2CONTROL2.flags2.Waiting = 0; //clear the Waiting flag
            write_Int_FRAM(LC2CONTROL2flagsaddress,LC2CONTROL2.full2);          //store flags in FRAM 
            T4CONbits.TON = 0;
            enableAlarm(Alarm1);
            tempTMR4 = TMR4; //save TMR4/5 registers
            tempTMR5 = TMR5HLD;
            //take_One_Complete_Reading();                                      REM VER 6.0.13
            take_One_Complete_Reading(STORE);
            TMR5HLD = tempTMR5; //restore TMR4/5 registers
            TMR4 = tempTMR4;
            if (!LC2CONTROL.flags.NetEnabled && LC2CONTROL.flags.USBpower)
                T4CONbits.TON = 1; //turn Timer4/5 back on
            //ClrWdt();                                                         //TEST REM VER 6.0.10
            //WDTSWDisable;                                                     //TEST REM VER 6.0.10
            U1STAbits.OERR = 0; //clear flag if overrun error
            IFS1bits.T5IF = 1;
        }

    }

    ClrWdt(); //TEST VER 6.0.10
    WDTSWDisable; //TEST VER 6.0.10
}

void __attribute__((__interrupt__)) _IC8Interrupt(void) //This is the VW period capture Low Word ISR
{
    IFS1bits.IC8IF = 0; //clear the interrupt flag
    ReadCapture8(&VW_period_buffer); //move capture to 16-bit working register
    timer23_value = ReadTimer23();
    CaptureFlag = 1; //set the capture flag
}


