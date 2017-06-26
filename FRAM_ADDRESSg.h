#ifndef FRAM_ADDRESSG_H
#define FRAM_ADDRESSG_H

//REV B 03/30/17    Remap memory so that common functions are grouped together
//REV C 04/14/17    Remap memory to allow address 0x0000 to be used as MODBUS page select address
//REV D 05/12/17    Add memory locations for MODBUS current time Clock Read and Clock Write
//REV E 06/09/17    Relocate MODBUS control registers to preceed gage settings registers
//REV F 06/15/17    Locate all MODBUS password protected registers together
//REV G 06/22/17    Locate all 16 bit registers together. Locate all 32 bit registers together


//----------CONTROL/CONFIGURATION REGISTERS STORED IN EXTERNAL MEMORY------------------------------

//NOTE: Data Memory (Readings) from 0x0 to 0x7f81f                              //522.272kB data  
//      0x7f820-0x7fa6b is temporary storage for 'X' command                    //587 bytes for X command
//      CFG Memory (configuration) from 0x7fa6c to 0x7ff2c                      //1216 bytes CFG


#define XmemStart                   0x7f820                                     //Beginning of X storage

#define	LC2CONTROLflagsaddress		0x7fa6c                                     //2 bytes   
#define	LC2CONTROL2flagsaddress		0x7fa6e                                     //2 bytes
#define	DISPLAY_CONTROLflagsaddress	0x7fa70                                     //2 bytes
#define FRAM_MEMORYflagsaddress     0x7fa72                                     //2 bytes
#define CONTROL_PORTflagsaddress	0x7fa74                                     //2 bytes

#define MemoryStatusaddress         0x7fa76                                     //2 bytes
#define OutputPositionaddress		0x7fa78                                     //2 bytes
#define UserPositionaddress         0x7fa7a                                     //2 bytes

#define BatteryReading              0x7fa7c                                     //2 bytes

#define LogItRemain1address         0x7fa7e                                     //2 bytes
#define LogItRemain2address         0x7fa80                                     //2 bytes
#define LogItRemain3address         0x7fa82                                     //2 bytes
#define LogItRemain4address         0x7fa84                                     //2 bytes
#define LogItRemain5address         0x7fa86                                     //2 bytes
#define LogItRemain6address         0x7fa88                                     //2 bytes

#define SingleLogItRemain1address	0x7fa8a                                     //2 bytes
#define SingleLogItRemain2address	0x7fa8c                                     //2 bytes
#define SingleLogItRemain3address	0x7fa8e                                     //2 bytes
#define SingleLogItRemain4address	0x7fa90                                     //2 bytes
#define SingleLogItRemain5address	0x7fa92                                     //2 bytes
#define SingleLogItRemain6address	0x7fa94                                     //2 bytes

#define LogIt1MASTERaddress         0x7fa96                                     //2 bytes
#define LogIt2MASTERaddress         0x7fa98                                     //2 bytes
#define LogIt3MASTERaddress         0x7fa9a                                     //2 bytes
#define LogIt4MASTERaddress         0x7fa9c                                     //2 bytes
#define LogIt5MASTERaddress         0x7fa9e                                     //2 bytes
#define LogIt6MASTERaddress         0x7faa0                                     //2 bytes
#define SingleLogIt1MASTERaddress   0x7faa2                                     //2 bytes
#define SingleLogIt2MASTERaddress   0x7faa4                                     //2 bytes
#define SingleLogIt3MASTERaddress   0x7faa6                                     //2 bytes
#define SingleLogIt4MASTERaddress   0x7faa8                                     //2 bytes
#define SingleLogIt5MASTERaddress   0x7faaa                                     //2 bytes
#define SingleLogIt6MASTERaddress   0x7faac                                     //2 bytes

