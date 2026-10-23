#include <STM32FreeRTOS.h>
#include <FreeRTOSConfig.h>

#define S1 PA1
#define S2 PA4
#define S3 PB0
#define QUEUE_LENGTH 9

QueueHandle_t xQueue = NULL;
TaskHandle_t xReceiverHandle = NULL;

int Sum = 0;

void vButtonTask(void *pvParameters)
{
    bool lastS1 = HIGH;
    bool lastS3 = HIGH;

    for (;;)
    {
        bool currentS1 = digitalRead(S1);

        if (lastS1 == HIGH && currentS1 == LOW)
        {
            vTaskDelay(pdMS_TO_TICKS(30));

            if (digitalRead(S1) == LOW)
            {
                if (xQueue == NULL)
                {
                    xQueue = xQueueCreate(QUEUE_LENGTH, sizeof(int));

                    if (xQueue != NULL)
                    {
                        Sum = 0;
                        Serial.println("Очередь создана");
                    }
                    else
                    {
                        Serial.println("Ошибка создания очереди");
                    }
                }
                else
                {
                    Serial.println("Очередь уже существует");
                }

                while (digitalRead(S1) == LOW)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }

        lastS1 = digitalRead(S1);

        if (digitalRead(S2) == LOW)
        {
            vTaskDelay(pdMS_TO_TICKS(30));

            if (digitalRead(S2) == LOW)
            {
                if (xQueue == NULL)
                {
                    Serial.println("Очередь не создана");
                }
                else if (Sum >= QUEUE_LENGTH)
                {
                    Serial.println("Очередь переполнена");
                }
                else
                {
                    int nextValue = Sum + 1;

                    if (xQueueSend(xQueue, &nextValue, 0) == pdPASS)
                    {
                        Sum = nextValue;
                        Serial.print("Отправлено: ");
                        Serial.println(Sum);
                    }
                    else
                    {
                        Serial.println("Очередь переполнена");
                    }
                }

                while (digitalRead(S2) == LOW)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }

        bool currentS3 = digitalRead(S3);

        if (lastS3 == HIGH && currentS3 == LOW)
        {
            vTaskDelay(pdMS_TO_TICKS(30));

            if (digitalRead(S3) == LOW)
            {
                if (xQueue != NULL)
                {
                    vTaskSuspend(xReceiverHandle);

                    vQueueDelete(xQueue);
                    xQueue = NULL;
                    Sum = 0;

                    Serial.println("Очередь удалена");

                    vTaskResume(xReceiverHandle);
                }
                else
                {
                    Serial.println("Очередь уже удалена");
                }

                while (digitalRead(S3) == LOW)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
            }
        }

        lastS3 = digitalRead(S3);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void vReceiverTask(void *pvParameters)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(6000));

        bool received = false;
        int receivedValue;

        while (xQueue != NULL &&
               xQueueReceive(xQueue, &receivedValue, 0) == pdPASS)
        {
            Serial.print("Получено: ");
            Serial.println(receivedValue);
            received = true;
        }

        if (xQueue != NULL && !received)
        {
            Serial.println("Очередь пуста");
        }
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(S1, INPUT_PULLUP);
    pinMode(S2, INPUT_PULLUP);
    pinMode(S3, INPUT_PULLUP);

    xQueue = NULL;

    xTaskCreate(
        vButtonTask,
        "Buttons",
        256,
        NULL,
        2,
        NULL
    );

    xTaskCreate(
        vReceiverTask,
        "Receiver",
        256,
        NULL,
        1,
        &xReceiverHandle
    );

    vTaskStartScheduler();
}

void loop()
{
}
