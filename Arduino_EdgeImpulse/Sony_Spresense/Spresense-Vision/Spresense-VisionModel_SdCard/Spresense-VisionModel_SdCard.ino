/***
 ************************************************************************************
 * The MIT License (MIT)                                                            *
 *                                                                                  *
 * This program is a combination of modifications to the Softwares:                 *
 * 1) "firmware-sony-spresense" originally found                                    *
 *     at "https://github.com/edgeimpulse/"                                         *
 * 2) "ei-spresense-4g-wildlife-camera originally found                             *
 *     at https://github.com/edgeimpulse/                                           *
 * and saved herin as the Software "Spresense-VisionModel_SdCard.ino"               *
 * Copyright (c) 2023 M. Marcial                                                    *
 * All Rights Reserved                                                              *
 *                                                                                  *
 * Permission is hereby granted, free of charge, to any person obtaining a copy     *
 * of this software and associated documentation files (the "Software"), to deal    *
 * in the Software without restriction, including without limitation the rights     *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
 * copies of the Software, and to permit persons to whom the Software is            *
 * furnished to do so, subject to the following conditions:                         *
 * The above copyright notice shall be included in                                  *
 * all copies or substantial portions of the Software.                              *
 *                                                                                  *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    *
 * SOFTWARE.                                                                        *
 ************************************************************************************
 ************************************************************************************
 ************************************************************************************
 * Some of this code came from "nano_ble33_sense_camera.ino" example.               *
 * Hence, the following Edge Impulse copyright statement.                           *
 *----------------------------------------------------------------------------------*
 * Edge Impulse Arduino examples                                                    *
 * Copyright (c) 2021 EdgeImpulse Inc.                                              *
 *                                                                                  *
 * Permission is hereby granted, free of charge, to any person obtaining a copy     *
 * of this software and associated documentation files (the "Software"), to deal    *
 * in the Software without restriction, including without limitation the rights     *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
 * copies of the Software, and to permit persons to whom the Software is            *
 * furnished to do so, subject to the following conditions:                         *
 *                                                                                  *
 * The above copyright notice and this permission notice shall be included in       *
 * all copies or substantial portions of the Software.                              *
 *                                                                                  *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    * 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    *
 * SOFTWARE.                                                                        *
 *                                                                                  *
 ************************************************************************************
 * The routine herein saveImageToSdCardAsBMP() is based on jlutzwpi code from here       *
 * https://github.com/jlutzwpi/Spresense-Potholes/blob/main/pothole_detector.inoand *
 * and carries the following license.                                               *
 *                                                                                  *
 * MIT License                                                                      *
 *                                                                                  *
 * Copyright (c) 2022 jlutzwpi                                                      *
 *                                                                                  *
 * Permission is hereby granted, free of charge, to any person obtaining a copy     *
 * of this software and associated documentation files (the "Software"), to deal    *
 * in the Software without restriction, including without limitation the rights     *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
 * copies of the Software, and to permit persons to whom the Software is            *
 * furnished to do so, subject to the following conditions:                         *  
 *                                                                                  *
 * The above copyright notice and this permission notice shall be included in all   *
 * copies or substantial portions of the Software.                                  *
 *                                                                                  * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    *
 * SOFTWARE.                                                                        *  
 ************************************************************************************
 * Some help was gleaned from here:                                                 * 
 * https://elchika.com/article/0eaf6193-cb30-4be5-80f4-54d33cc01946/                *
 ************************************************************************************ 

 ***/

/*
  --Program Description

  **Seeing What the Classifier() Saw in a FOMO Model**

  This program runs an EdgeImpulse (EI) FOMO Machine Learning (ML) model that is looking to find one or more screws
  in a single image captured by a Sony Spresense Camera module connected to a Sony Spresense Main board.
  When one or more FOMOed objects are found, then a BMP is written to a SD card on the Sony Spresense Extension board
  with the centroid of the predicated object location overlayed on the BMP in a rectangular black outline.
  The size and location of the centroid rectangle is an output on the EdgeImpulse FOMO model inference routine.
  (Note: On Windows 10 the Microsoft Photos app displays the BMPs.
  On Ubuntu the Image View app will not display the BMPs and says the BMP header is corrupt.)

  This program is intended to save the image that got FOMOed so that we can see where the Classifier drew
  a centroid of the bounding boxes. The code is optimized to save the images as grayscale BMPs and does
  not handle color BMPs. If you want to save color BMPs, see this [code](https://github.com/DeveloperMarcial/Edge_Impulse/tree/trunk/Arduino_EdgeImpulse/Sony_Spresense/Spresense-Vision/Spresense-CameraToSdCardAsBMP)
  that can handle grayscale and color BMPs but is more compilcated than it needs to be in that it contains
  many compiler directives and additional code so that it can handle either case.

*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      MEMORY INFO                                                                               //
//                      The program requires a minimum MainCore memory of 1024 kB                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/***
//------------------------------------------------------------------------------------------------------------------
  Edge Impulse Studio float32 Library Estimates: RAM 131k, Flash 182
//------------------------------------------------------------------------------------------------------------------

  Target 1536 kB
  Sketch uses           285,224 bytes (18%) of program storage space.
  Maximum is          1,572,864 bytes.
  Global variables use  285,224 bytes (18%) of dynamic memory,
  leaving             1,287,640 bytes for local variables.
  Maximum is          1,572,864 bytes.
  
  *** ---> Works!

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      PROGRAMMING INFO                                                                          //
//                      If Arduino fails to program the Spresense, execute the following command line.            //
//                      Change "<your_path>" depending on your system configuration                               //
//                      Change "/dev/ttyUSB0" to serial port the Spresense connected to.                          //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Flash from Ubuntu:
      "<your_path>/.arduino15/packages/SPRESENSE/tools/spresense-tools/2.6.0/flash_writer/linux/flash_writer"
      -s -c
      "/dev/ttyUSB0"
      -d -n
      "/tmp/arduino-sketch-<a_randon_32-bit_hex_value>>/Spresese-VisionModel_SdCard.ino.spk"
  
  Flash from Windows:
    "C:\Users\<username>\AppData\Local\Arduino15\packages\SPRESENSE\tools\spresense-tools\2.6.0/flash_writer/windows/flash_writer.exe"
    -s -c
    "COM11"
    -d -n
    "C:\Users\<username>\AppData\Local\Temp\arduino-sketch-<a_randon_32-bit_hex_value>/Spresese-VisionModel_SdCard.ino.spk"
//------------------------------------------------------------------------------------------------------------------
***/

