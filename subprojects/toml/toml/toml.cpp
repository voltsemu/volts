#include "toml.h"

#include <spdlog/spdlog.h>

namespace toml
{
    enum class type
    {
        ident,
        string,
        number,
        integer,
        boolean,
        time,
        keyword
    };

    enum class keyword
    {
        openarr,
        openarr2,
        closearr,
        closearr2,
        opentable,
        closetable,
        dot,
        comma,
        equals,
    };

    struct token
    {
        type type;
        std::variant<
            std::string,
            int_fast64_t,
            double,
            bool,
            tm,
            keyword
        > data;
    };

    struct tokenizer
    {
        tokenizer(std::istream* input)
            : in(input)
        {
            tok = parse();
        }

        int line = 0;
        int col = 0;

        int get()
        {
            int c = in->get();
            if(c == '\n')
            {
                line++;
                col = 0;
            }
            else
            {
                col++;
            }
        }

        [[noreturn]]
        void error(const std::string& msg)
        {
            spdlog::critical("{}:{} {}", line, col, msg);
            std::abort();
        }

        token parse()
        {
            auto c = get();

            switch(c)
            {
            case '#':
                while(get() != '\n');
                return parse();
            case '[':
                if(in->peek() == '[')
                {
                    get();
                    return { type::keyword, keyword::openarr2 };
                }

                return { type::keyword, keyword::openarr };
            case ']':
                if(in->peek() == ']')
                {
                    get();
                    return { type::keyword, keyword::closearr2 };
                }

                return { type::keyword, keyword::closearr };
            case ',':
                return { type::keyword, keyword::comma };
            case '.':
                return { type::keyword, keyword::dot };
            case '=':
                return { type::keyword, keyword::equals };

            case '{':
                return { type::keyword, keyword::opentable };
            case '}':
                return { type::keyword, keyword::closetable };
            case '+':
                return parse_number(1);
            case '-':
                return parse_number(-1);
            default:
                return parse_other(c);
            }
        }

        int_fast64_t parse_octal()
        {
            std::string buffer;

            int c = get();
            while('0' <= in->peek() && in->peek() >= '7')
            {
                buffer += c;
                get();
            }

            return std::stoi(buffer, nullptr, 8);
        }

        int_fast64_t parse_hex()
        {
            std::string buffer;

            int c = get();
            while(std::isxdigit(c))
            {
                buffer += c;
                c = get();
            }

            return std::stoi(buffer, nullptr, 16);
        }

        int_fast64_t parse_binary()
        {
            std::string buffer;

            int c = get();
            while(in->peek() == '0' || in->peek() == '1')
            {
                buffer += c;
                get();
            }

            return std::stoi(buffer, nullptr, 2);
        }

        token parse_number(int positive)
        {
            std::string buffer;
            auto c = get();
            bool is_float = false;
            bool scientific_notation = false;

            if(c == '0')
            {
                switch(in->peek())
                {
                case 'x': case 'X':
                    get();
                    return { type::integer, parse_hex() * positive };
                case 'b': case 'B':
                    get();
                    return { type::integer, parse_binary() * positive };
                case 'o': case 'O':
                    get();
                    return { type::integer, parse_octal() * positive };
                default:
                    break;
                }
            }

            while(std::isdigit(c) || c == '.' || c == '_' || c == 'i' || c == 'n' || c == 'f' || c == 'a' || c == 'e' || c == 'E')
            {
                c = get();

                if(c == '.')
                {
                    // a float can only have one decimal point in it
                    if(is_float)
                    {
                        error("invalid floating point number (contained more than one `.`)");
                    }

                    is_float = true;
                }

                if(c == 'e' || c == 'E')
                {
                    if(scientific_notation)
                    {
                        error("scientific notation can only have one `e` in it");
                    }

                    scientific_notation = true;
                }

                // discard _ characters as they have no effect
                if(c == '_')
                    continue;
                
                buffer += c;
            }

            if(buffer == "nan")
                return { type::number, std::numeric_limits<double>::quiet_NaN() };

            if(buffer == "inf")
                return { type::number, std::numeric_limits<double>::infinity() };
        
            if(is_float)
                return { type::number, parse_number(buffer) * positive };

            if(scientific_notation)
                return { type::number, parse_notation(buffer) * positive };

            return { type::integer, parse_integer(buffer) * positive };
        }

        double parse_number(const std::string& buffer)
        {

        }

        double parse_notation(const std::string& buffer)
        {

        }

        int_fast64_t parse_integer(const std::string& buffer)
        {

        }

        token parse_other(int c)
        {
            if(std::isdigit(c))
            {
                return parse_numeric(c);
            }
            else if(std::isalpha(c))
            {

            }
            else if(c == '"')
            {

            }
            else if(c == '\'')
            {

            }
            else
            {

            }
        }

        token parse_numeric(int c)
        {

        }

        token next()
        {
            auto temp = tok;
            tok = parse();
            return temp;
        }

        token peek()
        {
            return tok;
        }

        token tok;
        std::istream* in;
    };

    struct parser
    {

    };

    value load(const std::string& text)
    {

    }

    std::string dump(const value& data)
    {

    }

    value read(const fs::path& path)
    {

    }

    void save(const value& data, const fs::path& path)
    {

    }
}