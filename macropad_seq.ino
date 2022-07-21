#include "pe.h"

/*
   * preset[0] - hits
   * preset[1] - offset
   * preset[2] - limit
   * preset[3] - notes
   * preset[4] - mute
*/ 

void selectChannel(int channel){
    if (channel == select_ch){
      preset[4][channel] = !preset[4][channel];
    }else{
      if (currentMenuPos == 1){
        encoder.setPosition(preset[0][channel]);
      }else if (currentMenuPos == 2){
        encoder.setPosition(preset[1][channel]);
      }else if (currentMenuPos == 3){
        encoder.setPosition(preset[3][channel]);
      }else{
        encoder.setPosition(preset[2][channel]);
      }
    }
    select_ch = channel;
    
}

void selectMenu(int menu_page){
  for(int i = 0; i < 4; i++){
    pixels.setPixelColor(MENU_LED[i]-1, 0x001F00);
  }
  pixels.setPixelColor(MENU_LED[menu_page]-1, 0x00FF00);
  current_page = menu_page;
}


void drawMenu(){
  newPosition = encoder.getPosition();
  if (oldPosition != newPosition) {
    switch (currentMenuPos){
      case 1:
        ROTARYMIN = 0;
        ROTARYMAX = 16;
        if (newPosition <= ROTARYMIN) {
          encoder.setPosition(preset[0][select_ch]);
          newPosition = preset[0][select_ch];
        
        } else if (newPosition >= ROTARYMAX) {
          encoder.setPosition(ROTARYMAX);
          newPosition = ROTARYMAX;
        }
        preset[0][select_ch] = newPosition;
        break;
      case 2:
        ROTARYMIN = 0;
        ROTARYMAX = 16;
        if (newPosition <= ROTARYMIN) {
          encoder.setPosition(preset[1][select_ch]);
          newPosition = preset[1][select_ch];
        
        } else if (newPosition >= ROTARYMAX) {
          encoder.setPosition(ROTARYMAX);
          newPosition = ROTARYMAX;
        }
        preset[1][select_ch] = newPosition;
        break;
      case 3:
        ROTARYMIN = 21;
        ROTARYMAX = 108;
        if (newPosition <= ROTARYMIN) {
          encoder.setPosition(preset[3][select_ch]);
          newPosition = preset[3][select_ch];
        
        } else if (newPosition >= ROTARYMAX) {
          encoder.setPosition(ROTARYMAX);
          newPosition = ROTARYMAX;
        }
        preset[3][select_ch] = newPosition;
        break;
      case 4:
        ROTARYMIN = 0;
        ROTARYMAX = 16;
        if (newPosition <= ROTARYMIN) {
          encoder.setPosition(preset[2][select_ch]);
          newPosition = preset[2][select_ch];
        
        } else if (newPosition >= ROTARYMAX) {
          encoder.setPosition(ROTARYMAX);
          newPosition = ROTARYMAX;
        }
        preset[2][select_ch] = newPosition;
        break;
       case 5:
        ROTARYMIN = 40;
        ROTARYMAX = 200;
        if (newPosition <= ROTARYMIN) {
          encoder.setPosition(BPM);
          newPosition = BPM;
        
        } else if (newPosition >= ROTARYMAX) {
          encoder.setPosition(ROTARYMAX);
          newPosition = ROTARYMAX;
        }
        BPM = newPosition;
        break;
    }
    
  }
  display.setTextColor(SH110X_WHITE);
      for (int m = 0; m < 5; m++) {
        display.setCursor(0, m*10);
        display.print(menus[current_page][m]);
  }
  switch(current_page){
    case 0:
      
      //draw step dot
      for (int j = 0; j <= preset[2][k] - 1; j++) { // j = steps
        display.drawPixel(x16[j], y16[j], SH110X_WHITE);
      }
    
      if (offset_buf[k][playing_step[k]] == 0) {
        display.drawCircle(x16[playing_step[k]], y16[playing_step[k]], 2, SH110X_WHITE);
      }
    
      if (offset_buf[k][playing_step[k]] == 1 && preset[4][k]!= 0) {
        display.fillCircle(x16[playing_step[k]], y16[playing_step[k]], 3, SH110X_WHITE);
      }
    
      if (preset[4][k] == 0) {
        display.drawCircle(x16[playing_step[k]], y16[playing_step[k]], 2, SH110X_WHITE);
      }
      //draw 1 menu items
      if (preset[0][select_ch] > 1){
        for (j = 0; j < buf_count - 1; j++) {
          display.drawLine(line_xbuf[j], line_ybuf[j], line_xbuf[j + 1], line_ybuf[j + 1], SH110X_WHITE);
        }
        display.drawLine(line_xbuf[0], line_ybuf[0], line_xbuf[j], line_ybuf[j], SH110X_WHITE);
      }
      buf_count = 0;
      if (preset[0][select_ch] == 1) {
        Serial.println(preset[1][select_ch]);
        if (preset[1][select_ch] != 16 && preset[1][select_ch] != 0){
          display.drawLine(74, 32, x16[sizeof(x16) - preset[1][select_ch]], y16[sizeof(y16) - preset[1][select_ch]], SH110X_WHITE);
        } else {
          display.drawLine(74, 32, x16[0], y16[0], SH110X_WHITE);
        }
      }
      
      oldButtonVal = newButtonVal;
      newButtonVal = digitalRead(PIN_SWITCH);
      if (newButtonVal != oldButtonVal && newButtonVal > oldButtonVal){
        if (currentMenuPos >= 5){
          currentMenuPos = 0;
        }
        currentMenuPos++;
      }
      
      switch (currentMenuPos){
        case 1:
          display.drawTriangle(26, 3, 30, 0, 30, 6, SH110X_WHITE); // Step triangle
          display.fillTriangle(26, 3, 30, 0, 30, 6, SH110X_WHITE);
          break;
        case 2:
          display.drawTriangle(37, 13, 42, 10, 42, 16, SH110X_WHITE); // Offset triangle
          display.fillTriangle(37, 13, 42, 10, 42, 16, SH110X_WHITE);
          break;
        case 3:
          display.drawTriangle(26, 23, 30, 20, 30, 26, SH110X_WHITE); // Note triangle
          display.fillTriangle(26, 23, 30, 20, 30, 26, SH110X_WHITE);
          display.setCursor(69, 28);
          display.println(midiNotes(preset[3][select_ch]));
          break;
        case 4:
          display.drawTriangle(32, 33, 36, 30, 36, 36, SH110X_WHITE); // Limit triangle
          display.fillTriangle(32, 33, 36, 30, 36, 36, SH110X_WHITE);
          display.setCursor(69, 28);
          display.println(preset[2][select_ch]);
          break;
         case 5:
          display.drawTriangle(19, 43, 23, 40, 23, 46, SH110X_WHITE); // BPM triangle
          display.fillTriangle(19, 43, 23, 40, 23, 46, SH110X_WHITE);
          display.setCursor(66, 28);
          display.println(int(BPM));
          break;
      }
      oldPosition = newPosition;
      break;
     case 1:
       oldButtonVal = newButtonVal;
        newButtonVal = digitalRead(PIN_SWITCH);
        if (newButtonVal != oldButtonVal && newButtonVal > oldButtonVal){
          if (currentMenuPos >= 5){
            currentMenuPos = 0;
          }
          currentMenuPos++;
        }
       switch (currentMenuPos){
        case 1:
          display.drawTriangle(27, 3, 31, 0, 31, 6, SH110X_WHITE); // Channel triangle
          display.fillTriangle(27, 3, 31, 0, 31, 6, SH110X_WHITE);
          break;
        case 2:
          display.drawTriangle(27, 13, 31, 10, 31, 16, SH110X_WHITE); // Gate triangle
          display.fillTriangle(27, 13, 31, 10, 31, 16, SH110X_WHITE);
          display.setTextSize(3);
          display.setCursor(33, 18);
          display.println("+10ms");
          display.setTextSize(1);
          break;
        case 3:
          display.drawTriangle(27, 23, 31, 20, 31, 26, SH110X_WHITE); // Velocity triangle
          display.fillTriangle(27, 23, 31, 20, 31, 26, SH110X_WHITE);
          display.setTextSize(3);
          display.setCursor(55, 18);
          display.println("127");
          display.setTextSize(1);
          break;
        case 4:
          display.drawTriangle(27, 33, 31, 30, 31, 36, SH110X_WHITE); // Sync triangle
          display.fillTriangle(27, 33, 31, 30, 31, 36, SH110X_WHITE);
          break;
         case 5:
          display.drawTriangle(27, 43, 31, 40, 31, 46, SH110X_WHITE); // Sign triangle
          display.fillTriangle(27, 43, 31, 40, 31, 46, SH110X_WHITE);
          break;
       }
       oldPosition = newPosition;
      break;
     case 2:
      break;
     case 3:
      break;
  }
}

