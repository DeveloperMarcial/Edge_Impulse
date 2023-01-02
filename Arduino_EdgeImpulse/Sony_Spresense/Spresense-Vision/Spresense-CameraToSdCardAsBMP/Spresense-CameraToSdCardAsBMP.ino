/* 
 ***************************************************************************************
 * The MIT License (MIT)                                                               *
 *                                                                                     *
 * The software saved herin as the Software "Spresense-CameraToSdCardAsBMP.ino"        *
 * Copyright (c) 2022 M. Marcial                                                       *
 * All Rights Reseved                                                                  *
 * If you use any or all of this code please give me credit.                           * 
 *                                                                                     * 
 * Some help was gleaned from here:                                                    * 
 * https://elchika.com/article/0eaf6193-cb30-4be5-80f4-54d33cc01946/                   * 
 *                                                                                     *
 * Permission is hereby granted, free of charge, to any person obtaining a copy        *
 * of this software and associated documentation files (the "Software"), to deal       *
 * in the Software without restriction, including without limitation the rights        *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell           *
 * copies of the Software, and to permit persons to whom the Software is               *
 * furnished to do so, subject to the following conditions:                            *
 * The above copyright notice shall be included in                                     *    
 * all copies or substantial portions of the Software.                                 *
 *                                                                                     *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR          *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,            *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE         *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER              *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,       *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE       *
 * SOFTWARE.                                                                           *
 ***************************************************************************************
 */

// This program shows you how to capture a frame from the Sony Spresense with Camera module and save the output as a bitmap image.
// It will allow you to see the output directly on your computer without using any third party tools.
//
//  Sketch uses            272,160 bytes (17%) of program storage space.
//  Maximum is           1,572,864 bytes.
//  Global variables use   272,160 bytes (17%) of dynamic memory,
//  leaving              1,300,704 bytes for local variables.
//  Maximum is           1,572,864 bytes.

// *****************
// *** PICK ONE ****
// *****************
//#define BMP_COLOR
#define BMP_GRAY
// *****************
// *****************
// *****************

#if defined(BMP_COLOR) && defined(BMP_GRAY)
#error Please select either BMP_COLOR or BMP_GRAY
#endif

#ifndef BMP_COLOR
#ifndef BMP_GRAY
#error Please select either BMP_COLOR or BMP_GRAY. Currently neither is #defined.
#endif
#endif

#include <Camera.h>                                       // Sony's camera library. Note thats a capital C.
#include <SDHCI.h>                                        // An SD card is required!
                                                          //    The SD card holds the DSP files used for decoding if use
                                                          //    audio playback or encoding if you use audio recorder.
                                                          //    Secure Digital Host Controller Interface driver for using the SD card.
                                                          //    The SDHCI library has an API structure similar to the Arduino SD library,
                                                          //    and it can be easily ported from the existing sketch using the SD library.
                                                          //    SD and SDHC cards are supported, but SDXC cards is not supported.
                                                          //    Formats of FAT12, FAT16 and FAT32 are supported, exFAT is not supported.
                                                          //    Capacity of up to 32 GB (I verified) is supported. <--Sony docs say 16 GB.

#define FLOW                                              // Used to print when entering a routine so we can see the flowchart.

// Bitmap File Format
// The bitmap binary file needs to contain some information in order to tell the computer for example the resolution
// of the picture and the bit-depth (bpp). Bit depth refers to the color information stored in the image.
// The higher the bit depth of an image, the more colors it can store.
// As the bit depth increases, the file size of the image also increases, because more color information has to be stored for each pixel in the image.
#define IMAGE_HEIGHT (unsigned int)240
#define IMAGE_WIDTH (unsigned int)320
// Grayscale Example)
// Image data	76800 (=320 x 240) Bytes.	The raw image data, in this case each pixel has 8 bits (1 Byte) and the resolution is 320 x 240, with no compression.
// The final size of the file is 77,879 Bytes.
//                               77,879  - 76,800 =             1079 Bytes
//                                DIB	                            14 Bytes
//                                File Header	                    40 Bytes	
//                                Palette (Grayscale Color Map)	1025 Bytes

// BMP Color Info
#ifdef BMP_COLOR
#define IMAGE_DATA_SIZE (76800*2)
#define BITS_PER_PIXEL (unsigned int)16                                // Needed for RGB565    BMP headers.
#define PALETTE_SIZE  (unsigned int)(3 * 4)                            // RGB565: 3 colors of 4 bytes each.
#else
#define IMAGE_DATA_SIZE (76800)
#define BITS_PER_PIXEL (unsigned int)8                                 // Needed for Grayscale BMP headers.
#define PALETTE_COLORS_AMOUNT (unsigned int)(pow(2, BITS_PER_PIXEL))  // 1025 Bytes: This header is mandatory on bitmaps with a bpp ≤ 8, setting the grayscale.
#define PALETTE_SIZE  (unsigned int)(PALETTE_COLORS_AMOUNT * 4)       //    4 Bytes: 32-bit per color (3 bytes RGB and 1 byte 0x00).
#endif

