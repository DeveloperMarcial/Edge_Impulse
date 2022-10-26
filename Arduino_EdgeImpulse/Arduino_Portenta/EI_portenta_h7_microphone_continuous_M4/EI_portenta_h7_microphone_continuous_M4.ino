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
 * and saved herin as the Software "EI_portenta_h7_microphone_continuous_M4.ino"    *
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

#ifndef CORE_CM4
#error "This sketch should be compiled for Portenta (M4 core)"
#endif

/* Portenta H7 Memory Info ------------------------------------------------- */
// When programming M7 core maximum is   786,432 bytes.

// When programming M4 core maximum is 1,048,576 bytes.
// Sketch uses  282,592 bytes (26%) of program storage space.
// Maximum is 1,048,576 bytes.

// If your target is limited in memory remove this macro to save 10K RAM.
#define EIDSP_QUANTIZE_FILTERBANK   0

/* INFO: Edge Impulse Audio Machine Learning Model Info -------------------- */
/*
 * Define the number of slices per model window, e.g. a
 *               model window = 1000 ms
 *    slices per model window =    4
 *             ==> slice size =  250 ms
 * For more info: https://docs.edgeimpulse.com/docs/continuous-audio-sampling
 *
 * On each "slice" now, inference is run multiple times (depending on the number of slices used for a model).
 * For example, a 1 second keyword model with 4 slices (each 250 ms), will inference each slice 4 times.
 * So if now the keyword is on 2 edges of the slice buffers,
 * they're glued back together in the FIFO buffer and the keyword will be classified correctly.
 * This is also known as "grid registration error" or GRE.
 *
 * A moving average filter averages the classified() output and so flattens the peaks.
 * See the moving average filter in "ei_run_classifier.h".
 *
 * The EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW macro is used to set the number of slices to fill the complete model window.
 * The more slices per model, the smaller the slice size, thereby the more inference cycles on the sampled data.
 * Leading to more accurate results. The sampling process uses this macro for the buffer size.
 * Where following rule applies: the bigger the buffer, the longer the sampling cycle.
 * So on targets with lower processing capabilities, we can increase this macro to meet the timing constraint.
 * Increasing the slice size, increases the volatile memory uses times 2 (since we use double buffering).
 * On a target with limited volatile memory this could be a problem. In this case you want the slice size to be small.
 *
 * Summary:
 *                              [Slice Size] ∝ [RAM Size] x 2
 */
//#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 3

/* INFO: TFLite Arena Allocation Issue ------------------------------------- */
/*
 ** NOTE: If you run into TFLite arena allocation issue.
 **
 ** This may be due to may dynamic memory fragmentation.
 ** Try defining "-DEI_CLASSIFIER_ALLOCATION_STATIC" in boards.local.txt 
 ** (create if it doesn't exist) and copy this file to
 ** <ARDUINO_CORE_INSTALL_PATH>/arduino/hardware/<mbed_core>/<core_version>/.
 **
 ** See
 ** (https://support.arduino.cc/hc/en-us/articles/360012076960-Where-are-the-installed-cores-located-)
 ** to find where Arduino installs cores on your machine.
 **
 ** If the problem persists then there's not enough memory for this model and application.
 */

/* Includes ---------------------------------------------------------------- */
#include "RPC.h"                                                   // Comes with the mbed board installation.
                                                                   //    Used for cross-comminication between M7 & M4 cores. 

// The Portenta H7 has two omnidirectional digital-microphones (MP34DB02) that allow concurrent capture of stereo sounds.
// The Portenta Vision Shield has:
//                                  2x MP34DT06JTR MEMS PDM Digital Microphone
//                                  AOP = 122.5 dBSPL
//                                  64 dB signal-to-noise ratio
//                                  Omnidirectional sensitivity
//                                  –26 dBFS ± 1 dB sensitivity
//
// A description of PDM is here: http://st-videos.s3.amazonaws.com/STM32_Microphone_Audio_Acquisition_Part2_PDM_Microphone.mp4
#include <PDM.h>                                                   // The PDM library allows you to use pulse-density modulation microphones.
                                                                   //    The library takes care of the audio that will be accessible
                                                                   //    also through the ArduinoSound library.
                                                                   //    https://docs.arduino.cc/learn/built-in-libraries/pdm 

