
// nonsugar
//
// Copyright iorate 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string>

#include <nonsugar.hpp>

int main(int argc, char **argv)
{
    using namespace nonsugar;

    enum class Option { Help, Greet, Connect };
    enum class GreetOption { Help, Message, Decolate, Name };
    enum class ConnectOption { Help, Host, Port };

    auto const greetCmd = command<GreetOption>("subcmd greet", "Greeting command")
        .flag<GreetOption::Help>("?", "help", "display this help and exit")
        .flag<GreetOption::Message, std::string>("g", "greet", "STRING", "greeting message", "Hello")
        .flag<GreetOption::Decolate>("", "decolate", "decolate message")
        .argument<GreetOption::Name>("NAME")
        ;
    auto const connectCmd = command<ConnectOption>("subcmd connect", "Connect command")
        .flag<ConnectOption::Help>("?", "help", "display this help and exit")
        .flag<ConnectOption::Host, std::string>("h", "host", "HOST", "host name", "localhost")
        .flag<ConnectOption::Port, int>("p", "port", "PORT", "port number", 8080)
        ;
    auto const cmd = command<Option>("subcmd", "Test program for sub commands")
        .flag<Option::Help>("?", "help", "display this help and exit")
        .subcommand<Option::Greet>("greet", greetCmd)
        .subcommand<Option::Connect>("connect", connectCmd)
        ;

    auto const opts = parse(argc, argv, cmd);
    if (opts.has<Option::Help>()) {
        std::cout << usage(desc) << std::endl;
    } else if (auto const &greetOpts = opts.get_optional<Option::Greet>()) {
        if (greetOpts->has<GreetOption::Help>()) {
            std::cout << usage(greetDesc) << std::endl;
        } else {
            auto const x = greetOpts->get<GreetOption::Message>() + ", " + greetOpts->get<GreetOption::Name>();
            std::cout << (greetOpts->has<GreetOption::Decorate>() ? "***" + x + "***" : x) << std::endl;
        }
    } else if (auto const &connectOpts = opts.get_optional<Option::Connect>()) {
        if (connectOpts->has<ConnectOption::Help>()) {
            std::cout << usage(connectDesc) << std::endl;
        } else {
            auto const addr = connectOpts->get<ConnectOption::Host>() + ":" + std::to_string(connectOpts->get<ConnectOption::Port>());
            std::cout << "connect to " << addr << std::endl;
        }
    }
}
