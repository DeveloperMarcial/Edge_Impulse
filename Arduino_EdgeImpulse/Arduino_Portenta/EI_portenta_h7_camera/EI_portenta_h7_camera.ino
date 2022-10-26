/************************************************************************************
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
 ************************************************************************************
 * The MIT License (MIT)                                                            *
 *                                                                                  *
 * Modifications to the Software "portenta_h7_microphone_continuous.ino"            *
 * and saved herin as the Software "EI_portenta_h7_camera.ino"                      *
 * Copyright (c) 2022 M. Marcial                                                    *
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
 */

/* Includes ---------------------------------------------------------------- */
#include "RPC.h"                                            // Comes with the mbed board installation. Used for M4-M7 communications.

//version=1.0.18
#include <FOMO_Washers_and_Screws_96x96_inferencing.h>      // Edge Impulse generated code to handle the Machine Learning in my Arduino code.

#include "camera.h"                                         // Arduino library to handle color, resolution, framebuffer, framerate, etc.
                                                            // Also for frame capturing, motion detection and pattern recognition. 
#include "himax.h"                                          // Similar to "camera.h" but specific for HM01B0.
                                                            //    The HM01B0 is an ultralow power CMOS Image Sensor that enables the integration of an
                                                            //    “Always On” camera for computer vision applications such as gestures, intelligent ambient light
                                                            //    and proximity sensing, tracking and object identification.
                                                            // The Vision Shield Camera:
                                                            //    Specs: https://store-usa.arduino.cc/products/arduino-portenta-vision-shield-ethernet
                                                            //    Info: The onboard camera is a Himax HM-01B0 camera module
                                                            //    Camera Mfg: https://www.himax.com.tw/products/cmos-image-sensor/always-on-vision-sensors/hm01b0/ 
#include "edge-impulse-sdk/dsp/image/image.hpp"             // Edge Impulse library to handle image conversions, cropping
#include <string.h>                                         // Arduino library: https://www.arduino.cc/reference/en/language/variables/data-types/stringobject/ 

/* Private Constants and Structures ---------------------------------------- */
typedef struct {
    size_t width;
    size_t height;
} ei_device_resize_resolutions_t;

typedef struct {
		int x;
		int y;
		int width;
		int height;
    float value;  // Prediction value for what is inside bounding box.
	} mm_bounding_box_t;


/*
** @brief      A complete package has 3 screws and 3 washers in the image.
*/
const int bounding_box_count = 3;
mm_bounding_box_t bb_listScrew[bounding_box_count] =  {
                                                       {0, 1000, 0, 0, 0.0},
                                                       {0, 1000, 0, 0, 0.0},
                                                       {0, 1000, 0, 0, 0.0},
                                                      };
mm_bounding_box_t bb_listWasher[bounding_box_count] = {
                                                       {0,   -1, 0, 0, 0.0},
                                                       {0,   -1, 0, 0, 0.0},
                                                       {0,   -1, 0, 0, 0.0},
                                                      };

/* Constant Definitions ---------------------------------------------------- */

//CAMERA RESOLUTION: 320 x 320 active pixel resolution with support for QVGA.
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS           320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS           240

// ---------------------------------------------------------+
// Frame Buffer allocation options:                     //  |
// ---------------------------------------------------------+
//    - Static (default if neither below is chosen)     //  |
// No #define defined.                                  //  |
//    - Heap                                            //  |
//#define EI_CAMERA_FRAME_BUFFER_HEAP                   //  |
//    - SDRAM                                           //  |
#define EI_CAMERA_FRAME_BUFFER_SDRAM                    //  |
// ---------------------------------------------------------+

/* Edge Impulse Machine Learning Model Function Definitions ---------------- */
bool ei_camera_init(  void);
void ei_camera_deinit(void);
bool ei_camera_capture(          uint32_t img_width, uint32_t img_height, uint8_t *out_buf) ;
int  calculate_resize_dimensions(uint32_t out_width, uint32_t out_height, uint32_t *resize_col_sz, uint32_t *resize_row_sz, bool *do_resize);

/* INFO: Portenta SDRAM ---------------------------------------------------- */
#ifdef EI_CAMERA_FRAME_BUFFER_SDRAM
/*
    The board comes with an hefty 8MB of external fast RAM, which can be used:
    - as a framebuffer (raw mode)
    - as an expansion of on-chip RAM to store "standard" data
    Portenta has an external 8MB RAM module (albeit slower), you could have a huge heap by just using Portenta_SDRAM
    library and replacing the calls to malloc and free with ea_malloc and ea_free.
    Can also do this:
    #include <SDRAM.h> //Use the SDRAM library for the Portenta.
    SDRAMClass ram;
    uint8_t *b;
    void setup()
    {
      ram.begin();
      //Example for a uint8 array.
      b = (uint8_t *)ram.malloc(320 * 240 * sizeof(uint8_t));
    }
    SDRAM Example: https://github.com/arduino/ArduinoCore-mbed/issues/38
*/
#include "SDRAM.h"                      // Storage for frame buffer in 8Mb SDRAM.
#endif

