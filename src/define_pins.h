#ifndef define_pins
    #define define_pins

    // Display
    #define OLED_SDA 25
    #define OLED_SCL 26
    #define i2c_Address 0x3c
    
    // Encoder Pinout
    #define PIN_A 35
    #define PIN_B 34
    #define BUTTON 32

    // Buttons Pinout
    #define BUTTON1 15 
    #define BUTTON2 13
    #define BUTTON3 4
    #define BUTTON4 5

    // Leds Pinout
    int LED[4] = {18, 19, 21, 22};

    // External clock
    #define CLOCK_IN 23

#endif