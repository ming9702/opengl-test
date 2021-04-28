#pragma once
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

class RTMPInput {
  public:
    RTMPInput();
    ~RTMPInput();

  public:
    bool StreamOpen();

  private:
    void ReadThread();

  private:
    std::thread *read_thread_{nullptr};

    AVFormatContext *ctx_{nullptr};

    AVCodecContext *avctx_{nullptr};
    AVCodec *codec_{nullptr};
    int video_index_ = -1;

    SwsContext *swsctx_{nullptr};
};
