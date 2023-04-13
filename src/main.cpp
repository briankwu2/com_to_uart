/*
TODO:
- Implement mutexes to protect the queue



*/
#include <FreeRTOS_TEENSY4.h>
#include <task.h>
#include <queue.h>
#include <SerialLogs.h>


// Defines the size of the queue and each item in the queue
#define xQUEUE_SIZE 100
#define xQUEUE_LENGTH sizeof(uint32_t)
#define SERIAL2_TX_PIN 8
#define SERIAL2_RX_PIN 7
// Instantiate queues to pass message from receiveTask to sendTask
QueueHandle_t xQueue;



// FreeRTOS Tasks
void sendTask(void* parameter);
void receiveTask(void* parameter);
void blinkLEDForeverTask(void* parameter);
void receiveUSBTask(void* parameter);

void configSerialUART ();
void flushSerial();



void setup() {
  // Start serial communication on Serial port (USB) for debugging
  Serial.begin(115200);

  while(!Serial); // Wait for serial monitor to connect

  Serial.println("Serial has been opened.");
  configSerialUART();
  pinMode(LED_BUILTIN, OUTPUT);

  // Create the queue
  xQueue = xQueueCreate(xQUEUE_SIZE, xQUEUE_LENGTH);

  // Create the send and receive tasks
  xTaskCreate(sendTask, "SendTask", 1024, NULL, 1, NULL);
  xTaskCreate(receiveTask, "ReceiveTask", 1024, NULL, 1, NULL);
  xTaskCreate(receiveUSBTask, "ReceiveUSBTask", 1024, NULL, 1, NULL);
  xTaskCreate(blinkLEDForeverTask, "BlinkLED", 1024, NULL, 1, NULL);

  vTaskStartScheduler();
  Serial.println("Insufficient RAM");
  while (1)
    ;
}

void loop() {
  // Nothing to do here when using FreeRTOS
}

void configSerialUART () {
  // Wait for the CLI interface to be available

  Serial2.begin(115200);

  while (!Serial2);

  Serial.write("[CONFIG]: Serial 2 is up\n");

}


// --------------Tasks----------------


// Used to blink the LED to show that the program is running
void blinkLEDForeverTask(void* parameter)
{
  while (true)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000));
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}




/**
 * @brief FreeRTOS Task that sends any data received from the Rx queue to the Serial2 port
 * 
 * @param parameter Not Used
 */
void sendTask(void* parameter) {
  

  uint8_t incomingByte = 0;  

  while (true) {

    // Waits for any messages to appear in the xQueue. (From the receive task)
    // If something is in the queue send write to Serial2 Port, otherwise wait 10ms
    if (xQueueReceive(xQueue, &incomingByte, pdMS_TO_TICKS(10)) == pdTRUE) {
      Serial2.write(incomingByte);
    }


    vTaskDelay(pdMS_TO_TICKS(10)); // Give other tasks a chance to run

  }
}


/**
 * @brief FreeRTOS Task that reads any data received from the Serial2 port, prints it to the Serial (USB) port,
 * and then sends it to the xQueue to be sent to the sendTask
 * 
 * @param parameter 
 */

void receiveTask(void* parameter) {

  while (true) {

    // Read all available characters from the Serial2 port  (blocking)
    while (Serial2.available()) {
      char c = Serial2.read(); // Reads a character
      Serial.print(c); // Sends to Serial Monitor
    }

    // Not needed? Causes issues with sending however, unsure why
    // flushSerial(); // Prevents data sent to Serial to be received by receiveUSBTask

    vTaskDelay(pdMS_TO_TICKS(10)); // Give other tasks a chance to run

  }

}


/**
 * @brief Receives any data sent from the Serial Monitor (USB) and echos it back.
 * 
 * @param parameter 
 */
void receiveUSBTask(void* parameter) {

  while (true) {
    // Read all available characters from the Serial port 
    if (Serial.available()) {
      String str = Serial.readString(); // Reads until a \n character is found

      // Sends in the form of bytes
      for (int i = 0; i < str.length(); i++) {
        xQueueSend(xQueue, &str[i], pdMS_TO_TICKS(10)); // Sends the char to the queue
      }
      xQueueSend(xQueue, "\n", pdMS_TO_TICKS(10)); // Adds "\n" to the end of the string

    }

    vTaskDelay(pdMS_TO_TICKS(10)); // Give other tasks a chance to run

  }
}

// Helps flush the serial port to prevent looping data from being sent
void flushSerial() {
  while (Serial.available()) {
    Serial.read();
  }
}