/* INFO: TFLite Arena Allocation Issue ------------------------------------- */
/*
    NOTE: If you run into TFLite arena allocation issue.
          Failed to allocate TFLite arena (764,166 bytes) <-- My version of "boards.local.txt" did not resolve this error.
          ERR: Failed to run classifier (-6)
  
    This may be due to may dynamic memory fragmentation.

    "boards.local.txt" introduced in Arduino IDE 1.6.6. This file can be used to override properties defined in boards.txt or
    define new properties without modifying boards.txt. It must be placed in the same folder as the boards.txt it supplements.
 
    Try defining "-DEI_CLASSIFIER_ALLOCATION_STATIC" in boards.local.txt
    (create if it doesn't exist) and copy this file to
    <                    ARDUINO_CORE_INSTALL_PATH>\arduino\hardware\<mbed_core>  \<core_version>/
     C:\Users\<username>>\AppData\Local\Arduino15\packages\arduino\hardware\mbed_portenta\3.1.1\boards.local.txt
        envie_m7.build.extra_flags=-DEI_CLASSIFIER_ALLOCATION_STATIC <--Added 1 line
   
    See
    (https://support.arduino.cc/hc/en-us/articles/360012076960-Where-are-the-installed-cores-located-)
    to find where Arduino installs cores on your machine.
 
    If the problem persists then there's not enough memory for this model and application.
    The error then means we can’t allocate enough memory to load the NN.
*/

/* INFO: Portenta H7 Board ------------------------------------- */
/*
    Arm® Cortex®-M7 core at up to
      480 MHz with double-precision
      FPU and 16K data + 16K
      instruction L1 cache
    Arm® 32-bit Cortex®-M4 core at
      up to 240 MHz with FPU, Adaptive
      real-time accelerator (ART AcceleratorTM)

    Arduino Loader:
      Magic Number (validation):  a0
      Bootloader version:         22
      Clock source:               External oscillator
      USB Speed:                  USB 2.0/Hi-Speed (480 Mbps)
      Has Ethernet:               Yes
      Has WiFi module:            Yes
      SDRAM size:                  8 MB
      Flash QSPI size:            16 MB
      Has Video output:           Yes
      Has Crypto chip:            Yes

    Use "STM32H747_manageBootloader.ino" to
      load or update the "Arduino bootloader"
      load           the "MCUboot bootloader"

     2 Mbytes of Flash Memory with read-while-write support
     1 Mbytes of RAM
     8 Mbytes SDRAM
    16 Mbytes NOR Flash (Flash QSPI)
    If you need more memory, Portenta H7 can be special ordered with up to
         64 MByte of SDRAM, and
        128 MByte of QSPI Flash.

    The flash memory can be split by the Aruino IDE:
    M7      M4
    1.0MB   1.0MB
    1.5MB   0.5MB
    2.0MB   8.0 SDRAM

    QSPI can be be exposed as a USB Mass Storage
      "AccessFlashAsUsbDisk.ino" shows how to expose a QSPIF BlockDevice (16MB external flash on the Portenta H7)
      as an USB stick. It can be adapted to any kind of BlockDevice (FlashIAP or either RAM via HeapBlockDevice)
      Before loading this example, make sure you execute WiFiFirmwareUpdater.ino
      to create and format the proper partitions.
      Running "AccessFlashAsUsbDisk.ino" makes a PC see the Portenta as a USB Mass Storage.
      You can drag-n-drop files to/from the Portenta 16 MB QSPI.
      You will probably see:
        4343WA1.BIN 412 KB
        CaCert.PEM   62 KB

    This Sketch uses 297,664 bytes (37%) of program storage space.
    Maximum is  786,432 bytes.
*/

/* INFO: Portenta Speed Flag ----------------------------------------------- */
/*
    "C:\Users\<username>\AppData\Local\Arduino15\packages\arduino\hardware\mbed_portenta\3.1.1\variants\PORTENTA_H7_M7\cflags.txt" file,
    where you switch the default setting on line 14 from -Os to -O3.
*/