/* Defines & Includes ------------------------------------------------------------------------------------------- */
// The following are not supported on a SubCore (there may be others):
//                                                                      Audio.h,
//                                                                      Camera.h,
//                                                                      File.h,
//                                                                      MemoryUtil.h
#ifdef SUBCORE
#error "Core selection is wrong!! Must compile to MainCore!!!"
#endif

//Image Classification
//#include <Cup_inferencing.h>                            // This is the int8 model exported from the Edge Impulse Studio as an Arduino library.

//Arduino Boards Manager Additional Boards URL needs:
//   https://github.com/sonydevworld/spresense-arduino-compatible/releases/download/generic/package_spresense_index.json
//Arduino Ubuntu libraries are at: /home/nvme/Arduino/libraries/

//My Image Segmetation, aka Edge Impulse FOMO Test Libraries
//#include <FOMO_Washers_and_Screws_96x96_inferencing.h>    // This is the int8 model exported from the Edge Impulse Studio as an Arduino library.
//#include <Spresense_FOMO_inferencing.h>
#include <Spresense_FOMO_Screws_inferencing.h>
#define EI_PORTING_SONY_SPRESENSE (1)

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
#include "SaveModifiedBMP.h"                              // Global variables used by SaveModifiedBMP.ino.  

#define FLOW                                              // Used to print when entering a routine so we can see the flowchart.
///#define EI_CLASSIFIER_HAS_MODEL_VARIABLES       1
///#define EI_CLASSIFIER_INPUT_FRAMES              1
///#define EI_CLASSIFIER_INTERVAL_MS               1
///#define EI_CLASSIFIER_TFLITE_OUTPUT_DATA_TENSOR 1
///#define EI_CLASSIFIER_FULL_TFLITE               1
///#define EI_CLASSIFIER_OBJECT_DETECTION          1           // Assert for FOMO. Deassert for Image Classification.

/* The following #defines define the center, crop, and resize of the image to the Impulse image size.
   We will use the Spresense hardware accelerator inside the CPU CXD5602.
   NOTE: EI_CLASSIFIER_INPUT width and height must be less than RAW_HEIGHT * SCALE_FACTOR, and must
   simultaneously meet the requirements of the Spresense api:
   https://developer.sony.com/develop/spresense/developer-tools/api-reference/api-references-arduino/group__camera.html#ga3df31ea63c3abe387ddd1e1fd2724e97
*/
#define SCALE_FACTOR 1
// Camera Specs:
//              2608 (H) x 1960 (V) = approx. 5.11 M pixels
//              Y/C, RGB and JPEG

//The Spresense HW accelerator can only handle:
//  Maximum width  is  768 pixels.
//  Maximum height is 1024 pixels.

//#define RAW_WIDTH    768                  // <--!!!App crashes!!!
//#define RAW_HEIGHT   1024

//#define RAW_WIDTH  CAM_IMGSIZE_QQVGA_H   // 160 <-- Classifies ok.
//#define RAW_HEIGHT CAM_IMGSIZE_QQVGA_V   // 120

#define RAW_WIDTH    CAM_IMGSIZE_QVGA_H    // 320 <-- Classifies ok.
#define RAW_HEIGHT   CAM_IMGSIZE_QVGA_V    // 240

//#define RAW_WIDTH  CAM_IMGSIZE_VGA_H     // 640 <--!!!Although the HW can handle this the app crashes!!!
//#define RAW_HEIGHT CAM_IMGSIZE_VGA_V     // 480

//#define RAW_WIDTH  CAM_IMGSIZE_HD_H      //1280 <-- Exceeds HW accellerator maximum width of 768 pixels. 
//#define RAW_HEIGHT CAM_IMGSIZE_HD_V      // 720   

//#define RAW_WIDTH  CAM_IMGSIZE_QUADVGA_H //1280 <-- Exceeds HW accellerator maximum width of 768 pixels. 
//#define RAW_HEIGHT CAM_IMGSIZE_QUADVGA_V // 960

