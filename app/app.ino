// Arduino Ethernet Pins (Toggle Pins via Ethernet)

#include <Ethernet.h>
#include <SPI.h>
boolean reading = false;
boolean broadcasted = false;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte pins[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

EthernetClient client;
EthernetServer server = EthernetServer(80);

void setup() {
  Serial.begin(9600);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  Ethernet.begin(mac);

  server.begin();
  Serial.println(Ethernet.localIP());

  broadcast();
}

void broadcast() {
  /* broadcast our dchp ip to server */
  if (client.connect("arduino.kevinknoll.com", 80)) {
    client.print("GET /?ip=");
    client.print(Ethernet.localIP());
    client.println(" HTTP/1.1");
    client.println("Host: arduino.kevinknoll.com");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed");
  }
}

void checkForBroadcast() {
  // read bytes from broadcast response
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // when broadcast response ends, disconnect
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // trip broadcasted flag
    broadcasted = true;
  }
}

void loop() {
  if (broadcasted) {
    checkForClient();
  } else {
    checkForBroadcast();
  }
}

void checkForClient() {
  EthernetClient client = server.available();

  if (client) {

    boolean currentLineIsBlank = true;
    boolean sentHeader = false;

    while (client.connected()) {

      if (client.available()) {

        if (!sentHeader) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          sentHeader = true;
        }

        char c = client.read();

        if (reading && c == ' ') {
          reading = false;
        }
        if (c == '?') {
          reading = true;
        }

        if (reading) {
          Serial.print(c);

          switch (c) {
           case '2':
             togglePin(2, client);
             break;
           case '3':
             togglePin(3, client);
             break;
           case '4':
             togglePin(4, client);
             break;
           case '5':
             togglePin(5, client);
             break;
           case '6':
             togglePin(6, client);
             break;
           case '7':
             togglePin(7, client);
             break;
           case '8':
             togglePin(8, client);
             break;
           case '9':
             togglePin(9, client);
             break;
          }
        }

        if (c == '\n' && currentLineIsBlank) {
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }

    }

    // give the web browser time to receive the data
    delay(1);

    // close connection
    client.stop();
  }
}

void togglePin(int pin, EthernetClient client) {
  // toggle pin, return output to browser
  client.print("Toggling pin: ");
  client.println(pin);
  client.print("<br>");

  if (pins[pin]) {
    digitalWrite(pin, LOW);
  } else {
    digitalWrite(pin, HIGH);
  }

  // save pin state
  pins[pin] = !pins[pin];
}
