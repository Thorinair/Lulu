#include <ESP8266WiFi.h>
#include <ESPSoftwareSerial.h>
#include <Brain.h>

#include <TwiFi.h>

#include "Configuration.h"
#include "ConfigurationWiFi.h"
#include "ConfigurationLuna.h"


/* PIN Definitions */
#define PIN_BATTERY   A0
#define PIN_LED       13
#define PIN_SERIAL_RX 14
#define PIN_SERIAL_TX 16

/* RGB LED Statuses */
#define LED_MODE_OFF 0
#define LED_MODE_DIM 1
#define LED_MODE_ON  2

/* URL results */
#define URL_RESULT_DONE 0
#define URL_RESULT_FAIL 1

/* Flash Types */
#define FLASH_TYPE_DONE 0
#define FLASH_TYPE_FAIL 1

/* Flash Timings */
#define FLASH_TIME_SHORT 50
#define FLASH_TIME_LONG  500

/* Objects */
SoftwareSerial softSerial(PIN_SERIAL_RX, PIN_SERIAL_TX, false, 256);
Brain brain(softSerial);

/* Setup Functions */
void setupPins();

/* Utility Functions */
void flashStatusLED(int type);
int openURL(String url);

/* Processing Functions */
void processInterval();

void setLED(int ledMode);

void connectAttempt(int idEntry, int attempt);
void connectSuccess(int idEntry);
void connectFail(int idEntry);

/* Variables */
int wifiConnected;
int loopCounter;
bool initial;
bool powerState;
bool blinking;

/* Setup Functions */
void setupPins() {
    if (LED_ENABLE) {
        pinMode(PIN_LED, OUTPUT);
        setLED(LED_MODE_DIM);
    }
}



/* Utility Functions */
void flashStatusLED(int type) {
    if (LED_ENABLE) {
        if (type == FLASH_TYPE_DONE) {
            setLED(LED_MODE_ON);
            delay(FLASH_TIME_SHORT);
            setLED(LED_MODE_DIM);
            delay(FLASH_TIME_SHORT);
        }
        else if (type == FLASH_TYPE_FAIL) {
            setLED(LED_MODE_ON);
            delay(FLASH_TIME_LONG);
            setLED(LED_MODE_DIM);
            delay(FLASH_TIME_SHORT);
        }
    }
}

int openURL(String url) {
    if (LUNA_DEBUG)
        Serial.println("Opening URL: " + String(LUNA_IP) + url);
        
    WiFiClient client;
    if (!client.connect(LUNA_IP, LUNA_PORT)) {  
        if (LUNA_DEBUG)
            Serial.println("Error connecting!");
        return URL_RESULT_FAIL;
    }

    client.print("GET " + url + " HTTP/1.1\r\n" +
                 "Host: " + LUNA_IP + "\r\n" + 
                 "Connection: close\r\n\r\n");
    client.stop();
    
    if (LUNA_DEBUG)
        Serial.println("Connection success.");

    return URL_RESULT_DONE;
}



/* Processing Functions */
void processInterval() {
    if (brain.update()) {
        if (DEBUG_ENABLE) {
            Serial.println(brain.readErrors());
            Serial.println(brain.readCSV());
        }

        float eegbattery;
        if (BATTERY_VOLTAGE)
            eegbattery = ((float)analogRead(PIN_BATTERY) / 1024) * BATTERY_MULTIPLIER; 
        else
            eegbattery = ((((float)analogRead(PIN_BATTERY) / 1024) * BATTERY_MULTIPLIER - BATTERY_V_MIN) / (BATTERY_V_MAX - BATTERY_V_MIN)) * 100; 
        
        if (eegbattery < 0)
            eegbattery = 0;
        else if (eegbattery > 100)
            eegbattery = 100;            
        float eegsignal = ((float) (200 - brain.readSignalQuality()) / 200) * 100;
        byte eegattention = brain.readAttention();
        byte eegmeditation = brain.readMeditation();
        unsigned long eegwave0 = brain.readDelta();
        unsigned long eegwave1 = brain.readTheta();
        unsigned long eegwave2 = brain.readLowAlpha();
        unsigned long eegwave3 = brain.readHighAlpha();
        unsigned long eegwave4 = brain.readLowBeta();
        unsigned long eegwave5 = brain.readHighBeta();
        unsigned long eegwave6 = brain.readLowGamma();
        unsigned long eegwave7 = brain.readMidGamma();

        String url = LUNA_URL_EEG;
        url += "&key="        + String(LUNA_KEY);
        url += "&battery="    + String(eegbattery, 2);
        url += "&signal="     + String(eegsignal, 2);
        url += "&attention="  + String(eegattention);
        url += "&meditation=" + String(eegmeditation);
        url += "&wave0="      + String(eegwave0);
        url += "&wave1="      + String(eegwave1);
        url += "&wave2="      + String(eegwave2);
        url += "&wave3="      + String(eegwave3);
        url += "&wave4="      + String(eegwave4);
        url += "&wave5="      + String(eegwave5);
        url += "&wave6="      + String(eegwave6);
        url += "&wave7="      + String(eegwave7);

        if (openURL(url) == URL_RESULT_DONE)
             flashStatusLED(FLASH_TYPE_DONE);
        else     
             flashStatusLED(FLASH_TYPE_FAIL);    
    }
}

void setLED(int ledMode) {
    if (LED_ENABLE) {
        switch (ledMode) {
            case LED_MODE_OFF: 
                analogWrite(PIN_LED, PWMRANGE - 0);
                break;
            case LED_MODE_DIM: 
                analogWrite(PIN_LED, PWMRANGE - LED_BRIGHTNESS_DIM);
                break;
            case LED_MODE_ON: 
                analogWrite(PIN_LED, PWMRANGE - LED_BRIGHTNESS_ON);
                break;         
        }
    }
}



void connectAttempt(int idEntry, int attempt) {
    if (attempt % 2) {
        setLED(LED_MODE_OFF);
    }
    else {
        setLED(LED_MODE_DIM);
    }
}

void connectSuccess(int idEntry) {
}

void connectFail(int idEntry) {
    setLED(LED_MODE_ON);
}

void setup() {
    // Start the hardware serial.
    Serial.begin(115200);
    if (DEBUG_ENABLE)
        Serial.println("\nBOOT");
    // Start the software serial.
    softSerial.begin(9600);
    
    blinking = false;
    setupPins();    
    twifiInit(
        wifis,
        WIFI_COUNT,
        WIFI_HOST,
        WIFI_TIMEOUT,
        &connectAttempt,
        &connectSuccess,
        &connectFail,
        WIFI_DEBUG
        );
    twifiConnect(true);
    openURL(String(LUNA_URL_BOOT) + "&key=" + String(LUNA_KEY) + "&device=" + String(WIFI_HOST));

    setLED(LED_MODE_DIM);
}

void loop() {
    if (!twifiIsConnected())
        twifiConnect(true);
    else
        processInterval();
}