//#define RAW_WIDTH  CAM_IMGSIZE_FULLHD_H  //1920 <-- Exceeds HW accellerator maximum width of  768 pixels. 
//#define RAW_HEIGHT CAM_IMGSIZE_FULLHD_V  //1080 <-- Exceeds HW accellerator maximum height of 1024 pixels. 

//#define RAW_WIDTH  CAM_IMGSIZE_3M_H      //2048 <-- Exceeds HW accellerator maximum width of  768 pixels. 
//#define RAW_HEIGHT CAM_IMGSIZE_3M_V      //1536 <-- Exceeds HW accellerator maximum height of 1024 pixels. 

// Verified program !!!crashes!!! with these settings:
//#define RAW_WIDTH  CAM_IMGSIZE_5M_H      //2560 <-- Exceeds HW accellerator maximum width of  768 pixels. 
//#define RAW_HEIGHT CAM_IMGSIZE_5M_V      //1920 <-- Exceeds HW accellerator maximum height of 1024 pixels. 

// We need to clip (crop) the raw image from the camera to be the same size that the Impulse was configured with.
// The following 4 #defines are used to calculate the image:	lefttop_x, lefttop_y, rightbottom_x, & rightbottom_y.
// What is the difference between clip and crop?
// Clip: To limit or reduce the extent of one dataset by the extents or boundary of another.
// Crop: In this context means to cut or trim an image or raster file.
#define CLIP_WIDTH  (EI_CLASSIFIER_INPUT_WIDTH  * SCALE_FACTOR) // EI_CLASSIFIER_INPUT_WIDTH  is defined by the EI Model in "model_metadata.h"
#define CLIP_HEIGHT (EI_CLASSIFIER_INPUT_HEIGHT * SCALE_FACTOR) // EI_CLASSIFIER_INPUT_HEIGHT is defined by the EI Model in "model_metadata.h"
#define OFFSET_X   ((RAW_WIDTH  - CLIP_WIDTH)  / 2)             // (320-96) / 2 = 112
#define OFFSET_Y   ((RAW_HEIGHT - CLIP_HEIGHT) / 2)             // (240-96) / 2 =  72

#if EI_CLASSIFIER_INPUT_WIDTH  > RAW_HEIGHT * SCALE_FACTOR
#error "EI_CLASSIFIER_INPUT_WIDTH not compatiable with Spresense hardware accelerator.
#endif

#if EI_CLASSIFIER_INPUT_HEIGTH > RAW_HEIGHT * SCALE_FACTOR
#error "EI_CLASSIFIER_INPUT_HEIGTH not compatiable with Spresense hardware accelerator.
#endif

#define DEBUG_IT  true                  // Enable for very verbose logging from Edge Impulse SDK.
                                        // Show features from raw data during the Classify().

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      START GLOBALS                                                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Static Variables --------------------------------------------------------------------------------------------- */
       bool     StateLED0     = false;  //Global: Toggle when entering     CamCB()
       bool     StateLED1     = false;  //Global: Toggle before calling    camera_classify()
       bool     StateLED2     = false;  //Global: Toggles when we classify an image with high confidence.
       bool     StateLED3     = false;  //Global: Toggle when entering     ei_camera_cutout_get_data().

       bool     startCamera   = false;  // Global: If asserted then the camera is streaming.
       bool     gotAnImage    = false;  // Global: Set to true in CamCB().  

static CamImage sized_img;              //Global: Instatiate a Sony class to control the image from the camera.
                                        //        CamCB(),                     copies a modified img[] to sized_img[].
                                        //        ei_camera_cutout_get_data(), copies sized_img[]      to out_ptr[] as a grayscale image.
                                        //        saveImageToSdCardAsBMP(),    copies sized_img[]      to SD card.

static CamImage output_img;             //Global: Instatiate a Sony class to control the image from the camera to make it 320x240.



SDClass theSD;

/* Prototypes --------------------------------------------------------------------------------------------------- */
/*
              void printCamErr(              enum CamErr);
              void CamCB(                    CamImage);
static inline void mono_to_rgb(              uint8_t, uint8_t *, uint8_t *, uint8_t *);
int                ei_camera_cutout_get_data(size_t, size_t, float *);
static        void camera_classify(          bool);
              void camera_start_continuous(  bool);
              void setup();
              void loop();
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      END GLOBALS                                                                               //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
  ei_printf("[FLOW] Entering printCamErr(%d)...\n", err);
  #endif
  
  ei_printf("Error: ");
  switch (err)
  {
    case CAM_ERR_NO_DEVICE:
      ei_printf("No Device.\n");
      break;
    case CAM_ERR_ILLEGAL_DEVERR:
      ei_printf("Illegal device error.\n");
      break;
    case CAM_ERR_ALREADY_INITIALIZED:
      ei_printf("Already initialized.\n");
      break;
    case CAM_ERR_NOT_INITIALIZED:
      ei_printf("Not initialized.\n");
      break;
    case CAM_ERR_NOT_STILL_INITIALIZED:
      ei_printf("Still picture not initialized.\n");
      break;
    case CAM_ERR_CANT_CREATE_THREAD:
      ei_printf("Failed to create thread.\n");
      break;
    case CAM_ERR_INVALID_PARAM:
      ei_printf("Invalid parameter.\n");
      break;
    case CAM_ERR_NO_MEMORY:
      ei_printf("No memory.\n");
      break;
    case CAM_ERR_USR_INUSED:
      ei_printf("Buffer already in use.\n");
      break;
    case CAM_ERR_NOT_PERMITTED:
      ei_printf("Operation not permitted.\n");
      break;
    default:
      ei_printf("Unknown camera error.\n");
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      EDGE IMPULSE VISION ROUTINES                                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************************************
 * @brief      Convert Monochrome Data to RGB Values
 * @param[in]  mono_data  The mono data
 * @param[out] r          red pixel value
 * @param[out] g          green pixel value
 * @param[out] b          blue pixel value
 *******************************************************************************************************************/
 static inline void mono_to_rgb(uint8_t mono_data, uint8_t *r, uint8_t *g, uint8_t *b)
{
  #ifdef FLOW  
  ///ei_printf("[FLOW] Entering mono_to_rgb()...\n");
  #endif

  uint8_t v = mono_data;
  *r = *g = *b = v;                                         // Copy the same "mono_data" into 3 variables: "r", "g", "b".
}

