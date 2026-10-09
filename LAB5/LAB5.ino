#include <STM32FreeRTOS.h>
#include <FreeRTOSConfig.h>
#include <semphr.h>

// Светодиоды
const uint8_t LED1 = PA5;
const uint8_t LED2 = PA6;
const uint8_t LED3 = PA7;
const uint8_t LED4 = PB6;

// Кнопки
const uint8_t S2 = PA4;
const uint8_t S3 = PB0;

// Максимальное количество посетителей
#define MAX_VISITORS 12

// Счетный семафор посетителей
SemaphoreHandle_t xVisitorSemaphore;

// Семафор для Задачи 3
SemaphoreHandle_t xPrintSemaphore;

// Прототипы задач
void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);
void Task4(void *pvParameters);

void setup()
{
  Serial.begin(9600);

  // Настройка светодиода LED3
  pinMode(LED3, OUTPUT);
  digitalWrite(LED3, LOW);

  // Настройка кнопок
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);

  // Создание счетного семафора:
  // максимум = 12, начальное значение = 0
  xVisitorSemaphore = xSemaphoreCreateCounting(
    MAX_VISITORS,
    0
  );

  // Семафор для передачи события Задаче 3
  xPrintSemaphore = xSemaphoreCreateBinary();

  // Задача 1 - вход
  xTaskCreate(
    Task1,
    "Entry",
    128,
    NULL,
    1,
    NULL
  );

  // Задача 2 - выход
  xTaskCreate(
    Task2,
    "Exit",
    128,
    NULL,
    1,
    NULL
  );

  // Задача 3 - вывод информации
  xTaskCreate(
    Task3,
    "Print",
    128,
    NULL,
    1,
    NULL
  );

  // Задача 4 - индикация максимума
  xTaskCreate(
    Task4,
    "LED3",
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

// Задача 1
// Вход посетителя через S2
void Task1(void *pvParameters)
{
  for (;;)
  {
    if (digitalRead(S2) == LOW)
    {
      // Проверяем, не достигнут ли максимум
      if (uxSemaphoreGetCount(xVisitorSemaphore) < MAX_VISITORS)
      {
        // Увеличиваем счетчик на 1
        xSemaphoreGive(xVisitorSemaphore);

        // Передаем событие Задаче 3
        xSemaphoreGive(xPrintSemaphore);
      }

      // Защита от дребезга
      vTaskDelay(pdMS_TO_TICKS(200));

      // Ждем отпускания кнопки
      while (digitalRead(S2) == LOW)
      {
        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Задача 2
// Выход посетителя через S3
void Task2(void *pvParameters)
{
  for (;;)
  {
    if (digitalRead(S3) == LOW)
    {
      // Забираем один элемент, если посетители есть
      if (uxSemaphoreGetCount(xVisitorSemaphore) > 0)
      {
        xSemaphoreTake(xVisitorSemaphore, 0);

        // Передаем событие Задаче 3
        xSemaphoreGive(xPrintSemaphore);
      }

      // Защита от дребезга
      vTaskDelay(pdMS_TO_TICKS(200));

      // Ждем отпускания кнопки
      while (digitalRead(S3) == LOW)
      {
        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// Задача 3
// Вывод количества посетителей
void Task3(void *pvParameters)
{
  for (;;)
  {
    // Ожидаем событие от Задачи 1 или Задачи 2
    if (xSemaphoreTake(xPrintSemaphore, portMAX_DELAY) == pdTRUE)
    {
      int visitors = uxSemaphoreGetCount(xVisitorSemaphore);

      Serial.print("Количество людей на мосту: ");
      Serial.println(visitors);
    }
  }
}

// Задача 4
// Световая индикация при максимальном количестве
void Task4(void *pvParameters)
{
  for (;;)
  {
    int visitors = uxSemaphoreGetCount(xVisitorSemaphore);

    if (visitors >= MAX_VISITORS)
    {
      // LED3 мигает с периодом 0,5 секунды
      digitalWrite(LED3, !digitalRead(LED3));

      vTaskDelay(pdMS_TO_TICKS(500));
    }
    else
    {
      // Если максимум не достигнут - LED3 выключен
      digitalWrite(LED3, LOW);

      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}
