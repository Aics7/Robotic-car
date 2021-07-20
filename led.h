#ifndef LED_H
#define LED_H
void initialize_leds(void);
void RBBBR(void);
void  on_tail_red(void);
void  on_tail_white(void);
void  off_tail_light(void);
void reverse_beep(void);
void off_buzzer(void);
void beep_twice(void);
extern int inc_stop_count(int stop_count);
#endif
