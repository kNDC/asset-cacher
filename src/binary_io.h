#pragma once
#include <cstdint>
#include <fstream>
#include <vector>

template <typename T>
T ReadPrimitive(std::ifstream& ifs)
{
    T out;
    ifs.read((char*)&out, sizeof(out));
    return out;
}

template <typename T>
void WritePrimitive(std::ofstream& ofs, T t)
{
    ofs.write((char*)&t, sizeof(t));
}

template <typename T>
std::string ReadString(std::ifstream& ifs)
{
    T size = ReadPrimitive<T>(ifs);
    std::vector<char> buffer(size);
    ifs.read(buffer.data(), size);

    return { buffer.begin(), buffer.end() };
}

inline std::string ReadShortString(std::ifstream& ifs)
{
    return ReadString<uint8_t>(ifs);
}

template <typename T>
void WriteString(std::ofstream& ofs, const std::string& string)
{
    T size = (T)string.size();
    ofs.write((char*)&size, sizeof(size));
    ofs.write(string.data(), size);
}

inline void WriteShortString(std::ofstream& ofs, const std::string& string)
{
    WriteString<uint8_t>(ofs, string);
}

inline std::string ReadFixedSizeString(std::ifstream& ifs, 
    unsigned int size)
{
    std::vector<char> buffer(size);
    ifs.read(buffer.data(), size);

    std::vector<char>::iterator to = buffer.begin();
    while (to != buffer.end() && *to != '\0') ++to;
    
    return { buffer.begin(), to };
}