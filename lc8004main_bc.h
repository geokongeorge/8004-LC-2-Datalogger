
#ifndef LC8004main_bc_H
#define LC8004main_bc_H

// DSPIC33FJ256GP710A Configuration Bit Settings

// 'C' source line config statements

#include <p33Fxxxx.h>

 int FBS __attribute__((space(prog), address(0xF80000))) = 0xCF ;
//_FBS(
//    BWRP_WRPROTECT_OFF &                                                      // Boot Segment Write Protect (Boot Segment may be written)
//    BSS_NO_FLASH &                                                            // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
//    RBS_NO_RAM                                                                // Boot Segment RAM Protection (No Boot RAM)
//);
 int FSS __attribute__((space(prog), address(0xF80002))) = 0xCF ;
//_FSS(
//    SWRP_WRPROTECT_OFF &                                                      // Secure Segment Program Write Protect (Secure Segment may be written)
//    SSS_NO_FLASH &                                                            // Secure Segment Program Flash Code Protection (No Secure Segment)
//    RSS_NO_RAM                                                                // Secure Segment Data RAM Protection (No Secure RAM)
//);
 int FGS __attribute__((space(prog), address(0xF80004))) = 0x7 ;
//_FGS(
//    GWRP_OFF &                                                                // General Code Segment Write Protect (User program memory is not write-protected)
//    GSS_OFF                                                                   // General Segment Code Protection (User program memory is not code-protected)
//);
 //int FOSCSEL __attribute__((space(prog), address(0xF80006))) = 0x22 ;         REM REV AE
 int FOSCSEL __attribute__((space(prog), address(0xF80006))) = 0x23 ;           //REV AE
//_FOSCSEL(
//    FNOSC_PRI &                                                               // Oscillator Mode (Primary Oscillator (XT, HS, EC)) HSPLL
//    IESO_OFF                                                                  // Two-speed Oscillator Start-Up Enable (Start up with user-selected oscillator)
//);
 
 //int FOSC __attribute__((space(prog), address(0xF80008))) = 0xC6 ;            REM REV AE
 int FOSC __attribute__((space(prog), address(0xF80008))) = 0x06 ;              //REV AE
//_FOSC(
//    POSCMD_HS &                                                               // Primary Oscillator Source (HS Oscillator Mode)
//    OSCIOFNC_OFF &                                                            // OSC2 Pin Function (OSC2 pin has clock out function)
//    FCKSM_CSDCMD                                                              // Clock Switching Enabled and Fail-Safe Clock Monitor Enabled) REV AE
//);

 int FWDT __attribute__((space(prog), address(0xF8000A))) = 0x74 ;
//_FWDT(
//    WDTPOST_PS16 &                                                            // Watchdog Timer Postscaler (1:16)
//    WDTPRE_PR128 &                                                            // WDT Prescaler (1:128)
//    PLLKEN_ON &                                                               // PLL Lock Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
//    WINDIS_OFF &                                                              // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
//    FWDTEN_OFF                                                                // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)
//);
 int FPOR __attribute__((space(prog), address(0xF8000C))) = 0xE7 ;
//_FPOR(
//    FPWRT_PWR128                                                              // POR Timer Value (128ms)
//);
 int FICD __attribute__((space(prog), address(0xF8000E))) = 0xC3 ;
//_FICD(
//    ICS_PGD1 &                                                                // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
//    JTAGEN_OFF                                                                // JTAG Port Enable (JTAG is Disabled)
//);



void __attribute__((__interrupt__)) _U1RXInterrupt(void);                       //This is the UART1 Receive ISR
void __attribute__((__interrupt__)) _INT1Interrupt(void);                       //This is the RTC ISR       REV B
void __attribute__((__interrupt__)) _T5Interrupt(void);                         //This is the Timer 5 512mS interrupt
void __attribute__((__interrupt__)) _T7Interrupt(void);                         //This is the Timer 7 overflow interrupt


volatile unsigned char VWflags;                 //flags register
struct
{
	unsigned VWerror:1;                         //reading error flag
	unsigned pluckflag:1;                       //pluck computation flag
	unsigned synch:1;                           //0=reading time synchronized, 1=reading time to be synchronized
}VWflagsbits;


typedef struct{
	unsigned	memEmpty:1;			//0=FRAM Data Memory NOT Empty,1=FRAM Data Memory Empty

}DataFRAMMemoryBits;
typedef union{ unsigned int memory;
DataFRAMMemoryBits flags;
}mflags;
mflags	FRAM_MEMORY;

typedef struct{
	unsigned	monitorWasEnabled:1;            //0=monitor was not previously enabled, 1=monitor was previously enabled
	unsigned 	newPointer:1;                   //0=pointer has not been updated by user, 1=pointer has been updated by user
	unsigned 	BPD:1;                          //0=not working with pointer(B,P,D) commands, 1=working with pointer commands
	unsigned	Backup:1;                       //0=B command not yet issued, 1=B command already issued
	unsigned	Scan:1;                         //0=all readings have been read, 1=readings still to be read
	unsigned	Display:1;                      //0=first set of displayed readings after B or P command, 1=subsequent displayed readings
	unsigned	PS12V:1;                        //0=3V battery connected, 1=12V battery connected
	unsigned	Shutdown:1;                     //0=is powered up, 1=is shutdown
	unsigned	Shutup:1;                       //0=Allow message display, 1=Don't allow message display		
	unsigned 	WrapMemory:1;                   //0=memory won't wrap,1=memory will wrap (default)
	unsigned	Synch:1;                        //0=Readings will not be synchronized to top of hour,1=Readings synchronized (default)
	unsigned	TakingReading:1;                //Set when in process of taking a reading
    unsigned    bail:1;                         //0=don't bail out of data download,1=bail out of data download VER 6.0.2
    unsigned    TH:1;                           //0=VW Configuration,1=Thermistor Configuration VER 6.0.5
    unsigned    firstTime:1;                    //0=subsequent times, 1=1st time    REV W
    unsigned    BT:1;                            //0=Bluetooth Disabled, 1=Bluetooth Enabled REV AD
}DisplayControlBits;
typedef union{ unsigned int display;
DisplayControlBits flags;
}dflags;
dflags	DISPLAY_CONTROL;



typedef struct{
	unsigned Logging:1;				//0=logging stopped,1=logging started
	unsigned Monitor:1;				//0=monitor not active,1=monitor active
	unsigned LogInterval:1;                 	//0=log intervals not active,1=log intervals active
	unsigned LoggingStartTime:1;                    //0=no logging start time set,1=logging start time set
	unsigned LoggingStopTime:1;                     //0=no logging stop time set,1=logging stop time set
	unsigned ID:1;					//0=no ID entered,1=ID entered
	unsigned DateFormat:1;                          //0=julian,1=Month,Day
	unsigned LeapYear:1;            		//0=normal year,1=leap year
	unsigned ScanError:1;                   	//0=Scan interval OK, 1=Scan interval invalid character error
	unsigned USBpower:1;                            //0=USB not connected,1=USB connected and powering LC-2x4
	unsigned _3Vpower:1;                            //0=3v Battery not supplying power,1=3V battery supplying power
	unsigned ERROR:1;				//0=no error,1=error
	unsigned Conversion:1;                          //0=Linear Conversion, 1=Polynomial Conversion
	unsigned TimeFormat:1;                          //0=hhmm, 1=hh,mm
	unsigned NetEnabled:1;          		//0=Network disabled, 1=Network enabled
	unsigned Reset:1;				//0="RESET" wasn't issued, 1="RESET" was issued	
}LoggingFlagBits;
typedef union{ unsigned int full;
LoggingFlagBits flags;
}uflags;
uflags LC2CONTROL;



typedef struct{
	unsigned SetStartTime:1;                	//0=Start time not being set,1=Start time being set
	unsigned SetStopTime:1;                         //0=Stop time not being set,1=Stop time being set
	unsigned LoggingStartTimeTemp:1;        	//0=LoggingStartTime wasn't set,1=LoggingStartTime was set
	unsigned SetStartTimeTemp:1;                    //0=SetStartTime wasn't set,1=SetStartTime was set
	unsigned GageDisplay:1;                 	//used to tell toBCD() that gage display is calling
	unsigned InputSelection:2;                      //00=VW,01=EXTERNAL,10=PLUCK,11=GROUND
	unsigned ON:1;					//0=LC-2X shutdown,1=LC-2X powered
	unsigned FirstReading:1;                        //0=subsequent reading,1=first reading
	unsigned Interrupt:1;                           //0=no INT2 interrupt occurred,1=INT2 interrupt occurred
	unsigned X:1;					//0='X' command not in process,1='X' command in process
	unsigned Waiting:1;				//0=Scheduled Logging started,1=Waiting for 1st reading of scheduled logging	
	//unsigned FU:1;					//0=firmware not being updated,1=firmware being updated     REM VER 6.0.5
	unsigned R:1;					//0=R0-R1, 1=R1-R0
    //unsigned d:1;                                   //0=ASCII data download, 1=Binary data download     REM VER BA
    unsigned Modbus:1;                              //0=Command line communications, 1=MODBUS communications    VER BA
    unsigned ID:1;                                  //0=don't display ID in Binary data download, 1=display ID in Binary data download  VER 6.0.2 REM REV AA
    unsigned scheduled:1;                                                       //0=readings not scheduled, 1=readings scheduled    REV W
}LoggingFlag2Bits;
typedef union{ unsigned int full2;
LoggingFlag2Bits flags2;
}uflags2;
uflags2 LC2CONTROL2;

//MUX ENABLE FLAGS:
typedef struct{
	unsigned	mux16_4:3;			//0=4 channel mux selected  (VW4)VER 6.0.7
                                    //1=16 channel mux selected (VW16)
                                    //2=single channel selected (Single)
                                    //3=8 channel thermistor mux selected (TH8)
                                    //4=32 channel VW mux selected (VW32)
                                    //5=32 channel TH mux selected (TH32)
                                    //6=8 channel VW mux selected (VW8)
	unsigned	skip:1;				//0=channel enabled, 1=skip channel
}Mux_ControlBits;
typedef union{ unsigned int mux;
Mux_ControlBits mflags;
}ue4flags;
ue4flags	MUX4_ENABLE;



//MUX Enable flags:
typedef struct{
	unsigned	CH1:1;                                                  //0=DISABLED, 1=ENABLED
	unsigned	CH2:1;
	unsigned	CH3:1;
	unsigned	CH4:1;
	unsigned	CH5:1;
	unsigned	CH6:1;
	unsigned	CH7:1;
	unsigned	CH8:1;
	unsigned	CH9:1;
	unsigned	CH10:1;
	unsigned	CH11:1;
	unsigned	CH12:1;
	unsigned	CH13:1;
	unsigned	CH14:1;
	unsigned	CH15:1;
	unsigned	CH16:1;
}MUX_1_16EnableBits;
typedef union{ unsigned int MUXen1_16;
MUX_1_16EnableBits e1flags;
}ue1_16flags;
ue1_16flags    MUX_ENABLE1_16;

