void setup()
{
  DDRD &=(1<<2);
  DDRB |=(1<<5);
  
}
void loop()
{
  if((PIND&(1<<2))==0)
  {
    PORTB|=(1<<5);
  }
  else
  {
    PORTB&=~(1<<5);
  }
}