// BMP Headers Info
#define BITMAP_FILE_HEADER_SIZE (unsigned int)14  // For storing general information about the bitmap image file.
                                                  // 14 Bytes:	Bitmap information, setting the size of the file.

#define DIB_HEADER_SIZE (unsigned int)40          // For storing information about the image and define the pixel format.
                                                  // 40 Bytes: This header requires the resolution and the bpp (bit-depth).

#define HEADER_SIZE (BITMAP_FILE_HEADER_SIZE + DIB_HEADER_SIZE)  //= 14 + 40

static CamImage sized_img;                        //Global: Instatiate a Sony class to control the image from the camera.
                                                  //        The resized image is stored in sized_img.
                                                  //        Instantiated class img is a local variable in CamCB().
                                                  
SDClass theSD;                                    // SD card access

bool    holdOffCamCB       = false;               // Global: Don't let CamCB() modify "sized_img" while we copy it contents to paintAddy[].

bool    startCapture       = false;               // Global: Allows exiting from setup() and servicing the images streaming 
                                                  // and being captured in the callback CamCB().

uint8_t take_picture_count = 0;                   // Global: Used to make unique filenames sored on the SD card.

uint8_t paintAddy[IMAGE_DATA_SIZE];               // Global: This array is a copy of the captured camera data held in the instantiated CamImage img 
                                                  //         a local variable in CamCB(). 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      CAMERA HELPER ROUTINES                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************************************
 * @brief      Print Camera Error Message
 * @details    
 * @param[in]  err, error number 
 * @return     void
*******************************************************************************************************************/
void printCamErr(enum CamErr err)
{
  #ifdef FLOW
  printf("[FLOW] Entering printCamErr(%d)...\n", err);
  #endif
  
  printf("Error: ");
  switch (err)
  {
    case CAM_ERR_NO_DEVICE:
      printf("No Device.\n");
      break;
    case CAM_ERR_ILLEGAL_DEVERR:
      printf("Illegal device error.\n");
      break;
    case CAM_ERR_ALREADY_INITIALIZED:
      printf("Already initialized.\n");
      break;
    case CAM_ERR_NOT_INITIALIZED:
      printf("Not initialized.\n");
      break;
    case CAM_ERR_NOT_STILL_INITIALIZED:
      printf("Still picture not initialized.\n");
      break;
    case CAM_ERR_CANT_CREATE_THREAD:
      printf("Failed to create thread.\n");
      break;
    case CAM_ERR_INVALID_PARAM:
      printf("Invalid parameter.\n");
      break;
    case CAM_ERR_NO_MEMORY:
      printf("No memory.\n");
      break;
    case CAM_ERR_USR_INUSED:
      printf("Buffer already in use.\n");
      break;
    case CAM_ERR_NOT_PERMITTED:
      printf("Operation not permitted.\n");
      break;
    default:
      printf("Unknown camera error.\n");
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      SPRESENSE VISION ROUTINES                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************************************
 * @brief   Callback that checks for the presence of an image in the camera preview window.
 * @details 
 * @param
 * @return  void
 *******************************************************************************************************************/
void CamCB(CamImage img)
{
  #ifdef FLOW
  printf("[FLOW] Entering CamCB()...\n");
  #endif

  ///printf("CamImage img: Image Width      =%d\n", img.getWidth());        //          320
  ///printf("CamImage img: Image Height     =%d\n", img.getHeight());       //          240
  ///printf("CamImage img: Image Size       =%d\n", img.getImgSize());      //      153 600
  ///printf("CamImage img: Image Format     =%d\n", img.getPixFormat());    //1 498 831 189  
  ///printf("CamImage img: Image Buffer Size=%d\n", img.getImgBuffSize());  //      153 600
  ///printf("CamImage img: Image Avaliable  =%d\n", img.isAvailable());     //            1
  
  if (!img.isAvailable())
  {
    // If an error occurs during photography, it returns an empty CamImage.
    // To determine whether an instance of CamImage is empty,
    // you can check it with isAvailable() of CamImage class.    
    printf("In CamCB(): Image is no longer ready...\n");
    return; // Fast path if image is no longer ready.
  } 
  CamErr err;
  ///printf("In CamCB(): New frame processing...\n");  

  /* ***
  // Clip image is size is to big from camera and a downstream routine needs a smaller size.
  // Clip and resize Image with HW 2D accelerator.
  //    Clip and resize the image with 2D accelerator HW in CXD5602.
  //    First, clip the area specified by the arguments
  //       (#lefttop_x, #lefttop_y) - (#rightbottom_x, # rightbottom_y)
  //       for the original image and
  //    Specify the clipped image with arguments
  //       (#width, # height) resize to the size you made.
  //    The resized image is stored in the CamImage instance specified as the first argument
  //    with new image buffer created internally.
  //    If any error occured such as zero size case, this returns error code.
  //    This HW accelerator has limitation for resizing as below:
  //       Minimum width and height is 12 pixels.
  //       Maximum width  is  768 pixels.
  //       Maximum height is 1024 pixels.
  //       Resizing magnification is 2^n or 1/2^n, and resized image size must be integer.
  err = img.clipAndResizeImageByHW(sized_img,         // CamImage &img,
                                   0,                 // int 	     lefttop_x,
                                   0,                 // int 	     lefttop_y,
                                   IMAGE_WIDTH  - 1,  // int 	     rightbottom_x,
                                   IMAGE_HEIGHT - 1,  // int 	     rightbottom_y,
                                   IMAGE_WIDTH,       // int       width.
                                   IMAGE_HEIGHT);     // int       height.
  *** */
  
  // Resize YUV422 image into a known size so we can write a BMP with its required headers.
  err = img.resizeImageByHW(       sized_img,         // CamImage &img,
                                   IMAGE_WIDTH,       // int       width.
                                   IMAGE_HEIGHT);     // int       height.
  if (err)
  {
    printf("ERR: New frame processing failed. See clipAndResizeImageByHW()...\n");      
    printCamErr(err);
  }
  
  #ifdef BMP_COLOR
  //printf("Convert to color...\n");
  err = sized_img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);  // Change Save Format.
  #else
  while (holdOffCamCB)
  {
    delay(1);
  }
  //printf("Convert to grayscale...\n");
  err = sized_img.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);
  #endif

  if (err)
  {   
    printf("ERR: Converting processing failed. See convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY)...\n");      
  } 
}
/*******************************************************************************************************************
 * @brief      Initialize the camera for continuous monitoring of video feed.
 * @details    
 * @param[in]  doPrintCamErr, To show the camera errors or not.
 * @return     void
 *******************************************************************************************************************/
