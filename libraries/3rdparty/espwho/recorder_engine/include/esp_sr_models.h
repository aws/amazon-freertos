#pragma once
#include "esp_sr_iface.h"

//Contains declarations of all available speech recognion models. Pair this up with the right coefficients and you have a model that can recognize
//a specific phrase or word.
extern const esp_sr_iface_t esp_sr_wakenet2_float;
extern const esp_sr_iface_t sr_model_wakenet1_float;
extern const esp_sr_iface_t sr_model_wakenet1_quantized;
extern const esp_sr_iface_t esp_sr_wakenet2_quantized;
extern const esp_sr_iface_t esp_sr_wakenet3_quantized;
extern const esp_sr_iface_t esp_sr_wakenet4_quantized;


/* example

static const sr_model_iface_t *model = &sr_model_wakenet3_quantized;

//Initialize wakeNet model data
static model_iface_data_t *model_data=model->create(DET_MODE_90);

//Set parameters of buffer
int audio_chunksize=model->get_samp_chunksize(model_data);
int frequency = model->get_samp_rate(model_data);
int16_t *buffer=malloc(audio_chunksize*sizeof(int16_t));

//Detect
int r=model->detect(model_data, buffer);
if (r>0) {
    printf("Detection triggered output %d.\n",  r);
}

//Destroy model
model->destroy(model_data)

*/