#include <Crimson_Cherry_inferencing.h>                            // My Edge Impulse keyword inferencing library.
//#include "C:\Users\mmarc\OneDrive\Documents\Arduino\libraries\Crimson_Cherry_inferencing\src\Crimson_Cherry_inferencing.h"

using namespace rtos;                                              // Since the Portenta cores are based on the Mbed real-time OS,
                                                                   //    this allows access to the many features of the RTOS and its functions in your Arduino sketches
                                                                   //    such as for a multi-threaded Arduino sketch.
                                                                   //    Use the RTOS namespace to create, synchronize, and manage threads. 

Thread blinkR_Thread;                                              // Define a thread that will control the on-board LED.

/* Audio Buffers, Pointers and Selectors
   used by Edge Inpulse Audio Inferencer ----------------------------------- */
typedef struct
{
    signed short       *buffers[2];
    unsigned char       buf_select;
    unsigned char       buf_ready;
    unsigned int        buf_count;
    unsigned int        n_samples;
} inference_t;

static inference_t      inference;
       bool             continiousMic             = false;
       bool             showSummaryOfInferencing  = true;
static bool             debug_nn                  = false;         // Set this to true to see e.g. features generated from the raw signal.
static volatile bool    record_ready              = false;
static signed short     sampleBuffer[2048];                        // Alternate declaration: "*sampleBuffer;""
static int              print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);
static float            keywordConfidenceLevel    = 0.5;           // A value from 0.0 to 1.0 indicating a correct keyword detection.

/**
 * @brief      Blink the Portenta Red, Green, Blue Light Emitting Diode.
 *             This routine works in unison with a routine on the M7 called BinkRGB()
 *             so that the on-board LED blink Red--Green--Purple.
 *
 *             This function is spawned once from setup() via:
 *             blinkR_Thread.start(BlinkR);
 */
void BlinkR()
{
  while(true)
  {
    // M7 core is blinking red right now.
    delay(500);
    delay(500);

    // M7 core is blinking green right now.
    delay(500);
    delay(500);

    // M7 core is blinking blue right now.
    // By flashing the red the LED will look purple or violet, which is a secondary color.
    // *** Slow Purple ***
    digitalWrite(LEDR, LOW);                                       // Turn the red LED on (LOW is the voltage level).
    delay(500);                                                    // Wait for 500 milliseconds.
    digitalWrite(LEDR, HIGH);                                      // Turn the LED off by setting the voltage HIGH.
    delay(500);                                                    // Wait for 500 milliseconds.

    // *** Fast Purple ***
    // digitalWrite(LEDR, LOW);                                    // Turn the red LED on (LOW is the voltage level).
    // delay(125);                                                 // Wait for 125 milliseconds.
    // digitalWrite(LEDR, HIGH);                                   // Turn the LED off by setting the voltage HIGH.
    // delay(125);                                                 // Wait for 125 milliseconds.
    // digitalWrite(LEDR, LOW);                                    // Turn the red LED on (LOW is the voltage level).
    // delay(125);                                                 // Wait for 125 milliseconds.
    // digitalWrite(LEDR, HIGH);                                   // Turn the LED off by setting the voltage HIGH.
    // delay(125);                                                 // Wait for 125 milliseconds.
    // digitalWrite(LEDR, LOW);                                    // Turn the red LED on (LOW is the voltage level).
    // delay(125);                                                 // Wait for 125 milliseconds.
    // digitalWrite(LEDR, HIGH);                                   // Turn the LED off by setting the voltage HIGH.
    // delay(125);                                                 // Wait for 125 milliseconds.
    // digitalWrite(LEDR, LOW);                                    // Turn the red LED on (LOW is the voltage level).
    // delay(125);                                                 // Wait for 125 milliseconds.
    // digitalWrite(LEDR, HIGH);                                   // Turn the LED off by setting the voltage HIGH.
    // delay(125);                                                 // Wait for 125 milliseconds.
  }
}