/*******************************************************************************************************************
 * @brief      Convert Grayscale to RGB
 * @details    This is the routine that gets the data for the Impulse "signal".
 *             This routine is called repeatedly until it reads the whole FrameBuffer (FB).
 *             The size of the FB is (EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT).
 *             For an Impulse of 96x96 input it will have a FB size of 9216.  
 * @param[in]  offset,  pixel offset of raw buffer
 * @param[in]  length,  number of pixels to convert
 * @param[out] out_buf, pointer to store output image
 * @return     0
 *******************************************************************************************************************/
int ei_camera_cutout_get_data(size_t offset, size_t length, float *out_ptr)
{
  #ifdef FLOW
  ///ei_printf("[FLOW] Entering Grayscale, ei_camera_cutout_get_data()...offset=%d, length=%d\n", offset, length);
  #endif

  StateLED3 = !StateLED3;
  digitalWrite(LED3, StateLED3);
  
  size_t bytes_left = length;
  size_t out_ptr_ix = 0;

  uint8_t *cameraImageData = sized_img.getImgBuff(); // Get resized and grayscale image in class object: sized_img.
  
  // Read byte for byte.
  while (bytes_left != 0)
  {
    // Grab the pixel value.
    uint8_t pixel = cameraImageData[offset];

    // Convert to R/G/B.
    uint8_t r, g, b;
    mono_to_rgb(pixel, &r, &g, &b);

    // Save RGB value as a float.
    float pixel_f = (r << 16) + (g << 8) + b;
    out_ptr[out_ptr_ix] = pixel_f;

    // ...and go to the next pixel.
    out_ptr_ix++;
    offset++;
    bytes_left--;
  }

  // RGB image buffer is now ready.
  return 0;
}
/*******************************************************************************************************************
 * @brief      Runs Inference on the static "sized_image" Buffer using the provided Impulse.
 * @details    
 * @param[in]  debug, Show features created from raw data.
 * @return     void
*******************************************************************************************************************/
static void camera_classify(bool debug)
{
  #ifdef FLOW
  ei_printf("[FLOW] Entering camera_classify()...\n", debug);
  #endif
  
  holdOffCamCB = true;
  // Setup "signal": sets the callback function on the "signal_t" structure to reference the inference buffer.
  ei::signal_t signal;
  signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
  signal.get_data     = &ei_camera_cutout_get_data;                               // This tells the "signal" where to get the sampled data from.
  static ei_impulse_result_t ei_result = { 0 };                                   // Local: "results" of Classifier(). This doesn't need to be global.

  EI_IMPULSE_ERROR err = run_classifier(&signal, &ei_result, debug);
  
  if (err != EI_IMPULSE_OK)
  {
    ei_printf("ERROR: Failed to run classifier in camera_classify(). Err=%d\n", err);
    return;
  }
  holdOffCamCB = false;
  
  // Print the Predictions
  ei_printf("\tPredictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms, Total: %d ms.): \n",
             ei_result.timing.dsp,  ei_result.timing.classification,  ei_result.timing.anomaly,
             ei_result.timing.dsp + ei_result.timing.classification + ei_result.timing.anomaly);
             
  // FOMO Vision Model Details: Image Segmentation.

  uint8_t FOMO_Count = 0;
  for (size_t ix = 0; ix < EI_CLASSIFIER_OBJECT_DETECTION_COUNT; ix++)
  {
    if (ei_result.bounding_boxes[ix].value >0)
    {
      FOMO_Count = FOMO_Count  + 1;
    }
  }
  ei_printf("\t\tFOMO_Count=%d\n", FOMO_Count);
  
  bool bb_found = ei_result.bounding_boxes[0].value > 0;
  //ei_printf("BB Count=%d\n", ei_result.bounding_boxes_count); <--Is always returning 10 which is the minimum number of FOMOed objects that can be found (I think).

  if (!bb_found)
  {
    ei_printf("\t\tNo objects found.\n");
  }
  else
  {  
    for (size_t ix = 0; ix < EI_CLASSIFIER_OBJECT_DETECTION_COUNT; ix++)
    {
      auto bb = ei_result.bounding_boxes[ix];
      if (bb.value == 0)
      {
        ///ei_printf("\t\tix=%d, ", ix);
      }
      else
      {
        // Save image from Camera converted to grayscale with the Centroid of the Boundng Box found by the Classifer().
        saveImageToSdCardAsBMP(bb.x, bb.y, bb.width, bb.height);

        ei_printf("\t\tClass %d: %s (", ix, bb.label);
        ei_printf_float(bb.value);
        ei_printf(") [ x: %u, y: %u, width: %u, height: %u ]\n", bb.x, bb.y, bb.width, bb.height);
      }
    }
  }
  ei_printf("==========================================================\n");
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      SPRESENSE VISION ROUTINES                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************************************
 * @brief   Callback that checks for the presence of an image in the camera preview window.
 * @details CamCB() fires about every 400 ms.
 * @param
 * @return  void
 *******************************************************************************************************************/
void CamCB(CamImage img)
{
  #ifdef FLOW
  ei_printf("[FLOW] Entering CamCB()...\n");
  #endif

  StateLED0 = !StateLED0;
  digitalWrite(LED0, StateLED0);

  ///ei_printf("CamImage img when entering CamCB()...\n";
  ///ei_printf("CamImage img: Image Width      =%d\n", img.getWidth());        //          320
  ///ei_printf("CamImage img: Image Height     =%d\n", img.getHeight());       //          240
  ///ei_printf("CamImage img: Image Size       =%d\n", img.getImgSize());      //      153 600 = 2 * (320 * 240)
  ///ei_printf("CamImage img: Image Format     =%d\n", img.getPixFormat());    //1 498 831 189  
  ///ei_printf("CamImage img: Image Buffer Size=%d\n", img.getImgBuffSize());  //      153 600
  ///ei_printf("CamImage img: Image Avaliable  =%d\n", img.isAvailable());     //            1
  
  if (!img.isAvailable())
  {
    // If an error occurs during photography, it returns an empty CamImage.
    // To determine whether an instance of CamImage is empty,
    // you can check it with isAvailable() of CamImage class.    
    ei_printf("In CamCB(): Image is no longer ready...\n");
    return; // Fast path if image is no longer ready.
  } 
  CamErr err;
  ///ei_printf("In CamCB(): New frame processing...\n");
  
  //
  // Resize and convert image to grayscale to prepare for inferencing
  //

  while (holdOffCamCB)
  {
    delay(1);
  }
    
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
  ///ei_printf("Cliping and resizing img into sized_img...\n");
  err = img.clipAndResizeImageByHW(sized_img,                   // CamImage &img,
                                   OFFSET_X,                    // int 	     lefttop_x                    = 112,
                                   OFFSET_Y,                    // int 	     lefttop_y                    =  72,
                                   OFFSET_X + CLIP_WIDTH - 1,   // int 	     rightbottom_x = 112 + 96 - 1 = 207,
                                   OFFSET_Y + CLIP_HEIGHT - 1,  // int 	     rightbottom_y = 112 + 96 - 1 = 207,
                                   EI_CLASSIFIER_INPUT_WIDTH,   // int       width = 96.  Must be the same as the Impulse Input Block.
                                   EI_CLASSIFIER_INPUT_HEIGHT); // int       height = 96. Must be the same as the Impulse Input Block.
  if (err)
  {
    ei_printf("--- --- --- --- --- ERR: New frame processing failed. See clipAndResizeImageByHW(sized_img)...\n");      
    printCamErr(err);
  }
  ///ei_printf("CamImage sized_img after clipAndResizeImageByHW()...\n";
  ///ei_printf("CamImage sized_img: Image Width      =%d\n", sized_img.getWidth());        //           96
  ///ei_printf("CamImage sized_img: Image Height     =%d\n", sized_img.getHeight());       //           96
  ///ei_printf("CamImage sized_img: Image Size       =%d\n", sized_img.getImgSize());      //       18 432 = 2 * (96 * 96)
  ///ei_printf("CamImage sized_img: Image Format     =%d\n", sized_img.getPixFormat());    //1 498 831 189  
  ///ei_printf("CamImage sized_img: Image Buffer Size=%d\n", sized_img.getImgBuffSize());  //       18 432
  ///ei_printf("CamImage sized_img: Image Avaliable  =%d\n", sized_img.isAvailable());     //            1
  
  /*
  err = img.resizeImageByHW(       sized_img,                   // CamImage &img,
                                   EI_CLASSIFIER_INPUT_WIDTH,   // int       width.
                                   EI_CLASSIFIER_INPUT_HEIGHT); // int       height.                                   
  if (err)
  {
    ei_printf("--- --- --- --- --- ERR: New frame processing failed. See resizeImageByHW(sized_img)...\n");      
    printCamErr(err);
  }                                   
  */

  ///ei_printf("Converting sized_img to grayscale...\n");
  err = sized_img.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);
  if (err)
  {   
    ei_printf("--- --- --- --- --- ERR: Converting processing failed. See convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY)...\n");      
  }
  ///ei_printf("CamImage sized_img after convertPixFormat()...\n");
  ///ei_printf("CamImage sized_img: Image Width      =%d\n", sized_img.getWidth());        //           96
  ///ei_printf("CamImage sized_img: Image Height     =%d\n", sized_img.getHeight());       //           96
  ///ei_printf("CamImage sized_img: Image Size       =%d\n", sized_img.getImgSize());      //        9 216 = (96 * 96)
  ///ei_printf("CamImage sized_img: Image Format     =%d\n", sized_img.getPixFormat());    //1 195 724 875
  ///ei_printf("CamImage sized_img: Image Buffer Size=%d\n", sized_img.getImgBuffSize());  //       18 432 = 2 * 9216
  ///ei_printf("CamImage sized_img: Image Avaliable  =%d\n", sized_img.isAvailable());     //            1

  ///ei_printf("Cliping and resizing img into output_img...\n");
  // Resize YUV422 image into a known size so we can write a BMP with its required headers.
  err = img.resizeImageByHW(       output_img,                  // CamImage &img,
                                   RAW_WIDTH,                   // int       width.
                                   RAW_HEIGHT);                 // int       height.
  if (err)
  {
    ei_printf("--- --- --- --- --- ERR: New frame processing failed. See resizeImageByHW(output_img)...\n");      
    printCamErr(err);
  }
  ///ei_printf("CamImage output_img after resizeImageByHW()...\n");
  ///ei_printf("CamImage output_img: Image Width      =%d\n", output_img.getWidth());        //          320
  ///ei_printf("CamImage output_img: Image Height     =%d\n", output_img.getHeight());       //          240
  ///ei_printf("CamImage output_img: Image Size       =%d\n", output_img.getImgSize());      //      153 600 = 2 * (320 * 240)
  ///ei_printf("CamImage output_img: Image Format     =%d\n", output_img.getPixFormat());    //1 498 831 189  
  ///ei_printf("CamImage output_img: Image Buffer Size=%d\n", output_img.getImgBuffSize());  //      153 600
  ///ei_printf("CamImage output_img: Image Avaliable  =%d\n", output_img.isAvailable());     //            1
  
  //ei_printf("In CamCB(): Convert format: ");
  
  // Output still picture format: JPEG(4:2:2), Y/Cb/Cr, YUV, RGB, RAW, JPEG+YUV(thumbnail)
  // Still data rate: 5M pixel 15 frame/s JPEG output
  // CAM_IMAGE_PIX_FMT_RGB565 = V4L2_PIX_FMT_RGB565, /**< RGB565 format */
  // CAM_IMAGE_PIX_FMT_YUV422 = V4L2_PIX_FMT_UYVY,   /**< YUV422 packed. */
  // CAM_IMAGE_PIX_FMT_JPG    = V4L2_PIX_FMT_JPEG,   /**< JPEG format */
  // CAM_IMAGE_PIX_FMT_GRAY,                         /**< Gray-scale */
  // CAM_IMAGE_PIX_FMT_NONE,                         /**< No defined format */
  
  ///ei_printf("Converting output_img into grayscale...\n");
  err = output_img.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);
  if (err)
  {   
    ei_printf("--- --- --- --- --- ERR: Converting processing failed. See convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY)...\n");      
  }
  ///ei_printf("CamImage output_img after convertPixFormat()...\n");
  ///ei_printf("CamImage output_img: Image Width      =%d\n", output_img.getWidth());        //          320
  ///ei_printf("CamImage output_img: Image Height     =%d\n", output_img.getHeight());       //          240
  ///ei_printf("CamImage output_img: Image Size       =%d\n", output_img.getImgSize());      //       76 800 = (320 * 240)
  ///ei_printf("CamImage output_img: Image Format     =%d\n", output_img.getPixFormat());    //1 195 724 875
  ///ei_printf("CamImage output_img: Image Buffer Size=%d\n", output_img.getImgBuffSize());  //      153 600
  ///ei_printf("CamImage output_img: Image Avaliable  =%d\n", output_img.isAvailable());     //            1

  ///ei_printf("CamImage img later in CamCB()...\n");
  ///ei_printf("CamImage img: Image Width      =%d\n", img.getWidth());        //          320
  ///ei_printf("CamImage img: Image Height     =%d\n", img.getHeight());       //          240
  ///ei_printf("CamImage img: Image Size       =%d\n", img.getImgSize());      //      153 600 = 2 * (320 * 240)
  ///ei_printf("CamImage img: Image Format     =%d\n", img.getPixFormat());    //1 498 831 189  
  ///ei_printf("CamImage img: Image Buffer Size=%d\n", img.getImgBuffSize());  //      153 600
  ///ei_printf("CamImage img: Image Avaliable  =%d\n", img.isAvailable());     //            1
  
  ///ei_printf("CamImage        img: H,W,S=%d, %d, %d\n",        img.getWidth(),        img.getHeight(),        img.getImgSize());
  ///ei_printf("CamImage  sized_img: H,W,S=%d, %d, %d\n",  sized_img.getWidth(),  sized_img.getHeight(),  sized_img.getImgSize());
  ///ei_printf("CamImage output_img: H,W,S=%d, %d, %d\n", output_img.getWidth(), output_img.getHeight(), output_img.getImgSize());


  //
  // Copy Camera Preview Data into a Global Variable to we can Draw a BB on It
  //
  
  //uint8_t *imageData = sized_img.getImgBuff(); // Get grayscale image.
  //for (uint32_t i=0; i<IMPULSE_IMAGE_DATA_SIZE; i++)
  //{
  //  sizedAddy[i] = imageData[i];                          // Couldn't get memcpy() working so I am looping.
  //}
  //free(imageData);

  ///ei_printf("Copying output_img into printAddy[]. output_img.getImgSize()=%d...\n", output_img.getImgSize());
  uint8_t *imageData = output_img.getImgBuff(); // Get grayscale image.
  for (uint32_t i=0; i<IMAGE_DATA_SIZE; i++)
  {
    paintAddy[i] = imageData[i];                          // Couldn't get memcpy() working so I am looping.
  }
  //free(imageData);

  StateLED1 = !StateLED1;
  digitalWrite(LED1, StateLED1);  

  gotAnImage = true;

  //ei_printf("Leaving CamCB()...\n");
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
  ei_printf("[FLOW] Entering camera_start_continuous()...\n");
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
  //ei_printf("Starting the camera:\n");

  // The begin() method function is the first function to call when using "theCamera".
  // Start "theCamera" with
  //    Frame Rate = 5 images per second
  //    Image data pixel format = YUV 422
  // YUV 422 is a YUV model that defines one luminance component (Y) meaning physical linear-space brightness,
  // and two chrominance components, called U (blue projection) and V (red projection) respectively, aka YCbCr.
  err = theCamera.begin(1,                           // Number of image buffers. 
                        CAM_VIDEO_FPS_5,             // Defined in "camera.h". Default: 30 FPS
                        RAW_WIDTH,                   // Defined in this file.  Default: QVGA=320.
                        RAW_HEIGHT,                  // Defined in this file.  Default: QVGA=240.
                        CAM_IMAGE_PIX_FMT_YUV422);   // Defined in "camera.h". Default: CAM_IMAGE_PIX_FMT_YUV422
  // Using "CAM_IMAGE_PIX_FMT_JPG" results in "ERR: Converting processing failed. See convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY)".
  // Using "CAM_IMAGE_PIX_FMT_GRAY" results in "loop() crashing even with the following remmed:
  //    err = sized_img.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);
  // Using "CAM_IMAGE_PIX_FMT_RGB565" results in "ERR: Converting processing failed. See convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY)".

  if (err && doPrintCamErr)
  {
    ei_printf("--- --- --- --- --- ERR: Starting the camera failed. See theCamera.begin() in camera_start_continuous()...\n");      
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
  //ei_printf("Starting sending data:\n");
  startCamera = false;
  err = theCamera.startStreaming(startCamera, CamCB);  // Register the callback function that obtains the Preview image.
  if (err && doPrintCamErr)
  {
    ei_printf("--- --- --- --- --- ERR: In camera_start_continuous()...Start sending the data failed. See theCamera.startStreaming()...\n");      
    printCamErr(err);
  }

  /* Auto white balance configuration */
  Serial.println("Set Auto white balance parameter...");
  err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  if (err != CAM_ERR_SUCCESS)
  {
    printCamErr(err);
  }

  if (doPrintCamErr)
  {
    ei_printf("INFO: Started camera recording...\n");
  }
}

