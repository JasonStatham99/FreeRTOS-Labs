#include <STM32FreeRTOS.h>
#include <FreeRTOSConfig.h>

const uint8_t LED1_PIN = PA5;
const uint8_t LED2_PIN = PA6;
const uint8_t LED3_PIN = PA7;
const uint8_t LED4_PIN = PB6;

const uint8_t S3_PIN = PB0;

#define LED_TASK_STACK_SIZE 256
#define BUTTON_TASK_STACK_SIZE 256
#define SERIAL_TASK_STACK_SIZE 512

TaskHandle_t xLedTaskHandle = NULL;
TaskHandle_t xButtonTaskHandle = NULL;
TaskHandle_t xSerialTaskHandle = NULL;

StaticTask_t xSerialTaskTCB;
StackType_t xSerialTaskStack[SERIAL_TASK_STACK_SIZE];

const uint8_t ledSequence[] = {
  LED4_PIN,
  LED2_PIN,
  LED3_PIN,
  LED1_PIN,
  LED2_PIN,
  LED3_PIN,
  LED4_PIN
};

const uint8_t SEQUENCE_LENGTH = 7;

void vLedTask(void *pvParameters) {
  for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
    pinMode(ledSequence[i], OUTPUT);
  }

  for (;;) {
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, LOW);
      digitalWrite(LED3_PIN, LOW);
      digitalWrite(LED4_PIN, LOW);

      digitalWrite(ledSequence[i], HIGH);

      vTaskDelay(pdMS_TO_TICKS(125));
    }
  }
}

void vButtonTask(void *pvParameters) {
  pinMode(S3_PIN, INPUT_PULLUP);

  bool ledTaskRunning = true;
  bool lastButtonState = HIGH;

  for (;;) {
    bool currentButtonState = digitalRead(S3_PIN);

    if (lastButtonState == HIGH && currentButtonState == LOW) {
      if (ledTaskRunning) {
        vTaskSuspend(xLedTaskHandle);

        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
        digitalWrite(LED3_PIN, LOW);
        digitalWrite(LED4_PIN, LOW);

        ledTaskRunning = false;
      } else {
        vTaskResume(xLedTaskHandle);

        ledTaskRunning = true;
      }
    }

    lastButtonState = currentButtonState;

    vTaskDelay(pdMS_TO_TICKS(400));
  }
}

void vSerialTask(void *pvParameters) {
  for (;;) {
    Serial.println();
    Serial.println("===== FreeRTOS Stack High Water Mark =====");

    Serial.print("LedTask:     ");
    Serial.print(uxTaskGetStackHighWaterMark(xLedTaskHandle));
    Serial.println(" words");

    Serial.print("ButtonTask:  ");
    Serial.print(uxTaskGetStackHighWaterMark(xButtonTaskHandle));
    Serial.println(" words");

    Serial.print("SerialTask:  ");
    Serial.print(uxTaskGetStackHighWaterMark(xSerialTaskHandle));
    Serial.println(" words");

    Serial.println("==========================================");

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("START");
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);

  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  digitalWrite(LED4_PIN, LOW);

  xTaskCreate(
    vLedTask,
    "LedTask",
    LED_TASK_STACK_SIZE,
    NULL,
    3,
    &xLedTaskHandle);

  xTaskCreate(
    vButtonTask,
    "ButtonTask",
    BUTTON_TASK_STACK_SIZE,
    NULL,
    2,
    &xButtonTaskHandle);

  xSerialTaskHandle = xTaskCreateStatic(
    vSerialTask,
    "SerialTask",
    SERIAL_TASK_STACK_SIZE,
    NULL,
    1,
    xSerialTaskStack,
    &xSerialTaskTCB);

  vTaskStartScheduler();
}

void loop() {
}
