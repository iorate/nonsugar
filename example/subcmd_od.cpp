
// nonsugar
//
// Copyright iorate 2016-2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// https://github.com/tanakh/optparse-declarative/blob/master/example/subcmd.hs

#include <iostream>
#include <string>

#include <nonsugar.hpp>

int main(int argc, char **argv)
{
    using namespace nonsugar;

    enum class Option { Help, Greet, Connect };
    enum class GreetOption { Help, Message, Decorate, Name };
    enum class ConnectOption { Help, Host, Port };

    auto const greetCmd = command<GreetOption>("subcmd_od greet", "Greeting command")
        .flag<GreetOption::Help>({'?'}, {"help"}, "", "display this help and exit", flag_type::exclusive)
        .flag<GreetOption::Message, std::string>({'g'}, {"greet"}, "STRING", "greeting message", "Hello")
        .flag<GreetOption::Decorate>({}, {"decorate"}, "", "decorate message")
        .argument<GreetOption::Name, std::string>("NAME")
        ;
    auto const connectCmd = command<ConnectOption>("subcmd_od connect", "Connect command")
        .flag<ConnectOption::Help>({'?'}, {"help"}, "", "display this help and exit", flag_type::exclusive)
        .flag<ConnectOption::Host, std::string>({'h'}, {"host"}, "HOST", "host name", "localhost")
        .flag<ConnectOption::Port, int>({'p'}, {"port"}, "PORT", "port number", 8080)
        ;
    auto const cmd = command<Option>("subcmd_od", "Test program for sub commands")
        .flag<Option::Help>({'?'}, {"help"}, "", "display this help and exit", flag_type::exclusive)
        .subcommand<Option::Greet>("greet", "Greeting command", greetCmd)
        .subcommand<Option::Connect>("connect", "Connect command", connectCmd)
        ;

    try {
        auto const opts = parse(argc, argv, cmd);
        if (opts.has<Option::Help>()) {
            std::cout << usage(cmd) << std::endl;
        } else if (auto const greetOpts = opts.get_shared<Option::Greet>()) {
            if (greetOpts->has<GreetOption::Help>()) {
                std::cout << usage(greetCmd) << std::endl;
            } else {
                auto const x = greetOpts->get<GreetOption::Message>() + ", " + greetOpts->get<GreetOption::Name>();
                std::cout << (greetOpts->has<GreetOption::Decorate>() ? "*** " + x + " ***" : x) << std::endl;
            }
        } else if (auto const connectOpts = opts.get_shared<Option::Connect>()) {
            if (connectOpts->has<ConnectOption::Help>()) {
                std::cout << usage(connectCmd) << std::endl;
            } else {
                auto const addr = connectOpts->get<ConnectOption::Host>() + ":" + std::to_string(connectOpts->get<ConnectOption::Port>());
                std::cout << "connect to " << addr << std::endl;
            }
        }
    } catch (error const &e) {
        std::cerr << e.message() << std::endl;
    }
}
