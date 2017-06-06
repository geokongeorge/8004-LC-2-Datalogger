#ifndef LC8004extFRAM_c_H
#define LC8004extFRAM_c_H



//************************PROTOTYPES*************************************
void displayError(unsigned int fram);
unsigned int read_Int_FRAM(unsigned long absAdd);		
void read_Flt_FRAM(unsigned long absAdd, unsigned int x);	
//long read_longFRAM(unsigned int add);                                         //REM REV C
long read_longFRAM(unsigned long add);                                          //REV C
void write_Int_FRAM(unsigned long absAdd, unsigned int data);
void write_Flt_FRAM(unsigned long absAdd, float f);
//void write_longFRAM(long f, unsigned int add);                                //REM REV C
void write_longFRAM(long f, unsigned long add);                                 //REV C

//void selectBank_write(unsigned long absAdd);
//void selectBank_read(unsigned long absAdd);
void selectBank(unsigned long absAdd, unsigned char x);
void sleepFRAM(void);
unsigned int testFRAM(unsigned int);                        //was testExternalFRAM())
#endif
