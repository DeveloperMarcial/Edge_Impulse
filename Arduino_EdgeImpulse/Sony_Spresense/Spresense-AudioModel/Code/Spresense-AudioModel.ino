/***
 ************************************************************************************
 * The MIT License (MIT)                                                            *
 *                                                                                  *
 * Modifications to the Software "firmware-sony-spresense"                          *
 * originally found at "https://github.com/edgeimpulse/"                            *
 * and saved herin as the Software "Spresense-AudioModel.ino"                       *
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
 ***/

//All code is the same between Non-Continious mode and Continious mode,
//except for "setup()" and "loop()". Select only one of the 2 following #defines.
//
#define NON_CONTINIOUS  // Enable non-continious inferencing code.                 
// or
//#define CONTINIOUS        // Enable     continious inferencing code.
                          //
                          // <--Continious mode is not working.
                          //    NOTE: Continious mode also does not work at: 
                          //          https://github.com/edgeimpulse/firmware-sony-spresense
                          //
                          //    It will inferecne 8 times and then get stuck.
                          //    NOW! This is interesting since printResult is initialized as:
                          //    printResult = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW) = -4
                          //
                          //    ei_microphone_inference_record() calls get_dsp_data().
                          //    get_dsp_data() calls spresense_getAudio().
                          //    spresense_getAudio() returns zero bytes for ever & ever!!!

#ifdef NON_CONTINIOUS
#ifdef CONTINIOUS
#error NON_CONTINIOUS mode and CONTINIOUS mode cannot be enabled at the same time.
#endif
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      MEMORY INFO                                                                               //
//                      The program requires a minimum MainCore memory of 896 kB                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
//------------------------------------------------------------------------------------------------------------------
  Edge Impulse Studio float32 Library Estimates: RAM 70k, Flash 236
//------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------
  Target Memory Size = 1536 kB
  Won't run.
    ERROR: up_assert: Assertion failed at file:manager/audio_manager.cpp line: 534 task: init
--------------------------------------------------------------------------------------------------------------------
  Target Memory Size = 1280 kB
  Won't run.
    mptask_exec() failure. -12
    Attention: module[13][0] attention id[2]/code[18] (components/filter/src_filter_component.cpp L120)
    Attention!! Level 0x2 Code 0x12
    ERROR: Command(0x31) fails. Result code(0xf1) Module id(4) Error code(0x7) subcode(0x0)

  Sketch and global variables uses 602,664 bytes (46%) of program storage space.
  Leaving                          708,056 bytes for local variables.
  Maximum is                     1,310,720 bytes.
--------------------------------------------------------------------------------------------------------------------
  Target Memory Size = 1152 kB
  Program works!

  Sketch and global variables uses 602,664 bytes (51%) of program storage space.
  Leaving                          576,984 bytes for local variables.
  Maximum is                     1,179,648 bytes.
--------------------------------------------------------------------------------------------------------------------
  Target Memory Size = 1024 kB
  Program works!

  Sketch and global variables uses 602,664 bytes (51%) of program storage space.
  Leaving                          445,912 bytes for local variables.
  Maximum is                     1,048,576 bytes.
--------------------------------------------------------------------------------------------------------------------
  Target Memory Size =  896 kB
  Program works!

  Sketch and global variables uses 602,664 bytes (51%) of program storage space.
  Leaving                          314,840 bytes for local variables.
  Maximum is                       917,504 bytes.
--------------------------------------------------------------------------------------------------------------------
  Target Memory Size =  768 kB
  ERROR: FIFO area is not allocated.
--------------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      PROGRAMMING INFO                                                                          //
//                      If Arduino fails to program the Spresense, execute the following command line.            //
//                      Change "<your_path>" depending on your system configuration                               //
//                      Change "/dev/ttyUSB0" to serial port the Spresense connected to.                          //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------------------------
  Flash from Ubuntu:
      "<your_path>/.arduino15/packages/SPRESENSE/tools/spresense-tools/2.6.0/flash_writer/linux/flash_writer"
      -s -c
      "/dev/ttyUSB0"
      -d -n
      "/tmp/arduino-sketch-<a_randon_32-bit_hex_value>/Spresese-AudioModel.ino.spk"
  
  Flash from Windows:
    "C:\Users\<username>\AppData\Local\Arduino15\packages\SPRESENSE\tools\spresense-tools\2.6.0/flash_writer/windows/flash_writer.exe"
    -s -c
    "COM11"
    -d -n
    "C:\Users\<username>\AppData\Local\Temp\arduino-sketch-<a_randon_32-bit_hex_value>/Spresense-AudioModel.ino.spk"
//------------------------------------------------------------------------------------------------------------------
 */

/* Includes ----------------------------------------------------------------------------------------------------- */
// The following are not supported on a SubCore (there may be others):
//                                              Audio.h,
//                                              Camera.h,
//                                              File.h,
//                                              MemoryUtil.h
#ifdef SUBCORE
#error "Core selection is wrong!! Must compile to MainCore!!!"
#endif

// An SD card is required!
// The SD card holds the DSP files used for decoding if use audio playback or encoding if you use audio recorder.
#include <SDHCI.h>                          // Secure Digital Host Controller Interface driver for using the SD card.
                                            // The SDHCI library has an API structure similar to the Arduino SD library,
                                            // and it can be easily ported from the existing sketch using the SD library.
                                            // SD and SDHC cards are supported, but SDXC cards is not supported.
                                            // Formats of FAT12, FAT16 and FAT32 are supported, exFAT is not supported.
                                            // Capacity of up to 32 GB (I verified) is supported. <--Sony docs say 16 GB.

