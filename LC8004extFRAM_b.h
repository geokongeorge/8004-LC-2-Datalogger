#ifndef LC8004extFRAM_b_H
#define LC8004extFRAM_b_H



//************************PROTOTYPES*************************************
void displayError(unsigned int fram);
unsigned int read_Int_FRAM(unsigned long absAdd);		
void read_Flt_FRAM(unsigned long absAdd, unsigned int x);	
long read_longFRAM(unsigned int add);
void write_Int_FRAM(unsigned long absAdd, unsigned int data);
void write_Flt_FRAM(unsigned long absAdd, float f);
void write_longFRAM(long f, unsigned int add);

//void selectBank_write(unsigned long absAdd);
//void selectBank_read(unsigned long absAdd);
void selectBank(unsigned long absAdd, unsigned char x);
void sleepFRAM(void);
unsigned int testFRAM(unsigned int);                        //was testExternalFRAM())
#endif
