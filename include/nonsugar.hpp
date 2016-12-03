
// nonsugar
//
// Copyright iorate 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NONSUGAR_HPP
#define NONSUGAR_HPP

#include <algorithm>
#include <exception>
#include <initializer_list>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace boost {

template <class T>
class optional;

} // namespace boost;

namespace nonsugar {

template <class T, class = void>
struct is_optional : std::false_type {};

template <class T>
struct is_optional<std::unique_ptr<T>> : std::true_type {};

template <class T>
struct is_optional<std::shared_ptr<T>> : std::true_type {};

template <class T>
struct is_optional<boost::optional<T>> : std::true_type {};

template <class T, class = void>
struct is_container : std::false_type {};

template <class T>
struct is_container<
    T,
    std::conditional_t<
        false,
        std::tuple<
            typename T::value_type,
            typename T::iterator,
            typename T::size_type,
            typename T::reference>,
        void>> : std::true_type
{};

namespace detail {

template <class String, class OptionType, OptionType Option, class Value>
struct flag
{
    using string_type = String;
    using option_type = OptionType;
    using value_type = Value;
    static constexpr OptionType option = Option;

    std::vector<typename String::value_type> short_names;
    std::vector<String> long_names;
    String placeholder;
    String help;
    std::shared_ptr<Value> default_value;
    std::function<void (std::shared_ptr<Value> &, std::shared_ptr<String> const &)> read_value;
};

template <class String, class OptionType, OptionType Option, class Command>
struct subcommand
{
    using string_type = String;
    using option_type = OptionType;
    using command_type = Command;
    static constexpr OptionType option = Option;

    String name;
    String help;
    Command command;
};

template <class String, class OptionType, OptionType Option, class Value>
struct argument
{
    using string_type = String;
    using option_type = OptionType;
    using value_type = Value;
    static constexpr OptionType option = Option;

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

template <class Command>
inline typename Command::string_type usage(Command const &);

template <class String, class OptionType, class Flags, class Subcommands, class Arguments>
class basic_command
{
public:
    using char_type = typename String::value_type;
    using string_type = String;

    basic_command(String const &header, String const &footer) : m_header(header), m_footer(footer)
    {}

