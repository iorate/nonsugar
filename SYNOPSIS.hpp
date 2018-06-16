namespace nonsugar {

/* Errors */

template <class String>
class basic_error : public std::exception
{
public:
    using string_type = String;

    explicit basic_error(String const &message);

    String message() const;
};

using error = basic_error<std::string>;
using werror = basic_error<std::wstring>;
using u16error = basic_error<std::u16string>;
using u32error = basic_error<std::u32string>;

/* Enumerations */

enum class flag_type
{
    normal,
    exclusive
};

enum class argument_order
{
    strict,
    flexible
};

/* Commands */

template <
    class String,
    class OptionType,
    class = unspecified, class = unspecified, class = unspecified
    >
class basic_command
{
public:
    using char_type = typename String::value_type;
    using string_type = String;
    using option_type = OptionType;

    explicit basic_command(
        String const &header,
        String const &footer = String()
        );

    template <OptionType Option, class Value = void>
    basic_command<String, OptionType, unspecified, unspecified, unspecified>
    flag(
        std::initializer_list<char_type> short_names,
        std::initializer_list<String> long_names,
        String const &placeholder,
        String const &help,
        flag_type type = flag_type::normal
        ) const;

    template <OptionType Option, class Value>
    basic_command<String, OptionType, unspecified, unspecified, unspecified>
    flag(
        std::initializer_list<char_type> short_names,
        std::initializer_list<String> long_names,
        String const &placeholder,
        String const &help,
        Value const &default_value,
        flag_type type = flag_type::normal
        ) const;

    template <OptionType Option, class Command>
    basic_command<String, OptionType, unspecified, unspecified, unspecified>
    subcommand(
        String const &name,
        String const &help,
        Command const &command
        ) const;

    template <OptionType Option, class Value>
    basic_command<String, OptionType, unspecified, unspecified, unspecified>
    argument(
        String const &placeholder
        ) const;
};

template <class OptionType> using command = basic_command<std::string, OptionType>;
template <class OptionType> using wcommand = basic_command<std::wstring, OptionType>;
template <class OptionType> using u16command = basic_command<std::u16string, OptionType>;
template <class OptionType> using u32command = basic_command<std::u32string, OptionType>;

/* Option Maps */

template <class OptionType, class ...>
class option_map
{
public:
    using option_type = OptionType;

    template <OptionType Option>
    using type = unspecified;

    template <OptionType Option>
    bool has() const;

    template <OptionType Option>
    type<Option> get() const;

    template <OptionType Option>
    std::shared_ptr<type<Option>> get_shared() const;

    // C++17 only
    template <OptionType Option>
    std::optional<type<Option>> get_optional() const;
};

/* Parsers */

template <class Iterator, class Command>
option_map<typename Command::option_type, unspecified...>
parse(
    Iterator begin,
    Iterator end,
    Command const &command,
    argument_order order = argument_order::strict
    );

template <class Char, class Command>
option_map<typename Command::option_type, unspecified...>
parse(
    int argc,
    Char * const *argv,
    Command const &command,
    argument_order order = argument_order::strict
    );

// C++17 only
template <class Iterator, class Command>
std::optional<option_map<typename Command::option_type, unspecified...>>
parse_optional(
    Iterator begin,
    Iterator end,
    Command const &command,
    typename Command::string_type &message
    );

// C++17 only
template <class Iterator, class Command>
std::optional<option_map<typename Command::option_type, unspecified...>>
parse_optional(
    Iterator begin,
    Iterator end,
    Command const &command,
    argument_order order,
    typename Command::string_type &message
    );

// C++17 only
template <class Char, class Command>
std::optional<option_map<typename Command::option_type, unspecified...>>
parse_optional(
    int argc,
    Char * const *argv,
    Command const &command,
    typename Command::string_type &message
    );

// C++17 only
template <class Char, class Command>
std::optional<option_map<typename Command::option_type, unspecified...>>
parse_optional(
    int argc,
    Char * const *argv,
    Command const &command,
    argument_order order,
    typename Command::string_type &message
    );

/* Usage Generators */

template <class Command>
typename Command::string_type usage(Command const &command);


/* Customization Points */

template <class T, class = void>
struct optional_traits {};

template <class T>
struct optional_traits<std::shared_ptr<T>>
{
    using value_type = T;
    static std::shared_ptr<T> construct(T const &v);
};

template <class T>
struct optional_traits<boost::optional<T>>
{
    using value_type = T;
    static boost::optional<T> construct(T const &v);
};

// C++17 only
template <class T>
struct optional_traits<std::optional<T>>
{
    using value_type = T;
    static std::optional<T> construct(T const &v);
};

template <class T, class = void>
struct container_traits {};

template <class T>
struct container_traits<T, unspecified>
{
    using value_type = typename T::value_type;
    static void push_back(T &t, value_type const &v);
};

template <class String, class T, class = void>
struct value_read
{
    std::shared_ptr<T> operator()(String const &s) const;
};

template <class String>
struct value_read<String, String>
{
    std::shared_ptr<String> operator()(String const &s) const;
};

} // namespace nonsugar
