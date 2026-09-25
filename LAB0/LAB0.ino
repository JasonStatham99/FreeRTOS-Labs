#include <STM32FreeRTOS.h>
// Светодиод LED1 подключен к порту PA5
const uint8_t LED1 = PA5;
//------------------------------------------------------------------------------
static void vLEDBlinkTask(void *pvParameters) {
pinMode(LED1, OUTPUT);
for (;;) {
// Включение LED1
digitalWrite(LED1, HIGH);
// Задержка на 0,5 секунды
vTaskDelay(500);
// Выключение LED1
digitalWrite(LED1, LOW);
// Задержка на 0,5 секунды
vTaskDelay(500);
  }
}
//------------------------------------------------------------------------------
void setup() {
// создание задачи мигания светодиода
xTaskCreate(vLEDBlinkTask,
"Task1",
configMINIMAL_STACK_SIZE + 50,
NULL,
tskIDLE_PRIORITY + 2,
NULL);
// запуск планировщика FreeRTOS
vTaskStartScheduler();
while(1);
}
void loop() {
while(1) {
}
}