
// nonsugar
//
// Copyright iorate 2016-2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <memory>
#include <string>
#include <boost/lexical_cast.hpp>
#include <nonsugar.hpp>
using namespace nonsugar;

int main(int argc, char *argv[])
try {
    auto const cmd = command<char>("custom")
    .flag<'t', int>({'t'}, {"transparency"}, "N", "Set the transparency (0-255)",
        [](std::string const &s) -> std::shared_ptr<int>
        {
            try {
                int const n = boost::lexical_cast<int>(s);
                return 0 <= n && n <= 255 ? std::make_shared<int>(n) : nullptr;
            } catch (boost::bad_lexical_cast const &) {
                return nullptr;
            }
        })
    ;
    auto const opts = parse(argc, argv, cmd);
    if (opts.has<'t'>()) std::cout << "transparency: " << opts.get<'t'>() << "\n";
} catch (error const &e) {
    std::cerr << e.message() << "\n";
}
