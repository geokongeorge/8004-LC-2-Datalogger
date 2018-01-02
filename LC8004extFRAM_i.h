#ifndef LC8004extFRAM_i_H
#define LC8004extFRAM_i_H



//************************PROTOTYPES*************************************
void displayError(unsigned int fram);
unsigned int read_Int_FRAM(unsigned long absAdd);		
void read_Flt_FRAM(unsigned long absAdd, unsigned int x);	
float read_float(unsigned long add);                                            
unsigned long read_longFRAM(unsigned long add);                                 
void write_Int_FRAM(unsigned long absAdd, unsigned int data);                 
void write_Flt_FRAM(unsigned long absAdd, float f);                           
void write_longFRAM(unsigned long f, unsigned long add);                        
unsigned char selectBank(unsigned long absAdd, unsigned char x);                
void sleepFRAM(void);
unsigned char testNACK(void);                                                    
unsigned int testFRAM(unsigned int);                        
#endif
