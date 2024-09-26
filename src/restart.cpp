#include "restart.hpp"
#include "restart_db.hpp"
#include "coco_api.hpp"
#include "logging.hpp"

namespace restart
{
  restart::restart(const std::string &host, unsigned short port, const std::string &mongodb_uri, const std::string &transformer_host, unsigned short transformer_port) : coco_server(host, port, std::make_unique<restart_db>(mongodb_uri), transformer_host, transformer_port) {}

#ifdef ENABLE_AUTH
  void restart::init_routes()
  {
    add_route(network::Get, "^/$", std::bind(&restart::index, this, network::placeholders::request));
    add_route(network::Get, "^(/assets/.+)|/.+\\.ico|/.+\\.png", std::bind(&restart::assets, this, network::placeholders::request));
    add_route(network::Get, "^/open_api$", std::bind(&restart::open_api, this, network::placeholders::request));
    add_route(network::Get, "^/async_api$", std::bind(&restart::async_api, this, network::placeholders::request));

    add_route(network::Post, "^/login$", std::bind(&restart::login, this, network::placeholders::request));
    add_route(network::Get, "^/users$", std::bind(&restart::get_users, this, network::placeholders::request));
    add_route(network::Get, "^/user/.*$", std::bind(&restart::get_user, this, network::placeholders::request));
    add_route(network::Post, "^/user$", std::bind(&restart::create_user, this, network::placeholders::request));
    add_route(network::Put, "^/user/.*$", std::bind(&restart::update_user, this, network::placeholders::request));
    add_route(network::Delete, "^/user/.*$", std::bind(&restart::delete_user, this, network::placeholders::request));

    add_route(network::Get, "^/types$", std::bind(&restart::get_types, this, network::placeholders::request));
    add_route(network::Get, "^/type(\\?.*)?$", std::bind(&restart::get_type, this, network::placeholders::request));
    add_route(network::Post, "^/type$", std::bind(&restart::create_type, this, network::placeholders::request), {role::admin});
    add_route(network::Put, "^/type/.*$", std::bind(&restart::update_type, this, network::placeholders::request), {role::admin});
    add_route(network::Delete, "^/type/.*$", std::bind(&restart::delete_type, this, network::placeholders::request), {role::admin});

    add_route(network::Get, "^/items(\\?.*)?$", std::bind(&restart::get_items, this, network::placeholders::request), {role::admin, role::phisician});
    add_route(network::Get, "^/item(\\?.*)?$", std::bind(&restart::get_item, this, network::placeholders::request), {role::admin, role::phisician, role::patient});
    add_route(network::Post, "^/item$", std::bind(&restart::create_item, this, network::placeholders::request), {role::admin, role::phisician, role::patient});
    add_route(network::Put, "^/item/.*$", std::bind(&restart::update_item, this, network::placeholders::request), {role::admin});
    add_route(network::Delete, "^/item/.*$", std::bind(&restart::delete_item, this, network::placeholders::request), {role::admin});

    add_route(network::Get, "^/data/.*$", std::bind(&restart::get_data, this, network::placeholders::request), {role::admin, role::phisician, role::patient});
    add_route(network::Post, "^/data/.*$", std::bind(&restart::add_data, this, network::placeholders::request), {role::admin, role::phisician, role::patient});

    add_route(network::Get, "^/reactive_rules$", std::bind(&restart::get_reactive_rules, this, network::placeholders::request), {role::admin});
    add_route(network::Post, "^/reactive_rule$", std::bind(&restart::create_reactive_rule, this, network::placeholders::request), {role::admin});
    add_route(network::Put, "^/reactive_rule/.*$", std::bind(&restart::update_reactive_rule, this, network::placeholders::request), {role::admin});
    add_route(network::Delete, "^/reactive_rule/.*$", std::bind(&restart::delete_reactive_rule, this, network::placeholders::request), {role::admin});

    add_route(network::Get, "^/deliberative_rules$", std::bind(&restart::get_deliberative_rules, this, network::placeholders::request), {role::admin});
    add_route(network::Post, "^/deliberative_rule$", std::bind(&restart::create_deliberative_rule, this, network::placeholders::request), {role::admin});
    add_route(network::Put, "^/deliberative_rule/.*$", std::bind(&restart::update_deliberative_rule, this, network::placeholders::request), {role::admin});
    add_route(network::Delete, "^/deliberative_rule/.*$", std::bind(&restart::delete_deliberative_rule, this, network::placeholders::request), {role::admin});

    add_ws_route("/coco").on_open(std::bind(&restart::on_ws_open, this, network::placeholders::request)).on_message(std::bind(&restart::on_ws_message, this, std::placeholders::_1, std::placeholders::_2)).on_close(std::bind(&restart::on_ws_close, this, network::placeholders::request)).on_error(std::bind(&restart::on_ws_error, this, network::placeholders::request, std::placeholders::_2));
  }

  void restart::on_ws_open(network::ws_session &ws)
  {
    LOG_TRACE("New connection from " << ws.remote_endpoint());
    std::lock_guard<std::recursive_mutex> _(mtx);
    clients.emplace(&ws, "");
  }
  void restart::on_ws_message(network::ws_session &ws, const std::string &msg)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    auto x = json::load(msg);
    if (x.get_type() != json::json_type::object || !x.contains("type"))
    {
      ws.close();
      return;
    }

