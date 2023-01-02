// Bitmap File Format
// The bitmap binary file needs to contain some information in order to tell the computer for example the resolution
// of the picture and the bit-depth (bpp). Bit depth refers to the color information stored in the image.
// The higher the bit depth of an image, the more colors it can store.
// As the bit depth increases, the file size of the image also increases, because more color information has to be stored for each pixel in the image.
#define IMAGE_WIDTH (unsigned int)RAW_WIDTH     // These values are base on the EdgeImpulse machine leanring Model configuration in the other INO file.
#define IMAGE_HEIGHT (unsigned int)RAW_HEIGHT
//
// Grayscale Example)
// Image data	76800 (=320 x 240) Bytes.	The raw image data, in this case each pixel has 8 bits (1 Byte) and the resolution is 320 x 240, with no compression.
// The final size of the file is 77,879 Bytes.
//                               77,879  - 76,800 =             1079 Bytes
//                                DIB	                            14 Bytes
//                                File Header	                    40 Bytes	
//                                Palette (Grayscale Color Map)	1025 Bytes

#define IMPULSE_IMAGE_DATA_SIZE (EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT) // The size of the image data feeding the Impulse.
#define IMAGE_DATA_SIZE (76800)                                       // The size of the data in the BMP we are saving. Does not include Headers or Color Map.  
#define BITS_PER_PIXEL (unsigned int)8                                // Needed for Grayscale BMP headers.
#define PALETTE_COLORS_AMOUNT (unsigned int)(pow(2, BITS_PER_PIXEL))  // 1025 Bytes: This header is mandatory on bitmaps with a bpp ≤ 8, setting the grayscale.
#define PALETTE_SIZE  (unsigned int)(PALETTE_COLORS_AMOUNT * 4)       //    4 Bytes: 32-bit per color (3 bytes RGB and 1 byte 0x00).

// BMP File Headers
#define BITMAP_FILE_HEADER_SIZE (unsigned int)14            // For storing general information about the bitmap image file.
                                                            // 14 Bytes:	Bitmap information, setting the size of the file.
// BMP Info Headers
#define DIB_HEADER_SIZE (unsigned int)40                    // For storing information about the image and define the pixel format.
                                                            // 40 Bytes: This header requires the resolution and the bpp (bit-depth).

// Total Header Size (does not include the Color Map) 
//                                      = 14 + 40
#define HEADER_SIZE (BITMAP_FILE_HEADER_SIZE + DIB_HEADER_SIZE)

bool    holdOffCamCB       = false;                         // Global: Don't let CamCB() modify "sized_img" while we copy it contents to paintAddy[].

bool    startCapture       = false;                         // Global: Allows exiting from setup() and servicing the images streaming 
                                                            // and being captured in the callback CamCB().

uint8_t take_picture_count = 0;                             // Global: Used to make unique filenames sored on the SD card.

//uint8_t sizedAddy[IMPULSE_IMAGE_DATA_SIZE];                 // Global: This array is a copy of the captured camera data held in the instantiated CamImage img 
                                                            //         a local variable in CamCB(). 

uint8_t paintAddy[IMAGE_DATA_SIZE];                         // Global: This array is a copy of the captured camera data held in the instantiated CamImage img 
                                                            //         a local variable in CamCB(). 

void saveImageToSdCardAsBMP(uint32_t bb_x, uint32_t bb_y, uint32_t bb_width, uint32_t bb_height);
void setFileHeaders(unsigned char *bitmapFileHeader, unsigned char *bitmapDIBHeader, int fileSize);
void setColorMap(unsigned char *colorMap);
void AddBoundingBox(uint32_t bb_x, uint32_t bb_y, uint32_t bb_width, uint32_t bb_height);