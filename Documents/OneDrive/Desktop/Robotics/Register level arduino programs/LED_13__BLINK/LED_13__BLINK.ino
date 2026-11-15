 void setup(){
 DDRB=0x20;
 }
 void loop(){
 PORTB|=0X20;
 delay(200);
 PORTB&=~0X20;
 delay(200);
}
