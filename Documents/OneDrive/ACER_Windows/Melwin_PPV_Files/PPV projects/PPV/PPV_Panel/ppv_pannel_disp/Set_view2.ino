void Set_view2(int o) {
    Flag = 21; 
    if (o == 1) {
        Flag = 22;
        lcd.setCursor(1, 0);
        lcd.print("  RAW WATER PUMB");    
        lcd.setCursor(4, 1);
        lcd.print(unit1);
        if(unit1<10){
           lcd.setCursor(5, 1); 
           lcd.print(" ");
        }
        lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit2);
        lcd.setCursor(8, 1);
        lcd.print(" ");    
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }

    if (o == 2) {
        Flag = 23;
        lcd.setCursor(1, 0);
        lcd.print("  AIR BLOWER");
        lcd.setCursor(4, 1);
        lcd.print(unit3);
        if (unit3 < 10) {
            lcd.print(" ");
        }
        lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit4); 
        lcd.setCursor(8, 1);
        lcd.print(" ");   
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }

    if (o == 3) {
        Flag = 24;
        lcd.setCursor(1, 0);
        lcd.print(" RECIRCULATION PUMB");    
        lcd.setCursor(4, 1);
        lcd.print(unit5);
        if (unit5 < 10) {
            lcd.print(" ");
        }
        lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit6);
        lcd.setCursor(8, 1);
        lcd.print(" ");    
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }

    if (o == 4) {
        Flag = 25;
        lcd.setCursor(1, 0);
        lcd.print(" FILTER FEED PUMB");  
        lcd.setCursor(4, 1);
        lcd.print(unit7);
        if (unit7 < 10) {
            lcd.print(" ");
        }
        lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit8);
        lcd.setCursor(8, 1);
        lcd.print(" ");    
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }

    if (o == 5) {
        Flag = 26;
        lcd.setCursor(1, 0);
        lcd.print(" TREATED WATER PUMB");    
        lcd.setCursor(4, 1);
        lcd.print(unit9);
        if (unit9 < 10) {
            lcd.print("  ");
        }
       lcd.setCursor(6, 1);
        lcd.print(".");
        lcd.setCursor(7, 1);
        lcd.print(unit10);
        lcd.setCursor(8, 1);
        lcd.print(" ");    
        lcd.setCursor(9, 1);
        lcd.print("Amps");   
        lcd.setCursor(1, 3);
        lcd.print("[Select to Feed]");
    }
}