void camera_start_continuous(bool doPrintCamErr)
{
  #ifdef FLOW
  printf("[FLOW] Entering camera_start_continuous()...\n");
  #endif
  
  // Start the camera.
  CamErr err;

  /* NOTE: The last optional argument in theCamera.begin() and theCamera.setStillPictureImageFormat() is 
           "int jpgbufsize_divisor = 7" that reduces the size of the captured JPEG.

           If the acquisition of CamImage fails in JPEG cases, it is possible that the capacity of the JPEG
           buffer size set by setStillPictureImageFormat() is insufficient.
           You can increase the JPEG buffer size by reducing the value of the function parameter "jpgbufsize_divisor".
           Also, if the overall memory capacity is insufficient and the buffer size cannot be obtained,
           it may be possible to solve it by expanding the memory used by the MainCore application.
  */
  //printf("Starting the camera:\n");

  // The begin() method function is the first function to call when using "theCamera".
  // Start "theCamera" with
  //    Frame Rate = 5 images per second
  //    Image data pixel format = YUV 422
  // YUV 422 is a YUV model that defines one luminance component (Y) meaning physical linear-space brightness,
  // and two chrominance components, called U (blue projection) and V (red projection) respectively, aka YCbCr.
  err = theCamera.begin(1,                           // Number of image buffers. 
                        CAM_VIDEO_FPS_5,             // Defined in "camera.h". Default: 30 FPS
                        IMAGE_WIDTH,                 // Defined in this file.  Default: QVGA=320.
                        IMAGE_HEIGHT,                // Defined in this file.  Default: QVGA=240.
                        CAM_IMAGE_PIX_FMT_YUV422);   // Defined in "camera.h". Default: CAM_IMAGE_PIX_FMT_YUV422
                      //CAM_IMAGE_PIX_FMT_GRAY);
                      //CAM_IMAGE_PIX_FMT_RGB565);
  

  if (err && doPrintCamErr)
  {
    printf("ERR: Starting the camera failed. See theCamera.begin() in camera_start_continuous()...\n");      
    printCamErr(err);
  }

  // Start streaming the Preview images to the Classifier().
  // The viewfinder of a camera shows real-time images shown on a camera.
  // This real-time image (real-time movie) is called Preview image.
  // "theCamera" has a function to acquire this Preview image frame by frame.
  //
  // "startStreaming()" registers the callback function, CamCB(), to obtain the Preview image.
  // CamCB() is a user function.
  // When "true" is specified as the first argument of startStreaming(),
  //    acquisition of the video image for Preview is started, and
  //    the registered callback function is called each time the image is acquired.
  //    The frequency of acquiring images is determined by the frame rate specified by the begin() method function.
  //    The callback function of the next frame will not be called unless the callback function implemented by the user is terminated.
  //    To stop the acquisition of the Preview image, call the startStreaming() method function
  //    with the first argument of the startStreaming() method function set to "false".
  //printf("Starting sending data:\n");
  err = theCamera.startStreaming(true, CamCB);  // Register the callback function that obtains the Preview image.
  if (err && doPrintCamErr)
  {
    printf("ERR: Start sending the data failed. See theCamera.startStreaming()...\n");      
    printCamErr(err);
  }

  /* Auto white balance configuration */
  Serial.println("Set Auto white balance parameter...");
  err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  if (err != CAM_ERR_SUCCESS)
  {
    printCamErr(err);
  }

  // We are not taking snapshots but this is how to set it up.
  // Still image format must be JPEG to allow for compressed storage/transmit.
  //printf("Set format:\n");

  // Test to see high res.
  //err = theCamera.setStillPictureImageFormat( CAM_IMGSIZE_5M_H,
  //                                            CAM_IMGSIZE_5M_V,
  //                                            CAM_IMAGE_PIX_FMT_JPG );
  
  /*
  err = theCamera.setStillPictureImageFormat( IMAGE_WIDTH,
                                              IMAGE_HEIGHT,
                                            //CAM_IMAGE_PIX_FMT fmt = CAM_IMAGE_PIX_FMT_JPEG);
                                              CAM_IMAGE_PIX_FMT_YUV422);
                                            //CAM_IMAGE_PIX_FMT_JPG );
                                            //CAM_IMAGE_PIX_FMT_GRAY);
                                            //CAM_IMAGE_PIX_FMT_RGB565);
  */                                       
  if (err && doPrintCamErr)
  {
    printf("ERR: Setting the still image format failed. See theCamera.setStillPictureImageFormat())...\n");      
    printCamErr(err);
  }

  if (doPrintCamErr)
  {
    printf("INFO: Started camera recording...\n");
  }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 5000);

    // Setup on-board LEDs.
    pinMode(LED0, OUTPUT);  // Before we save an image
    pinMode(LED1, OUTPUT);  // we turn on all LEDs.
    pinMode(LED2, OUTPUT);  // Then we turn them off one by one
    pinMode(LED3, OUTPUT);  // in a countdown type display.
  
    // Since we can't clear the Serial Monitor on program startup,
    // we print a demark.
    printf("... ... ... ... ....\n");
    printf("... ... ... ... ....\n");
    printf("... ... ... ... ....\n");
    printf("Spresense Vision Model Inferencing starting up...\n");

    // Print what INO file we are running.
    printf(__FILE__ " " __DATE__ " " __TIME__);
    printf("  IDE "); printf(ARDUINO); printf("\n");

    // Check if SD card is inserted.
    if (!theSD.begin())
    {
      Serial.println("SD card not inserted. Program halted. Power down device, insert SD card, then power up device.");  
      return;
    }

    // Init the cam
    camera_start_continuous(true); 

    Serial.println("Waiting for A, B or C from keyboard.");
    Serial.println("A starts filenames at 1.");
    Serial.println("B starts filenames at 50.");
    Serial.println("C starts filenames at 100.");
    while(false==startCapture)
    {
      handleSerial();
      delay(333);
    }
    Serial.println("Fetching camera image...");
    
    // Push the shutter 5 times.
    // NOTE: No need to do "theCamera.takePicture()" since CamCB()
    //       is constantly campturing iamges sent from the camera.
    countDownBlink(); saveImageToSdCardAsBMP();
