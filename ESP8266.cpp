#include "ESP8266.h"

//TODO: take serial as parameter
//TODO: delegate this part outside of program
bool ESP8266::init() {
    Serial1.begin(115200);
    sendCommand(TOGGLE_ECHO, "0"); //turns off command echo
    if (readReply() != OK) {
        return false;
    }
    sendCommand(CHECK_MODULE); //sends AT command
    if (readReply() != OK) {
        return false;
    }
    sendCommand(CHANGE_WIFI_MODE, "1"); //changes to station mode
    if (readReply() != OK) {
        return false;
    }
    sendCommand(CHANGE_CON_MODE, "1"); //changes to multiple connection mode
    if (readReply() != OK) {
        return false;
    }
    return true;
}

void ESP8266::sendCommand(Command cmd, const char* param) {
    char cmdBuffer[64]; //create a var to store the command
    strcpy_P(cmdBuffer, (char*) pgm_read_word(&(MEM_COMMANDS[cmd]))); //copy cmd from program memory
    Serial1.write(cmdBuffer); //write the cmd
    Serial1.write(param); //write the cmd parameters
    Serial1.write("\r\n"); //end the cmd

    Serial.print("Sent: ");
    Serial.print(cmdBuffer);
    Serial.println(param);
}

ESP8266::Reply ESP8266::readReply(unsigned long timeout = 5000) {
    char rep[128];
    uint8_t readResult = readBuffer(rep, timeout); //read the buffer
    while (readResult == 0) {
        for (uint8_t i = 0; i < LAST_REPLY; i++) { //loop through all available replies
            if (strstr(rep, REPLIES[i]) != NULL) { //check if the buffer contains the reply
                switch (i) {
                    case WIFI_GOT_IP:
                        //connectedToWifi = true;
                        break;
                    case WIFI_CONNECTED:
                    case WIFI_DISCONNECT:
                        //connectedToWifi = false;
                        break;
                    case RECEIVE_DATA:
                        //TODO: parse data
                        Serial.print("Rec'd data:");
                        Serial.println(rep);
                        break;
                    default: //if the command is not recognized as one of the above, then return the reply
                        return (Reply) i; //return the reply that we found
                }
            }
        }
        readResult = readBuffer(rep, timeout);
    }
    return INVALID_REPLY;
}

bool ESP8266::sendLinkMessage(uint8_t linkId, const char msg[]) {
    char param[128];
    snprintf(param, 128, "%u,%d", linkId, strlen(msg)); //prepare parameters with link id, and chars to write
    sendCommand(SEND_DATA, param); //indicate to ESP you want to send data
    Serial1.setTimeout(5000);
    if (Serial1.find('>')) { //wait for ESP to give wrap return
        Serial1.write(msg); //write data
        Serial.print("Sent: ");
        Serial.println(msg);
    } else {
        return false;
    }
    return readReply() != ERROR;
}

bool ESP8266::connectWifi(const char ssid[64], const char pass[64]) {
    char param[128];
    snprintf(param, 128, "\"%s\",\"%s\"", ssid, pass); //format parameters for command
    sendCommand(JOIN_NETWORK_ONCE, param); //send command to connect to wifi
    return readReply(15000) != FAIL; //wait at least 15 sec for final reply
}

void ESP8266::disconnectWifi() {
    sendCommand(QUIT_NETWORK); //disconnect from wifi
    readReply();
}

bool ESP8266::startTCPConnection(uint8_t linkId, const char host[64], unsigned long port) {
    char param[128];
    snprintf(param, 128, "%u,\"TCP\",\"%s\",%u", linkId, host, port);
    sendCommand(START_CONNECTION, param);
    return readReply(10000) != ERROR;
}

void ESP8266::closeConnection(uint8_t linkId) {
    char param[2];
    itoa(linkId, param, 10);
    sendCommand(CLOSE_CONNECTION, param);
    readReply();
}

uint8_t ESP8266::readBuffer(char msg[128], unsigned long timeout = 5000) {
    Serial1.setTimeout(timeout);
    char buf[128];
    uint8_t len = Serial1.readBytesUntil(LF, buf, 128); //read a whole message, until line feed
    if (len > 0) { //if a message was read
        strlcpy(msg, buf, len);
        Serial.print("Rec'd: ");
        Serial.println(msg);
    } else {
        return 1;
    }
    return 0;
}