#include <Audio.h>                          // Sony library! C:\Users\<userName>\AppData\Local\Arduino15\packages\SPRESENSE\hardware\spresense\2.6.0\libraries\Audio.
#include <a6_Models_-_Yes_No_inferencing.h> // <--Reference your Edge Impulse Studio Arduino library export "Audio float32 Model".
//#include <edge-impulse-sdk/classifier/ei_run_classifier.h>

/* Audio Sampling Config */
#define AUDIO_SAMPLING_FREQUENCY            16000
#define AUDIO_SAMPLES_PER_MS                (AUDIO_SAMPLING_FREQUENCY / 1000)
#define AUDIO_DSP_SAMPLE_LENGTH_MS          16
#define AUDIO_DSP_SAMPLE_RESOLUTION         (sizeof(short))
// RE: AUDIO_DSP_SAMPLE_BUFFER_SIZE
//    There is a note is some EI code that I think is wrong:
//    AUDIO_DSP_SAMPLE_BUFFER_SIZE = AUDIO_SAMPLES_PER_MS * AUDIO_DSP_SAMPLE_LENGTH_MS * AUDIO_DSP_SAMPLE_RESOLUTION
//                                   16000/1000 = 16      * 16                         * 2 = 512
//    AUDIO_DSP_SAMPLE_BUFFER_SIZE = 512
//    ...and in other places it is simpily set to 1600 that is close to 768*2 (=1536)
//    (times 2 to double buffer data)
//    More info about "768" is here: https://developer.sony.com/develop/spresense/docs/arduino_developer_guide_en.html#_important_point_of_various_operations 
//    That said, the program works if
//    AUDIO_DSP_SAMPLE_BUFFER_SIZE = 512 and the sketch is reduced by 2048 bytes. Leaving 2048 more bytes for local variables.
#define AUDIO_DSP_SAMPLE_BUFFER_SIZE        512

/** INFO: Edge Impulse Audio Machine Learning Model Info -------------------------------------------------------- */
/*
 * In regards to continuous audio sampling:
 *    See https://docs.edgeimpulse.com/docs/tutorials/continuous-audio-sampling
 *    When you are classifying audio - for example to detect keywords - you want to make sure that every piece of information is both captured and analyzed,
 *    to avoid missing events. This means that your device need to capture audio samples and analyze them at the same time. 
 *    
 *    Prerequisites
 *    Ideally the embedded target needs to support running of multiple processes in parallel.
 *    This can either be achieved by an operating system;
 *        1 low  priority thread will run inferencing and
 *        1 high priority thread will collect sample data.
 *    Or the processor should support processor offloading.
 *        This is usually done by the
 *           audio peripheral or
 *           DMA (Direct Memory Access) wherein audio samples are collected in a buffer without involvement of the processor.
 *        
 *    Double buffering
 *        How do we know when new sample data is available? For this we use a double buffering mechanism.
 *        Hereby 2 sample buffers are used:
 *           1 buffer for the audio sampling process, filling the buffer with new sample data
 *           1 buffer for the inference process, get sample data out the buffer, extract the features and run inference
 *        At start, the sampling process starts filling a buffer with audio samples.
 *        Meanwhile, the inference process waits until the buffer is full.
 *        When that happens, the sampling process passes the buffer to the inference process and starts sampling on the second buffer.
 *        Each iteration, the buffers will be switched so that there is always an empty buffer for sampling and a full buffer of samples for inferencing.
 *    
 * When doing continious sampling the concept of a Slice comes in.
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
 * See https://docs.edgeimpulse.com/docs/tutorials/continuous-audio-sampling#1.-model-slices
 *    Using continuous inferencing, smaller sampling buffers (slices) are used and passed to the inferencing process.
 *    In the inferencing process the buffers are time sequentially placed in a FIFO (First In First Out) buffer that matches the model size.
 *    After each iteration, the oldest slice is removed at the end of the buffer and a new slice is inserted at the beginning.
 *    On each slice now, inference is run multiple times (depending on the number of slices used for a model).
 *    For example, a 1 second keyword model with 4 slices (each 250 ms), will inference each slice 4 times.
 *    So if now the keyword is on 2 edges of the slice buffers, they're glued back together in the FIFO buffer and the keyword will be classified correctly.
 *
 * See https://docs.edgeimpulse.com/docs/tutorials/continuous-audio-sampling#2.-averaging
 *    A moving average filter averages the classified() output and so flattens the peaks.
 *    See the moving average filter in "ei_run_classifier.h".
 *    
 *    Another advantage of this averageing technique is that it filters out false positives.
 *    Take for instance a yes-no keyword spotting model.
 *    The word 'yesterday' should not be classified as a yes (or no).
 *    But if the 'yes-' is sampled in the first buffer and '-terday' in the next,
 *    there is a big chance that the inference step will classify the first buffer as a yes.
 *    By running inference multiple times over the slices,
 *    continuous inferencing will filter out this false positive.
 *    When the 'yes' buffer enters the FIFO it will surely classify a 'yes'.
 *    But as the rest of the word enters, the classified value for 'yes' will drop quickly.
 *    We just have to make sure that we don't react on peak values.
 *    Therefore, a moving average filter averages the classified output and so flattens the peaks.
 *    To have a valid 'yes', we now need multiple high rated classifications.
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
 *                              [Slice Size] ∝ [RAM Size] x 2 <-- times 2 since we are double buffering.
 *          The macro here determines the slice length:
 *             #define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 2
 *          If you’re
 *             window size is 1000ms and
 *             CLASSIFIER_SLICES_PER_MODEL_WINDOW is 4,
 *             the slice length is 250ms. If it’s 3 it’s 333ms. etc.
 **/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      START GLOBALS                                                                             //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Audio Buffers, Pointers and Selectors
   used by Edge Inpulse Audio Inferencer ----------------------------------- */

