
// nonsugar
//
// Copyright iorate 2016-2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE usage_test

#include <boost/test/unit_test.hpp>

#include <memory>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <nonsugar.hpp>

BOOST_AUTO_TEST_CASE(usage_argument)
{
    using namespace nonsugar;
    enum class Option { A, B, C, D, E, F, G };
    auto const cmd = command<Option>("usage", "usage test\nyes usage test")
        .flag<Option::A>({'a'}, {"absorb"}, "HOGE") // deprecated style
        .flag<Option::B, int>({'b','B'}, {"burn"}, "N", "Once upon a time,\nthere is...")
        .flag<Option::C, boost::optional<int>>({'c'}, {"color","colour"}, "COLOR", "Set color")
        .argument<Option::D, std::string>("NAME")
        .argument<Option::E, boost::optional<int>>("NUMBER")
        .argument<Option::F, std::vector<std::string>>("PATHS")
        .flag<Option::G>({'g'}, {}, "UNUSED", "G")
        ;
    BOOST_TEST(usage(cmd) ==
R"(Usage: usage [OPTION...] NAME [NUMBER] [PATHS...]
  usage test
  yes usage test

Options:
  -a          --absorb                           HOGE
  -b N, -B N  --burn=N                           Once upon a time,
                                                 there is...
  -c[COLOR]   --color[=COLOR], --colour[=COLOR]  Set color
  -g                                             G
)");
}

BOOST_AUTO_TEST_CASE(usage_subcommand)
{
    using namespace nonsugar;
    enum class Option { A, B, C };
    enum class BOption {};
    enum class COption {};
    auto const cmd = command<Option>("usage", "usage test")
        .flag<Option::A>({'?'}, {"help"}, "")
        .subcommand<Option::B>("b", "B\ncommand", command<BOption>("usage b", ""))
        .subcommand<Option::C>("c", "", command<COption>("usage c", ""))
        ;
    BOOST_TEST(usage(cmd) ==
R"(Usage: usage [OPTION...] COMMAND [ARG...]
  usage test

Options:
  -?  --help  

Commands:
  b  B
     command
  c  
)");
}

BOOST_AUTO_TEST_CASE(usage_empty)
{
    using namespace nonsugar;
    enum class Option {};
    BOOST_TEST(usage(command<Option>("usage", "")) == R"(Usage: usage
)");
}

BOOST_AUTO_TEST_CASE(usage_wstring)
{
    using namespace nonsugar;
    enum class Option { A };
    auto const cmd = wcommand<Option>(L"usage", L"usage test")
        .flag<Option::A>({L'?'}, {L"help"}, L"show this help")
        ;
    BOOST_CHECK(usage(cmd) ==
LR"(Usage: usage [OPTION...]
  usage test

Options:
  -?  --help  show this help
)");
}
