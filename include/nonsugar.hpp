
// nonsugar
//
// Copyright iorate 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NONSUGAR_HPP
#define NONSUGAR_HPP

#include <algorithm>
#include <cstddef>
#include <exception>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <regex>
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

template <class T>
struct optional_traits;

template <class T>
struct optional_traits<std::shared_ptr<T>>
{
    using value_type = T;
    static std::shared_ptr<T> make_optional(T const &t) { return std::make_shared<T>(t); }
};

template <class T>
struct optional_traits<boost::optional<T>>
{
    using value_type = T;
    static boost::optional<T> make_optional(T const &t) { return t; }
};

template <class String, class T, class = void>
struct value_of { using type = T; };

template <class String, class T>
struct value_of<String, T, std::enable_if_t<is_optional<T>::value>>
{
    using type = typename optional_traits<T>::value_type;
};

template <class String, class T>
struct value_of<
    String, T,
    std::enable_if_t<!std::is_same<String, T>::value && is_container<T>::value>>
{
    using type = typename T::value_type;
};

template <class String, class OptionType, OptionType Option, class Value>
struct flag
{
    using string_type = String;
    using option_type = OptionType;
    using value_type = Value;
    static constexpr OptionType option() { return Option; }

    std::vector<typename String::value_type> short_names;
    std::vector<String> long_names;
    String placeholder;
    String help;
    std::shared_ptr<Value> default_value;
    std::function<std::shared_ptr<typename value_of<String, Value>::type> (String const &)> read;
};

template <class String, class OptionType, OptionType Option, class Command>
struct subcommand
{
    using string_type = String;
    using option_type = OptionType;
    using command_type = Command;
    static constexpr OptionType option() { return Option; }

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
    static constexpr OptionType option() { return Option; }

    String placeholder;
    std::function<std::shared_ptr<typename value_of<String, Value>::type> (String const &)> read;
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

namespace detail {

template <class String>
[[noreturn]] void throw_error(char const *message)
{
    throw basic_error<String>(widen<String>(message));
}

} // namespace detail

template <class String, class T, class = void>
struct default_read
{
    std::shared_ptr<T> operator()(String const &s) const
    {
        T t;
        detail::sstream<String> ss(s);
        ss >> t;
        return ss && (ss.peek(), ss.eof()) ? std::make_shared<T>(t) : nullptr;
    }
};

template <class String>
struct default_read<String, void>
{
    std::shared_ptr<void> operator()(String const &) const { return nullptr; }
};

template <class String>
struct default_read<String, String>
{
    std::shared_ptr<String> operator()(String const &s) const
    {
        return std::make_shared<String>(s);
    }
};

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
    using char_type = typename String::value_type;
    using string_type = String;

    using flag_tuple_type = Flags;
    using subcommand_tuple_type = Subcommands;
    using argument_tuple_type = Arguments;

    basic_command(String const &header, String const &footer) : m_header(header), m_footer(footer)
    {}

    basic_command(
        String const &header, String const &footer, Flags const &flags,
        Subcommands const &subcommands, Arguments const &arguments) :
        m_header(header), m_footer(footer), m_flags(flags), m_subcommands(subcommands),
        m_arguments(arguments)
    {}

