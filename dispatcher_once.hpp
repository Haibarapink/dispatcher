#pragma once

#include <random>
#include <vector>
#include <fstream>
#include <cstring>
#include <mutex>
#include <queue>
#include <functional>
#include <memory>
#include <list>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <cassert>

constexpr int chunk_size = 4096 * 1024;
constexpr int chunk_num = 100;

struct Chunk {
    std::vector<char> data;

    Chunk() {
        data = std::vector<char>(chunk_size);
    }
};

class Worker;

class DispatcherOnce {
    friend class Worker;

    std::vector<Chunk> chunks;
    std::vector<std::shared_ptr<Worker>> workers;
    bool quit = false;
    size_t split_num = 0;
    size_t one_worker_chunk_count = 0;
    std::atomic_int finished_worker_count = 0;
    std::condition_variable condition;
    std::mutex mtx;
    /// 用来steal数据
    std::mutex queue_mtx;
    std::queue<size_t> idle_workers;
    int steal_worker = 0;
public:
    DispatcherOnce() {
        /// 1024 * 4096 * 100 = 400MB
        chunks = std::vector<Chunk>(chunk_num);
        for (auto &&i: chunks) {
            for (int j = 0; j < 4096; j++) {
                i.data[j] = 'a' + rand() % 26;
            }
        }
    }

    /// 计算分发数据的数量
    inline void CalculateSplitNum() {
        size_t worker_num = workers.size();
        split_num = chunk_num / worker_num;
        one_worker_chunk_count = chunk_num / worker_num;
    }

    /// 分发数据
    void AppendWorker(std::shared_ptr<Worker> w) {
        workers.emplace_back(std::move(w));
    }

    void D();

    inline bool empty() const {
        return chunks.empty();
    }

    void Run();
};


class WorkState {
public:
    std::atomic_bool done;
};

class Worker {
    friend class DispatcherOnce;
    std::mutex mtx;
    std::queue<Chunk> q;
    std::function<void(Chunk, std::shared_ptr<WorkState> &state)> callback;
    std::shared_ptr<WorkState> state;
    std::condition_variable condition;
    bool quit = false;
public:
    DispatcherOnce* dispatcher_ref = nullptr;
public:
    explicit Worker(std::function<void(Chunk, std::shared_ptr<WorkState> &state)> cb, std::shared_ptr<WorkState> s)
            : callback(std::move(cb)), state(std::move(s)) {}

    void Run() {
        for (;;) {
            Chunk chunk;
            {
                std::unique_lock<std::mutex> lock{this->mtx};
                this->condition.wait(lock, [this] { return !this->q.empty() || this->quit; });
                if (this->quit && this->q.empty()) {
                    return;
                }
                chunk = this->q.front();
                this->q.pop();
                if (q.empty()) {
                    assert(dispatcher_ref != nullptr);
                }
            }
            callback(chunk, state);
        }
    }
};

//inline void DispatcherOnce::D(std::shared_ptr<Worker> w) {
//    if (chunks.empty()) {
//        return;
//    }
//    {
//        std::unique_lock<std::mutex> lock{w->mtx};
//        w->q.push(chunks.front());
//        chunks.pop_front();
//    }
//
//    w->condition.notify_all();
//}

inline void DispatcherOnce::D() {
    for (auto i = 0; i < workers.size(); ++i) {
        if (this->chunks.size() < one_worker_chunk_count) {
            for (auto j = 0; j < this->chunks.size(); ++j) {
                workers[i]->q.push(this->chunks[j]);
            }
            this->chunks.clear();
        } else {
            for (auto j = 0; j < one_worker_chunk_count; ++j) {
                workers[i]->q.push(this->chunks[j]);
            }
            this->chunks.erase(this->chunks.begin(), this->chunks.begin() + one_worker_chunk_count);
        }
    }
}

inline void DispatcherOnce::Run() {
    /// 负责分发数据
//    while (!quit) {
//        for (auto &&w : workers) {
//            D(w);
//            if (chunks.empty()) {
//                break;
//            }
//        }
//        if (chunks.empty()) {
//            for (auto &&w : workers) {
//                w->quit = true;
//                w->condition.notify_all();
//            }
//            quit = true;
//            break;
//        }
//    }
    CalculateSplitNum();
    D();
    for (auto &&w: workers) {
        w->quit = false;
        w->condition.notify_all();
    }

    /// 接受steal数据请求
    for (;;) {
        int idle = -1;
        {
            std::unique_lock<std::mutex> lock{this->mtx};
            this->condition.wait(lock, [this] { return !this->idle_workers.empty() || this->quit; });
            if (this->quit) {
                return;
            }
            idle = this->idle_workers.front();
            this->idle_workers.pop();
            /// TODO steal
            std::cout << "idle " << idle << std::endl;
            steal_worker++;
            if (steal_worker == workers.size()) {
                quit = true;
                for (auto &&w : workers) {
                    w->quit = true;
                    w->condition.notify_all();
                }
                return;
            }
        }
    }


}


class CharCountState : public WorkState {
public:
    char ch{'a'};
    int sum = 0;
};

