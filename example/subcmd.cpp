
// nonsugar
//
// Copyright iorate 2016-2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string>
#include <vector>
#include <nonsugar.hpp>
using namespace nonsugar;

int main(int argc, char *argv[])
try {
    // Create subcommands.
    auto const helloCmd = command<char>("subcmd hello")
        .argument<'n', std::string>("NAME")
        ;
    auto const addCmd = command<char>("subcmd add")
        .argument<'l', int>("LHS")
        .argument<'r', int>("RHS")
        ;

    // Create the main command.
    auto const cmd = command<char>("subcmd")
        // Add a subcommand.
        .subcommand<'H'>("hello", "hello command", helloCmd)
         // The template argument is the option identifier.
         // The 1st parameter is the subcommand name.
         // The 2nd parameter is the subcommand summary used in usage.
         // The 3rd parameter is the subcommand itself.

        // Add a subcommand.
        .subcommand<'A'>("add", "add command", addCmd)

        .flag<'h'>({'h'}, {"help"}, "", "produce help message", flag_type::exclusive)
         // flag_type::exclusive indicates that the subcommands are ignored when this flag is
         // specified.
        ;

    // Parse the command line.
    auto const opts = parse(argc, argv, cmd);

    if (opts.has<'h'>()) {
        std::cout << usage(cmd);
        return 0;
    }

    // Read the subcommands.
    if (opts.has<'H'>()) {
        // For subcommands, get() returns the option map of the subcommand.
        auto const helloOpts = opts.get<'H'>();
        auto const name = helloOpts.get<'n'>();
        std::cout << "Hello, " << name << "!\n";
    }
    else if (opts.has<'A'>()) {
        auto const addOpts = opts.get<'A'>();
        std::cout << addOpts.get<'l'>() + addOpts.get<'r'>() << "\n";
    }
} catch (error const &e) {
    std::cerr << e.message() << "\n";
}
