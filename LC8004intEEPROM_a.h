#ifndef LC8004intEEPROM_a_H
#define LC8004intEEPROM_a_H


//************************PROTOTYPES*************************************
//void pushW(void);
//void popW(void);
unsigned int read_intEEPROM(unsigned int add);
unsigned int Struc2Reg(unsigned int add);
int write_intEEPROM(unsigned int add, unsigned int data);
int read_signedintEEPROM(unsigned int add);
unsigned int testInternalEEPROM(void);
void write_floatEEPROM(float f, unsigned int add);
float read_floatEEPROM(unsigned int add);
void write_longEEPROM(long f, unsigned int add);
long read_longEEPROM(unsigned int add);
#endif
