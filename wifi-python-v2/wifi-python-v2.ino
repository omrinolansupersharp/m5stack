#include <M5Stack.h>
#include <WiFi.h>

const char* ssid = "SS_RandD";
const char* password = "Sup3r$h4rp2021";
const char* hostname = "M5Stack_Device";
WiFiServer server(22);  // Change port number to 21

void setup() {
    M5.begin();
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    // Set the hostname
    WiFi.setHostname(hostname);    

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());
    Serial.print("Hostname: ");
    Serial.println(WiFi.getHostname());

    server.begin();
}

void loop() {
    WiFiClient client = server.available();
    if (client) {
        Serial.println("New Client Connected");
        Serial.print("Client IP: ");
        Serial.println(client.remoteIP());

        bool firstLineChecked = false;

        while (client.connected()) {
            if (client.available()) {
                String line = client.readStringUntil('\n');
                Serial.println(line);

                if (!firstLineChecked) {
                    // Check if the first message starts with "Hello M5Stack"
                    if (line.startsWith("Hello M5Stack")) {
                        client.println("Echo: " + line);
                        firstLineChecked = true;
                    } else {
                        client.println("Message rejected");
                        break; // Disconnect if the first line is not valid
                    }
                } else {
                    // Process subsequent lines
                    client.println("Received: " + line);
                }
            }
        }
        client.stop();
        Serial.println("Client Disconnected");
    }
}