#define LogItRemain1MASTERaddress   0x7faae                                     //2 bytes
#define LogItRemain2MASTERaddress   0x7fab0                                     //2 bytes
#define LogItRemain3MASTERaddress   0x7fab2                                     //2 bytes
#define LogItRemain4MASTERaddress   0x7fab4                                     //2 bytes
#define LogItRemain5MASTERaddress   0x7fab6                                     //2 bytes
#define LogItRemain6MASTERaddress   0x7fab8                                     //2 bytes
#define SingleLogItRemain1MASTERaddress    0x7faba                              //2 bytes
#define SingleLogItRemain2MASTERaddress    0x7fabc                              //2 bytes
#define SingleLogItRemain3MASTERaddress    0x7fabe                              //2 bytes
#define SingleLogItRemain4MASTERaddress    0x7fac0                              //2 bytes
#define SingleLogItRemain5MASTERaddress    0x7fac2                              //2 bytes
#define SingleLogItRemain6MASTERaddress    0x7fac4                              //2 bytes

#define LogIntLength1MASTERaddress         0x7fac6                              //4 bytes
#define LogIntLength2MASTERaddress         0x7faca                              //4 bytes
#define LogIntLength3MASTERaddress         0x7face                              //4 bytes
#define LogIntLength4MASTERaddress         0x7fad2                              //4 bytes
#define LogIntLength5MASTERaddress         0x7fad6                              //4 bytes
#define LogIntLength6MASTERaddress         0x7fada                              //4 bytes
#define SingleLogIntLength1MASTERaddress   0x7fade                              //4 bytes
#define SingleLogIntLength2MASTERaddress   0x7fae2                              //4 bytes
#define SingleLogIntLength3MASTERaddress   0x7fae6                              //4 bytes
#define SingleLogIntLength4MASTERaddress   0x7faea                              //4 bytes
#define SingleLogIntLength5MASTERaddress   0x7faee                              //4 bytes
#define SingleLogIntLength6MASTERaddress   0x7faf2                              //4 bytes

#define TotalStartSecondsaddress	0x7faf6                                     //4 bytes
#define TotalStopSecondsaddress		0x7fafa                                     //4 bytes

#define scratchaddress              0x7fafe                                     //4 bytes 


//MODBUS PASSWORD PROTECTED REGISTERS:
#define baudrateaddress             0x7fb02                                     //2 bytes   PASSWORD PROTECTED
#define MODBUSaddress               0x7fb04                                     //2 bytes   PASSWORD PROTECTED
#define HardwareRevision            0x7fb06                                     //2 bytes   REV E   PASSWORD PROTECTED
#define FirmwareRevision            0x7fb08                                     //2 bytes   REV E   PASSWORD PROTECTED
#define SerialNumberHIGH            0x7fb0a                                     //4 bytes   REV E   PASSWORD PROTECTED
#define SerialNumberLOW             0x7fb0c                                     
#define ProtectedRESERVED1          0x7fb0e                                     //2 bytes   PASSWORD PROTECTED
#define ProtectedRESERVED2          0x7fb10                                     //2 bytes   PASSWORD PROTECTED

//MODBUS Open Registers:
#define passwordaddressHIGH         0x7fb12                                     //4 bytes   
#define passwordaddressLOW          0x7fb14  
#define MODBUS_PAGEaddress          0x7fb16                                     //2 bytes  
#define MODBUS_STATUS1address       0x7fb18                                     //MODBUS STATUS1 flags  2 bytes
#define MODBUS_STATUS2address       0x7fb1a                                     //MODBUS STATUS2 flags  2 bytes   
#define RTCYearsaddress             0x7fb1c                                     //2 bytes   
#define RTCMonthsaddress            0x7fb1e                                     //2 bytes   
#define RTCDaysaddress              0x7fb20                                     //2 bytes   
#define RTCHoursaddress             0x7fb22                                     //2 bytes   
#define RTCMinutesaddress           0x7fb24                                     //2 bytes   
#define RTCSecondsaddress           0x7fb26                                     //2 bytes   
#define RESERVED1                   0x7fb28                                     //2 bytes
#define RESERVED2                   0x7fb2a                                     //2 bytes

