void Gen(){
  
 if(Flag == 45)
 {
  Set_view6();
  if(U == LOW)
  {
   C--;
   if(C < 1)
   {
    C=2;  
   }
   curs(C);
  }
  if(D == LOW)
  {
   C++;
   if(C > 2)
   {
    C=1;
   }
   curs(C);
  }
 } 
}