    template <OptionType Option>
    auto flag(
        std::initializer_list<char_type> short_names, std::initializer_list<String> long_names,
        String const &help) const
    {
        detail::flag<String, OptionType, Option, void> f {
            short_names, long_names, {}, help, {}, default_read<String, void>() };
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
            short_names, long_names, placeholder, help, {},
            default_read<String, typename detail::value_of<String, Value>::type>() };
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
            default_read<String, typename detail::value_of<String, Value>::type>() };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <
        OptionType Option, class Value, class Read,
        std::enable_if_t<!std::is_convertible<Read, Value>::value> * = nullptr>
    auto flag(
        std::initializer_list<char_type> short_names, std::initializer_list<String> long_names,
        String const &placeholder, String const &help, Read &&read) const
    {
        detail::flag<String, OptionType, Option, Value> f {
            short_names, long_names, placeholder, help, {}, std::forward<Read>(read) };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, detail::tuple_append(m_flags, std::move(f)), m_subcommands,
            m_arguments);
    }

    template <OptionType Option, class Value, class Read>
    auto flag(
        std::initializer_list<char_type> short_names, std::initializer_list<String> long_names,
        String const &placeholder, String const &help, Value const &default_value,
        Read &&read) const
    {
        detail::flag<String, OptionType, Option, Value> f {
            short_names, long_names, placeholder, help, std::make_shared<Value>(default_value),
            std::forward<Read>(read) };
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
        detail::argument<String, OptionType, Option, Value> a {
            placeholder, default_read<String, typename detail::value_of<String, Value>::type>() };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, m_flags, m_subcommands,
            detail::tuple_append(m_arguments, std::move(a)));
    }

    template <OptionType Option, class Value, class Read>
    auto argument(String const &placeholder, Read &&read) const
    {
        detail::argument<String, OptionType, Option, Value> a {
            placeholder, std::forward<Read>(read) };
        return detail::make_command<String, OptionType>(
            m_header, m_footer, m_flags, m_subcommands,
            detail::tuple_append(m_arguments, std::move(a)));
    }

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

template <class Command>
struct to_option_map;

} // namespace detail

template <class Iterator, class Command>
inline typename detail::to_option_map<Command>::type parse(Iterator, Iterator, Command const &);

template <class OptionType, class ...Pairs>
class option_map : private Pairs...
{
private:
    template <class Iterator, class Command>
    friend typename detail::to_option_map<Command>::type parse(Iterator, Iterator, Command const &);

    template <OptionType Option, class Value>
    static auto &priv_value_impl(detail::option_pair<OptionType, Option, Value> &p)
    {
        return p.value;
    }
    
    template <OptionType Option, class Value>
    static auto const &priv_value_impl(detail::option_pair<OptionType, Option, Value> const &p)
    {
        return p.value;
    }
    
    template <OptionType Option>
    auto const &priv_value() const { return priv_value_impl<Option>(*this); }

    template <OptionType Option>
    auto &priv_value() { return priv_value_impl<Option>(*this); }
    
public:
    template <OptionType Option>
    using type = typename std::remove_reference_t<
        decltype(priv_value_impl<Option>(std::declval<option_map const &>()))>::element_type;
    
    template <OptionType Option>
    bool has() const { return static_cast<bool>(priv_value<Option>()); }

    template <OptionType Option>
    auto get() const { return *priv_value<Option>(); }

    template <OptionType Option>
    auto get_shared() const { return priv_value<Option>(); }
};

