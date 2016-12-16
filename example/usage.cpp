
// nonsugar
//
// Copyright iorate 2016.
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
    auto const cmd = command<char>("compiler", "nonsugar example")
     // Template argument is an integer or enumeration type that identifies options.
     // 1st argument is the program name used in usage and error messages.
     // 2nd argument is the program summary used in usage (default: "").

        // Add a flag without value.
        .flag<'h'>({'h'}, {"help"}, "produce help message", true)
         // The template argument is the option identifier.
         // 1st argument is the list of short names.
         // 2nd argument is the list of long names.
         // 3rd argument is the flag summary used in usage.
         // 4th argument is whether to skip parsing of subcommands and arguments (default: false).
         //  It is useful for --help and --version flag.

        // Add a flag with value.
        .flag<'o', int>({}, {"optimization","optimisation"}, "N", "optimization level", 10)
         // 2nd template argument is the value type.
         //  If an optional type (e.g. boost::optional<T>) is given, the value is optional.
         //  If a container type (e.g. std::vector<T>) is given, the flag can be specified multiple
         //  times.
         // 3rd argument is the value placeholder used in usage.
         // 5th argument is the default value (optional).

        // Add an argument.
        .argument<'i', std::vector<std::string>>("INPUT-FILE")
         // 1st template argument is the option identifier.
         // 2nd template argument is the value type.
         // The argument is the value placeholder used in usage.
        ;

    // Parse the command line and get the option map.
    auto const opts = parse(argc, argv, cmd);

#if 0
    // Is the option specified?
    if (opts.has<'o'>()) {
     // The template argument is the option identifier.
     // For arguments, has() always returns true.

        // Get the option value.
        int const n = opts.get<'o'>();
         // The template argument is the option identifier.
         // If the flag is not specified, get() leads to undefined behavior.

        std::cout << "optimization level: " << n << "\n";
    }
#else
    // get_shared() may be a smarter way.
    // It returns std::make_shared<Value>(value) when the flag is specified, nullptr otherwise.
    if (auto const n = opts.get_shared<'o'>()) {
        std::cout << "optimization level: " << *n << "\n";
    }
#endif
}
catch (error const &e) {
 // When the parsing is failed, an object of basic_error<String> is thrown (error is the typedef of
 // basic_error<std::string>).

    // Get the error message.
    std::cerr << e.message();
}
