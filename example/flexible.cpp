
// nonsugar
//
// Copyright iorate 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string>
#include <nonsugar.hpp>
using namespace nonsugar;

int main(int argc, char *argv[])
try {
    auto const cmd = command<char>("flexible")
        .flag<'h'>({'h'}, {"help"}, "", "display this help and exit", flag_type::exclusive)
        .flag<'o', std::string>({'o'}, {"output"}, "FILE", "write output to FILE")
        .argument<'i', std::string>("INPUT-FILE")
        ;
    auto const opts = parse(argc, argv, cmd, argument_order::flexible);

    if (opts.has<'h'>()) {
        std::cout << usage(cmd);
        return 0;
    }

    std::cout << "Input: " << opts.get<'i'>() << std::endl;
    if (opts.has<'o'>()) {
        std::cout << "Output: " << opts.get<'o'>() << std::endl;
    }
} catch (error const &e) {
    std::cerr << e.message() << std::endl;
}
