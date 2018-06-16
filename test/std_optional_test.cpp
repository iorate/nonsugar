
// nonsugar
//
// Copyright iorate 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE std_optional_test

#include <boost/test/unit_test.hpp>

#include <optional>
#include <string>
#include <vector>
#include <nonsugar.hpp>
#include <iostream>

using namespace nonsugar;

BOOST_AUTO_TEST_CASE(std_optional)
{
    auto const cmd = command<char>("std_optional")
        .flag<'A', std::optional<int>>({'a'}, {}, "N", "")
        .flag<'B', std::optional<int>>({'b'}, {}, "N", "")
        ;
    std::vector<std::string> const args = { "-a", "-b42" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST((opts.has<'A'>() && opts.get<'A'>() == std::nullopt));
    BOOST_TEST((opts.has<'B'>() && opts.get<'B'>() == 42));
}

BOOST_AUTO_TEST_CASE(get_optional)
{
    auto const cmd = command<char>("get_optional")
        .flag<'A', int>({'a'}, {}, "N", "")
        .flag<'B', int>({'b'}, {}, "N", "")
        ;
    std::vector<std::string> const args = { "-b42" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    BOOST_TEST((opts.get_optional<'A'>() == std::nullopt));
    BOOST_TEST((opts.get_optional<'B'>() == 42));
}