//MUX Enable flags:
typedef struct{
	unsigned	CH17:1;
	unsigned	CH18:1;
	unsigned	CH19:1;
	unsigned	CH20:1;
	unsigned	CH21:1;
	unsigned	CH22:1;
	unsigned	CH23:1;
	unsigned	CH24:1;
	unsigned	CH25:1;
	unsigned	CH26:1;
	unsigned	CH27:1;
	unsigned	CH28:1;
	unsigned	CH29:1;
	unsigned	CH30:1;
	unsigned	CH31:1;
	unsigned	CH32:1;
}MUX_17_32EnableBits;
typedef union{ unsigned int MUXen17_32;
MUX_17_32EnableBits e2flags;
}ue17_32flags;
ue17_32flags    MUX_ENABLE17_32;

/*
//REV AA:
//FLOATING POINT DISPLAY:
typedef struct{
	unsigned	CH1:1;                                                  
	unsigned	CH2:1;
	unsigned	CH3:1;
	unsigned	CH4:1;
	unsigned	CH5:1;
	unsigned	CH6:1;
	unsigned	CH7:1;
	unsigned	CH8:1;
	unsigned	CH9:1;
	unsigned	CH10:1;
	unsigned	CH11:1;
	unsigned	CH12:1;
	unsigned	CH13:1;
	unsigned	CH14:1;
	unsigned	CH15:1;
	unsigned	CH16:1;
}MUX_1_16ConversionBits;
typedef union{ unsigned long MUXconv1_16;
MUX_1_16ConversionBits c1flags;
}uc1_16flags;
uc1_16flags	MUX_CONVERSION1_16;
*/



//MUX Conversion flags:
typedef struct{
	unsigned	CH1:1;                                                  //0=Linear Conversion, 1=Polynomial Conversion
	unsigned	CH2:1;
	unsigned	CH3:1;
	unsigned	CH4:1;
	unsigned	CH5:1;
	unsigned	CH6:1;
	unsigned	CH7:1;
	unsigned	CH8:1;
	unsigned	CH9:1;
	unsigned	CH10:1;
	unsigned	CH11:1;
	unsigned	CH12:1;
	unsigned	CH13:1;
	unsigned	CH14:1;
	unsigned	CH15:1;
	unsigned	CH16:1;
}MUX_1_16ConversionBits;
typedef union{ unsigned long MUXconv1_16;
MUX_1_16ConversionBits c1flags;
}uc1_16flags;
uc1_16flags	MUX_CONVERSION1_16;


//MUX Conversion flags:
typedef struct{
	unsigned	CH17:1;
	unsigned	CH18:1;
	unsigned	CH19:1;
	unsigned	CH20:1;
	unsigned	CH21:1;
	unsigned	CH22:1;
	unsigned	CH23:1;
	unsigned	CH24:1;
	unsigned	CH25:1;
	unsigned	CH26:1;
	unsigned	CH27:1;
	unsigned	CH28:1;
	unsigned	CH29:1;
	unsigned	CH30:1;
	unsigned	CH31:1;
	unsigned	CH32:1;
}MUX_17_32ConversionBits;
typedef union{ unsigned long MUXconv17_32;
MUX_17_32ConversionBits c2flags;
}uc17_32flags;
uc17_32flags	MUX_CONVERSION17_32;

typedef struct{
	unsigned	ControlPortON:1;                                                //0=Control Port OFF, 1=Control Port ON
	unsigned	PortTimerEN:1;                                                  //0=Control Port Timer Disabled, 1=Control Port Timer Enabled
	unsigned	SetAlarm2Time:1;                                                //0=Control Port Time on not being set, 1=Control Port Time on being set
	unsigned	SetAlarm2StopTime:1;                                            //0=Control Port Time off not being set,1=Control Port Time off being set
	unsigned	Alarm1Enabled:1;                                                //0=RTC Alarm 1 disabled,1=enabled
	unsigned	Alarm2Enabled:1;                                                //0=RTC Alarm 2 disabled,1=enabled
	unsigned	O1issued:1;                                                     //0=O1 was not issued,1=O1 was issued
	unsigned	O0issued:1;                                                     //0=O0 was not issued,1=O0 was issued
	unsigned	CPTime:1;                                                       //0=Not in Port Timer ON time,1=In Port Timer ON time
    unsigned    brgh:1;                                                         //REV AE  
    unsigned    BTTime:1;                                                       //REV AG
    unsigned    BluetoothON:1;                                                  //REV AG
    unsigned    B1issued:1;                                                     //REV AG
    unsigned    B0issued:1;                                                     //REV AG
    unsigned    BluetoothTimerEN:1;                                             //REV AG
}ControlPortBits;
typedef union{ unsigned int control;
ControlPortBits flags;
}cflags;
cflags  PORT_CONTROL;

//extern cflags PORT_CONTROL;                                                   //REV B - NEED TO DEBUG THIS FURTHER

//REV J:
typedef struct{
	unsigned	sign:1;					//0=positive,1=negative
	unsigned	whole:11;				//9 bit whole # (0-2047)
	unsigned	tenths:4;				//4 bit tenths (0-9)
}DecimalTempBits;
typedef union{ unsigned int decimaltemp;
DecimalTempBits temp;
}tempflags;
tempflags	DEC_TEMP;

//REV BA
//MODBUS STATUS1 REGISTER:
typedef struct{                                                                 //      READ                              WRITE
	unsigned	Logging:1;                                                      //1=Started, 0=Stopped              1=Start, 0=Stop                  Msb
	unsigned 	Loging:1;                                                       //1=Log Ints Enabled, 0=Disabled    1=Enable, 0=Disable
	unsigned 	Wrap:1;                                                         //1=Memory Wrap Enabled, 0=Disabled 1=Enable, 0=Disable
	unsigned	BT:1;                                                           //1=Bluetooth ON, 0=OFF             1=Turn BT ON, 0=Turn OFF
	unsigned	BT_Timer:1;                                                     //1=BT Timer Enabled, 0=Disabled    1=Enable BT Timer, 0=Disable
	unsigned	CP:1;                                                           //1=Control Port ON, 0=OFF          1=Turn ON, 0=Turn OFF
	unsigned	CP_Timer:1;                                                     //1=CP Timer Enabled, 0=Disabled    1=Enable CP Timer, 0=Disable
	unsigned	Sync:1;                                                         //1=Readings Sync'd, 0=not Sync'd   1=Sync Readings,0=Don't Sync
	unsigned	CFG:3;                                                          //110   VW8 		
                                                                                //101   TH32
                                                                                //100   VW32
                                                                                //011   TH8
                                                                                //010   VW/TH Single
                                                                                //001   VW/TH16
                                                                                //000   VW/TH4
	unsigned 	ST:1;                                                           //1=Start Time Enabled,0=Disabled   1=Enable Start Time,0=Disable
    unsigned 	SP:1;                                                           //1=Stop Time Enabled,0=Disabled    1=Enable Stop Time,0=Disable
	unsigned	R:1;                                                            //                                  1=Reset memory pointers
	unsigned	X:1;                                                            //                                  1=Take single reading not stored
    unsigned    E:1;                                                            //                                  1=Sleep
}Status1ControlBits;
typedef union{ unsigned int status1;
Status1ControlBits statusflags;
}s1flags;
s1flags	S1;

//*********************************************************************

//------------------Global Variables-------------------------------------
unsigned char TxBinaryBUF[152];                                                 //REV AB
char buffer[52];
char NABUF[7];                                                                  //temporary storage for network address
char DS3231Integer=0;                                                           //REV K
char DS3231Fraction=0;                                                          //REV K
unsigned char gain=0xFF;                                                        //initial gain value (Av=500, Rf=499K)  REV F
unsigned char intSource=0;				
unsigned char temp=0;
unsigned char ModbusTestIdx=0;                                                  //REV BA
unsigned char MSB=0;                                    
unsigned char MMSB=0;                                   
unsigned char MMMSB=0;                                  
unsigned char LSB=0;                                    
char tempBUF[10];						
char testBUF[20];					//FOR TEST ONLT
unsigned char Thermtype=0;                                                      //REV J
unsigned char BCDones=0;
unsigned char BCDtens=0;
unsigned char BCDhundreds=0;
unsigned char BCDthousands=0;
unsigned char BCDtenthousands=0;
unsigned char Hbyte;
unsigned char Lbyte;
unsigned char RTCdata=0;
unsigned char RTCmonths=0;
unsigned char RTCdays=0;
unsigned char RTCyears=0;
unsigned char RTChours=0;
unsigned char RTCminutes=0;
unsigned char RTCseconds=0;
unsigned char LoggingStartDays=0;
unsigned char who=0;                                                            //Who woke the datalogger?
                                                                                //1=RTC
                                                                                //2=USB
                                                                                //3=RS-485
                                                                                //0=Error

unsigned int cap_high=0;                                                        //32 bit VW period capture working register - high word
unsigned int *VW_period_buffer;                                                 //16 bit VW period capture working register - low word
unsigned int data;
unsigned int julian;
unsigned int StopTime;
unsigned int year;                                                              //decimal values of RTC registers
unsigned int month;						
unsigned int day;
unsigned int DayOfWeek;
unsigned int hour;
unsigned int LoggingStartDay;

/* REM REV V:
const unsigned int  maxSingleVW=29015;
const unsigned int  maxFourVW=14507;
const unsigned int  maxEightVW=10445;
const unsigned int  maxSixteenVW=4835;
const unsigned int  maxThirtytwoVW=3730;
const unsigned int  maxEightTH=18652;
const unsigned int  maxThirtytwoTH=6872;
*/

//REV V:
//Max # of readings for allowable data space (0x7F794 (522132) is max array start address in data space based on 140 byte array size for VW32 configuration)
//CFG MEM START - 140 bytes = max data space start address
//0x7f820 - 0x8C = 0x7f794
//522272 - 140 = 522132
const unsigned int  maxSingleVW=29000;                                          //522132/18 = 29007
const unsigned int  maxFourVW=14500;                                            //522132/36 = 14503
const unsigned int  maxEightVW=10440;                                           //522132/50 = 10442
const unsigned int  maxSixteenVW=4830;                                          //522132/108 = 4834          
const unsigned int  maxThirtytwoVW=3720;                                        //522132/140 = 3729
const unsigned int  maxEightTH=18640;                                           //522132/28 = 18647
const unsigned int  maxThirtytwoTH=6870;                                        //522132/76 = 6870

//const unsigned char minScanSingleVW=3;                                          //TEST REM REV V
const unsigned char minScanSingleVW=2;                                          //TEST REV V
const unsigned char minScanFourVW=5;
//const unsigned char minScanEightVW=8;                                         //REM REV W
const unsigned char minScanEightVW=6;                                           //REV W
const unsigned char minScanSixteenVW=20;
const unsigned char minScanThirtytwoVW=30;
const unsigned char minScanEightTH=3;                                           
const unsigned char minScanThirtytwoTH=8;                                       //REV U  

