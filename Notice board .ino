#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

const char* ssid = "YOUR_WIFI_SSID";        
const char* password = "YOUR_WIFI_PASSWORD";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// SH1106 constructor
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiServer server(80);

String message = "Welcome!";
const int buzzerPin = 23;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);

  // I2C Pins
  Wire.begin(21, 22);

  // Initialize SH1106
  if (!display.begin(0x3C, true)) {
    Serial.println("SH1106 allocation failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,10);
  display.println("Waiting...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected");
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    int startIdx = request.indexOf("message=");
    if (startIdx != -1) {
      int endIdx = request.indexOf(" ", startIdx);
      String newMessage = request.substring(startIdx + 8, endIdx);
      newMessage.replace("+", " ");
      newMessage.replace("%20", " ");
      message = newMessage;

      // Buzzer beep 3 times
      for (int i = 0; i < 3; i++) {
        digitalWrite(buzzerPin, HIGH);
        delay(200);
        digitalWrite(buzzerPin, LOW);
        delay(200);
      }

      // Display clean message
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0,10);
      display.println(message);
      display.display();
    }

    // Serve Advanced Web Page
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    client.println("<!DOCTYPE html><html>");
    client.println("<head>");
    client.println("<title>BILLI ARMY Notice Board</title>");
    client.println("<style>");
    client.println("body { background: linear-gradient(to right, #000428, #004e92); color: white; font-family: 'Roboto', sans-serif; text-align: center; }");
    client.println("h1 { font-size: 40px; margin-top: 20px; }");
    client.println("form { margin-top: 50px; }");
    client.println("input[type=text] { width: 300px; height: 40px; font-size: 20px; padding: 5px; border-radius: 10px; border: none; }");
    client.println("input[type=submit] { height: 50px; width: 150px; font-size: 20px; background-color: #00c3ff; border: none; border-radius: 10px; color: white; margin-top: 20px; cursor: pointer; }");
    client.println("input[type=submit]:hover { background-color: #0077b6; }");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>BILLI ARMY</h1>");
    client.println("<h2>Send Message to Notice Board</h2>");
    client.println("<form action=\"/\">");
    client.println("<input type=\"text\" name=\"message\" placeholder=\"Type your message here...\">");
    client.println("<br>");
    client.println("<input type=\"submit\" value=\"Send\">");
    client.println("</form>");
    client.println("</body>");
    client.println("</html>");
  }
}
