
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
    auto const cmd = command<char>("compiler", "nonsugar example")
        .flag<'h'>({}, {"help"}, "", "produce help message")
        .flag<'v'>({'v'}, {"version"}, "", "print version string")
        .flag<'o', int>({}, {"optimization"}, "N", "optimization level")
        .flag<'I', std::vector<std::string>>({'I'}, {"include-path"}, "PATH", "include path")
        .argument<'i', std::vector<std::string>>("INPUT-FILE")
        ;
    auto const opts = parse(argc, argv, cmd);
    if (opts.has<'h'>()) {
        std::cout << usage(cmd);
        return 0;
    }
    if (opts.has<'v'>()) {
        std::cout << "compiler, version 1.0\n";
        return 0;
    }
    if (opts.has<'o'>()) {
        std::cout << "optimization level: " << opts.get<'o'>() << "\n";
    }
    if (opts.has<'I'>()) {
        std::cout << "include paths:";
        for (auto const &path : opts.get<'I'>()) std::cout << " " << path;
        std::cout << "\n";
    }
    std::cout << "input files:";
    for (auto const &file : opts.get<'i'>()) std::cout << " " << file;
    std::cout << "\n";
} catch (error const &e) {
    std::cerr << e.message() << "\n";
    return 1;
}