typedef struct
{
    signed short      *buffers[2]; // This is a 2-dimensional array.
                                   // 1st dimension what buffer we are working with in a double-buffering manner: buffer1 or buffer2
                                   // 2nd dimension is how many samples we have read from the microphone.
    unsigned char     buf_select;  // Are we working with double-buffering buffer: buffer1 or buffer2
    unsigned char     buf_ready;   // The inference process is signaled by setting the flag "inference.buf_ready". 
    unsigned int      buf_count;   // buf_count grows until it equals n_samples = AUDIO_SAMPLING_FREQUENCY.
    unsigned int      n_samples;   // n_samples = AUDIO_SAMPLING_FREQUENCY
} inference_t;

static inference_t   inference;             // Global: Typical Usage: inference.buffers[inference.buf_select][inference.buf_count++] = sampleBuffer[i];
static volatile bool record_ready  = false; // Global: 
static          bool debugNN       = false; // Global: Set this to true to see e.g. features generated from the raw signal
                bool errEndIt      = false; // Global: Used to gracefully exit the loop() when an error ocvcurs.
                bool StateLED0     = false; // Global: 
                bool StateLED1     = false; // Global: 
                bool StateLED2     = false; // Global: 
                bool StateLED3     = false; // Global: 

/* Audio Variables */
AudioClass *theAudio;                                             // Global: Audio class.

static const int32_t bufferSize = AUDIO_DSP_SAMPLE_BUFFER_SIZE;   // Global: 768sample,1ch,16bit.
static       int16_t audioBuffer[ AUDIO_DSP_SAMPLE_BUFFER_SIZE];  // Global: Alternate declaration: "*audioBuffer;".

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      END GLOBALS                                                                               //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      AUDIO HELPER ROUTINES                                                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************************************
 * @brief   Audio Attention Callback
 * @details When audio internal error occurc, this function will be called back.
 *          Sony declares "Response Errors" and "Attention Errors"
 *          See https://developer.sony.com/develop/spresense/docs/arduino_developer_guide_en.html#_response_error
 *          See https://developer.sony.com/develop/spresense/docs/arduino_developer_guide_en.html#_attention_error 
 * @param   *atprm, structure to error number and other details like line #, sub_module_id, etc.
 * @return  void
 ********************************************************************************************************************/
static void audio_attention_cb(const ErrorAttentionParam *atprm)
{
  //ei_printf("[FLOW] Entering audio_attention_cb()...\n");

  ei_printf("Attention!");
  
  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
    {
      errEndIt = true;
   }
}

/*******************************************************************************************************************
 * @brief   Execute Frames for FIFO Empty
 * @details 
 * @param   void
 * @return  void
 ********************************************************************************************************************/
