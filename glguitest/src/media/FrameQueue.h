#pragma once
#include <mutex>
#include <queue>

#include <libavformat/avformat.h>

class FrameQueue {
  public:
    void PushFreeFrame(AVFrame *frame) {
        std::lock_guard<std::mutex> lk(queue_mtx_);
        free_frame_list_.push(frame);
    }

    AVFrame *PopFreeFrame() {
        std::lock_guard<std::mutex> lk(queue_mtx_);

        if (free_frame_list_.size() <= 0) return nullptr;

        AVFrame *frame = free_frame_list_.front();
        free_frame_list_.pop();
        return frame;
    }

    void PushUsedFrame(AVFrame *frame) {
        std::lock_guard<std::mutex> lk(queue_mtx_);
        used_frame_list_.push(frame);
    }

    AVFrame *PopUsedFrame() {
        std::lock_guard<std::mutex> lk(queue_mtx_);

        if (used_frame_list_.size() <= 0) return nullptr;

        AVFrame *frame = used_frame_list_.front();
        used_frame_list_.pop();
        return frame;
    }

  private:
    std::mutex queue_mtx_;
    std::queue<AVFrame *> free_frame_list_;
    std::queue<AVFrame *> used_frame_list_;
};
