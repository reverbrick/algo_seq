#include <Arduino.h>
#define  ENCODER_OPTIMIZE_INTERRUPTS //countermeasure of encoder noise
#include <RotaryEncoder.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <define_pins.h>

// External clock
bool trg_in = 0; //external trigger in H=1,L=0
bool old_trg_in = 0;
unsigned long gate_timer = 0; //countermeasure of sw chattering

// Initialize Rotary Encoder
int oldPosition  = -999;
int newPosition = -999;
int i = 0;
#define ROTARYMIN 0 // Rotary min range
#define ROTARYMAX sizeof(y16) // Rotary max range

// Sequence variable
byte j = 0;
byte k = 0;
byte m = 0;
byte buf_count = 0;
byte line_xbuf[17];//Buffer for drawing lines
byte line_ybuf[17];//Buffer for drawing lines

// Each circle X, Y coordinates
const byte x16[16] = {74, 84, 91, 96, 98, 96, 91, 84, 74, 64, 57, 52, 50, 52, 57, 64};//Vertex coordinates
const byte y16[16] = {8,  10,  15,  22,  32, 42, 49, 54, 56, 54, 49, 42, 32, 22, 15,  10};//Vertex coordinates

// Global variables store
byte playing_step[6] = {0, 0, 0, 0, 0, 0}; //playing step number , CH1,2,3,4,5,6
bool mute[6] = {0, 0, 0, 0, 0, 0}; //mute 0 = off , 1 = on
uint16_t buttonCounter = 0;
int newButtonVal = 0;
int oldButtonVal = 0;
int currentMenuPos = 0;

// Each channel param
byte hits[6] = { 4, 4, 5, 3, 2, 16};//each channel hits
byte offset[6] = { 0, 2, 0, 8, 3, 9};//each channele step offset

// Initializations
Adafruit_SH1106 display(OLED_SDA, OLED_SCL);
RotaryEncoder encoder(PIN_A, PIN_B, BUTTON);

bool offset_buf[6][16];//offset buffer , Stores the offset result
byte limit[6] = {16, 16, 16, 16, 16, 16};//eache channel max step
byte select_ch = 0; //0~5 = each channel -1 , 6 = random mode
byte bar_now = 1;//count 16 steps, the bar will increase by 1.
byte bar_max[4] = {2, 4, 8, 16} ;//selectable bar
byte bar_select = 1;//selected bar
byte step_cnt = 0;//count 16 steps, the bar will increase by 1.
bool disp_reflesh = 1;//0=not reflesh display , 1= reflesh display , countermeasure of display reflesh bussy

