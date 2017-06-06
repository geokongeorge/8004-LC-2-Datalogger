#ifndef FRAM_ADDRESSA_H
#define FRAM_ADDRESSA_H

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
                                                                                
#endif