/* INFO: Edge Impulse Studio ------------------------------------- */
/*
    Build in EI "Arduino Portenta H7"
    If this build fails try reducing the image data. From the EI Studio goto the left side menu "Impulse Design" - "Create Image".
    On the right side the 1st block is "Image Data". Change "Image Width" to 96. Change "Image Height" to 96.

    Put Portenta in firmware load mode by double-clicking the RESET button on the Portenta.

    The execute "flash_windows.bat"
      Finding Arduino Mbed core...
      arduino:mbed_portenta 3.1.1     3.1.1  Arduino Mbed OS Portenta Boards
      Finding Arduino Mbed core OK
      Finding Arduino Portenta H7...
      Finding Arduino Portenta H7 OK at COM7 <-- Not sure about this. The Portenta is on COM8.
      TOUCH: error during reset: opening port at 1200bps: Invalid serial port
      dfu-util 0.10-dev

      Copyright 2005-2009 Weston Schmidt, Harald Welte and OpenMoko Inc.
      Copyright 2010-2021 Tormod Volden and Stefan Schmidt
      This program is Free Software and has ABSOLUTELY NO WARRANTY
      Please report bugs to http://sourceforge.net/p/dfu-util/tickets/

      Warning: Invalid DFU suffix signature
      A valid DFU suffix will be required in a future dfu-util release
      Opening DFU capable USB device...
      Device ID 2341:035b
      Device DFU version 011a
      Claiming USB DFU Interface...
      Setting Alternate Interface #0 ...
      Determining device status...
      DFU state(2) = dfuIDLE, status(0) = No error condition is present
      DFU mode device DFU version 011a
      Device returned transfer size 4096
      DfuSe interface name: "Internal Flash   "
      Downloading element to address = 0x08040000, size = 652452
      Erase           [=========================] 100%       652452 bytes
      Erase    done.
      Download        [=========================] 100%       652452 bytes
      Download done.
      File downloaded successfully
      Transitioning to dfuMANIFEST state
      
      Flashed your Arduino Portenta H7 development board

      To set up your development with Edge Impulse, run 'edge-impulse-daemon'
      To run your impulse on your development board, run 'edge-impulse-run-impulse'
      Press any key to continue . . .
 */

#define ALIGN_PTR(p,a)   ((p & (a-1)) ?(((uintptr_t)p + a) & ~(uintptr_t)(a-1)) : p)

/*
** @brief      Since we will be evaluating the location of the screws versus the washers later we start at a known location.
*/
void InitializeBB()
{
  // Initialize Bounding Box List[].
	for (size_t ix = 0; ix < bounding_box_count; ix++)
	{
    bb_listScrew[ix].x       =  0;
    bb_listScrew[ix].y       =  1000; // Screws must be below washers for a correct package.
    bb_listScrew[ix].width   =  0;    // Here we force screws well above washers. (a value outside the photo)
    bb_listScrew[ix].height  =  0;
    bb_listScrew[ix].value   =  0.0;

    bb_listWasher[ix].x      =  0;    // Washers must be above screws for a correct package.
    bb_listWasher[ix].y      = -1;    // Here we force washers below screws.
    bb_listWasher[ix].width  =  0;
    bb_listWasher[ix].height =  0;
    bb_listWasher[ix].value  =  0.0;
  }
}

/* Private Variables ------------------------------------------------------- */

/*
** @brief      Points to the output of the capture
*/
static uint8_t *ei_camera_capture_out = NULL;

/*
** @brief Used to store the raw frame.
*/
#if defined(EI_CAMERA_FRAME_BUFFER_SDRAM) || defined(EI_CAMERA_FRAME_BUFFER_HEAP)
static uint8_t *ei_camera_frame_mem;
static uint8_t *ei_camera_frame_buffer; // 32-byte aligned
#else
static uint8_t ei_camera_frame_buffer[EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS] __attribute__((aligned(32)));
#endif

static bool debug_nn          = false;  // Set this to true to see e.g. features generated from the raw signal
static bool is_initialised    = false;
static bool is_ll_initialised = false;
HM01B0 himax;                           // Reference Vision Shield on-board camera.
static Camera cam(himax);
FrameBuffer fb;                         // Frame Buffer.

bool pinD05IsAsserted         = false;  // Simulate pulling D5 low indicating yellow bin is in place.
bool pauseCapture             = false;  // Pause M7 loop(). (Used for debugging.)

/* Local Subroutines ------------------------------------------------------- */

/**
 * @brief      Check if new serial data is available.
 *
 * @return     Returns number of available bytes.
 */
int ei_get_serial_available(void)
{
    return Serial.available();
}

/**
 * @brief      Get next available byte.
 *
 * @return     Byte read.
 */
char ei_get_serial_byte(void)
{
    return Serial.read();
}

/* Setup M4 to M7 Communication -------------------------------------------- */
int m7IntGlobal = 1234;                 // Set an M7 core global variable.

int setOnVarInM7(int varFromCM4)
{
    // TODO: Create a message buffer to feed println() since setOnVarInM7()
    //       can be triggered async to actions in this code.
    //       Otherwise, the Serial Monitor can get garbled.
    m7IntGlobal = (int)varFromCM4;
    Serial.println("M4 sent:" + String(m7IntGlobal));
    switch (m7IntGlobal)
    {
      case 0:
      case 42:
      {
          pinD05IsAsserted = true;
          Serial.println("pinD05IsAsserted = true via M4");      
          break;
      }
      default:
      {
          Serial.println("M4 sent an unknow value:" + String(m7IntGlobal));
          break;
      }
    }
    return m7IntGlobal ;                // Return set value to M4.
}