unsigned int minute;
unsigned int second;
unsigned int Analogreading=0;
unsigned int extThermreading=0;
unsigned int intThermreading=0;
unsigned int lithBatreading=0;
unsigned int mainBatreading=0;                                                  //REV K
unsigned int tempTMR4=0;
unsigned int tempTMR5=0;
unsigned int trap=0;
unsigned int RxData;					//holds value of UART1 receive buffer
float percent=0.0;
float processedReading=0.0;
long LogIntLength=10;					
long LogIntLength1=0;					//Scan rate for Logarithmic Intervals
long LogIntLength2=0;
long LogIntLength3=0;
long LogIntLength4=0;
long LogIntLength5=0;
long LogIntLength6=0;
unsigned int LogIt1=0;					//# of iterations per interval
unsigned int LogIt2=0;
unsigned int LogIt3=0;
unsigned int LogIt4=0;
unsigned int LogIt5=0;
unsigned int LogIt6=0;
unsigned int validRTC=0;				
volatile unsigned long VWcountLSW=0;                                            //REV AF
volatile unsigned long VWcountMSW=0;                                            //REV AF
unsigned int baudrate;				
const unsigned int V0_5=137;                                                    //REV Z

unsigned int TimeOut=15;                                                        //15 second timeout
//unsigned int FUTimeOut=30;				//30 second firmware update background timer	REM VER 6.0.5

int LogItRemain1=0;                                     //remaining iterations until next interval
int LogItRemain2=0;
int LogItRemain3=0;
int LogItRemain4=0;
int LogItRemain5=0;
int LogItRemain6=0;

int tempUserPosition=0;
int CaptureFlag=0;
int gageType=0;	
int NAdata=0;						//network address value for display		
int netTest=0;			
int stopTimeTest;

//const float tcy=0.000000135633680556;                                           //7.3728MHz Fcy   REM REV AE
const float tcy=0.0000000339084201389;                                          //29.4912MHz Fcy    REV AE
//const float fudgeFactor=17.70;                                                REM REV BA
const float mS512=511.965;                                                      //REV M
const float mS256=256.0;                                                        //REV AF
//const unsigned int mS256=256;                                                   //REV AF
const float mul3V=4.55401;                                                      //VER 6.0.2
const float mul12V=9.06452;                                                     //VER 6.0.2
const float mullith3V=1.44;                                                     //REV J
const double Vref=2.5;                                                          //TEST REM REV Q  
const float bat_12LOW=8.0;                                                      //12V low battery level REV Z 
const float bat_3LOW=2.5;                                                       //3V low battery level  REV Z
const float bat_3MIN=1.7;                                                       //3V minimum battery level  REV Z
const float bat_12MIN=5.0;                                                      //12V minimum battery level REV Z

unsigned long ScanInterval;				//scan interval	
unsigned long timer23_value=0;
unsigned long TotalStartSeconds=0;
unsigned long TotalStopSeconds=0;
unsigned long seconds_since_midnight=0;
unsigned long TESTSECONDS=0;
unsigned long LoggingStopHours=0;
unsigned long LoggingStopMinutes=0;
unsigned long LoggingStopSeconds=0;

unsigned char PortOffHours=0;				
unsigned char PortOffMinutes=0;				
unsigned char PortOnHours=0;				
unsigned char PortOnMinutes=0;				

volatile unsigned long memoryStatus;
volatile unsigned long outputPosition;
volatile unsigned long userPosition;

float zeroReading=0.0;
float gageFactor=1.0;
float gageOffset=0.0;
float Lithiumreading=0.0;
float polyCoA=1.0;
float polyCoB=1.0;
float polyCoC=1.0;
float VWreading=0.0;
float VWreadingProcessed=0.0;
float gageReading=0.0;
float batteryReading=0.0;



const float C0=-55.267;					//C0-C5 coefficients for temperature conversion
const float C1=159.66;
const float C2=-240.67;
const float C3=240.12;
const float C4=-120.01;
const float C5=24.382;

//RTC Variables:                                                                //REV D
unsigned char BCDone;
unsigned char BCDten;
unsigned char BCDtwenty;
unsigned char BCDthirty;
unsigned char BCDforty;
unsigned char BCDfifty;
unsigned int decimalRTC;



//----------CONTROL/CONFIGURATION REGISTERS STORED IN EXTERNAL MEMORY------------------------------

//NOTE: Data Memory (Readings) from 0x0 to 0x7f81f                              //522.272kB data  
//      CFG Memory (configuration) from 0x7f820 to 0x7fe86                      //1601 bytes CFG
//NOTE: 0x7f820-0x7fa6b is temporary storage for 'X' command                    //includes 587 bytes for X command
#define XmemStart                   0x7f820                                     //Beginning of X storage

#define	LC2CONTROLflagsaddress		0x7fa6c                                     //Beginning of CFG memory   
#define Offsetaddress               0x7fa6e                                     //2 bytes
#define ScanHoursaddress            0x7fa70                                     //2 bytes
#define ScanMinutesaddress          0x7fa72                                     //2 bytes
#define ScanSecondsaddress          0x7fa74                                     //2 bytes
#define LoggingStopHoursaddress		0x7fa76                                     //2 bytes
#define LoggingStopMinutesaddress	0x7fa78                                     //2 bytes
#define LoggingStopSecondsaddress	0x7fa7a                                     //2 bytes
#define GageTypeaddress             0x7fa7c                                     //2 bytes
#define MemoryStatusaddress         0x7fa7e                                     //2 bytes
#define OutputPositionaddress		0x7fa80                                     //2 bytes
#define UserPositionaddress         0x7fa82                                     //2 bytes

//IDaddress = beginning of 16 character ID array 
#define IDaddress                   0x7fa84                                     //16 bytes     

#define FRAM_MEMORYflagsaddress     0x7fa94                                     //2 bytes
#define	DISPLAY_CONTROLflagsaddress	0x7fa96                                     //2 bytes
#define TotalStartSecondsaddress	0x7fa98                                     //4 bytes
#define TotalStopSecondsaddress		0x7fa9c                                     //4 bytes
#define	Dayaddress                  0x7faa0                                     //2 bytes
#define	LC2CONTROL2flagsaddress		0x7faa2                                     //2 bytes
#define	LogIntLength1address		0x7faa4                                     //4 bytes
#define LogIt1address               0x7faa8                                     //2 bytes
#define LogItRemain1address         0x7faaa                                     //2 bytes
#define LogIntLength2address		0x7faac                                     //4 bytes
#define LogIt2address               0x7fab0                                     //2 bytes
#define LogItRemain2address         0x7fab2                                     //2 bytes
#define LogIntLength3address		0x7fab4                                     //4 bytes
#define LogIt3address               0x7fab8                                     //2 bytes
#define LogItRemain3address         0x7faba                                     //2 bytes
#define LogIntLength4address		0x7fabc                                     //4 bytes
#define LogIt4address               0x7fac0                                     //2 bytes
#define LogItRemain4address         0x7fac2                                     //2 bytes
#define LogIntLength5address		0x7fac4                                     //4 bytes
#define LogIt5address               0x7fac8                                     //2 bytes
#define LogItRemain5address         0x7faca                                     //2 bytes
#define LogIntLength6address		0x7facc                                     //4 bytes
#define LogIt6address               0x7fad0                                     //2 bytes
#define LogItRemain6address         0x7fad2                                     //2 bytes
#define Netaddress                  0x7fad4                                     //2 bytes
#define LoggingStartDayaddress		0x7fad6                                     //2 bytes    
#define baudrateaddress             0x7fad8                                     //2 bytes
#define CONTROL_PORTflagsaddress	0x7fada                                     //2 bytes
#define PortOffHoursaddress         0x7fadc                                     //2 bytes
#define PortOffMinutesaddress		0x7fade                                     //2 bytes
#define PortOnHoursaddress          0x7fae0                                     //2 bytes
#define PortOnMinutesaddress		0x7fae2                                     //2 bytes
#define	MUX4_ENABLEflagsaddress		0x7fae4                                     //2 bytes
#define MUX_ENABLE1_16flagsaddress  0x7fae6                                     //2 bytes
#define MUX_ENABLE17_32flagsaddress 0x7fae8                                     //2 bytes
#define MUX_CONV1_16flagsaddress    0x7faea                                     //2 bytes
#define MUX_CONV17_32flagsaddress   0x7faec                                     //2 bytes
#define TrapRegisteraddress         0x7faee                                     //2 bytes

#define	SingleLogIntLength1address	0x7faf0                                     //4 bytes
#define SingleLogIt1address         0x7faf4                                     //2 bytes
#define SingleLogItRemain1address	0x7faf6                                     //2 bytes

#define SingleLogIntLength2address	0x7faf8                                     //4 bytes
#define SingleLogIt2address         0x7fafc                                     //2 bytes
#define SingleLogItRemain2address	0x7fafe                                     //2 bytes

#define SingleLogIntLength3address	0x7fb00                                     //4 bytes
#define SingleLogIt3address         0x7fb04                                     //2 bytes
#define SingleLogItRemain3address	0x7fb06                                     //2 bytes

#define SingleLogIntLength4address	0x7fb08                                     //4 bytes
#define SingleLogIt4address         0x7fb0c                                     //2 bytes
#define SingleLogItRemain4address	0x7fb0e                                     //2 bytes

#define SingleLogIntLength5address	0x7fb10                                     //4 bytes
#define SingleLogIt5address         0x7fb14                                     //2 bytes
#define SingleLogItRemain5address	0x7fb16                                     //2 bytes

#define SingleLogIntLength6address	0x7fb18                                     //4 bytes
#define SingleLogIt6address         0x7fb1c                                     //2 bytes
#define SingleLogItRemain6address	0x7fb1e                                     //2 bytes

#define CH1GTaddress                0x7fb20                                     //2 bytes
#define CH1ZRaddress                0x7fb22                                     //4 bytes
#define CH1GFaddress                0x7fb26                                     //4 bytes
#define CH1GOaddress                0x7fb2a                                     //4 bytes
#define CH1PolyCoAaddress           0x7fb2e                                     //4 bytes
#define CH1PolyCoBaddress           0x7fb32                                     //4 bytes
#define CH1PolyCoCaddress           0x7fb36                                     //4 bytes

#define CH2GTaddress                0x7fb3a                                     //2 bytes	
#define CH2ZRaddress                0x7fb3c                                     //4 bytes
#define CH2GFaddress                0x7fb40                                     //4 bytes
#define CH2GOaddress                0x7fb44                                     //4 bytes
#define CH2PolyCoAaddress           0x7fb48                                     //4 bytes
#define CH2PolyCoBaddress           0x7fb4c                                     //4 bytes
#define CH2PolyCoCaddress           0x7fb50                                     //4 bytes

