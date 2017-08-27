#include <Arduino.h>

class ESP8266 {
public:
    bool init();

    bool connectWifi(const char ssid[64], const char pass[64]);
    void disconnectWifi();
    bool startTCPConnection(uint8_t linkId, const char host[64], long port);
    bool sendLinkMessage(uint8_t linkId, const char msg[]);
    void closeConnection(uint8_t linkId);

private:
    static const uint8_t LF = 10;

    /** Numerical designations for common replies. */
    enum Reply {
        OK,
        FAIL,
        ERROR,
        SEND_OK,
        ALREADY_CONNECT,
        LINK_IS_BUILDED,
        WIFI_CONNECTED,
        WIFI_DISCONNECT,
        WIFI_GOT_IP,
        RECEIVE_DATA,
        LAST_REPLY,
        INVALID_REPLY = LAST_REPLY
    };

    /** Numerical designations for common commands. */
    enum Command {
        /** A simple "AT" command used for verifying module is operational. */
        CHECK_MODULE,
        /** A "AT+CWMODE_DEF" command which sets whether wifi will be station,
           softAP, or both.\n
           1: station\n
           2: softAP\n
           3: station + softAP*/
        CHANGE_WIFI_MODE,
        CHANGE_CON_MODE,
        LIST_NETWORKS,
        /** A "AT+CWJAP_CUR" command is sent, taking parameters "ssid,pwd[,bssid]"*/
        JOIN_NETWORK_ONCE,
        /** A "AT+CWJAP_DEF" command is sent, taking parameters "ssid,pwd[,bssid]"
          Network will also be made default and will be joined when device is next booted.*/
        JOIN_NETWORK_DEFAULT,
        QUIT_NETWORK,
        START_CONNECTION,
        SEND_DATA,
        CLOSE_CONNECTION,
        TOGGLE_ECHO,
        CONNECTION_STATUS
    };
    /**
      Sends the specified command with the indicated parameters.
      @param cmd the command to be sent
      @param param the command parameters
     */
    void sendCommand(Command cmd, const char* param);

    void sendCommand(Command cmd) {
        sendCommand(cmd, "");
    };

    Reply readReply(unsigned long timeout = 5000);

    /**
      Reads the next message from the Serial1 buffer.

      @param msg char array to store the received message
      @param lock true if buffer should lock until a message is received or timeout is reached
      @param timeout the length in milliseconds to lock while waiting for message
      @return 0 for successful msg, 1 if timed out
     */
    uint8_t readBuffer(char msg[128], unsigned long timeout = 5000);

    const char REPLIES[10][16] = {
        "OK",
        "FAIL",
        "ERROR",
        "SEND OK",
        "ALREADY CONNECT",
        "Link is builded",
        "WIFI CONNECTED",
        "WIFI DISCONNECT",
        "WIFI GOT IP",
        "+IPD"
    };


};

const char AT[] PROGMEM = "AT";
const char CWMODE[] PROGMEM = "AT+CWMODE_DEF=";
const char CIPMUX[] PROGMEM = "AT+CIPMUX=";
const char CWLAP[] PROGMEM = "AT+CWLAP";
const char CWJAP_CUR[] PROGMEM = "AT+CWJAP_CUR=";
const char CWJAP_DEF[] PROGMEM = "AT+CWJAP_DEF=";
const char CWQAP[] PROGMEM = "AT+CWQAP";
const char CIPSTART[] PROGMEM = "AT+CIPSTART=";
const char CIPSEND[] PROGMEM = "AT+CIPSEND=";
const char CIPCLOSE[] PROGMEM = "AT+CIPCLOSE=";
const char ATE[] PROGMEM = "ATE";
const char CIPSTATUS[] PROGMEM = "AT+CIPSTATUS";
/**
    All of the commands to be stored in program memory and retrieved using:
   <c>char buffer[30];
   <c>strcpy_P(buffer, (char*)pgm_read_word(&(MEM_COMMANDS[CHECK_MODULE])));
 */
const char* const MEM_COMMANDS[] PROGMEM = {
    AT,
    CWMODE,
    CIPMUX,
    CWLAP,
    CWJAP_CUR,
    CWJAP_DEF,
    CWQAP,
    CIPSTART,
    CIPSEND,
    CIPCLOSE,
    ATE,
    CIPSTATUS
};