bool micInferenceComplete = true;       // When M4 starts a mic inference it will set this false.
                                        // This assures multiple groupings of M4 RPC.println() are completely
                                        // sent to the M7, aka, the serial output is kept organized and pretty.
                                        // Otherwise, for all other M4 println() we will just let them print.
/**
 * @brief      To prevent the serial output from looking garbled we wait in
 *             RPC.avaliable() until "micInferenceComplete" asserts.
 *             NOTE: In setup() make sure to do something like:
 *                   RPC.bind("setMicInferenceComplete", setMicInferenceComplete);
 *
 * @return     Value set in M7.
 */
bool setMicInferenceComplete(bool var1_FromCM4) 
{
    micInferenceComplete = (bool)var1_FromCM4;// Set M7 variable to M4 sent in value.
    return micInferenceComplete;              // Return value set in M7.                   
}

/**
 * @brief      Handle keyboard input from user.
 *             Check if new serial data is available.
 */
void handleSerial()
{
  while (Serial.available() > 0)
  {
    char incomingCharacter = Serial.read();
    if ( (10==incomingCharacter) || (13==incomingCharacter) )
    {
      continue;                         // Skip carriage return and line feed.
    }
    Serial.print("incomingCharacter=");
    Serial.println(incomingCharacter);
    switch (incomingCharacter)
    {      
      case 'I':
      case 'i':
      {
        // Show audio inferencing configuration in M4.
        RPC.call("getSummaryOfInferencingOnM4", 0xACE);
        break;
      }
      case 'L':
      case 'l':
      {
        // Simulate asserting pin D05
        pinD05IsAsserted = true;
        Serial.println("pinD05IsAsserted = true via keyboard");      
        delay(5555);
        break;
      }
      case 'P':
      case 'p':
      {
        // Pause M7 loop() below.
        pauseCapture = !pauseCapture;
        Serial.print("pauseCapture=");
        Serial.println(pauseCapture);
        break;
      }
      case 'Q':
      case 'q':
      {
        // Toggle pause of M4.
        int res = RPC.call("setOneVarInM4", 1).as<int>();
        break;
      }
      case 'U':
      case 'u':
      {
        // Show M4 firmware version.
        // Send value to M7 indicating the successful results.
          RPC.call("getM4Version", 0xACE);     
        break;
      }
      case 'V':
      case 'v':
      {
        // Show M7 firmware version.
        // Ex) C:\Users\<username>\OneDrive\Documents\Arduino\EI_portenta_h7_camera\
        //     EI_portenta_h7_camera.ino
        //     Jun 26 2022 13:02:34
        //     IDE 10607
        Serial.print(__FILE__ " " __DATE__ " " __TIME__);
        Serial.print("  IDE "); Serial.println(ARDUINO);
        break;
      }
      case 'X':
      {
        // Do something eXtreme!
        break;
      }
    }
  }
}

/**
* @brief      Arduino setup() function.
*/
void setup()
{
    // Put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("Edge Impulse Camera Image Inferencing Demo on M7.\n");
    Serial.println("Edge Impulse Microphone Audio Inferencing Demo on M4.\n");
    Serial.println("A correct Audio Inference on M4 will trigger an action in M7.\n");

    Serial.print(__FILE__ " " __DATE__ " " __TIME__);
    Serial.print("  IDE "); Serial.println(ARDUINO);

    /* Setup M4 to M7 Communication ---------------------------------------- */
    RPC.begin(); 

    //bootM4(); //LL_RCC_ForceCM4Boot();// Including the bootM4() causes the M4 core to run on startup.

    RPC.bind("setOneVar", setOnVarInM7);// These arguments do not have to be the same.
                                        // M4 calls "setOnVar()".
                                        // M7 has a routine called "setOnVarInM7()".
                                        // Thus, bind() is a bounden between the name and function.    

    // When an audio prediction is complete on the M4,
    // the M4 sends multiple text statements to the M7 to be sent to the serial output.
    // To assure the M7 prints all these statements as one grouping, we use this binding.
    RPC.bind("setMicInferenceComplete", setMicInferenceComplete);   

    /* Setup Camera -------------------------------------------------------- */
#ifdef EI_CAMERA_FRAME_BUFFER_SDRAM
    // initialise the SDRAM
    SDRAM.begin(SDRAM_START_ADDRESS);   // Same as SDRAM.begin(); 
#endif

    if (ei_camera_init() == false)
    {
        ei_printf("ERROR: Failed to initialize Camera!\r\n");
    }
    else {
        ei_printf("Camera initialized\r\n");
    }

    /* Configure Edge Impulse Machine Learning Library --------------------- */
    for (size_t ix = 0; ix < ei_dsp_blocks_size; ix++)
    {
        ei_model_dsp_t block = ei_dsp_blocks[ix];
        if (block.extract_fn == &extract_image_features)
        {
            ei_dsp_config_image_t config = *((ei_dsp_config_image_t*)block.config);
            int16_t channel_count = strcmp(config.channels, "Grayscale") == 0 ? 1 : 3;
            if (channel_count == 3)
            {
                ei_printf("WARN: You've deployed a color model, but the Arduino Portenta H7 only has a monochrome image sensor. Set your DSP block to 'Grayscale' for best performance.\r\n");
                break; // Pnly print this once.
            }
        }
    }
    pinMode(LEDR, OUTPUT);              // On-board RED LED.
    pinMode(LEDG, OUTPUT);              // On-board GRN LED.
    pinMode(LEDB, OUTPUT);              // On-board BLU LED.
    pinMode(D5, INPUT_PULLUP);          // Assume this is a limit switch indicating empty package is in place.
}