/*******************************************************************************************************************
 * @brief   Arduino setup function
 * @details 
 * @param   void
 * @return  void
 *******************************************************************************************************************/
void setup()
{
  // Setup on-board LEDs.
  pinMode(LED0, OUTPUT);  // Toggles when entering    CamCB().
  pinMode(LED1, OUTPUT);  // Toggles before calling   camera_classify().
  pinMode(LED2, OUTPUT);  // Toggles when we classify an image with high confidence.
  pinMode(LED3, OUTPUT);  // Toggles when entering    ei_camera_cutout_get_data().
  
  Serial.begin(115200);

  // Since we can't clear the Serial Monitor on program startup,
  // we print a demark.
  ei_printf("... ... ... ... ....\n");
  ei_printf("... ... ... ... ....\n");
  ei_printf("... ... ... ... ....\n");
  ei_printf("Spresense Vision Model Inferencing starting up...\n");

  // Print what INO file we are running.
  ei_printf(__FILE__ " " __DATE__ " " __TIME__);
  ei_printf("  IDE "); ei_printf(ARDUINO); ei_printf("\n");

  // Summary of inferencing settings (from model_metadata.h)
  ei_printf("Inferencing settings:\n");
  ei_printf("\tImage resolution: %dx%d\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);  // Stored in "model_metadata.h" = 96x96 // Depends on you Impulse. Go larger if the Model will fit in memory.
  ei_printf("\tFrame size: %d\n",          EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);                     // Stored in "model_metadata.h" = 9216
  ei_printf("\tImage frame size %d\n",     EI_CLASSIFIER_NN_INPUT_FRAME_SIZE);                      // Stored in "model_metadata.h" = 9216
  ei_printf("\tImage type: %d\n",          EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME);                    // Stored in "model_metadata.h" = `
  ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));  // Stored in "model_variables.h" = 3 (depends on classes in your model)
  ei_printf("\tNo. of labels: %d\n",              EI_CLASSIFIER_LABEL_COUNT);                                                           // Stored in "model_metadata.h"  = 3 (depends on classes in your model)
  ei_printf("\tEI_CLASSIFIER_INPUT_WIDTH: %d\n",  EI_CLASSIFIER_INPUT_WIDTH);                                                           
  ei_printf("\tEI_CLASSIFIER_INPUT_HEIGHT: %d\n", EI_CLASSIFIER_INPUT_HEIGHT);

  /* Sony camera can't resize smaller then 96x64 */
  if(EI_CLASSIFIER_INPUT_WIDTH < 96 || EI_CLASSIFIER_INPUT_HEIGHT < 64)
  {
    ei_printf("ERR: Image resolution too small should be at least 96x64.\r\n");
    return;
  }
    
  for (int ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)                                            // Stored in "model_metadata.h" = 
  {
    ei_printf("\t\tClass %i: %s\r\n", ix, ei_classifier_inferencing_categories[ix]);                // <labels> of your classes.
  }
  
  ei_printf("\tRaw Image Width: %d Height: %d\n", RAW_WIDTH, RAW_HEIGHT);                           // Stored in this file.         = Raw Image Width: 320 Height: 240
  ei_printf("\tClip Width:      %d Height: %d\n", CLIP_WIDTH, CLIP_HEIGHT);                         // Stored in this file.         = Clip Width: 96 Height: 96
  ei_printf("\tOffset X:        %d Y:      %d\n", OFFSET_X, OFFSET_Y);                              // Stored in this file.         = Offset X: 112 Y: 72
    
  #if defined(CMSIS_NN)
    // CMSIS NN software library, a collection of efficient neural network kernels developed
    // to maximize the performance and minimize the memory footprint of neural networks on Cortex-M processor cores.
    ei_printf("Enabled CMSIS_NN\n"); 
  #endif

  #if defined(EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN )
    ei_printf("Enabled EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN : %d\n", EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN ); 
  #endif

  Serial.println("Waiting for A, B or C from keyboard.");
  Serial.println("A starts filenames at 1.");
  Serial.println("B starts filenames at 50.");
  Serial.println("C starts filenames at 100.");
  while(false==startCapture)
  {
    handleSerial();
    delay(333);
  }

  // This sets up CamCB() to act as a callback whenever the Camera emits a captures image.
  camera_start_continuous(DEBUG_IT);                        // Kick-off the inferencing loop.  
}

