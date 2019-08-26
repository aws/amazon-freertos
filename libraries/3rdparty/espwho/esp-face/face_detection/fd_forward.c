/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include <string.h>
#include <math.h>
#include "esp_system.h"
#include "fd_forward.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define CONFIG_MTMN_LITE_QUANT    ( 1 )

box_array_t * pnet_forward( dl_matrix3du_t * image,
                            fptp_t min_face,
                            fptp_t pyramid,
                            net_config_t * config )
{ /*{{{*/
    mtmn_net_t * out;
    fptp_t scale = 1.0f * config->w / min_face;
    image_list_t sorted_list[ 4 ] = { NULL };
    image_list_t * origin_head[ 4 ] = { NULL }; /* store original point to free */
    image_list_t all_box_list = { NULL };
    box_array_t * pnet_box_list = NULL;
    box_t * pnet_box = NULL;

    int width = round( image->w * scale );
    int height = round( image->h * scale );

    dl_matrix3du_t * in = dl_matrix3du_alloc( 1, width, height, image->c );

    for( int i = 0; i < 4; i++ )
    {
        if( DL_IMAGE_MIN( width, height ) <= config->w )
        {
            break;
        }

        image_resize_linear( in->item, image->item, width, height, in->c, image->w, image->h );

        in->h = height;
        in->w = width;
        in->stride = in->w * in->c;

        #if CONFIG_MTMN_LITE_FLOAT
            out = pnet_lite_f( in );
        #endif

        #if CONFIG_MTMN_LITE_QUANT
            out = pnet_lite_q( in, DL_XTENSA_IMPL );
        #endif

        #if CONFIG_MTMN_HEAVY_QUANT
            out = pnet_heavy_q( in, DL_XTENSA_IMPL );
        #endif

        if( out )
        {
            origin_head[ i ] = image_get_valid_boxes( out->category->item,
                                                      out->offset->item,
                                                      out->category->w,
                                                      out->category->h,
                                                      config->w,
                                                      config->threshold.score,
                                                      scale );

            if( origin_head[ i ] )
            {
                image_sort_insert_by_score( &sorted_list[ i ], origin_head[ i ] );

                image_nms_process( &sorted_list[ i ], 0.5, true );
            }

            dl_matrix3d_free( out->category );
            dl_matrix3d_free( out->offset );
            dl_matrix3d_free( out->landmark );
            free( out );
        }

        scale *= pyramid;
        width = round( image->w * scale );
        height = round( image->h * scale );
    }

    dl_matrix3du_free( in );

    for( int i = 0; i < 4; i++ )
    {
        image_sort_insert_by_score( &all_box_list, &sorted_list[ i ] );
    }

    image_nms_process( &all_box_list, config->threshold.nms, false );

    if( all_box_list.len )
    {
        if( all_box_list.len > config->threshold.candidate_number )
        {
            all_box_list.len = config->threshold.candidate_number;
        }

        image_calibrate_by_offset( &all_box_list );

        pnet_box_list = ( box_array_t * ) calloc( 1, sizeof( box_array_t ) );

        pnet_box = ( box_t * ) calloc( all_box_list.len, sizeof( box_t ) );

        image_box_t * t = all_box_list.head;

        /* no need to store landmark */
        for( int i = 0; i < all_box_list.len; i++, t = t->next )
        {
            pnet_box[ i ] = t->box;
        }

        pnet_box_list->box = pnet_box;
        pnet_box_list->len = all_box_list.len;
    }

    for( int i = 0; i < 4; i++ )
    {
        if( origin_head[ i ] )
        {
            free( origin_head[ i ]->origin_head );
            free( origin_head[ i ] );
        }
    }

    return pnet_box_list;
} /*}}}*/

