
// nonsugar
//
// Copyright iorate 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE parse_optional_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <nonsugar.hpp>

BOOST_AUTO_TEST_CASE(parse_optional_)
{
    using namespace nonsugar;
    auto const cmd = command<char>("parse_optional")
        .flag<'h'>({}, {"help"}, "", "produce help message")
        .flag<'o', int>({}, {"optimization"}, "N", "optimization level")
        .argument<'i', std::string>("INPUT-FILE")
        ;
    {
        std::vector<std::string> const args = { "input.cpp", "--optimization", "4" };
        std::string err;
        auto const opts = parse_optional(args.begin(), args.end(), cmd, err);
        BOOST_TEST(!opts);
        BOOST_TEST(err == "parse_optional: unrecognized argument: --optimization");
    }
    {
        char arg0[] = "parse1";
        char arg1[] = "input.cpp";
        char arg2[] = "--optimization";
        char arg3[] = "4";
        char *argv[] = { arg0, arg1, arg2, arg3, nullptr };
        std::string err;
        auto const opts = parse_optional(4, argv, cmd, err);
        BOOST_TEST(!opts);
        BOOST_TEST(err == "parse_optional: unrecognized argument: --optimization");
    }
    {
        std::vector<std::string> const args = { "input.cpp", "--optimization", "4" };
        std::string err;
        auto const opts = parse_optional(args.begin(), args.end(), cmd, argument_order::flexible, err);
        BOOST_TEST_REQUIRE(opts.has_value());
        BOOST_TEST(!opts->has<'h'>());
        BOOST_TEST(opts->get<'o'>() == 4);
        BOOST_TEST(opts->get<'i'>() == "input.cpp");
    }
    {
        char arg0[] = "parse_optional";
        char arg1[] = "input.cpp";
        char arg2[] = "--optimization";
        char arg3[] = "4";
        char *argv[] = { arg0, arg1, arg2, arg3, nullptr };
        std::string err;
        auto const opts = parse_optional(4, argv, cmd, argument_order::flexible, err);
        BOOST_TEST_REQUIRE(opts.has_value());
        BOOST_TEST(!opts->has<'h'>());
        BOOST_TEST(opts->get<'o'>() == 4);
        BOOST_TEST(opts->get<'i'>() == "input.cpp");
    }
}
