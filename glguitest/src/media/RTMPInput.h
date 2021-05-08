#pragma once
#include <string>
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include "FrameQueue.h"

struct Decoder {
    AVCodecContext *avctx{nullptr};
    AVCodec *codec_{nullptr};
};

class RTMPInput {
  public:
    RTMPInput();
    ~RTMPInput();

  public:
    bool StreamOpen(const std::string &url);
    AVFrame *BeginUsedFrame();
    void EndUsedFrame(AVFrame *frame);

  private:
    bool CreateDecoder(Decoder *d, AVStream *st);
    void ReadThread();

  private:
    std::string url_;
    std::thread *read_thread_{nullptr};

    AVFormatContext *ctx_{nullptr};

    Decoder vd_;
    Decoder ad_;

    int video_index_ = -1;
    int audio_index_ = -1;

    SwsContext *swsctx_{nullptr};

    FrameQueue frame_vq_;
    FrameQueue frame_aq_;
};