namespace detail {

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

template <class String, class Value, class = void>
struct parse_flag_long
{
    template <class Iterator, class Flag>
    void operator()(
        std::match_results<typename String::const_iterator> const &match,
        Iterator &arg_it, Iterator arg_last,
        std::shared_ptr<Value> &value, Flag const &flg, String &err) const
    {
        String s;
        if (!match[2].matched) {
            ++arg_it;
            if (arg_it == arg_last) {
                err = widen<String>("argument required: ") + match.str(0);
                return;
            }
            s = *arg_it;
        } else {
            s = match.str(2);
        }
        auto const v = flg.read(s);
        if (!v) {
            err = widen<String>("invalid argument: --") + match.str(1) + widen<String>("=") + s;
            return;
        }
        value = v;
    }
};

template <class String>
struct parse_flag_long<String, void, void>
{
    template <class Iterator, class Flag>
    void operator()(
        std::match_results<typename String::const_iterator> const &match, Iterator &, Iterator,
        std::shared_ptr<void> &value, Flag const &, String &err) const
    {
        if (match[2].matched) {
            err = widen<String>("argument not allowed: ") + match.str(0);
            return;
        }
        value = std::make_shared<int>();
    }
};

template <class String, class Value>
struct parse_flag_long<String, Value, std::enable_if_t<is_optional<Value>::value>>
{
    template <class Iterator, class Flag>
    void operator()(
        std::match_results<typename String::const_iterator> const &match, Iterator &, Iterator,
        std::shared_ptr<Value> &value, Flag const &flg, String &err) const
    {
        if (!match[2].matched) {
            value = std::make_shared<Value>();
        } else {
            auto const v = flg.read(match.str(2));
            if (!v) {
                err = widen<String>("invalid argument: ") + match.str(0);
                return;
            }
            value = std::make_shared<Value>(optional_traits<Value>::make_optional(*v));
        }
    }
};

template <class String, class Value>
struct parse_flag_long<
    String, Value,
    std::enable_if_t<!std::is_same<String, Value>::value && is_container<Value>::value>>
{
    template <class Iterator, class Flag>
    void operator()(
        std::match_results<typename String::const_iterator> const &match,
        Iterator &arg_it, Iterator arg_last,
        std::shared_ptr<Value> &value, Flag const &flg, String &err) const
    {
        std::shared_ptr<typename Value::value_type> v;
        parse_flag_long<String, typename Value::value_type>()(
            match, arg_it, arg_last, v, flg, err);
        if (!err.empty()) return;
        if (!value) value = std::make_shared<Value>();
        value->push_back(std::move(*v));
    }
};

template <class String, class Value, class = void>
struct parse_flag_short
{
    template <class NameIt, class ArgIt, class Flag>
    void operator()(
        NameIt &it, NameIt last, ArgIt &arg_it, ArgIt arg_last,
        std::shared_ptr<Value> &value, Flag const &flg, String &err) const
    {
        auto const name = *it;
        String s;
        if (std::next(it) == last) {
            ++arg_it;
            if (arg_it == arg_last) {
                err = widen<String>("argument required: -") + name;
                return;
            }
            s = *arg_it;
        } else {
            s = String(std::next(it), last);
            it = std::prev(last);
        }
        auto const v = flg.read(s);
        if (!v) {
            err = widen<String>("invalid argument: -") + name + widen<String>(" ") + s;
            return;
        }
        value = v;
    }
};

template <class String>
struct parse_flag_short<String, void, void>
{
    template <class NameIt, class ArgIt, class Flag>
    void operator()(
        NameIt &, NameIt, ArgIt &, ArgIt,
        std::shared_ptr<void> &value, Flag const &, String &) const
    {
        value = std::make_shared<int>();
    }
};

template <class String, class Value>
struct parse_flag_short<String, Value, std::enable_if_t<is_optional<Value>::value>>
{
    template <class NameIt, class ArgIt, class Flag>
    void operator()(
        NameIt &it, NameIt last, ArgIt &arg_it, ArgIt arg_last,
        std::shared_ptr<Value> &value, Flag const &flg, String &err) const
    {
        auto const name = *it;
        if (std::next(it) == last) {
            value = std::make_shared<Value>();
        } else {
            auto const s = String(std::next(it), last);
            it = std::prev(last);
            auto const v = flg.read(s);
            if (!v) {
                err = widen<String>("invalid argument: -") + name + widen<String>(" ") + s;
                return;
            }
            value = std::make_shared<Value>(optional_traits<Value>::make_optional(*v));
        }
    }
};

template <class String, class Value>
struct parse_flag_short<
    String, Value,
    std::enable_if_t<!std::is_same<String, Value>::value && is_container<Value>::value>>
{
    template <class NameIt, class ArgIt, class Flag>
    void operator()(
        NameIt &it, NameIt last, ArgIt &arg_it, ArgIt arg_last,
        std::shared_ptr<Value> &value, Flag const &flg, String &err) const
    {
        std::shared_ptr<typename Value::value_type> v;
        parse_flag_short<String, typename Value::value_type>()(
            it, last, arg_it, arg_last, v, flg, err);
        if (!err.empty()) return;
        value = std::make_shared<Value>();
        value->push_back(std::move(*v));
    }
};

template <class String, class Value, class = void>
struct parse_argument
{
    template <class Iterator, class Argument>
    void operator()(
        Iterator &it, Iterator last, std::shared_ptr<Value> &value, Argument const &arg,
        String &err) const
    {
        if (it == last) {
            err = widen<String>("argument required: ") + arg.placeholder;
            return;
        }
        auto const v = arg.read(*it);
        if (!v) {
            err = widen<String>("bad argument: ") + arg.placeholder + widen<String>("=") + *it;
            return;
        }
        ++it;
        value = v;
    }
};

template <class String, class Value>
struct parse_argument<String, Value, std::enable_if_t<is_optional<Value>::value>>
{
    template <class Iterator, class Argument>
    void operator()(
        Iterator &it, Iterator last, std::shared_ptr<Value> &value, Argument const &arg,
        String &err) const
    {
        if (it == last) {
            value = std::make_shared<Value>();
        } else {
            auto const v = arg.read(*it);
            if (!v) {
                err = widen<String>("bad argument: ") + arg.placeholder + widen<String>("=") + *it;
                return;
            }
            ++it;
            value = std::make_shared<Value>(optional_traits<Value>::make_optional(*v));
        }
    }
};

template <class String, class Value>
struct parse_argument<
    String, Value,
    std::enable_if_t<!std::is_same<String, Value>::value && is_container<Value>::value>>
{
    template <class Iterator, class Argument>
    void operator()(
        Iterator &it, Iterator last, std::shared_ptr<Value> &value, Argument const &arg,
        String &err) const
    {
        value = std::make_shared<Value>();
        while (it != last) {
            std::shared_ptr<typename Value::value_type> v;
            parse_argument<String, typename Value::value_type>()(it, last, v, arg, err);
            if (!err.empty()) return;
            value->push_back(std::move(*v));
        }
    }
};

} // namespace detail

