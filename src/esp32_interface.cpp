#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include "k2_action/action/esp32.hpp"

using namespace std::chrono_literals;
namespace asio = boost::asio;
using ESP32Action = k2_action::action::ESP32;
using GoalHandleESP32 = rclcpp_action::ServerGoalHandle<ESP32Action>;

class ESP32ActionServer : public rclcpp::Node {
public:
    ESP32ActionServer() : Node("esp32_action_server"), serial_(io_service_) {
        // Parameters for serial port
        this->declare_parameter<std::string>("serial_port", "/dev/ttyUSB0");
        this->declare_parameter<int>("baud_rate", 115200);

        std::string port = this->get_parameter("serial_port").as_string();
        int baud_rate = this->get_parameter("baud_rate").as_int();

        // Open serial connection
        try {
            serial_.open(port);
            serial_.set_option(asio::serial_port_base::baud_rate(baud_rate));
            RCLCPP_INFO(this->get_logger(), "Connected to ESP32 on %s", port.c_str());
        } catch (std::exception &e) {
            RCLCPP_ERROR(this->get_logger(), "Failed to open serial port: %s", e.what());
            rclcpp::shutdown();
        }

        // Create action server
        action_server_ = rclcpp_action::create_server<ESP32Action>(
            this, "esp32_command",
            std::bind(&ESP32ActionServer::handle_goal, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&ESP32ActionServer::handle_cancel, this, std::placeholders::_1),
            std::bind(&ESP32ActionServer::handle_accepted, this, std::placeholders::_1));
    }

private:
    asio::io_service io_service_;
    asio::serial_port serial_;

    rclcpp_action::Server<ESP32Action>::SharedPtr action_server_;

    void writeSerial(const std::string &cmd) {
        asio::write(serial_, asio::buffer(cmd + "\n"));
    }

    std::string readSerial() {
        asio::streambuf buf;
        asio::read_until(serial_, buf, "\n");
        std::istream is(&buf);
        std::string response;
        std::getline(is, response);
        return response;
    }

    // Handle goal request
    rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID &uuid,
                                            std::shared_ptr<const ESP32Action::Goal> goal) {
        RCLCPP_INFO(this->get_logger(), "Received command: %s on pin %d", goal->command.c_str(), goal->pin);
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    // Handle cancel request
    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandleESP32> goal_handle) {
        RCLCPP_INFO(this->get_logger(), "Cancel request received");
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    // Execute the goal
    void handle_accepted(const std::shared_ptr<GoalHandleESP32> goal_handle) {
        std::thread{std::bind(&ESP32ActionServer::execute, this, goal_handle)}.detach();
    }

    void execute(const std::shared_ptr<GoalHandleESP32> goal_handle) {
        const auto goal = goal_handle->get_goal();
        auto feedback = std::make_shared<ESP32Action::Feedback>();
        auto result = std::make_shared<ESP32Action::Result>();

        std::string command;

        if (goal->command == "SET_DIGITAL") {
            command = "SET D " + std::to_string(goal->pin) + " " + std::to_string(goal->value);
        } else if (goal->command == "READ_DIGITAL") {
            command = "READ D " + std::to_string(goal->pin);
        } else if (goal->command == "READ_ANALOG") {
            command = "READ A " + std::to_string(goal->pin);
        } else {
            result->response = "ERR Unknown Command";
            goal_handle->abort(result);
            return;
        }

        RCLCPP_INFO(this->get_logger(), "Sending command...");

        feedback->status = "Sending command...";
        goal_handle->publish_feedback(feedback);

        writeSerial(command);
        std::this_thread::sleep_for(100ms); // Allow ESP32 to process

        RCLCPP_INFO(this->get_logger(), "Command was sent...waiting for response");

        feedback->status = "Waiting for response...";
        goal_handle->publish_feedback(feedback);

        result->response = readSerial();
        RCLCPP_INFO(this->get_logger(), "Response was received");
        goal_handle->succeed(result);
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ESP32ActionServer>());
    rclcpp::shutdown();
    return 0;
}
