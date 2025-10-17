#pragma once
#include "io_traits.h"
#include <iostream>

template <typename T>
class BasicProgressBar
{
private:
    size_t n_steps_ = 1;
    std::basic_ostream<T>& os_;
    size_t width_ = 40;

    std::basic_string<T> bar_;
    size_t n_steps_taken_ = 0;

private:
    size_t Progress() const
    {
        return n_steps_ ? width_ * n_steps_taken_ / n_steps_ : width_;
    }

    void Update()
    {
        os_ << '\r';
        
        size_t progress = Progress();
        for (size_t i = 0; i < progress; ++i) bar_[i] = '#';
        for (size_t i = progress; i < bar_.size(); ++i) bar_[i] = ' ';
        
        os_ << '[' << bar_ << "] " << Percentage() << '%';
        os_.flush();
    }

public:
    BasicProgressBar(size_t n_steps, 
        std::basic_ostream<T>& os = traits<T>::tcout, 
        unsigned int width = 40) : 
        os_(os), 
        n_steps_(n_steps), 
        width_(width), 
        bar_(width_, ' ')
    {
        Update();
    }

    ~BasicProgressBar() noexcept
    {
        os_.flush();
        os_ << '\n';
    }

    size_t Percentage() const
    {
        return n_steps_ ? 100 * n_steps_taken_ / n_steps_ : 100;
    }

    void operator+=(size_t n_extra_steps)
    {
        if (n_steps_taken_ == n_steps_) return;

        size_t curr_progress = Progress();
        n_steps_taken_ += n_extra_steps;

        if (curr_progress != Progress()) Update();
    }

    void operator++()
    {
        operator+=(1);
    }

    void operator++(int)
    {
        operator+=(1);
    }
};

typedef BasicProgressBar<char> ProgressBar;
typedef BasicProgressBar<wchar_t> WProgressBar;