bool packageLoading     = false;        // We start in a "not loading" state.
bool packageCorrect     = false;        // We assume the package is not correct.
int screwIdx            = 0;            // Index into screw buffer[].
int washerIdx           = 0;            // Index into washer buffer[].
long packageLoadingTime = 0;            // How fast die the package get made in.

/**
* @brief      Get data and run inferencing.
*
* @param[in]  debug  Get debug info if true.
*/
void loop()
{
    handleSerial(); // Allow user to control aspects of loop(). Mainly used for simulating I/O.

    if (pauseCapture)
    {
      handleSerial();                   // Pause video capture so we can see microphone capture.
      delay(500);
    }

    /* Handle M4 to M7 Communication --------------------------------------- */
    //Serial.println("M7 global variable: "+ String(m7IntGlobal));
    // Start printing the M4 status update to the serial output.
    // On M7, print everything that is received over the RPC stream interface.
    // Buffer it, otherwise all characters will be interleaved by other prints.
    String buffer = "";
    while (RPC.available() && micInferenceComplete)
    {
      // M4 has sent an RPC println().
      buffer += (char)RPC.read();       // Fill the buffer with characters.
    }

    if (buffer.length() > 0)
    {
      // Start printing the M7 status update to the serial output.
      Serial.print(buffer);
    }

    // TODO: When yellow bin enters frame set packageLoading to true. Do this via vision ML.
    // For now we assume package loading start immediately after the last package got loaded AND
    // pin D5 is low indicating the package is in place via
    //    a virtual limit switch: pinD05IsAsserted ==> D5==LOW    
    // or
    //    The M4 core recognizes the keyphrase and then notifies this core, M7.
    //    The voice recognition indicates the yellow bin is in place.
    //    This is for educational purposes only, provinge we can do vision and voice ML at the same time on the same board.
    //    Vision ML on M7 and voice ML on M4.
    if (false==packageLoading && pinD05IsAsserted)
    {
      packageLoading    = true;         // Update currect state of package operation.
      pinD05IsAsserted  = false;        // Reset trigger that got us in here.
      packageCorrect    = false;        // Update final state of package operation.
      packageLoadingTime = millis();    // Set the start time.
      digitalWrite(LEDR,HIGH);          // Red is off.
      digitalWrite(LEDG,HIGH);          // Green is off.
      digitalWrite(LEDB,LOW);           // Blue is on meaning we are loading a package.
      Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Starting to load a new package...");
    }

    if(packageCorrect)
    {
      // Do nothing until package leaves frame and a new loading bin comes into the frame.
      // For now we simulate with pinD05IsAsserted, e.g. a limit switch is asserted on D05 pin or
      // a keyphrase is recognized.
    }
    else
    {

    ei_printf("\nStarting image inferencing in 2 seconds...\n");

    // Instead of "wait_ms", we'll wait on the "signal", this allows threads to cancel us...
    if (ei_sleep(2000) != EI_IMPULSE_OK)
    {
        return;
    }

    ei_printf("Taking photo with Portenta Vision camera...\n");

    // Setup "signal": sets the callback function on the "signal_t" structure to reference the inference buffer.
    ei::signal_t          signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data     = &ei_camera_cutout_get_data;  // This tells the "signal" where to get the sampled data from.

    // Take a photo.
    if (ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH,
                          (size_t)EI_CLASSIFIER_INPUT_HEIGHT, NULL) == false)
    {
        ei_printf("ERROR: Failed to capture image!\r\n");
        return;
    }
    
    ei_impulse_result_t result = { 0 }; // "result" of Classifier().

    // Run the Classifier().
    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
    if (err != EI_IMPULSE_OK)
    {
        ei_printf("ERROR: Failed to run classifier (%d)!\n", err);
        return;
    }

    // Print the Predictions from the Classifier().
    ei_printf("Predictions (DSP: %d ms.,       Classification: %d ms.,       Anomaly: %d ms.): \n",
                            result.timing.dsp, result.timing.classification, result.timing.anomaly);

#if EI_CLASSIFIER_OBJECT_DETECTION == 1

    // Evaluate the Bounding Boxes from the Classifier().
    bool bb_found = result.bounding_boxes[0].value > 0;    

    // Reset Location Data.
    InitializeBB();                     // Initialize bounding box values.
                                        // Washers must be above screws for a correct package.
                                        // Here we force washers below screws.
    screwIdx           =  0;            // Index into screw  bounding box buffer[].
    washerIdx          =  0;            // Index into washer bounding box buffer[].
    

    // Loop thru all objects identified in the photo.
    for (size_t ix = 0; ix < EI_CLASSIFIER_OBJECT_DETECTION_COUNT; ix++)
    {
        auto bb = result.bounding_boxes[ix];
        if (bb.value == 0)
        {
            // The prediction percentage for what has been identified in the this bounding box is ZERO!
            continue;                   // Continues with the next iteration in the for-loop.
        }

        // Print bounding box attributes.
        // TODO: Collect all object "bb" data and print out after this for-loop.
        //       All screws and washers are lie along their respective y-axis'.
        //       Print the "bb" data in ascending x-axis values for a given y-axis.
        //       This will allow easily evaluating what object is
        //       being mis-identified or with a low prediction accuracy.
        //       This appraising will help guide how additional image data is taken
        //       as well as adjudging existing data in the Training dataset.
        ei_printf("    %s (\t", bb.label);  // Label: "washer", "screw"
        ei_printf_float(bb.value);          // Prediction "label" is 'label', aka, like 96% sure it's a washer.
        ei_printf(") [ x: %lu, y: %lu, width: %lu, height: %lu ]\n", bb.x, bb.y, bb.width, bb.height);

        // TODO: Fix this. I found it hard to train a model with screws, washers and a relatively large yellow bin (2"x3").
        // // Determine if a new packaging bin has come into the loading area.
        // if ( (strcmp(bb.label,"package_bin")==0) && (false==packageLoading) )
        // {
        //   //Emperically got yellow bin width and height.
        //   //  package_bin (0.625000) [ x: 32, y: 64, width: 8, height: 8 ]
        //   //Figure out how to verify yellow bin is centered in frame.
        //   //  Won't be able todo this because the bb is returning the size of the centroid[8x8].
        //   if ( (0==bb.width) && (0==bb.height) )
        //   {
        //     packageLoading = true;
        //     break;  // Don't care what else is in frame buffer. We will assume it is safe to start loading package.
        //   }
        // }
        
        // Example Screw Location Data:
        //    Predictions (DSP: 0 ms., Classification: 71 ms., Anomaly: 0 ms.):
        //    washer (0.996094) [ x: 32, y: 32, width:  8, height:  8 ]
        //    washer (0.996094) [ x: 48, y: 32, width:  8, height:  8 ]
        //    washer (0.996094) [ x: 64, y: 32, width:  8, height:  8 ]
        //    screw  (0.980469) [ x: 24, y: 56, width: 16, height: 16 ]
        //    screw  (0.996094) [ x: 48, y: 56, width:  8, height: 16 ]
        //    screw  (0.988281) [ x: 64, y: 64, width: 16, height:  8 ]

        // Save the currently identified screw's location.
        if (strcmp(bb.label,"screw")==0)
        {
          int screwIdx_Buf = screwIdx;
          if(screwIdx >= bounding_box_count)     // This is based on the current bounding boxes buffer size.
          {
            screwIdx_Buf = bounding_box_count-1; // Don't overflow the buffer[].
            packageCorrect = false;              // screwIdx is too large.
          }
          // Save all location data not just the y-axis that we are currently interested in.
          bb_listScrew[screwIdx_Buf].x      = bb.x;
          bb_listScrew[screwIdx_Buf].y      = bb.y;
          bb_listScrew[screwIdx_Buf].width  = bb.width;
          bb_listScrew[screwIdx_Buf].height = bb.height;
          bb_listScrew[screwIdx_Buf].value  = bb.value;
          screwIdx = screwIdx + 1;      // Point to next save location.
        }
        // Save the currently identified washer's location.
        if (strcmp(bb.label,"washer")==0)
        {
          int washerIdx_Buf = washerIdx;
          if(washerIdx >= bounding_box_count)
          {
            washerIdx_Buf = bounding_box_count - 1;            
          }
          bb_listWasher[washerIdx_Buf].x      = bb.x;
          bb_listWasher[washerIdx_Buf].y      = bb.y;
          bb_listWasher[washerIdx_Buf].width  = bb.width;
          bb_listWasher[washerIdx_Buf].height = bb.height;          
          bb_listWasher[washerIdx_Buf].value  = bb.value;   
          washerIdx = washerIdx + 1;
        }
    } // END: Loop thru all objects identified.

    if ( (screwIdx > bounding_box_count) || (washerIdx > bounding_box_count) ) // These Idx point to next element to be loaded.
    {
      //packageCorrect = false;
      digitalWrite(LEDR,LOW);           // RED LED means bad load.
      digitalWrite(LEDG,HIGH);
      digitalWrite(LEDB,HIGH);
      if (screwIdx > bounding_box_count)
      {
        ei_printf("Reject package. To many screws: %u\n", screwIdx);
      }
      if (washerIdx > bounding_box_count)
      {
        ei_printf("Reject package. To many washers: %u\n", washerIdx);
      }
    }
    else
    {
      digitalWrite(LEDR,HIGH);  
      digitalWrite(LEDG,HIGH);
      digitalWrite(LEDB,LOW);                      // Resume loading package: LED = BLU.
      int washerCorrect = 0;
      for (int i=0; i<bounding_box_count; i++)     // Loop thru washers.
      {
        for (int j=0; j<bounding_box_count; j++)   // Loop thru srews for a given washer.
        {
          if (bb_listWasher[i].y > bb_listScrew[j].y)
          {
            washerCorrect = washerCorrect + 1;
          }
        } 
      }

      ei_printf("washerCorrect=%i\n", washerCorrect);
      if (9==washerCorrect)
      {
        ei_printf("Package loaded correctly.\n");
        digitalWrite(LEDR,HIGH);      
        digitalWrite(LEDG,LOW);         // GRN LED means good package.
        digitalWrite(LEDB,HIGH);
        packageCorrect = true;
        // Turn on LED.
        packageLoading = false;         // Start loading next package.
        ei_printf(">> >> >> >> >> >> Package loaded in %lu milliseconds. packageLoading = false.\n", millis()-packageLoadingTime);
      }
    } 

    if (!bb_found)
    {
        ei_printf("    No objects found\n");
    }
    ei_printf("^^^ ^^^ ^^^ End Image Inference ^^^ ^^^ ^^^ ^^^");
#else // Not doing Object Detection.

    // Print the Predictions from the Classifier().
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
        ei_printf("    %s: ", result.classification[ix].label);
        ei_printf_float(result.classification[ix].value);
        ei_printf("\n");
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: ");
    ei_printf_float(result.anomaly);
    ei_printf("\n");
#endif
#endif
  } // END: Package is still loading.
}