    if (x["type"] == "login")
    {
      if (!x.contains("token"))
      {
        ws.close();
        return;
      }

      std::string token = x["token"];

      if (!db->has_item(token))
      {
        ws.close();
        return;
      }

      clients[&ws] = token;

      auto &itm = db->get_item(token);

      if (itm.get_type().get_name() == "User")
      {
        auto usr = db->get_user(token);
        if (auto it = users.find(token); it != users.end())
          it->second.second.insert(&ws);
        else
          users.emplace(token, std::pair{usr.get_roles(), std::set<network::ws_session *>{&ws}});

        if (usr.get_roles().find(role::admin) != usr.get_roles().end())
        {
          ws.send(make_types_message(*this).dump()); // we send the types

          ws.send(make_items_message(*this).dump()); // we send the items

          ws.send(make_reactive_rules_message(*this).dump()); // we send the reactive rules

          ws.send(make_deliberative_rules_message(*this).dump()); // we send the deliberative rules

          ws.send(make_solvers_message(*this).dump()); // we send the solvers

          for (const auto &cc_exec : get_solvers()) // we send the executors
          {
            ws.send(make_solver_state_message(cc_exec.get()).dump());
            ws.send(make_solver_graph_message(cc_exec.get().get_solver().get_graph()).dump());
          }
        }

        if (usr.get_roles().find(role::phisician) != usr.get_roles().end())
        {
          ws.send(make_types_message(*this).dump()); // we send the types

          ws.send(make_items_message(*this).dump()); // we send the items
        }
      }
    }
  }
  void restart::on_ws_close(network::ws_session &ws)
  {
    LOG_TRACE("Connection closed with " << ws.remote_endpoint());
    std::lock_guard<std::recursive_mutex> _(mtx);
    auto token = clients.at(&ws);
    if (auto it = users.find(token); it != users.end())
    {
      it->second.second.erase(&ws);
      if (it->second.second.empty())
        users.erase(it);
    }
    clients.erase(&ws);
    LOG_DEBUG("Connected clients: " + std::to_string(clients.size()));
  }
  void restart::on_ws_error(network::ws_session &ws, [[maybe_unused]] const std::error_code &ec)
  {
    LOG_TRACE("Connection error with " << ws.remote_endpoint() << ": " << ec.message());
    on_ws_close(ws);
  }

  void restart::new_type(const coco::type &tp)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_new_type_message(tp));
  }
  void restart::updated_type(const coco::type &tp)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_updated_type_message(tp));
  }
  void restart::deleted_type(const std::string &tp_id)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(coco::make_deleted_type_message(tp_id));
  }

  void restart::new_item(const coco::item &itm)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_new_item_message(itm));
  }
  void restart::updated_item(const coco::item &itm)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_updated_item_message(itm));
  }
  void restart::deleted_item(const std::string &itm_id)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(coco::make_deleted_item_message(itm_id));
  }

  void restart::new_data(const coco::item &itm, const json::json &data, const std::chrono::system_clock::time_point &timestamp)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_new_data_message(itm, data, timestamp));
  }

  void restart::new_solver(const coco::coco_executor &exec)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_new_solver_message(exec));
  }
  void restart::deleted_solver(const uintptr_t id)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(ratio::executor::make_deleted_solver_message(id));
  }

  void restart::new_reactive_rule(const coco::rule &r)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_new_reactive_rule_message(r));
  }
  void restart::new_deliberative_rule(const coco::rule &r)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(coco::make_new_deliberative_rule_message(r));
  }

  void restart::state_changed(const coco::coco_executor &exec)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_solver_state_message(exec));
  }

  void restart::flaw_created(const coco::coco_executor &, const ratio::flaw &f)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_flaw_created_message(f));
  }
  void restart::flaw_state_changed(const coco::coco_executor &, const ratio::flaw &f)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_flaw_state_changed_message(f));
  }
  void restart::flaw_cost_changed(const coco::coco_executor &, const ratio::flaw &f)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_flaw_cost_changed_message(f));
  }
  void restart::flaw_position_changed(const coco::coco_executor &, const ratio::flaw &f)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_flaw_position_changed_message(f));
  }
  void restart::current_flaw(const coco::coco_executor &, const ratio::flaw &f)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_current_flaw_message(f));
  }

  void restart::resolver_created(const coco::coco_executor &, const ratio::resolver &r)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_resolver_created_message(r));
  }
  void restart::resolver_state_changed(const coco::coco_executor &, const ratio::resolver &r)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_resolver_state_changed_message(r));
  }
  void restart::current_resolver(const coco::coco_executor &, const ratio::resolver &r)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_current_resolver_message(r));
  }

  void restart::causal_link_added(const coco::coco_executor &, const ratio::flaw &f, const ratio::resolver &r)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_causal_link_added_message(f, r));
  }

  void restart::executor_state_changed(const coco::coco_executor &exec, ratio::executor::executor_state)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_solver_execution_state_changed_message(exec));
  }

  void restart::tick(const coco::coco_executor &exec, const utils::rational &)
  {
    std::lock_guard<std::recursive_mutex> _(mtx);
    broadcast(make_tick_message(exec));
  }
#endif
} // namespace restart