// Euclidian rythm
const static byte euc16[17][16] PROGMEM = {
 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
 {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
 {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
 {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0},
 {1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0},
 {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
 {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
 {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0},
 {1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1},
 {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1},
 {1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
 {1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1},
 {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
 {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
 {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

void selectChannel(int channel){
    if (channel == select_ch){
      mute[channel] = !mute[channel];
    }else if (currentMenuPos == 1){
      encoder.setPosition(hits[channel]);
    }else {
      encoder.setPosition(offset[channel]);
    }
    select_ch = channel;
}

void drawMenu(){

  oldPosition = newPosition;
  newPosition = encoder.getPosition();
  
  if (newPosition <= ROTARYMIN) {
    encoder.setPosition(ROTARYMIN);
    newPosition = ROTARYMIN;
  
  } else if (newPosition >= ROTARYMAX) {
    encoder.setPosition(ROTARYMAX);
    newPosition = ROTARYMAX;
  }
  
  if (oldPosition != newPosition) {
    oldPosition = newPosition;
  }
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("STEP");
  display.setCursor(0, 10);
  display.print("OFFSET");
  display.setCursor(0, 20);
  display.print("NOTE");
  display.setCursor(0, 30);
  display.print("LIMIT");

  
  oldButtonVal = newButtonVal;
  newButtonVal = encoder.getPushButton();

  if (newButtonVal != oldButtonVal && newButtonVal > oldButtonVal){
    if (currentMenuPos >= 4){
      currentMenuPos = 0;
    }
    
    currentMenuPos++;
  }
  switch (currentMenuPos){
    case 1:
      display.drawTriangle(27, 3, 31, 0, 31, 6, WHITE); // Step triangle
      display.fillTriangle(27, 3, 31, 0, 31, 6, WHITE);
      hits[select_ch] = newPosition;
      break;
    case 2:
      display.drawTriangle(37, 13, 42, 10, 42, 16, WHITE); // Offset triangle
      display.fillTriangle(37, 13, 42, 10, 42, 16, WHITE);
      offset[select_ch] = newPosition;
      break;
    case 3:
      display.drawTriangle(27, 23, 31, 20, 31, 26, WHITE); // Note triangle
      display.fillTriangle(27, 23, 31, 20, 31, 26, WHITE);
      break;
    case 4:
      display.drawTriangle(31, 33, 35, 30, 35, 36, WHITE); // Limit triangle
      display.fillTriangle(31, 33, 35, 30, 35, 36, WHITE);
      break;
  }

}

void channelProcess(int select_ch) {
  
  drawMenu();
  
  if (mute[select_ch] == 0){
  
    display.fillRect(0, 53, 35, 12, WHITE);
    display.setCursor(3, 55);
    display.setTextColor(BLACK);
    display.print("MUTED");
  }
  
  buf_count = 0;

  for (m = 0; m < 16; m++) {
    if (offset_buf[select_ch][m] == 1) {
      line_xbuf[buf_count] = x16[m];
      line_ybuf[buf_count] = y16[m];
      buf_count++;
    }
  }
  
  for (j = 0; j < buf_count - 1; j++) {
    display.drawLine(line_xbuf[j], line_ybuf[j], line_xbuf[j + 1], line_ybuf[j + 1], WHITE);
  }
  
  display.drawLine(line_xbuf[0], line_ybuf[0], line_xbuf[j], line_ybuf[j], WHITE);

  buf_count = 0;
  if (hits[select_ch] == 1) {
    display.drawLine(64, 32, x16[offset[select_ch]], y16[offset[select_ch]], WHITE);
  }
}

void ICACHE_RAM_ATTR encoderISR()
{
  encoder.readAB();
}

void ICACHE_RAM_ATTR encoderButtonISR()
{
  encoder.readPushButton();
}

void ICACHE_RAM_ATTR BUTTON1_INT()
{
  selectChannel(0);
}

void ICACHE_RAM_ATTR BUTTON2_INT()
{
  selectChannel(1);
}

void ICACHE_RAM_ATTR BUTTON3_INT()
{
  selectChannel(2);
}

void ICACHE_RAM_ATTR BUTTON4_INT()
{
  selectChannel(3);
}

void setup() {
  encoder.begin();

  pinMode(CLOCK_IN, INPUT); // external clock
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
  
  for (i = 0; i < sizeof(LED[4]); i++){
    pinMode(LED[i], OUTPUT); // each channel
  }
 
  attachInterrupt(digitalPinToInterrupt(PIN_A),  encoderISR,       CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON), encoderButtonISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON1), BUTTON1_INT, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON2), BUTTON2_INT, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON3), BUTTON3_INT, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON4), BUTTON4_INT, FALLING);
 
  Serial.begin(115200);
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void drawCircle() {
  k = select_ch;
  display.setTextColor(WHITE);
  display.print(k+1);
  display.setCursor(105,0);
  display.print("CH");
  
  //draw step dot
  for (j = 0; j <= limit[k] - 1; j++) { // j = steps
    display.drawPixel(x16[j], y16[j], WHITE);
  }

  if (offset_buf[k][playing_step[k]] == 0) {
    display.drawCircle(x16[playing_step[k]], y16[playing_step[k]], 2, WHITE);
  }

  if (offset_buf[k][playing_step[k]] == 1 && mute[k]!= 0) {
    display.fillCircle(x16[playing_step[k]], y16[playing_step[k]], 3, WHITE);
  }

  if (mute[k] == 0) {
    display.drawCircle(x16[playing_step[k]], y16[playing_step[k]], 2, WHITE);
  }
 
  for (int x = 0; x < sizeof(LED[4]); x++){
    if (x == select_ch){
      channelProcess(select_ch);
    }
  }

  for (k = 0; k < 5; k++){
    for (i = offset[k]; i <= 15; i++) {
      offset_buf[k][i - offset[k]] = (pgm_read_byte(&(euc16[hits[k]][i]))) ;
    }
  
    for (i = 0; i < offset[k]; i++) {
      offset_buf[k][16 - offset[k] + i] = (pgm_read_byte(&(euc16[hits[k]][i])));
    }

    if (offset_buf[k][playing_step[k]] == 1 && mute[k] == 1) {
      digitalWrite(LED[k], HIGH);
    }
    
    else{
      digitalWrite(LED[k], LOW);
    }
  }
}

void seqDisplay() {
  trg_in = digitalRead(CLOCK_IN);
  if (old_trg_in == 0 && trg_in == 1) {
    display.clearDisplay();
    display.setCursor(120, 0);
    gate_timer = millis();
    for (i = 0; i <= 5; i++) {
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
  newPosition = encoder.getPosition();
  seqDisplay();
}