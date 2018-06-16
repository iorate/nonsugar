
// nonsugar
//
// Copyright iorate 2016-2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE mock_wstring_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <nonsugar.hpp>

BOOST_AUTO_TEST_CASE(mock_wstring)
{
    using namespace nonsugar;
    enum class SubOption { A, B, C, D, E, F, G };
    auto const subcmd = wcommand<SubOption>(L"mock sub", L"subcommand test")
        .flag<SubOption::A>({L'a'}, {L"active"}, L"be active")
        .flag<SubOption::B, int>({L'b'}, {L"buy"}, L"N", L"buy N dollars")
        .flag<SubOption::C, int>({L'c'}, {L"code"}, L"C", L"set the code to C", 100)
        .flag<SubOption::D, int>({L'd'}, {L"dump"}, L"N", L"dump N times",
            [](auto const &str)
            {
                return std::make_shared<int>(std::stoi(str));
            })
        .flag<SubOption::E, int>({L'e'}, {L"enum"}, L"X", L"set X (100-200) default: 150", 150,
            [](auto const &str)
            {
                auto const n = std::stoi(str);
                if (n < 100 || 200 < n) throw werror(L"invalid value");
                return std::make_shared<int>(n);
            })
        .argument<SubOption::F, std::wstring>(L"FAKE")
        .argument<SubOption::G, std::wstring>(
            L"GREAT", [](auto const &str) { return std::make_shared<std::wstring>(str); })
        ;
    enum class Option { A, B };
    auto const cmd = wcommand<Option>(L"mock", L"mock test")
        .flag<Option::A>({L'a'}, {L"abort"}, L"abort")
        .subcommand<Option::B>(L"sub", L"test subcommand", subcmd)
        ;
    std::vector<std::wstring> const args = { L"sub", L"-ab12", L"-e", L"120", L"fake", L"great" };
    auto const opts = parse(args.begin(), args.end(), cmd);
    if (opts.has<Option::B>()) {
        auto const subopts = opts.get<Option::B>();
        auto const code = subopts.get_shared<SubOption::E>();
        BOOST_TEST(*code == 120);
    } else {
        BOOST_TEST(false);
    }
}
