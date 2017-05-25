
#ifndef LC8004main_cj_H
#define LC8004main_cj_H

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
	//unsigned NetEnabled:1;          		//0=Network disabled, 1=Network enabled REM REV CH
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
    unsigned uCclock:1;                                                         //0=HS osc (Fcy=7.3728MHz) 1=HSPLL osc (Fcy=29.4912 MHz)
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

/*
//REV CF:
//MODBUS STATUS1 REGISTER:
typedef struct{                                                                 //   BIT          READ                              WRITE
	unsigned	CFG:3;                                                          //   012    110   VW8                               VW8                       Lsb
                                                                                //          101   TH32                              TH32
                                                                                //          100   VW32                              VW32
                                                                                //          011   TH8                               TH8
                                                                                //          010   VW/TH Single                      VW/TH Single
                                                                                //          001   VW/TH16                           VW/TH16
                                                                                //          000   VW/TH4                            VW/TH4
    unsigned    Setrtc:1;                                                       //   3                                        1=RTC Current Time Set REV CJ
	unsigned 	Logint:1;                                                       //   4      1=Log Ints Enabled, 0=Disabled    1=Enable, 0=Disable
	unsigned 	Wrap:1;                                                         //   5      1=Memory Wrap Enabled, 0=Disabled 1=Enable, 0=Disable
	unsigned	BT:1;                                                           //   6      1=Bluetooth ON, 0=OFF             1=Turn BT ON, 0=Turn OFF
	unsigned	BT_Timer:1;                                                     //   7      1=BT Timer Enabled, 0=Disabled    1=Enable BT Timer, 0=Disable
	unsigned	OP:1;                                                           //   8      1=Output Port ON, 0=OFF           1=Turn ON, 0=Turn OFF
	unsigned	OP_Timer:1;                                                     //   9      1=OP Timer Enabled, 0=Disabled    1=Enable OP Timer, 0=Disable
	unsigned	Sync:1;                                                         //   10     1=Readings Sync'd, 0=not Sync'd   1=Sync Readings,0=Don't Sync
	unsigned 	ST:1;                                                           //   11     1=Start Time Enabled,0=Disabled   1=Enable Start Time,0=Disable
    unsigned 	SP:1;                                                           //   12     1=Stop Time Enabled,0=Disabled    1=Enable Stop Time,0=Disable    
    unsigned    Readrtc:1;                                                      //   13                                       1=RTC Current Time Read
    unsigned    X:1;                                                            //   14                                       1=Take single reading not stored                                  
	unsigned	Logging:1;                                                      //   15     1=Started, 0=Stopped              1=Start, 0=Stop                 Msb    
}Status1ControlBits;
typedef union{ unsigned int status1;
Status1ControlBits status1flags;
}s1flags;
s1flags	S_1;


//REV CJ:
//MODBUS STATUS2 REGISTER:
typedef struct{                                                                 //   BIT          READ                              WRITE
	unsigned	R:1;                                                            //   0              0                           1=Reset Memory Pointers           Lsb
    unsigned    RST:1;                                                          //   1              0                           1=Reboot uC  
    unsigned    CMD:1;                                                          //   2              0                           1=Reboot into command line interface
	unsigned 	LD:1;                                                           //   3              0                           1=Load Defaults
	unsigned 	CNV:1;                                                          //   4      1=Linear Conversion,0=Poly          1=Select Linear, 0=Select Poly
	unsigned	bit5:1;                                                          
	unsigned	bit6:1;                                                     
	unsigned	bit7:1;                                                           
	unsigned	bit8:1;                                                     
	unsigned	bit9:1;                                                         
	unsigned 	bit10:1;                                                           
    unsigned 	bit11:1;                                                              
    unsigned    bit12:1;                                                      
    unsigned    bit13:1;                                                                                             
	unsigned	bit14:1;
    unsigned    bit15:1;                                                        //                                                                                  Msb    
}Status2ControlBits;
typedef union{ unsigned int status2;
Status2ControlBits status2flags;
}s2flags;
s2flags	S_2;
*/
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
char trapBUF[6];                                                                //REV CF
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

