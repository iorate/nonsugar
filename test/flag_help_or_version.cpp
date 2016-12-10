
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

using namespace nonsugar;

BOOST_AUTO_TEST_CASE(flag_help)
{
    auto const cmd = command<char>("flag_help", "")
        .flag<'H'>({'h'}, {"help"}, "", true)
        .subcommand<'S'>("sub", "subcommand", command<char>("", ""))
        ;
    std::vector<std::string> const args = { "-h" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST(opts.has<'H'>());
}

BOOST_AUTO_TEST_CASE(flag_version)
{
    auto const cmd = command<char>("flag_version", "")
        .flag<'V'>({'v'}, {"version"}, "", true)
        .argument<'A', std::string>("ARG")
        ;
    std::vector<std::string> const args = { "--version", "arg" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST(opts.has<'V'>());
    BOOST_TEST(!opts.has<'A'>());
}
