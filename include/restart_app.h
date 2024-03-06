#pragma once

#include "coco_core.h"
#include "coco_listener.h"
#include "server.hpp"
#include "client.hpp"

using string_req = boost::beast::http::request<boost::beast::http::string_body>;
using string_res = boost::beast::http::response<boost::beast::http::string_body>;
using file_res = boost::beast::http::response<boost::beast::http::file_body>;

namespace restart
{
  class restart_db;

  class restart_app : public coco::coco_core, public coco::coco_listener, public network::server
  {
  public:
    restart_app(restart_db &db, const std::string &dashboard_host = DASHBOARD_HOST, const unsigned short dashboard_port = DASHBOARD_PORT);

  private:
    friend void understand(Environment *env, UDFContext *udfc, UDFValue *out);
    void on_intent(const string_res &res);

    friend void trigger_intent(Environment *env, UDFContext *udfc, UDFValue *out);
    void on_intent_response(const string_res &res);

    friend void compute_response(Environment *env, UDFContext *udfc, UDFValue *out);
    void on_response(const string_res &res);

    friend void say(Environment *env, UDFContext *udfc, UDFValue *out);

  private:
    void on_ws_open(network::websocket_session &ws);
    void on_ws_message(network::websocket_session &ws, const std::string &msg);
    void on_ws_close(network::websocket_session &ws);
    void on_ws_error(network::websocket_session &ws, const boost::system::error_code &ec);

    void new_message(const std::string &device, const std::string &content);

    void get_sensor_data(const string_req &req, string_res &res);
    void add_sensor_data(const string_req &req, string_res &res);

    void new_sensor_type(const coco::sensor_type &s) override;
    void updated_sensor_type(const coco::sensor_type &s) override;
    void deleted_sensor_type(const std::string &id) override;

    void new_sensor(const coco::sensor &s) override;
    void updated_sensor(const coco::sensor &s) override;
    void deleted_sensor(const std::string &id) override;

    void new_sensor_value(const coco::sensor &s, const std::chrono::system_clock::time_point &timestamp, const json::json &value) override;
    void new_sensor_state(const coco::sensor &s, const std::chrono::system_clock::time_point &timestamp, const json::json &state) override;

    void new_solver(const coco::coco_executor &exec) override;
    void deleted_solver(const uintptr_t id) override;

    void state_changed(const coco::coco_executor &exec) override;

    void flaw_created(const coco::coco_executor &exec, const ratio::flaw &f) override;
    void flaw_state_changed(const coco::coco_executor &exec, const ratio::flaw &f) override;
    void flaw_cost_changed(const coco::coco_executor &exec, const ratio::flaw &f) override;
    void flaw_position_changed(const coco::coco_executor &exec, const ratio::flaw &f) override;
    void current_flaw(const coco::coco_executor &exec, const ratio::flaw &f) override;

    void resolver_created(const coco::coco_executor &exec, const ratio::resolver &r) override;
    void resolver_state_changed(const coco::coco_executor &exec, const ratio::resolver &r) override;
    void current_resolver(const coco::coco_executor &exec, const ratio::resolver &r) override;

    void causal_link_added(const coco::coco_executor &exec, const ratio::flaw &f, const ratio::resolver &r) override;

    void executor_state_changed(const coco::coco_executor &exec, ratio::executor::executor_state state) override;

    void tick(const coco::coco_executor &exec, const utils::rational &time) override;

    void start(const coco::coco_executor &exec, const std::unordered_set<ratio::atom *> &atoms) override;
    void end(const coco::coco_executor &exec, const std::unordered_set<ratio::atom *> &atoms) override;

  private:
    void broadcast(const std::shared_ptr<const std::string> &msg);
    void broadcast(const std::string &&msg) { broadcast(std::make_shared<std::string>(msg)); }
    void broadcast(const json::json &msg) { broadcast(msg.to_string()); }

  private:
    network::client language_client;
    std::unordered_set<network::websocket_session *> sessions;
  };
} // namespace restart