/**
 * @brief      Summary of Inferencing Settings (from model_metadata.h)
 *
 */
void show_summary_of_inferencing()
{
  RPC.println("Inferencing settings:\n");                       

  //             Interval:                              0.06 ms.
  RPC.println("\tInterval:                              " + String(EI_CLASSIFIER_INTERVAL_MS) + " ms.");

  //             Frame size:                            16000 <-- Model Window Size.
  RPC.println("\tFrame size:                            " + String(EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE));

  //             Sample length:                         1000 ms. 
  RPC.println("\tSample length:                         " + String(EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16) + " ms.");
  
  //             EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW: 4
  RPC.println("\tEI_CLASSIFIER_SLICES_PER_MODEL_WINDOW: " + String(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW));
  
  //             EI_CLASSIFIER_SLICE_SIZE:              4000
  RPC.println("\tEI_CLASSIFIER_SLICE_SIZE:              " + String(EI_CLASSIFIER_SLICE_SIZE));
  
  //             No. of classes:                        3
  RPC.println("\tNo. of classes:                        " + String( ( sizeof(ei_classifier_inferencing_categories) /
                                                                      sizeof(ei_classifier_inferencing_categories[0]))));
}

bool detectedKeypharse = false; // Used to control when we print to serial output.
bool pauseM4           = false; // Allow M7 to pause the M4 loop() so we can make the serial output less busy.
int  m4IntGlobalVar1   = 4444;  // This is a global variable in M4 that the M7 can change via setOneM4Var().

/**
 * @brief      When M7 does RPC with M4 it is bounded to land here.
 *             Allows M7 to set a M4 global variable.
 *             Toggles pausing the M4 loop().
 *             setup() has "RPC.bind("setOneVarInM4", setOneM4Var);"
 */
int setOneM4Var(int var1_FromM7)
{
    m4IntGlobalVar1 = (int)var1_FromM7;                            // Save M7 value into an M4 global variable.

    pauseM4 = !pauseM4;                                            // Toggle pausing of the M4 loop().

    String toM7 = "M4 says: m4IntGlobalVar1=" + String(m4IntGlobalVar1); 
    RPC.println(toM7);                                             // Show what M7 value got stored in M4.
                                                                   // TODO: Create a wrapper function so we can use a statement here that looks like:
                                                                   //       RPC_println("My integer = %i", myInt);

    return m4IntGlobalVar1;                                        // Return what M7 value got stored in M4.
                                                                   // Hopefully, this is the same as what got sent in.
}
/**
 * @brief      Allow M7 to show M4 version
 *             M4 setup() has     "RPC.bind("getM4Version", printM4Version);"
 *             M7 has a statement "RPC.call("getM4Version", 0xACE);"
 */
void printM4Version(int varFromM4)
{
  static const String toM7 = String(__FILE__)  + 
                             String(" ")       +
                             String(__DATE__)  +
                             String(" ")       +
                             String(__TIME__)  +
                             String("  IDE  ") +
                             String(ARDUINO);
  RPC.println(toM7);
}
/**
 * @brief      Show audio inferencing configuration.
 *             M4 setup() has     "RPC.bind("getSummaryOfInferencingOnM4", showSummaryOfInferencingOnM4);"
 *             M7 has a statement "RPC.call("getSummaryOfInferencingOnM4", 0xACE);"
 */
void showSummaryOfInferencingOnM4(int varFromM4)
{
  show_summary_of_inferencing();
}

/**
 * @brief      Arduino Setup Function
 */
