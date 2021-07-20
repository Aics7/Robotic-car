#ifndef BLUETOOTH_H
#define BLUETOOTH_H
void init_pins(void);
void init_UART(void);
void doTxRx_task(void);
char get_bluetooth_message(void);
void bluetooth_init(void);

int get_forward(void);;
int get_reverse(void);
int get_left(void);
int get_right(void);
int get_stop(void);
int get_fast(void);
int get_slow(void);
int get_accelerate(void);
int get_automatic();

void set_forward(void);
void set_reverse(void);
void set_left(void);;
void set_right(void);
void set_stop(void);
void set_fast(void);;
void  set_slow(void);
void set_accelerate(void);
void set_automatic(void);

void Init_RGB_LEDs(void);
void lightLED(void);
void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on);
#endif
