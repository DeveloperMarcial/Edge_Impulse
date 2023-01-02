#define FLOW
//#include "SaveModifiedBMP.h" <--Don't include like this for this is an INO file.

// Set the Headers Data
// To manipulate the file you will need to set the headers on the binary information as follows:
void setFileHeaders(unsigned char *bitmapFileHeader, unsigned char *bitmapDIBHeader, int fileSize)
{
  #ifdef FLOW
  ei_printf("[FLOW] Entering setFileHeaders()...\n");
  #endif
    //
    // Make Space for Headers and Set All Values to 0
    //
    memset(bitmapFileHeader, (unsigned char)(0), BITMAP_FILE_HEADER_SIZE);
    memset(bitmapDIBHeader,  (unsigned char)(0), DIB_HEADER_SIZE);

    //
    // Configure BMP File Header
    //

    // File header: 14 Bytes.
    bitmapFileHeader[0] = 'B';
    bitmapFileHeader[1] = 'M';

    bitmapFileHeader[2] = (unsigned char)(fileSize >>  0);  //76,854 = 0x00 01 2C 36
    bitmapFileHeader[3] = (unsigned char)(fileSize >>  8);
    bitmapFileHeader[4] = (unsigned char)(fileSize >> 16);
    bitmapFileHeader[5] = (unsigned char)(fileSize >> 24);

    bitmapFileHeader[6] = 0;                                //2 Bytes: Reserved; actual value depends on the
    bitmapFileHeader[7] = 0;                                //          application that creates the image, if created manually can be 0 

    bitmapFileHeader[8] = 0;                                //2 Bytes: Reserved; actual value depends on the
    bitmapFileHeader[9] = 0;                                //         application that creates the image, if created manually can be 0     

    //                                    54          + 1024 = 1078 = 0x0436
    bitmapFileHeader[10] = (unsigned char)((HEADER_SIZE + PALETTE_SIZE) >> 0); // 4 Bytes: The offset, i.e. starting address, 
    bitmapFileHeader[11] = (unsigned char)((HEADER_SIZE + PALETTE_SIZE) >> 8); //          of the byte where the bitmap image data (pixel array) can be found.
  //bitmapFileHeader[10] = = 0x36;                                                                      
  //bitmapFileHeader[11] = = 0x04;
  //bitmapFileHeader[12] = = 0x00;
  //bitmapFileHeader[13] = = 0x00;

    //
    // Configure BMP DIB File Header
    //

    // Info header: 40 Bytes.
    bitmapDIBHeader[0] = (unsigned char)(DIB_HEADER_SIZE);  //4 Bytes: The size of this header, in bytes (40)
    bitmapDIBHeader[1] = 0x00;                              //         40 = 0x28
    bitmapDIBHeader[2] = 0x00;                              //
    bitmapDIBHeader[3] = 0x00;                              //  

    bitmapDIBHeader[4] = (unsigned char)(IMAGE_WIDTH);      //4 Bytes: The bitmap width in pixels (signed integer)
    bitmapDIBHeader[5] = (unsigned char)(IMAGE_WIDTH >> 8); //         320 = 0x0140  
    bitmapDIBHeader[6] = 0x00;                              //
    bitmapDIBHeader[7] = 0x00;                              //  

    //bitmapDIBHeader[8] = (unsigned char)(IMAGE_HEIGHT);     //4 Bytes: the bitmap height in pixels (signed integer)
    //bitmapDIBHeader[9] = (unsigned char)(IMAGE_HEIGHT >> 8);//         240 = 0x00F0
    //bitmapDIBHeader[10] = 0x00;                             //  
    //bitmapDIBHeader[11] = 0x00;                             //   
    bitmapDIBHeader[8] = 0x10;                                // Negative 240 = 0xFFFF FF10.
    bitmapDIBHeader[9] = 0xFF;                                // NOTE: This hard coded to "-240".
    bitmapDIBHeader[10] = 0xFF;                               //       This encoding allows for a normal non-mirrored image.     
    bitmapDIBHeader[11] = 0xFF;                               //

    bitmapDIBHeader[12] = 0x01;                             //2 Bytes: the number of color planes (must be 1)
    bitmapDIBHeader[13] = 0x00;                             //

    bitmapDIBHeader[14] = (unsigned char)(BITS_PER_PIXEL);  //2 Bytes: the number of bits per pixel,
    bitmapDIBHeader[15] = 0x00;                             //         which is the color depth of the image. Typical values are 1, 4, 8, 16, 24 and 32.

    //4 Bytes: The compression method being used.
    // compression type (3 - BI_BITFIELDS)
    bitmapDIBHeader[16] = 0x03;                             //
    bitmapDIBHeader[17] = 0x00;                             //
    bitmapDIBHeader[18] = 0x00;                             //
    bitmapDIBHeader[19] = 0x00;                             //

    //4 Bytes: The image size. This is the size of the raw bitmap data; a dummy 0 can be given for BI_RGB bitmaps.
    // 76,800 = 0x0001 2c00
    //bitmapDIBHeader[20] = 0x00;                           // Hard-coded to 76,800 = 0x0001 2c00.
    //bitmapDIBHeader[21] = 0x2c;                           // Note LSBy comes 1st.
    //bitmapDIBHeader[22] = 0x01;                           //
    //bitmapDIBHeader[23] = 0x00;                           //
    bitmapDIBHeader[20] = (unsigned char)((IMAGE_WIDTH * IMAGE_HEIGHT) >>  0 );
    bitmapDIBHeader[21] = (unsigned char)((IMAGE_WIDTH * IMAGE_HEIGHT) >>  8 );
    bitmapDIBHeader[22] = (unsigned char)((IMAGE_WIDTH * IMAGE_HEIGHT) >> 16 );
    bitmapDIBHeader[23] = (unsigned char)((IMAGE_WIDTH * IMAGE_HEIGHT) >> 24 );

    //4 Bytes: The horizontal resolution of the image. (pixel per metre, signed integer) horizontal resolution (dummy)
    bitmapDIBHeader[24] = 0x00;                             //
    bitmapDIBHeader[25] = 0x00;                             //
    bitmapDIBHeader[26] = 0x00;                             //
    bitmapDIBHeader[27] = 0x00;                             //

    //4 Bytes: The vertical resolution of the image. (pixel per metre, signed integer) vertical resolution (dummy)
    bitmapDIBHeader[28] = 0x00;                             //
    bitmapDIBHeader[29] = 0x00;                             //
    bitmapDIBHeader[30] = 0x00;                             //
    bitmapDIBHeader[31] = 0x00;                             //

    //4 Bytes: The number of colors in the color palette, or 0 to default to 2n number of using colors
    bitmapDIBHeader[32] = 0x00;                             //
    bitmapDIBHeader[33] = 0x00;                             //
    bitmapDIBHeader[34] = 0x00;                             //
    bitmapDIBHeader[35] = 0x00;                             //

    //4 Bytes: The number of important colors used, or 0 when every color is important; generally ignored number of important colors
    bitmapDIBHeader[36] = 0x00;                             //
    bitmapDIBHeader[37] = 0x00;                             //
    bitmapDIBHeader[38] = 0x00;                             //
    bitmapDIBHeader[39] = 0x00;                             //
}