static void empty_audio_fifo(void)
{
  //ei_printf("[FLOW] Entering empty_audio_fifo()...\n");

  uint32_t read_size = 0;

    char *s_buffer;

    s_buffer = (char *)malloc(bufferSize);

    if(!s_buffer) {
      ei_printf("Memory allocation failed\r\n");
    }

    do
    {
        err_t err = theAudio->readFrames(s_buffer, bufferSize, &read_size);
        if ( (err != AUDIOLIB_ECODE_OK) &&
             (err != AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA))
        {
            break;
        }
    }
    while (read_size > 0);
    free(s_buffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      SPRESENSE AUDIO                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************************************
 * @brief   Create Audio Instance and Setup Audio Channel
 * @details Uses PCM format MONO @ 16KHz
 * @param   void
 * @return  Error code from calls to "setRecorderMode()" and "spresense_setupAudio()".
 ********************************************************************************************************************/
int spresense_setupAudio(void)
{
  //ei_printf("[FLOW] Entering spresense_setupAudio()...\n");
    
  int retVal = 0;

  // If not enuf memory, you'll get:
  //  Error -12.
  //    mpshm_init() failure.
  //    Memory pool initilization error.
  //    Audio activation error.
  theAudio = AudioClass::getInstance();

//theAudio->begin();                                                                          // Start without attention error handler.
  theAudio->begin(audio_attention_cb);                                                        // Start with    attention error handler.

  /* Select Input Device as Microphone */
  ei_printf("[DEBUG] AS_SETRECDR_STS_INPUTDEVICE_MIC=%d\n", AS_SETRECDR_STS_INPUTDEVICE_MIC); // AS_SETRECDR_STS_INPUTDEVICE_MIC=0
  ei_printf("[DEBUG] SIMPLE_FIFO_BUF_SIZE=%d\n", SIMPLE_FIFO_BUF_SIZE);
  // With #define WRITE_FIFO_FRAME_NUM  (4)
  //      #define READ_FIFO_FRAME_NUM   (4)
  // SIMPLE_FIFO_BUF_SIZE=  65,536
  // With #define WRITE_FIFO_FRAME_NUM  (8)
  //      #define READ_FIFO_FRAME_NUM   (10)
  // SIMPLE_FIFO_BUF_SIZE= 163,840
                                                                                              
  //Mic Inputs:
  //  AS_SETRECDR_STS_INPUTDEVICE_MIC
  //  AS_SETRECDR_STS_INPUTDEVICE_MIC_A
  //  AS_SETRECDR_STS_INPUTDEVICE_MIC_D
  //int32_t input_gain:
  //  Analog Mic Gain Setting: 0:0dB, 5:+0.5dB, ... , 210:+21.0dB (Max is 21dB.)
  //err_t AudioClass::setRecorderMode(uint8_t input_device,            int32_t input_gain, uint32_t bufsize,     bool is_digital)
  retVal |= theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC, 210,                SIMPLE_FIFO_BUF_SIZE, false          ); 

  //Set PCM capture sampling rate parameters to 16 kb/s.
  //Set channel to MONO.
  //Search for PCM codec in SD Card "/mnt/sd0/BIN" directory
  //The SD Card should look like:
  //  |--BIN/
  //  |--|--MP3DEC
  //  |--|--MP3ENC
  //  |--|--SRC
  //  |--|--WAVEDEC
  //Get files for SD Card here: https://developer.sony.com/develop/spresense/docs/arduino_tutorials_en.html#_install_dsp_files
  ei_printf("Setting up CODEC...\n");
  retVal |= theAudio->initRecorder(AS_CODECTYPE_PCM, "/mnt/sd0/BIN", AS_SAMPLINGRATE_16000, AS_CHANNEL_MONO);

  ei_printf("[DEBUG] Leaving spresense_setupAudio()...\n");
  ///delay(50000);  //Debug: Give me time to see what is happening here.
  return retVal;
}

/*******************************************************************************************************************
 * @brief   Puts a sample array[] in audioBuffer[] with size in bytes
 * @details loop() calls ei_microphone_inference_record()
 *          ei_microphone_inference_record() returns when the audioBuffer[] is full.
 *          ei_microphone_inference_record() calls get_dsp_data()
 *          A loop is formed between the following 3 routines untill the audioBuffer[] is full.
 *             get_dsp_data() calls spresense_getAudio().
 *             spresense_getAudio() calls back audio_buffer_inference_callback().
 *             That is, the PCM buffer full callback that copies audio data to inference buffers.
 *             Once the audioBuffer[] is full, inference.buf_ready = 1, and this callback loop terminates.  
 * @param   *audioBuffer, pointer to where the raw audio is stored (this is not the inference buffer.).
 * @param   *size, a pointer to the length of the captured data.
 * @return  True when audioBuffer[] is filled.
 ********************************************************************************************************************/
bool spresense_getAudio(char *audioBuffer, unsigned int* size)
{
  //ei_printf("[FLOW] Entering spresense_getAudio()...\n");

  bool data_ready = false;

  // Read in some more data until the "audioBuffer[]" is full.
  err_t err = theAudio->readFrames(audioBuffer, bufferSize, (uint32_t *)size);
  ///ei_printf("In spresense_getAudio()...err=%d\n", err);

  //           AUDIOLIB_ECODE_OK ==> Execution result OK
  if( ((err == AUDIOLIB_ECODE_OK) || (err == AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA)) && (*size > 0))
  {
    data_ready = true;
  }
  else
  {
    //"audioBuffer[]" not full yet, come back later and check again...
  }
  ///ei_printf("[FLOW] Leaving spresense_getAudio()...data_ready=%s\n", data_ready ? "true" : "false");
  return data_ready;
}

/*******************************************************************************************************************
 * @brief  Send command to start recording audio
 * @details
 * @param  bool to start or stop the Recorder object.
 * @return True if recorder starts or 
 *         False and prints error code.
 ********************************************************************************************************************/
bool spresense_startStopAudio(bool start)
{
  //ei_printf("[FLOW] Entering spresense_startStopAudio()...\n");
  
  bool cmdOk = true;

  if(start == true)
  {
    unsigned int errAudioLib = spresense_setupAudio();
    switch      (errAudioLib)
    {
      //Audio Library Error Code Definitions.
      //See https://developer.sony.com/develop/spresense/docs/arduino_developer_guide_en.html 
      case AUDIOLIB_ECODE_OK:                       cmdOk = true;  break; // 0, Execution result OK, Normal termination
      case AUDIOLIB_ECODE_SHARED_MEMORY_ERROR:      cmdOk = false; break; // 1, Shared Memory allocation error
      case AUDIOLIB_ECODE_SIMPLEFIFO_ERROR:         cmdOk = false; break; // 2, Simple FIFO error
      case AUDIOLIB_ECODE_AUDIOCOMMAND_ERROR:       cmdOk = false; break; // 3, API execution error
      case AUDIOLIB_ECODE_FILEACCESS_ERROR:         cmdOk = false; break; // 4, File access error
      case AUDIOLIB_ECODE_FILEEND:                  cmdOk = false; break; // 5, End-of-file error
      case AUDIOLIB_ECODE_BUFFER_AREA_ERROR:        cmdOk = false; break; // 6, Buffer area undefined error
      case AUDIOLIB_ECODE_BUFFER_SIZE_ERROR:        cmdOk = false; break; // 7, Buffer size specification error
      case AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA: cmdOk = false; break; // 8, Buffer area shortage error
      case AUDIOLIB_ECODE_WAV_PARSE_ERROR:          cmdOk = false; break; // 9, Unused
      case AUDIOLIB_ECODE_PARAMETER_ERROR:          cmdOk = false; break; //10, Parameter error
    }
    if(!cmdOk)
    {
      ei_printf("[ERR] Error = %d\n", errAudioLib);
    }
    theAudio->startRecorder();    // See https://developer.sony.com/develop/spresense/docs/arduino_developer_guide_en.html#_about_high_level_interface
  }
  else
  {
    theAudio->stopRecorder();     // See https://developer.sony.com/develop/spresense/docs/arduino_developer_guide_en.html#_about_high_level_interface
    empty_audio_fifo();
    theAudio->setReadyMode();     // This function switches the mode of the Audio library to the initial state.
                                  // For mode details, follow the state transition chart on the developer guide:
                                  //    https://developer.sony.com/develop/spresense/docs/arduino_developer_guide_en.html#_about_high_level_interface
                                  // This function cannot be called after transition to "Ready mode".
                                  // Immediately after boot, it is in Ready mode.
                                  // In this function, we will release resources which used in each mode, change HW to the standby state, etc.
    theAudio->end();
  }

  return cmdOk;
}

/*******************************************************************************************************************
 * @brief   Stop audio recording without releasing memory.
 * @details If you want to release memory or switch Recorder modes, then call spresense_startStopAudio().
 * @param   To pause or not to pause, that is the question.
 * @return  void
 ********************************************************************************************************************/
void spresense_pauseAudio(bool pause)
{
  //ei_printf("[FLOW] Entering spresense_pauseAudio()...\n");
    if(pause)
    {
        theAudio->stopRecorder();
    }
    else
    {
        theAudio->startRecorder();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                      EDGE IMPULSE AUDIO                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************************************************
 * @brief   Reset buffer counters for non-continuous inferecing
 * @details 
 * @param   void
 * @return  void
 ********************************************************************************************************************/
void ei_microphone_inference_reset_buffers(void)
{
  //ei_printf("[FLOW] Entering ei_microphone_inference_reset_buffers()...\n");
    
  inference.buf_ready = 0;
  inference.buf_count = 0;
  record_ready        = true;
}

/*******************************************************************************************************************
 * @brief   Init inferencing struct and setup and start the Recorder.
 * @details Called from setup().
 *          Initializes 2 inferencing buffers to size of n_samples = EI_CLASSIFIER_SLICE_SIZE = 4000.
 * @param   n_samples, # of samples in one inference.buffer[], AUDIO_SAMPLING_FREQUENCY
 * @return  True if inference.buffers[] got created.
 ********************************************************************************************************************/
static bool ei_microphone_inference_start(uint32_t n_samples)
{
  ///ei_printf("[FLOW] Entering ei_microphone_inference_start(): n_samples=%li...\n", n_samples);
  
  // Remember "inference" is declared as "static inference_t inference" and has 2 buffers[].
  
  // Create inference buffer 0.
      inference.buffers[0] = (int16_t *)ei_malloc(n_samples * sizeof(int16_t));
  if (inference.buffers[0] == NULL)
  {
    return false;
  }

  // Create inference buffer 1.
      inference.buffers[1] = (int16_t *)ei_malloc(n_samples * sizeof(int16_t));
  if (inference.buffers[1] == NULL)
  {
      ei_free(inference.buffers[0]);
      return false;
  }

  // Reset buffer status.
  inference.buf_select = 0;         // We will fill buffer[0].
  inference.buf_count  = 0;         // No samples in buffer.
  inference.n_samples  = n_samples; // How many samples we need.
                                    //    We have 2 inference.buffers[] since we are double buffering.
                                    //    inference.n_samples = # of samples stored in one of the buffers.
                                    //    inference.n_samples = EI_CLASSIFIER_SLICE_SIZE = 4000
  inference.buf_ready  = 0;         // The buffer is not full.

  // Start the Recorder.
  spresense_startStopAudio(true);

  return true;
}

/*******************************************************************************************************************
 * @brief   Wait on new data
 * @details 
 * @param   void   
 * @return  True when finished
 ********************************************************************************************************************/
bool ei_microphone_inference_record(void)
{
  ei_printf("[FLOW] Entering ei_microphone_inference_record()...\n");

  bool ret = true;

  if (inference.buf_ready == 1)
  {
    // There are 2 constraints in this story: timing and memory.
    // When switching the buffers there must be a 100% guarantee that the inference process is finished when the sampling process passes a full buffer.
    // If not, the sampling process overruns the buffer and sampled data will get lost.
    // When that happens on the microcontroller running the impulse is aborted and the following error is returned:
    ei_printf("Error sample buffer overrun. Decrease the number of slices per model window (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)\n");
    // The EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW macro is used to set the number of slices to fill the complete model window.
    // The more slices per model,
    //    the smaller the slice size,
    //    thereby the more inference cycles on the sampled data.
    //    Leading to more accurate results.
    // The sampling process uses this macro for the buffer size.
    // Where following rule applies:
    //    the bigger the buffer,
    //    the longer the sampling cycle.
    // So on targets with lower processing capabilities, we can increase this macro to meet the timing constraint.
    // Increasing the slice size, increases the volatile memory uses times 2 (since we use double buffering).
    // On a target with limited volatile memory this could be a problem. In this case you want the slice size to be small.
    ret = false;
  }
  
  inference.buf_ready = 0;          // The buffer is not full.
  inference.buf_count = 0;          // There are no bytes in the buffer.

  ///uint32_t startTime = millis();

  while (inference.buf_ready == 0)  // "audio_buffer_inference_callback()" eventually asserts "inference.buf_ready".
  {
    get_dsp_data(&audio_buffer_inference_callback);
    // get_dsp_data() calls spresense_getAudio() that executes theAudio->readFrames().
    // spresense_getAudio() calls audio_buffer_inference_callback()
  };
  
  record_ready        = false;
  inference.buf_ready = 0;
  
  ///uint32_t stopTime   = millis();
  ///uint32_t elapsedTime = stopTime - startTime;
  ///ei_printf("ei_microphone_inference_record()-->elapsedTime = %d ms\n", elapsedTime);
  
  return true;
}

/*******************************************************************************************************************
 * @brief      Check DSP semaphores, when ready get sample buffer that belongs
 *             to the semaphore.
 *             Called from "ei_microphone_inference_record()"
 * @param[in]  callback  Callback needs to handle the audio samples
  *
 * ei_microphone_inference_record() calls this routine like:
 *    get_dsp_data(&audioBuffer_inference_callback);
 *    So,
 *               &audioBuffer_inference_callback = void (*callback)
 *               &audioBuffer_inference_callback = void (*callback)(void *buffer, uint32_t n_bytes)
 *    static void audioBuffer_inference_callback                   (void *buffer, uint32_t n_bytes)
 ********************************************************************************************************************/
 static void get_dsp_data(void (*callback)(void *buffer, uint32_t n_bytes))
{
  //ei_printf("[FLOW] Entering get_dsp_data()...\n");

  size_t length;
  bool res = spresense_getAudio((char *)&audioBuffer[0], &length);

  ///ei_printf("In get_dsp_data()...audioBuffer[] length=%d\n", length);

  if(res)
  {
    ///ei_printf("In get_dsp_data()...audioBuffer[] is full...\n");
    callback((void *)&audioBuffer[0], length);
    StateLED0 = !StateLED0;
    digitalWrite(LED0, StateLED0);
    digitalWrite(LED1, LOW);
  }
  else
  {
    //"audioBuffer" not full yet, come back later and execute "spresense_getAudio()"" again...
    ///ei_printf("--- --- --- no audio data gotten...\n");
    StateLED1 = !StateLED1;
    digitalWrite(LED0, LOW);
    digitalWrite(LED1, StateLED1);
  }
 }

/*******************************************************************************************************************
 * @brief      Inference audio callback, store samples in ram buffer.
 *             Signal when buffer is full, and swap buffers.
 * @param      buffer   Pointer to source buffer
 * @param[in]  n_bytes  Number of bytes to write
 ********************************************************************************************************************/
static void audio_buffer_inference_callback(void *buffer, uint32_t n_bytes)
{
  ///ei_printf("[FLOW] Entering audio_buffer_inference_callback()...\n");

  int16_t *samples = (int16_t *)buffer;

  for(uint32_t i = 0; i < (n_bytes >> 1); i++)
  {
    inference.buffers[inference.buf_select][inference.buf_count++] = samples[i];

    if(inference.buf_count >= inference.n_samples)
    {
      inference.buf_select ^= 1;
      inference.buf_count   = 0;
      inference.buf_ready   = 1;
    }
  }
}

/*******************************************************************************************************************
 * @brief   Get raw audio signal data and convert to float.
 * @details Converts the elements of the Q15 vector to floating-point vector.
 * @param   offset
 * @param   length,   number of samples to process
 * @param   *out_ptr, points to the floating-point output vector that the Classifier() uses.
 * @return  0
 ********************************************************************************************************************/
int ei_microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
  ///ei_printf("Entering ei_microphone_audio_signal_get_data()...\n");

  //void arm_q15_to_float(const q15_t * pSrc,                                   float32_t * pDst, uint32_t blockSize);
         arm_q15_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr,          length);

  return 0;
}

/*******************************************************************************************************************
 * @brief   Free memory.
 * @details Stop Recorder and allow the Audio object to chage modes as desired.
 * @param   void
 * @return  true
 ********************************************************************************************************************/
bool ei_microphone_inference_end(void)
{
  ///ei_printf("[FLOW] Entering ei_microphone_inference_end()...\n");

  record_ready = false;
  spresense_startStopAudio(false);  

  ei_free(inference.buffers[0]);
  ei_free(inference.buffers[1]);
  return true;
}

#ifdef NON_CONTINIOUS

/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// 

/*******************************************************************************************************************
 * @brief   Arduino setup function
 * @details 
 * @param   void
 * @return  void
 ********************************************************************************************************************/
void setup()
{
  // Setup on-board LEDs.
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
    
  // Start serial port to display status updates.
  Serial.begin(115200);

  ei_printf("*** Edge Impulse Inferencing Audio Demo with the Sony Spresense ***\n");
  ei_printf("*** Non-Continious Mode ***\n");
    
  ei_printf("[DEBUG] sizeof(short):"); Serial.println(sizeof(short));

  // Display summary of inferencing settings (from model_metadata.h):
  ei_printf("Inferencing settings:\n");
  ei_printf("\tInterval: ");
  ei_printf_float(                (float)EI_CLASSIFIER_INTERVAL_MS);                      //      0.062500 ms.
  ei_printf(" ms.\n");
  ei_printf("\tFrame size: %d\n",        EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);             // 16,000.
  ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);            //  1,000 ms.
  ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) /
                                      sizeof(ei_classifier_inferencing_categories[0]));   //      4.
  
  // Initialize the "impulse" inside the EdgeImpulse Model library exported from EdgeImpulse Studio.
  run_classifier_init();

  // Init inferencing struct and setup and start the Recorder.
  if (ei_microphone_inference_start(EI_CLASSIFIER_RAW_SAMPLE_COUNT) == false)
  {
      ei_printf("ERR: Failed to setup audio sampling.\r\n");
      return;
  }
  else
  {
    ei_printf("^^^ Setup of audio sampling completed. ^^^\r\n");
  }
}

