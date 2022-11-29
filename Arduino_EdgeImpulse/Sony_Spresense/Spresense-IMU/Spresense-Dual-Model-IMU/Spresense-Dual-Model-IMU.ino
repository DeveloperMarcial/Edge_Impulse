/* 
 ***************************************************************************************
 * Edge Impulse ingestion SDK                                                          *
 * Copyright (c) 2022 EdgeImpulse Inc.                                                 *
 *                                                                                     *
 * Licensed under the Apache License, Version 2.0 (the "License");                     *
 * you may not use this file except in compliance with the License.                    *
 * You may obtain a copy of the License at                                             *
 * http://www.apache.org/licenses/LICENSE-2.0                                          * 
 *                                                                                     *
 * Unless required by applicable law or agreed to in writing, software                 *
 * distributed under the License is distributed on an "AS IS" BASIS,                   * 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.            *
 * See the License for the specific language governing permissions and                 *
 * limitations under the License.                                                      * 
 *                                                                                     *
 ***************************************************************************************
 * The MIT License (MIT)                                                               *
 *                                                                                     *
 * Modifications to the Software Edge Impulse example programs                         *
 * referenced above as "Edge Impulse ingestion SDK"                                    *
 * for the IMU library called "static_buffer.ino".                                     *
 * Some code copied some code from the "firmware-sony-spresense":                      *
 * https://github.com/edgeimpulse/firmware-sony-spresense/blob/main/ei_run_impulse.cpp *
 * and saved herin as the Software "Spresense-Dual-IMU.ino"                            *
 * Copyright (c) 2022 M. Marcial                                                       *
 * All Rights Reseved                                                                  *
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

////////////////////////////////////////////////////////////////////////////////////////
// Rem in one of the following 2 lines in to choose what code to compile. 
// IMU_126 is to be compiled for the MainCore.                            
// IMU_224 is to be compiled for the SubCore.                             
//                                                                        
  #define IMU_126  // Change Core to MainCore.
//#define IMU_224  // Change Core to SubCore.
////////////////////////////////////////////////////////////////////////////////////////

#ifdef IMU_126
// To prove this code runs with the KX126 on the MainCore
// rem the following statement to run just the MainCore with KX126 IMU.
#define BOOT_SUBCORE1
#endif
////////////////////////////////////////////////////////////////////////////////////////

#ifdef IMU_224
// To prove this code runs with the KX224 on the SubCore1
// load "Spresese_MainCore.ino" into the MainCore.
// The only function "Spresese_MainCore.ino" performs is to boot SubCore1.
// And above 
//    //#define IMU_126 <--   rem this line
//      #define IMU_224 <-- unrem this line
#endif
////////////////////////////////////////////////////////////////////////////////////////

#if defined(IMU_126) && defined(IMU_224)
#error "Please select either IMU_126 or IMU_224 but not both.
#endif

#if !defined(IMU_126) && !defined(IMU_224)
#error "Please select either IMU_126 or IMU_224. Neither is selected."
#endif
////////////////////////////////////////////////////////////////////////////////////////

/* Includes ---------------------------------------------------------------- */
#include <Tutorial_Continuous_motion_recognition__no_mods__inferencing.h> // <-- Change to your deployed Edge Impulse Arduino library.

#include <Wire.h>                                     // I2C: Bus0. SCL=D15, SDA=D14.
                                                      // I2C: Bus1. SCL=D09, SDA=D03.

#include <MP.h>                                       // Spresense multi-core intercomm.
#include <MPMutex.h>

/* Create a MPMutex object */
MPMutex mutex(MP_MUTEX_ID0);

#ifdef IMU_126
#include <KX126.h>                                    // 3-axis IMU.
//#include <BM1383AGLV.h>                             // Barometric pressure & tempeture.
//#include <BM1422AGMV.h>                             // 3-axis magnetometer.
KX126 KX126(KX126_DEVICE_ADDRESS_1F);                 // 3-axis IMU from KX126
#warning "Compiling for MainCore with KX126."
#ifdef SUBCORE
#error "This needs to be compiled for the Sony Spresense MainCore." \
       "If you are trying to compile for SubCore1 then:" \
       "un-rem  #define IMU_224" \
       "and rem #define IMU_126" '
#endif
#endif

#ifdef IMU_224
#include <KX224_I2C.h>                                // 3-axis IMU.
KX224 KX224(KX224_DEVICE_ADDRESS_1E);                 // 3-axis IMU from KX224.
#warning "Compiling for SubCore1 with KX224."
#if (SUBCORE != 1)
#error "This needs to be compiled for the Sony Spresense SubCore1." \
       "If you are trying to compile for MainCore then:" \
       "un-rem  #define IMU_126" \
       "and rem #define IMU_224"
#endif
#endif

