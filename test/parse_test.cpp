
// nonsugar
//
// Copyright iorate 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE parse_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <nonsugar.hpp>

BOOST_AUTO_TEST_CASE(parse_)
{
    using namespace nonsugar;
    auto const cmd = command<char>("parse")
        .flag<'h'>({}, {"help"}, "", "produce help message")
        .flag<'o', int>({}, {"optimization"}, "N", "optimization level")
        .argument<'i', std::string>("INPUT-FILE")
        ;
    {
        std::vector<std::string> const args = { "input.cpp", "--optimization", "4" };
        try {
            parse(args.begin(), args.end(), cmd);
            BOOST_TEST(false);
        } catch (error const &e) {
            BOOST_TEST(e.message() == "parse: unrecognized argument: --optimization");
        }
    }
    {
        char arg0[] = "parse";
        char arg1[] = "input.cpp";
        char arg2[] = "--optimization";
        char arg3[] = "4";
        char *argv[] = { arg0, arg1, arg2, arg3, nullptr };
        try {
            parse(4, argv, cmd);
            BOOST_TEST(false);
        } catch (error const &e) {
            BOOST_TEST(e.message() == "parse: unrecognized argument: --optimization");
        }
    }
    {
        std::vector<std::string> const args = { "input.cpp", "--optimization", "4" };
        auto const opts = parse(args.begin(), args.end(), cmd, argument_order::flexible);
        BOOST_TEST(!opts.has<'h'>());
        BOOST_TEST(opts.get<'o'>() == 4);
        BOOST_TEST(opts.get<'i'>() == "input.cpp");
    }
    {
        char arg0[] = "parse";
        char arg1[] = "input.cpp";
        char arg2[] = "--optimization";
        char arg3[] = "4";
        char *argv[] = { arg0, arg1, arg2, arg3, nullptr };
        auto const opts = parse(4, argv, cmd, argument_order::flexible);
        BOOST_TEST(!opts.has<'h'>());
        BOOST_TEST(opts.get<'o'>() == 4);
        BOOST_TEST(opts.get<'i'>() == "input.cpp");
    }
}