#define	MUX4_ENABLEflagsaddress		0x7fb2c                                     //2 bytes
#define MUX_ENABLE1_16flagsaddress  0x7fb2e                                     //2 bytes
#define MUX_ENABLE17_32flagsaddress 0x7fb30                                     //2 bytes
#define MUX_CONV1_16flagsaddress    0x7fb32                                     //2 bytes
#define MUX_CONV17_32flagsaddress   0x7fb34                                     //2 bytes

#define ScanHoursaddress            0x7fb36                                     //2 bytes
#define ScanMinutesaddress          0x7fb38                                     //2 bytes
#define ScanSecondsaddress          0x7fb3a                                     //2 bytes

#define LoggingStopHoursaddress		0x7fb3c                                     //2 bytes
#define LoggingStopMinutesaddress	0x7fb3e                                     //2 bytes
#define LoggingStopSecondsaddress	0x7fb40                                     //2 bytes

#define TrapRegisteraddress         0x7fb42                                     //2 bytes

#define PortOffHoursaddress         0x7fb44                                     //2 bytes
#define PortOffMinutesaddress		0x7fb46                                     //2 bytes
#define PortOnHoursaddress          0x7fb48                                     //2 bytes
#define PortOnMinutesaddress		0x7fb4a                                     //2 bytes
#define startHoursaddress           0x7fb4c                                     //2 bytes  
#define startMinutesaddress         0x7fb4e                                     //2 bytes    
#define startSecondsaddress         0x7fb50                                     //2 bytes    
#define LoggingStartDayaddress		0x7fb52                                     //2 bytes 

//IDaddress = beginning of 16 character ID array 
#define IDaddress                   0x7fb54                                     //16 bytes     

//Gage Type Addresses (0x7fb64 -> 0x7fba2)  64 Bytes                           
#define CH1GTaddress                0x7fb64                                     //2 bytes
#define CH2GTaddress                0x7fb66                                     //2 bytes	
#define CH3GTaddress                0x7fb68                                     //2 bytes
#define CH4GTaddress                0x7fb6a                                     //2 bytes
#define CH5GTaddress                0x7fb6c                                     //2 bytes
#define CH6GTaddress                0x7fb6e                                     //2 bytes
#define CH7GTaddress                0x7fb70                                     //2 bytes
#define CH8GTaddress                0x7fb72                                     //2 bytes
#define CH9GTaddress                0x7fb74                                     //2 bytes
#define CH10GTaddress               0x7fb76                                     //2 bytes
#define CH11GTaddress               0x7fb78                                     //2 bytes
#define CH12GTaddress               0x7fb7a                                     //2 bytes
#define CH13GTaddress               0x7fb7c                                     //2 bytes
#define CH14GTaddress               0x7fb7e                                     //2 bytes
#define CH15GTaddress               0x7fb80                                     //2 bytes
#define CH16GTaddress               0x7fb82                                     //2 bytes
#define CH17GTaddress               0x7fb84                                     //2 bytes	
#define CH18GTaddress               0x7fb86                                     //2 bytes
#define CH19GTaddress               0x7fb88                                     //2 bytes
#define CH20GTaddress               0x7fb8a                                     //2 bytes
#define CH21GTaddress               0x7fb8c                                     //2 bytes
#define CH22GTaddress               0x7fb8e                                     //2 bytes
#define CH23GTaddress               0x7fb90                                     //2 bytes
#define CH24GTaddress               0x7fb92                                     //2 bytes
#define CH25GTaddress               0x7fb94                                     //2 bytes
#define CH26GTaddress               0x7fb96                                     //2 bytes
#define CH27GTaddress               0x7fb98                                     //2 bytes
#define CH28GTaddress               0x7fb9a                                     //2 bytes
#define CH29GTaddress               0x7fb9c                                     //2 bytes
#define CH30GTaddress               0x7fb9e                                     //2 bytes
#define CH31GTaddress               0x7fba0                                     //2 bytes
#define CH32GTaddress               0x7fba2                                     //2 bytes