/* Constant defines -------------------------------------------------------- */
#define N_AXIS_SAMPLED			3  // Number of axes samples.
#define SIZEOF_N_AXIS_SAMPLED	(sizeof(sample_format_t) * N_AXIS_SAMPLED)
#define CONVERT_G_TO_MS2    9.80665f
#define ACC_SAMPLE_TIME_MS  0.49922f
#define FLASH_WRITE_TIME_MS 5.288f

/* Global variables ------------------------------------------------------- */
static int      acc_sample_count = 0;                         // Used as an index into acc_buf[].
static uint32_t samplerate_divider;                           // For this program with this library:
                                                              //    = (sample_interval_ms / ACC_SAMPLE_TIME_MS);      
                                                              //    =                  16 / 0.49922f = 32.04999
static float    acc_buf[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];  // Buffer to collect IMU data samples.
static float    imu_data[N_AXIS_SAMPLED];                     // Buffer to hold one IMU sample, aka {x,y,z}. N_AXIS_SAMPLED=3.
typedef float   sample_format_t;                              // Number of axis used and sample data format.

#ifdef IMU_126
//String coreName = "Main: "; <-- Can't declare like this since ei_printf() wil consume it.
char coreName[] = {"MainCore: "};
#endif

#ifdef IMU_224
//String coreName = "SubCore1: "; <-- Can't declare like this since ei_printf() wil consume it.
char coreName[] = {"SubCore1: "};
#endif

// Callback sampler of IMU chip.
typedef bool (*sampler_callback)(const void *sample_buf, uint32_t byteLenght);
typedef bool (*starter_callback)(sampler_callback callback, float sample_interval_ms);
sampler_callback  cb_sampler;

/**
 * @brief      Setup timing and data handle callback function
 *
 * @param[in]  callsampler         Function to handle the sampled data
 * @param[in]  sample_interval_ms  The sample interval milliseconds
 *
 * @return     true
 */
bool ei_inertial_sample_start(sampler_callback callsampler, float sample_interval_ms)
{
    cb_sampler = callsampler;

  //samplerate_divider  =  (int)(sample_interval_ms / ACC_SAMPLE_TIME_MS); // 16 / 0.49922f = 32.04999. <--Code from Firmware.
  //samplerate_divider += ((int) sample_interval_ms);                      // 32.04999 + 16 = 48.04999. <--Code from Firmware.

    samplerate_divider  = ((int) sample_interval_ms);                      // Yields a 1888ms sample collection time in loop().

    return true;
}

/**
 * @brief      Get data from sensor, convert and call callback to handle.
  */
int ei_inertial_read_data(void)
{
  float acc_data[3];
  volatile uint32_t div_sample_count;
  for (div_sample_count = 0; div_sample_count < samplerate_divider; div_sample_count++)
  {
      if(spresense_getAcc(acc_data))
      {
          return -1;
      }
  }

  imu_data[0] = acc_data[0] * CONVERT_G_TO_MS2;
  imu_data[1] = acc_data[1] * CONVERT_G_TO_MS2;
  imu_data[2] = acc_data[2] * CONVERT_G_TO_MS2;

  // We got new data. Release callback to copy imu_data[] to acc_buf[].
  cb_sampler((const void *)&imu_data[0], SIZEOF_N_AXIS_SAMPLED);  // Calls acc_data_callback().

  return 0;
}

/**
 * @brief      Get data from specific sensor.
 * @param[out] acc_val[]. 
 */
int spresense_getAcc(float acc_val[3])
{
    #ifdef IMU_126
    int returnVal = KX126.get_val(acc_val);
    return returnVal;
    #endif

    #ifdef IMU_224
    int returnVal = KX224.get_val(acc_val);
    return returnVal;
    #endif
}

/**
 * @brief      Called by the inertial sensor module when a sample is received.
 *             Stores sample data in acc_buf
 * @param[in]  sample_buf  The sample buffer
 * @param[in]  byteLenght  The byte length
 *
 * @return     { description_of_the_return_value }
 */
static bool acc_data_callback(const void *sample_buf, uint32_t byteLength)
{
    float *buffer = (float *)sample_buf;
    for(uint32_t i = 0; i < (byteLength / sizeof(float)); i++)
    {
        acc_buf[acc_sample_count + i] = buffer[i];
    }

    return true;
}

/**
 * @brief      Arduino setup function
 */