#define CH3GTaddress                0x7fb54                                     //2 bytes	
#define CH3ZRaddress                0x7fb56                                     //4 bytes
#define CH3GFaddress                0x7fb5a                                     //4 bytes
#define CH3GOaddress                0x7fb5e                                     //4 bytes
#define CH3PolyCoAaddress           0x7fb62                                     //4 bytes
#define CH3PolyCoBaddress           0x7fb66                                     //4 bytes
#define CH3PolyCoCaddress           0x7fb6a                                     //4 bytes

#define CH4GTaddress                0x7fb6e                                     //2 bytes
#define CH4ZRaddress                0x7fb70                                     //4 bytes
#define CH4GFaddress                0x7fb74                                     //4 bytes
#define CH4GOaddress                0x7fb78                                     //4 bytes
#define CH4PolyCoAaddress           0x7fb7c                                     //4 bytes
#define CH4PolyCoBaddress           0x7fb80                                     //4 bytes
#define CH4PolyCoCaddress           0x7fb84                                     //4 bytes

#define CH5GTaddress                0x7fb88                                     //2 bytes
#define CH5ZRaddress                0x7fb8a                                     //4 bytes
#define CH5GFaddress                0x7fb8e                                     //4 bytes
#define CH5GOaddress                0x7fb92                                     //4 bytes
#define CH5PolyCoAaddress           0x7fb96                                     //4 bytes
#define CH5PolyCoBaddress           0x7fb9a                                     //4 bytes
#define CH5PolyCoCaddress           0x7fb9e                                     //4 bytes

#define CH6GTaddress                0x7fba2                                     //2 bytes
#define CH6ZRaddress                0x7fba4                                     //4 bytes
#define CH6GFaddress                0x7fba8                                     //4 bytes
#define CH6GOaddress                0x7fbac                                     //4 bytes
#define CH6PolyCoAaddress           0x7fbb0                                     //4 bytes
#define CH6PolyCoBaddress           0x7fbb4                                     //4 bytes
#define CH6PolyCoCaddress           0x7fbb8                                     //4 bytes

#define CH7GTaddress                0x7fbbc                                     //2 bytes		
#define CH7ZRaddress                0x7fbbe                                     //4 bytes
#define CH7GFaddress                0x7fbc2                                     //4 bytes
#define CH7GOaddress                0x7fbc6                                     //4 bytes
#define CH7PolyCoAaddress           0x7fbca                                     //4 bytes
#define CH7PolyCoBaddress           0x7fbce                                     //4 bytes
#define CH7PolyCoCaddress           0x7fbd2                                     //4 bytes

#define CH8GTaddress                0x7fbd6                                     //2 bytes
#define CH8ZRaddress                0x7fbd8                                     //4 bytes
#define CH8GFaddress                0x7fbdc                                     //4 bytes
#define CH8GOaddress                0x7fbe0                                     //4 bytes
#define CH8PolyCoAaddress           0x7fbe4                                     //4 bytes
#define CH8PolyCoBaddress           0x7fbe8                                     //4 bytes
#define CH8PolyCoCaddress           0x7fbec                                     //4 bytes

#define CH9GTaddress                0x7fbf0                                     //2 bytes
#define CH9ZRaddress                0x7fbf2                                     //4 bytes
#define CH9GFaddress                0x7fbf6                                     //4 bytes
#define CH9GOaddress                0x7fbfa                                     //4 bytes
#define CH9PolyCoAaddress           0x7fbfe                                     //4 bytes
#define CH9PolyCoBaddress           0x7fc02                                     //4 bytes
#define CH9PolyCoCaddress           0x7fc06                                     //4 bytes

#define CH10GTaddress               0x7fc0a                                     //2 bytes
#define CH10ZRaddress               0x7fc0c                                     //4 bytes   
#define CH10GFaddress               0x7fc10                                     //4 bytes
#define CH10GOaddress               0x7fc14                                     //4 bytes
#define CH10PolyCoAaddress          0x7fc18                                     //4 bytes
#define CH10PolyCoBaddress          0x7fc1c                                     //4 bytes
#define CH10PolyCoCaddress          0x7fc20                                     //4 bytes

#define CH11GTaddress               0x7fc24                                     //2 bytes
#define CH11ZRaddress               0x7fc26                                     //4 bytes
#define CH11GFaddress               0x7fc2a                                     //4 bytes
#define CH11GOaddress               0x7fc2e                                     //4 bytes
#define CH11PolyCoAaddress          0x7fc32                                     //4 bytes
#define CH11PolyCoBaddress          0x7fc36                                     //4 bytes
#define CH11PolyCoCaddress          0x7fc3a                                     //4 bytes

#define CH12GTaddress               0x7fc3e                                     //2 bytes
#define CH12ZRaddress               0x7fc40                                     //4 bytes
#define CH12GFaddress               0x7fc44                                     //4 bytes
#define CH12GOaddress               0x7fc48                                     //4 bytes
#define CH12PolyCoAaddress          0x7fc4c                                     //4 bytes
#define CH12PolyCoBaddress          0x7fc50                                     //4 bytes
#define CH12PolyCoCaddress          0x7fc54                                     //4 bytes

#define CH13GTaddress               0x7fc58                                     //2 bytes
#define CH13ZRaddress               0x7fc5a                                     //4 bytes
#define CH13GFaddress               0x7fc5e                                     //4 bytes
#define CH13GOaddress               0x7fc62                                     //4 bytes
#define CH13PolyCoAaddress          0x7fc66                                     //4 bytes
#define CH13PolyCoBaddress          0x7fc6a                                     //4 bytes
#define CH13PolyCoCaddress          0x7fc6e                                     //4 bytes

#define CH14GTaddress               0x7fc72                                     //2 bytes
#define CH14ZRaddress               0x7fc74                                     //4 bytes
#define CH14GFaddress               0x7fc78                                     //4 bytes
#define CH14GOaddress               0x7fc7c                                     //4 bytes
#define CH14PolyCoAaddress          0x7fc80                                     //4 bytes
#define CH14PolyCoBaddress          0x7fc84                                     //4 bytes
#define CH14PolyCoCaddress          0x7fc88                                     //4 bytes

#define CH15GTaddress               0x7fc8c                                     //2 bytes
#define CH15ZRaddress               0x7fc8e                                     //4 bytes
#define CH15GFaddress               0x7fc92                                     //4 bytes
#define CH15GOaddress               0x7fc96                                     //4 bytes
#define CH15PolyCoAaddress          0x7fc9a                                     //4 bytes
#define CH15PolyCoBaddress          0x7fc9e                                     //4 bytes
#define CH15PolyCoCaddress          0x7fca2                                     //4 bytes

#define CH16GTaddress               0x7fca6                                     //2 bytes
#define CH16ZRaddress               0x7fca8                                     //4 bytes
#define CH16GFaddress               0x7fcac                                     //4 bytes
#define CH16GOaddress               0x7fcb0                                     //4 bytes
#define CH16PolyCoAaddress          0x7fcb4                                     //4 bytes   
#define CH16PolyCoBaddress          0x7fcb8                                     //4 bytes
#define CH16PolyCoCaddress          0x7fcbc                                     //4 bytes

#define CH17GTaddress               0x7fcc0                                     //2 bytes	
#define CH17ZRaddress               0x7fcc2                                     //4 bytes
#define CH17GFaddress               0x7fcc6                                     //4 bytes
#define CH17GOaddress               0x7fcca                                     //4 bytes
#define CH17PolyCoAaddress          0x7fcce                                     //4 bytes
#define CH17PolyCoBaddress          0x7fcd2                                     //4 bytes
#define CH17PolyCoCaddress          0x7fcd6                                     //4 bytes

#define CH18GTaddress               0x7fcda                                     //2 bytes
#define CH18ZRaddress               0x7fcdc                                     //4 bytes
#define CH18GFaddress               0x7fce0                                     //4 bytes
#define CH18GOaddress               0x7fce4                                     //4 bytes
#define CH18PolyCoAaddress          0x7fce8                                     //4 bytes
#define CH18PolyCoBaddress          0x7fcec                                     //4 bytes
#define CH18PolyCoCaddress          0x7fcf0                                     //4 bytes

#define CH19GTaddress               0x7fcf4                                     //2 bytes
#define CH19ZRaddress               0x7fcf6                                     //4 bytes
#define CH19GFaddress               0x7fcfa                                     //4 bytes
#define CH19GOaddress               0x7fcfe                                     //4 bytes
#define CH19PolyCoAaddress          0x7fd02                                     //4 bytes
#define CH19PolyCoBaddress          0x7fd06                                     //4 bytes
#define CH19PolyCoCaddress          0x7fd0a                                     //4 bytes

#define CH20GTaddress               0x7fd0e                                     //2 bytes
#define CH20ZRaddress               0x7fd10                                     //4 bytes
#define CH20GFaddress               0x7fd14                                     //4 bytes
#define CH20GOaddress               0x7fd18                                     //4 bytes
#define CH20PolyCoAaddress          0x7fd1c                                     //4 bytes
#define CH20PolyCoBaddress          0x7fd20                                     //4 bytes
#define CH20PolyCoCaddress          0x7fd24                                     //4 bytes
    
#define CH21GTaddress               0x7fd28                                     //2 bytes
#define CH21ZRaddress               0x7fd2a                                     //4 bytes
#define CH21GFaddress               0x7fd2e                                     //4 bytes
#define CH21GOaddress               0x7fd32                                     //4 bytes
#define CH21PolyCoAaddress          0x7fd36                                     //4 bytes
#define CH21PolyCoBaddress          0x7fd3a                                     //4 bytes
#define CH21PolyCoCaddress          0x7fd3e                                     //4 bytes

#define CH22GTaddress               0x7fd42                                     //2 bytes
#define CH22ZRaddress               0x7fd44                                     //4 bytes
#define CH22GFaddress               0x7fd48                                     //4 bytes
#define CH22GOaddress               0x7fd4c                                     //4 bytes
#define CH22PolyCoAaddress          0x7fd50                                     //4 bytes
#define CH22PolyCoBaddress          0x7fd54                                     //4 bytes
#define CH22PolyCoCaddress          0x7fd58                                     //4 bytes

#define CH23GTaddress               0x7fd5c                                     //2 bytes
#define CH23ZRaddress               0x7fd5e                                     //4 bytes
#define CH23GFaddress               0x7fd62                                     //4 bytes
#define CH23GOaddress               0x7fd66                                     //4 bytes
#define CH23PolyCoAaddress          0x7fd6a                                     //4 bytes
#define CH23PolyCoBaddress          0x7fd6e                                     //4 bytes
#define CH23PolyCoCaddress          0x7fd72                                     //4 bytes

#define CH24GTaddress               0x7fd76                                     //2 bytes
#define CH24ZRaddress               0x7fd78                                     //4 bytes
#define CH24GFaddress               0x7fd7c                                     //4 bytes
#define CH24GOaddress               0x7fd80                                     //4 bytes
#define CH24PolyCoAaddress          0x7fd84                                     //4 bytes
#define CH24PolyCoBaddress          0x7fd88                                     //4 bytes
#define CH24PolyCoCaddress          0x7fd8c                                     //4 bytes