//    countDownBlink(); saveImageToSdCardAsBMP();
  //  countDownBlink(); saveImageToSdCardAsBMP();
    //countDownBlink(); saveImageToSdCardAsBMP();
    //countDownBlink(); saveImageToSdCardAsBMP();

    printf("Program is Finished!\n");
}

void loop()
{
  // Program is finished saving 5 BMPs.
}

// Set the Headers Data
// To manipulate the file you will need to set the headers on the binary information as follows:
void setFileHeaders(unsigned char *bitmapFileHeader, unsigned char *bitmapDIBHeader, int fileSize)
{
    #ifdef FLOW
    printf("[FLOW] Entering setFileHeaders()...\n");
    #endif

    // Set the headers to 0
    memset(bitmapFileHeader, (unsigned char)(0), BITMAP_FILE_HEADER_SIZE);
    memset(bitmapDIBHeader,  (unsigned char)(0), DIB_HEADER_SIZE);

    // File header: 14 Bytes.
    bitmapFileHeader[0] = 'B';
    bitmapFileHeader[1] = 'M';

    bitmapFileHeader[2] = (unsigned char)(fileSize >>  0);  //76,854 = 0x00 01 2C 36
    bitmapFileHeader[3] = (unsigned char)(fileSize >>  8);
    bitmapFileHeader[4] = (unsigned char)(fileSize >> 16);
    bitmapFileHeader[5] = (unsigned char)(fileSize >> 24);

    bitmapFileHeader[6] = 0;  //2 Bytes: Reserved; actual value depends on the application that creates the image, if created manually can be 0
    bitmapFileHeader[7] = 0;

    bitmapFileHeader[8] = 0;  //2 Bytes: Reserved; actual value depends on the application that creates the image, if created manually can be 0
    bitmapFileHeader[9] = 0;

    //                                    54          + 1024 = 1078 = 0x0436
    bitmapFileHeader[10] = (unsigned char)((HEADER_SIZE + PALETTE_SIZE) >> 0); // 4 Bytes: The offset, i.e. starting address, 
    bitmapFileHeader[11] = (unsigned char)((HEADER_SIZE + PALETTE_SIZE) >> 8); //          of the byte where the bitmap image data (pixel array) can be found.
  //bitmapFileHeader[10] = = 0x36;                                                                      
  //bitmapFileHeader[11] = = 0x04;
  //bitmapFileHeader[12] = = 0x00;
  //bitmapFileHeader[13] = = 0x00;

    // Info header: 40 Bytes.
    bitmapDIBHeader[0] = (unsigned char)(DIB_HEADER_SIZE);      //4 Bytes: The size of this header, in bytes (40)
    bitmapDIBHeader[1] = 0x00;                                  //         40 = 0x28
    bitmapDIBHeader[2] = 0x00;
    bitmapDIBHeader[3] = 0x00;

    bitmapDIBHeader[4] = (unsigned char)(IMAGE_WIDTH);          //4 Bytes: The bitmap width in pixels (signed integer)
    bitmapDIBHeader[5] = (unsigned char)(IMAGE_WIDTH >> 8);     //         320 = 0x0140  
    bitmapDIBHeader[6] = 0x00;
    bitmapDIBHeader[7] = 0x00;

    //bitmapDIBHeader[8] = (unsigned char)(IMAGE_HEIGHT);         //4 Bytes: the bitmap height in pixels (signed integer)
    //bitmapDIBHeader[9] = (unsigned char)(IMAGE_HEIGHT >> 8);    //         240 = 0x00F0
    //bitmapDIBHeader[10] = 0x00;
    //bitmapDIBHeader[11] = 0x00;
    bitmapDIBHeader[8] = 0x10;                                    // Negative 240 = 0xFFFF FF10.
    bitmapDIBHeader[9] = 0xFF;
    bitmapDIBHeader[10] = 0xFF;
    bitmapDIBHeader[11] = 0xFF;

    bitmapDIBHeader[12] = 0x01;                                 //2 Bytes: the number of color planes (must be 1)
    bitmapDIBHeader[13] = 0x00;

    bitmapDIBHeader[14] = (unsigned char)(BITS_PER_PIXEL);      //2 Bytes: the number of bits per pixel,
    bitmapDIBHeader[15] = 0x00;                                 //         which is the color depth of the image. Typical values are 1, 4, 8, 16, 24 and 32.

    //4 Bytes: The compression method being used.
    // compression type (3 - BI_BITFIELDS)
    bitmapDIBHeader[16] = 0x03;
    bitmapDIBHeader[17] = 0x00;
    bitmapDIBHeader[18] = 0x00;
    bitmapDIBHeader[19] = 0x00;

    //4 Bytes: The image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps.
    // 76,800 = 0x02 2c00
    //bitmapDIBHeader[20] = 0x00;
    //bitmapDIBHeader[21] = 0x2c;
    //bitmapDIBHeader[22] = 0x01;
    //bitmapDIBHeader[23] = 0x00;
    bitmapDIBHeader[20] = (unsigned char)(((IMAGE_WIDTH * IMAGE_HEIGHT)*(BITS_PER_PIXEL/8)) >>  0 );
    bitmapDIBHeader[21] = (unsigned char)(((IMAGE_WIDTH * IMAGE_HEIGHT)*(BITS_PER_PIXEL/8)) >>  8 );
    bitmapDIBHeader[22] = (unsigned char)(((IMAGE_WIDTH * IMAGE_HEIGHT)*(BITS_PER_PIXEL/8)) >> 16 );
    bitmapDIBHeader[23] = (unsigned char)(((IMAGE_WIDTH * IMAGE_HEIGHT)*(BITS_PER_PIXEL/8)) >> 24 );

    //4 Bytes: The horizontal resolution of the image. (pixel per metre, signed integer)
    // horizontal resolution (dummy)
    bitmapDIBHeader[24] = 0x00;
    bitmapDIBHeader[25] = 0x00;
    bitmapDIBHeader[26] = 0x00;
    bitmapDIBHeader[27] = 0x00;

    //4 Bytes: The vertical resolution of the image. (pixel per metre, signed integer)
    // vertical resolution (dummy)
    bitmapDIBHeader[28] = 0x00;
    bitmapDIBHeader[29] = 0x00;
    bitmapDIBHeader[30] = 0x00;
    bitmapDIBHeader[31] = 0x00;

    //4 Bytes: The number of colors in the color palette, or 0 to default to 2n
    // number of using colors
    bitmapDIBHeader[32] = 0x00;
    bitmapDIBHeader[33] = 0x00;
    bitmapDIBHeader[34] = 0x00;
    bitmapDIBHeader[35] = 0x00;

    //4 Bytes: The number of important colors used, or 0 when every color is important; generally ignored
    // number of important colors
    bitmapDIBHeader[36] = 0x00;
    bitmapDIBHeader[37] = 0x00;
    bitmapDIBHeader[38] = 0x00;
    bitmapDIBHeader[39] = 0x00;
}

