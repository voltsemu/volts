#include "backed.h"

namespace svl
{
    std::string backed_value::operator=(const std::string& v)
    {
        val = v;
        (*parent)[name] = val;
    }

    backed::backed(const fs::path& p)            
        : path(p)
    {
        load();
    }

    backed::~backed()
    {
        save();
    }

    backed_value backed::operator[](const std::string& val)
    {
        backed_value bval;
        bval.parent = this;
        bval.name = val;
        bval.val = data[val];

        return bval;
    }

    load_error backed::load()
    {
        auto file = open(path, mode::read);
        if(!file.valid())
            return load_error::file;
            
        std::string name;
        std::string val;
        for(;;)
        {
            // read until first space to get the key
            char c;
            while((c = file.read<char>()))
            {
                if(c == '=')
                    break;

                if(c == ' ')
                {
                    while(file.read<char>() != '=');
                    break;
                }

                name += c;
            }

            while((c = file.read<char>()))
            {
                if(c == ';')
                    break;

                val += c;
            }

            // TODO: whitespace
            data[name] = val;

            // TODO: better eof checking
            if(c == -1)
                goto end;
        }
    
    end:return load_error::ok;
    }

    void backed::save()
    {
        auto file = open(path, mode::write);
        for(auto& [key, val] : data)
        {
            file.write(key);
            file.write(" = ");
            file.write(val);
            file.write(";;\n");
        }
    }
}