// In this case that our image (320x240) is 8 bits per pixel and grayscale
// on the bitmap rules you need to define the color table (color map) to assign a
// specific RGB color for our 8 bit color.
// This function sets the color map as a grayscale of RGB colors from
//    [R:0x00 G:0x00 B:0x00] to
//    [R:0xFF G:0xFF B:0xFF].
void setColorMap(unsigned char *colorMap)
{
    #ifdef FLOW
    ei_printf("[FLOW] Entering setColorMap()...\n");
    #endif

    //Init the palette with zeroes
    memset(colorMap, (unsigned char)(0), PALETTE_SIZE);

    // Gray scale color palette, 4 bytes per color (R, G, B, 0x00).
    // Create 1024 values from [0,0,0,0] to [FF,FF,FF,00].
    for (int i = 0; i < PALETTE_COLORS_AMOUNT; i++)
    {
        colorMap[i * 4 + 0] = i;
        colorMap[i * 4 + 1] = i;
        colorMap[i * 4 + 2] = i;
    }
}

void addBoundingBox(uint32_t bb_x, uint32_t bb_y, uint32_t bb_width, uint32_t bb_height)
{
  #ifdef FLOW
  ei_printf("[FLOW] Entering addBoundingBox()...\n");
  #endif

  /*********************************************************************************************
   * The image coordinates look like this for a 320x240 capture:                               *
   * X increments to the right.                                                                *
   * Y increments down.                                                                        *
   * X ---------->                                                                             *
   * Y                     +----------------------+                                            *
   * |        paintAddy[0] |{0,  0}      {319,  0}| paintAddy[319]                             *
   * |                     |                      |                                            *
   * |                     |     paintAddy[]      |                                            *
   * \/                    |                      |                                            *
   *      paintAddy[76560] |{0,239}      {319,239}| paintAddy[76800]                           *    
   *                       +----------------------+ <-- { IMAGE_WIDTH=320, IMAGE_HEIGHT=240 }  *
   *********************************************************************************************/

  //
  // Calculate index into big array, paintAddy[] for each Corber of the BOunding Box.
  //
  uint32_t bb_x_off = OFFSET_X + bb_x;
  uint32_t bb_y_off = OFFSET_Y + bb_y;

  uint32_t idx_UL = (( bb_y_off  * IMAGE_WIDTH ) + bb_x_off); //mike
  uint32_t idx_UR = (idx_UL +   bb_width);
  uint32_t idx_LL = (idx_UL + ( bb_height * IMAGE_WIDTH));
  uint32_t idx_LR = (idx_LL +   bb_width);

  ///printf("idx_UL=%d\n", idx_UL); //idx_UL=11520
  ///printf("idx_UR=%d\n", idx_UR); //idx_UR=11680
  ///printf("idx_LL=%d\n", idx_LL); //idx_LL=30560
  ///printf("idx_LR=%d\n", idx_LR); //idx_LR=30720
  
  //
  // Draw BB Centroid
  //

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
}