void setup()
{
    RPC.begin();                                                   // Allow remote procedure calls with M7.

    // Allows M7 to set a M4 global variable and
    // toggles pausing the M4 loop().
    RPC.bind("setOneVarInM4", setOneM4Var);                        // These arguments do not have to be the same.
                                                                   // M7 calls "setOneVarInM4()".
                                                                   // M4 has a routine called "setOneM4Var()".
                                                                   // Thus, bind() is a bounden between the name and function.    
    // Show M4 version.
    RPC.bind("getM4Version", printM4Version);

    // Show Audio Inferencing Configuration.
    RPC.bind("getSummaryOfInferencingOnM4", showSummaryOfInferencingOnM4);

    // Init Static Vars used by Edge Impulse Machine Learning Model.
    run_classifier_init();                    

    // Start the Audio Inferencer.
    if (microphone_inference_start(EI_CLASSIFIER_SLICE_SIZE) == false)
    {
        RPC.println("ERROR: Failed to setup audio sampling.");
        return;
    }
    
    // Initialize digital pin LED_BUILTIN as an output.
    pinMode(LEDR, OUTPUT);        
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);

    // The LED will be manipulated via a separate running thread spawned by this statement:
    blinkR_Thread.start(BlinkR);
}

/**
 * @brief      Arduino main function. Runs the inferencing loop.
 */
void loop()
{
    String toM7 = "";                                              // Temporary building string for messages going to M7.

    if(showSummaryOfInferencing)
    {
      showSummaryOfInferencing = false;                            // Only show info once.
      show_summary_of_inferencing();                               // TODO: Investigate why this crashes the M4 when placed in setup(). 
    }

    while(pauseM4)
    {
    //BlinkR(); <-- This is being handled on a thread.             // Wait until we get an RPC call in, meanwhile make the LED flash Red--Green--Purple.
                                                                   // This purple color is due to the M7 and M4 cores working together.
      delay(0xBAD);                                                // Wait about 3 seconds until we get an RPC call in.       
    }
    
    bool m = microphone_inference_record();                        // Go wait until audio inference buffer is ready.
    if (!m)
    {
        RPC.println("ERROR: Failed to record audio...");
        return;
    }
    // ...Inference buffer is ready so classify it.

    // Setup "signal": sets the callback function on the "signal_t" structure to reference the audio inference buffer:
    signal_t signal_mic;
    signal_mic.total_length    = EI_CLASSIFIER_SLICE_SIZE;
    signal_mic.get_data        = &microphone_audio_signal_get_data;// This tells the "signal_mic" where to get the sampled data from.

    ei_impulse_result_t result = { 0 };                            // Used by "run_classifier_continuous()"".

    // Classify it: The "run_classifier_continuous()" is called which will
    //    take the slice of data,
    //    run the DSP pipeline over the data,
    //    stitch data together, and then 
    //    classify the data.
    // See https://github.com/edgeimpulse/inferencing-sdk-cpp/blob/04579b9b1784db858c3b444e38ca4bd89c65c925/classifier/ei_run_classifier.h#L159
    EI_IMPULSE_ERROR r = run_classifier_continuous(&signal_mic, &result, debug_nn);
    if (r != EI_IMPULSE_OK)
    {
        toM7 = "ERROR: Failed to run classifier: " + String(r);
        RPC.println(toM7);
        return;
    }
    
    if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW))
    {         
        bool res = RPC.call("setMicInferenceComplete", false).as<bool>();     // Force M7 to wait for all M4 Classifier() results to be
                                                                              // x-mitted before M7 leaves its RPC.available() while-loop.
                                                                              // This makes for a appealing and engaging serial output.
        RPC.println("--- M4 --- --- M4 --- --- M4 --- --- M4 --- --- M4 ---");
        // Print the Predictions: P-the-P.
        String s1 = String(result.timing.dsp);
        String s2 = String(result.timing.classification);
        String s3 = String(result.timing.anomaly);
        s1.trim();
        s2.trim();                                                 // Was getting a bunch of extra spaces when converting
        s3.trim();                                                 // "result.timing.classification" to a string. But trim() is not helping.
        toM7 = String("Audio predictions from microphone:\n\t(DSP [ms]=\t") + s1 +               
               String("\tAnomaly [ms]=\t")                                  + s3 +
               String("\tClassification [ms]=\t")                           + s2;
        RPC.println(toM7);

        //
        // Print Objects that Got Classified
        //
        // (Mainly we want to examine inference label: "take_it_to_the_edge")
        detectedKeypharse = false;                            // Used to control when we print to serial output.
        for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
        {
            //
            // Keep serial output pleasing like a table.
            //
            String alignmentTab = "";                              
            if("noise"==result.classification[ix].label)
            {
                alignmentTab =":\t\t\t";
            }
            else if("unknown"==result.classification[ix].label)
            {
              alignmentTab =":\t\t";
            }
            else
            {
              alignmentTab =":\t";
            }
            toM7 = String(result.classification[ix].label) + alignmentTab + String(result.classification[ix].value);
            RPC.println(toM7);                                     // Print a row of the table.
            
            // Did we find the keyphrase?
            if ("take_it_to_the_edge" == result.classification[ix].label)
            {
              // Is there high confidence that this keyphrase is what we are looking for.
              ///toM7 = String("\t\t\tComparing ") + String(result.classification[ix].value) + alignmentTab + String(keywordConfidenceLevel);
              ///RPC.println(toM7);                               // Make sure we are comparing float values.
              if(result.classification[ix].value > keywordConfidenceLevel)
              {
                ///toM7 = String("\t\t\tdetectedKeypharse=true");
                ///RPC.println(toM7);                              // Debug status update.
                detectedKeypharse = true;                          // We will print after this for-loop so output remains consistent.
                                                                   // (Provides a future parser of the serial output less work.)
              }
            }
        }

        // If needed notify M7 that we detected the keyphrase.
        // Send M7 a value as well as some text to be printed to the serial output.
        if (detectedKeypharse)
        {
          detectedKeypharse = false;
          int keywordSuccessValue = 42;                            // Send a fixed  value.
        //int keywordSuccessValue = rand() % 100;                  // Send a random value.   

          // Send value to M7 indicating the successful results.
          auto res = RPC.call("setOneVar", keywordSuccessValue).as<int>();     

          // Send text to M7 indicating the successful results to be printed to serial output.
          toM7 = "\t\t\t!!! M4 is sending M7 a value                   = " + String(keywordSuccessValue) + "!!!\n" + 
                 "\t\t\t!!! Recognized take_it_to_the_edge Keyphrase !!!";
          RPC.println(toM7);                                       // Have M7 print the successful results.
        }
        
        RPC.println("--- End Audo Inference Running on M4 ---");

        res = RPC.call("setMicInferenceComplete", true).as<bool>();// All Classifier() status updates have been x-mitted to M7.
                                                                   // Allow M7 to leave its RPC.available() while-loop.
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        toM7 = "    anomaly score: " + String(result.anomaly);
        RPC.println(toM7);
#endif
        print_results = 0;
    }
}

