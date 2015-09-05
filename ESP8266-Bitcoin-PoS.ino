#include <ESP8266WiFi.h>
#include <TextFinder.h>
#include <Adafruit_NeoPixel.h>

#define PIN            2   //WS2811 connected to GPIO2
#define NUMPIXELS      5

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const char* ssid     = "<SSID>";
const char* password = "<PASSWORD>";
const char* host = "api.blockcypher.com";

void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(" BTC FTW ");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  Serial.println("");
}

//int value = 0;
unsigned long balance = 0;
String address;
int rateLimit = 0;

void loop() {
  delay(500);
 // ++value;

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  TextFinder finder(client);

  String url = "/v1/btc/main/addrs/<YOUR_BTC_ADDRESS>/balance?token=<OPTIONAL_TOKEN>";

  Serial.print("Requesting data from ");
  Serial.println(host);
  Serial.println("");
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
               
  displayLED(0, 100, 0);   //blink green

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {

    //String json = client.readString();

    if (finder.find("X-Ratelimit-Remaining")) {
      rateLimit = finder.getValue();
      Serial.print("Remaining queries: ");
      Serial.println(rateLimit);
    }
    if (finder.find("address")) {
      address = client.readStringUntil('\n');
      Serial.print("Address: ");
      Serial.println(address.substring(4, 38));
    }
    if (finder.find("balance")) {
      balance = finder.getValue() / 100;
      Serial.print("Balance: ");
      Serial.print(balance);
      Serial.println(" bits");
      Serial.println("");
      Serial.println("");
      displayLED(100, 0, 100);   //blink purple
      return;
    }
  }
}

void displayLED(byte r, byte g, byte b){
  strip.setPixelColor(0, strip.Color(r, g, b));
  strip.setPixelColor(1, strip.Color(r, g, b));
  strip.setPixelColor(2, strip.Color(r, g, b));
  strip.setPixelColor(3, strip.Color(r, g, b));
  strip.setPixelColor(4, strip.Color(r, g, b));
  strip.show();
  //digitalWrite(BUILTIN_LED, LOW);
  delay(200);                      
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.setPixelColor(2, strip.Color(0, 0, 0));
  strip.setPixelColor(3, strip.Color(0, 0, 0));
  strip.setPixelColor(4, strip.Color(0, 0, 0));
  strip.show();
  //digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(800);
}