void channelProcess(int select_ch) {
  
  drawMenu();
  
  if (preset[4][select_ch] == 0){
  
    display.fillRect(0, 53, 40, 12, SH110X_WHITE);
    display.setTextColor(SH110X_BLACK);
    display.setCursor(2, 55);
    display.print(select_ch + 1);
    display.setCursor(9, 55);
    display.print("MUTED");
  } else {
    display.fillRect(0, 53, 9, 12, SH110X_WHITE);
    display.setTextColor(SH110X_BLACK);
    display.setCursor(2, 55);
    display.print(select_ch + 1);
    }
  
  buf_count = 0;

  for (int m = 0; m < 16; m++) {
    if (offset_buf[select_ch][m] == 1) {
      line_xbuf[buf_count] = x16[m];
      line_ybuf[buf_count] = y16[m];
      buf_count++;
    }
  }
}

void checkPosition() {  encoder.tick(); }
void onPressed1() { selectChannel(0);}
void onPressed2() { selectChannel(1);}
void onPressed3() { selectChannel(2);}
void onPressed4() { selectChannel(3);}
void onPressed5() { selectChannel(4);}
void onPressed6() { selectChannel(5);}
void onPressed7() { selectChannel(6);}
void onPressed8() { selectChannel(7);}

void onPressed9() { selectMenu(0);}
void onPressed10() { selectMenu(1);}
void onPressed11() { selectMenu(2);}
void onPressed12() { selectMenu(3);}