//REV V:
//Max # of readings for allowable data space (0x7F794 (522132) is max array start address in data space based on 140 byte array size for VW32 configuration)
//CFG MEM START - 140 bytes = max data space start address
//0x7f820 - 0x8C = 0x7f794
//522272 - 140 = 522132
const unsigned int  maxSingleVW=29000;                                          //522132/18 = 29007
const unsigned int  maxFourVW=14500;                                            //522132/36 = 14503
const unsigned int  maxEightVW=11860;                                           //522132/44 = 11866 REV BE
const unsigned int  maxSixteenVW=4830;                                          //522132/108 = 4834          
const unsigned int  maxThirtytwoVW=3720;                                        //522132/140 = 3729
const unsigned int  maxEightTH=18640;                                           //522132/28 = 18647
const unsigned int  maxThirtytwoTH=6870;                                        //522132/76 = 6870

const unsigned char minScanSingleVW=4;                                          //TEST REV V
const unsigned char minScanFourVW=5;
const unsigned char minScanEightVW=10;                                           //REV BH
const unsigned char minScanSixteenVW=20;
const unsigned char minScanThirtytwoVW=30;
const unsigned char minScanEightTH=5;                                           
const unsigned char minScanThirtytwoTH=10;                                       //REV BH  

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
unsigned int MODBUSaddressvalue;                                                //REV BC
float percent=0.0;
float processedReading=0.0;
/*REM REV CI:
long LogIntLength=10;					
long LogIntLength1=0;					//Scan rate for Logarithmic Intervals
long LogIntLength2=0;
long LogIntLength3=0;
long LogIntLength4=0;
long LogIntLength5=0;
long LogIntLength6=0;
*/
//REV CI:
unsigned long LogIntLength=10;					
unsigned long LogIntLength1=0;					//Scan rate for Logarithmic Intervals
unsigned long LogIntLength2=0;
unsigned long LogIntLength3=0;
unsigned long LogIntLength4=0;
unsigned long LogIntLength5=0;
unsigned long LogIntLength6=0;
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
int NAdata=0;                                                                   //network address value for display		
//int netTest=0;			REM REV CH
int stopTimeTest;

int testScanInterval = 0;                                                       //REV CF

const float tcy=0.0000000339084201389;                                          //29.4912MHz Fcy    REV AE
const float mS512=511.965;                                                      //REV M
const float mS256=256.0;                                                        //REV AF
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
#define SCL_VW                  LATAbits.LATA2      //SCL for AD5241 digital pot    (output)    REV CB
#define SDA_VW                  LATAbits.LATA3      //SDA for AD5241 digital pot    (output)    REV CB
#define _CLK_INT                PORTAbits.RA12		//RTC Interrupt - falling edge	(INPUT)
#define PAUSE                   PORTAbits.RA13      //External control input    (INPUT) REV CA

//PortB Defines
#define VW                      PORTBbits.RB0		//VW analog input AN0   (INPUT)
#define V3_SENSE                PORTBbits.RB2       //3V analog input AN2   (INPUT)
#define V12_SENSE               PORTBbits.RB3       //12V analog input AN3  (INPUT)
#define V_TH_EXT                PORTBbits.RB4       //Ext Therm analog input AN4    (INPUT)
#define V_LITH                  PORTBbits.RB5       //Lithium cell analog input AN5 (INPUT)
#define VW_LPF                  PORTBbits.RB8       //VW_LPF analog input AN8   (INPUT)
#define V_AGC                   PORTBbits.RB9       //AGC Control voltage analog input  (INPUT)

//PortC Defines
#define VW100                   PORTCbits.RC1       //VW100 T2CK counter input  (INPUT)
#define _AMP_SHDN               LATCbits.LATC2      //AGC amp shutdown control (1=on,0=off) (OUTPUT) REV CA
#define IN1                     LATCbits.LATC3       //H-Bridge drive OUT1 control (OUTPUT) REV CA
#define EXC_EN                  LATCbits.LATC4       //H-Bridge drive Enable    (OUTPUT) REV CA

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
#define RS485TX_EN              LATDbits.LATD15     //1=Enable RS485 Tx, 0=disable RS485 Tx (OUTPUT)        REV CA
//PortE Defines
#define _EXC_EN                 LATEbits.LATE5      //Excitation enable                     (OUTPUT)
                                                    //1=disabled, 0=enabled(default)        (OUTPUT)