box_array_t * pnet_forward2( dl_matrix3du_t * image,
                             fptp_t min_face,
                             fptp_t pyramid,
                             int pyramid_times,
                             net_config_t * config )
{ /*{{{*/
    mtmn_net_t * out;
    fptp_t scale = 1.0f * config->w / min_face;
    image_list_t * sorted_list = ( image_list_t * ) calloc( pyramid_times, sizeof( image_list_t ) );
    image_list_t ** origin_head = ( image_list_t ** ) calloc( pyramid_times, sizeof( image_list_t * ) ); /* store original point to free */
    image_list_t all_box_list = { NULL };
    box_array_t * pnet_box_list = NULL;
    box_t * pnet_box = NULL;

    int width = round( image->w * scale );
    int height = round( image->h * scale );

    dl_matrix3du_t * in = dl_matrix3du_alloc( 1, width, height, image->c );

    for( int i = 0; i < pyramid_times; i++ )
    {
        if( DL_IMAGE_MIN( width, height ) <= config->w )
        {
            break;
        }

        image_resize_linear( in->item, image->item, width, height, in->c, image->w, image->h );

        in->h = height;
        in->w = width;
        in->stride = in->w * in->c;

        #if CONFIG_MTMN_LITE_FLOAT
            out = pnet_lite_f( in );
        #endif

        #if CONFIG_MTMN_LITE_QUANT
            out = pnet_lite_q( in, DL_XTENSA_IMPL );
        #endif

        #if CONFIG_MTMN_HEAVY_QUANT
            out = pnet_heavy_q( in, DL_XTENSA_IMPL );
        #endif

        if( out )
        {
            origin_head[ i ] = image_get_valid_boxes( out->category->item,
                                                      out->offset->item,
                                                      out->category->w,
                                                      out->category->h,
                                                      config->w,
                                                      config->threshold.score,
                                                      scale );

            if( origin_head[ i ] )
            {
                image_sort_insert_by_score( &sorted_list[ i ], origin_head[ i ] );

                image_nms_process( &sorted_list[ i ], 0.5, true );
            }

            dl_matrix3d_free( out->category );
            dl_matrix3d_free( out->offset );
            dl_matrix3d_free( out->landmark );
            free( out );
        }

        scale *= pyramid;
        width = round( image->w * scale );
        height = round( image->h * scale );
    }

    dl_matrix3du_free( in );

    for( int i = 0; i < pyramid_times; i++ )
    {
        image_sort_insert_by_score( &all_box_list, &sorted_list[ i ] );
    }

    image_nms_process( &all_box_list, config->threshold.nms, false );

    if( all_box_list.len )
    {
        if( all_box_list.len > config->threshold.candidate_number )
        {
            all_box_list.len = config->threshold.candidate_number;
        }

        image_calibrate_by_offset( &all_box_list );

        pnet_box_list = ( box_array_t * ) calloc( 1, sizeof( box_array_t ) );

        pnet_box = ( box_t * ) calloc( all_box_list.len, sizeof( box_t ) );

        image_box_t * t = all_box_list.head;

        /* no need to store landmark */
        for( int i = 0; i < all_box_list.len; i++, t = t->next )
        {
            pnet_box[ i ] = t->box;
        }

        pnet_box_list->box = pnet_box;
        pnet_box_list->len = all_box_list.len;
    }

    for( int i = 0; i < pyramid_times; i++ )
    {
        if( origin_head[ i ] )
        {
            free( origin_head[ i ]->origin_head );
            free( origin_head[ i ] );
        }
    }

    free( sorted_list );
    free( origin_head );

    return pnet_box_list;
} /*}}}*/

