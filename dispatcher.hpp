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

constexpr int chunk_size = 4096 * 1024;
constexpr int chunk_num = 100;

struct Chunk {
    std::vector<char> data;

    Chunk() {
        data = std::vector<char>(chunk_size);
    }
};

class Worker;

class DispatcherOneByOne {
    std::list<Chunk> chunks;
    std::vector<std::shared_ptr<Worker>> workers;
    bool quit = false;
public:
    DispatcherOneByOne() {
        /// 1024 * 4096 * 100 = 400MB
        chunks = std::list<Chunk>(chunk_num);
        for (auto &&i : chunks) {
            for (int j = 0; j < 4096; j++) {
                i.data[j] = 'a' + rand() % 26;
            }
        }
    }

    /// 分发数据
    void D(std::shared_ptr<Worker> w);
    void AppendWorker(std::shared_ptr<Worker> w) {
        workers.emplace_back(std::move(w));
    }

    inline bool empty() const {
        return chunks.empty();
    }

    void Run();
};

class DispatcherOnce {
    std::vector<Chunk> chunks;
    std::vector<std::shared_ptr<Worker>> workers;
    bool quit = false;
public:
    DispatcherOnce() {
        /// 1024 * 4096 * 100 = 400MB
        chunks = std::vector<Chunk>(chunk_num);
        for (auto &&i : chunks) {
            for (int j = 0; j < 4096; j++) {
                i.data[j] = 'a' + rand() % 26;
            }
        }
    }

    void D(std::shared_ptr<Worker> w);
    void AppendWorker(std::shared_ptr<Worker> w) {
        workers.emplace_back(std::move(w));
    }

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
    friend class DispatcherOneByOne;
    std::mutex mtx;
    std::queue<Chunk> q;
    std::function<void(Chunk, std::shared_ptr<WorkState>& state)> callback;
    std::shared_ptr<WorkState> state;
    std::condition_variable condition;
    bool quit = false;
public:
    explicit Worker(std::function<void(Chunk, std::shared_ptr<WorkState>& state)> cb, std::shared_ptr<WorkState> s): callback(std::move(cb)), state(std::move(s)) {}

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
            }
            callback(chunk, state);
        }
    }
};

inline void DispatcherOneByOne::D(std::shared_ptr<Worker> w) {
    if (chunks.empty()) {
        return;
    }
    {
        std::unique_lock<std::mutex> lock{w->mtx};
        w->q.push(chunks.front());
        chunks.pop_front();
    }

    w->condition.notify_all();
}

inline void DispatcherOneByOne::Run() {
    /// 负责分发数据
    while (!quit) {
        for (auto &&w : workers) {
            D(w);
            if (chunks.empty()) {
                break;
            }
        }
        if (chunks.empty()) {
            for (auto &&w : workers) {
                w->quit = true;
                w->condition.notify_all();
            }
            quit = true;
            break;
        }
    }
}


class CharCountState : public WorkState {
public:
    char ch {'a'};
    int sum = 0;
};