#define CH25GTaddress               0x7fd90                                     //2 bytes
#define CH25ZRaddress               0x7fd92                                     //4 bytes
#define CH25GFaddress               0x7fd96                                     //4 bytes
#define CH25GOaddress               0x7fd9a                                     //4 bytes
#define CH25PolyCoAaddress          0x7fd9e                                     //4 bytes
#define CH25PolyCoBaddress          0x7fda2                                     //4 bytes
#define CH25PolyCoCaddress          0x7fda6                                     //4 bytes

#define CH26GTaddress               0x7fdaa                                     //2 bytes
#define CH26ZRaddress               0x7fdac                                     //4 bytes
#define CH26GFaddress               0x7fdb0                                     //4 bytes
#define CH26GOaddress               0x7fdb4                                     //4 bytes
#define CH26PolyCoAaddress          0x7fdb8                                     //4 bytes
#define CH26PolyCoBaddress          0x7fdbc                                     //4 bytes
#define CH26PolyCoCaddress          0x7fdc0                                     //4 bytes

#define CH27GTaddress               0x7fdc4                                     //2 bytes
#define CH27ZRaddress               0x7fdc6                                     //4 bytes
#define CH27GFaddress               0x7fdca                                     //4 bytes
#define CH27GOaddress               0x7fdce                                     //4 bytes
#define CH27PolyCoAaddress          0x7fdd2                                     //4 bytes
#define CH27PolyCoBaddress          0x7fdd6                                     //4 bytes    
#define CH27PolyCoCaddress          0x7fdda                                     //4 bytes    

#define CH28GTaddress               0x7fdde                                     //2 bytes
#define CH28ZRaddress               0x7fde0                                     //4 bytes
#define CH28GFaddress               0x7fde4                                     //4 bytes
#define CH28GOaddress               0x7fde8                                     //4 bytes    
#define CH28PolyCoAaddress          0x7fdec                                     //4 bytes
#define CH28PolyCoBaddress          0x7fdf0                                     //4 bytes
#define CH28PolyCoCaddress          0x7fdf4                                     //4 bytes

#define CH29GTaddress               0x7fdf8                                     //2 bytes
#define CH29ZRaddress               0x7fdfa                                     //4 bytes
#define CH29GFaddress               0x7fdfe                                     //4 bytes
#define CH29GOaddress               0x7fe02                                     //4 bytes
#define CH29PolyCoAaddress          0x7fe06                                     //4 bytes
#define CH29PolyCoBaddress          0x7fe0a                                     //4 bytes
#define CH29PolyCoCaddress          0x7fe0e                                     //4 bytes

#define CH30GTaddress               0x7fe12                                     //2 bytes
#define CH30ZRaddress               0x7fe14                                     //4 bytes
#define CH30GFaddress               0x7fe18                                     //4 bytes
#define CH30GOaddress               0x7fe1c                                     //4 bytes
#define CH30PolyCoAaddress          0x7fe20                                     //4 bytes
#define CH30PolyCoBaddress          0x7fe24                                     //4 bytes
#define CH30PolyCoCaddress          0x7fe28                                     //4 bytes

#define CH31GTaddress               0x7fe2c                                     //2 bytes
#define CH31ZRaddress               0x7fe2e                                     //4 bytes
#define CH31GFaddress               0x7fe32                                     //4 bytes
#define CH31GOaddress               0x7fe36                                     //4 bytes
#define CH31PolyCoAaddress          0x7fe3a                                     //4 bytes    
#define CH31PolyCoBaddress          0x7fe3e                                     //4 bytes
#define CH31PolyCoCaddress          0x7fe42                                     //4 bytes        

#define CH32GTaddress               0x7fe46                                     //2 bytes
#define CH32ZRaddress               0x7fe48                                     //4 bytes
#define CH32GFaddress               0x7fe4c                                     //4 bytes
#define CH32GOaddress               0x7fe50                                     //4 bytes    
#define CH32PolyCoAaddress          0x7fe54                                     //4 bytes
#define CH32PolyCoBaddress          0x7fe58                                     //4 bytes
#define CH32PolyCoCaddress          0x7fe5c                                     //4 bytes


//REV T:
#define CH1THaddress                0x7fe60                                     //CH1 thermistor type
#define CH2THaddress                0x7fe62                                     //CH2 thermistor type
#define CH3THaddress                0x7fe64                                     //CH3 thermistor type
#define CH4THaddress                0x7fe66                                     //CH4 thermistor type
#define CH5THaddress                0x7fe68                                     //CH5 thermistor type
#define CH6THaddress                0x7fe6a                                     //CH6 thermistor type
#define CH7THaddress                0x7fe6c                                     //CH7 thermistor type
#define CH8THaddress                0x7fe6e                                     //CH8 thermistor type
#define CH9THaddress                0x7fe70                                     //CH9 thermistor type
#define CH10THaddress               0x7fe72                                     //CH10 thermistor type
#define CH11THaddress               0x7fe74                                     //CH11 thermistor type
#define CH12THaddress               0x7fe76                                     //CH12 thermistor type
#define CH13THaddress               0x7fe78                                     //CH13 thermistor type
#define CH14THaddress               0x7fe7a                                     //CH14 thermistor type
#define CH15THaddress               0x7fe7c                                     //CH15 thermistor type
#define CH16THaddress               0x7fe7e                                     //CH16 thermistor type
#define CH17THaddress               0x7fe80                                     //CH17 thermistor type
#define CH18THaddress               0x7fe82                                     //CH18 thermistor type
#define CH19THaddress               0x7fe84                                     //CH19 thermistor type
#define CH20THaddress               0x7fe86                                     //CH20 thermistor type
#define CH21THaddress               0x7fe88                                     //CH21 thermistor type
#define CH22THaddress               0x7fe8a                                     //CH22 thermistor type
#define CH23THaddress               0x7fe8c                                     //CH23 thermistor type
#define CH24THaddress               0x7fe8e                                     //CH24 thermistor type
#define CH25THaddress               0x7fe90                                     //CH25 thermistor type
#define CH26THaddress               0x7fe92                                     //CH26 thermistor type
#define CH27THaddress               0x7fe94                                     //CH27 thermistor type
#define CH28THaddress               0x7fe96                                     //CH28 thermistor type
#define CH29THaddress               0x7fe98                                     //CH29 thermistor type
#define CH30THaddress               0x7fe9a                                     //CH30 thermistor type
#define CH31THaddress               0x7fe9c                                     //CH31 thermistor type
#define CH32THaddress               0x7fe9e                                     //CH32 thermistor type
#define startHoursaddress           0x7fea0                                     //start time hours  REV W
#define startMinutesaddress         0x7fea2                                     //start time minutes    REV W
#define startSecondsaddress         0x7fea4                                     //start time seconds    REV W
#define scratchaddress              0x7fea6                                     //REV AA
#define MODBUSaddress               0x7fea8                                     //REV BC
                                                                                


//----------------------------------------------------------------------------
//		         DS3231 Real-Time Clock Addressing 
//   
//----------------------------------------------------------------------------

#define	RTCSecondsAddress       0x00
#define RTCMinutesAddress       0x01
#define RTCHoursAddress         0x02
#define RTCDayOfWeekAddress     0x03
#define	RTCDaysAddress          0x04
#define	RTCMonthsAddress        0x05
#define	RTCYearsAddress         0x06
#define RTCAlarm1SecondsAddress 0x07
#define RTCAlarm1MinutesAddress	0x08
#define RTCAlarm1HoursAddress	0x09
#define RTCAlarm1DaysAddress	0x0A
#define RTCAlarm2MinutesAddress	0x0B	
#define	RTCAlarm2HoursAddress	0x0C	
#define RTCControlAddress		0x0E
#define RTCStatusAddress		0x0F

//PortA Defines
#define _CLK_INT                PORTAbits.RA12		//RTC Interrupt - falling edge	(INPUT)

//PortB Defines
#define VW                      PORTBbits.RB0		//VW analog input AN0   (INPUT)
#define V3_SENSE                PORTBbits.RB2       //3V analog input AN2   (INPUT)
#define V12_SENSE               PORTBbits.RB3       //12V analog input AN3  (INPUT)
#define V_TH_EXT                PORTBbits.RB4       //Ext Therm analog input AN4    (INPUT)
#define V_LITH                  PORTBbits.RB5       //Lithium cell analog input AN5 (INPUT)
#define VW_LPF                  PORTBbits.RB8       //VW_LPF analog input AN8   (INPUT))

//PortC Defines
#define VW100                   PORTCbits.RC1       //VW100 T2CK counter input  (INPUT)
#define N2G                     LATCbits.LATC2       //H-Bridge drive            (OUTPUT) REV L
#define _P1G                    LATCbits.LATC3       //H-Bridge drive            (OUTPUT) REV L
#define _P2G                    LATCbits.LATC4       //H-Bridge drive            (OUTPUT) REV L

//PortD Defines
#define	_CLK_RST                LATDbits.LATD1		//Reset the DS3231 RTC	(OUTPUT)
                                                    //1=Normal operation(default), 0=Reset
#define V3_X_CONTROL            LATDbits.LATD2		//Analog power circuitry power control 	(OUTPUT)
                                                    //1=Analog power ON, 0=Analog power OFF(default)
#define SAMPLE_LITHIUM          LATDbits.LATD3		//1=Sampling lithium battery, 0=disconnected(default)	(OUTPUT)
#define BT_BAUD                 LATDbits.LATD4      //1=Force 9600bps, 0=115.2kbps or SW setting(default)   (OUTPUT)
#define BT_CONNECT              PORTDbits.RD5       //1=BT connected, 0=BT disconnected (INPUT)
#define BT_ENABLE               LATDbits.LATD6      //1=Enable BT, 0=Disable BT(default) 
#define SLEEP12V                LATDbits.LATD7		//1=12V/3V converter off, 0=12V/3V converter on(default)	(OUTPUT)	
#define TEST2                   LATDbits.LATD8      //TEST POINT 2 (OUTPUT)
#define TEST1                   LATDbits.LATD9      //TEST POINT 1 (OUTPUT)
#define _BT_FR                  LATDbits.LATD12     //BT factory reset
                                                    //1=Reset, 0=Normal operation(default)    (OUTPUT)
#define _BT_RESET               LATDbits.LATD13     //BT reset
                                                    //1=Normal operation, 0=Reset(default)  (OUTPUT)
//PortE Defines
#define _PAUSE                  PORTEbits.RE4       //_PAUSE control                        (INPUT)
#define _EXC_EN                 LATEbits.LATE5      //Excitation enable                     (OUTPUT)
                                                    //1=disabled, 0=enabled(default)        (OUTPUT)
#define V12_EXC                 LATEbits.LATE6      //12V Excitation                        (OUTPUT)
                                                    //1=enabled, 0=disabled(default)        

//PortF Defines
#define	CONTROL                 LATFbits.LATF0		//Control port for external circuit	(OUTPUT)
                                                    //1=active, 0=inactive(default)