box_array_t * pnet_forward_fast( dl_matrix3du_t * image,
                                 fptp_t min_face,
                                 int pyramid_times,
                                 net_config_t * config )
{ /*{{{*/
    mtmn_net_t * out;
    fptp_t origin_scale = 1.0f * config->w / min_face;
    fptp_t pyramid = 0.707106781; /* sqrt(0.5) */
    image_list_t * sorted_list = ( image_list_t * ) calloc( pyramid_times, sizeof( image_list_t ) );
    image_list_t ** origin_head = ( image_list_t ** ) calloc( pyramid_times, sizeof( image_list_t * ) );
    image_list_t all_box_list = { NULL };
    box_array_t * pnet_box_list = NULL;
    box_t * pnet_box = NULL;

    int resized_w = round( image->w * origin_scale );
    int resized_h = round( image->h * origin_scale );
    fptp_t resized_scale = origin_scale;
    dl_matrix3du_t * resized_image = dl_matrix3du_alloc( 1, resized_w, resized_h, image->c );

    for( size_t i = 0; i < ( pyramid_times + 1 ) / 2; i++ )
    {
        if( DL_IMAGE_MIN( resized_w, resized_h ) < config->w )
        {
            break;
        }

        if( 0 == i )
        {
            image_resize_linear( resized_image->item,
                                 image->item,
                                 resized_w,
                                 resized_h,
                                 resized_image->c,
                                 image->w,
                                 image->h );
        }
        else
        {
            image_zoom_in_twice( resized_image->item,
                                 resized_w,
                                 resized_h,
                                 resized_image->c,
                                 resized_image->item,
                                 resized_image->w,
                                 resized_image->c );
        }

        resized_image->w = resized_w;
        resized_image->h = resized_h;
        resized_image->stride = resized_image->w * resized_image->c;

        #if CONFIG_MTMN_LITE_FLOAT
            out = pnet_lite_f( resized_image );
        #endif

        #if CONFIG_MTMN_LITE_QUANT
            out = pnet_lite_q( resized_image, DL_XTENSA_IMPL );
        #endif

        #if CONFIG_MTMN_HEAVY_QUANT
            out = pnet_heavy_q( resized_image, DL_XTENSA_IMPL );
        #endif

        if( out )
        {
            origin_head[ i ] = image_get_valid_boxes( out->category->item,
                                                      out->offset->item,
                                                      out->category->w,
                                                      out->category->h,
                                                      config->w,
                                                      config->threshold.score,
                                                      resized_scale );

            if( origin_head[ i ] )
            {
                image_sort_insert_by_score( &sorted_list[ i ], origin_head[ i ] );

                image_nms_process( &sorted_list[ i ], 0.5, true );
            }

            dl_matrix3d_free( out->category );
            dl_matrix3d_free( out->offset );
            dl_matrix3d_free( out->landmark );
            free( out );
        }

        resized_w /= 2;
        resized_h /= 2;
        resized_scale /= 2;
    }

    resized_w = round( image->w * origin_scale * pyramid );
    resized_h = round( image->h * origin_scale * pyramid );
    resized_scale = origin_scale * pyramid;

    for( int i = ( pyramid_times + 1 ) / 2; i < pyramid_times; i++ )
    {
        if( DL_IMAGE_MIN( resized_w, resized_h ) < config->w )
        {
            break;
        }

        if( ( pyramid_times + 1 ) / 2 == i )
        {
            image_resize_linear( resized_image->item,
                                 image->item,
                                 resized_w,
                                 resized_h,
                                 resized_image->c,
                                 image->w,
                                 image->h );
        }
        else
        {
            image_zoom_in_twice( resized_image->item,
                                 resized_w,
                                 resized_h,
                                 resized_image->c,
                                 resized_image->item,
                                 resized_image->w,
                                 resized_image->c );
        }

        resized_image->w = resized_w;
        resized_image->h = resized_h;
        resized_image->stride = resized_image->w * resized_image->c;

        #if CONFIG_MTMN_LITE_FLOAT
            out = pnet_lite_f( resized_image );
        #endif

        #if CONFIG_MTMN_LITE_QUANT
            out = pnet_lite_q( resized_image, DL_XTENSA_IMPL );
        #endif

        #if CONFIG_MTMN_HEAVY_QUANT
            out = pnet_heavy_q( resized_image, DL_XTENSA_IMPL );
        #endif

        if( out )
        {
            origin_head[ i ] = image_get_valid_boxes( out->category->item,
                                                      out->offset->item,
                                                      out->category->w,
                                                      out->category->h,
                                                      config->w,
                                                      config->threshold.score,
                                                      resized_scale );

            if( origin_head[ i ] )
            {
                image_sort_insert_by_score( &sorted_list[ i ], origin_head[ i ] );

                image_nms_process( &sorted_list[ i ], 0.5, true );
            }

            dl_matrix3d_free( out->category );
            dl_matrix3d_free( out->offset );
            dl_matrix3d_free( out->landmark );
            free( out );
        }

        resized_w /= 2;
        resized_h /= 2;
        resized_scale /= 2;
    }

    dl_matrix3du_free( resized_image );

    for( int i = 0; i < pyramid_times; i++ )
    {
        image_sort_insert_by_score( &all_box_list, &sorted_list[ i ] );
    }

    image_nms_process( &all_box_list, config->threshold.nms, false );

    if( all_box_list.len )
    {
        if( all_box_list.len > config->threshold.candidate_number )
        {
            all_box_list.len = config->threshold.candidate_number;
        }

        image_calibrate_by_offset( &all_box_list );

        pnet_box_list = ( box_array_t * ) calloc( 1, sizeof( box_array_t ) );

        pnet_box = ( box_t * ) calloc( all_box_list.len, sizeof( box_t ) );

        image_box_t * t = all_box_list.head;

        /* no need to store landmark */
        for( int i = 0; i < all_box_list.len; i++, t = t->next )
        {
            pnet_box[ i ] = t->box;
        }

        pnet_box_list->box = pnet_box;
        pnet_box_list->len = all_box_list.len;
    }

    for( int i = 0; i < pyramid_times; i++ )
    {
        if( origin_head[ i ] )
        {
            free( origin_head[ i ]->origin_head );
            free( origin_head[ i ] );
        }
    }

    free( sorted_list );
    free( origin_head );

    return pnet_box_list;
} /*}}}*/