#define V9_EXC                  LATEbits.LATE6      //9V Excitation                        (OUTPUT) REV CA
                                                    //1=enabled, 0=disabled(default)        

//PortF Defines
#define	CONTROL                 LATFbits.LATF0		//Control port for external circuit	(OUTPUT)
                                                    //1=active, 0=inactive(default)
#define _READ                   LATFbits.LATF1      //LED control                       (OUTPUT)
                                                    //1=not lit(default), 0=lit
#define _RS485RX_EN             LATFbits.LATF4      //RS485 receive enable              (OUTPUT)
                                                    //1=RX disabled(default), 0=RX enabled
#define _232SHDN                LATFbits.LATF5      //RS-232 enable                     (OUTPUT)
                                                    //1=RS-232 enabled, 0=RS-232 shutdown(default)
#define WP                      LATFbits.LATF6		//0=EEPROM Enabled, 1=EEPROM Write Protected(default)	(OUTPUT)
#define _232                    PORTFbits.RF7       //0=RS-232 connected,1=RS-232 not connected     (INPUT) 
#define	USB_PWR                 PORTFbits.RF8		//1= USB powered, 0= USB not powered	(INPUT)
#define	MUX_CLOCK               LATFbits.LATF12		//Multiplexer Clock (default 0)		(OUTPUT)
#define	MUX_RESET               LATFbits.LATF13		//Multiplexer Reset	(default 0) 	(OUTPUT)

//PortG Defines
#define IN2                     LATGbits.LATG6      //H-Bridge drive OUT2 control   (OUTPUT)    REV CA
#define A                       LATGbits.LATG12     //PLL Fco Select control                (OUTPUT)
#define B                       LATGbits.LATG13     //PLL Fco Select control                (OUTPUT)
#define C                       LATGbits.LATG14     //PLL Fco Select control                (OUTPUT)
#define D                       LATGbits.LATG15     //PLL Fco Select control                (OUTPUT)

//----------------------------------------------------------------------------
//              FCY
//----------------------------------------------------------------------------
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
#define VW8Bytes                44                                              //10445 readings max    
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
#define	backspace		0x08			//ASCII Backspace
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
char BTOFF[]={"Bluetooth OFF."};                                                //REV CF
char BTON[]={"Bluetooth ON."};                                                  //REV CF
char BTDisablenotAllowed[]={"Disabling Bluetooth not allowed while connected."};//REV AD
char BTTIMEREN[]={"Bluetooth Timer Enabled"};                                   //REV AG
char BTTIMERDIS[]={"Bluetooth Timer Disabled"};                                 //REV AG

char c[]={"		       c = Conversion Type(L/P)"};
char Calclock[]={"Calibrate clock circuit now."};
//char Changes[]={"Changes not allowed while networked."};		REM REV CH
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
char Displaynnnn[]={"Dnnnnn                 Display nnnnn arrays from pointer"};    //REV BC
char DL[]={"DL		       Datalogger Type"};					
char DnotAllowed[]={"DEFAULT Not Allowed While Logging"};
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