/**
 * @brief     Callback function to process the data from the PDM microphone.
 *              NOTE: This callback is executed as part of an ISR.
 *              Therefore using `Serial` to print messages inside this function isn't supported.
 *             Pulse-Density Modulation (PDM) buffer full callback.
 *                The callback is configured in microphone_inference_start() that is called during setup().
 *                A callback is just a function that you tell some other function to call under some condition.
 *             Copy sampled audio data to inferecne app buffers.
 *             The audio sampling process calls this function when there is audio data.
 *             The PDM library allows you to use Pulse-Density Modulation microphones.
 *             See #include above for more comments.
 */
static void pdm_data_ready_inference_callback(void)
{
    // Get the number of bytes available for reading from the PDM interface.
    // This is data that has already arrived and was stored in the PDM receive buffer.
    int bytesAvailable = PDM.available();

    // Read into the Sample Buffer.
    int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

    // 16-bit, 2 bytes per sample
    //int samplesRead = bytesAvailable / 2;

    if ((inference.buf_ready == 0) && (record_ready == true))
    {
        for(int i = 0; i < bytesRead>>1; i++)
        {
            inference.buffers[inference.buf_select][inference.buf_count++] = sampleBuffer[i];

            if (inference.buf_count >= inference.n_samples)
            {
                inference.buf_select ^= 1;
                inference.buf_count   = 0;
                inference.buf_ready   = 1;                         // The inference process is signaled by setting the flag "inference.buf_ready".
                break;
            }
        }
    }
}