//Thermistor Type Addresses  (0x7fba4 -> 0x7fbe2)   64 Bytes
#define CH1THaddress                0x7fba4                                     //CH1 thermistor type   2 bytes
#define CH2THaddress                0x7fba6                                     //CH2 thermistor type   2 bytes
#define CH3THaddress                0x7fba8                                     //CH3 thermistor type   2 bytes
#define CH4THaddress                0x7fbaa                                     //CH4 thermistor type   2 bytes
#define CH5THaddress                0x7fbac                                     //CH5 thermistor type   2 bytes
#define CH6THaddress                0x7fbae                                     //CH6 thermistor type   2 bytes
#define CH7THaddress                0x7fbb0                                     //CH7 thermistor type   2 bytes
#define CH8THaddress                0x7fbb2                                     //CH8 thermistor type   2 bytes
#define CH9THaddress                0x7fbb4                                     //CH9 thermistor type   2 bytes
#define CH10THaddress               0x7fbb6                                     //CH10 thermistor type  2 bytes
#define CH11THaddress               0x7fbb8                                     //CH11 thermistor type  2 bytes
#define CH12THaddress               0x7fbba                                     //CH12 thermistor type  2 bytes
#define CH13THaddress               0x7fbbc                                     //CH13 thermistor type  2 bytes
#define CH14THaddress               0x7fbbe                                     //CH14 thermistor type  2 bytes
#define CH15THaddress               0x7fbc0                                     //CH15 thermistor type  2 bytes
#define CH16THaddress               0x7fbc2                                     //CH16 thermistor type  2 bytes
#define CH17THaddress               0x7fbc4                                     //CH17 thermistor type  2 bytes
#define CH18THaddress               0x7fbc6                                     //CH18 thermistor type  2 bytes
#define CH19THaddress               0x7fbc8                                     //CH19 thermistor type  2 bytes
#define CH20THaddress               0x7fbca                                     //CH20 thermistor type  2 bytes
#define CH21THaddress               0x7fbcc                                     //CH21 thermistor type  2 bytes
#define CH22THaddress               0x7fbce                                     //CH22 thermistor type  2 bytes
#define CH23THaddress               0x7fbd0                                     //CH23 thermistor type  2 bytes
#define CH24THaddress               0x7fbd2                                     //CH24 thermistor type  2 bytes
#define CH25THaddress               0x7fbd4                                     //CH25 thermistor type  2 bytes
#define CH26THaddress               0x7fbd6                                     //CH26 thermistor type  2 bytes
#define CH27THaddress               0x7fbd8                                     //CH27 thermistor type  2 bytes
#define CH28THaddress               0x7fbda                                     //CH28 thermistor type  2 bytes
#define CH29THaddress               0x7fbdc                                     //CH29 thermistor type  2 bytes
#define CH30THaddress               0x7fbde                                     //CH30 thermistor type  2 bytes
#define CH31THaddress               0x7fbe0                                     //CH31 thermistor type  2 bytes
#define CH32THaddress               0x7fbe2                                     //CH32 thermistor type  2 bytes

#define LogIt1address               0x7fbe4                                     //2 bytes
#define LogIt2address               0x7fbe6                                     //2 bytes
#define LogIt3address               0x7fbe8                                     //2 bytes
#define LogIt4address               0x7fbea                                     //2 bytes
#define LogIt5address               0x7fbec                                     //2 bytes
#define LogIt6address               0x7fbee                                     //2 bytes
#define SingleLogIt1address         0x7fbf0                                     //2 bytes
#define SingleLogIt2address         0x7fbf2                                     //2 bytes
#define SingleLogIt3address         0x7fbf4                                     //2 bytes
#define SingleLogIt4address         0x7fbf6                                     //2 bytes
#define SingleLogIt5address         0x7fbf8                                     //2 bytes
#define SingleLogIt6address         0x7fbfa                                     //2 bytes

#define RESERVED1                   0x7fbfc                                     //2 bytes
#define RESERVED2                   0x7fbfe                                     //2 bytes