/*******************************************************************************************************************
 * @brief   Arduino main function. Runs the inferencing loop.
 * @details Non-continious inferencing.
 * @param   void
 * @return  void
 ********************************************************************************************************************/
void loop()
{
  ///ei_printf("[DEBUG] Resetting mic buffers...\n");

  ei_microphone_inference_reset_buffers();
  ///ei_printf("[DEBUG] Resetting mic buffers...Done!\n");
  
  //delay(3333); <-- ATTENTION! If you do this here you get in the Serial Monitor:
  //                 "Attention: module[4][0] attention id[1]/code[6] (objects/media_recorder/audio_recorder_sink.cpp L84)"
  //                 This probably has something to do with de-asserting the volitale variable "record_ready"
  //                 in "ei_microphone_inference_reset_buffers()" that must be used in an iterrupt somewhere(?).
  //                 ...and/or the "spresense_pauseAudio(false)" has started the Recoder via "theAudio->startRecorder()", 
  //                 So we need to go get the bytes that are coming in.
  //                 TODO: Check the nutxx library for file "audio_recorder_sink.cpp" to see what is happening.
  
  ei_printf("--- --->Recording...\n");
  uint32_t startTime = millis();

  // Once "ei_microphone_inference_record()" gets enuf Samples it asserts "record_ready" and returns.
  bool m = ei_microphone_inference_record();
  if (!m)
  {
    ei_printf("ERR: Failed to record audio...\n");
    return;
  }

  // We are not doing continious sampling or inferencing so pause the audio.
  spresense_pauseAudio(true);

  StateLED0 = false;
  StateLED1 = false;
  StateLED2 = false;
  StateLED3 = false;
  digitalWrite(LED0, StateLED0);
  digitalWrite(LED1, StateLED1);
  digitalWrite(LED2, StateLED2);
  digitalWrite(LED3, StateLED3);

  uint32_t stopTime = millis();
  uint32_t elapsedTime = stopTime - startTime;
  ei_printf("--- --->Recording done in %d ms\n", elapsedTime);  // Should be close to "EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16"
  
  signal_t signal;
  signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
  signal.get_data = &ei_microphone_audio_signal_get_data;
  ei_impulse_result_t result = { 0 };

  ///ei_printf("Calling run_classifier()...\n");
  ///startTime = millis();
  EI_IMPULSE_ERROR r = run_classifier(&signal, &result, debugNN);
  ///stopTime = millis();
  ///elapsedTime = stopTime - startTime;
  ///ei_printf("Calling run_classifier()...Done in %li seconds.\n", elapsedTime);
  
  if (r != EI_IMPULSE_OK)
  {
    ei_printf("ERR: Failed to run classifier (%d)\n", r);
    return;
  }

  // Print the Predictions.
  ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms, Total: %d ms.): \n",
             result.timing.dsp,  result.timing.classification,  result.timing.anomaly,
             result.timing.dsp + result.timing.classification + result.timing.anomaly);
        
  // Determine max inference value so we can star it when we display the list of predictions.      
  uint8_t maxInferenceIdx = 0;
  for (size_t ix = 1; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
  {
    if( result.classification[ix].value > result.classification[maxInferenceIdx].value)
    {
      maxInferenceIdx = ix;
    }          
  }
  for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
  {
    if (maxInferenceIdx == ix)
    {
      // Highlight max prediction.
      ei_printf("*** %s: \t%f ***\r\n", result.classification[ix].label, result.classification[ix].value);
    }
    else
    {
     // Show other predictions.            
      ei_printf("    %s: \t%f\r\n", result.classification[ix].label, result.classification[ix].value);
    }
  }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
  ei_printf("    anomaly score: %f\r\n", result.anomaly);
#endif
        
  uint32_t delay_ms = 3000;        
  ei_printf("Starting inferencing  in [sec]...%li...", delay_ms / 1000);
  while (delay_ms > 0)
  {
    delay(1000);
    delay_ms = delay_ms - 1000;
    ei_printf("%li...", delay_ms/1000);
  }
  ei_printf("\n");
  
  //delay(50000);  // Allow me to see what is happening.
  
  spresense_pauseAudio(false);
  //DO NOT ADD ANY DELAYS AFTER THIS...
}