template <class Iterator, class Command>
inline typename detail::to_option_map<Command>::type parse(
    Iterator arg_first, Iterator arg_last, Command const &command)
{
    using string_type = typename Command::string_type;
    using error_type = basic_error<string_type>;
    using regex_type = std::basic_regex<typename string_type::value_type>;
    auto const _ = &detail::widen<string_type>;

    typename detail::to_option_map<Command>::type opts;
    detail::tuple_for_each(command.m_flags, [&](auto const &flg)
        {
            constexpr auto option = std::remove_reference_t<decltype(flg)>::option();
            opts.template priv_value<option>() = flg.default_value;
        });

    auto arg_it = arg_first;
    for (; arg_it != arg_last; ++arg_it) {
        auto const cur = *arg_it;
        if (cur == _("--")) {
            ++arg_it;
            break;
        }
        std::match_results<typename string_type::const_iterator> match;
        if (std::regex_match(cur, match, regex_type(_("--([^=]+)(?:=(.*))?")))) {
            // long flag
            auto const name = match.str(1);
            std::vector<string_type> exact_matches, partial_matches;
            detail::tuple_for_each(command.m_flags, [&](auto const &flg)
                {
                    for (auto const &long_name : flg.long_names) {
                        if (name == long_name) {
                            exact_matches.push_back(long_name);
                        }
                        else if (
                            name.size() <= long_name.size() &&
                            std::equal(name.begin(), name.end(), long_name.begin())) {
                            partial_matches.push_back(long_name);
                        }
                    }
                });
            if (exact_matches.empty()) {
                if (partial_matches.empty()) {
                    throw error_type(command.m_header + _(": unrecognized option: --") + name);
                } else if (partial_matches.size() >= 2) {
                    detail::sstream<string_type> ss;
                    ss << command.m_header << ": ambiguous option: --" << name << " [";
                    bool first = true;
                    for (auto const &pm : partial_matches) {
                        if (!std::exchange(first, false)) ss << ", ";
                        ss << "--" << pm;
                    }
                    ss << "]";
                    throw error_type(ss.str());
                }
            } else if (exact_matches.size() >= 2) {
                throw error_type(command.m_header + _(": ambiguous option: --") + name);
            }
            detail::tuple_for_each(command.m_flags, [&](auto const &flg)
                {
                    using flag_type = std::remove_cv_t<std::remove_reference_t<decltype(flg)>>;
                    using value_type = typename flag_type::value_type;
                    constexpr auto option = flag_type::option();
                    for (auto const &long_name : flg.long_names) {
                        if (name.size() <= long_name.size() &&
                            std::equal(name.begin(), name.end(), long_name.begin())) {
                            string_type err;
                            detail::parse_flag_long<string_type, value_type>()(
                                match, arg_it, arg_last,
                                opts.template priv_value<option>(), flg, err);
                            if (!err.empty()) {
                                throw error_type(command.m_header + _(": ") + err);
                            }
                        }
                    }
                });
        } else if (std::regex_match(cur, match, regex_type(_("-(.+)")))) {
            // short flag
            for (auto it = match[1].first; it != match[1].second; ++it) {
                auto const name = *it;
                std::vector<typename string_type::value_type> matches;
                detail::tuple_for_each(command.m_flags, [&](auto const &flg)
                    {
                        for (auto short_name : flg.short_names) {
                            if (name == short_name) {
                                matches.push_back(short_name);
                            }
                        }
                    });
                if (matches.empty()) {
                    throw error_type(command.m_header + _(": unrecognized option: -") + name);
                } else if (matches.size() >= 2) {
                    throw error_type(command.m_header + _(": ambiguous option: -") + name);
                }
                detail::tuple_for_each(command.m_flags, [&](auto const &flg)
                    {
                        using flag_type = std::remove_cv_t<std::remove_reference_t<decltype(flg)>>;
                        using value_type = typename flag_type::value_type;
                        constexpr auto option = flag_type::option();
                        if (std::find(flg.short_names.begin(), flg.short_names.end(), name) !=
                            flg.short_names.end()) {
                            string_type err;
                            detail::parse_flag_short<string_type, value_type>()(
                                it, match[1].second, arg_it, arg_last,
                                opts.template priv_value<option>(), flg, err);
                            if (!err.empty()) {
                                throw error_type(command.m_header + _(": ") + err);
                            }
                        }
                    });
            }
        } else {
            // subcommand or argument
            break;
        }
    }

    if (std::tuple_size<typename Command::subcommand_tuple_type>::value != 0 &&
        arg_it != arg_last) {
        bool given = false;
        detail::tuple_for_each(command.m_subcommands, [&](auto const &subcmd)
            {
                if (subcmd.name == *arg_it) {
                    if (std::exchange(given, true)) {
                        throw error_type(
                            command.m_header + _(": ambiguous subcommand: ") + *arg_it);
                    }
                }
            });
        if (!given) {
            throw error_type(command.m_header + _(": unrecognized subcommand: ") + *arg_it);
        }
        detail::tuple_for_each(command.m_subcommands, [&](auto const &subcmd)
            {
                if (subcmd.name == *arg_it) {
                    using subcommand_type = std::remove_cv_t<
                        std::remove_reference_t<decltype(subcmd)>>;
                    constexpr auto option = subcommand_type::option();
                    ++arg_it;
                    opts.template priv_value<option>() =
                        std::make_shared<typename decltype(opts)::template type<option>>(
                            parse(arg_it, arg_last, subcmd.command));
                }
            });
        return opts;
    }

    detail::tuple_for_each(command.m_arguments, [&](auto const &arg)
        {
            using argument_type = std::remove_cv_t<std::remove_reference_t<decltype(arg)>>;
            using value_type = typename argument_type::value_type;
            constexpr auto option = argument_type::option();
            string_type err;
            detail::parse_argument<string_type, value_type>()(
                arg_it, arg_last, opts.template priv_value<option>(), arg, err);
            if (!err.empty()) {
                throw error_type(command.m_header + _(": ") + err);
            }
        });

    if (arg_it != arg_last) {
        throw error_type(command.m_header + _(": unrecognized argument: ") + *arg_it);
    }

    return opts;
}

template <class Char, class Command>
inline auto parse(int argc, Char * const *argv, Command const &command)
{
    return parse(argv + 1, argv + argc, command);
}

namespace detail {

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
