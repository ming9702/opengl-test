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

            if (CreateDecoder(&vd_, st)) {
                swsctx_ = sws_getContext(vd_.avctx->width, vd_.avctx->height, vd_.avctx->pix_fmt,
                                         vd_.avctx->width, vd_.avctx->height, AV_PIX_FMT_RGB24,
                                         SWS_FAST_BILINEAR, NULL, NULL, NULL);

                printf("{%d,%d}\t{%d,%d}\n", vd_.avctx->pkt_timebase.num,
                       vd_.avctx->pkt_timebase.den, st->time_base.num, st->time_base.den);
            }

        } else if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_index_ = stream_index;

            CreateDecoder(&ad_, st);
        }
    }

    for (int i = 0; i < 20; i++) {
        AVFrame *frame = av_frame_alloc();

        int buffer_size =
            av_image_get_buffer_size(AV_PIX_FMT_RGB24, vd_.avctx->width, vd_.avctx->height, 1);
        printf("av_image_get_buffer_size %d\n", buffer_size);
        uint8_t *out_buffer = (uint8_t *) av_malloc(buffer_size);
        av_image_fill_arrays(frame->data, frame->linesize, out_buffer, AV_PIX_FMT_RGB24,
                             vd_.avctx->width, vd_.avctx->height, 1);

        frame_vq_.PushFreeFrame(frame);
    }

    read_thread_ = new std::thread(&RTMPInput::ReadThread, this);
}

AVFrame *RTMPInput::BeginUsedFrame() { return frame_vq_.PopUsedFrame(); }

void RTMPInput::EndUsedFrame(AVFrame *frame) { frame_vq_.PushFreeFrame(frame); }

bool RTMPInput::CreateDecoder(Decoder *d, AVStream *st) {
    AVCodecContext *avctx = avcodec_alloc_context3(NULL);

    int ret = avcodec_parameters_to_context(avctx, st->codecpar);
    if (ret > 0) {
        printf("avcodec_parameters_to_context fail\n");
        return false;
    }

    AVCodec *codec = avcodec_find_decoder(avctx->codec_id);

    ret = avcodec_open2(avctx, codec, nullptr);
    if (ret > 0) {
        printf("avcodec_open2 fail\n");
        return false;
    }

    d->avctx = avctx;
    d->codec_ = codec;

    return true;
}

void RTMPInput::ReadThread() {
    AVPacket pkt;

    AVFrame *frame = av_frame_alloc();
    //     int buffer_size = av_image_get_buffer_size(AV_PIX_FMT_BGR24, avctx_->width,
    //     avctx_->height, 1); printf("av_image_get_buffer_size %d\n", buffer_size); uint8_t
    //     *out_buffer = (uint8_t *) av_malloc(buffer_size); av_image_fill_arrays(frame->data,
    //     frame->linesize, out_buffer, AV_PIX_FMT_BGR24,
    //                          avctx_->width, avctx_->height, 1);

    printf("frame_out->width %d frame_out->linesize %d %d %d %d\n", vd_.avctx->width,
           frame->linesize[0], frame->linesize[1], frame->linesize[2], frame->linesize[3]);

    av_init_packet(&pkt);

    while (1) {
        av_read_frame(ctx_, &pkt);

        if (pkt.stream_index == video_index_) {
            avcodec_send_packet(vd_.avctx, &pkt);

            while (avcodec_receive_frame(vd_.avctx, frame) >= 0) {

                if (AVFrame *frame_out = frame_vq_.PopFreeFrame()) {

                    printf("frame pts:%lld, dts:%lld \n", frame->pts, frame->pkt_dts);

                    sws_scale(swsctx_, frame->data, frame->linesize, 0, frame->height,
                              frame_out->data, frame_out->linesize);

                    frame_out->width = frame->width;
                    frame_out->height = frame->height;

                    frame_vq_.PushUsedFrame(frame_out);

                } else {
                    printf("free_frame_list_ null lost one frame\n");
                }
            }
        } else if (pkt.stream_index == audio_index_) {
            avcodec_send_packet(ad_.avctx, &pkt);
            while (avcodec_receive_frame(ad_.avctx, frame) >= 0) {
                printf("audio frame pts:%lld, dts:%lld \n", frame->pts, frame->pkt_dts);
            }
        }
    }

    //     av_free(out_buffer);
    av_frame_free(&frame);
}
