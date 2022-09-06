#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>
#include <SPI.h>
#include <SD.h>


#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif

#define YP A1 //
#define XM A2
#define YM 7
#define XP 6



// Calibrate values
#define TS_MINX 940
#define TS_MINY 120
#define TS_MAXX 150
#define TS_MAXY 970

//the resistance between X+ and X-, Used multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
#define bmpDraw
//  readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 2
#define ERASE  4
#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define SD_CS 53
int oldcolor, currentcolor;
int oldfd , currentfd;
int currentpage;


//**WAIT ONE TOUCH**//

TSPoint waitOneTouch() {

  TSPoint p;
  do {
    p = ts.getPoint();

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

  } while ((p.z < MINPRESSURE ) || (p.z > MAXPRESSURE));

  return p;
}

//**MENU SCREEN **//

void screen()
{
  tft.fillScreen(BLACK);

  tft.fillRect(0, 0, BOXSIZE * 9, BOXSIZE, RED);
  tft.setCursor (10, 10);
  tft.setTextSize (3);
  tft.setTextColor(WHITE);
  tft.println("ENGLISH");

  tft.fillRect(0, BOXSIZE, BOXSIZE * 9, BOXSIZE, WHITE);
  tft.setCursor (0, BOXSIZE);
  tft.setTextSize (3);
  tft.setTextColor(RED);
  tft.println("HINDI");

  tft.fillRect(0, BOXSIZE * 2, BOXSIZE * 9, BOXSIZE, GREEN);
  tft.setCursor (0, BOXSIZE * 2);
  tft.setTextSize (3);
  tft.setTextColor(RED);
  tft.println("MATHS");
}


//**SUB MENU SCREEN**//

void english ()
{ currentcolor = BLUE;
  tft.drawRoundRect(0, 0, 320, 480, 10, WHITE);
  tft.fillScreen(WHITE);
  tft.setCursor (20, 20);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println("ENGLISH NOTEBOOK");
  back();
  erase();
  pen();
  save();
}

void hindi ()
{
  tft.drawRoundRect(0, 0, 320, 480, 10, WHITE);
  currentcolor = BLUE;
  tft.fillScreen(WHITE);
  tft.setCursor (20, 20);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println("HINDI NOTEBOOK");
  back();
  erase();
  pen();
  save();
}

void maths()
{
   currentcolor = BLUE;
  tft.drawRect(0, 0, 320, 480, YELLOW);
  tft.fillScreen(WHITE);
  tft.setCursor (20, 20);
  tft.setTextSize (2);
  tft.setTextColor(BLACK);
  tft.println("MATHS NOTEBOOK");
  back();
  erase();
  pen();
  save();
}


//**REQUIRED BUTTONS ON SCREEN*//

void erase() {
  tft.drawRoundRect(BOXSIZE * 3, 435, BOXSIZE * 2 , BOXSIZE , 8, WHITE);
  tft.fillRoundRect(BOXSIZE * 3, 435, BOXSIZE * 2 , BOXSIZE , 8, BLACK);
  tft.setCursor (140, 450);
  tft.setTextSize (1);
  tft.setTextColor(WHITE);
  tft.println("ERASE ");

}

void pen() {
  tft.drawRoundRect(BOXSIZE * 6, 435, BOXSIZE * 2, BOXSIZE , 8, WHITE);
  tft.fillRoundRect(BOXSIZE * 6, 435, BOXSIZE * 2 , BOXSIZE , 8, BLACK);
  tft.setCursor (260, 450);
  tft.setTextSize (1);
  tft.setTextColor(WHITE);
  tft.println("PEN ");

}
void save()  {
  tft.drawRoundRect(BOXSIZE * 6 , 0 , BOXSIZE * 2 , BOXSIZE , 8 , WHITE);
  tft.fillRoundRect(BOXSIZE * 6, 0, BOXSIZE * 2 , BOXSIZE , 8, BLACK);
  tft.setCursor (260, 20);
  tft.setTextSize (1);
  tft.setTextColor(WHITE);
  tft.println("SAVE ");
}

void back() {
  tft.drawRoundRect(0, 435, BOXSIZE * 2, BOXSIZE , 8, WHITE);
  tft.fillRoundRect(0, 435, BOXSIZE * 2 , BOXSIZE , 8, BLACK);
  tft.setCursor (20, 450);
  tft.setTextSize (1);
  tft.setTextColor(WHITE);
  tft.println("BACK ");
}


//**SD CARD INTIALISATION**//

void sdcard()
{
  Serial.begin(9600);

  tft.reset();
  tft.begin(0x9325);

  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_CS))
  {
    Serial.println(F("failed"));
    return;
  }
}


