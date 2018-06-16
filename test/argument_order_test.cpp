
// nonsugar
//
// Copyright iorate 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE argument_order_test

#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>
#include <nonsugar.hpp>

using namespace nonsugar;

BOOST_AUTO_TEST_CASE(argument_order_strict)
{
    auto const cmd = command<char>("argument_order_strict")
        .flag<'a'>({'a'}, {}, "", "")
        .flag<'b'>({'b'}, {}, "", "")
        .argument<'c', std::vector<std::string>>("C")
        ;
    std::vector<std::string> const args = { "-a", "string", "-b" };
    auto const opts = parse(args.begin(), args.end(), cmd, argument_order::strict);
    BOOST_TEST(opts.has<'a'>());
    BOOST_TEST(!opts.has<'b'>());
    BOOST_TEST_REQUIRE(opts.has<'c'>());
    BOOST_TEST(
        opts.get<'c'>() == (std::vector<std::string>{"string", "-b"}),
        boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(argument_order_flexible)
{
    auto const cmd = command<char>("argument_order_flexible")
        .flag<'a'>({'a'}, {}, "", "")
        .flag<'b', int>({'b'}, {}, "N", "")
        .flag<'c'>({'c'}, {}, "", "")
        .argument<'d', std::vector<std::string>>("D")
        ;
    std::vector<std::string> const args = { "-a", "23", "-b", "42", "--", "-c" };
    auto const opts = parse(args.begin(), args.end(), cmd, argument_order::flexible);
    BOOST_TEST(opts.has<'a'>());
    BOOST_TEST((opts.has<'b'>() && opts.get<'b'>() == 42));
    BOOST_TEST(!opts.has<'c'>());
    BOOST_TEST(
        opts.get<'d'>() == (std::vector<std::string>{"23", "-c"}),
        boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(argument_order_flexible_subcommand)
{
    auto const subcmd = command<char>("argument_order_flexible_subcommand sub")
        .flag<'b'>({'b'}, {}, "", "")
        .flag<'c'>({'c'}, {}, "", "")
        .argument<'d', std::string>("STR")
        ;
    auto const cmd = command<char>("argument_order_flexible_subcommand")
        .flag<'a'>({'a'}, {}, "", "")
        .flag<'b'>({'b'}, {}, "", "")
        .subcommand<'S'>("sub", "", subcmd)
        ;
    std::vector<std::string> const args = { "-a", "sub", "-b", "str", "-c" };
    auto const opts = parse(args.begin(), args.end(), cmd, argument_order::flexible);
    BOOST_TEST(opts.has<'a'>());
    BOOST_TEST(!opts.has<'b'>());
    BOOST_TEST_REQUIRE(opts.has<'S'>());
    auto const subopts = opts.get<'S'>();
    BOOST_TEST(subopts.has<'b'>());
    BOOST_TEST(subopts.has<'c'>());
    BOOST_TEST((subopts.has<'d'>() && subopts.get<'d'>() == "str"));
}
