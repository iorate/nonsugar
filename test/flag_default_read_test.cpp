
// nonsugar
//
// Copyright iorate 2016-2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE flag_default_read_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <nonsugar.hpp>

using namespace nonsugar;

BOOST_AUTO_TEST_CASE(flag_default)
{
    auto const cmd = command<char>("flag_default", "")
        .flag<'A', int>({'a'}, {}, "INT", "", 42)
        ;
    std::vector<std::string> const args;
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST((opts.has<'A'>() && opts.get<'A'>() == 42));
}

BOOST_AUTO_TEST_CASE(flag_read)
{
    auto const f = [](std::string const &str) -> std::shared_ptr<int>
        {
            try {
                auto const n = std::stoi(str);
                return n <= 100 ? std::make_shared<int>(n) : nullptr;
            } catch (...) {
                return nullptr;
            }
        };
    auto const cmd = command<char>("flag_read", "")
        .flag<'A', int>({'a'}, {}, "N", "", f)
        .flag<'B', int>({'b'}, {}, "N", "", 50, f)
        ;
    {
        std::vector<std::string> const args = { "-a33" };
        auto const opts = parse(args.begin(), args.end(), cmd);
        BOOST_TEST((opts.has<'A'>() && opts.get<'A'>() == 33));
        BOOST_TEST((opts.has<'B'>() && opts.get<'B'>() == 50));
    }
    {
        std::vector<std::string> const args = { "-a33", "-b66" };
        auto const opts = parse(args.begin(), args.end(), cmd);
        BOOST_TEST((opts.has<'A'>() && opts.get<'A'>() == 33));
        BOOST_TEST((opts.has<'B'>() && opts.get<'B'>() == 66));
    }
    try {
        std::vector<std::string> const args = { "-a120" };
        parse(args.begin(), args.end(), cmd);
    } catch (error const &e) {
        BOOST_TEST(e.message() == "flag_read: invalid argument: -a 120");
        try {
            std::vector<std::string> const args = { "-a33", "-b120" };
            parse(args.begin(), args.end(), cmd);
        } catch (error const &er) {
            BOOST_TEST(er.message() == "flag_read: invalid argument: -b 120");
            return;
        }
        BOOST_TEST(false);
        return;
    }
    BOOST_TEST(false);
}
