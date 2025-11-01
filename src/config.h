#pragma once
#include "json.h"
#include "io_traits.h"

#include <cstdint>
#include <iostream>
#include <conio.h>

#include <string>
#include <vector>

template <typename T>
struct Config
{
    // What must be done with missing inputs?
    enum class InputPolicy : uint8_t
    {
        Relaxed = 0, // Do nothing.
        Informative, // Log them.
        Pedantic // Omit them altogether.
    };
    
    bool show_settings = true;
    bool incremental = false;
    InputPolicy input_policy = InputPolicy::Informative;

private:
    template <typename Str>
    static bool PostBinaryPrompt(const Str& message, 
        std::basic_ostream<T>&);
    
    InputPolicy PostPolicyPrompt(std::basic_ostream<T>&);

public:
    Config() = default;
    ~Config() = default;

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
bool Config<T>::PostBinaryPrompt(const Str& message, 
    std::basic_ostream<T>& os)
{
    using namespace std::string_view_literals;

    os << message << '\n';
    os << "Press Y/y/Enter for Yes: "sv;

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
typename Config<T>::InputPolicy 
Config<T>::PostPolicyPrompt(std::basic_ostream<T>& os)
{
    using namespace std::string_view_literals;

    os << "Please select a policy for chunks' missing inputs:"sv << '\n';
    os << " *Relaxed: chunks' missing inputs are treated without warnings\n"sv;
    os << " *Informative: chunks' missing inputs are logged\n"sv;
    os << " *Pedantic: chunks' missing inputs are skipped\n"sv;

    os << "Press R/r for Relaxed, I/i for Informative, P/p for Pedantic: "sv;

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
void Config<T>::Read(S&& s)
{
    using namespace std::literals;

    std::basic_ifstream<T> ifs(std::forward<S>(s));
    if (!ifs.is_open()) return;

    json::Dict<T> json_config;
    {
        // Are the options in the right format (json::Dict)?
        json::Document json_doc = json::Load(ifs);

        if (!json_doc.GetRoot().IsMap()) return;
        json_config.swap(json_doc.GetRoot().AsMap());
    }

    typename json::Dict<T>::const_iterator pos;

    pos = json_config.find("Incremental"s);
    if (pos != json_config.end())
    {
        incremental = pos->second;
    }

    pos = json_config.find("Show settings"s);
    if (pos != json_config.end())
    {
        show_settings = pos->second;
    }
    
    pos = json_config.find("Input policy"s);
    if (pos != json_config.end())
    {
        std::basic_string<T> policy = pos->second.AsString();
        for (char& c : policy) c = std::towlower(c);
        
        if (policy == "relaxed"sv) input_policy = InputPolicy::Relaxed;
        else if (policy == "informative"sv) input_policy = InputPolicy::Informative;
        else if (policy == "pedantic"sv) input_policy = InputPolicy::Pedantic;
    }
}

template <typename T>
void Config<T>::Read()
{
    using namespace std::string_literals;
    Read(".//config.json"s);
}

template <typename T>
template <typename S>
void Config<T>::Save(S&& s)
{
    using namespace std::string_literals;

    json::Document<T> json_doc{json::Dict<T>{}};
    json::Dict<T>& json_config = json_doc.GetRoot().AsMap();

    json_config["Incremental"s] = incremental;
    json_config["Show settings"s] = show_settings;

    switch (input_policy)
    {
    case InputPolicy::Relaxed:
        json_config["Input policy"s] = "Relaxed"s;
        break;
    
    case InputPolicy::Informative:
        json_config["Input policy"s] = "Informative"s;
        break;
    
    case InputPolicy::Pedantic:
        json_config["Input policy"s] = "Pedantic"s;
        break;
    
    default:
        break;
    }

    std::basic_ofstream<T> ofs(std::forward<S>(s));
    json_doc.Print(ofs);
}

template <typename T>
void Config<T>::Save()
{
    using namespace std::string_literals;
    Save(".//config.json"s);
}

template <typename T>
void Config<T>::Set(std::basic_istream<T>& is, 
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

    if (!PostBinaryPrompt("Would you like to edit the settings?"sv, 
        os)) return;

    incremental = 
        PostBinaryPrompt("Append data to an existing .dat file?"sv, 
        os);
    show_settings = 
        PostBinaryPrompt("Show the settings menu next time?"sv, 
        os);
    input_policy = PostPolicyPrompt(os);
}