void setup()
{
  Serial.begin(115200);
  while (!Serial);

  #ifdef BOOT_SUBCORE1
  int subid = 1;
  int ret = MP.begin(subid);
  if (ret < 0)
  {
    // Returns error code. It returns minus value on failure.
    // Return values:
    //     -22(-EINVAL)	   Invalid argument
    //      -2(-ENOENT)	   No such SubCore program
    //    -116(-ETIMEDOUT) No response of boot completion from SubCore
    MPLog("MP.begin(%d) error = %d\n", subid, ret);
  }
  #endif

  #ifdef IMU_224
  MP.begin();
  #endif

  ei_printf("%sEdge Impulse Inferencing Demo\r\n", coreName);

  // Summary of inferencing settings (from model_metadata.h).
  ei_printf("%sInferencing settings:\r\n", coreName);

  ei_printf("%s\tInterval: %.4f ms\r\n",   coreName,                           (float)EI_CLASSIFIER_INTERVAL_MS);           // 16ms
  ei_printf("%s\tFrame size: %d\r\n",      coreName,                                  EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);  // 375
  ei_printf("%s\tSample length: %.4f ms.\r\n", coreName, 1000.0f * static_cast<float>(EI_CLASSIFIER_RAW_SAMPLE_COUNT) /
                                                        (1000.0f / static_cast<float>(EI_CLASSIFIER_INTERVAL_MS)));         // 1000*125 / (1000/16) = 2000ms
                                                                                                                            // 125000   / 62.5      = 2000ms
                                                                                                                            // 125      * 16        = 2000ms
  ei_printf("%s\tNo. of classes: %d\n", coreName, sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));

  // Begin I2C bus.
  #ifdef IMU_126
  Wire.begin();
  #else
  Wire1.begin();
  #endif
  
  byte rc;
  #ifdef IMU_126
  // Initialize 3-axis IMU from KX126.
  ei_printf("%sKX126 initialization about to begin...\r\n", coreName);
  rc = KX126.init();
  if (rc != 0)
  {
    ei_printf("%sKX126 initialization failed...\r\n", coreName);
    return;
  }
  ei_printf("%sKX126 initialization about to begin...Done!\r\n", coreName);
  #endif

  #ifdef IMU_224
  // Initialize 3-axis IMU from KX224.
  ei_printf("%sKX224 initialization about to begin...\r\n", coreName);
  rc = KX224.init();
  if (rc != 0)
  {
    ei_printf("%sKX224 initialization failed...\r\n", coreName);
  }
  ei_printf("%sKX224 initialization finished...\r\n", coreName);
  #endif

  ei_inertial_sample_start(&acc_data_callback, EI_CLASSIFIER_INTERVAL_MS);


  ei_printf("%sStarting inferencing...\r\n", coreName);
}

/**
 * @brief      Arduino main function
 */
void loop()
{
    /* Busy wait until lock the mutex */
    int ret = 0;
    do
    {
      ret = mutex.Trylock();
    } while (ret != 0);

    /* Run Sampler */
    // Note: To get this sampler loop to operate correctly "ei_inertial_sample_start()" was modified."
    // Symptom:
    //    Line: for(int i = 0; i < EI_CLASSIFIER_RAW_SAMPLE_COUNT; i++)
    //    The EI Firmware code had this line but it creates a 6 second sample collection period.
    // The Firmware did:
    //    samplerate_divider  =  (int)(sample_interval_ms / ACC_SAMPLE_TIME_MS);  // 16 / 0.49922f = 32.04999. <--Code from Firmware.
    //    samplerate_divider += ((int) sample_interval_ms);                       // 32.04999 + 16 = 48.04999. <--Code from Firmware.
    // This was changed to the following
    //    samplerate_divider  = ((int) sample_interval_ms);                      // Yields a 1888ms sample collection time in loop().
    // Note: The for-loop must count up to EI_CLASSIFIER_RAW_SAMPLE_COUNT so that the acc_buf[] gets filled completely.
    //       If acc_buf[] is not filled completely, then inferencing will be wrong.
    ei_printf("%sSampling...\r\n", coreName);
    acc_sample_count = 0;    
    uint32_t startTime = millis();
    for(int i = 0; i < EI_CLASSIFIER_RAW_SAMPLE_COUNT; i++)
    {
      if(ei_inertial_read_data())
      {
        ei_printf("%sErr: failed to get sensor data\r\n", coreName);
        continue;
      }
      acc_sample_count += EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;  // Update index into acc_buf[].
    }
    uint32_t endTime = millis() - startTime;
    ei_printf("%sSampling...Done in %d ms!\r\n", coreName, endTime);

    /* Unlock the mutex */
    mutex.Unlock();    
    
    //
    // Create a data structure to represent this Window of data.
    //
    signal_t signal;
    // Update the "signal" with data from the acc_buf[].
    int err = numpy::signal_from_buffer(acc_buf, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
    if (err != 0)
    {
      ei_printf("%sERR: signal_from_buffer failed (%d)\n", coreName, err);
      return;
    }
    ei_impulse_result_t result = { 0 };

    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false /* debug */);
    ei_printf("%srun_classifier returned: %d\n", coreName, res);

    if (res != 0) return;

    // print the predictions
    ei_printf("%sPredictions ", coreName);
    ei_printf("%s(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)", coreName,
                result.timing.dsp,
                result.timing.classification,
                result.timing.anomaly);
    ei_printf(":\r\n");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
        ei_printf("%s    %s: %.5f\r\n", coreName, result.classification[ix].label, result.classification[ix].value);
    }
    ei_printf("%s    anomaly score: %.3f\r\n", coreName, result.anomaly);
}
