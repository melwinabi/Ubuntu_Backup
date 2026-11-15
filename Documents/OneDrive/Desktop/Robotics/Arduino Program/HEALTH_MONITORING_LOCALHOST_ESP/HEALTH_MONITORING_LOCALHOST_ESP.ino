#include <ESP8266WiFi.h>
 String inputString = "";         // a String to hold incoming data
bool stringComplete = false;
// Replace with your network credentials
const char* ssid     = "HEALTH";   //wifi name
const char* password = "12345678";      //wifi password
 
// Set web server port number to 80
WiFiServer server(80);
 
// Variable to store the HTTP request
String header;
 
// Auxiliar variables to store the current output state
String output0State = "off";
String output1State = "off";
String output2State = "off";
String output3State = "off";
 
// Assign output variables to GPIO pins
const int output0 = D2;     //GPIO4
const int output1 = D5;     //GPIO14
const int output2 = D6;     //GPIO12
const int output3 = D1;     //GPIO5
 
void setup() {
  Serial.begin(9600);
  // Initialize the output variables as outputs
  pinMode(output0, OUTPUT);
  pinMode(output1, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output3, OUTPUT);
  
  // Set outputs to LOW
  digitalWrite(output0, LOW);
  digitalWrite(output1, LOW);
  digitalWrite(output2, LOW);
  digitalWrite(output3, LOW);
 
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
   inputString.reserve(200);
}
 
void loop() 
{
  if (Serial.available())
  {
    inputString = Serial.readString();
    Serial.println(inputString); 
  } 
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) 
  {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
 
            // turns the GPIOs on and off
            if (header.indexOf("GET /4/on") >= 0) 
            {
              Serial.println("GPIO 4 on");
              output0State = "on";
              digitalWrite(output0, HIGH);
            } 
            else if (header.indexOf("GET /4/off") >= 0) 
            {
              Serial.println("GPIO 4 off");
              output0State = "off";
              digitalWrite(output0, LOW);
            } 
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
 
            // Web Page Heading
            client.println("<body><h1>IOT HEALTH MONITORING</h1>");
            //client.println("<body><h2>using Local Web Server</h2>");
 
            // Display current state, and ON/OFF buttons for GPIO 4
            client.println("<p>" + inputString + "</p>");
            // If the output0State is off, it displays the ON button
           /* if (output0State == "off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">PAY</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">PAID</button></a></p>");
            }*/
            // client.println("<style>table,th,td{border:1px solid black;}</style><body><h2>TR elements define table rows</h2><table style="width:100%"><tr><td>Emil</td><td>Tobias</td><td>Linus</td>
//  </tr//></table>");
    
//  <tr>
//    <td>16</td>
//    <td>14</td>
//    <td>10</td>
//  </tr>
//   <tr>
//    <td>16</td>
//    <td>14</td>
//    <td>10</td>
//  </tr>

            //client.println("<style>table, th, td {border:1px solid black;}</style><table style=width:100%><tr><td>PRODUCT</td><td>RACK-NUMBER</td></tr><tr><td>BISCUIT</td><td>1</td></tr><tr><td>MAGGIE</td><td>2</td></tr><tr><td>BOOST</td><td>3</td></tr></table></body> <meta http-equiv=refresh content=5></html>");
 
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