/**
 * @brief      Init inferencing struct and setup/start PDM.
 *             Called from setup().
 *             Initializes 2 inferencing buffers.
 *
 * @param[in]  n_samples  The n samples <-- Nice descript!
 *                        n_samples is the slice size being used by the Classifier().
 *                        n_samples = EI_CLASSIFIER_SLICE_SIZE.
 *
 * @return     { description_of_the_return_value }
 */
static bool microphone_inference_start(uint32_t n_samples)
{
    // Remember "inference" is declared as "static inference_t inference" and has 2 buffers[].
        inference.buffers[0] = (signed short *)malloc(n_samples * sizeof(signed short));
    if (inference.buffers[0] == NULL)
    {
        return false;
    }

        inference.buffers[1] = (signed short *)malloc(n_samples * sizeof(signed short));
    if (inference.buffers[1] == NULL)
    {
        ei_free(inference.buffers[0]);
        return false;
    }

    //     sampleBuffer = (signed short *)malloc((n_samples >> 1) * sizeof(signed short));
    // if (sampleBuffer == NULL) {
    //     ei_free(inference.buffers[0]);
    //     ei_free(inference.buffers[1]);
    //     return false;
    // }

    inference.buf_select = 0;
    inference.buf_count  = 0;
    inference.n_samples  = n_samples;
    inference.buf_ready  = 0;

    // Configure the Data Receive Callback.
    // This sets the callback function that is called when new PDM data is ready to be read.
    PDM.onReceive(&pdm_data_ready_inference_callback);

    // Optionally set the gain, Portenta defaults to 24.
    // Note: values >=52 not supported. <-- There is conflicting data on the web. TODO: Experiment with values.
    //PDM.setGain(40);

    // Set the buffer size (in bytes) used by the PDM interface.
    // Must be called before PDM.begin(), a default buffer size of 512 is used if not called.
    // Buffer[512] is enough to hold 256 16-bit samples.
    PDM.setBufferSize(2048);                                       // Buffer[2048] is enough to hold 1024 16-bit samples.

    // Initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate for the Arduino Nano 33 BLE Sense
    // - a 32 kHz or 64 kHz sample rate for the Arduino Portenta Vision Shield
    if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY))
    {
        RPC.println("ERROR: Failed to start PDM!");
        return false;
    }

    record_ready = true;

    return true;
}

/**
 * @brief      Wait on new data
 *
 * @return     True when finished
 */
static bool microphone_inference_record(void)
{
    bool ret = true;

    if (inference.buf_ready == 1)
    {
        // There are 2 constraints in this story: timing and memory.
        // When switching the buffers there must be a 100% guarantee that the inference process is finished when the sampling process passes a full buffer.
        // If not, the sampling process overruns the buffer and sampled data will get lost.
        // When that happens on the microcontroller running the impulse is aborted and the following error is returned:
        RPC.println("Error sample buffer overrun. Decrease the number of slices per model window (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)");
        ret = false;
    }

    while (inference.buf_ready == 0)
    {
        delay(1);
    }

    inference.buf_ready = 0;

    return ret;
}

 /**
 * @brief      Get raw audio signal data.
 *             This function tells the "signal" where to get the data from.
 *
 * @return     0
 */
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr, length);

    return 0;
}

/**
 * @brief      Stop PDM and release buffers
 */
static void microphone_inference_end(void)
{
    PDM.end();                                                     // De-initialize the PDM interface.
    ei_free(inference.buffers[0]);
    ei_free(inference.buffers[1]);
  //ei_free(sampleBuffer);
    record_ready = false;
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_MICROPHONE
#error "Invalid model for current sensor."
#endif
