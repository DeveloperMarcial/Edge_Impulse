---Take a Snapshot

...in setup()

  // We are not taking snapshots but this is how to set it up.
  // Still image format must be JPEG to allow for compressed storage/transmit.
  //ei_printf("Set format:\n");

  // Test to see high res.
  //err = theCamera.setStillPictureImageFormat( CAM_IMGSIZE_5M_H,
  //                                            CAM_IMGSIZE_5M_V,
  //                                            CAM_IMAGE_PIX_FMT_JPG );
  
  err = theCamera.setStillPictureImageFormat( RAW_WIDTH,
                                              RAW_HEIGHT,
                                            //CAM_IMAGE_PIX_FMT fmt = CAM_IMAGE_PIX_FMT_JPEG);
                                            //CAM_IMAGE_PIX_FMT_YUV422);
                                              CAM_IMAGE_PIX_FMT_JPG );
                                            //CAM_IMAGE_PIX_FMT_GRAY);
                                            //CAM_IMAGE_PIX_FMT_RGB565);
                                         
  if (err && doPrintCamErr)
  {
    ei_printf("ERR: Setting the still image format failed. See theCamera.setStillPictureImageFormat())...\n");      
    printCamErr(err);
  }

<HR>

...call this routine when you want to press the shutter button on your camera.
/*******************************************************************************************************************
 * @brief   Take a new snapshot with the camera and save it to SD card as a JPEG.
 * @details 
 * @param   none
 * @return  void
 *******************************************************************************************************************/
void SaveCameraToSdCard()
{
  char filename[200];
  
  // Tkae a snapshot and save a JPEG.
  if(take_picture_count < 20)
  {
    CamImage img = theCamera.takePicture();   // Push the shutter.

    if (img.isAvailable())
    {
      sprintf(filename, "photos/photo_%d.jpg", take_picture_count);
      ei_printf("INFO: saving %s to SD card...", filename);
      theSD.remove(filename);
      File myFile = theSD.open(filename, FILE_WRITE);
      myFile.write(img.getImgBuff(), img.getImgSize());
      myFile.close();
    }
    else
    {
      Serial.println("ERR: Failed to compress and save image, check that camera and SD card are connected properly!");
    }
  }
  take_picture_count++;
  Serial.println("Image count incremented.");

}
<HR>
---Caputre Camera for EdgeImpulse (EI) Signal used by EI Classifier as a Color Image
...in CamCB()
  //ei_printf("RGB565 Capture\n");
  err = sized_img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
  if (err)
  {   
    ei_printf("ERR: Converting processing failed. See convertPixFormat(CCAM_IMAGE_PIX_FMT_RGB565)...\n");      
  }

  #endif
  if (err)
  {    
    printCamErr(err);
  }

<HR>

/*******************************************************************************************************************
 * @brief      Convert RGB565 raw camera buffer to RGB888.
 * @details    This is the routine that gets the data for the Impulse "signal".
 *             This is from the "nano_ble33_sense_camera.ino" example.
 * @param[in]  offset,  pixel offset of raw buffer
 * @param[in]  length,  number of pixels to convert
 * @param[out] out_buf, pointer to store output image
*******************************************************************************************************************/
int ei_camera_cutout_get_data(size_t offset, size_t length, float *out_ptr)
{
    //ei_printf("[FLOW] Entering RGB565, ei_camera_cutout_get_data()...\n");    
    StateLED3 = !StateLED3;
    digitalWrite(LED3, StateLED3);

    size_t pixel_ix   = offset * 2; 
    size_t bytes_left = length;
    size_t out_ptr_ix = 0;

    // Grab the value and convert to RGB.
    uint8_t *buffer = sized_img.getImgBuff(); // Get RGB565 image.

    // Read byte for byte.
    while (bytes_left != 0)
    {
      uint16_t pixel = (buffer[pixel_ix]   << 8) | 
                        buffer[pixel_ix+1] <<0;
      uint8_t r, g, b;
      r = ( (pixel >> 11 ) & 0x1f) << 3;
      g = ( (pixel >> 5  ) & 0x3f) << 2;
      b =   (pixel & 0x1f)         << 3;

      // Then convert to "out_ptr" format.
      float pixel_f = (r << 16) + 
                      (g <<  8) + 
                      (b <<  0);
      out_ptr[out_ptr_ix] = pixel_f;

      // ...and go to the next pixel.
      out_ptr_ix++;
      pixel_ix+=2;
      bytes_left--;
    }

    // ...and done converting RGB565 raw camera buffer to RGB888.
    return 0;
}
#endif

