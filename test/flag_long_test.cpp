
// nonsugar
//
// Copyright iorate 2016-2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE flag_long_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <nonsugar.hpp>

using namespace nonsugar;

BOOST_AUTO_TEST_CASE(flag_long_value)
{
    auto const cmd = command<char>("flag_long_value", "")
        .flag<'v'>({}, {"version"}, "") // deprecated style
        .flag<'V'>({}, {"verbose"}, "", "")
        .flag<'a', int>({}, {"a"}, "N", "")
        .flag<'b', int>({}, {"b"}, "N", "")
        .flag<'c', boost::optional<int>>({}, {"c"}, "N", "")
        .flag<'d', boost::optional<int>>({}, {"d"}, "N", "")
        .flag<'e', boost::optional<int>>({}, {"e"}, "N", "")
        .flag<'f', std::vector<std::string>>({}, {"f"}, "STRING", "")
        .flag<'g', int>({}, {"g"}, "N", "")
        ;
    std::vector<std::string> const args = {
        "--vers", "--verb", "--a=23", "--b", "42",
        "--c", "--d=580", "--e=147", "--f=foo", "--f", "bar" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST(opts.has<'v'>());
    BOOST_TEST(opts.has<'V'>());
    BOOST_TEST((opts.has<'a'>() && opts.get<'a'>() == 23));
    BOOST_TEST((opts.has<'b'>() && opts.get<'b'>() == 42));
    BOOST_TEST((opts.has<'c'>() && opts.get<'c'>() == boost::none));
    BOOST_TEST((opts.has<'d'>() && opts.get<'d'>() == 580));
    BOOST_TEST((opts.has<'e'>() && opts.get<'e'>() == 147));
    BOOST_TEST_REQUIRE(opts.has<'f'>());
    BOOST_TEST(
        opts.get<'f'>() == (std::vector<std::string>{"foo", "bar"}),
        boost::test_tools::per_element());
    BOOST_TEST(!opts.has<'g'>());
}

BOOST_AUTO_TEST_CASE(flag_long_fail_ambiguous)
{
    auto const cmd = command<char>("flag_long_fail_ambiguous", "")
        .flag<'v'>({}, {"version"}, "")
        .flag<'V'>({}, {"verbose"}, "")
        .flag<'a'>({}, {"a"}, "")
        .flag<'A'>({}, {"a"}, "")
        .flag<'b'>({}, {"b"}, "")
        .flag<'B'>({}, {"bb"}, "")
        ;
    try {
        std::vector<std::string> const args = { "--ver" };
        parse(args.begin(), args.end(), cmd);
    } catch (error const &e) {
        BOOST_TEST(e.message() == "flag_long_fail_ambiguous: ambiguous option: --ver [--version, --verbose]");
        try {
            std::vector<std::string> const args = { "--a" };
            parse(args.begin(), args.end(), cmd);
        } catch (error const &er) {
            BOOST_TEST(er.message() == "flag_long_fail_ambiguous: ambiguous option: --a");
            std::vector<std::string> const args = { "--b", "--bb" };
            auto const opts = parse(args.begin(), args.end(), cmd);
            BOOST_TEST(opts.has<'b'>());
            BOOST_TEST(opts.has<'B'>());
            return;
        }
        BOOST_TEST(false);
        return;
    }
    BOOST_TEST(false);
}