#define	LogIntLength1address		0x7fc00                                     //4 bytes   32 bit unsigned int
#define LogIntLength2address		0x7fc04                                     //4 bytes
#define LogIntLength3address		0x7fc08                                     //4 bytes
#define LogIntLength4address		0x7fc0c                                     //4 bytes
#define LogIntLength5address		0x7fc10                                     //4 bytes
#define LogIntLength6address		0x7fc14                                     //4 bytes
#define	SingleLogIntLength1address	0x7fc18                                     //4 bytes
#define SingleLogIntLength2address	0x7fc1c                                     //4 bytes
#define SingleLogIntLength3address	0x7fc20                                     //4 bytes
#define SingleLogIntLength4address	0x7fc24                                     //4 bytes
#define SingleLogIntLength5address	0x7fc28                                     //4 bytes
#define SingleLogIntLength6address	0x7fc2c                                     //4 bytes

//Zero Reading Addresses (0x7fc30 -> 0x7fcac)   128 Bytes
#define CH1ZRaddress                0x7fc30                                     //4 bytes   32 bit float
#define CH2ZRaddress                0x7fc34                                     //4 bytes
#define CH3ZRaddress                0x7fc38                                     //4 bytes
#define CH4ZRaddress                0x7fc3c                                     //4 bytes
#define CH5ZRaddress                0x7fc40                                     //4 bytes
#define CH6ZRaddress                0x7fc44                                     //4 bytes
#define CH7ZRaddress                0x7fc48                                     //4 bytes
#define CH8ZRaddress                0x7fc4c                                     //4 bytes
#define CH9ZRaddress                0x7fc50                                     //4 bytes
#define CH10ZRaddress               0x7fc54                                     //4 bytes  
#define CH11ZRaddress               0x7fc58                                     //4 bytes
#define CH12ZRaddress               0x7fc5c                                     //4 bytes
#define CH13ZRaddress               0x7fc60                                     //4 bytes
#define CH14ZRaddress               0x7fc64                                     //4 bytes
#define CH15ZRaddress               0x7fc68                                     //4 bytes
#define CH16ZRaddress               0x7fc6c                                     //4 bytes
#define CH17ZRaddress               0x7fc70                                     //4 bytes
#define CH18ZRaddress               0x7fc74                                     //4 bytes
#define CH19ZRaddress               0x7fc78                                     //4 bytes
#define CH20ZRaddress               0x7fc7c                                     //4 bytes
#define CH21ZRaddress               0x7fc80                                     //4 bytes
#define CH22ZRaddress               0x7fc84                                     //4 bytes
#define CH23ZRaddress               0x7fc88                                     //4 bytes
#define CH24ZRaddress               0x7fc8c                                     //4 bytes
#define CH25ZRaddress               0x7fc90                                     //4 bytes
#define CH26ZRaddress               0x7fc94                                     //4 bytes
#define CH27ZRaddress               0x7fc98                                     //4 bytes
#define CH28ZRaddress               0x7fc9c                                     //4 bytes
#define CH29ZRaddress               0x7fca0                                     //4 bytes
#define CH30ZRaddress               0x7fca4                                     //4 bytes
#define CH31ZRaddress               0x7fca8                                     //4 bytes
#define CH32ZRaddress               0x7fcac                                     //4 bytes


