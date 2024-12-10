#include "../dispatcher.hpp"
#include <cassert>
#include <memory>
#include <thread>
#include <iostream>

#define _D_ASSERT(x, line) if (!(x)) { std::cout << "Assertion failed at line " << line << std::endl; assert(false); }
#define D_ASSERT(x) _D_ASSERT(x, __LINE__)

struct WorkThreadContext {
    std::shared_ptr<WorkState> state;
    std::shared_ptr<Worker> worker;
    int id{};

    WorkThreadContext() : state(std::make_shared<CharCountState>()) {
        auto func = [](const Chunk &ch, std::shared_ptr<WorkState> &state) {
            std::shared_ptr<CharCountState> ch_state = std::static_pointer_cast<CharCountState>(state);
            for (auto i: ch.data) {
                if (i == ch_state->ch) {
                    ch_state->sum++;
                }
            }
        };
        this->worker = std::make_shared<Worker>(func, state);
    }

    void Print() {
        std::cout << "id: " << id << ", sum: " << std::static_pointer_cast<CharCountState>(state)->sum << std::endl;
    }
};

void DispatchWork() {
    DispatcherOneByOne dispatcher;
    std::vector<WorkThreadContext> contexts;
    for (int i = 0; i < 10; i++) {
        contexts.emplace_back();
        contexts.back().id = i;
    }

    for (auto &context: contexts) {
        dispatcher.AppendWorker(context.worker);
    }

//    std::thread worker_thread([&contexts](){
//        for (auto& context : contexts) {
//            context.worker->Run();
//        }
//    });
    std::vector<std::thread> threads;

    for (auto i = 0; i < 10; ++i) {
        threads.emplace_back([&contexts, i]() {
            contexts[i].worker->Run();
        });
    }

    std::thread dispatcher_thread([&dispatcher]() {
        dispatcher.Run();
    });

//    worker_thread.join();
    for (auto &thread: threads) {
        thread.join();
    }
    dispatcher_thread.join();

    for (auto &context: contexts) {
        context.Print();
    }
}

int main() {
    DispatchWork();
}