//**SAVING IN SD CARD**//

File bmpFile;
void writeTwo (uint16_t word) {
  bmpFile.write(word & 0xFF); bmpFile.write((word >> 8) & 0xFF);
}

// Write four bytes, least significant byte first
void writeFour (uint32_t word) {
  bmpFile.write(word & 0xFF); bmpFile.write((word >> 8) & 0xFF);
  bmpFile.write((word >> 16) & 0xFF); bmpFile.write((word >> 24) & 0xFF);
}

void bmpSave() {

  // Write two bytes, least significant byte first
  uint32_t filesize, offset;
  uint16_t width = tft.width(), height = tft.height();
  char filename[12] = "image1.bmp";
  SD.begin();
  while (SD.exists(filename)) {
    filename[5]++;
  }
  bmpFile = SD.open(filename, FILE_WRITE);
  // On error hang up
  if (!bmpFile) for (;;);
  digitalWrite(LED_BUILTIN, HIGH);
  if (bmpFile) {
    // File header: 14 bytes
    bmpFile.write('B'); bmpFile.write('M');
    writeFour(14 + 40 + 12 + width * height * 2); // File size in bytes
    writeFour(0);
    writeFour(14 + 40 + 12);            // Offset to image data from start
    Serial.print("\n Writing");
    // Image header: 40 bytes
    writeFour(40);                      // Header size
    writeFour(width);                   // Image width
    writeFour(height-1);                // Image height
    writeTwo(1);                        // Planes
    writeTwo(16);                       // Bits per pixel
    writeFour(0);                       // Compression (none)
    writeFour(0);                       // Image size (0 for uncompressed)
    writeFour(0);                       // Preferred X resolution (ignore)
    writeFour(0);                       // Preferred Y resolution (ignore)
    writeFour(0);                       // Colour map entries (ignore)
    writeFour(0);                       // Important colours (ignore)
    //
    // Colour masks: 12 bytes
    writeFour(0b0000011111100000);      // Green
    writeFour(0b1111100000000000);      // Red
    writeFour(0b0000000000011111);      // Blue
    //
    // Image data: width * height * 2 bytes
    for (int y = height-2; y >= 0; y--) {
      for (int x = 0; x < width; x++) {
        writeTwo(tft.readPixel(x, y));
        // Each row must be a multiple of four bytes
      }
    }
    Serial.print("\n reading pixel done ");
    // Close the file
    bmpFile.close();
    digitalWrite(LED_BUILTIN, LOW);

    Serial.print(" \n done saving ");
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}



//*****************//
//**VOID SETUP**//
void setup(void) {
  Serial.begin(9600);
  Serial.print("Initialzing SD card...");
  pinMode(10, OUTPUT);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  Serial.begin(9600);
  Serial.println(F("Paint!"));
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(0);

  //initial screen
  tft.fillScreen(WHITE);
  // Initial screen
  tft.setCursor (75, 90);
  tft.setTextSize (4);
  tft.setTextColor(MAGENTA);
  tft.println("WRITING");
  tft.setCursor (130, 150);
  tft.setTextSize (4);
  tft.setTextColor(MAGENTA);
  tft.println("PAD");
  tft.setCursor (110, 250);
  tft.setTextSize (1);
  tft.setTextColor(BLACK);
  tft.println("Touch to proceed");
  currentpage = 0;
  waitOneTouch();
  tft.fillScreen(BLACK);
  screen();
  pinMode(13, OUTPUT);

}




//**************//
//**VOID LOOP **//

void loop()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (currentpage == 0)
  {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());

      // *** SPFD5408 change -- End
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
      if (p.y < BOXSIZE) {
        Serial.println("ENGLISH");

        tft.fillRoundRect(60, 180, 200, 40, 8, WHITE);
        delay(70);
        tft.fillRoundRect(60, 180, 200, 40, 8, RED);
        tft.drawRoundRect(60, 180, 200, 40, 8, WHITE);
        tft.setCursor(80, 250);
        tft.println("loading....");
        delay(70);
        currentpage = 1;
        english();
      }


      else if (p.y < BOXSIZE * 2)
      {
        Serial.println("HINDI");
        currentpage = 2;
        tft.fillRoundRect(60, 180, 200, 40, 8, WHITE);
        delay(70);

        tft.fillRoundRect(60, 180, 200, 40, 8, RED);
        tft.drawRoundRect(60, 180, 200, 40, 8, WHITE);

        tft.setCursor(80, 250);
        tft.print("loading....");
        delay(70);

        hindi();
      }

      else  if (p.y < BOXSIZE * 3)
      {
        Serial.println("MATHS");

        currentpage = 3;

        tft.fillRoundRect(60, 180, 200, 40, 8, WHITE);
        delay(70);

        tft.fillRoundRect(60, 180, 200, 40, 8, RED);
        tft.drawRoundRect(60, 180, 200, 40, 8, WHITE);

        tft.setCursor(80, 250);
        tft.print("loading....");
        delay(70);

        maths();
      }
      else if (p.y > BOXSIZE * 3)
      { screen();
      }
    }
  }

  if (currentpage == 1)
  {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

      if ((p.y > 435) && (p.x < BOXSIZE * 2 ) && (p.x > 0))
      { screen();
        currentpage = 0; // BACK
      }
      else if ((p.y > 435) && (p.x < BOXSIZE * 8 ) && (p.x > BOXSIZE * 6))
      {
        currentcolor = BLUE;
        //PEN
      }
      else if ((p.x < BOXSIZE * 5) && (p.x > BOXSIZE * 3) && (p.y > 435))
      { currentcolor = WHITE;
        //ERASER
      }
      else if ((p.y > 0) && (p.y < 40) && ( p.x < BOXSIZE * 8) && ( p.x > BOXSIZE * 6))
      {
         tft.setCursor (120, 420);
        tft.setTextSize (2);
        tft.setTextColor(RED);
        tft.println("Saving...");
        bmpSave(); //save
       
        tft.fillRoundRect(120, 420, 120, 20, 6, WHITE);
        tft.drawRoundRect(120, 420, 120, 20, 6, WHITE);
        tft.setCursor (120, 420);
        tft.setTextSize (2);
        tft.setTextColor(RED);
        tft.println("DONE SAVING");
        delay(1000);
        currentpage = 1;
        english();

      }

    }

    if (((p.y - PENRADIUS) > 0) && ((p.y + PENRADIUS) < tft.height() - 45)) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);   //(height -45 since we are using y = 435 so 480 - 435 = 45 , pen will not draw in that area)
    }

  }


  if (currentpage == 2)
  {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

      if ((p.y > 435) && (p.x < BOXSIZE * 2 ) && (p.x > 0))
      {
        screen();
        currentpage = 0; // BACK
      }
      else if ((p.y > 435) && (p.x < BOXSIZE * 8 ) && (p.x > BOXSIZE * 6))
      {
        currentcolor = BLUE;
        //PEN
      }
      else if ((p.x < BOXSIZE * 5) && (p.x > BOXSIZE * 3) && (p.y > 435))
      {
        currentcolor = WHITE;
        //ERASER
      }
      else if ((p.y > 0) && (p.y < 40) && ( p.x < BOXSIZE * 8) && ( p.x > BOXSIZE * 6))
      {
        tft.setCursor (120, 420);
        tft.setTextSize (2);
        tft.setTextColor(RED);
        tft.println("Saving...");
        bmpSave();//save
          tft.fillRoundRect(120, 420, 120, 20, 6, WHITE);
        tft.drawRoundRect(120, 420, 120, 20, 6, WHITE);
        tft.setCursor (120, 420);
        tft.setTextSize (2);
        tft.setTextColor(RED);
        tft.println("DONE SAVING");
        delay(1000);
        currentpage = 2;
        hindi();
      }

    }

    if (((p.y - PENRADIUS) > 0) && ((p.y + PENRADIUS) < tft.height() - 45)) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }


  }

  if (currentpage == 3)
  {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());

      if ((p.y > 435) && (p.x < BOXSIZE * 2 ) && (p.x > 0))
      {
        screen();
        currentpage = 0; // BACK
      }
      else if ((p.y > 435) && (p.x < BOXSIZE * 8 ) && (p.x > BOXSIZE * 6))
      {
        currentcolor = BLUE;
        //PEN
      }
      else if ((p.x < BOXSIZE * 5) && (p.x > BOXSIZE * 3) && (p.y > 435))
      {
        currentcolor = WHITE;
        //ERASER
      }
      else if ((p.y > 0) && (p.y < 40) && ( p.x < BOXSIZE * 8) && ( p.x > BOXSIZE * 6))
      {
        tft.setCursor (120, 420);
        tft.setTextSize (2);
        tft.setTextColor(RED);
        tft.println("Saving...");
        bmpSave();//save
          tft.fillRoundRect(120, 420, 120, 20, 6, WHITE);
        tft.drawRoundRect(120, 420, 120, 20, 6, WHITE);
        tft.setCursor (120, 420);
        tft.setTextSize (2);
        tft.setTextColor(RED);
        tft.println("DONE SAVING");
        delay(1000);
        currentpage = 3;
        maths();
      }
    }
    if (((p.y - PENRADIUS) > 0) && ((p.y + PENRADIUS) < tft.height() - 45)) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }
  }
}