// In this case that our image (320x240) is 8 bits per pixel and grayscale
// on the bitmap rules you need to define the color table (color map) to assign a
// specific RGB color for our 8 bit color.
// This function it sets the color map as a grayscale of RGB colors from
//    [R:0x00 G:0x00 B:0x00] to
//    [R:0xFF G:0xFF B:0xFF]
void setColorMap(unsigned char *colorMap)
{
    #ifdef FLOW
    printf("[FLOW] Entering setColorMap()...\n");
    #endif

    //Init the palette with zeroes
    memset(colorMap, (unsigned char)(0), PALETTE_SIZE);
    
    #ifdef BMP_COLOR  
    // Change Save Format.
    /* color mask */
    // red (5)
    colorMap[0]  = 0x00;
    colorMap[1]  = 0xF8;
    colorMap[2]  = 0x00;
    colorMap[3]  = 0x00;
    // green (6)
    colorMap[4]  = 0xE0;
    colorMap[5]  = 0x07;
    colorMap[6]  = 0x00;
    colorMap[7]  = 0x00;
    // blue (5)
    colorMap[8]  = 0x1F;
    colorMap[9]  = 0x00;
    colorMap[10] = 0x00;
    colorMap[11] = 0x00;
    #else
    // Gray scale color palette, 4 bytes per color (R, G, B, 0x00).
    // Create 1024 values from [0,0,0,0] to [FF,FF,FF,00].
    for (int i = 0; i < PALETTE_COLORS_AMOUNT; i++)
    {
        colorMap[i * 4 + 0] = i;
        colorMap[i * 4 + 1] = i;
        colorMap[i * 4 + 2] = i;
    }
    #endif
}

