
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
using namespace nonsugar; // if you like

int main(int argc, char *argv[])
try {
    // Create a command.
    auto const cmd = command<char>("usage", "nonsugar example")
     // The template parameter is an integer or enumeration type that identifies options.
     // The 1st parameter is the program name used in a usage and error messages.
     // The 2nd parameter is the program summary used in a usage (optional).

        // Add a flag without a value.
        .flag<'h'>({'h'}, {"help"}, "", "produce help message", flag_type::exclusive)
         // The template parameter is the option identifier.
         // The 1st parameter is the list of short names.
         // The 2nd parameter is the list of long names.
         // The 3rd parameter is the value placeholder, which is never used.
         // The 4th parameter is the flag summary used in a usage.
         // The 5th parameter is the flag type (optional). When flag_type::exclusive is given,
         // specifying the flag makes a parser ignore subcommands and arguments.
         //  It is useful for --help and --version flag.

        // Add a flag with a value.
        .flag<'o', int>({}, {"optimization","optimisation"}, "N", "optimization level", 10)
         // The 2nd template parameter is the value type.
         //  If an optional type (e.g. boost::optional<T>) is given, the value is optional.
         //  If a container type (e.g. std::vector<T>) is given, the flag can be specified multiple
         //  times.
         // The 3rd parameter is the value placeholder used in a usage.
         // The 5th parameter is the default value (optional).

        // Add an argument.
        .argument<'i', std::vector<std::string>>("INPUT-FILE")
         // The 1st template parameter is the option identifier.
         // The 2nd template parameter is the value type.
         // The parameter is the value placeholder used in a usage.
        ;

    // Parse the command line and get the option map.
    auto const opts = parse(argc, argv, cmd);

    if (opts.has<'h'>()) {
        // Show the help message.
        std::cout << usage(cmd);
        return 0;
    }

    // Is the option specified?
    if (opts.has<'o'>()) {
        // The template parameter is the option identifier.
        // For arguments, has() always returns true.

        // Get the option value.
        int const n = opts.get<'o'>();
        // The template parameter is the option identifier.
        // If the flag is not specified, get() leads to undefined behavior.

        std::cout << "optimization level: " << n << "\n";
    }
} catch (error const &e) {
 // When the parser failed, an object of basic_error<String> is thrown (error is a typedef of
 // basic_error<std::string>).

    // Get the error message.
    std::cerr << e.message() << "\n";
}
