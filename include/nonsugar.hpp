
// nonsugar
//
// Copyright iorate 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NONSUGAR_HPP
#define NONSUGAR_HPP

#include <exception>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace nonsugar {

namespace detail {

template <class String, class OptionType, OptionType Option, class Value>
struct flag
{
    static constexpr OptionType option = Option;

    String short_names;
    String long_names;
    String placeholder;
    String help;
    std::shared_ptr<Value> default_value;
    std::function<void (std::shared_ptr<Value> &, std::shared_ptr<String> const &)> read_value;
};

template <class String, class OptionType, OptionType Option, class Command>
struct subcommand
{
    static constexpr OptionType option = Option;

    String name;
    String help;
    Command command;
};

template <class String, class OptionType, OptionType Option, class Value>
struct argument
{
    static constexpr OptionType option = Option;
    using value = Value;

    String placeholder;
    std::function<void (std::shared_ptr<Value> &, std::shared_ptr<String> const &)> read_value;
};

} // namespace detail

template <
    class String,
    class OptionType,
    class Flags = std::tuple<>,
    class Subcommands = std::tuple<>,
    class Arguments = std::tuple<>>
class basic_command;

namespace detail {

template <class String, class OptionType, class Flags, class Subcommands, class Arguments>
inline basic_command<String, OptionType, Flags, Subcommands, Arguments> make_command(
    String const &header, String const &footer, Flags const &flags, Subcommands const &subcommands,
    Arguments const &arguments)
{
    return { header, footer, flags, subcommands, arguments };
}

template <class Tuple, class T>
inline auto tuple_append(Tuple const &tuple, T &&t)
{
    return std::tuple_cat(tuple, std::make_tuple(std::forward<T>(t)));
}

} // namespace detail

template <class String, class OptionType, class Flags, class Subcommands, class Arguments>
class basic_command
{
public:
    using string_type = String;

    basic_command(String const &header, String const &footer) : m_header(header), m_footer(footer)
    {}

