#pragma once
#include "container_utils.h"
#include <fstream>
#include <vector>

namespace augmented
{
    template <typename T>
    struct basic_ifstream
    {
    private:
        std::basic_string<T> file_path_{};
        std::basic_string_view<T> file_stem_{};
        std::basic_string_view<T> file_ext_{};
        std::basic_ifstream<T> ifs_{};

    private:
        void SetStem();
        void SetExtension();

    public:
        basic_ifstream() = default;

        template <typename S>
        basic_ifstream(S&&, std::ios::openmode);

        template <typename S, typename Container>
        void Open(S&&,
            std::ios::openmode,
            const Container& formats = {},
            bool binary_search = false);

        std::basic_ifstream<T>& FS() { return ifs_; }
        const std::basic_string<T>& FilePath() const { return file_path_; }
        const std::basic_string_view<T>& FileStem() const { return file_stem_; }
        const std::basic_string_view<T>& FileExt() const { return file_ext_; }
    };

    template <typename T>
    void basic_ifstream<T>::SetStem()
    {
        size_t pos = file_path_.find_last_of('\\');
        pos = (pos == std::string::npos) ? 0 : pos;

        file_stem_ = file_path_;
        file_stem_ = file_stem_.substr(pos + 1);
    }

    template <typename T>
    void basic_ifstream<T>::SetExtension()
    {
        size_t pos = file_path_.find_last_of('.');
        pos = (pos == std::string::npos) ? 0 : pos;

        file_ext_ = file_path_;
        file_ext_ = file_ext_.substr(pos);
    }

    template <typename T>
    template <typename S>
    basic_ifstream<T>::basic_ifstream(S&& s,
        std::ios_base::openmode mode) :
        file_path_{ std::forward<S>(s) },
        ifs_{ file_path_, mode }
    {
        SetStem();
        SetExtension();
    }

    template <typename T>
    template <typename S, typename Container>
    void basic_ifstream<T>::Open(S&& s,
        std::ios::openmode mode,
        const Container& formats,
        bool binary_search)
    {
        file_path_ = { std::forward<S>(s) };

        if (!formats.size()) return ifs_.open(file_path_, mode);

        SetExtension();

        if (binary_search)
        {
            typename Container::const_iterator pos =
                BinarySearch(formats.begin(), formats.end(), file_ext_);

            if (pos != formats.end())
            {
                SetStem();
                return ifs_.open(file_path_, mode);
            }
        }
        else
        {
            typename Container::const_iterator pos =
                LinearSearch(formats.begin(), formats.end(), file_ext_);

            if (pos != formats.end())
            {
                SetStem();
                return ifs_.open(file_path_, mode);
            }
        }
    }

    typedef basic_ifstream<char> ifstream;
    typedef basic_ifstream<wchar_t> wifstream;
}