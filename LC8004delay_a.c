

#include "LC8004delay_a.h"
#include <timer.h>


extern	const float tcy;					



//***************************************************************************
//						delay()
//
//	Delay for a specific # of processor cycles
//
//	Parameters received: unsigned long (multiples of 7.3728MHz Fcy)
//						 Tcy=1/7.3728E6=135.633680556nS
//	Returns: nothing
//
//	Called from: main()
//
//***************************************************************************
void delay(unsigned long x)
{
	while(--x);			//decrement x until 0
}



//***************************************************************************
//						shutdownTimer()
//
//	variable timer - will generate a medium priority interrupt (priority 4)
//	Timer 4/5 configured as 32 bit timer. Used to power-off the LC-2
//	if no communications activity for the decimal time in seconds included
//  as the argument. Doesn't vector - just sets the interrupt flag
//
//	ex.: tcy=135.633680556nS
//	shutdownTimer(15)								//15 seconds till shutdown
//
//	15/tcy = 110592000 (decimal) = 0x6977FFF
//
//	Value loaded to Timer 4/5 register = 0xFFFFFFFF-0x6977FFF =  0xF9688000
//	Rollover will occur @ 0xFFFFFFFF -> 0x00000000
//	0xFFFFFFFF - 0xF9688000 = 0x6977FFF = d110591999 X 135.633680556E-9(S) = 15S
//										   (Timer load)  (tcy)	  (timeout)
//	Parameters received:unsigned int timerseconds 
//	Returns: nothing, but sets T5 interrupt flag
//
//	Called from: comm()
//
//***************************************************************************
void shutdownTimer(unsigned int timerseconds)
{
	unsigned long timerload=0xFFFFFFFF;

	timerload=timerload-(timerseconds/tcy);                                     //compute value to count
	
	ConfigIntTimer5(T5_INT_PRIOR_1 & T5_INT_OFF);
	OpenTimer45(T5_ON & T4_GATE_OFF &                                           //Start Timer45
				T4_PS_1_1 & T4_32BIT_MODE_ON & 
				T4_SOURCE_INT, 0xFFFF);	
	WriteTimer45(timerload);                                                    //load Timer45
}


//***************************************************************************
//						shutdownTimerwithReset()
//
//	variable timer - will generate a medium priority interrupt (priority 4)
//	Timer 4/5 configured as 32 bit timer. Used to prevent system hang if
//	if no EEPROM communications activity for the decimal time in seconds included
//  as the arguement. Vectors to empty ISR which should force a reset
//
//	ex.: tcy=271.267361nS
//	shutdownTimer(1)								//1 second till reset
//
//	1/tcy = 3686400 (decimal) = 0x384000
//
//	Value loaded to Timer 4/5 register = 0xFFFFFFFF-0x384000 =  0xFFC7BFFF
//	Rollover will occur @ 0xFFFFFFFF -> 0x00000000
//	0xFFFFFFFF - 0xFFC7BFFF = 0x384000 = d3686400 X 271.267361E-9(S) = 1S
//										   (Timer load)  (tcy)	  (timeout)
//	Parameters received:unsigned int timerseconds 
//	Returns: nothing, but sets T5 interrupt flag and forces vector to empty ISR
//	which will reset the processor
//
//	Called from: LC2intEEPROM.c and LC2extEEPROM.c
//
//***************************************************************************
void shutdownTimerwithReset(unsigned int timerseconds)
{
	unsigned long timerload=0xFFFFFFFF;

	timerload=timerload-(timerseconds/tcy);								//compute value to count
	
//	ConfigIntTimer45(T5_INT_PRIOR_7 & T5_INT_ON);
//	OpenTimer45(T4_ON & T4_GATE_OFF & 						//Start Timer45
//				T4_PS_1_1 & T4_32BIT_MODE_ON & 
//				T4_SOURCE_INT, 0xFFFF);	
//	WriteTimer45(timerload);								//load Timer45
}


//***************************************************************************
//						StopshutdownTimer
//***************************************************************************
void StopshutdownTimer(void)
{
	//CloseTimer45();											//Stop Timer45
}