// Save the Headers and the Image Data into the .BMP File
void saveImageToSdCardAsBMP(uint32_t bb_x, uint32_t bb_y, uint32_t bb_width, uint32_t bb_height)
{
    #ifdef FLOW
    ei_printf("[FLOW] Entering saveImageToSdCardAsBMP()...\n");
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
    // Create Filename to Save the Image Data to.
    //

    char imagePath[22];    

    // Filenames run in a circular pattern as in "photos/bmp_000.bmp" to "photos/bmp_255.bmp".
    // Files get over-written by the next circle pattern.
    sprintf(imagePath, "photos/screw_%03d.bmp", take_picture_count);  
    printf("INFO: saving %s to SD card...\n", imagePath);
    theSD.remove(imagePath);
    File myFile = theSD.open(imagePath, FILE_WRITE);

    //
    // Setup and Populate BMP file Headers
    //

    // Bitmap structure (Head + DIB Head + ColorMap + binary image)
    unsigned char bitmapFileHeader[BITMAP_FILE_HEADER_SIZE];
    unsigned char bitmapDIBHeader[DIB_HEADER_SIZE];
    unsigned char colorMap[PALETTE_SIZE];                   // Needed for <= 8bpp grayscale bitmaps    

    //             14                      + 40              + 76,800   (=320*240) = 76,854 = 0x01 2c36.
    int fileSize = BITMAP_FILE_HEADER_SIZE + DIB_HEADER_SIZE + ((IMAGE_WIDTH * IMAGE_HEIGHT)*(BITS_PER_PIXEL/8));
    setFileHeaders(bitmapFileHeader, bitmapDIBHeader, fileSize);
    setColorMap(colorMap);
    
    //
    // Draw Bounding Box on BMP
    //
    addBoundingBox(bb_x, bb_y,  bb_width, bb_height);

    //
    // Write the Bitmap File
    //
    myFile.write(bitmapFileHeader, BITMAP_FILE_HEADER_SIZE);        //           14 = 0x 000E, 0x0000 to 0x000D (0x0D = 13.0, aka, zero based counting)
    myFile.write(bitmapDIBHeader,  DIB_HEADER_SIZE);                //           40 = 0x 0028, 0x000E to 0x0035
    myFile.write(colorMap,         PALETTE_SIZE);                   //         1024 = 0x 0400, 0x0036 to 0x0435
    
    //myFile.write(output_img.getImgBuff(),  output_img.getImgSize());  //320x240=76800 = 0x12C00, 0x0436 to 0x13035
    //printf("File closed. output.getImgSize()=%d\n", output_img.getImgSize());

    myFile.write(paintAddy,  sizeof(paintAddy));  //320x240=76800 = 0x12C00, 0x0436 to 0x13035
    printf("File closed. sizeof(paintAddy)=%d\n", sizeof(paintAddy));

    // Close the file stream.
    myFile.close();
    

    // Next filename.
    take_picture_count++;
}