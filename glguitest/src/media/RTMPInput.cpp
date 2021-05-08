#include "RTMPInput.h"

#include <stdio.h>
#include <string>

RTMPInput::RTMPInput() {}

RTMPInput::~RTMPInput() {}

bool RTMPInput::StreamOpen(const std::string &url) {
    url_ = url;
    av_register_all();
    avformat_network_init();

    int ret = avformat_open_input(&ctx_, url.c_str(), nullptr, nullptr);
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
                                     avctx_->height, AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL,
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

    for (int i = 0; i < 20; i++) {
        AVFrame *frame = av_frame_alloc();

        int buffer_size =
            av_image_get_buffer_size(AV_PIX_FMT_RGB24, avctx_->width, avctx_->height, 1);
        printf("av_image_get_buffer_size %d\n", buffer_size);
        uint8_t *out_buffer = (uint8_t *) av_malloc(buffer_size);
        av_image_fill_arrays(frame->data, frame->linesize, out_buffer, AV_PIX_FMT_RGB24,
                             avctx_->width, avctx_->height, 1);

        free_frame_list_.push(frame);
    }

    read_thread_ = new std::thread(&RTMPInput::ReadThread, this);
}

AVFrame *RTMPInput::BeginUsedFrame() {
    std::lock_guard<std::mutex> lk(queue_mtx_);

    if (used_frame_list_.size() <= 0) return nullptr;

    AVFrame *frame = used_frame_list_.front();
    used_frame_list_.pop();
    return frame;
}

void RTMPInput::EndUsedFrame(AVFrame *frame) { 
    free_frame_list_.push(frame);
}

void RTMPInput::ReadThread() {
    AVPacket pkt;

    AVFrame *frame = av_frame_alloc();
//     int buffer_size = av_image_get_buffer_size(AV_PIX_FMT_BGR24, avctx_->width, avctx_->height, 1);
//     printf("av_image_get_buffer_size %d\n", buffer_size);
//     uint8_t *out_buffer = (uint8_t *) av_malloc(buffer_size);
//     av_image_fill_arrays(frame->data, frame->linesize, out_buffer, AV_PIX_FMT_BGR24,
//                          avctx_->width, avctx_->height, 1);

    printf("frame_out->width %d frame_out->linesize %d %d %d %d\n", avctx_->width,
           frame->linesize[0], frame->linesize[1], frame->linesize[2], frame->linesize[3]);

    av_init_packet(&pkt);

    int i = 0;
    while (1) {
        av_read_frame(ctx_, &pkt);

        if (pkt.stream_index == video_index_) {
            avcodec_send_packet(avctx_, &pkt);

            while (avcodec_receive_frame(avctx_, frame) >= 0) {

                std::lock_guard<std::mutex> lk(queue_mtx_);
                if (free_frame_list_.size() > 0) {

                    AVFrame *frame_out = free_frame_list_.front();
                    free_frame_list_.pop();

                    printf("frame pts:%lld, dts:%lld \n", frame->pts, frame->pkt_dts);

                    sws_scale(swsctx_, frame->data, frame->linesize, 0, frame->height,
                              frame_out->data, frame_out->linesize);

                    frame_out->width = frame->width;
                    frame_out->height = frame->height;

                    used_frame_list_.push(frame_out);
                } else {
                    printf("free_frame_list_ null\n");
                }
            }
        }
    }

//     av_free(out_buffer);
    av_frame_free(&frame);
}
