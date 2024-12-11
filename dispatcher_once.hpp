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
    DispatcherOnce *dispatcher_ref = nullptr;
    int id;
public:
    explicit Worker(std::function<void(Chunk, std::shared_ptr<WorkState> &state)> cb, std::shared_ptr<WorkState> s)
            : callback(std::move(cb)), state(std::move(s)) {}

    void Run() {
        for (;;) {
            bool should_steal = false;
            Chunk chunk;
            {
                std::unique_lock<std::mutex> lock{this->mtx};
                std::cout << "starting running " << this->id << std::endl;
                this->condition.wait(lock, [this] { return !this->q.empty() || this->quit; });
                chunk = this->q.front();
                this->q.pop();
                if (this->q.empty()) {
                    should_steal = true;
                }
            }
            callback(chunk, state);
            if (should_steal) {
                if (dispatcher_ref != nullptr) {
                    std::cout << "wake up " << this->id << std::endl;
                    std::unique_lock<std::mutex> lock{dispatcher_ref->queue_mtx};
                    dispatcher_ref->idle_workers.push(this->id);
                    dispatcher_ref->finished_worker_count++;
                    dispatcher_ref->condition.notify_all();
                }
                return;
            }
        }
    }
};

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

    CalculateSplitNum();
    D();
    for (auto &&w: workers) {
        w->quit = false;
        w->condition.notify_all();
    }
    while (true) {
        std::unique_lock<std::mutex> lock{this->mtx};
        std::cout << this->finished_worker_count << " " << workers.size() << std::endl;
        this->condition.wait(lock, [this] { return this->finished_worker_count == workers.size(); });
        auto front = idle_workers.front();
        idle_workers.pop();
        std::cout << "release " << front << std::endl;
        if (this->finished_worker_count == workers.size()) {
            std::cout << "all worker finished" << std::endl;
            return;
        }
    }


}


class CharCountState : public WorkState {
public:
    char ch{'a'};
    int sum = 0;
};

