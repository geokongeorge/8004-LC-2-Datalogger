#ifndef LC8004delay_b_H
#define LC8004delay_b_H
void delay(unsigned long);						//delay function
void OneMinuteTimer(void);						//1 minute power-off timer
void shutdownTimer(unsigned int timerseconds);
void shutdownTimerwithReset(unsigned int timerseconds);
void stopshutdownTimer(void);
void StopOneMinuteTimer(void);					//Stop the 1 minute power-off timer
#endif
