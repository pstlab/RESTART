#include "restart_app.h"
#include "restart_db.h"
#include "coco_executor.h"
#include <iomanip>

namespace restart
{
    restart_app::restart_app(restart_db &db, const std::string &restart_app_host, const unsigned short restart_app_port) : coco_core(db), coco_listener(static_cast<coco::coco_core &>(*this)), server(restart_app_host, restart_app_port)
    {
        LOG_DEBUG("Creating restart_app..");
        add_route(boost::beast::http::verb::get, "^/$", std::function{[](const string_req &, file_res &res)
                                                                      {
                                                                          res.result(boost::beast::http::status::ok);
                                                                          boost::beast::error_code ec;
                                                                          res.body().open("client/dist/index.html", boost::beast::file_mode::read, ec);
                                                                          if (ec)
                                                                              LOG_ERR("Failed to open index.html: " << ec.message());
                                                                      }});

        add_route(boost::beast::http::verb::get, "^(/assets/.+)|/.+\\.ico|/.+\\.png$", std::function{[](const string_req &req, file_res &res)
                                                                                                     {
                                                                                                         std::string target = req.target().to_string();
                                                                                                         if (target.find('?') != std::string::npos)
                                                                                                             target = target.substr(0, target.find('?'));
                                                                                                         res.result(boost::beast::http::status::ok);
                                                                                                         res.set(boost::beast::http::field::content_type, network::mime_types.at(target.substr(target.find_last_of('.') + 1)));
                                                                                                         boost::beast::error_code ec;
                                                                                                         res.body().open(("client/dist" + target).c_str(), boost::beast::file_mode::read, ec);
                                                                                                         if (ec)
                                                                                                             LOG_ERR("Failed to open " << target << ": " << ec.message());
                                                                                                     }});

        add_ws_route("/restart")
            .on_open(std::bind(&restart_app::on_ws_open, this, std::placeholders::_1))
            .on_message(std::bind(&restart_app::on_ws_message, this, std::placeholders::_1, std::placeholders::_2))
            .on_close(std::bind(&restart_app::on_ws_close, this, std::placeholders::_1))
            .on_error(std::bind(&restart_app::on_ws_error, this, std::placeholders::_1, std::placeholders::_2));

        add_route<boost::beast::http::string_body, boost::beast::http::string_body>(boost::beast::http::verb::get, "^/sensor/.+$", std::bind(&restart_app::get_sensor_data, this, std::placeholders::_1, std::placeholders::_2));
        add_route<boost::beast::http::string_body, boost::beast::http::string_body>(boost::beast::http::verb::post, "^/sensor/.+$", std::bind(&restart_app::add_sensor_data, this, std::placeholders::_1, std::placeholders::_2));
    }

    void restart_app::get_sensor_data(const string_req &req, string_res &res)
    {
        const std::lock_guard<std::recursive_mutex> lock(cc.get_mutex());

        std::size_t query_start = req.target().find('?', 8);
        std::string sensor_id = query_start == std::string::npos ? req.target().substr(8).to_string() : req.target().substr(8, query_start - 8).to_string();
        if (!cc.get_database().has_sensor(sensor_id))
        {
            res.result(boost::beast::http::status::not_found);
            res.set(boost::beast::http::field::content_type, "application/json");
            res.body() = json::json{{"success", false}, {"message", "Sensor not found"}}.to_string();
            return;
        }
        std::map<std::string, std::string> fields = query_start != std::string::npos ? network::parse_query(req.target().substr(query_start + 1).to_string()) : std::map<std::string, std::string>{};

        std::chrono::system_clock::time_point to;
        if (fields.count("to"))
            to = std::chrono::system_clock::time_point{std::chrono::milliseconds{std::stoll(fields["to"])}};
        else
            to = std::chrono::system_clock::now();

        std::chrono::system_clock::time_point from;
        if (fields.count("from"))
            from = std::chrono::system_clock::time_point{std::chrono::milliseconds{std::stoll(fields["from"])}};
        else
            from = to - std::chrono::hours{24 * 30};

#ifdef VERBOSE_LOG
        auto from_t = std::chrono::system_clock::to_time_t(from);
        auto to_t = std::chrono::system_clock::to_time_t(to);

        LOG_DEBUG("From: " << std::put_time(std::localtime(&from_t), "%c %Z"));
        LOG_DEBUG("To: " << std::put_time(std::localtime(&to_t), "%c %Z"));
#endif

        res.set(boost::beast::http::field::content_type, "application/json");
        res.body() = cc.get_database().get_sensor_data(cc.get_database().get_sensor(sensor_id), from, to).to_string();
    }

    void restart_app::add_sensor_data(const string_req &req, string_res &res)
    {
        const std::lock_guard<std::recursive_mutex> lock(cc.get_mutex());

        std::string sensor_id = req.target().to_string().substr(8);
        if (!cc.get_database().has_sensor(sensor_id))
        {
            res.result(boost::beast::http::status::not_found);
            res.set(boost::beast::http::field::content_type, "application/json");
            res.body() = json::json{{"success", false}, {"message", "Sensor not found"}}.to_string();
            return;
        }

        cc.add_sensor_data(cc.get_database().get_sensor(sensor_id), json::load(req.body().data()));
    }

