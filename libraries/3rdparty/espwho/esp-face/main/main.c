#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "sdkconfig.h"
#include "image_util.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include "fr_flash.h"
#include "ref_id.h"

void md_task(void *arg)
{
    float ratio = 0.3;
    int ori_w = 320;
    int ori_h = 240;
    int w = ori_w * ratio;
    int h = ori_h * ratio;
    int c = 3;
    int init = 1;
    camera_fb_t * fb = NULL;
    dl_matrix3du_t *image_ori = dl_matrix3du_alloc(1, ori_w, ori_h, c);
    dl_matrix3du_t *image_motion = dl_matrix3du_alloc(1, w, h, c);
    dl_matrix3du_t *image_new = dl_matrix3du_alloc(1, w, h, c);
    dl_matrix3du_t *image_tmp = dl_matrix3du_alloc(1, w, h, c);

    //dl_matrix3du_t *image1 = (dl_matrix3du_t *)get_coeff_ref_in.getter_3d("image1", NULL, 0);
    //dl_matrix3du_t *image2 = (dl_matrix3du_t *)get_coeff_ref_in.getter_3d("image2", NULL, 0);

    while (1)
    {
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
            break;
        }
        if(!fmt2rgb888(fb->buf, fb->len, fb->format, image_ori->item))
        {
            ESP_LOGW(TAG, "fmt2rgb888 failed");
            //res = ESP_FAIL;
            //dl_matrix3du_free(image_matrix);
            //break;
        }
        image_resize_linear(image_new->item, image_ori->item, w, h, c, ori_w, ori_h);
        int count = w * h * c;
        if (init)
        {
            memcpy(image_motion->item, image_new->item, count * sizeof(uint8_t));
            init = 0;
            continue;
        }

        image_abs_diff(image_new->item, image_motion->item, image_new->item, count);
        image_threshold(image_new->item, image_new->item, 35, 255, count, BINARY);
        image_erode(image_tmp->item, image_new->item, w, h, c);

        int motion_changes = 0;
        int padding = 10;
        for (int j = padding; j < h - padding - 1; j += 2)
        {
            uint8_t *data = image_tmp->item + j * w * c;
            for (int i = padding; i < w - padding - 1; i += 2)
            {
                uint8_t *d = data + i * 3;
                if (d[0] + d[1] + d[2] > 750)
                {
                    motion_changes++;
                }
            }
        }
        printf("changes: %lld\n", motion_changes);
    }

}

void app_main()
{
    xTaskCreatePinnedToCore(&md_task, "md_task", 4096, NULL, 5, NULL, 0);
}
