# tabby-2.0
The Thin -film-transistor driver used in this project is ILI0x9341 and the Adafruit library does not 'readpixel' code for the driver ILI9341 so everytime you try to 
read pixel in source code a blank bmp image will get saved in SD card. So you need to add readpixel code in the library by your self 
If you are using a below mentioned driver then you can directly use Adafruit_TFTLCD but if you using a driver 0X9341 then you need to add the code for the driver manually.
follow the below steps to add the code for your driver
(i) locate the arduino folder in you machine , it might be located in c-> user -> admin -> documents -> Arduino
(ii) Then go to libraries and try to find the AdafruitTft LCD Library
(iii) Then find Adafruit_TFTLCD.cpp and open it with text editor 
(iv) then find a 'readpixel' code in it , you will find it as "uint16_t Adafruit_TFTLCD::readPixel(int16_t x, int16_t y)"
(v) you will see that the code is designed for driver == ID_932X and driver == ID_7575
(vi) now copy the below code and paster it before 'else, return' statement 

(vii)
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
  
  
  (viii) now press cltr +c and your library is ready to use
