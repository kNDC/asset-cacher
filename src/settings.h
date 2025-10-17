#pragma once
#include "json.h"
#include "io_traits.h"

#include <cstdint>
#include <iostream>
#include <conio.h>
#include <string>
#include <vector>

template <typename T>
struct Settings
{
    // What must be done with missing inputs?
    enum class InputPolicy : uint8_t
    {
        Relaxed = 0, // nothing
        Informative, // log them
        Pedantic // omit them altogether
    };
    
    bool show_settings = true;
    bool incremental = false;
    InputPolicy input_policy = InputPolicy::Informative;

private:
    template <typename Str>
    static bool PostMessageYN(const Str& message, 
        std::basic_ostream<T>&, 
        const Str& prefix = {});
    
    template <typename Str>
    InputPolicy PostMessagePolicy(std::basic_ostream<T>&, 
        const Str& prefix = {});

public:
    Settings() = default;
    ~Settings() = default;

    template <typename S>
    void Read(S&&);
    void Read();

    template <typename S>
    void Save(S&&);
    void Save();

    void Set(std::basic_istream<T>& is = traits<T>::tcin, 
        std::basic_ostream<T>& os = traits<T>::tcout);
};

template <typename T>
template <typename Str>
bool Settings<T>::PostMessageYN(const Str& message, 
    std::basic_ostream<T>& os, 
    const Str& prefix)
{
    using namespace std::string_view_literals;

    os << prefix << message << '\n';
    os << prefix << "Press Y/y/Enter for Yes: "sv;

    T response = _getch();

    switch (response)
    {
    case 'Y':
    case 'y':
    case '\r':
        os << "Yes\n\n"sv;
        return true;
        
    default:
        os << "No\n\n"sv;
        return false;
    }
}

template <typename T>
template <typename Str>
typename Settings<T>::InputPolicy 
Settings<T>::PostMessagePolicy(std::basic_ostream<T>& os, 
    const Str& prefix)
{
    using namespace std::string_view_literals;

    os << prefix << "Please select a policy for chunks' missing inputs:"sv << '\n';
    os << prefix << " *Relaxed: chunks' missing inputs are treated without warnings\n"sv;
    os << prefix << " *Informative: chunks' missing inputs are logged\n"sv;
    os << prefix << " *Pedantic: chunks' missing inputs are skipped\n"sv;

    os << prefix << "Press R/r for Relaxed, I/i for Informative, P/p for Pedantic: "sv;

    T response = _getch();

    switch (response)
    {
    case 'R':
    case 'r':
        os << "Relaxed\n\n"sv;
        return InputPolicy::Relaxed;
    
    case 'I':
    case 'i':
        os << "Informative\n\n"sv;
        return InputPolicy::Informative;
    
    case 'P':
    case 'p':
        os << "Pedantic\n\n"sv;
        return InputPolicy::Pedantic;
    
    default:
        switch (input_policy)
        {
        case InputPolicy::Relaxed:
            os << "Relaxed"sv;
            break;
            
        case InputPolicy::Informative:
            os << "Informative"sv;
            break;
            
        case InputPolicy::Pedantic:
            os << "Pedantic"sv;
            break;
        }

        os << " (current choice)\n\n"sv;
        return input_policy;
}
}

template <typename T>
template <typename S>
void Settings<T>::Read(S&& s)
{
    using namespace std::literals;

    std::basic_ifstream<T> ifs(std::forward<S>(s));
    if (!ifs.is_open()) return;
    
    json::Document<T> json_settings = json::Load(ifs);

    if (!json_settings.GetRoot().IsMap()) return;

    typename json::Dict<T>::const_iterator pos;

    pos = json_settings.GetRoot().AsMap().find("Incremental"s);
    if (pos != json_settings.GetRoot().AsMap().end())
    {
        incremental = pos->second;
    }

    pos = json_settings.GetRoot().AsMap().find("Show settings"s);
    if (pos != json_settings.GetRoot().AsMap().end())
    {
        show_settings = pos->second;
    }
    
    pos = json_settings.GetRoot().AsMap().find("Input policy"s);
    if (pos != json_settings.GetRoot().AsMap().end())
    {
        std::basic_string_view<T> policy = pos->second.AsString();
        
        if (policy == "Relaxed"sv || policy == "relaxed"sv)
        {
            input_policy = InputPolicy::Relaxed;
        }
        else if (policy == "Informative"sv || policy == "informative"sv)
        {
            input_policy = InputPolicy::Informative;
        }
        else if (policy == "Pedantic"sv || policy == "pedantic"sv)
        {
            input_policy = InputPolicy::Pedantic;
        }
    }
}

template <typename T>
void Settings<T>::Read()
{
    using namespace std::string_literals;
    Read(".//settings.json"s);
}

template <typename T>
template <typename S>
void Settings<T>::Save(S&& s)
{
    using namespace std::string_literals;

    json::Document<T> json_settings{json::Dict<T>{}};
    json_settings.GetRoot().AsMap()["Incremental"s] = incremental;
    json_settings.GetRoot().AsMap()["Show settings"s] = show_settings;

    switch (input_policy)
    {
    case InputPolicy::Relaxed:
        json_settings.GetRoot().AsMap()["Input policy"s] = "Relaxed"s;
        break;
    
    case InputPolicy::Informative:
        json_settings.GetRoot().AsMap()["Input policy"s] = "Informative"s;
        break;
    
    case InputPolicy::Pedantic:
        json_settings.GetRoot().AsMap()["Input policy"s] = "Pedantic"s;
        break;
    
    default:
        break;
    }

    std::basic_ofstream<T> ofs(std::forward<S>(s));
    json_settings.Print(ofs);
}

template <typename T>
void Settings<T>::Save()
{
    using namespace std::string_literals;
    Save(".//settings.json"s);
}

template <typename T>
void Settings<T>::Set(std::basic_istream<T>& is, 
    std::basic_ostream<T>& os)
{
    using namespace std::string_view_literals;
    
    if (!show_settings) return;
    
    os << "**Current settings:\n"sv;
    os << "  Append data to an existing file = "sv 
       << (incremental ? "Yes"sv : "No"sv) << '\n';
    
    os << "  Input policy = "sv;
    switch (input_policy)
    {
    case InputPolicy::Relaxed:
        os << "Relaxed: chunks' missing inputs are treated without warnings\n"sv;
        break;
    
    case InputPolicy::Informative:
        os << "Informative: chunks' missing inputs are logged\n"sv;
        break;
    
    case InputPolicy::Pedantic:
        os << "Pedantic: chunks' missing inputs are skipped\n"sv;
        break;
    
    default:
        break;
    }
    os << "\n";

    if (!PostMessageYN("Would you like to edit the settings?"sv, 
        os)) return;

    incremental = 
        PostMessageYN("Append data to an existing .dat file?"sv, 
        os);
    show_settings = 
        PostMessageYN("Show the settings menu next time?"sv, 
        os);
    input_policy = PostMessagePolicy(os, ""sv);
}