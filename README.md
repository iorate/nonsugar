# nonsugar
A simple command line parser for C++

## Description
`nonsugar` is a single header library that provides command line parsers for C++14. It is inspired by [Boost.Program\_options](http://www.boost.org/doc/libs/1_62_0/doc/html/program_options.html), [optparse-declarative](https://github.com/tanakh/optparse-declarative) and so on.

## Example
This is a reimplementation of [Boost.Program\_options tutorial](http://www.boost.org/doc/libs/1_62_0/doc/html/program_options/tutorial.html).
```cpp
// compiler.cpp
#include <iostream>
#include <string>
#include <vector>
#include <nonsugar.hpp>
using namespace nonsugar;

int main(int argc, char *argv[])
try {
    auto const cmd = command<char>("compiler", "nonsugar example")
        .flag<'h'>({}, {"help"}, "produce help message")
        .flag<'v'>({'v'}, {"version"}, "print version string")
        .flag<'o', int>({}, {"optimization"}, "N", "optimization level")
        .flag<'I', std::vector<std::string>>({'I'}, {"include-path"}, "PATH", "include path")
        .argument<'i', std::vector<std::string>>("INPUT-FILE")
        ;
    auto const opts = parse(argc, argv, cmd);
    if (opts.has<'h'>()) {
        std::cout << usage(cmd);
        return 0;
    }
    if (opts.has<'v'>()) {
        std::cout << "compiler, version 1.0\n";
        return 0;
    }
    if (opts.has<'o'>()) {
        std::cout << "optimization level: " << opts.get<'o'>() << "\n";
    }
    if (opts.has<'I'>()) {
        std::cout << "include paths:";
        for (auto const &path : opts.get<'I'>()) std::cout << " " << path;
        std::cout << "\n";
    }
    std::cout << "input files:";
    for (auto const &file : opts.get<'i'>()) std::cout << " " << file;
    std::cout << "\n";
} catch (error const &e) {
    std::cerr << e.message() << "\n";
    return 1;
}
```
``` shell
$ g++ -I.. compiler.cpp -o compiler

$ ./compiler --help
Usage: compiler [OPTION...] [INPUT-FILE...]
  nonsugar example

Options:
           --help               produce help message
  -v       --version            print version string
           --optimization=N     optimization level
  -I PATH  --include-path=PATH  include path

$ ./compiler --optimization=4 -I foo a.cpp b.cpp
optimization level: 4
include paths: foo
input files: a.cpp b.cpp
```

## Features
* wide character support
* subcommand support
* short option bundling (e.g. `tar -xvf`)
* multiple names for one option
* generic data types
* built-in help
* unit tests

## Requirement
A compiler that supports C++14. `nonsugar` is tested under g++ 7.2.0 and clang++ 5.0.0.

## Installation
Just copy `nonsugar.hpp` to your include directory.

## Usage
1\. Include `nonsugar.hpp`.
```cpp
#include <nonsugar.hpp>
using namespace nonsugar; // if you like
```
2\. Create a command.
```cpp
    // Create a command.
    auto const cmd = command<char>("compiler", "nonsugar example")
     // The template parameter is an integer or enumeration type that identifies options.
     // The 1st parameter is the program name used in a usage and error messages.
     // The 2nd parameter is the program summary used in a usage (default: "").

        // Add a flag without a value.
        .flag<'h'>({'h'}, {"help"}, "produce help message", true)
         // The template parameter is the option identifier.
         // The 1st parameter is the list of short names.
         // The 2nd parameter is the list of long names.
         // The 3rd parameter is the flag summary used in a usage.
         // The 4th parameter is whether to skip parsing of subcommands and arguments (default:
         // false).
         //  It is useful for --help and --version flag.

        // Add a flag with a value.
        .flag<'o', int>({}, {"optimization","optimisation"}, "N", "optimization level", 10)
         // The 2nd template parameter is the value type.
         //  If an optional type (e.g. boost::optional<T>) is given, the value is optional.
         //  If a container type (e.g. std::vector<T>) is given, the flag can be specified multiple
         //  times.
         // The 3rd parameter is the value placeholder used in a usage.
         // The 5th parameter is the default value (optional).

        // Add an argument.
        .argument<'i', std::vector<std::string>>("INPUT-FILE")
         // The 1st template parameter is the option identifier.
         // The 2nd template parameter is the value type.
         // The parameter is the value placeholder used in a usage.
        ;
```
3\. Parse the command line and get the option map.
```cpp
try {
    // Parse the command line and get the option map.
    auto const opts = parse(argc, argv, cmd);
} catch (error const &e) {
 // When the parsing failed, an object of basic_error<String> is thrown (error is a typedef of
 // basic_error<std::string>).

    // Get the error message.
    std::cerr << e.message() << "\n";
}
```
4\. Read the option map.
```cpp
// Is the option specified?
if (opts.has<'o'>()) {
 // The template parameter is the option identifier.
 // For arguments, has() always returns true.

    // Get the option value.
    int const n = opts.get<'o'>();
     // The template parameter is the option identifier.
     // If the flag is not specified, get() leads to undefined behavior.

    std::cout << "optimization level: " << n << "\n";
}
```
``` cpp
// get_shared() may be a smarter way.
// It returns std::make_shared<Value>(value) when the flag is specified, nullptr otherwise.
if (auto const n = opts.get_shared<'o'>()) {
    std::cout << "optimization level: " << *n << "\n";
}
```
5\. Show the help message if necessary.
```cpp
if (opts.has<'h'>()) {
    // Show the help message.
    std::cout << usage(cmd);
    return 0;
}
```

## Go Further

### Wide character support
You can use character types other than `char`.
```cpp
try {
    auto const cmd = wcommand<char>(L"compiler")
        .flag<'h'>({L'h',L'?'}, {L"help"}, L"produce help message")
        ;
    auto const opts = parse(argc, wargv, cmd);
    if (opts.has<'h'>()) std::wcout << usage(cmd);
} catch (werror const &e) {
    std::wcerr << e.message() << L"\n";
}
```

### Subcommand support
`nonsugar` supports subcommands. Example:
```cpp
// subcmd.cpp
#include <iostream>
#include <string>
#include <vector>
#include <nonsugar.hpp>
using namespace nonsugar;

int main(int argc, char *argv[])
try {
    // Create subcommands.
    auto const helloCmd = command<char>("subcmd hello")
        .argument<'n', std::string>("NAME")
        ;
    auto const addCmd = command<char>("subcmd add")
        .argument<'l', int>("LHS")
        .argument<'r', int>("RHS")
        ;
    
    // Create the main command.
    auto const cmd = command<char>("subcmd")
        // Add a subcommand.
        .subcommand<'H'>("hello", "hello command", helloCmd)
         // The template parameter is the option identifier.
         // The 1st parameter is the subcommand name.
         // The 2nd parameter is the subcommand summary used in a usage.
         // The 3rd parameter is the subcommand itself.

        // Add a subcommand.
        .subcommand<'A'>("add", "add command", addCmd)

        .flag<'h'>({'h'}, {"help"}, "produce help message", true)
         // The 4th parameter indicates that the subcommands are ignored when this flag is
         // specified.
        ;

    // Parse the command line.
    auto const opts = parse(argc, argv, cmd);

    if (opts.has<'h'>()) {
        std::cout << usage(cmd);
        return 0;
    }

    // Read the subcommands.
    if (opts.has<'H'>()) {
        // For a subcommand, get() returns the option map of the subcommand.
        auto const helloOpts = opts.get<'H'>();
        auto const name = helloOpts.get<'n'>();
        std::cout << "Hello, " << name << "!\n";
    }
    else if (opts.has<'A'>()) {
        auto const addOpts = opts.get<'A'>();
        std::cout << addOpts.get<'l'>() + addOpts.get<'r'>() << "\n";
    }
} catch (error const &e) {
    std::cerr << e.message() << "\n";
}
```
``` shell
$ g++ -I.. subcmd.cpp -o subcmd

$ ./subcmd
subcmd: command required

$ ./subcmd --help
Usage: subcmd [OPTION...] COMMAND [ARG...]

Options:
  -h  --help  produce help message

Commands:
  hello  hello command
  add    add command

$ ./subcmd hello world
Hello, world!

$ ./subcmd add 3 4
7
```

### Custom value reader
By default, the flag value is translated by `std::basic_stringstream`. You can customize this behavior by giving a custom value reader to `flag()`.  A custom value reader shall be have a signature `std::shared_ptr<Value> (String const &)`. It shall return `nullptr` when an invalid value is passed.
```cpp
auto const cmd = command<char>("custom")
    .flag<'t', int>({'t'}, {"transparency"}, "Set the transparency (0-255)",
        [](std::string const &s) -> std::shared_ptr<int>
        {
            try {
                int const n = boost::lexical_cast<int>(s);
                return 0 <= n && n <= 255 ? std::make_shared<int>(n) : nullptr;
            } catch (boost::bad_lexical_cast const &) {
                return nullptr;
            }
        })
    ;
```
If you'd just like to validate the value, you can use `predicate()` helper function.
```cpp
auto const cmd = command<char>("custom")
    .flag<'t', int>({'t'}, {"transparency"}, "Set the transparency (0-255)",
        predicate<int>([](int n) { return 0 <= n && n <= 255; }))
    ;
```

## Author
[@iorate](https://twitter.com/iorate)

## License
[Boost Software License 1.0](http://www.boost.org/LICENSE_1_0.txt)