/**
 * @brief   Setup image sensor (camera), start streaming & setup the Frame Buffer.
 *
 * @retval  False if initialisation failed.
 */
bool ei_camera_init(void)
{
    if (is_initialised) return true;

    if (is_ll_initialised == false)
    {
        if (!cam.begin(CAMERA_R320x240, CAMERA_GRAYSCALE, 30))
        {
            ei_printf("ERR: Failed to initialise camera\r\n");
            return false;
        }

    #ifdef EI_CAMERA_FRAME_BUFFER_SDRAM
        // Initialize Frame Buffer in SDRAM.
        ei_camera_frame_mem = (uint8_t *) SDRAM.malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS + 32 /*alignment*/);
        if(ei_camera_frame_mem == NULL)
        {
            ei_printf("ERROR: Failed to create ei_camera_frame_mem\r\n");
            return false;
        }
        ei_camera_frame_buffer = (uint8_t *)ALIGN_PTR((uintptr_t)ei_camera_frame_mem, 32);
    #endif

        is_ll_initialised = true;
    }

#if defined(EI_CAMERA_FRAME_BUFFER_HEAP)
    // Initialize Frame Buffer in the Heap.
    ei_camera_frame_mem = (uint8_t *) ei_malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS + 32 /*alignment*/);
    if(ei_camera_frame_mem == NULL)
    {
        ei_printf("ERROR: Failed to create ei_camera_frame_mem\r\n");
        return false;
    }
    ei_camera_frame_buffer = (uint8_t *)ALIGN_PTR((uintptr_t)ei_camera_frame_mem, 32);