String midiNotes(int noteNumber) {
  byte octave = noteNumber / 12;
  byte noteInOctave = noteNumber % 12;
  return *noteNames[noteInOctave] + String(octave - 1);
}

void setup() {
  // start pixels!
  pixels.begin();
  pixels.setBrightness(255);
  pixels.show(); // Initialize all pixels to 'off'
  selectMenu(0);
  
  //pixels.setPixelColor(MENU_LED[0]-1, 0x00FF00);
  //pixels.setPixelColor(MENU_LED[1]-1, 0x000F00);
  //pixels.setPixelColor(MENU_LED[2]-1, 0x000F00);
  //pixels.setPixelColor(MENU_LED[3]-1, 0x000F00);
  display.begin(0, true); // we dont use the i2c address but we will reset!
  display.clearDisplay();
  /*
  int display_interval = 600;
  for (int l = 0; l < 20; l++){
    display.drawBitmap(7,0, bitmap_logo3, 112, 64, SH110X_WHITE);
    display.display();
    delay(30);
    if (l == 19){
      display.clearDisplay();
      display.drawBitmap(7,0, bitmap_logo, 112, 64, SH110X_WHITE);
      display.display();
      delay(1000);
    }
    display.clearDisplay();
    display.display();
    delay(display_interval);
    display_interval = int(display_interval / 2);
  }
  
  */

  // set rotary encoder inputs and interrupts
  pinMode(PIN_ROTA, INPUT_PULLUP);
  pinMode(PIN_ROTB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ROTA), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ROTB), checkPosition, CHANGE);
  
  button1.begin();
  button2.begin();
  button3.begin();
  button4.begin();
  button5.begin();
  button6.begin();
  button7.begin();
  button8.begin();

  menu_button1.begin();
  menu_button2.begin();
  menu_button3.begin();
  menu_button4.begin();
  

  button1.onPressed(onPressed1);
  button2.onPressed(onPressed2);
  button3.onPressed(onPressed3);
  button4.onPressed(onPressed4);
  button5.onPressed(onPressed5);
  button6.onPressed(onPressed6);
  button7.onPressed(onPressed7);
  button8.onPressed(onPressed8);

  menu_button1.onPressed(onPressed9);
  menu_button2.onPressed(onPressed10);
  menu_button3.onPressed(onPressed11);
  menu_button4.onPressed(onPressed12);


  // text display tests
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setTextColor(SH110X_WHITE, SH110X_BLACK);
  usbMIDI.begin();
  //while( !TinyUSBDevice.mounted() ) delay(1);
}