    void restart_app::on_ws_open(network::websocket_session &ws)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        sessions.insert(&ws);
        LOG_DEBUG("New websocket connection. Total: " << sessions.size());
    }

    void restart_app::on_ws_message(network::websocket_session &ws, const std::string &msg)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        auto x = json::load(msg);
        if (x.get_type() != json::json_type::object || !x.get_object().count("type"))
        {
            ws.close(boost::beast::websocket::close_code::bad_payload);
            return;
        }

        if (x["type"] == "login")
        {
            auto &db = static_cast<restart_db &>(cc.get_database());

            // we send the login response
            ws.send(json::json{{"type", "login"}, {"success", true}, {"config", db.get_config()}}.to_string());

            // we send the sensor types
            ws.send(sensor_types_message(db.get_sensor_types()).to_string());

            // we send the sensors
            ws.send(sensors_message(db.get_sensors()).to_string());

            // we send the solvers
            ws.send(solvers_message(cc.get_executors()).to_string());

            for (const auto &cc_exec : cc.get_executors())
            {
                // we send the solver state
                ws.send(executor_state_message(cc_exec->get_executor()).to_string());

                // we send the solver graph
                ws.send(executor_graph_message(*cc_exec).to_string());
            }
        }
    }

    void restart_app::on_ws_close(network::websocket_session &ws)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        sessions.erase(&ws);
        LOG_DEBUG("Websocket connection closed. Total: " << sessions.size());
    }

    void restart_app::on_ws_error(network::websocket_session &ws, [[maybe_unused]] const boost::system::error_code &ec)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        sessions.erase(&ws);
        LOG_DEBUG("Websocket connection error. Total: " << sessions.size());
    }

    void restart_app::new_sensor_type(const coco::sensor_type &st)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(new_sensor_type_message(st));
    }
    void restart_app::updated_sensor_type(const coco::sensor_type &st)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(updated_sensor_type_message(st));
    }
    void restart_app::deleted_sensor_type(const std::string &id)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(coco::deleted_sensor_type_message(id));
    }

    void restart_app::new_sensor(const coco::sensor &s)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(new_sensor_message(s));
    }
    void restart_app::updated_sensor(const coco::sensor &s)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(updated_sensor_message(s));
    }
    void restart_app::deleted_sensor(const std::string &id)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(coco::deleted_sensor_message(id));
    }

    void restart_app::new_sensor_value(const coco::sensor &s, const std::chrono::system_clock::time_point &timestamp, const json::json &value)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(sensor_value_message(s, timestamp, value));
    }
    void restart_app::new_sensor_state(const coco::sensor &s, const std::chrono::system_clock::time_point &timestamp, const json::json &state)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(sensor_state_message(s, timestamp, state));
    }

    void restart_app::new_solver(const coco::coco_executor &exec)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(ratio::executor::new_solver_message(exec.get_executor()));
    }
    void restart_app::deleted_solver(const uintptr_t id)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(ratio::executor::deleted_solver_message(id));
    }

    void restart_app::state_changed(const coco::coco_executor &exec)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(executor_state_changed_message(exec.get_executor()));
    }

    void restart_app::flaw_created(const coco::coco_executor &, const ratio::flaw &f)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(flaw_created_message(f));
    }
    void restart_app::flaw_state_changed(const coco::coco_executor &, const ratio::flaw &f)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(flaw_state_changed_message(f));
    }
    void restart_app::flaw_cost_changed(const coco::coco_executor &, const ratio::flaw &f)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(flaw_cost_changed_message(f));
    }
    void restart_app::flaw_position_changed(const coco::coco_executor &, const ratio::flaw &f)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(flaw_position_changed_message(f));
    }
    void restart_app::current_flaw(const coco::coco_executor &, const ratio::flaw &f)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(current_flaw_message(f));
    }

    void restart_app::resolver_created(const coco::coco_executor &, const ratio::resolver &r)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(resolver_created_message(r));
    }
    void restart_app::resolver_state_changed(const coco::coco_executor &, const ratio::resolver &r)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(resolver_state_changed_message(r));
    }
    void restart_app::current_resolver(const coco::coco_executor &, const ratio::resolver &r)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(current_resolver_message(r));
    }

    void restart_app::causal_link_added(const coco::coco_executor &, const ratio::flaw &f, const ratio::resolver &r)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(causal_link_added_message(f, r));
    }

    void restart_app::executor_state_changed(const coco::coco_executor &exec, ratio::executor::executor_state)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(executor_state_changed_message(exec.get_executor()));
    }

    void restart_app::tick(const coco::coco_executor &exec, const utils::rational &time)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(tick_message(exec.get_executor(), time));
    }

    void restart_app::start(const coco::coco_executor &exec, const std::unordered_set<ratio::atom *> &atoms)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(start_message(exec.get_executor(), atoms));
    }
    void restart_app::end(const coco::coco_executor &exec, const std::unordered_set<ratio::atom *> &atoms)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        broadcast(end_message(exec.get_executor(), atoms));
    }

    void restart_app::broadcast(const std::shared_ptr<const std::string> &msg)
    {
        std::lock_guard<std::recursive_mutex> _(cc.get_mutex());
        for (const auto &ws : sessions)
            ws->send(msg);
    }
} // namespace restart