#endif

    fb.setBuffer(ei_camera_frame_buffer);
    is_initialised = true;

    return true;
}

/**
 * @brief      Stop streaming of sensor data.
 */
void ei_camera_deinit(void) {

#if defined(EI_CAMERA_FRAME_BUFFER_HEAP)
    ei_free(ei_camera_frame_mem);
    ei_camera_frame_mem    = NULL;
    ei_camera_frame_buffer = NULL;
#endif

    is_initialised = false;
}

/**
 * @brief      Capture, rescale and crop image.
 *
 * @param[in]  img_width     Width  of output image.
 * @param[in]  img_height    Height of output image.
 * @param[in]  out_buf       Pointer to store output image, NULL may be used.
 *                           If "ei_camera_frame_buffer" is to be used for capture and resize/cropping.
 *
 * @retval     False if not initialised, image captured, rescaled or cropped failed.
 *
 */
bool ei_camera_capture(uint32_t  img_width,
                       uint32_t  img_height, 
                       uint8_t  *out_buf)
{
    bool do_resize = false;
    bool do_crop   = false;

    if (!is_initialised)
    {
        ei_printf("ERR: Camera is not initialized\r\n");
        return false;
    }

    int snapshot_response = cam.grabFrame(fb, 3000);
    if (snapshot_response != 0)
    {
        ei_printf("ERROR: Failed to get snapshot (%d)\r\n", snapshot_response);
        return false;
    }

    uint32_t resize_col_sz;
    uint32_t resize_row_sz;
    // Choose Resize Dimensions.
    int res = calculate_resize_dimensions(img_width,
                                          img_height,
                                         &resize_col_sz,
                                         &resize_row_sz,
                                         &do_resize);
    if (res)
    {
        ei_printf("ERROR: Failed to calculate resize dimensions (%d)\r\n", res);
        return false;
    }

    if ( (  img_width  != resize_col_sz)   ||
           (img_height != resize_row_sz) )
    {
        do_crop = true;
    }

    // The following variables should always be assigned
    // if this routine is to return true cutout values.
    ei_camera_capture_out = ei_camera_frame_buffer;

    if (do_resize)
    {

        // If only resizing then and out_buf provided then use it instead.
        if (out_buf && !do_crop) ei_camera_capture_out = out_buf;

        //ei_printf("resize cols: %d, rows: %d\r\n", resize_col_sz,resize_row_sz);
        ei::image::processing::resize_image( ei_camera_frame_buffer,
                                             EI_CAMERA_RAW_FRAME_BUFFER_COLS,
                                             EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
                                             ei_camera_capture_out,
                                             resize_col_sz,
                                             resize_row_sz,
                                             1); // bytes per pixel
    }

    if (do_crop)
    {
        uint32_t crop_col_sz;
        uint32_t crop_row_sz;
        uint32_t crop_col_start;
        uint32_t crop_row_start;
        crop_row_start = (resize_row_sz - img_height) / 2;
        crop_col_start = (resize_col_sz - img_width)  / 2;
        crop_col_sz    = img_width;
        crop_row_sz    = img_height;

        // If (also) cropping and out_buf provided then use it instead.
        if (out_buf) ei_camera_capture_out = out_buf;

        //ei_printf("crop cols: %d, rows: %d\r\n", crop_col_sz,crop_row_sz);
        ei::image::processing::cropImage( ei_camera_frame_buffer,
                                          resize_col_sz,
                                          resize_row_sz,
                                          crop_col_start,
                                          crop_row_start,
                                          ei_camera_capture_out,
                                          crop_col_sz,
                                          crop_row_sz,
                                          8); // bits per pixel
    }

    return true;
}