box_array_t * rnet_forward( dl_matrix3du_t * image,
                            box_array_t * net_boxes,
                            net_config_t * config )
{ /*{{{*/
    int valid_count = 0;
    image_list_t valid_list = { NULL };
    image_list_t sorted_list = { NULL };
    dl_matrix3du_t * resized_image;
    dl_matrix3du_t * sliced_image;
    image_box_t * valid_box = NULL;
    box_t * net_box = NULL;
    box_array_t * net_box_list = NULL;

    if( NULL == net_boxes )
    {
        return NULL;
    }

    valid_box = ( image_box_t * ) calloc( config->threshold.candidate_number, sizeof( image_box_t ) );
    resized_image = dl_matrix3du_alloc( 1, config->w, config->h, image->c );

    image_rect2sqr( net_boxes, image->w, image->h );

    for( int i = 0; i < net_boxes->len; i++ )
    {
        int x = round( net_boxes->box[ i ].box_p[ 0 ] );
        int y = round( net_boxes->box[ i ].box_p[ 1 ] );
        int w = round( net_boxes->box[ i ].box_p[ 2 ] ) - x + 1;
        int h = round( net_boxes->box[ i ].box_p[ 3 ] ) - y + 1;
        sliced_image = dl_matrix3du_alloc( 1, w, h, image->c );

        dl_matrix3du_slice_copy( sliced_image, image, x, y, w, h );

        image_resize_linear( resized_image->item, sliced_image->item, config->w, config->h, image->c, w, h );

        #if CONFIG_MTMN_LITE_FLOAT
            mtmn_net_t * out = rnet_lite_f_with_score_verify( resized_image, config->threshold.score );
        #endif

        #if CONFIG_MTMN_LITE_QUANT
            mtmn_net_t * out = rnet_lite_q_with_score_verify( resized_image, config->threshold.score, DL_XTENSA_IMPL );
        #endif

        #if CONFIG_MTMN_HEAVY_QUANT
            mtmn_net_t * out = rnet_heavy_q_with_score_verify( resized_image, config->threshold.score, DL_XTENSA_IMPL );
        #endif

        if( out )
        {
            assert( out->category->stride == 2 );
            assert( out->offset->stride == 4 );
            assert( out->offset->c == 4 );
            valid_box[ valid_count ].score = out->category->item[ 1 ];
            valid_box[ valid_count ].box = net_boxes->box[ i ];
            valid_box[ valid_count ].offset.box_p[ 0 ] = out->offset->item[ 0 ];
            valid_box[ valid_count ].offset.box_p[ 1 ] = out->offset->item[ 1 ];
            valid_box[ valid_count ].offset.box_p[ 2 ] = out->offset->item[ 2 ];
            valid_box[ valid_count ].offset.box_p[ 3 ] = out->offset->item[ 3 ];
            valid_box[ valid_count ].next = &( valid_box[ valid_count + 1 ] );
            valid_count++;

            dl_matrix3d_free( out->category );
            dl_matrix3d_free( out->offset );
            free( out );
        }

        dl_matrix3du_free( sliced_image );

        if( valid_count > config->threshold.candidate_number - 1 )
        {
            break;
        }
    }

    dl_matrix3du_free( resized_image );

    if( valid_count )
    {
        valid_box[ valid_count - 1 ].next = NULL;
    }
    else
    {
        valid_box[ 0 ].next = NULL;
    }

    valid_list.head = valid_box;
    valid_list.len = valid_count;
    image_sort_insert_by_score( &sorted_list, &valid_list );

    image_nms_process( &sorted_list, config->threshold.nms, false );

    if( sorted_list.len )
    {
        image_calibrate_by_offset( &sorted_list );

        net_box_list = ( box_array_t * ) calloc( 1, sizeof( box_array_t ) );
        net_box = ( box_t * ) calloc( sorted_list.len, sizeof( box_t ) );

        image_box_t * t = sorted_list.head;

        for( int i = 0; i < sorted_list.len; i++, t = t->next )
        {
            net_box[ i ] = t->box;
        }

        net_box_list->box = net_box;
        net_box_list->len = sorted_list.len;
    }

    free( valid_box );

    return net_box_list;
} /*}}}*/

