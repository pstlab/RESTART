#pragma once

#include "coco_server.hpp"

namespace restart
{
  enum role
  {
    admin,
    phisician,
    patient
  };

  class restart : public coco::coco_server
  {
  public:
    restart(const std::string &host = SERVER_HOST, unsigned short port = SERVER_PORT, const std::string &mongodb_uri = "mongodb://" MONGODB_HOST ":" MONGODB_PORT, const std::string &transformer_host = TRANSFORMER_HOST, unsigned short transformer_port = TRANSFORMER_PORT);

#ifdef ENABLE_AUTH
  private:
    void init_routes() override;

    void on_ws_open(network::ws_session &ws) override;
    void on_ws_message(network::ws_session &ws, const std::string &msg) override;
    void on_ws_close(network::ws_session &ws) override;
    void on_ws_error(network::ws_session &ws, const std::error_code &) override;

    void new_type(const coco::type &tp) override;
    void updated_type(const coco::type &tp) override;
    void deleted_type(const std::string &tp_id) override;

    void new_item(const coco::item &itm) override;
    void updated_item(const coco::item &itm) override;
    void deleted_item(const std::string &itm_id) override;

    void new_data(const coco::item &itm, const json::json &data, const std::chrono::system_clock::time_point &timestamp) override;

    void new_solver(const coco::coco_executor &exec) override;
    void deleted_solver(const uintptr_t id) override;

    void new_reactive_rule(const coco::rule &r) override;
    void new_deliberative_rule(const coco::rule &r) override;

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

  private:
    std::unordered_map<network::ws_session *, std::string> clients;
    std::unordered_map<std::string, std::pair<std::set<int>, std::set<network::ws_session *>>> users;
    std::unordered_map<std::string, std::set<network::ws_session *>> sensors;
    std::unordered_map<std::string, std::set<network::ws_session *>> actuators;
#endif
  };
} // namespace restart
