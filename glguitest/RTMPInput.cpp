#include "RTMPInput.h"

// #include <Windows.h>
// #include <WINGDI.h>
#include <stdio.h>
#include <string>
// 
// bool DumpBmp(const char *filename, uint8_t *pRGBBuffer, int width, int height, int bpp) {
//     BITMAPFILEHEADER bmpheader;
//     BITMAPINFOHEADER bmpinfo;
//     FILE *fp = NULL;
// 
//     fp = fopen(filename, "wb");
//     if (fp == NULL) {
//         return false;
//     }
// 
//     bmpheader.bfType = ('M' << 8) | 'B';
//     bmpheader.bfReserved1 = 0;
//     bmpheader.bfReserved2 = 0;
//     bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//     bmpheader.bfSize = bmpheader.bfOffBits + width * height * bpp / 8;
// 
//     bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
//     bmpinfo.biWidth = width;
//     bmpinfo.biHeight = 0 - height;
//     bmpinfo.biPlanes = 1;
//     bmpinfo.biBitCount = bpp;
//     bmpinfo.biCompression = BI_RGB;
//     bmpinfo.biSizeImage = 0;
//     bmpinfo.biXPelsPerMeter = 100;
//     bmpinfo.biYPelsPerMeter = 100;
//     bmpinfo.biClrUsed = 0;
//     bmpinfo.biClrImportant = 0;
// 
//     fwrite(&bmpheader, sizeof(BITMAPFILEHEADER), 1, fp);
//     fwrite(&bmpinfo, sizeof(BITMAPINFOHEADER), 1, fp);
//     fwrite(pRGBBuffer, width * height * bpp / 8, 1, fp);
//     fclose(fp);
//     fp = NULL;
// 
//     return true;
// }

RTMPInput::RTMPInput() {}

RTMPInput::~RTMPInput() {}

bool RTMPInput::StreamOpen() {
    av_register_all();
    avformat_network_init();

    int ret = avformat_open_input(&ctx_, "rtmp://10.11.0.189/live/cartoon", nullptr, nullptr);
    if (ret > 0) {
        printf("avformat_open_input fail\n");
        return false;
    }

    printf("open success\n");
    if (avformat_find_stream_info(ctx_, 0) >= 0) {
        av_dump_format(ctx_, 0, ctx_->filename, 0);
    }

    for (int stream_index = 0; stream_index < ctx_->nb_streams; ++stream_index) {
        AVStream *st = ctx_->streams[stream_index];

        if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index_ = stream_index;
            avctx_ = avcodec_alloc_context3(NULL);
            ret = avcodec_parameters_to_context(avctx_, st->codecpar);
            if (ret > 0) {
                printf("avcodec_parameters_to_context fail\n");
                return false;
            }

            swsctx_ = sws_getContext(avctx_->width, avctx_->height, avctx_->pix_fmt, avctx_->width,
                                     avctx_->height, AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL,
                                     NULL, NULL);

            printf("{%d,%d}\t{%d,%d}\n", avctx_->pkt_timebase.num, avctx_->pkt_timebase.den,
                   st->time_base.num, st->time_base.den);

            codec_ = avcodec_find_decoder(avctx_->codec_id);

            ret = avcodec_open2(avctx_, codec_, nullptr);
            if (ret > 0) {
                printf("avcodec_open2 fail\n");
                return false;
            }
        }
    }

    read_thread_ = new std::thread(&RTMPInput::ReadThread, this);
}

void RTMPInput::ReadThread() {
    AVPacket pkt;
    AVFrame *frame = av_frame_alloc();

    AVFrame *frame_out = av_frame_alloc();
    int buffer_size = av_image_get_buffer_size(AV_PIX_FMT_BGR24, avctx_->width, avctx_->height, 1);
    printf("av_image_get_buffer_size %d\n", buffer_size);
    uint8_t *out_buffer = (uint8_t *) av_malloc(buffer_size);
    av_image_fill_arrays(frame_out->data, frame_out->linesize, out_buffer, AV_PIX_FMT_BGR24,
                         avctx_->width, avctx_->height, 1);

    printf("frame_out->width %d frame_out->linesize %d %d %d %d\n", avctx_->width,
           frame_out->linesize[0], frame_out->linesize[1], frame_out->linesize[2],
           frame_out->linesize[3]);

    av_init_packet(&pkt);

    int i = 0;
    while (1) {
        av_read_frame(ctx_, &pkt);

        if (pkt.stream_index == video_index_) {
            avcodec_send_packet(avctx_, &pkt);
            int ret = avcodec_receive_frame(avctx_, frame);
            if (ret == 0) {
                sws_scale(swsctx_, frame->data, frame->linesize, 0, frame->height, frame_out->data,
                          frame_out->linesize);

                char save_file[50] = "DumpTest%d.bmp";
                sprintf(save_file, std::to_string(++i).c_str());
                // DumpBmp(save_file, out_buffer, frame->width, frame->height, 32);
            }
        }
    }

    av_free(out_buffer);
    av_frame_free(&frame_out);
    av_frame_free(&frame);
}