//Gage Factor Addresses (0x7fcb0 -> 0x7fd2c)    128 Bytes
#define CH1GFaddress                0x7fcb0                                     //4 bytes   32 bit float
#define CH2GFaddress                0x7fcb4                                     //4 bytes
#define CH3GFaddress                0x7fcb8                                     //4 bytes
#define CH4GFaddress                0x7fcbc                                     //4 bytes
#define CH5GFaddress                0x7fcc0                                     //4 bytes
#define CH6GFaddress                0x7fcc4                                     //4 bytes
#define CH7GFaddress                0x7fcc8                                     //4 bytes
#define CH8GFaddress                0x7fccc                                     //4 bytes
#define CH9GFaddress                0x7fcd0                                     //4 bytes
#define CH10GFaddress               0x7fcd4                                     //4 bytes
#define CH11GFaddress               0x7fcd8                                     //4 bytes
#define CH12GFaddress               0x7fcdc                                     //4 bytes
#define CH13GFaddress               0x7fce0                                     //4 bytes
#define CH14GFaddress               0x7fce4                                     //4 bytes
#define CH15GFaddress               0x7fce8                                     //4 bytes
#define CH16GFaddress               0x7fcec                                     //4 bytes
#define CH17GFaddress               0x7fcf0                                     //4 bytes
#define CH18GFaddress               0x7fcf4                                     //4 bytes
#define CH19GFaddress               0x7fcf8                                     //4 bytes
#define CH20GFaddress               0x7fcfc                                     //4 bytes
#define CH21GFaddress               0x7fd00                                     //4 bytes
#define CH22GFaddress               0x7fd04                                     //4 bytes
#define CH23GFaddress               0x7fd08                                     //4 bytes
#define CH24GFaddress               0x7fd0c                                     //4 bytes
#define CH25GFaddress               0x7fd10                                     //4 bytes
#define CH26GFaddress               0x7fd14                                     //4 bytes
#define CH27GFaddress               0x7fd18                                     //4 bytes
#define CH28GFaddress               0x7fd1c                                     //4 bytes
#define CH29GFaddress               0x7fd20                                     //4 bytes
#define CH30GFaddress               0x7fd24                                     //4 bytes
#define CH31GFaddress               0x7fd28                                     //4 bytes
#define CH32GFaddress               0x7fd2c                                     //4 bytes


//Gage Offset Addresses (0x7fd30 -> 0x7fdac)    128 Bytes
#define CH1GOaddress                0x7fd30                                     //4 bytes   32 bit float
#define CH2GOaddress                0x7fd34                                     //4 bytes
#define CH3GOaddress                0x7fd38                                     //4 bytes
#define CH4GOaddress                0x7fd3c                                     //4 bytes
#define CH5GOaddress                0x7fd40                                     //4 bytes
#define CH6GOaddress                0x7fd44                                     //4 bytes
#define CH7GOaddress                0x7fd48                                     //4 bytes
#define CH8GOaddress                0x7fd4c                                     //4 bytes
#define CH9GOaddress                0x7fd50                                     //4 bytes
#define CH10GOaddress               0x7fd54                                     //4 bytes
#define CH11GOaddress               0x7fd58                                     //4 bytes
#define CH12GOaddress               0x7fd5c                                     //4 bytes
#define CH13GOaddress               0x7fd60                                     //4 bytes
#define CH14GOaddress               0x7fd64                                     //4 bytes
#define CH15GOaddress               0x7fd68                                     //4 bytes
#define CH16GOaddress               0x7fd6c                                     //4 bytes
#define CH17GOaddress               0x7fd70                                     //4 bytes
#define CH18GOaddress               0x7fd74                                     //4 bytes
#define CH19GOaddress               0x7fd78                                     //4 bytes
#define CH20GOaddress               0x7fd7c                                     //4 bytes
#define CH21GOaddress               0x7fd80                                     //4 bytes
#define CH22GOaddress               0x7fd84                                     //4 bytes
#define CH23GOaddress               0x7fd88                                     //4 bytes
#define CH24GOaddress               0x7fd8c                                     //4 bytes
#define CH25GOaddress               0x7fd90                                     //4 bytes
#define CH26GOaddress               0x7fd94                                     //4 bytes
#define CH27GOaddress               0x7fd98                                     //4 bytes
#define CH28GOaddress               0x7fd9c                                     //4 bytes  
#define CH29GOaddress               0x7fda0                                     //4 bytes
#define CH30GOaddress               0x7fda4                                     //4 bytes
#define CH31GOaddress               0x7fda8                                     //4 bytes
#define CH32GOaddress               0x7fdac                                     //4 bytes 


