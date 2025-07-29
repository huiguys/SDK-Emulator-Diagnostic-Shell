#include "dashboard.hpp"
#include <fstream>
#include <httplib.h>
#include <sstream>


namespace ti_sdk {
namespace web {

void Dashboard::runServer() {
  httplib::Server svr;

  // Serve static files
  svr.set_mount_point("/", "./web");

  // API endpoints
  svr.Get("/api/state",
          [this](const httplib::Request &, httplib::Response &res) {
            res.set_content(getStateJSON(), "application/json");
          });

  // WebSocket endpoint
  svr.ws.on("/ws", [this](const httplib::Request &, httplib::WebSocket &ws) {
    while (ws.is_open() && running_) {
      std::string msg;
      if (ws.recv(msg)) {
        handleWebSocket(msg);
      }
    }
  });

  // Start server
  svr.listen("localhost", port_);
}

void Dashboard::handleWebSocket(const std::string &message) {
  try {
    auto j = nlohmann::json::parse(message);
    std::string type = j["type"];

    if (type == "gpio") {
      uint8_t port = j["port"];
      uint8_t pin = j["pin"];
      bool state = j["state"];
      // Handle GPIO command
      LOG_INFO("WebSocket GPIO command: port=" + std::to_string(port) +
               " pin=" + std::to_string(pin) +
               " state=" + std::to_string(state));
    } else if (type == "uart") {
      uint8_t channel = j["channel"];
      std::string data = j["data"];
      // Handle UART command
      LOG_INFO("WebSocket UART command: channel=" + std::to_string(channel) +
               " data=" + data);
    }
  } catch (const std::exception &e) {
    LOG_ERROR("WebSocket message parse error: " + std::string(e.what()));
  }
}

void Dashboard::notifyClients(const std::string &type) {
  // In a real implementation, this would send updates to connected WebSocket
  // clients
  LOG_DEBUG("State update: " + type);
}

} // namespace web
} // namespace ti_sdk