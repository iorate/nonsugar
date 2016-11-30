
// nonsugar
//
// Copyright iorate 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <nonsugar.hpp>

BOOST_AUTO_TEST_CASE(mock)
{
    using namespace nonsugar;
    enum class SubOption { A, B, C, D, E, F, G };
    auto const subcmd = command<SubOption>("mock sub", "subcommand test")
        .flag<SubOption::A>("a", "active", "be active")
        .flag<SubOption::B, int>("b", "buy", "N", "buy N dollars")
        .flag<SubOption::C, int>("c", "code", "C", "set the code to C", 100)
        .flag<SubOption::D, int>("d", "dump", "N", "dump N times",
            [](auto &val, auto const &str)
            {
                if (!val) val = std::make_shared<int>(std::stoi(*str));
            })
        .flag<SubOption::E, int>("", "enum", "X", "set X (100-200) default: 150", 150,
            [](auto &val, auto const &str)
            {
                auto const n = std::stoi(*str);
                if (n < 100 || 200 < n) throw error("invalid value");
                val = std::make_shared<int>(n);
            })
        .argument<SubOption::F, std::string>("FAKE")
        .argument<SubOption::G, std::string>(
            "GREAT", [](auto &val, auto const &str) { if (!val) val = str; })
        ;
    enum class Option { A, B };
    auto const cmd = command<Option>("mock", "mock test")
        .flag<Option::A>("a", "abort", "abort")
        .subcommand<Option::B>("sub", "test subcommand", subcmd)
        ;
    std::vector<std::string> const args = { "mock", "sub", "-ab10", "-e", "120" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    if (opts.has<Option::B>()) {
        auto const subopts = opts.get<Option::B>();
        auto const code = subopts.get_shared<SubOption::C>();
        *code;
    }
}
