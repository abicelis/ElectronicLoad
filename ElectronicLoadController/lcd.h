/*
 * LCD
 * Wiring for the LCD:
 * LCD PIN    Arduino pin     DFROBOT shield pinout
 * RS         12              8
 * Enable     11              9
 * D4         7               7
 * D5         6               6
 * D6         5               5
 * D7         4               4
 * R/W pin to ground
 * VSS pin to ground
 * VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */
#define LCD_RS                  12
#define LCD_ENABLE              11
#define LCD_D4                  7
#define LCD_D5                  6
#define LCD_D6                  5
#define LCD_D7                  4
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

int getLenghtOfFloat(float var)
{
  if(var >= 10)
    return 5;
  else
    return 4;
}

int getLenghtOfInt(int var)
{
  if(var >= 1000)
    return 4; 
  if(var >= 100)
    return 3;  
  else if(var >= 10)
    return 2;
  else
    return 1;
}


/*
 *@Param numberChars: The max space in chars the int can be, if val can be max 999 then numberChars=3
 */
void lcdPrintIntWhitespace(int val, byte numberChars) {
  for(int i = 0; i < numberChars-getLenghtOfInt(val) ; i++) {
    lcd.print(" ");
  }
}

void resetLcdToDefault() {
  lcd.clear();
  lcd.setCursor(9, 0);
  lcd.write(byte(1)); //VL= char

  lcd.setCursor(15, 0);
  lcd.write("v");
  lcd.setCursor(9, 1);
  lcd.write(byte(2)); //IL= char
  lcd.setCursor(14, 1);
  lcd.write("mA");
}


/* Custom LCD chars */
// ohm sign
byte ohm[8] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01010,
  B11011,
  B00000
};
byte squaree[8] = {
  0b00000,
  0b01110,
  0b01010,
  0b01010,
  0b01010,
  0b01010,
  0b11011,
  0b00000
};
byte triang1[8] = {
  0b00000,
  0b00100,
  0b01010,
  0b10001,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte triang2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10001,
  0b01010,
  0b00100
};
byte sine1[8] = {
  0b00000,
  0b00000,
  0b01110,
  0b10001,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte sine2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

byte vLoad[8] = {
  0b10100,
  0b10100,
  0b10100,
  0b01000,
  0b00001,
  0b10000,
  0b10001,
  0b11100
};

byte iLoad[8] = {
  0b11100,
  0b01000,
  0b01000,
  0b11100,
  0b00001,
  0b10000,
  0b10001,
  0b11100
};

byte hi[8] = {
  0b10100,
  0b11100,
  0b10101,
  0b00000,
  0b11101,
  0b01000,
  0b11100,
  0b00000
};

byte lo[8] = {
  0b10000,
  0b10000,
  0b11101,
  0b00000,
  0b11101,
  0b10100,
  0b11100,
  0b00000
};
