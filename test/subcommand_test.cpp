
// nonsugar
//
// Copyright iorate 2016-2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE subcommand_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <nonsugar.hpp>

using namespace nonsugar;

BOOST_AUTO_TEST_CASE(subcommand_success1)
{
    auto const add = command<char>("calc add", "add command")
        .argument<'L', int>("LHS")
        .argument<'R', int>("RHS")
        ;
    auto const sub = command<char>("calc sub", "sub command")
        .argument<'L', int>("LHS")
        .argument<'R', int>("RHS")
        ;
    auto const cmd = command<char>("calc", "subcommand test")
        .subcommand<'A'>("add", "add command", add)
        .subcommand<'S'>("sub", "sub command", sub)
        .flag<'H'>({'h'}, {"help"}, "show help message")
        ;
    std::vector<std::string> args = { "add", "23", "42" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST_REQUIRE(opts.has<'A'>());
    auto const addopts = opts.get<'A'>();
    BOOST_TEST_REQUIRE(addopts.has<'L'>());
    BOOST_TEST_REQUIRE(addopts.has<'R'>());
    auto const n = addopts.get<'L'>() + addopts.get<'R'>();
    BOOST_TEST(n == 65);
}

BOOST_AUTO_TEST_CASE(subcommand_failure1)
{
    auto const add = command<char>("calc add", "");
    auto const cmd = command<char>("calc", "")
        .subcommand<'A'>("add", "add command", add)
        .subcommand<'B'>("add", "add command", add)
        ;
    try {
        std::vector<std::string> args = { "add" };
        parse(args.begin(), args.end(), cmd);
    } catch (error const &e) {
        BOOST_TEST(e.message() == "calc: ambiguous command: add");
        try {
            std::vector<std::string> args = { "sub" };
            parse(args.begin(), args.end(), cmd);
        } catch (error const &er) {
            BOOST_TEST(er.message() == "calc: unrecognized command: sub");
            try {
                std::vector<std::string> args = {};
                parse(args.begin(), args.end(), cmd);
            } catch (error const &err) {
                BOOST_TEST(err.message() == "calc: command required");
                return;
            }
        }
    }
    BOOST_TEST(false);
}