    template <OptionType Option>
    auto flag(
        std::initializer_list<char_type> short_names, std::initializer_list<String> long_names,
        String const &help) const
    {
        detail::flag<String, OptionType, Option, void> f {
            short_names, long_names, {}, help, {}, {} };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <OptionType Option, class Value>
    auto flag(
        std::initializer_list<char_type> short_names, std::initializer_list<String> long_names,
        String const &placeholder, String const &help) const
    {
        detail::flag<String, OptionType, Option, Value> f {
            short_names, long_names, placeholder, help, {}, {} };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <OptionType Option, class Value>
    auto flag(
        std::initializer_list<char_type> short_names, std::initializer_list<String> long_names,
        String const &placeholder, String const &help, Value const &default_value) const
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
        std::initializer_list<char_type> short_names, std::initializer_list<String> long_names,
        String const &placeholder, String const &help, ReadValue &&read_value) const
    {
        detail::flag<String, OptionType, Option, Value> f {
            short_names, long_names, placeholder, help, {}, std::forward<ReadValue>(read_value) };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <OptionType Option, class Value, class ReadValue>
    auto flag(
        std::initializer_list<char_type> short_names, std::initializer_list<String> long_names,
        String const &placeholder, String const &help, Value const &default_value,
        ReadValue &&read_value) const
    {
        detail::flag<String, OptionType, Option, Value> f {
            short_names, long_names, placeholder, help, std::make_shared<Value>(default_value),
            std::forward<ReadValue>(read_value) };
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

    friend String usage<>(basic_command const &);

    using flag_tuple_type = Flags;
    using subcommand_tuple_type = Subcommands;
    using argument_tuple_type = Arguments;

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
        return std::const_pointer_cast<Value const>(p.value);
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

template <class String>
using sstream = std::basic_stringstream<typename String::value_type, typename String::traits_type>;

template <class String>
inline String widen(char const *s)
{
    sstream<String> ss;
    ss << s;
    return ss.str();
}

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

namespace detail {

struct eat
{
    template <class ...Args>
    constexpr eat(Args &&...) {}
};

template <class T, class F, std::size_t ...I>
inline void tuple_for_each_impl(T &&t, F &&f, std::index_sequence<I...>)
{
    eat { (f(std::get<I>(t)), void(), 0)... };
}

template <class T, class F>
inline void tuple_for_each(T &&t, F &&f)
{
    tuple_for_each_impl(
        std::forward<T>(t), std::forward<F>(f),
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<T>>::value>());
}

template <class String>
inline std::vector<String> lines(String const &s)
{
    sstream<String> ss(s);
    String line;
    std::vector<String> ret;
    while (std::getline(ss, line)) ret.push_back(std::move(line));
    return ret;
}

template <class Range, class String, class F>
inline String join_map(Range const &r, String const &sep, F &&f)
{
    String ret;
    bool first = true;
    for (auto const &e : r) {
        if (!std::exchange(first, false)) ret += sep;
        ret += f(e);
    }
    return ret;
}

} // namespace detail

template <class Command>
inline typename Command::string_type usage(Command const &command)
{
    using string_type = typename Command::string_type;
    detail::sstream<string_type> ss;
    auto const _ = &detail::widen<string_type>;

    // Usage
    ss << "Usage: " << command.m_header;
    if (std::tuple_size<typename Command::flag_tuple_type>::value > 0) {
        ss << " [OPTION...]";
    }
    if (std::tuple_size<typename Command::subcommand_tuple_type>::value > 0) {
        ss << " <COMMAND> [ARGS...]";
    }
    detail::tuple_for_each(command.m_arguments, [&](auto const &arg)
        {
            using value_type = typename std::remove_reference_t<decltype(arg)>::value_type;
            if (is_optional<value_type>::value) {
                ss << " [" << arg.placeholder << "]";
            } else if (
                !std::is_same<value_type, string_type>::value && is_container<value_type>::value) {
                ss << " [" << arg.placeholder << "...]";
            } else {
                ss << " " << arg.placeholder;
            }
        });
    ss << "\n";
    for (auto const &f : detail::lines(command.m_footer)) {
        ss << "  " << f << "\n";
    }

    // Flags
    if (std::tuple_size<typename Command::flag_tuple_type>::value > 0) {
        ss << "\nOptions:\n";
        std::vector<std::tuple<string_type, string_type, string_type>> table;
        typename string_type::size_type c0_max = 0, c1_max = 0;
        detail::tuple_for_each(command.m_flags, [&](auto const &flg)
            {
                using value_type = typename std::remove_reference_t<decltype(flg)>::value_type;
                auto const short_ = detail::join_map(flg.short_names, _(", "), [&](auto const c)
                    {
                        if (std::is_void<value_type>::value) {
                            return _("-") + c;
                        } else if (is_optional<value_type>::value) {
                            return _("-") + c + _("[") + flg.placeholder + _("]");
                        } else {
                            return _("-") + c + _(" ") + flg.placeholder;
                        }
                    });
                auto const long_ = detail::join_map(flg.long_names, _(", "), [&](auto const &s)
                    {
                        if (std::is_void<value_type>::value) {
                            return _("--") + s;
                        } else if (is_optional<value_type>::value) {
                            return _("--") + s + _("[=") + flg.placeholder + _("]");
                        } else {
                            return _("--") + s + _("=") + flg.placeholder;
                        }
                    });
                auto help = detail::lines(flg.help);
                if (help.empty()) help.push_back(string_type());
                bool first = true;
                for (auto const &h : help) {
                    if (std::exchange(first, false)) table.emplace_back(short_, long_, h);
                    else table.emplace_back(string_type(), string_type(), h);
                }
                c0_max = std::max(c0_max, short_.size());
                c1_max = std::max(c1_max, long_.size());
            });
        for (auto const &row : table) {
            ss << "  " << std::get<0>(row);
            for (auto i = std::get<0>(row).size(); i < c0_max; ++i) ss << " ";
            ss << "  " << std::get<1>(row);
            for (auto i = std::get<1>(row).size(); i < c1_max; ++i) ss << " ";
            ss << "  " << std::get<2>(row) << "\n";
        }
    }

    // Subcommands
    if (std::tuple_size<typename Command::subcommand_tuple_type>::value > 0) {
        ss << "\nCommands:\n";
        std::vector<std::tuple<string_type, string_type>> table;
        typename string_type::size_type c0_max = 0;
        detail::tuple_for_each(command.m_subcommands, [&](auto const &subcmd)
            {
                auto help = detail::lines(subcmd.help);
                if (help.empty()) help.push_back(string_type());
                bool first = true;
                for (auto const &h : help) {
                    if (std::exchange(first, false)) table.emplace_back(subcmd.name, h);
                    else table.emplace_back(string_type(), h);
                }
                c0_max = std::max(c0_max, subcmd.name.size());
            });
        for (auto const &row : table) {
            ss << "  " << std::get<0>(row);
            for (auto i = std::get<0>(row).size(); i < c0_max; ++i) ss << " ";
            ss << "  " << std::get<1>(row) << "\n";
        }
    }

    return ss.str();
}

} // namespace nonsugar

#endif
