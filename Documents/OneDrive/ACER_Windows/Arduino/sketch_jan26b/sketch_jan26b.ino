String Read1;
String Read2;
int j = 0;

void setup() 
{ 
    Serial.begin(9600);
    Serial2.begin(9600); // Assuming Serial2 is the communication interface
    Serial.println("Transmitter Ready...");
    Serial2.println("AT+CTXADDRSET=02"); // Set the transmitter address
    Serial2.println("AT+CADDRSET=01");   // Set the receiver address
    Serial2.println("AT+CTX=433000000,5,0,1,21,1"); // Configure the transmitter
    delay(500);
}

void loop() 
{
    j++;
    Serial2.print("Message: "); // Sending a message with a counter
    String data2 = j + "\r";
    Serial2.println(data2);

    // If there's a response from the receiver
    if (Serial2.available()) 
    {
        Read2 = Serial2.readString();
        Serial.println("Received from Receiver: " + Read2);
    }

    delay(1000); // Delay for better synchronization
}
