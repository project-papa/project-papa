#ifndef PI_SUBSCRIPTIONS_H
#define PI_SUBSCRIPTIONS_H

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <iostream>
#include <atomic>
#include <mutex>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "scope_reader.h"
#include "server.h"

using namespace rapidjson;

class pi_subscriptions {
    public:
        pi_subscriptions(server* ws, int shm_port=4556) : ws(ws), stop(false) {
            reader.reset(new scope_reader(shm_port));
        }

        ~pi_subscriptions() {
            stop = true;
            join();
        }

        void update_scopes(std::vector<unsigned int>& new_scopes) {
            scope_mutex.lock();
            scopes = new_scopes;
            scope_mutex.unlock();
        }

        void run() {
            std::cout << "Scope reader started\n";

            thread = std::thread([this] () {
                while (!stop) {
                    int delay = 16;

                    Document d;
                    d.SetObject();
                    auto& alloc = d.GetAllocator();

                    d.AddMember("message_type", "scope/amp", alloc);

                    Value a;
                    a.SetObject();

                    scope_mutex.lock();

                    if (scopes.size() == 0) {
                        scope_mutex.unlock();
                        continue;
                    }

                    for (int i = 0; i < scopes.size(); i++) {
                        stereo_data data = reader->read_scope(scopes[i]);

                        double acc = 0;
                        if (data.size() > 0) {
                            for (size_t j = 0; j < data[0].size(); j++) {
                                acc += data[0][j] * data[0][j];
                            }

                            acc /= data[0].size();
                            acc = sqrt(acc);
                        }

                        Value num = Value(std::to_string(scopes[i]).c_str(), alloc);
                        a.AddMember(num, Value(acc).Move(), alloc);
                    }

                    d.AddMember("data", a, alloc);

                    scope_mutex.unlock();

                    StringBuffer buffer;
                    Writer<StringBuffer> writer(buffer);
                    d.Accept(writer);

                    std::string json_string(buffer.GetString());
                    ws->broadcast_message(json_string);

                    #ifdef _WIN32
                    Sleep(delay);
                    #else
                    usleep(delay*1000);
                    #endif
                }
            });
        }

        void join() {
            thread.join();
        }
    private:
        std::atomic<bool> stop;
        std::thread thread;

        std::unique_ptr<scope_reader> reader;
        std::vector<unsigned int> scopes;
        std::mutex scope_mutex;

        server* ws;
};

#endif
