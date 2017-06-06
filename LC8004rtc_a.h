#ifndef LC8004rtc_a_H
#define LC8004rtc_a_H

/*8004 REV B: NEED TO DEBUG THIS
typedef struct{
	unsigned	ControlPortON:1;                //0=Control Port OFF, 1=Control Port ON
	unsigned	PortTimerEN:1;                  //0=Control Port Timer Disabled, 1=Control Port Timer Enabled
	unsigned	SetAlarm2Time:1;                //0=Control Port Time on not being set, 1=Control Port Time on being set
	unsigned	SetAlarm2StopTime:1;            //0=Control Port Time off not being set,1=Control Port Time off being set
	unsigned	Alarm1Enabled:1;                //0=RTC Alarm 1 disabled,1=enabled
	unsigned	Alarm2Enabled:1;                //0=RTC Alarm 2 disabled,1=enabled
	unsigned	O1issued:1;			//0=O1 was not issued,1=O1 was issued
	unsigned	O0issued:1;			//0=O0 was not issued,1=O0 was issued
	unsigned	CPTime:1;			//0=Not in Port Timer ON time,1=In Port Timer ON time
}ControlPortBits;
typedef union{ unsigned int control;
ControlPortBits flags;
}cflags;
cflags  PORT_CONTROL;
*/


//Prototypes
void displayClock(unsigned char data);
void disableAlarm(unsigned char);			
void enableAlarm(unsigned char);			
unsigned char readClock(unsigned char address);
unsigned int RTCtoDecimal(unsigned char data);
void setClock(unsigned char address,unsigned char data);
//unsigned char Start1HzSQW(void);
void start32KHz(void);
void stop32KHz(void);
//void Stop1HzSQW(unsigned char);

#endif
