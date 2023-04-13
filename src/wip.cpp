/*
Trying to parse strings from UART
Not working right now, issues with a lot of things

*/
// /*
// TODO:
// - Implement mutexes to protect the queue
// - Implement Serial Monitor to send data to output


// */
// #include <FreeRTOS_TEENSY4.h>
// #include <task.h>
// #include <queue.h>
// #include <SerialLogs.h>


// // Defines the size of the queue and each item in the queue
// #define xQUEUE_SIZE 100
// #define xQUEUE_LENGTH sizeof(uint32_t)
// #define SERIAL2_TX_PIN 8
// #define SERIAL2_RX_PIN 7

// // Instantiate queues to pass message from receiveTask to sendTask
// QueueHandle_t xQueue;
// QueueHandle_t xWriteQueue;


// // FreeRTOS Tasks
// void sendTask(void* parameter);
// void receiveTask(void* parameter);
// void blinkLEDForeverTask(void* parameter);
// void receiveUSBTask(void* parameter);
// void processStringTask(void *parameter);

// void configSerialUART ();
// String readAString(size_t max, uint32_t DelayMs);


// void setup() {
//   // Start serial communication on Serial port (USB) for debugging
//   Serial.begin(115200);

//   while(!Serial); // Wait for serial monitor to connect

//   Serial.println("Serial has been opened.");
//   configSerialUART();
//   pinMode(LED_BUILTIN, OUTPUT);

//   // Create the queue
//   xQueue = xQueueCreate(xQUEUE_SIZE, xQUEUE_LENGTH);
//   xWriteQueue = xQueueCreate(xQUEUE_SIZE, xQUEUE_LENGTH);

//   // Create the send and receive tasks
//   xTaskCreate(sendTask, "SendTask", 1024, NULL, 1, NULL);
//   xTaskCreate(receiveTask, "ReceiveTask", 1024, NULL, 1, NULL);
//   xTaskCreate(receiveUSBTask, "ReceiveUSBTask", 1024, NULL, 1, NULL);
//   xTaskCreate(processStringTask, "ProcessStringTask", 1024, NULL, 1, NULL);
//   xTaskCreate(blinkLEDForeverTask, "BlinkLED", 1024, NULL, 1, NULL);

//   vTaskStartScheduler();
//   Serial.println("Insufficient RAM");
//   while (1)
//     ;
// }

// void loop() {
//   // Nothing to do here when using FreeRTOS
// }

// void configSerialUART () {
//   // Wait for the CLI interface to be available

//   Serial2.begin(115200);

//   while (!Serial2);

//   Serial.write("[CONFIG]: Serial 2 is up\n");
//   Serial.write("[CONFIG]: Start sending data, will echo any received data\n");
// }


// /**
//  * @brief Attempts to read a string from the queue, with a timeout.
//  * Stops at a '\n' character or when the max length is reached.
//  * 
//  * @return Returns the string including the newline character 
//  *         or an incomplete string if the timeout is reached.
//  */
// String readAString(size_t max, uint32_t DelayMs)
// {
//   String str = "";
//   size_t length = 0;
//   uint8_t inputByte = 0;
//   uint32_t remainingTime = DelayMs;

//   char retVal = 0;

//   // Convert remainingTime to ticks
//   const uint32_t delayTicks = DelayMs / portTICK_PERIOD_MS;
//   const uint32_t startingTime = xTaskGetTickCount();
//   uint32_t endingTime = startingTime + delayTicks; 

//   while (length < max)
//   {
//     if (DelayMs != portMAX_DELAY)
//     {
//       remainingTime = endingTime - xTaskGetTickCount();
//     }

//     retVal = xQueueReceive(xQueue, &inputByte, remainingTime);
//     if (retVal != pdTRUE || inputByte == '\n') // If the queue is empty or a newline character is received, will break and return str
//         break;
//     str += (char)inputByte;
//     length++;
//   }

//   return str;
// }

// // --------------Tasks----------------

// void blinkLEDForeverTask(void* parameter)
// {
//   while (true)
//   {
//     digitalWrite(LED_BUILTIN, HIGH);
//     vTaskDelay(pdMS_TO_TICKS(1000));
//     digitalWrite(LED_BUILTIN, LOW);
//     vTaskDelay(pdMS_TO_TICKS(1000));
//   }
// }




// /**
//  * @brief FreeRTOS Tasks that sends any data received from the Rx queue to the Serial2 port
//  * Essentially echos any received data
//  * 
//  * @param parameter Not Used
//  */
// void sendTask(void* parameter) {
  

//   String incomingString = "";
//   byte incomingByte = 0;

//   while (true) {

//     // Waits for any messages to appear in the xQueue. (From the receive task)
//     // If something is in the queue send write to Serial2 Port, otherwise wait 10ms
//     if (xQueueReceive(xWriteQueue, &incomingString, pdMS_TO_TICKS(10)) == pdTRUE) {

//       incomingString += '\n'; // Adds a newline character to the end of the string
//       for (int i = 0; i < incomingString.length(); i++)
//       {
//         incomingByte = incomingString[i];
//         Serial2.write(incomingByte);
//       }
//       Serial.printf("Sent: %s\n", incomingString.c_str()); 

//     }

//     vTaskDelay(pdMS_TO_TICKS(10)); // Give other tasks a chance to run
//   }
// }


// /**
//  * @brief FreeRTOS Task that reads any data received from the Serial2 port, prints it to the Serial (USB) port,
//  * and then sends it to the xQueue to be sent to the sendTask
//  * 
//  * @param parameter 
//  */

// void receiveTask(void* parameter) {
//   char c = 0;

//   while (true) {
//     // Read all available characters from the Serial2 port 
//     if (Serial2.available()) {
//       c = Serial2.read();
//       xQueueSend(xQueue, &c, pdMS_TO_TICKS(10)); // Sends the char to the queue
//     }

//     vTaskDelay(pdMS_TO_TICKS(10)); // Give other tasks a chance to run

//   }

// }

// void processStringTask(void *parameter)
// {
//   String str = "";
//   while (true)
//   {
//     if (Serial2.available())
//       str = readAString(100, 1000); // Waits 1 seconds to timeout
//     if (str.length() != 0) 
//     {
//       Serial.printf("Received: %s\n", str.c_str());
//       xQueueSend(xWriteQueue, &str, pdMS_TO_TICKS(10));
//     }

//     vTaskDelay(pdMS_TO_TICKS(10)); // Give other tasks a chance to run
//   }
// }
// /**
//  * @brief Receives any data sent from the Serial Monitor (USB) and echos it back.
//  * 
//  * @param parameter 
//  */
// void receiveUSBTask(void* parameter) {

//   while (true) {
//     // Read all available characters from the Serial port 
//     if (Serial.available()) {
//       String str = Serial.readString(); // Reads until a \n character is found
//       Serial.print(str);
//     }

//     vTaskDelay(pdMS_TO_TICKS(10)); // Give other tasks a chance to run

//   }

// }