/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// 

#endif

#ifdef CONTINIOUS

/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// 

static int  printResult = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);  //Global: Show predictions if we have enuf SLICES.

/*******************************************************************************************************************
 * @brief   Arduino setup function
 * @details 
 * @param   void
 * @return  void
 ********************************************************************************************************************/
void setup()
{
  // Setup on-board LEDs.
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
    
  // Start serial port to display status updates.
  Serial.begin(115200);

  if (EI_CLASSIFIER_FREQUENCY != 16000)
  {
    ei_printf("ERR: Frequency is %d but can only sample at 16000Hz\n", (int)EI_CLASSIFIER_FREQUENCY);
    return;
  }  

  ei_printf("*** Edge Impulse Inferencing Audio Demo with the Sony Spresense ***\n");
  ei_printf("*** Continious Mode ***\n");
    
  ei_printf("[DEBUG] sizeof(short):"); Serial.println(sizeof(short));

  // Display summary of inferencing settings (from model_metadata.h):
  ei_printf("Inferencing settings:\n");
  
  ei_printf("\tInterval: ");
  ei_printf_float(                                      (float)EI_CLASSIFIER_INTERVAL_MS);              // 0.062500 ms.
  ei_printf(" ms.\n");
  
  ei_printf("\tFrame Size: %d.\n",                             EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);     // 16000.

  ei_printf("\tRaw Sample Count: %d.\n",                       EI_CLASSIFIER_RAW_SAMPLE_COUNT);         // 16000.
  
  ei_printf("\tSample Length: %d ms.\n",                       EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);    // 1000 ms.
  
  ei_printf("\tClassifier Slice Size: %d.\n",                  EI_CLASSIFIER_SLICE_SIZE);               // 4000.

  ei_printf("\tClassifier Slice Size per Model Window: %d.\n", EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);  // 4
  
  ei_printf("\tNo. of Classes: %d\n", sizeof(ei_classifier_inferencing_categories) /
                                      sizeof(ei_classifier_inferencing_categories[0]));                 // 4.

  // Initialize the "impulse" inside the EdgeImpulse Model library exported from EdgeImpulse Studio.
  run_classifier_init();

  // Init inferencing struct and setup and start the Recorder.
  if (ei_microphone_inference_start(EI_CLASSIFIER_SLICE_SIZE) == false) // 4000.
  {
      ei_printf("ERR: Failed to setup audio sampling.\r\n");
      return;
  }
  else
  {
    ei_printf("^^^ Setup of continious audio sampling completed. ^^^\r\n");
  }
}

