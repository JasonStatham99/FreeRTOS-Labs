#include <STM32FreeRTOS.h>
#include <FreeRTOSConfig.h>

#define S2 PA4
#define S3 PB0

QueueHandle_t xQueue;

int Sum = 0;

void vButtonTask(void *pvParameters)
{
    bool lastS3 = HIGH;

    for (;;)
    {
        if (digitalRead(S2) == LOW)
        {
            vTaskDelay(pdMS_TO_TICKS(30));

            if (digitalRead(S2) == LOW)
            {
                Sum++;

                if (xQueue != NULL)
                {
                    if (uxQueueMessagesWaiting(xQueue) >= 9)
                    {
                        Serial.println("Очередь переполнена");
                    }
                    else
                    {
                        xQueueSend(xQueue, &Sum, 0);
                    }
                }

                while (digitalRead(S2) == LOW)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }

                vTaskDelay(pdMS_TO_TICKS(30));
            }
        }

        bool currentS3 = digitalRead(S3);

        if (lastS3 == HIGH && currentS3 == LOW)
        {
            taskENTER_CRITICAL();

            if (xQueue != NULL)
            {
                vQueueDelete(xQueue);
                xQueue = NULL;
            }

            xQueue = xQueueCreate(9, sizeof(int));

            taskEXIT_CRITICAL();

            if (xQueue != NULL)
            {
                Serial.println("Очередь удалена");
            }
            else
            {
                Serial.println("Ошибка создания очереди");
            }

            vTaskDelay(pdMS_TO_TICKS(50));
        }

        lastS3 = currentS3;

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void vReceiverTask(void *pvParameters)
{
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(6000));

        if (xQueue != NULL)
        {
            int receivedValue;
            bool received = false;

            while (xQueueReceive(xQueue, &receivedValue, 0) == pdPASS)
            {
                Serial.print("Получено: ");
                Serial.println(receivedValue);
                received = true;
            }

            if (!received)
            {
                Serial.println("Очередь пуста");
            }
        }
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(S2, INPUT_PULLUP);
    pinMode(S3, INPUT_PULLUP);

    xQueue = xQueueCreate(9, sizeof(int));

    if (xQueue == NULL)
    {
        Serial.println("Ошибка создания очереди");

        while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    xTaskCreate(vButtonTask, "Buttons", 256, NULL, 2, NULL);
    xTaskCreate(vReceiverTask, "Receiver", 256, NULL, 1, NULL);

    vTaskStartScheduler();
}

void loop()
{
}