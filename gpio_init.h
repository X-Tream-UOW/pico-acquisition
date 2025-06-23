#ifndef GPIO_INIT_H
#define GPIO_INIT_H

#define READY_PIN 21
#define ACK_PIN   26

void init_gpios(void);
void wait_for_polling(void);

#endif
