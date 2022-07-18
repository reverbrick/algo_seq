#include "pe.h"

int current_page = 0;

void selectChannel(int channel){
    if (channel == select_ch){
      mute[channel] = !mute[channel];
    }else{
      if (currentMenuPos == 1){
        encoder.setPosition(hits[channel]);
      }else if (currentMenuPos == 2){
        encoder.setPosition(offset[channel]);
      }else if (currentMenuPos == 3){
        encoder.setPosition(notes[channel]);
      }else{
        encoder.setPosition(limit[channel]);
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
          encoder.setPosition(ROTARYMIN);
          newPosition = ROTARYMIN;
        
        } else if (newPosition >= ROTARYMAX) {
          encoder.setPosition(ROTARYMAX);
          newPosition = ROTARYMAX;
        }
        hits[select_ch] = newPosition;
        break;
      case 2:
        ROTARYMIN = 0;
        ROTARYMAX = 16;
        if (newPosition <= ROTARYMIN) {
          encoder.setPosition(ROTARYMIN);
          newPosition = ROTARYMIN;
        
        } else if (newPosition >= ROTARYMAX) {
          encoder.setPosition(ROTARYMAX);
          newPosition = ROTARYMAX;
        }
        offset[select_ch] = newPosition;
        break;
      case 3:
        ROTARYMIN = 21;
        ROTARYMAX = 108;
        if (newPosition <= ROTARYMIN) {
          encoder.setPosition(ROTARYMIN);
          newPosition = ROTARYMIN;
        
        } else if (newPosition >= ROTARYMAX) {
          encoder.setPosition(ROTARYMAX);
          newPosition = ROTARYMAX;
        }
        notes[select_ch] = newPosition;
        break;
      case 4:
        ROTARYMIN = 0;
        ROTARYMAX = 16;
        if (newPosition <= ROTARYMIN) {
          encoder.setPosition(ROTARYMIN);
          newPosition = ROTARYMIN;
        
        } else if (newPosition >= ROTARYMAX) {
          encoder.setPosition(ROTARYMAX);
          newPosition = ROTARYMAX;
        }
        limit[select_ch] = newPosition;
        break;
       case 5:
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
      for (int j = 0; j <= limit[k] - 1; j++) { // j = steps
        display.drawPixel(x16[j], y16[j], SH110X_WHITE);
      }
    
      if (offset_buf[k][playing_step[k]] == 0) {
        display.drawCircle(x16[playing_step[k]], y16[playing_step[k]], 2, SH110X_WHITE);
      }
    
      if (offset_buf[k][playing_step[k]] == 1 && mute[k]!= 0) {
        display.fillCircle(x16[playing_step[k]], y16[playing_step[k]], 3, SH110X_WHITE);
      }
    
      if (mute[k] == 0) {
        display.drawCircle(x16[playing_step[k]], y16[playing_step[k]], 2, SH110X_WHITE);
      }
      //draw 1 menu items
      oldButtonVal = newButtonVal;
      newButtonVal = digitalRead(PIN_SWITCH);
      
      if (newButtonVal != oldButtonVal && newButtonVal > oldButtonVal){
        if (currentMenuPos >= 5){
          //encoder.setPosition(hits[select_ch]);
          //oldPosition = newPosition = hits[select_ch];
          currentMenuPos = 0;
        }
        currentMenuPos++;
      }
      switch (currentMenuPos){
        case 1:
          display.drawTriangle(27, 3, 31, 0, 31, 6, SH110X_WHITE); // Step triangle
          display.fillTriangle(27, 3, 31, 0, 31, 6, SH110X_WHITE);
          break;
        case 2:
          display.drawTriangle(37, 13, 42, 10, 42, 16, SH110X_WHITE); // Offset triangle
          display.fillTriangle(37, 13, 42, 10, 42, 16, SH110X_WHITE);
          break;
        case 3:
          display.drawTriangle(27, 23, 31, 20, 31, 26, SH110X_WHITE); // Note triangle
          display.fillTriangle(27, 23, 31, 20, 31, 26, SH110X_WHITE);
          display.setCursor(69, 28);
          display.println(midiNotes(notes[select_ch]));
          break;
        case 4:
          display.drawTriangle(35, 33, 39, 30, 39, 36, SH110X_WHITE); // Limit triangle
          display.fillTriangle(35, 33, 39, 30, 39, 36, SH110X_WHITE);
          display.setCursor(69, 28);
          display.println(limit[select_ch]);
          break;
         case 5:
          display.drawTriangle(36, 43, 40, 40, 40, 46, SH110X_WHITE); // BPM triangle
          display.fillTriangle(36, 43, 40, 40, 40, 46, SH110X_WHITE);
          display.setCursor(69, 28);
          display.println(int(BPM));
          break;
      }
      oldPosition = newPosition;
      break;
     case 1:
      break;
     case 2:
      break;
     case 3:
      break;
  }
  display.setCursor(109, 53);
  display.print("PG");
  display.setCursor(122, 53);
  display.print(current_page + 1);
}