//Polynomial Coefficient 'A' Addresses  (0x7fdb0 -> 0x7fe2c)    128 Bytes
#define CH1PolyCoAaddress           0x7fdb0                                     //4 bytes   32 bit float
#define CH2PolyCoAaddress           0x7fdb4                                     //4 bytes
#define CH3PolyCoAaddress           0x7fdb8                                     //4 bytes
#define CH4PolyCoAaddress           0x7fdbc                                     //4 bytes
#define CH5PolyCoAaddress           0x7fdc0                                     //4 bytes
#define CH6PolyCoAaddress           0x7fdc4                                     //4 bytes
#define CH7PolyCoAaddress           0x7fdc8                                     //4 bytes
#define CH8PolyCoAaddress           0x7fdcc                                     //4 bytes
#define CH9PolyCoAaddress           0x7fdd0                                     //4 bytes
#define CH10PolyCoAaddress          0x7fdd4                                     //4 bytes
#define CH11PolyCoAaddress          0x7fdd8                                     //4 bytes
#define CH12PolyCoAaddress          0x7fddc                                     //4 bytes
#define CH13PolyCoAaddress          0x7fde0                                     //4 bytes
#define CH14PolyCoAaddress          0x7fde4                                     //4 bytes
#define CH15PolyCoAaddress          0x7fde8                                     //4 bytes
#define CH16PolyCoAaddress          0x7fdec                                     //4 bytes 
#define CH17PolyCoAaddress          0x7fdf0                                     //4 bytes
#define CH18PolyCoAaddress          0x7fdf4                                     //4 bytes
#define CH19PolyCoAaddress          0x7fdf8                                     //4 bytes
#define CH20PolyCoAaddress          0x7fdfc                                     //4 bytes
#define CH21PolyCoAaddress          0x7fe00                                     //4 bytes
#define CH22PolyCoAaddress          0x7fe04                                     //4 bytes
#define CH23PolyCoAaddress          0x7fe08                                     //4 bytes
#define CH24PolyCoAaddress          0x7fe0c                                     //4 bytes
#define CH25PolyCoAaddress          0x7fe10                                     //4 bytes
#define CH26PolyCoAaddress          0x7fe14                                     //4 bytes
#define CH27PolyCoAaddress          0x7fe18                                     //4 bytes
#define CH28PolyCoAaddress          0x7fe1c                                     //4 bytes
#define CH29PolyCoAaddress          0x7fe20                                     //4 bytes
#define CH30PolyCoAaddress          0x7fe24                                     //4 bytes
#define CH31PolyCoAaddress          0x7fe28                                     //4 bytes 
#define CH32PolyCoAaddress          0x7fe2c                                     //4 bytes


//Polynomial Coefficient 'B' Addresses  (0x7fe30 -> 0x7feac)    128 Bytes
#define CH1PolyCoBaddress           0x7fe30                                     //4 bytes   32 bit float
#define CH2PolyCoBaddress           0x7fe34                                     //4 bytes
#define CH3PolyCoBaddress           0x7fe38                                     //4 bytes
#define CH4PolyCoBaddress           0x7fe3c                                     //4 bytes
#define CH5PolyCoBaddress           0x7fe40                                     //4 bytes
#define CH6PolyCoBaddress           0x7fe44                                     //4 bytes
#define CH7PolyCoBaddress           0x7fe48                                     //4 bytes
#define CH8PolyCoBaddress           0x7fe4c                                     //4 bytes
#define CH9PolyCoBaddress           0x7fe50                                     //4 bytes
#define CH10PolyCoBaddress          0x7fe54                                     //4 bytes
#define CH11PolyCoBaddress          0x7fe58                                     //4 bytes
#define CH12PolyCoBaddress          0x7fe5c                                     //4 bytes
#define CH13PolyCoBaddress          0x7fe60                                     //4 bytes
#define CH14PolyCoBaddress          0x7fe64                                     //4 bytes
#define CH15PolyCoBaddress          0x7fe68                                     //4 bytes
#define CH16PolyCoBaddress          0x7fe6c                                     //4 bytes
#define CH17PolyCoBaddress          0x7fe70                                     //4 bytes
#define CH18PolyCoBaddress          0x7fe74                                     //4 bytes
#define CH19PolyCoBaddress          0x7fe78                                     //4 bytes
#define CH20PolyCoBaddress          0x7fe7c                                     //4 bytes
#define CH21PolyCoBaddress          0x7fe80                                     //4 bytes
#define CH22PolyCoBaddress          0x7fe84                                     //4 bytes
#define CH23PolyCoBaddress          0x7fe88                                     //4 bytes
#define CH24PolyCoBaddress          0x7fe8c                                     //4 bytes
#define CH25PolyCoBaddress          0x7fe90                                     //4 bytes
#define CH26PolyCoBaddress          0x7fe94                                     //4 bytes
#define CH27PolyCoBaddress          0x7fe98                                     //4 bytes
#define CH28PolyCoBaddress          0x7fe9c                                     //4 bytes
#define CH29PolyCoBaddress          0x7fea0                                     //4 bytes
#define CH30PolyCoBaddress          0x7fea4                                     //4 bytes
#define CH31PolyCoBaddress          0x7fea8                                     //4 bytes
#define CH32PolyCoBaddress          0x7feac                                     //4 bytes




