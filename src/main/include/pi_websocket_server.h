#ifndef PI_WS_SERVER_H
#define PI_WS_SERVER_H

#define GUI_ID "PROJECT_PAPA"

#include "rapidjson/document.h"

#include "server.h"
#include "osc_sender.h"
#include "oscpkt.hh"

#include "pi_subscriptions.h"

/**
 * Server to connect with Sonic Pi.
 */
class pi_ws_server : public server {
    public:
        /**
         * Constructs a pi_ws_server.
         * @param port the port to listen to websockets on
         * @param pi_port the port to send OSC messages to sonic pi over,
         *                default is 4557
         */
        pi_ws_server(int port, int pi_port=4557, int shm_port=4556) :
            server(port), sender(pi_port)
        {
            subs.reset(new pi_subscriptions(this, shm_port));
            subs->run();
        }

    protected:
        void on_message(connection _, message msg) {        
            rapidjson::Document d;

            if (d.Parse(msg->string().c_str()).HasParseError())
                return;

            if (!d.HasMember("command") || !d["command"].IsString())
                return;

            std::string command = d["command"].GetString();

            if (command == "stop-all-jobs") {
                oscpkt::Message m("/stop-all-jobs");
                m.pushStr(GUI_ID);
                sender.send_osc(m);
            }
            else if (command == "run-code") {
                if (d.HasMember("code") && d["code"].IsString()) {
                    // TODO: set up prettier parsing
                    oscpkt::Message m("/run-code");
                    m.pushStr(GUI_ID);
                    m.pushStr(d["code"].GetString());
                    sender.send_osc(m);
                }
            }
            else if (command == "subscribe") {
                if (d.HasMember("scopes") && d["scopes"].IsArray()) {
                    std::vector<unsigned int> scopes;
                    for (auto itr = d["scopes"].Begin(); itr != d["scopes"].End(); ++itr) {
                        if (itr->IsUint()) {
                            scopes.push_back(itr->GetUint());
                        }
                    }

                    subs->update_scopes(scopes);
                }
            }
        }

        void on_close(connection c, int s, const std::string& r) {
            oscpkt::Message m("/stop-all-jobs");
            m.pushStr(GUI_ID);
            sender.send_osc(m);
        }

        osc_sender sender;
        std::unique_ptr<pi_subscriptions> subs;
};

#endif