void channelProcess(int select_ch) {
  
  drawMenu();
  
  if (mute[select_ch] == 0){
  
    display.fillRect(0, 53, 35, 12, SH110X_WHITE);
    display.setCursor(3, 55);
    display.setTextColor(SH110X_BLACK);
    display.print("MUTED");
  }
  
  buf_count = 0;

  for (int m = 0; m < 16; m++) {
    if (offset_buf[select_ch][m] == 1) {
      line_xbuf[buf_count] = x16[m];
      line_ybuf[buf_count] = y16[m];
      buf_count++;
    }
  }

  int j = 0;
  if (hits[select_ch] > 1){
    for (j = 0; j < buf_count - 1; j++) {
      display.drawLine(line_xbuf[j], line_ybuf[j], line_xbuf[j + 1], line_ybuf[j + 1], SH110X_WHITE);
    }
    display.drawLine(line_xbuf[0], line_ybuf[0], line_xbuf[j], line_ybuf[j], SH110X_WHITE);
  }
  buf_count = 0;
  if (hits[select_ch] == 1) {
    Serial.println(offset[select_ch]);
    if (offset[select_ch] != 16 && offset[select_ch] != 0){
      display.drawLine(74, 32, x16[sizeof(x16) - offset[select_ch]], y16[sizeof(y16) - offset[select_ch]], SH110X_WHITE);
    } else {
      display.drawLine(74, 32, x16[0], y16[0], SH110X_WHITE);
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
  // set all mechanical keys to inputs
  // for (uint8_t i=0; i<=12; i++) {
  //   pinMode(i, INPUT_PULLUP);
  // }

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
  display.setTextColor(SH110X_WHITE);
  display.setCursor(120,0);
  display.print(k+1);
  display.setCursor(105,0);
  display.print("CH");
 
  for (int x = 0; x < 8; x++){
    if (x == select_ch){
      channelProcess(select_ch);
    }
  }

  for (int k = 0; k < 8; k++){
    for (int i = offset[k]; i <= 15; i++) {
      offset_buf[k][i - offset[k]] = (pgm_read_byte(&(euc16[hits[k]][i]))) ;
    }
  
    for (int i = 0; i < offset[k]; i++) {
      offset_buf[k][16 - offset[k] + i] = (pgm_read_byte(&(euc16[hits[k]][i])));
    }

    if (offset_buf[k][playing_step[k]] == 1 && mute[k] == 1 && k == select_ch) {
      usbMIDI.sendNoteOn(notes[k], 127, k+1);
      pixels.setPixelColor(LED[k]-1, 0xFFFFFF);
    }
    else if (offset_buf[k][playing_step[k]] == 1 && mute[k] == 1) {
      usbMIDI.sendNoteOn(notes[k], 127, k+1);
      pixels.setPixelColor(LED[k]-1, 0x1F1F1F);
    }
    else if(mute[k] == 0 && k == select_ch){
      pixels.setPixelColor(LED[k]-1, 0xFF0000);
    }
    else if(mute[k] == 0){
      pixels.setPixelColor(LED[k]-1, 0x1F0000);
    }
    else{
      pixels.setPixelColor(LED[k]-1, 0x0);
    }
    if  (gate_timer + 10 <= millis()) { 
      usbMIDI.sendNoteOn(notes[k], 0, k+1);
    }
  }
}

void seqDisplay() {
  trg_in = 1;//digitalRead(CLOCK_IN);
  if (old_trg_in == 0 && trg_in == 1) {
    gate_timer = millis();
    for (int i = 0; i < 8; i++) {
      playing_step[i]++;
      if (playing_step[i] >= limit[i]) {
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
      //j++;
      //long int t2 = millis();
    }
    display.clearDisplay();
}
