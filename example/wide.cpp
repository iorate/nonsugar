
// nonsugar
//
// Copyright iorate 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <nonsugar.hpp>
using namespace nonsugar;

int wmain(int argc, wchar_t *wargv[])
try {
    auto const cmd = wcommand<char>(L"compiler")
        .flag<'h'>({L'h',L'?'}, {L"help"}, L"produce help message")
        ;
    auto const opts = parse(argc, wargv, cmd);
    if (opts.has<'h'>()) std::wcout << usage(cmd);
} catch (werror const &e) {
    std::wcerr << e.message() << L"\n";
}