/*******************************************************************************************************************
 * @brief   Arduino main function. Runs the inferencing loop.
 * @details Continious inferencing.
 * @param   void
 * @return  void
********************************************************************************************************************/
void loop()
{
  // Once "ei_microphone_inference_record()" gets enuf Samples it asserts "record_ready" and returns.
  bool m = ei_microphone_inference_record();
  if (!m)
  {
    ei_printf("ERR: Failed to record audio...\n");
    return;
  }

  StateLED0 = false;
  StateLED1 = false;
  StateLED2 = false;
  StateLED3 = false;
  digitalWrite(LED0, StateLED0);
  digitalWrite(LED1, StateLED1);
  digitalWrite(LED2, StateLED2);
  digitalWrite(LED3, StateLED3);

  // Setup "signal": sets the callback function on the "signal_t" structure to reference the audio inference buffer:
  signal_t signal;
  signal.total_length        = EI_CLASSIFIER_SLICE_SIZE;           //4000.
  signal.get_data            = &ei_microphone_audio_signal_get_data;
  ei_impulse_result_t result = { 0 };

  // Classify it: The "run_classifier_continuous()" is called which will
  //    take the slice of data,
  //    run the DSP pipeline over the data,
  //    stitch data together, and then 
  //    classify the data.
  // See https://github.com/edgeimpulse/inferencing-sdk-cpp/blob/04579b9b1784db858c3b444e38ca4bd89c65c925/classifier/ei_run_classifier.h#L159
  // See https://docs.edgeimpulse.com/docs/tutorials/continuous-audio-sampling 
  EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debugNN);
  
  if (r != EI_IMPULSE_OK)
  {
    ei_printf("ERR: Failed to run classifier (%d)\n", r);
    return;
  }

  if (++printResult >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW >> 1))  //EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW = 4
  {
    // Print the Predictions
    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms, Total: %d ms.): \n",
               result.timing.dsp,  result.timing.classification,  result.timing.anomaly,
               result.timing.dsp + result.timing.classification + result.timing.anomaly);
        
    // Determine max inference value so we can star it when we display the list of predictions.      
    uint8_t maxInferenceIdx = 0;
    for (size_t ix = 1; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
      if( result.classification[ix].value > result.classification[maxInferenceIdx].value)
      {
        maxInferenceIdx = ix;
      }          
    }
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
      if (maxInferenceIdx == ix)
      {
        // Highlight max prediction.
        ei_printf("*** %s: \t%f ***\r\n", result.classification[ix].label, result.classification[ix].value);
      }
      else
      {
      // Show other predictions.            
        ei_printf("    %s: \t%f\r\n", result.classification[ix].label, result.classification[ix].value);
      }
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %f\r\n", result.anomaly);
#endif
    
    printResult = 0;

  } // End of printing Continious Predictions.
}

/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// ///
/// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// 

#endif

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_MICROPHONE
#error "Invalid model for current sensor."
#endif