    template <OptionType Option>
    auto flag(String const &short_names, String const &long_names, String const &help) const
    {
        detail::flag<String, OptionType, Option, void> f {
            short_names, long_names, {}, help, {}, {} };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <OptionType Option, class Value>
    auto flag(
        String const &short_names, String const &long_names, String const &placeholder, 
        String const &help) const
    {
        detail::flag<String, OptionType, Option, Value> f {
            short_names, long_names, placeholder, help, {}, {} };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <OptionType Option, class Value>
    auto flag(
        String const &short_names, String const &long_names, String const &placeholder, 
        String const &help, Value const &default_value) const
    {
        detail::flag<String, OptionType, Option, Value> f {
            short_names, long_names, placeholder, help, std::make_shared<Value>(default_value),
            {} };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <
        OptionType Option, class Value, class ReadValue,
        std::enable_if_t<!std::is_convertible<ReadValue, Value>::value> * = nullptr>
    auto flag(
        String const &short_names, String const &long_names, String const &placeholder, 
        String const &help, ReadValue read_value) const
    {
        detail::flag<String, OptionType, Option, Value> f {
            short_names, long_names, placeholder, help, {}, std::move(read_value) };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <OptionType Option, class Value, class ReadValue>
    auto flag(
        String const &short_names, String const &long_names, String const &placeholder, 
        String const &help, Value const &default_value, ReadValue read_value) const
    {
        detail::flag<String, OptionType, Option, Value> f {
            short_names, long_names, placeholder, help, std::make_shared<Value>(default_value),
            std::move(read_value) };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <OptionType Option, class Command>
    auto subcommand(String const &name, String const &help, Command const &command) const
    {
        detail::subcommand<String, OptionType, Option, Command> s { name, help, command };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, m_flags, detail::tuple_append(m_subcommands, std::move(s)),
            m_arguments);
    }

    template <OptionType Option, class Value>
    auto argument(String const &placeholder) const
    {
        detail::argument<String, OptionType, Option, Value> a { placeholder, {} };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, m_flags, m_subcommands,
            detail::tuple_append(m_arguments, std::move(a)));
    }

    template <OptionType Option, class Value, class ReadValue>
    auto argument(String const &placeholder, ReadValue read_value) const
    {
        detail::argument<String, OptionType, Option, Value> a {
            placeholder, std::move(read_value) };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, m_flags, m_subcommands,
            detail::tuple_append(m_arguments, std::move(a)));
    }

private:
    friend basic_command detail::make_command<>(
        String const &, String const &, Flags const &, Subcommands const &, Arguments const &);

    basic_command(
        String const &header, String const &footer, Flags const &flags,
        Subcommands const &subcommands, Arguments const &arguments) :
        m_header(header), m_footer(footer), m_flags(flags), m_subcommands(subcommands),
        m_arguments(arguments)
    {}

    String m_header;
    String m_footer;
    Flags m_flags;
    Subcommands m_subcommands;
    Arguments m_arguments;
};

template <class OptionType> using command = basic_command<std::string, OptionType>;
template <class OptionType> using wcommand = basic_command<std::wstring, OptionType>;

namespace detail {

template <class OptionType, OptionType Option, class Value>
struct option_pair
{
    std::shared_ptr<Value> value;
};

} // namespace detail

template <class OptionType, class ...Pairs>
class option_map : private Pairs...
{
public:
    template <OptionType Option>
    bool has() const { return static_cast<bool>(get_shared<Option>()); }

    template <OptionType Option>
    auto get() const { return *get_shared<Option>(); }

    template <OptionType Option>
    auto get_shared() const { return get_shared_impl<Option>(*this); }

private:
    template <OptionType Option, class Value>
    static auto get_shared_impl(detail::option_pair<OptionType, Option, Value> const &p)
    {
        return std::shared_ptr<Value const>(p.value);
    }
};

namespace detail {

template <class Command>
struct to_option_map;

template <class>
struct to_option_pair;

template <class String, class OptionType, OptionType Option, class Value>
struct to_option_pair<flag<String, OptionType, Option, Value>>
{
    using type = option_pair<OptionType, Option, Value>;
};

template <class String, class OptionType, OptionType Option, class Command>
struct to_option_pair<subcommand<String, OptionType, Option, Command>>
{
    using type = option_pair<OptionType, Option, typename to_option_map<Command>::type>;
};

template <class String, class OptionType, OptionType Option, class Value>
struct to_option_pair<argument<String, OptionType, Option, Value>>
{
    using type = option_pair<OptionType, Option, Value>;
};

template <class String, class OptionType, class ...Flags, class ...Subcommands, class ...Arguments>
struct to_option_map<basic_command<
    String, OptionType, std::tuple<Flags...>, std::tuple<Subcommands...>, std::tuple<Arguments...>>>
{
    using type = option_map<
        OptionType, typename to_option_pair<Flags>::type...,
        typename to_option_pair<Subcommands>::type...,
        typename to_option_pair<Arguments>::type...>;
};

} // namespace detail

template <class String>
class basic_error : public std::exception
{
public:
    explicit basic_error(String const &message) : m_message(message) {}

    char const *what() const noexcept override { return "nonsugar::basic_error"; }
    String message() const { return m_message; }

private:
    String m_message;
};

using error = basic_error<std::string>;
using werror = basic_error<std::wstring>;

template <class Iterator, class Command>
inline typename detail::to_option_map<Command>::type parse(
    Iterator arg_first, Iterator arg_last, Command const &command)
{
    return {};
}

template <class Char, class Command>
inline auto parse(int argc, Char * const *argv, Command const &command)
{
    return parse(argv + 1, argv + argc, command);
}

template <class Command>
inline typename Command::string_type usage(Command const &command)
{
    return {};
}

} // namespace nonsugar

#endif
