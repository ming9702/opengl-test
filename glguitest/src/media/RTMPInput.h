#pragma once
#include <mutex>
#include <queue>
#include <string>
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
    bool StreamOpen(const std::string &url);
    AVFrame *BeginUsedFrame();
    void EndUsedFrame(AVFrame *frame);

  private:
    void ReadThread();

  private:
    std::string url_;
    std::thread *read_thread_{nullptr};

    AVFormatContext *ctx_{nullptr};

    AVCodecContext *avctx_{nullptr};
    AVCodec *codec_{nullptr};
    int video_index_ = -1;

    SwsContext *swsctx_{nullptr};

    std::mutex queue_mtx_;
    std::queue<AVFrame *> free_frame_list_;
    std::queue<AVFrame *> used_frame_list_;
};
