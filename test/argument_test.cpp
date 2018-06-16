
// nonsugar
//
// Copyright iorate 2016-2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE argument_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <nonsugar.hpp>

using namespace nonsugar;

BOOST_AUTO_TEST_CASE(argument_success1)
{
    auto const cmd = command<char>("argument_test", "Test required argument")
        .argument<'N', std::string>("NAME")
        .argument<'A', int>("AGE")
        .argument<'L', std::shared_ptr<std::string>>("LASTNAME")
        .argument<'I', boost::optional<int>>("ID")
        ;
    std::vector<std::string> args = { "John", "23", "Doe" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST(opts.get<'N'>() == "John");
    BOOST_TEST(opts.get<'A'>() == 23);
    BOOST_TEST(*opts.get<'L'>() == "Doe");
    BOOST_TEST(!opts.get<'I'>());
}

BOOST_AUTO_TEST_CASE(argument_success2)
{
    auto const cmd = command<char>("argument_test", "Test container argument")
        .argument<'N', std::vector<int>>("NUMS")
        ;
    std::vector<std::string> args = { "23", "42" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    auto const nums = opts.get<'N'>();
    BOOST_TEST(nums == (std::vector<int> { 23, 42 }), boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(argument_failure1)
{
    auto const cmd = command<char>("foobar", "test argument error")
        .argument<'N', std::string>("NAME")
        .argument<'A', int>("A_INT")
        .argument<'B', boost::optional<int>>("B_INT")
        .argument<'C', std::vector<int>>("C_INT")
        ;
    try {
        std::vector<std::string> args = {};
        parse(args.begin(), args.end(), cmd);
    } catch (error const &e) {
        BOOST_TEST(e.message() == "foobar: argument required: NAME");
        try {
            std::vector<std::string> args = { "iorate", "foo" };
            parse(args.begin(), args.end(), cmd);
        } catch (error const &er) {
            BOOST_TEST(er.message() == "foobar: invalid argument: A_INT=foo");
            try {
                std::vector<std::string> args = { "iorate", "12", "bar" };
                parse(args.begin() , args.end(), cmd);
            } catch (error const &err) {
                BOOST_TEST(err.message() == "foobar: invalid argument: B_INT=bar");
                try {
                    std::vector<std::string> args = { "iorate", "12", "23", "34", "baz" };
                    parse(args.begin(), args.end(), cmd);
                } catch (error const &errr) {
                    BOOST_TEST(errr.message() == "foobar: invalid argument: C_INT=baz");
                    return;
                }
            }
        }
    }
    BOOST_TEST(false);
}
