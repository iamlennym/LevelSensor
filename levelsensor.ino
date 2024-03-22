/*
    This sketch will plot the level of a watertank. To add more 'watertanks' simply instantiate
    another instance e.g. : 
    
      LevelSensor TankInstanceName("Gauge Title", PIN_TO_READ, X_AXEL_START_POSITION);

      then call the TankInstanceName.refresh() method in the loop()

      That should be it... ;-)
*/


// #include "U8glib.h" // Include U8glib library
#include <U8g2lib.h>

// Create display and set pins:
//U8GLIB_PCD8544 u8g(13, 11, 10, 9, 8); // (CLK=13, DIN=11, CE=10, DC=9, RST=8)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); 

int delayTime = 100;

class LevelSensor {
  private:
    int sensorPin;
    int xOffset;
    int yOffset;
    int yInterval;
    int maxHeight;
    char *barTitle = NULL;
    float sensorVoltage = 0;
    int bWidth = 12;

    void drawBase() {
      if (barTitle != NULL && strlen(barTitle) > 0)
        u8g.drawStr(xOffset, yOffset-2, barTitle); // Print the graph title
      
      u8g.drawLine(xOffset, yOffset, xOffset, yOffset + maxHeight);  // Side-line

      // This array holds the line marker widths (measure marks)
      int lWidths[] = {8, 3, 6, 3, 8, 3, 6, 3, 8};

      // This loop will draw the actual gauge markers
      int ySpacer = 0;
      for (int i=0; i<9; i++) {
        u8g.drawLine(xOffset, yOffset + ySpacer, xOffset + lWidths[i], yOffset + ySpacer);
        ySpacer += yInterval;
      }

      // This will draw the frame for the gauge
      u8g.drawRFrame(xOffset + 11, yOffset, bWidth, maxHeight, 1); // Draw a rounded square (x,y,width,height,radius)
    }

    void drawBar() {
      int boxHeight = sensorVoltage / 5 * maxHeight;
      u8g.drawRBox(xOffset + 11, yOffset + (maxHeight - boxHeight), bWidth, boxHeight, 0); 
    }

  public:
    // Constructor
    LevelSensor(char *title, int pin = A0, int x = 0, int y = 9, int height = 49) : sensorPin(pin), xOffset(x), yOffset(y), maxHeight(height) {
      pinMode(pin, INPUT);

      // Store the title
      barTitle = malloc (strlen(title)+1);
      strcpy(barTitle, title);

      // Work out the interval for the stripes on the bar graph
      yInterval = maxHeight  / 8;
    }

    void refresh() {
      drawBase();
      sensorVoltage = (5. / 1023.) * analogRead(sensorPin);
      Serial.println(sensorVoltage);
      drawBar();
    }
};

// Define the Pins we read
int FRONT_TANK_PIN = A0;
int MIDDLE_TANK_PIN = A1;
int REAR_TANK_PIN = A2;
int SIDE_TANK_PIN = A3;

// Instantiate the watertank sensor instances
LevelSensor frontTank("Front", FRONT_TANK_PIN, 0);
LevelSensor middleTank("Middle", MIDDLE_TANK_PIN, 35);
LevelSensor rearTank("Rear", REAR_TANK_PIN, 70);
LevelSensor sideTank("Side", SIDE_TANK_PIN, 105);

void setup() // Start of setup
{
  Serial.begin(9600); // initialize serial communication at 9600 bits per second
  u8g.begin(); // Initialize the display
  u8g.setFont(u8g_font_5x7); // Select a font
} // End of setup

void loop() // Start of loop
{
  // Begin displaying the gauges
  u8g.firstPage(); 
  do               
  {
    frontTank.refresh();
    middleTank.refresh();
    rearTank.refresh();
    sideTank.refresh();
  } while (u8g.nextPage()); 
  // End of the display logic

  // Sleep a bit
  delay(delayTime);
}
