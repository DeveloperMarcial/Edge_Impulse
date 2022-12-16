/**
 * @brief      Copy raw feature data in out_ptr.
 *             Function called by Edge Impulse inference library.
 *
 * @param[in]  offset   The offset
 * @param[in]  length   The length
 * @param      out_ptr  The out pointer. The Edge Impulse Classifier uses this buffer as inpuy.
 *
 * @return     0
 */
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr)
{
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

/**
 * @brief      Edge Impulse Machine Learning Model Implementation Function
 *
 * @param[in]  NA
 * @return     void
 **/
void ML_loop()
{
    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");
    
    features[0] = myMoistureValue;

    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE)
    {
        ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
                   EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
        delay(1000);
        return;
    }

    ei_impulse_result_t result = { 0 };         // The results of the ML classification.

    // The Edge Impulse Classifier() requires a "signal" as input.
    signal_t features_signal;
    features_signal.total_length = sizeof(features) / sizeof(features[0]);
    features_signal.get_data = &raw_feature_get_data;

    // Invoke the Edge Impulse Impulse.
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    ei_printf("run_classifier returned: %d\n", res);

    if (res != 0)
    {
      ei_printf("run_classifier() errored...\n", res);
      return;
    }
    
    // print the predictions
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
                result.timing.dsp,
                result.timing.classification,
                result.timing.anomaly);
    ei_printf(": \n");

    ei_printf("[");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
        ei_printf("%.5f", result.classification[ix].value);
        #if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf(", ");
        #else
        if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
            ei_printf(", ");
        }
        #endif
    }
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("%.3f", result.anomaly);
    #endif
    ei_printf("]\n");

    // Human-readable predictions.
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
        ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
        if ("Dry"==result.classification[ix].label)
        {
          dryPrediction = result.classification[ix].value;
        }
        if ("Wet"==result.classification[ix].label)
        {
          wetPrediction = result.classification[ix].value;
        }
    }
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
    #endif
}
