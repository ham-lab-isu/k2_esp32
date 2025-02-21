#include <Arduino.h>

#define SERIAL_BAUDRATE 115200

void handleSetDigital(String);

void handleReadDigital(String);

void handleReadAnalog(String);


void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    while (!Serial) { delay(10); } // Wait for Serial to be ready
    Serial.println("ESP32-S3 Ready");
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');  // Read command until newline
        command.trim();  // Remove leading/trailing whitespace
        
        if (command.startsWith("SET D")) {
            handleSetDigital(command);
        } else if (command.startsWith("READ D")) {
            handleReadDigital(command);
        } else if (command.startsWith("READ A")) {
            handleReadAnalog(command);
        } else {
            Serial.println("ERR Invalid Command");
        }
    }
}

void handleSetDigital(String cmd) {
    int pin, value;
    if (sscanf(cmd.c_str(), "SET D %d %d", &pin, &value) == 2) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, value);
        Serial.println("OK");
    } else {
        Serial.println("ERR Syntax: SET D <pin> <0/1>");
    }
}

void handleReadDigital(String cmd) {
    int pin;
    if (sscanf(cmd.c_str(), "READ D %d", &pin) == 1) {
        pinMode(pin, INPUT);
        int state = digitalRead(pin);
        Serial.printf("OK %d\n", state);
    } else {
        Serial.println("ERR Syntax: READ D <pin>");
    }
}

void handleReadAnalog(String cmd) {
    int pin;
    if (sscanf(cmd.c_str(), "READ A %d", &pin) == 1) {
        int value = analogRead(pin);
        Serial.printf("OK %d\n", value);
    } else {
        Serial.println("ERR Syntax: READ A <pin>");
    }
}
