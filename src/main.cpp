#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <PubSubClient.h>

#include "ESP8266WiFi.h"
#include "config.h"
#include "led.h"

WiFiClient espClient;
PubSubClient mqtt(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

enum LabStatus { Unknown,
                 Open,
                 Closed };

void setup() {
    Serial.begin(115200);

    strip.begin();
    setColour(&strip, 0, 0, 255);

    setup_wifi();

    mqtt.setServer(MQTT_SERVER, 1883);
    mqtt.setCallback(callback);
}

void loop() {
    if (!mqtt.connected()) {
        reconnect();
    }
    mqtt.loop();
}

void setup_wifi() {
    delay(10);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Length: ");
    Serial.print(length);
    Serial.print(". Message: ");

    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
    }
    Serial.println();

    if (strcmp(topic, MQTT_STATUS_TOPIC) != 0) {
        Serial.println("Ignoring message for non-status topic.");
        return;
    }

    LabStatus status = Unknown;

    if (length == 1) {
        char c = (char)message[0];
        if (c == '1') {
            status = Open;
        } else if (c == '0') {
            status = Closed;
        }
    }

    Serial.print("Receiced lab status update: ");
    if (status == Closed) {
        Serial.println("Lab closed");
        setColour(&strip, 255, 0, 0);
    } else if (status == Open) {
        Serial.println("Lab open");
        setColour(&strip, 0, 255, 0);
    } else {
        Serial.println("Status unknown");
        setColour(&strip, 255, 255, 0);
    }
    Serial.println();
}

// reconnect to MQTT broker
void reconnect() {
    while (!mqtt.connected()) {
        Serial.print("Attempting MQTT connection...");

        if (mqtt.connect("ESP8266Client")) {
            Serial.println("connected");
            mqtt.subscribe(MQTT_STATUS_TOPIC);
            flash(&strip);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt.state());
            Serial.println(" try again in 5 seconds");

            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
