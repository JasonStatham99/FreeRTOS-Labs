#include <STM32FreeRTOS.h>

const uint8_t LED3_PIN = PA7;
const uint8_t BUZZER_PIN = PB3;

TaskHandle_t xLTaskHandle = NULL;
TaskHandle_t xBTaskHandle = NULL;

void vLTask(void *pvParameters) {
  pinMode(LED3_PIN, OUTPUT);
  for (;;) {
    digitalWrite(LED3_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(225));
    digitalWrite(LED3_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(225));
  }
}

void vBTask(void *pvParameters) {
  pinMode(BUZZER_PIN, OUTPUT);
  for (;;) {
    digitalWrite(BUZZER_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(400));
    digitalWrite(BUZZER_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(400));
  }
}

void vCTask(void *pvParameters) {
  char cmd;
  for (;;) {
    if (Serial.available() > 0) {
      cmd = Serial.read();

      if (cmd == 'B') {
        if (xBTaskHandle == NULL) {
          xTaskCreate(vBTask, "BuzzerTask", 128, NULL, 4, &xBTaskHandle);
        }
      }
      else if (cmd == 'D') {
        if (xBTaskHandle != NULL) {
          vTaskDelete(xBTaskHandle);
          xBTaskHandle = NULL;
          digitalWrite(BUZZER_PIN, LOW);
          pinMode(BUZZER_PIN, INPUT);
        }
      }
      else if (cmd == 'X') {
        if (xLTaskHandle != NULL) {
          vTaskSuspend(xLTaskHandle);
          digitalWrite(LED3_PIN, LOW);
        }
      }
      else if (cmd == 'Y') {
        if (xLTaskHandle != NULL) {
          vTaskResume(xLTaskHandle);
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  Serial.begin(9600);

  xTaskCreate(vLTask, "LedTask", 128, NULL, 5, &xLTaskHandle);
  xTaskCreate(vCTask, "ControlTask", 256, NULL, 6, NULL);

  vTaskStartScheduler();
}

void loop() {
}