#define _READ                   LATFbits.LATF1      //LED control                       (OUTPUT)
                                                    //1=not lit(default), 0=lit
#define _RS485RX_EN             LATFbits.LATF4      //RS485 receive enable              (OUTPUT)
                                                    //1=TX enabled(default), 0=RX enabled
#define _232SHDN                LATFbits.LATF5      //RS-232 enable                     (OUTPUT)
                                                    //1=RS-232 enabled, 0=RS-232 shutdown(default)
#define WP                      LATFbits.LATF6		//0=EEPROM Enabled, 1=EEPROM Write Protected(default)	(OUTPUT)
#define _232                    PORTFbits.RF7       //0=RS-232 connected,1=RS-232 not connected     (INPUT) 
#define	USB_PWR                 PORTFbits.RF8		//1= USB powered, 0= USB not powered	(INPUT)
#define	MUX_CLOCK               LATFbits.LATF12		//Multiplexer Clock (default 0)		(OUTPUT)
#define	MUX_RESET               LATFbits.LATF13		//Multiplexer Reset	(default 0) 	(OUTPUT)

//PortG Defines
#define N1G                     LATGbits.LATG6      //H-Bridge drive                        (OUTPUT)
#define A                       LATGbits.LATG12     //PLL Fco Select control                (OUTPUT)
#define B                       LATGbits.LATG13     //PLL Fco Select control                (OUTPUT)
#define C                       LATGbits.LATG14     //PLL Fco Select control                (OUTPUT)
#define D                       LATGbits.LATG15     //PLL Fco Select control                (OUTPUT)

//----------------------------------------------------------------------------
//              FCY
//----------------------------------------------------------------------------
//#define FCY 7372800UL                                                         //REM REV AE
#define FCY 29491200UL                                                          //Fcy = 29.4912MHz  REV AE


//----------------------------------------------------------------------------
//              Timer 4/5 values (256mS GATE)
//----------------------------------------------------------------------------
#define mS256MSW    0x001c                                                      //REV AF
#define mS256LSW    0xcccd                                                      //REV AF  
//----------------------------------------------------------------------------
//				Baud Rate 
//
//----------------------------------------------------------------------------
//#define	brg9600                 47                                          //BRG value for 9600 bps        //REM REV AE
//#define	brg115200               3                                           //BRG value for 115200 bps      //REM REV AE
#define	brg9600                 191                                             //BRG value for 9600 bps            //REV AE
#define	brg115200               63                                              //BRG value for 115200 bps          //REV AE
#define	brg230400               31                                              //BRG value for 230400 bps          //REV AE
#define brg460800               15                                              //BRG value for 460800 bps          //REV AE

//------------------------------------------------------------------------------
//                              Configurations
//------------------------------------------------------------------------------
#define VW4                     0                                               
#define VW16                    1
#define Single                  2
#define TH8                     3
#define VW32                    4
#define TH32                    5
#define VW8                     6


//------------------------------------------------------------------------------    
//                      External FRAM base addresses
//------------------------------------------------------------------------------
//
//sector size (bytes)
#define SingleVWBytes           18                                              //29015 readings max
#define VW4Bytes                36                                              //14507 readings max
#define VW8Bytes                50                                              //10445 readings max
#define VW16Bytes               108                                             //4835 readings max
#define VW32Bytes               140                                             //3730 readings max
#define TH8Bytes                28                                              //18652 readings max
#define TH32Bytes               76                                              //6872 readings max

//address calculation:
#define SingleVWPosition        SingleVWBytes*(outputPosition-1)                //address calculation
#define VW4Position             VW4Bytes*(outputPosition-1)
#define VW8Position             VW8Bytes*(outputPosition-1)
#define VW16Position            VW16Bytes*(outputPosition-1)
#define VW32Position            VW32Bytes*(outputPosition-1)
#define TH8Position             TH8Bytes*(outputPosition-1)
#define TH32Position            TH32Bytes*(outputPosition-1)

//********************************************************************************************************




//-------------------------------
//         ASCII Table
//-------------------------------

#define nul             0x00			//ASCII NUL
#define stx             0x02            //ASCII Start Of Text                   //REV AA
#define etx             0x03            //ASCII End Of Text                     //REV AA
#define eot             0x04            //ASCII End Of Transmission             //REV AA
#define	backspace		0x08;			//ASCII Backspace
#define tab             0x09			//ASCII Printer Tab
#define lf              0x0A			//ASCII Line Feed
#define page			0x0C			//ASCII Page Feed
#define cr              0x0D			//ASCII Carriage Return
#define xon             0x11			//ASCII transmit on code(control Q)
#define xoff			0x13			//ASCII transmit off code(control S)
#define escape			0x1B			//ASCII Escape code
#define space			0x20			//ASCII " " - space
#define pound			0x23			//ASCII "#" - pound
#define	pcent			0x25			//ASCII "%" - percent
#define asterisk		0x2A			//ASCII "*" - asterisk
#define plus 			0x2B			//ASCII "+" - plus sign
#define comma			0x2C			//ASCII "," - comma
#define minus			0x2D			//ASCII "-" - minus sign
#define decimal			0x2E			//ASCII "." - decimal point
#define slash			0x2F			//ASCII "/" - right slash
#define ascii			0x30			//ASCII shift code

#define zero			0x30			//ASCII "0"
#define one             0x31			//ASCII "1"
#define two             0x32			//ASCII "2"
#define three			0x33			//ASCII "3"
#define four			0x34			//ASCII "4"
#define five			0x35			//ASCII "5"
#define six             0x36			//ASCII "6"
#define seven			0x37			//ASCII "7"
#define eight			0x38			//ASCII "8"
#define nine			0x39			//ASCII "9"

#define colon			0x3A			//ASCII ":" - colon
#define semicolon		0x3B			//ASCII ";" - semicolon
#define lessthan		0x3C			//ASCII "<" - less than
#define equal			0x3D			//ASCII "=" - equal
#define greaterthan		0x3E			//ASCII ">" - greater than
#define question 		0x3F			//ASCII "?" - question mark

#define capA			0x41			//ASCII "A" - capital A
#define capB			0x42			//ASCII "B" - capital B
#define capC			0x43			//ASCII "C" - capital C
#define capD			0x44			//ASCII "D" - capital D
#define capE			0x45			//ASCII "E" - capital E
#define capF			0x46			//ASCII "F" - capital F
#define capG			0x47			//ASCII "G" - capital G
#define capH			0x48			//ASCII "H" - capital H
#define capI			0x49			//ASCII "I" - capital I
#define capJ			0x4A			//ASCII "J" - capital J
#define capK			0x4B			//ASCII "K" - capital K
#define capL			0x4C			//ASCII "L" - capital L
#define capM			0x4D			//ASCII "M" - capital M
#define capN			0x4E			//ASCII "N" - capital N
#define capO			0x4F			//ASCII "O" - capital O
#define capP			0x50			//ASCII "P" - capital P
#define capQ			0x51			//ASCII "Q" - capital Q
#define capR			0x52			//ASCII "R" - capital R
#define capS			0x53			//ASCII "S" - capital S
#define capT			0x54			//ASCII "T" - capital T
#define capU			0x55			//ASCII "U" - capital U
#define capV			0x56			//ASCII "V" - capital V
#define capW			0x57			//ASCII "W" - capital W
#define capX			0x58			//ASCII "X" - capital X
#define capY			0x59			//ASCII "Y" - capital Y
#define capZ			0x5A            //ASCII "Z" - capital Z
#define tilde			0x7E			//ASCII "~" - tilde (last ASCII char)

// Test Points:
#define MUXRST			0x01			//MUX_RESET Test Point
#define MUXCLK			0x02			//MUX_CLOCK Test Point

// RTC Alarm							
#define Alarm1			0x01			//RTC Alarm1 interrupt enable bit (A1IE)
#define Alarm2			0x02			//RTC Alarm2 interrupt enable bit (A2IE)

// Reading Storage                                      
#define STORE           1
#define NOSTORE         0

//----------------------------------------------------------------------------
//		         String Section 
//   
//----------------------------------------------------------------------------

char AllChannelsLoaded[]={"Restored to factory default settings."};             //VER 6.0.0

char Blankarray[]={"Blank array."};

char BT[]={"BT                     Bluetooth sTatus."};                         //REV AD
char BT0[]={"BT0                    Disable Bluetooth."};                       //REV AD
char BT1[]={"BT1                    Enable Bluetooth."};                        //REV AD
char BTD[]={"BTD                    Bluetooth Timer Disabled"};                 //REV AG
char BTE[]={"BTE                    Bluetooth Timer Enabled"};                  //REV AG
char BTDisabled[]={"Bluetooth Disabled."};                                      //REV AD
char BTEnabled[]={"Bluetooth Enabled."};                                        //REV AD
char BTDisablenotAllowed[]={"Disabling Bluetooth not allowed while connected."};//REV AD
char BTTIMEREN[]={"Bluetooth Timer Enabled"};                                   //REV AG
char BTTIMERDIS[]={"Bluetooth Timer Disabled"};                                 //REV AG

char c[]={"		       c = Conversion Type(L/P)"};
char Calclock[]={"Calibrate clock circuit now."};
char Changes[]={"Changes not allowed while networked."};		
char CH1[]={"CH 1: "};
char CH2[]={"CH 2: "};
char CH3[]={"CH 3: "};
char CH4[]={"CH 4: "};
char CH5[]={"CH 5: "};
char CH6[]={"CH 6: "};
char CH7[]={"CH 7: "};
char CH8[]={"CH 8: "};
char CH9[]={"CH 9: "};
char CH10[]={"CH 10: "};
char CH11[]={"CH 11: "};
char CH12[]={"CH 12: "};
char CH13[]={"CH 13: "};
char CH14[]={"CH 14: "};
char CH15[]={"CH 15: "};
char CH16[]={"CH 16: "};
char CH17[]={"CH 17: "};
char CH18[]={"CH 18: "};
char CH19[]={"CH 19: "};
char CH20[]={"CH 20: "};
char CH21[]={"CH 21: "};
char CH22[]={"CH 22: "};
char CH23[]={"CH 23: "};
char CH24[]={"CH 24: "};
char CH25[]={"CH 25: "};
char CH26[]={"CH 26: "};
char CH27[]={"CH 27: "};
char CH28[]={"CH 28: "};
char CH29[]={"CH 29: "};
char CH30[]={"CH 30: "};
char CH31[]={"CH 31: "};
char CH32[]={"CH 32: "};
char Clockset[]={"CSmm/dd/yy/hh:mm:ss    Clock Set"};
char CnotAllowed[]={"Configuration Change Not Allowed While Logging"};	
char CommandDescription[]={"Command                Description"};
char ConfigureChannel[]={"Configure Channel# "};
char CPOFF[]={"Control Port OFF."};											
char CPON[]={"Control Port ON."};											
//char CPTIMERACT[]={"Control Port Timer activate at "};						REM REV AG
//char CPTIMERDEACT[]={"Control Port Timer deactivate at "};                    REM REV AG
char CPTIMERACT[]={"Timer activate at "};                                       //REV AG
char CPTIMERDEACT[]={"Timer deactivate at "};                                   //REV AG
char CPTIMEREN[]={"Control Port Timer Enabled."};							
char CPTIMERDIS[]={"Control Port Timer Disabled."};							

