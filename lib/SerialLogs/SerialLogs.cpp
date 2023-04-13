/*
 * Library to print logs using mutex.
 */
#include "SerialLogs.h"

xSemaphoreHandle xMutex;

void initSerialLogs(void){
    Serial.begin(115200);
    xMutex = xSemaphoreCreateMutex();
}
void serialLog(const char *format){
    double time;
    time = double(millis())/1000;
    if (xSemaphoreTake( xMutex, portMAX_DELAY )==pdTRUE){
        Serial.print("[");
        Serial.print(time,2);
        Serial.print("] ");
        Serial.println(format);
        xSemaphoreGive(xMutex);
    }
}