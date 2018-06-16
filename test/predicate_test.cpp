
// nonsugar
//
// Copyright iorate 2016-2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE predicate_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <nonsugar.hpp>

using namespace nonsugar;

BOOST_AUTO_TEST_CASE(test_predicate)
{
    auto const cmd = command<char>("test_predicate")
        .flag<'A', int>({'a'}, {}, "N", "100 <= N < 1000",
            predicate<int>([](int n) { return 100 <= n && n < 1000; }))
        .flag<'B', std::string>({'b'}, {}, "S", "S is one of abc, def, ghi",
            predicate<std::string>(
                [](auto const &s) { return s == "abc" || s == "def" || s == "ghi"; }))
        ;
    std::vector<std::string> args = { "-a", "10" };
    try {
        parse(args.begin(), args.end(), cmd);
    } catch (error const &e) {
        BOOST_TEST(e.message() == "test_predicate: invalid argument: -a 10");
        args = { "-babd" };
        try {
            parse(args.begin(), args.end(), cmd);
        } catch (error const &er) {
            BOOST_TEST(er.message() == "test_predicate: invalid argument: -b abd");
            args = { "-a500", "-b", "def" };
            auto const opts = parse(args.begin(), args.end(), cmd);
            BOOST_TEST(opts.get<'A'>() == 500);
            BOOST_TEST(opts.get<'B'>() == "def");
            return;
        }
    }
    BOOST_TEST(false);
}
