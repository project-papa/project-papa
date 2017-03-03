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
            _scopes = new_scopes;
            scope_mutex.unlock();
        }

        std::vector<unsigned int> get_scopes() {
            scope_mutex.lock();
            std::vector<unsigned int> scopes_copy = _scopes;
            scope_mutex.unlock();
            return scopes_copy;
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

                    std::vector<unsigned int> scopes = get_scopes();
                    if (scopes.size() == 0) {
                        continue;
                    }

                    bool has_members = false;

                    for (int i = 0; i < scopes.size(); i++) {
                        stereo_data data = reader->read_scope(scopes[i]);

                        if (data.size() == 0) {
                            continue;
                        }
                        else {
                            has_members = true;
                        }

                        double acc = 0;
                        size_t total = 0;
                        for (size_t c = 0; c < data.size(); c++) {
                            total += data[c].size();
                            for (size_t j = 0; j < data[c].size(); j++) {
                                acc += data[c][j] * data[c][j];
                            }
                        }
                        acc /= total;
                        acc = sqrt(acc);

                        Value num = Value(std::to_string(scopes[i]).c_str(), alloc);
                        a.AddMember(num, Value(acc).Move(), alloc);
                    }

                    d.AddMember("data", a, alloc);

                    if (has_members) {
                        StringBuffer buffer;
                        Writer<StringBuffer> writer(buffer);
                        d.Accept(writer);

                        std::string json_string(buffer.GetString());
                        ws->broadcast_message(json_string);
                    }
                    else if (++emptyFrames >= 10) {
                        reader->reconnect(); 
                    }

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
        std::vector<unsigned int> _scopes;
        std::mutex scope_mutex;

        server* ws;

        int emptyFrames = 0;
};

#endif