char Dashes[]={"------------------------------------------------"};
char DataloggerID[]={"Datalogger ID:"};
char Date[]={"Date: "};
char Datejulian[]={"Date format is julian."};
char Datemonthday[]={"Date format is month,day."};
char Default[]={"DEFAULT                Load factory DEFAULT settings"};
char DefaultsNotLoaded[]={"Settings not restored to factory defaults."};        //VER 6.0.0
char DF[]={"DF                     Date Format(0=julian,1=month,day)"};
char Disabled[]={"DISABLED"};
char DisabledDisplay[]={"---"};
char Displaynext[]={"N                      Display Next time to read"};
//char Displaynnnn[]={"Dnnnnn                 Display nnnnn arrays (ascii format) from pointer"};   REM REV BC
char Displaynnnn[]={"Dnnnnn                 Display nnnnn arrays from pointer"};    //REV BC
//char DisplayXnnnn[]={"DXnnnnn                Display nnnnn arrays (binary format) from pointer"}; REM REV BC
char DL[]={"DL		       Datalogger Type"};					
char DnotAllowed[]={"DEFAULT Not Allowed While Logging"};
//char DX[]={"DX                     Current Download Data Format."};             //REV AF  REM REV BC
//char Dx0[]={"DX0                    ASCII Data Download."};                     //REV AF  REM REV BC
//char Dx1[]={"DX1                    Binary Data Download."};                    //REV AF  REM REV BC
//char DX0[]={"ASCII Data Download."};                                            //VER 6.0.2   REM REV BC
//char DX1[]={"Binary Data Download."};                                           //VER 6.0.2   REM REV BC

char Enabled[]={"ENABLED"};
char End[]={"E                      End communications and go to sleep"};
char Error[]={"ERROR"};
char Errors[]={"Errors detected in "};
char Escape[]={"(Press <Esc> to exit):"};
char ExtInOff[]={"External Input OFF.                                                    "};
char Exttemp[]={"External temperature."};

//char FirmwareNotUpdated[]={"Firmware not updated."};				REM VER 6.0.5
//char FUWhileLogging[]={"Firmware Update Not Allowed While Logging"};		REM VER 6.0.5

char forLinear[]={"		       For Linear (L) Conversion:"};
char forPoly[]={"		       For Polynomial (P) Conversion:"}; 

char GageinfoWhere[]={"Gage information, where;"};
char GageinformationA[]={"Gnn/c/tt/saaaaaa/sbbbbbb/scccccc"};
char GageinformationZ[]={"Gnn/c/tt/szzzzzz/sffffff/soooooo"};
char GF[]={"   GF: "};
char GO[]={"   GO: "};
char GT[]={"GT: "};

char Hello[]={"Hello. Press ? for Help."};
char Hightemp0[]={"Standard thermistor selected."};                             //REV AF
char Hightemp1[]={"BR55KA822J thermistor selected."};                           //REV J
char Hightemp2[]={"103JL1A thermistor selected."};                              //REV J

char ID[]={"IDdddddddddddddddd     view current ID,set to dddddddddddddddd"};
char Interval[]={"Interval #"};
char Inttemp[]={"Internal temperature."};
char Iterations[]={"Iterations: "};
char iterations[]={"                       iii = iterations of interval"};

char LCtwo[]={"LC-2"};          //VER 6.0.0
char LCtwobyfour[]={"LC-2x4"};
char LCtwobysixteen[]={"LC-2x16"};
char LCtwobyeightVW[]={"LC-2x8VW"};                                             //VER 6.0.13
char LCtwobyeightTH[]={"LC-2x8TH"};                                             //VER 6.0.13
char LCtwobythirtytwoVW[]={"LC-2x32VW"};                                        //VER 6.0.13
char LCtwobythirtytwoTH[]={"LC-2x32TH"};                                        //VER 6.0.13
char Length[]={"Length: "};
char length[]={"                       lllll = length"};
char Lithiumcell[]={"Lithium Cell."};
char LithiumVoltage[]={"Lithium Cell Voltage = "};
char Log[]={"Ln/lllll/iii	       view Log intervals/change n interval"};
char Logdisabled[]={"Log intervals disabled."};
char Logdisenable[]={"LD,LE                  Log intervals Disable, Enable"};
char Logenabled[]={"Log intervals enabled."};
char Loggingalreadystarted[]={"Logging already started!"};
char Loggingalreadystopped[]={"Logging already stopped!"};
char Loggingstarted[]={"Logging started."};
char Loggingstopped[]={"Logging stopped."};
char Loggingwillstart[]={"Logging will start at: "};
char Loggingwillstop[]={"Logging will stop at: "};
char Loggingwillstopmemfull[]={"Logging will stop when memory is full"};
char Loggingwillnotstopmemfull[]={"Logging will not stop when memory is full"};
char Logintalreadydisabled[]={"Log intervals already disabled!"};
char Logintalreadyenabled[]={"Log intervals already enabled!"};
char Loglist[]={"              Log Intervals List"};
char LogIntStopTime[]={"Log Intervals Not Allowed When Using Preset Stop Logging Time."};	
char LogIntWhileLogging[]={"Enabling Log Intervals Not Allowed While Logging."};			

char Modbusaddress[]={"MAddd		       MODBUS Address (1-247)"};            //REV BC
char ModbusaddressIS[]={"MODBUS Address: "};                                    //REV BC
char Mainbattery[]={"Main Battery."};
char MEMcleared[]={"Memory cleared."};
char MEMnotcleared[]={"Memory not cleared."};
char MEMLOC[]={"MEMORY LOCATION: "};
char MinInterror[]={"ERROR: Minimum Log Interval Length for this configuration is "};
char Minscanerror[]={"ERROR: Minimum Scan Interval for this configuration is "};
char Monitoralreadydisabled[]={"Monitor mode already disabled!"};
char Monitoralreadyenabled[]={"Monitor mode already enabled!"};
char Monitordisabled[]={"Monitor mode disabled."};
char Monitorenabled[]={"Monitor mode enabled."};
char Monitorstatus[]={"M,MD,ME                Monitor status, Disable, Enable"};
char MS[]={"MS:"};
char MX[]={"MXS		       Display Multiplexer Setup"}; 
char MXselect[]={"MX#		       Select Multiplexer Configuration(1,4,8,8T,16,32,32T)"};  //VER 6.0.12
char MUX1setupmenu[]={"LC-2 Single Channel VW Setup:"};                         //VER 6.0.0
char MUX4setupmenu[]={"LC-2MUX 4-Channel VW Multiplexer Setup:"};
char MUX8setupmenu[]={"LC-2MUX 8-Channel VW Multiplexer Setup:"};               //VER 6.0.12
char MUX8Tsetupmenu[]={"LC-2MUX 8-Channel Thermistor Multiplexer Setup:"};      //VER 6.0.13
char MUX16setupmenu[]={"LC-2MUX 16-Channel VW Multiplexer Setup:"};
char MUX32setupmenu[]={"LC-2MUX 32-Channel VW Multiplexer Setup:"};             //VER 6.0.12
char MUX32Tsetupmenu[]={"LC-2MUX 32-Channel Thermistor Multiplexer Setup:"};    //VER 6.0.13
char MUX1[]={"Single Channel VW Datalogger Selected."};
char MUX4[]={"4 Channel VW Multiplexer Selected."};
char MUX8[]={"8 Channel Thermistor Multiplexer Selected."};                     //VER 6.0.6
char MUX8VW[]={"8 Channel VW Multiplexer Selected."};                           //VER 6.0.11
char MUX16[]={"16 Channel VW Multiplexer Selected."};
char MUX32[]={"32 Channel Thermistor Multiplexer Selected."};                   //VER 6.0.9
char MUX32VW[]={"32 Channel VW Multiplexer Selected."};                         //VER 6.0.11

char NetworkaddressIS[]={"Network address: "};
char Networkstatus_enable_disable[]={"NS,ND,NE	       Network Status, Disable, Enable"}; 
char Networkaddress[]={"NAddd		       Network Address (1-256)"};
char Networkdisabled[]={"Network recognition disabled."};
char Networkenabled[]={"Network recognition enabled."};
char Nexttime2read[]={"Next time to read: "};
char nn[]={"                       nn = Channel #"};
char Noarrays2display[]={"There are no arrays to display."};

char O[]={"O                      pOrt status"};					
char O0[]={"O0                     pOrt OFF (0)"};					
char O1[]={"O1                     pOrt ON (1)"};					
char OD[]={"OD                     pOrt timer Disabled"};			
char OE[]={"OE                     pOrt timer Enabled"};			
char OTT[]={"OThh:mm                pOrt/Bluetooth sTart time"};				//REV AG
char OPT[]={"OPhh:mm                pOrt/Bluetooth stoP time"};                 //REV AG

char OP[]={"OP:"};
char or[]={"or"};

char PA[]={"   PA: "};
char PB[]={"   PB: "};
char PC[]={"   PC: "};
char Position[]={"Pnnnnn                 Position array pointer to nnnnn"};
char Poweron[]={"Analog power supplies on."};
char Poweroff[]={"Analog power supplies off."};

char Reset[]={"R                      Reset memory"};
char RESET[]={"RESET                  RESET processor"};		
char Resetting[]={"Resetting..."};								
char ResetComplete[]={"RESET COMPLETE"};						
char Rev[]={"bc"};
char RnotAllowed[]={"RESET Not Allowed While Logging"};			
char RUsure[]={"Are you sure(Y/N)?"};
char R0[]={"R0-R1"};				
char R1[]={"R1-R0"};
char RTC32KHZON[]={"RTC 32KHz output on."};
char RTC32KHZOFF[]={"RTC 32KHz output off."};

char saaaa[]={"                       saaaaaa = polynomial coefficient A with sign"};
char sbbbb[]={"                       sbbbbbb = polynomial coefficient B with sign"};
char Scanint[]={"SCnnnnn                view SCan interval/enter nnnnn interval"};
char Scaninterval[]={"Scan interval: "};
char scccc[]={"                       scccccc = polynomial coefficient C with sign"};
char Seconds[]={" second(s)."};
char Setup[]={"Setup:"};
char sffff[]={"                       sffffff = gage factor with sign"};
char Softwareversion[]={"Software version: "};
char soooo[]={"                       soooooo = offset with sign"};
char Space[]={" "};
char Startlog[]={"SThh:mm                STart logging, hh:mm = start time"};
char Stoplog[]={"SPhh:mm                StoP logging, hh:mm = stop time"};
char StopTimeNotAllowed[]={"Stop Time Not Allowed While Log Intervals Are Enabled."};	
char sR[]={"SR                     Synchronize Readings(0=not synch'd,1=synch'd)"};	
char SV[]={"SV                     Software Version"};
char SwitchtoModbus[]={"Switch to MODBUS communications."};                     //VER BA
char Synch[]={"Readings are synchronized to the top of the hour."};	
char Synchnot[]={"Readings are not synchronized to the top of the hour."};	
char SystemStatus[]={"S,SS                   datalogger Status, System Status"};
char szzzz[]={"                       szzzzzz = zero reading with sign"};