box_array_t * onet_forward( dl_matrix3du_t * image,
                            box_array_t * net_boxes,
                            net_config_t * config )
{ /*{{{*/
    int valid_count = 0;
    image_list_t valid_list = { NULL };
    image_list_t sorted_list = { NULL };
    dl_matrix3du_t * resized_image;
    dl_matrix3du_t * sliced_image;
    image_box_t * valid_box = NULL;
    box_t * net_box = NULL;
    fptp_t * net_score = NULL;
    landmark_t * net_landmark = NULL;
    box_array_t * net_box_list = NULL;

    if( NULL == net_boxes )
    {
        return NULL;
    }

    valid_box = ( image_box_t * ) calloc( config->threshold.candidate_number, sizeof( image_box_t ) );
    resized_image = dl_matrix3du_alloc( 1, config->w, config->h, image->c );

    image_rect2sqr( net_boxes, image->w, image->h );

    for( int i = 0; i < net_boxes->len; i++ )
    {
        int x = round( net_boxes->box[ i ].box_p[ 0 ] );
        int y = round( net_boxes->box[ i ].box_p[ 1 ] );
        int w = round( net_boxes->box[ i ].box_p[ 2 ] ) - x + 1;
        int h = round( net_boxes->box[ i ].box_p[ 3 ] ) - y + 1;
        sliced_image = dl_matrix3du_alloc( 1, w, h, image->c );

        dl_matrix3du_slice_copy( sliced_image, image, x, y, w, h );

        image_resize_linear( resized_image->item, sliced_image->item, config->w, config->h, image->c, w, h );

        #if CONFIG_MTMN_LITE_FLOAT
            mtmn_net_t * out = onet_lite_f_with_score_verify( resized_image, config->threshold.score );
        #endif

        #if CONFIG_MTMN_LITE_QUANT
            mtmn_net_t * out = onet_lite_q_with_score_verify( resized_image, config->threshold.score, DL_XTENSA_IMPL );
        #endif

        #if CONFIG_MTMN_HEAVY_QUANT
            mtmn_net_t * out = onet_heavy_q_with_score_verify( resized_image, config->threshold.score, DL_XTENSA_IMPL );
        #endif

        if( out )
        {
            assert( out->category->stride == 2 );
            assert( out->offset->stride == 4 );
            assert( out->offset->c == 4 );
            valid_box[ valid_count ].score = out->category->item[ 1 ];
            valid_box[ valid_count ].box = net_boxes->box[ i ];
            valid_box[ valid_count ].offset.box_p[ 0 ] = out->offset->item[ 0 ];
            valid_box[ valid_count ].offset.box_p[ 1 ] = out->offset->item[ 1 ];
            valid_box[ valid_count ].offset.box_p[ 2 ] = out->offset->item[ 2 ];
            valid_box[ valid_count ].offset.box_p[ 3 ] = out->offset->item[ 3 ];
            assert( out->landmark->stride == 10 );
            memcpy( &( valid_box[ valid_count ].landmark ), out->landmark->item, sizeof( landmark_t ) );
            valid_box[ valid_count ].next = &( valid_box[ valid_count + 1 ] );
            valid_count++;

            dl_matrix3d_free( out->category );
            dl_matrix3d_free( out->offset );
            dl_matrix3d_free( out->landmark );
            free( out );
        }

        dl_matrix3du_free( sliced_image );

        if( valid_count > config->threshold.candidate_number - 1 )
        {
            break;
        }
    }

    dl_matrix3du_free( resized_image );

    if( valid_count )
    {
        valid_box[ valid_count - 1 ].next = NULL;
    }
    else
    {
        valid_box[ 0 ].next = NULL;
    }

    valid_list.head = valid_box;
    valid_list.len = valid_count;
    image_sort_insert_by_score( &sorted_list, &valid_list );

    if( sorted_list.len )
    {
        image_landmark_calibrate( &sorted_list );

        image_calibrate_by_offset( &sorted_list );

        image_nms_process( &sorted_list, config->threshold.nms, false );

        net_box_list = ( box_array_t * ) calloc( 1, sizeof( box_array_t ) );
        net_box = ( box_t * ) calloc( sorted_list.len, sizeof( box_t ) );
        net_score = ( fptp_t * ) calloc( sorted_list.len, sizeof( fptp_t ) );
        net_landmark = ( landmark_t * ) calloc( sorted_list.len, sizeof( landmark_t ) );

        image_box_t * t = sorted_list.head;

        for( int i = 0; i < sorted_list.len; i++, t = t->next )
        {
            net_box[ i ] = t->box;
            net_score[ i ] = t->score;
            net_landmark[ i ] = t->landmark;
        }

        net_box_list->score = net_score;
        net_box_list->box = net_box;
        net_box_list->landmark = net_landmark;
        net_box_list->len = sorted_list.len;
    }

    free( valid_box );

    return net_box_list;
} /*}}}*/
box_array_t * face_detect( dl_matrix3du_t * image_matrix,
                           mtmn_config_t * config )
{ /*{{{*/
    net_config_t pnet_config = { 0 };

    pnet_config.w = 12;
    pnet_config.h = 12;
    pnet_config.threshold = config->p_threshold;

    box_array_t * pnet_boxes = NULL;

    if( FAST == config->type )
    {
        pnet_boxes = pnet_forward_fast( image_matrix,
                                        config->min_face,
                                        config->pyramid_times,
                                        &pnet_config );
    }
    else if( NORMAL == config->type )
    {
        pnet_boxes = pnet_forward2( image_matrix,
                                    config->min_face,
                                    config->pyramid,
                                    config->pyramid_times,
                                    &pnet_config );
    }

    if( NULL == pnet_boxes )
    {
        return NULL;
    }

    net_config_t rnet_config = { 0 };
    rnet_config.w = 24;
    rnet_config.h = 24;
    rnet_config.threshold = config->r_threshold;

    box_array_t * rnet_boxes = rnet_forward( image_matrix,
                                             pnet_boxes,
                                             &rnet_config );

    free( pnet_boxes->box );
    free( pnet_boxes );

    if( NULL == rnet_boxes )
    {
        return NULL;
    }

    net_config_t onet_config = { 0 };
    onet_config.w = 48;
    onet_config.h = 48;
    onet_config.threshold = config->o_threshold;

    box_array_t * onet_boxes = onet_forward( image_matrix,
                                             rnet_boxes,
                                             &onet_config );

    free( rnet_boxes->box );
    free( rnet_boxes );

    return onet_boxes;
} /*}}}*/