// Save the headers and the image data into the .bmp file
void saveImageToSdCardAsBMP()
{
    #ifdef FLOW
    printf("[FLOW] Entering saveImageToSdCardAsBMP()...\n");
    #endif

    /* *** No need to check if an image is avaliable.
           CamCB() is constantly rxing data from the camera.
           So we will just use the latest camera capture.
    while(!sized_img.isAvailable())
    {
      delay(1);      
    }
    *** */

    //
    // Create filename to save the image data to.
    //
    char imagePath[20];    

    // Filenames run in a circular pattern as in "photos/bmp_000.bmp" to "photos/bmp_255.bmp".
    // Files get over-written by the next circle pattern
    sprintf(imagePath, "photos/bmp_%03d.bmp", take_picture_count);  
    printf("INFO: saving %s to SD card...\n", imagePath);
    theSD.remove(imagePath);
    File myFile = theSD.open(imagePath, FILE_WRITE);

    //
    // Setup and Populate BMP file Headers
    //
    // Bitmap structure (Head + DIB Head + ColorMap + binary image)
    unsigned char bitmapFileHeader[BITMAP_FILE_HEADER_SIZE];
    unsigned char bitmapDIBHeader[DIB_HEADER_SIZE];
    unsigned char colorMap[PALETTE_SIZE]; // Needed for <= 8bpp grayscale bitmaps    

    //             14                      + 40              + 76,800   (=320*240) = 76,854 = 0x01 2c36.
    int fileSize = BITMAP_FILE_HEADER_SIZE + DIB_HEADER_SIZE + ((IMAGE_WIDTH * IMAGE_HEIGHT)*(BITS_PER_PIXEL/8));
    setFileHeaders(bitmapFileHeader, bitmapDIBHeader, fileSize);
    setColorMap(colorMap);

    //
    // Draw Bounding Box
    //
    AddBoundingBox();

    //
    // Write the bitmap file
    //
    myFile.write(bitmapFileHeader, BITMAP_FILE_HEADER_SIZE);        //           14 = 0x 000E, 0x0000 to 0x000D (0x0D = 13.0, aka, zero based counting)
    myFile.write(bitmapDIBHeader,  DIB_HEADER_SIZE);                //           40 = 0x 0028, 0x000E to 0x0035
    myFile.write(colorMap,         PALETTE_SIZE);                   //         1024 = 0x 0400, 0x0036 to 0x0435
    
    //myFile.write(sized_img.getImgBuff(),  sized_img.getImgSize());  //320x240=76800 = 0x12C00, 0x0436 to 0x13035
    //printf("sized_img.getImgSize()=%d\n", sized_img.getImgSize());
    myFile.write(paintAddy,  sizeof(paintAddy));  //320x240=76800 = 0x12C00, 0x0436 to 0x13035
    printf("sizeof(paintAddy)=%d\n", sizeof(paintAddy));

    // Close the file stream.
    myFile.close();

    // Next filename.
    take_picture_count++;
}