/*******************************************************************************************************************
 * @brief   Arduino main function.
 * @details 
 * @param   void
 * @return  void
*******************************************************************************************************************/
void loop()
{
  CamErr err;

  ei_printf("In loop()...\n");  

  if (false==startCamera)
  {
    // Register the callback function that obtains the Preview image.  
    startCamera = true;
    err = theCamera.startStreaming(startCamera, CamCB);
    if (err)
    {
      ei_printf("--- --- --- --- --- ERR: In loop()...Start sending the data failed. See theCamera.startStreaming()...\n");      
      printCamErr(err);
    }
  }

  // We got an image so go classify it.  
  //   The call to camera_classify() could be moved to the end of CamCB() but
  //   loop() will not be returned to since CamCB() will fire about every 400 ms given a chance too,
  //   then camera_classify() is taking baout 1200 ms. Therefore, CamCB() will be re-entered as soon as
  //   camera_classify() exists. 
  if (gotAnImage)
  {    
    startCamera = false;
    err = theCamera.startStreaming(startCamera);
    if (err)
    {
      ei_printf("--- --- --- --- --- ERR: In loop()...Stop sending the data failed. See theCamera.startStreaming()...\n");      
      printCamErr(err);
    }

    gotAnImage = false;

    // +-------------------------------------------+
    // | Classify the captured image.              |
    // +-------------------------------------------+
    ///ei_printf("Calling camera_classify() from CamCB()...\n");
    camera_classify(false);
  }
  
  // Do other stuff like maybe sleep...
  sleep(5);
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
/* Flowchart ---------------------------------------------------------------------------------------------------- */
/*
            setup()
              camera_start_continuous()                          

            loop()
              Entering CamCB()...
              Entering camera_classify()...
              Entering Grayscale, ei_camera_cutout_get_data()...offset=0, length=1024
              Entering Grayscale, ei_camera_cutout_get_data()...offset=1024, length=1024
              Entering Grayscale, ei_camera_cutout_get_data()...offset=2048, length=1024
              Entering Grayscale, ei_camera_cutout_get_data()...offset=3072, length=1024
              Entering Grayscale, ei_camera_cutout_get_data()...offset=4096, length=1024
              Entering Grayscale, ei_camera_cutout_get_data()...offset=5120, length=1024
              Entering Grayscale, ei_camera_cutout_get_data()...offset=6144, length=1024
              Entering Grayscale, ei_camera_cutout_get_data()...offset=7168, length=1024
              Entering Grayscale, ei_camera_cutout_get_data()...offset=8192, length=1024
              Predictions (DSP: 9 ms., Classification: 1264 ms., Anomaly: 0 ms, Total: 1273 ms.): 
                No objects found

              Entering CamCB()
              Entering camera_classify()
              Entering Grayscale, ei_camera_cutout_get_data()
                . . .                
              Entering Grayscale, ei_camera_cutout_get_data()
              Predictions (DSP: 9 ms., Classification: 1264 ms., Anomaly: 0 ms, Total: 1273 ms.): 
                No objects found

                . . .
*/