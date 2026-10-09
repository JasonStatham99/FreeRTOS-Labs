#include <STM32FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <semphr.h>

// Светодиоды
const uint8_t LED1 = PA5;
const uint8_t LED2 = PA6;
const uint8_t LED3 = PA7;
const uint8_t LED4 = PB6;

// Кнопка S1
const uint8_t S1 = PA1;

// Семафоры
SemaphoreHandle_t xSemaphoreTask1;
SemaphoreHandle_t xSemaphoreTask2;

// Прототипы задач и обработчика прерывания
void Task1(void *pvParameters);
void Task2(void *pvParameters);
void buttonInterruptHandler();

void setup()
{
  Serial.begin(9600);

  // Настройка светодиодов
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  // Настройка кнопки S1
  pinMode(S1, INPUT_PULLUP);

  // Создание бинарных семафоров
  xSemaphoreTask1 = xSemaphoreCreateBinary();
  xSemaphoreTask2 = xSemaphoreCreateBinary();

  // Внешнее прерывание от S1
  attachInterrupt(
    digitalPinToInterrupt(S1),
    buttonInterruptHandler,
    FALLING
  );

  // Создание Задачи 1
  xTaskCreate(
    Task1,
    "Task1",
    128,
    NULL,
    1,
    NULL
  );

  // Создание Задачи 2
  xTaskCreate(
    Task2,
    "Task2",
    128,
    NULL,
    1,
    NULL
  );

  // Запуск планировщика
  vTaskStartScheduler();
}

void loop()
{
}

// Обработчик внешнего прерывания S1
void buttonInterruptHandler()
{
  // Передаем семафор Задаче 1
  xSemaphoreGiveFromISR(xSemaphoreTask1, NULL);
}

// Задача 1
void Task1(void *pvParameters)
{
  for (;;)
  {
    // Ожидаем семафор от прерывания
    if (xSemaphoreTake(xSemaphoreTask1, portMAX_DELAY) == pdTRUE)
    {
      // 2 раза по очереди включаем LED1 и LED2
      for (int i = 0; i < 2; i++)
      {
        // LED1
        digitalWrite(LED1, HIGH);
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(LED1, LOW);

        // LED2
        digitalWrite(LED2, HIGH);
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(LED2, LOW);
      }

      // После завершения передаем семафор Задаче 2
      xSemaphoreGive(xSemaphoreTask2);
    }
  }
}

// Задача 2
void Task2(void *pvParameters)
{
  for (;;)
  {
    // Ожидаем семафор от Задачи 1
    if (xSemaphoreTake(xSemaphoreTask2, portMAX_DELAY) == pdTRUE)
    {
      Serial.println("Ввод данных завершен");
    }
  }
}