// We have visual feedback with 3 GRN blinks after the photo is taken,
// once the blue LED is ON it means the picture was taken.
void countDownBlink()
{
    #ifdef FLOW
    printf("[FLOW] Entering countDownBlink()...\n");
    #endif

    digitalWrite(LED3, HIGH); digitalWrite(LED2, HIGH); digitalWrite(LED1, HIGH); digitalWrite(LED0, HIGH); delay(500);
    digitalWrite(LED3, LOW);  digitalWrite(LED2, HIGH); digitalWrite(LED1, HIGH); digitalWrite(LED0, HIGH); delay(500);
    digitalWrite(LED3, LOW);  digitalWrite(LED2, LOW);  digitalWrite(LED1, HIGH); digitalWrite(LED0, HIGH); delay(500);
    digitalWrite(LED3, LOW);  digitalWrite(LED2, LOW);  digitalWrite(LED1, LOW);  digitalWrite(LED0, HIGH); delay(500);
    digitalWrite(LED3, LOW);  digitalWrite(LED2, LOW);  digitalWrite(LED1, LOW);  digitalWrite(LED0, LOW);  delay(500);
}

/**
 * @brief      Get user input via keyboard.
 */
void handleSerial()
{
  while (Serial.available() > 0)
  {
    char incomingCharacter = Serial.read();
    if ( (10==incomingCharacter) || (13==incomingCharacter) )
    {
      continue;                         // Ignore carriage return and linefeed chars.
    }
    
    // Show what single char came in on Serial port.
    Serial.print("incomingCharacter="); Serial.println(incomingCharacter, DEC);
    
    switch (incomingCharacter)
    {
      case 'A':
      case 'a':
      {
        startCapture = true;
        take_picture_count = 1;
        break;
      }

      case 'B':
      case 'b':
      {
        startCapture = true;
        take_picture_count = 50;
        break;
      }

      case 'C':
      case 'c':
      {
        startCapture = true;
        take_picture_count = 100;
        break;
      }

      case 'X':
      case 'x':
      {
        // Do Something eXtremely Useful.
        break;
      }
    }
 }
}
void AddBoundingBox()
//void AddBoundingBox(uint32_t bb_x, uint32_t bb_y, uint32_t bb_width, uint32_t bb_height)
{
  #ifdef FLOW
  printf("[FLOW] Entering AddBoundingBox()...\n");
  #endif

  /*
  The image coordinates look like this for a 320x240 capture:
                  +----------------------+
     paintAddy[0] |{0,  0}      {319,  0}| paintAddy[319]
                  |                      |
                  |     paintAddy[]      |
                  |                      |
 paintAddy[76560] |{0,239}      {319,239}| paintAddy[76800]
                  +----------------------+ <-- { IMAGE_WIDTH=320, IMAGE_HEIGHT=240 }
  */

  /* Test BB: Random in Middle */
  // idx_UL=16100   idx_UR=16150
  // idx_LL=64100   idx_LR=64150
  //uint32_t bb_x      = 100;       
  //uint32_t bb_y      =  50;       
  //uint32_t bb_width  =  50;
  //uint32_t bb_height = 150;

  /* Test BB: Upper Left */
  // idx_UL=0         idx_UR=50
  // idx_LL=48000     idx_LR=48050
  //uint32_t bb_x      =   0;      
  //uint32_t bb_y      =   0;      
  //uint32_t bb_width  =  50;
  //uint32_t bb_height = 150;
  
  /* Test BB: Lower Right */
  // idx_UL=28749     idx_UR=28799
  // idx_LL=76749     idx_LR=76799
  uint32_t bb_x      = 269;  // 269 = 319 -  50 
  uint32_t bb_y      =  89;  //  89 = 239 - 150   
  uint32_t bb_width  =  50;
  uint32_t bb_height = 150;

  //
  // Calculate index into big array, paintAddy[].
  //    
  uint32_t idx_UL = ( (bb_y * (BITS_PER_PIXEL/8)) * IMAGE_WIDTH ) +
                      (bb_x * (BITS_PER_PIXEL/8));
  uint32_t idx_UR = idx_UL + (  bb_width  * (BITS_PER_PIXEL/8));
  uint32_t idx_LL = idx_UL + ( (bb_height * (BITS_PER_PIXEL/8)) * IMAGE_WIDTH);
  uint32_t idx_LR = idx_LL + (  bb_width  * (BITS_PER_PIXEL/8));

  printf("idx_UL=%d\n", idx_UL); //idx_UL=11520
  printf("idx_UR=%d\n", idx_UR); //idx_UR=11680
  printf("idx_LL=%d\n", idx_LL); //idx_LL=30560
  printf("idx_LR=%d\n", idx_LR); //idx_LR=30720

  //
  // Copy Camera Preview Data into a Global Variable to we can Draw a BB on It
  //
  holdOffCamCB = true;
    uint8_t *imageData = sized_img.getImgBuff();
    for (uint32_t i=0; i<IMAGE_DATA_SIZE; i++)
    {
      paintAddy[i] = imageData[i];
    }
  holdOffCamCB = false;

  //
  // Draw BB Lines
  //

#ifdef BMP_GRAY
  /* Draw Top Line of BB */
  for (uint32_t i=idx_UL; i<idx_UL+bb_width; i++)
  {
    paintAddy[i] = 0x00;
  }
  /* Draw Bottom Line of BB */
  for (uint32_t i=idx_LL; i<idx_LL+bb_width; i++)
  {
    paintAddy[i] = 0x00;
  }
  /* Draw Left Line of BB */
  for (uint32_t i=idx_UL; i<(idx_UL + (bb_height*IMAGE_WIDTH)); i=i+IMAGE_WIDTH)
  {
    paintAddy[i] = 0x00;
  }
  /* Draw Right Line of BB */
  for (uint32_t i=idx_UR; i<(idx_UR + (bb_height*IMAGE_WIDTH)); i=i+IMAGE_WIDTH)
  {
    paintAddy[i] = 0x00;
  }
#else
  /* Draw Top Line of BB */
  for (uint32_t i=idx_UL; i<idx_UL + (bb_width*(BITS_PER_PIXEL/8)); i=i+2)
  {
    paintAddy[i+0] = 0x00;
    paintAddy[i+1] = 0x00;
  }
  /* Draw Bottom Line of BB */
  for (uint32_t i=idx_LL; i<idx_LL + (bb_width*(BITS_PER_PIXEL/8)); i=i+2)
  {
    paintAddy[i+0] = 0x00;
    paintAddy[i+1] = 0x00;
  }
  /* Draw Left Line of BB */
  for (uint32_t i=idx_UL; i<(idx_UL + (bb_height*IMAGE_WIDTH*(BITS_PER_PIXEL/8))); i=i+(IMAGE_WIDTH*2))
  {
    paintAddy[i+0] = 0x00;
    paintAddy[i+1] = 0x00;
  }
  /* Draw Right Line of BB */
  for (uint32_t i=idx_UR; i<(idx_UR + (bb_height*IMAGE_WIDTH*(BITS_PER_PIXEL/8))); i=i+(IMAGE_WIDTH*2))
  {
    paintAddy[i+0] = 0x00;
    paintAddy[i+1] = 0x00;
  }
#endif

  //
  // Draw Test Lines on BMP
  //

  /* TEST 1*/
  /* Color 1st pixel white */
  //printf("paintAddy[0:2]=%08x, %08x, %08x, %08x\n", paintAddy[0], paintAddy[1], paintAddy[2], paintAddy[3]); //paintAddy[0]=
  //paintAddy[0] = 0xFF;
  //paintAddy[1] = 0x00;
  //paintAddy[2] = 0x00;
  //printf("paintAddy[0:2]=%08x, %08x, %08x, %08x\n", paintAddy[0], paintAddy[1], paintAddy[2], paintAddy[3]); //paintAddy[0]=

  /* TEST 2*/
  /* Color 1st 5 rows white */
  //for (uint32_t i=0; i<320*5; i=i+1)
  //{
  //  paintAddy[i] = 0xFF;
  //}

  /* TEST 3*/
  /* Color row 2, 4 ,6 white. */
  //for (uint32_t j = 2; j<7; j=j+2)
  //{
  //  for (uint32_t i=(320*j); i<((320*j)+320); i=i+1)
  //  {
  //    paintAddy[i] = 0xFF;
  //  }
  //}
}