void drawCircle() {
  k = select_ch;
//  display.setTextColor(SH110X_WHITE);
//  display.setCursor(120,0);
//  display.print(k+1);
//  display.setCursor(105,0);
//  display.print("CH");
 
  for (int x = 0; x < 8; x++){
    if (x == select_ch){
      channelProcess(select_ch);
    }
  }


    /*
        * preset[0] - hits
        * preset[1] - offset
        * preset[2] - limit
        * preset[3] - notes
        * preset[4] - mute
    */

  for (int k = 0; k < 8; k++){
    for (int i = preset[1][k]; i <= 15; i++) {
      offset_buf[k][i - preset[1][k]] = (pgm_read_byte(&(euc16[preset[0][k]][i]))) ;
    }
  
    for (int i = 0; i < preset[1][k]; i++) {
      offset_buf[k][16 - preset[1][k] + i] = (pgm_read_byte(&(euc16[preset[0][k]][i])));
    }

    if (offset_buf[k][playing_step[k]] == 1 && preset[4][k] == 1 && k == select_ch) {
      usbMIDI.sendNoteOn(preset[3][k], 127, k+1);
      pixels.setPixelColor(LED[k]-1, 0xFFFFFF);
    }
    else if (offset_buf[k][playing_step[k]] == 1 && preset[4][k] == 1) {
      usbMIDI.sendNoteOn(preset[3][k], 127, k+1);
      pixels.setPixelColor(LED[k]-1, 0x1F1F1F);
    }
    else if(preset[4][k] == 0 && k == select_ch){
      usbMIDI.sendNoteOff(preset[3][k], 127, k+1);
      pixels.setPixelColor(LED[k]-1, 0xFF0000);
    }
    else if(preset[4][k] == 0){
      usbMIDI.sendNoteOff(preset[3][k], 127, k+1);
      pixels.setPixelColor(LED[k]-1, 0x1F0000);
    }
    else{
      pixels.setPixelColor(LED[k]-1, 0x0);
    }
    if  (gate_timer + 10 <= millis()) { 
      usbMIDI.sendNoteOn(preset[3][k], 0, k+1);
    }
  }
}

void seqDisplay() {
  trg_in = 1;//digitalRead(CLOCK_IN);
  if (old_trg_in == 0 && trg_in == 1) {
    gate_timer = millis();
    for (int i = 0; i < 8; i++) {
      playing_step[i]++;
      if (playing_step[i] >= preset[2][i]) {
        playing_step[i] = 0;
      }
    }
    drawCircle();
    disp_reflesh = 1;
  }
  if (disp_reflesh == 1) {
    disp_reflesh = 0;
  }
  display.display();
}

void loop() {
    long int t1 = millis();
    button1.read();
    button2.read();
    button3.read();
    button4.read();
    button5.read();
    button6.read();
    button7.read();
    button8.read();
    menu_button1.read();
    menu_button2.read();
    menu_button3.read();
    menu_button4.read();
    if((t1 - t2 >= ( 60.0 / BPM ) * 250.0 )) {
      t2 = t1;
      seqDisplay();
      
      if (!digitalRead(PIN_SWITCH)) {
        pixels.setBrightness(255);     // bright!
      } else {
        pixels.setBrightness(80);
      }

      pixels.show();
      j++;
      //long int t2 = millis();
      display.clearDisplay();
    }
}