/**
 * @brief      Convert monochrome data to RGB values.
 *
 * @param[in]  mono_data  The mono data.
 * @param      r          Red   pixel value.
 * @param      g          Green pixel value.
 * @param      b          Blue  pixel value.
 */
static inline void mono_to_rgb(uint8_t mono_data, uint8_t *r, uint8_t *g, uint8_t *b)
{
    uint8_t v = mono_data;
    *r = *g = *b = v;
}

int ei_camera_cutout_get_data(size_t offset, size_t length, float *out_ptr)
{
    size_t bytes_left = length;
    size_t out_ptr_ix = 0;

    // read byte for byte
    while (bytes_left != 0)
    {
        // Grab the value and convert to r/g/b.
        uint8_t pixel = ei_camera_capture_out[offset];

        uint8_t             r,  g,  b;
        mono_to_rgb(pixel, &r, &g, &b);

        // Then convert to "out_ptr" format.
        float pixel_f = (r << 16) +
                        (g <<  8) +
                         b;
        out_ptr[out_ptr_ix] = pixel_f;

        // And go to the next pixel...
        out_ptr_ix++;
        offset++;
        bytes_left--;
    }

    // ...and done!
    return 0;
}

/**
 * @brief      Determine whether to resize and to which dimension.
 *
 * @param[in]  out_width     Width  of output image.
 * @param[in]  out_height    Height of output image.
 * @param[out] resize_col_sz Pointer to frame buffer's column/width value.
 * @param[out] resize_row_sz Pointer to frame buffer's rows/height  value.
 * @param[out] do_resize     Returns whether to resize (or not).
 *
 */
int calculate_resize_dimensions(uint32_t  out_width,
                                uint32_t  out_height,
                                uint32_t *resize_col_sz,
                                uint32_t *resize_row_sz,
                                bool     *do_resize)
{
    const size_t list_size = 6;
    const ei_device_resize_resolutions_t list[list_size] = {
        {128, 96},
        {160, 120},
        {200, 150},
        {256, 192},
        {320, 240},
    };

    // (default) Conditions
    *resize_col_sz = EI_CAMERA_RAW_FRAME_BUFFER_COLS;
    *resize_row_sz = EI_CAMERA_RAW_FRAME_BUFFER_ROWS;
    *do_resize     = false;

    for (size_t ix = 0; ix < list_size; ix++)
    {
        if ( (out_width  <= list[ix].width)   && 
             (out_height <= list[ix].height) )
        {
            *resize_col_sz = list[ix].width;
            *resize_row_sz = list[ix].height;
            *do_resize = true;
            break;
        }
    }

    return 0;
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_CAMERA
#error "Invalid model for current sensor"
#endif
