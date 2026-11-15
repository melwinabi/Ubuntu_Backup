String Read1;
String Read2;

void setup() 
{ 
    Serial.begin(9600);
    Serial2.begin(9600); // Assuming Serial2 is the communication interface
    Serial.println("Receiver Ready...");
    Serial2.println("AT+CADDRSET=02"); // Set the receiver address
    Serial2.println("AT+CRXS=433000000,5,0,1,"); // Configure the receiver
    delay(500);
}

void loop() 
{
    // If a message is received from the transmitter
    if (Serial2.available()) 
    {
        Read2 = Serial2.readString();
        Serial.println("Received from Transmitter: " + Read2);

        // Send acknowledgment back to the transmitter
        Serial2.println("ACK: " + Read2);
    }

    // Allow user input from Serial Monitor for testing
    if (Serial.available()) 
    {
        Read1 = Serial.readString();
        Serial2.println(Read1);
    }
}
