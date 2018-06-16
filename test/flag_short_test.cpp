
// nonsugar
//
// Copyright iorate 2016-2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE flag_short_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <nonsugar.hpp>

using namespace nonsugar;

BOOST_AUTO_TEST_CASE(flag_short_value)
{
    auto const cmd =command<char>("flag_short_value", "")
        .flag<'A'>({'a'}, {}, "") // deprecated style
        .flag<'B', int>({'b'}, {}, "I", "")
        .flag<'C', int>({'c'}, {}, "I", "")
        .flag<'D', std::shared_ptr<int>>({'d'}, {}, "I", "")
        .flag<'E', boost::optional<int>>({'e'}, {}, "I", "")
        .flag<'F', boost::optional<int>>({'f'}, {}, "I", "")
        .flag<'G', std::vector<std::string>>({'g'}, {}, "S", "")
        .flag<'H'>({'h'}, {}, "", "")
        .flag<'I', int>({'i'}, {}, "I", "")
        ;
    std::vector<std::string> const args = {
        "-a", "-b23", "-c", "42", "-d", "-e29", "-f", "-gfoo", "-g" "bar", "-h" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST(opts.has<'A'>());
    BOOST_TEST((opts.has<'B'>() && opts.get<'B'>() == 23));
    BOOST_TEST((opts.has<'C'>() && opts.get<'C'>() == 42));
    BOOST_TEST((opts.has<'D'>() && opts.get<'D'>() == nullptr));
    BOOST_TEST((opts.has<'E'>() && opts.get<'E'>() == 29));
    BOOST_TEST((opts.has<'F'>() && opts.get<'F'>() == boost::none));
    BOOST_TEST_REQUIRE(opts.has<'G'>());
    BOOST_TEST(
        opts.get<'G'>() == (std::vector<std::string>{"foo", "bar"}),
        boost::test_tools::per_element());
    BOOST_TEST(opts.has<'H'>());
    BOOST_TEST(!opts.has<'I'>());
}

BOOST_AUTO_TEST_CASE(flag_short_bundling)
{
    auto const cmd = command<char>("flag_short_bundling", "")
        .flag<'A'>({'a'}, {}, "")
        .flag<'B', std::string>({'b'}, {}, "STRING", "")
        .flag<'C'>({'c'}, {}, "")
        .flag<'D', int>({'d'}, {}, "INT", "")
        .flag<'E'>({'e'}, {}, "")
        .flag<'F', boost::optional<int>>({'f'}, {}, "INT", "")
        .flag<'G'>({'g'}, {}, "")
        .flag<'H', boost::optional<std::string>>({'h'}, {}, "STRING", "")
        .argument<'I', int>("INT")
        ;
    std::vector<std::string> const args = {
        "-abc", "-cd", "10", "-ef12", "-gh", "14" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST(opts.has<'A'>());
    BOOST_TEST((opts.has<'B'>() && opts.get<'B'>() == "c"));
    BOOST_TEST(opts.has<'C'>());
    BOOST_TEST((opts.has<'D'>() && opts.get<'D'>() == 10));
    BOOST_TEST(opts.has<'E'>());
    BOOST_TEST((opts.has<'F'>() && opts.get<'F'>() == 12));
    BOOST_TEST(opts.has<'G'>());
    BOOST_TEST((opts.has<'H'>() && opts.get<'H'>() == boost::none));
    BOOST_TEST((opts.has<'I'>() && opts.get<'I'>() == 14));
}

BOOST_AUTO_TEST_CASE(flag_short_fail_ambiguous)
{
    try {
        auto const cmd = command<char>("flag_short_fail_ambiguous", "")
            .flag<'A'>({'a','x'}, {}, "")
            .flag<'B'>({'b','x'}, {}, "")
            ;
        std::vector<std::string> const args = { "-x" };
        parse(args.begin(), args.end(), cmd);
    } catch (error const &e) {
        BOOST_TEST(e.message() == "flag_short_fail_ambiguous: ambiguous option: -x");
        return;
    }
    BOOST_TEST(false);
}

BOOST_AUTO_TEST_CASE(flag_short_fail_unrecognized)
{
    try {
        auto const cmd = command<char>("flag_short_fail_unrecognized", "");
        std::vector<std::string> const args = { "-x" };
        parse(args.begin(), args.end(), cmd);
    } catch (error const &e) {
        BOOST_TEST(e.message() == "flag_short_fail_unrecognized: unrecognized option: -x");
        return;
    }
    BOOST_TEST(false);
}

BOOST_AUTO_TEST_CASE(flag_short_fail_value)
{
    auto const cmd = command<char>("flag_short_fail_value", "")
        .flag<'A', int>({'a'}, {}, "INT", "")
        .flag<'B', int>({'b'}, {}, "INT", "")
        .flag<'C', int>({'c'}, {}, "INT", "")
        ;
    try {
        std::vector<std::string> const args = { "-ax", "-b", "3", "-c", "5" };
        parse(args.begin(), args.end(), cmd);
    } catch (error const &e) {
        BOOST_TEST(e.message() == "flag_short_fail_value: invalid argument: -a x");
        try {
            std::vector<std::string> const args = { "-a1", "-b", "x", "-c", "5" };
            parse(args.begin(), args.end(), cmd);
        } catch (error const &er) {
            BOOST_TEST(er.message() == "flag_short_fail_value: invalid argument: -b x");
            try {
                std::vector<std::string> const args = { "-a1", "-b", "3", "-c" };
                parse(args.begin(), args.end(), cmd);
            } catch (error const &err) {
                BOOST_TEST(err.message() == "flag_short_fail_value: argument required: -c");
                return;
            }
            BOOST_TEST(false);
            return;
        }
        BOOST_TEST(false);
        return;
    }
    BOOST_TEST(false);
}
