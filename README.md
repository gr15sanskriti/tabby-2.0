# tabby-2.0
writing table using Arduino Uno and Thin Film Tranistor LCD
The driver ofThin-flim-Transistor used in this project was 0x9341 and the Adaruit library does not provide readpixel code for this particular driver.
If you are using the below driver then you can directly use the adafruit library otherwise you need to download the modified version of library given in the repository
with the name 'SPDF ADAFRUIT MASTER LIBRARY' or follow the below steps to manually add the 'readpixel' code in your ADAFRUIT library.
(i) first navigate to the arduino document in your windows,it might be located in  c ->user ->admin ->documents ->arduino
(ii) then go to libraries and look for the adafruit library and find the Adafruit library in it
(iii) once you are able to locate the file then you have to look for ADAFRUIT_TFTLCD.cpp file and open it with text editor
(iv) now find the 'readpixel' code in the .cpp file it looks like "uint16_t Adafruit_TFTLCD::readPixel(int16_t x, int16_t y)"
(V) you will find the code is available for driver == ID_932X and driver == ID_7575
(vi) now you need to add the code for your driver ILI9341, just copy and paste the below code before 'else,return'command ( or refer attached image)

(vii)code.....

else if(driver == ID_9341) {

    uint8_t r, g, b;
#define ILI9341_MEMORYREAD         0x2E
    setAddrWindow( x,y,x,y);
    CS_ACTIVE;
    CD_COMMAND;
    write8( ILI9341_MEMORYREAD);
    setReadDir();  // Set up LCD data port(s) for READ operations
    CD_DATA;
    read8(r);      // First byte back is a dummy read
    read8(r);
    read8(g);
    read8(b);
    setWriteDir(); // Restore LCD data port(s) to WRITE configuration
    CS_IDLE;
    return (((uint16_t)r & B11111000) << 8) |
           (((uint16_t)g & B11111100) << 3) |
           (           b              >> 3);
  } 


(viii) now press cltr+c , and your library is ready to use 


