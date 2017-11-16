#ifndef AD5241b_H
#define AD5241b_H



//************************PROTOTYPES*************************************
void write_AD5241(unsigned char tap);                                           //Write 8 bit value to the AD5241 digital pot
unsigned char read_AD5241(void);                                                //Read 8 bit value from AD5241 digital pot

#endif