//Polynomial Coefficient 'C' Addresses  (0x7feb0 -> 0x 7ff2c)   128 Bytes
#define CH1PolyCoCaddress           0x7feb0                                     //4 bytes   32 bit float
#define CH2PolyCoCaddress           0x7feb4                                     //4 bytes
#define CH3PolyCoCaddress           0x7feb8                                     //4 bytes
#define CH4PolyCoCaddress           0x7febc                                     //4 bytes
#define CH5PolyCoCaddress           0x7fec0                                     //4 bytes
#define CH6PolyCoCaddress           0x7fec4                                     //4 bytes
#define CH7PolyCoCaddress           0x7fec8                                     //4 bytes
#define CH8PolyCoCaddress           0x7fecc                                     //4 bytes
#define CH9PolyCoCaddress           0x7fed0                                     //4 bytes
#define CH10PolyCoCaddress          0x7fed4                                     //4 bytes
#define CH11PolyCoCaddress          0x7fed8                                     //4 bytes
#define CH12PolyCoCaddress          0x7fedc                                     //4 bytes
#define CH13PolyCoCaddress          0x7fee0                                     //4 bytes
#define CH14PolyCoCaddress          0x7fee4                                     //4 bytes
#define CH15PolyCoCaddress          0x7fee8                                     //4 bytes
#define CH16PolyCoCaddress          0x7feec                                     //4 bytes
#define CH17PolyCoCaddress          0x7fef0                                     //4 bytes
#define CH18PolyCoCaddress          0x7fef4                                     //4 bytes
#define CH19PolyCoCaddress          0x7fef8                                     //4 bytes
#define CH20PolyCoCaddress          0x7fefc                                     //4 bytes
#define CH21PolyCoCaddress          0x7ff00                                     //4 bytes
#define CH22PolyCoCaddress          0x7ff04                                     //4 bytes
#define CH23PolyCoCaddress          0x7ff08                                     //4 bytes
#define CH24PolyCoCaddress          0x7ff0c                                     //4 bytes
#define CH25PolyCoCaddress          0x7ff10                                     //4 bytes
#define CH26PolyCoCaddress          0x7ff14                                     //4 bytes
#define CH27PolyCoCaddress          0x7ff18                                     //4 bytes    
#define CH28PolyCoCaddress          0x7ff1c                                     //4 bytes
#define CH29PolyCoCaddress          0x7ff20                                     //4 bytes
#define CH30PolyCoCaddress          0x7ff24                                     //4 bytes
#define CH31PolyCoCaddress          0x7ff28                                     //4 bytes        
#define CH32PolyCoCaddress          0x7ff2c                                     //4 bytes

#define LastMemoryaddress                  0x7ffff                              

                                                                                
#endif