char t[]={"                       t = Thermistor Type (0,1,2)"};                //REV J
char tt[]={"		       tt = Gage Type"};
char Tab[]={"   "};					//3 space tab
char TF[]={"TF                     Time Format(0=hhmm,1=hh,mm)"};
//char TH[]={"TH		       THermistor type(0=standard therm,1=high temp BR55KA822J,2=high temp 103JL1A)"};	//REM REV J 
char TH[]={"TH: "};					//REV J
char TherminfoWhere[]={"Thermistor information, where;"};                       //REV J
char THREEV[]={"3V Battery Voltage = "};
char TWELVEV[]={"12V Battery Voltage = "};
char ThisWillLoadDefaults[]={"This will load the factory default settings!"};   //VER 6.0.0
char Time[]={"   Time: "};
char Timeformat[]={"Time format is hhmm."};
char Timeformatcomma[]={"Time format is hh,mm."};
//char TimeOutFirmwareNotUpdated[]={"Timed-out...Firmware not updated."};       REM VER 6.0.5
char TM1[]={"LC-2MUX TEST MENU:"};
char TM2[]={"SELECTION         TEST"};
char TM4[]={"    4        EXCITATION"};
char TM9[]={"    1        FRAM"};
char TM10[]={"    2        +3X_X"};
char TM11[]={"    3        RTC 32KHz"};
char TM5[]={"    5        TEST VW CHANNEL"};
char TM12[]={"ENTER SELECTION:"};
char TM13[]={"    X        EXIT TEST MENU"};
char Tnn[]={"Tnn/t                  Thermistor information, where;"};                                                           //REV J
char TnotAllowed[]={"Test Functions Not Allowed While Logging"};
char TP[]={"TEST PASSED"};
char TFAIL[]={"TEST FAILED @ "};
char TR[]={"TR,TR0                 display TRap count, zero TRap count"}; 
char Trapcount[]={"Trap count: "};		
char _5VEXCON[]={"Excitation on."};
char _5VEXCOFF[]={"Excitation off."};
char _VWON1[]={"VW Channel on: GageType 1 (1400-3500Hz) selected."};             //REV H
char _VWON2[]={"VW Channel on: GageType 2 (2800-4500Hz) selected."};                                               //REV H
char _VWON3[]={"VW Channel on: GageType 3 (400-1200Hz) selected."};                                               //REV H
char _VWON4[]={"VW Channel on: GageType 4 (1200-2800Hz) selected."};                                               //REV H
char _VWON5[]={"VW Channel on: GageType 5 (2500-4500Hz) selected."};                                               //REV H
char _VWON6[]={"VW Channel on: GageType 6 (800-1600Hz) selected."};                                               //REV H
char _VWOFF[]={"VW Channel off."};                                             //REV H
char _selectGT[]={"Select GageType (1-6) Type any key to exit..."};                //REV K
char _wrong[]={"Not a valid GageType."};                                            //REV H


//char UF[]={"UF                     Update Firmware"};				REM VER 6.0.5
//char UpdateComplete[]={"Firmware Update Complete"};				REM VER 6.0.5
//char UpdateFirmware[]={"Update Firmware(Y/N)?"};				REM VER 6.0.5
char UP[]={"UP:"};

char Viewclock[]={"C                      view current Clock"};
char VL[]={"VL		       display Lithium cell Voltage"};
char VTHREE[]={"V3		       display 3V Battery Voltage"};
char VTWELVE[]={"V12		       display 12V Battery Voltage"};	
char VWTimeout[]={"VW Timeout Occurred!"};

//char WaitingForUpdate[]={"Waiting for update..."};				REM VER 6.0.5
char WF[]={"WF                     Wrap Format(0=don't wrap memory,1=wrap memory"}; 

char XCMD[]=("X                      Single Reading - NOT stored");
char ZR[]={"   ZR: "};


//RESET STRINGS:
char oscfail[]={"Oscillator Failure occurred."};                                //REV AE
char trapr[]={"A Trap Conflict Reset has occurred."};
char iopuwr[]={"An Illegal Opcode, Illegal Address or Uninitialized W has occurred."};
char extr[]={"A Master Clear (pin) Reset has occurred."};
char swr[]={"A RESET Instruction has been executed."};
char wdto[]={"WDT Timeout has occurred."};
char sleep[]={"Device has been in Sleep mode."};
char bor[]={"A Brown-out Reset has occurred."};
char por[]={"A Power-up Reset has occurred."};



void _3VX_on(void);
void _3VX_off(void);
unsigned long bcd2d(unsigned char bcd);
void Blink(unsigned char);                  
void BTStatus(void);                                                            //REV AG
void Buf2DateTime(char[]);
void Buf2GageData(char[]);
unsigned long Buffer2Decimal(char[], unsigned int, unsigned int);
int checkScanInterval(void);
void checkSynch(unsigned long);			
void clockMux(unsigned int);
void clockSwitch(unsigned char);                                                //REV AF
void clockThMux(unsigned int);                                                  
void CMDcomm(void);                                                             //REV BA
void config_Ports_Low_Power(void);
void configShutdownTimer(void);                                                 //REV Z
void configTimers(void);                                                        //REV M
void configUARTnormal(void);
//void configUARTsleep(void);   REM REV D
//void configVWchannel(unsigned char gageType);                                 //REM REV M
unsigned int convertStruc2Reg(unsigned char);
//void controlPortStatus(void);                                                 REM REV AG
void controlPortStatus(unsigned char);                                          //REV AG	
void crlf(void);
unsigned int debounce(void);
void disableBT(void);                                                           //REV AD
void disableChannel(int);                                                    
//void disableCOM(void);                                                        
//void disableINT2(void);                                                       //REM REV B
void disableINT1(void);                                                         //REV B
//void disableLPF(void);                                                          //REV G   REM REV H
void disableTimers(void);                                                       //REV M
void disableVWchannel(void);                                                    //REV H
void displayBCD(void);
//void displayBT(void);                                                           //REV AD  REM REV AG
void displayEEPROMError(int);                 
//void displayExternalAddress(unsigned int);    //REM VER 6.0.12
void displayGageInfo(int);
void displayLoggingWillStart(void);
void displayLoggingWillStop(void);
void displayLogInterval(int);
void displayLogTable(void);
void displayMemoryStatus(void);
void displayMUX(int);
void displayReading(int, unsigned long);
void displayReadingorDisabled(int);
void displayScanInterval(unsigned long, unsigned int);
void displayTemporaryStatus(int);
void displayTempReading(void);
void displayTestMenu(void);	
void enableChannel(int);
void enableBT(void);                                                            //REV AD
//void enableCOM(void);                                                         
void enableINT1(void);                                                          //REV B
//void enableLPF(unsigned char gageType);                                         //REV G   REM REV H
void enableVWchannel(unsigned char gageType);                                         //REV H
//void enableINT2(void);                                                        //REM REV B
unsigned int f32toINT16(float);                                                 //REV T
void formatandDisplayGageInfo(float TEMPVAL);
//void Fout(unsigned char gageType);                                            
int getConversion(void);
int getChannel(void);
void getGageInfo(unsigned char, int);
int getGageType(int);
int getLogInterval(void);
void getLogIntLength(void);
int getLogIterations(void);
//float getPeriod(unsigned char gageType);                                      //REM REV M
float getFrequency(void);                                                       //REV M
char getThermtype(void);                                                        //REV J
unsigned long getTxChecksum(unsigned int);                                      //REV AB
void handleCOMError(void);
void handleEEPROMResults(unsigned int);
void hms(unsigned long, unsigned char);
unsigned long hms2s(void);
float INT16tof32(void);                                                          //REV J
void loadDefaults(void);
int main(void);
unsigned int pack(unsigned char, unsigned char);
void pluck(unsigned int _Fstart, unsigned int _Fstop, unsigned int _cycles);
void pluckOFF(void);                                                            //REV E
void pluckPOS(void);                                                            //REV E
void pluckNEG(void);                                                            //REV E
void processReading(float, int);
void processDS3231Temperature(int);                                             //REV K
void prompt(void);
int qualifyNetAddress(void);
//float read_vw(unsigned char gageType);                                        //REM REV M
float read_vw(void);                                                            //REV M
void resetMemory(void);
//void resetVWchannel(void);                                                    //REM REV M
void restoreSettings(void);
unsigned long RTChms2s(unsigned char);
void saveSettings(void);
void seconds2hms(unsigned long s);
void setADCnormal(void);                                                        //REV D
void setADCsleep(void);                                                         //REV D
void setADCwake(void);                                                          //REV E
void setChannelConversion(int, int);
void setup(void);               //VER 6.0.0
void shutdown(void);
void shutdownNetworked(void);
void startLogging(void);
void stopLogging(void);
void storeGageType(int channel, int gageType);
void storeLogInterval(int, int);
void storeChannelReading(int);                                                
void storeTempChannelReading(int);
void storeTempChannelTHReading(int);                                            
unsigned long storeReading(int);
void storeTempReading(int);
unsigned long synch(unsigned long, unsigned long);
//int take_analog_reading(unsigned char);                                       //REM REV K
unsigned int take_analog_reading(unsigned char);                                //REV K
//void take_One_Complete_Reading(void);                                         
//void take_One_Complete_Temp_Reading(void);                                    
void take_One_Complete_Reading(unsigned char);                                  
float take_reading(unsigned char);
void testReset(void);
void testRx(void);              
int testStopTime(void);
void testPoint(unsigned char, unsigned char);                                 
void testPoint2(unsigned char);   
void toBCD(unsigned char value);
unsigned int toJulian(void);
unsigned char toMonthDay(unsigned int, unsigned int, unsigned char);
void unpack(unsigned int);
void upD8RTCAlarm1(void);
float V_HT2C(float, unsigned int);                                              //REV J
void wait(void);
void wait2S(void);                                                              //REV A
void wrap_stop(void);                                                           
//RTC prototypes:                                                               //REV D
void displayClock(unsigned char data);
void disableAlarm(unsigned char);			
void enableAlarm(unsigned char);			
unsigned char readClock(unsigned char address);
int readDS3231temperature(void);                                                //REV K
unsigned int RTCtoDecimal(unsigned char data);
void setChannelThermtype(int,char);                                             //REV J
void setClock(unsigned char address,unsigned char data);
//unsigned char Start1HzSQW(void);
void start32KHz(void);
void stop32KHz(void);
//void Stop1HzSQW(unsigned char);


#endif