//char NetworkaddressIS[]={"Network address: "};    REM REV CH
//char Networkstatus_enable_disable[]={"NS,ND,NE	       Network Status, Disable, Enable"};   REM REV CH
//char Networkaddress[]={"NAddd		       Network Address (1-256)"};   REM REV CH
//char Networkdisabled[]={"Network recognition disabled."}; REM REV CH
//char Networkenabled[]={"Network recognition enabled."};   REM REV CH
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
char Rev[]={"cj"};
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
void BT_D(void);                                                                //REV CJ
void BT_E(void);                                                                 //REV CJ
void BTONE(void);                                                               //REV CJ
void BTStatus(void);                                                            //REV AG
void BTZERO(void);                                                              //REV CJ
void Buf2DateTime(char[]);
void Buf2GageData(char[]);
unsigned long Buffer2Decimal(char[], unsigned int, unsigned int);
int checkScanInterval(void);
void checkSynch(unsigned long);			
void clockMux(unsigned int);
void clockSwitch(unsigned char);                                                //REV AF
void clockThMux(unsigned int);                                                  
void CMDcomm(void);                                                             //REV BA
void CMD_LINE(void);                                                            //REV CH
void configShutdownTimer(void);                                                 //REV Z
void configTimers(void);                                                        //REV M
void configUARTnormal(void);
unsigned int convertStruc2Reg(unsigned char);
void controlPortStatus(unsigned char);                                          //REV AG	
void crlf(void);
unsigned int debounce(void);
void disableBT(void);                                                           //REV AD
void disableOP(void);                                                           //REV CC
void disableChannel(int);                                                    
void disableINT1(void);                                                         //REV B
void DISLOGINT(void);                                                           //REV CI
void disableTimers(void);                                                       //REV M
void disableVWchannel(void);                                                    //REV H
void displayBCD(void);
void displayEEPROMError(int);                 
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
void ENLOGINT(void);                                                            //REV CI
void enableOP(void);                                                            //REV CC
void enableINT1(void);                                                          //REV B
void enableVWchannel(unsigned char gageType);                                         //REV H
unsigned int f32toINT16(float);                                                 //REV T
void formatandDisplayGageInfo(float TEMPVAL);
int getConversion(void);
int getChannel(void);
void getGageInfo(unsigned char, int);
int getGageType(int);
int getLogInterval(void);
void getLogIntLength(void);
int getLogIterations(void);
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
void MX1(void);                                                                 //REV CD
void MX4(void);                                                                 //REV CD
void MX8(void);                                                                 //REV CD
void MX8T(void);                                                                //REV CD
void MX16(void);                                                                //REV CD
void MX32(void);                                                                //REV CD
void MX32T(void);                                                               //REV CD
void O_D(void);                                                                 //REV CJ
void O_E(void);                                                                 //REV CJ
unsigned int pack(unsigned char, unsigned char);
void pluck(unsigned int _Fstart, unsigned int _Fstop, unsigned int _cycles);
void pluckOFF(void);                                                            //REV E
void pluckON(void);                                                             //REV CB
void pluckPOS(void);                                                            //REV E
void pluckNEG(void);                                                            //REV E
void processReading(float, int);
void processDS3231Temperature(int);                                             //REV K
void prompt(void);
int qualifyNetAddress(void);
void R(void);                                                                   //REV CH
void READ_TIME(void);                                                           //REV CG
void RST(void);                                                                 //REV CH
float read_vw(void);                                                            //REV M
void reloadLogTable(void);                                                      //REV CI
void resetMemory(void);
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
//void shutdownNetworked(void); REM REV CH
unsigned int START(void);                                                       //REV CF
void startLogging(void);
unsigned int STOP(void);                                                        //REV CF
void stopLogging(void);
void storeGageType(int channel, int gageType);
void storeLogInterval(int, int);
void storeChannelReading(int);                                                
void storeTempChannelReading(int);
void storeTempChannelTHReading(int);                                            
unsigned long storeReading(int);
void storeTempReading(int);
unsigned long synch(unsigned long, unsigned long);
void synch_one(void);                                                           //REV CC
void synch_zero(void);                                                          //REV CC
unsigned int take_analog_reading(unsigned char);                                //REV K
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
void wrap_one(void);                                                            //REV CC
void wrap_stop(void);                                                           
void wrap_zero(void);                                                           //REV CC
void WRITE_TIME(void);                                                          //REV CG
void X(void);                                                                   //REV CE


//RTC prototypes:                                                               //REV D
void displayClock(unsigned char data);
void disableAlarm(unsigned char);			
void enableAlarm(unsigned char);			
unsigned char readClock(unsigned char address);
int readDS3231temperature(void);                                                //REV K
unsigned int RTCtoDecimal(unsigned char data);
void setChannelThermtype(int,char);                                             //REV J
void setClock(unsigned char address,unsigned char data);
void start32KHz(void);
void stop32KHz(void);

#endif