<HR>

---Parsing EdgeImpulse (EI) Image Classification Classifier() Results.
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
#else
    // Vision Model Details: Image Classification.

    // Determine max inference value so we can star it when we display the list of predictions.      
    uint8_t maxInferenceIdx = 0;
    for (size_t ix = 1; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
      if( ei_result.classification[ix].value > ei_result.classification[maxInferenceIdx].value)
      {
        maxInferenceIdx = ix;
      }          
    }
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
      if (maxInferenceIdx == ix)
      {
        // Highlight the label with the maximum prediction value.
        ei_printf("*** %s:\t\t", ei_result.classification[ix].label);
        ei_printf_float(ei_result.classification[ix].value);
        ei_printf(" ***\n");
      }
      else
      {      
        // Show other labels with their prediction value.
        ei_printf("    %s:\t\t", ei_result.classification[ix].label);
        ei_printf_float(ei_result.classification[ix].value);
        ei_printf("\n");
      }
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    // Handle Anomaly Details.
    ei_printf("    anomaly score: ");
    ei_printf_float(ei_result.anomaly);
    ei_printf("\n");
#endif
#endif

<HR>

---Untested and probable JUNK Routines

/*******************************************************************************************************************
 * @brief   Save sized_img to SD card as a JPEG.
 * @details 
 * @param   none
 * @return  void
 *******************************************************************************************************************/
/*
void saveImageToSdCardAsBMP()
{ //mike
  char filename[200];
  // Take a snapshot and save a JPEG.
  if(take_picture_count < 20)
  {
    if (sized_img.isAvailable())
    {
      sprintf(filename, "photos/fomo_%d.txt", take_picture_count);
      ei_printf("INFO: saving %s to SD card...", filename);
      theSD.remove(filename);
      File myFile = theSD.open(filename, FILE_WRITE);
      myFile.write(sized_img.getImgBuff(), sized_img.getImgSize());
      myFile.close();
    }
    else
    {
      Serial.println("ERR: Failed to compress and save sized_img, check that camera and SD card are connected properly!");
    }
  }
  take_picture_count++;
  Serial.println("Image count incremented.");

}
*/

<HR>

---Misc
Endianness of Spresense RGB565 Image Data
See https://forum.edgeimpulse.com/t/endianness-of-spresense-rgb565-image-data/5351

--EdgeImpulse Python tool to convert a Framebuffer to a jpg

My efforts could not make this work...

As a first step I tried to save the Framebuffer and view it as a JPEG. Saving the Framebuffer is easy enuf to a text file. Then using [edge-impulse-framebuffer2jpg](https://docs.edgeimpulse.com/reference/remote-management/remote-mgmt-serial-protocol#at+snapshot-1) from the EI CLI to make a JPEG but this method fails.

Digging deeper I found file `framebuffer-to-jpg.js` that executes:

```
const s = fs_1.default.readFileSync(frameBufferFile, 'utf-8')
snapshot = Buffer.from(s, 'base64');
```

The input text file saved by the Spresense is 9216 bytes as shown in Notepad++ with UTF-8 encoding or in Windows File Explorer.

When I print debug info from within `framebuffer-to-jpg.js` I get:
														
* console.log(Buffer.from(s).toString('base64'));		// Results: 12480 characters when log pasted into Notepad++. (**Why this is bigger than 9216?**)
* console.log("s.length=       " + s.length);			// Results: s.length=9198 (**Maybe a 18 byte header got stripped off?**)	
* console.log("snapshot.length=" + snapshot.length);	// Results: snapshot.length=1 (**Why is the length for snapshot not the expected 9216 or 27648 values?**)

