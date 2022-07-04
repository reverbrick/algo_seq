//Encoder setting
#define  ENCODER_OPTIMIZE_INTERRUPTS //countermeasure of encoder noise
#include <RotaryEncoder.h>
#include<Wire.h>
#include<Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

// Encoder Pinout
#define PIN_A 35
#define PIN_B 34
#define BUTTON 32

// Display Pinout
#define OLED_SDA 25
#define OLED_SCL 26
#define i2c_Address 0x3c

// Buttons Pinout
#define BUTTON1 15 
#define BUTTON2 13
#define BUTTON3 4
#define BUTTON4 5

// Leds Pinout

int LED[4] = {18, 19, 21, 22};

// Clock in Pinout
#define CLOCK_IN 23

// Initialize Oled Display
Adafruit_SH1106 display(25, 26);

// Initialize Rotary Encoder
uint16_t buttonCounter = 0;
RotaryEncoder encoder(PIN_A, PIN_B, BUTTON);
int oldPosition  = -999;
int newPosition = -999;
int i = 0;

//push button
bool sw = 0;//push button
bool old_sw;//countermeasure of sw chattering
unsigned long sw_timer = 0;//countermeasure of sw chattering

//Sequence variable
byte j = 0;
byte k = 0;
byte m = 0;
byte buf_count = 6;

const byte graph_x[6] = {0, 40, 80, 15, 55, 95};//each chanel display offset
const byte graph_y[6] = {0, 0,  0,  32, 32, 32};//each chanel display offset

byte line_xbuf[17];//Buffer for drawing lines
byte line_ybuf[17];//Buffer for drawing lines

// OLD BYTES const byte x16[16] = {15,  21, 26, 29, 30, 29, 26, 21, 15, 9,  4,  1,  0,  1,  4,  9};//Vertex coordinates
// OLD BYTES const byte y16[16] = {0,  1,  4,  9,  15, 21, 26, 29, 30, 29, 26, 21, 15, 9,  4,  1};//Vertex coordinates
const byte x16[16] = {64, 74, 81, 86, 88, 86, 81, 74, 64, 54, 47, 42, 40, 42, 47, 54};//Vertex coordinates
const byte y16[16] = {8,  10,  15,  22,  32, 42, 49, 54, 56, 54, 49, 42, 32, 22, 15,  10};//Vertex coordinates

bool offset_buf[6][16];//offset buffer , Stores the offset result
byte playing_step[6] = {0, 0, 0, 0, 0, 0}; //playing step number , CH1,2,3,4,5,6
byte limit[6] = {16, 16, 16, 16, 16, 16};//eache channel max step


byte select_ch = 0; //0~5 = each channel -1 , 6 = random mode

bool mute[6] = {0, 0, 0, 0, 0, 0}; //mute 0 = off , 1 = on

bool trg_in = 0;//external trigger in H=1,L=0
bool old_trg_in = 0;

unsigned long gate_timer = 0;//countermeasure of sw chattering


byte bar_now = 1;//count 16 steps, the bar will increase by 1.
byte bar_max[4] = {2, 4, 8, 16} ;//selectable bar
byte bar_select = 1;//selected bar
byte step_cnt = 0;//count 16 steps, the bar will increase by 1.

bool disp_reflesh = 1;//0=not reflesh display , 1= reflesh display , countermeasure of display reflesh bussy

const static byte euc16[17][16] PROGMEM = {//euclidian rythm
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

//each channel param
byte hits[6] = { 4, 4, 5, 3, 2, 16};//each channel hits
byte offset[6] = { 0, 2, 0, 8, 3, 9};//each channele step offset

void ICACHE_RAM_ATTR encoderISR()
{
  encoder.readAB();
}

void selectChannel(int channel){
    if (channel == select_ch){
      mute[channel] = !mute[channel];
    }
    select_ch = channel;
    
}

void channelProcess(int select_ch) {
  if (mute[select_ch] == 0){
    display.setCursor(0, 0);
    display.print("Mute");
  }
  k = select_ch;
  buf_count = 0;
   for (m = 0; m < 16; m++) {
     if (offset_buf[k][m] == 1) {
       line_xbuf[buf_count] = x16[m];// + graph_x[k];//store active step
       line_ybuf[buf_count] = y16[m];// + graph_y[k];
       buf_count++;
     }
   }

   for (j = 0; j < buf_count - 1; j++) {
     display.drawLine(line_xbuf[j], line_ybuf[j], line_xbuf[j + 1], line_ybuf[j + 1], WHITE);
   }
   display.drawLine(line_xbuf[0], line_ybuf[0], line_xbuf[j], line_ybuf[j], WHITE);
   
   buf_count = 0;
   if (hits[k] == 1) {
     display.drawLine(15, 15, x16[offset[k]], y16[offset[k]], WHITE);
   }
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

 pinMode(LED[0], OUTPUT); //CH1
 pinMode(LED[1], OUTPUT); //CH2
 pinMode(LED[2], OUTPUT); //CH3
 pinMode(LED[3], OUTPUT); //CH4
 
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
   //draw step dot
   k = select_ch;

   display.print(k + 1);
   
   for (j = 0; j <= limit[k] - 1; j++) { // j = steps
     display.drawPixel(x16[j], y16[j], WHITE);
   }
 
   if (offset_buf[k][playing_step[k]] == 0) {
     display.drawCircle(x16[playing_step[k]], y16[playing_step[k]], 2, WHITE);
   }
   if (offset_buf[k][playing_step[k]] == 1) {
     display.fillCircle(x16[playing_step[k]], y16[playing_step[k]], 3, WHITE);
   }
   
   switch (select_ch) {
     case 0://CH1
       //clearLeds();
       channelProcess(select_ch);
       //digitalWrite(LED1, HIGH);
       break;
     case 1://CH2
       //clearLeds();
       channelProcess(select_ch);
       //digitalWrite(LED2, HIGH);
       break;

     case 2://CH3
       //clearLeds();
       channelProcess(select_ch);
       //digitalWrite(LED3, HIGH);
       break;

     case 3://CH4
       //clearLeds();
       channelProcess(select_ch);
       //digitalWrite(LED4, HIGH);
       break;
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

   trg_in = digitalRead(CLOCK_IN);//external trigger in
   if (old_trg_in == 0 && trg_in == 1) {
     display.clearDisplay();
     display.setCursor(120, 0);
     gate_timer = millis();
     for (i = 0; i <= 5; i++) {
       playing_step[i]++;      //When the trigger in, increment the step by 1.
       if (playing_step[i] >= limit[i]) {
         playing_step[i] = 0;  //When the step limit is reached, the step is set back to 0.
       }
     }

     drawCircle();
    
     disp_reflesh = 1;//Updates the display where the trigger was entered.If it update it all the time, the response of gate on will be worse.
     
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