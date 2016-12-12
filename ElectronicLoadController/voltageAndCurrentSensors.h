float getVLoadSample() { 
  return (((float)analogRead(VLoadPin)/1024)*5)*3.4816;      //VloadPin*K    K=(R1+R2)/R2=3.4816,  R1=7445ohm, R2=3000ohm.
}
void updateVLoad() {
  VLoadBuffer -= VLoad;               //Remove the VLoad avg from buffer
  VLoadBuffer += getVLoadSample();    //Add a new sample
  VLoad = VLoadBuffer/BUFFER_SIZE;    //Recalculate VLoad avg
}

int getILoadSample() { 
  return (((double)analogRead(ILoadPin)/1024)*(5*1000));      //ILoad in mA
}
void updateILoad() {
  ILoadBuffer -= ILoad;               //Remove the ILoad avg from buffer
  ILoadBuffer += getILoadSample();    //Add a new sample
  ILoad = ILoadBuffer/BUFFER_SIZE;    //Recalculate ILoad avg
}


void printVLoadAndILoad() {
  lcd.setCursor(10, 0);
  if(getLenghtOfFloat(VLoad)== 4)
    lcd.print(" ");
  lcd.print(VLoad, 2);

  lcd.setCursor(10, 1);
  lcdPrintIntWhitespace(ILoad, 4);
  lcd.print(ILoad); 
}

