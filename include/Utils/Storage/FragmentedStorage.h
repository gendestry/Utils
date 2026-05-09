//
// Created by Bobi on 9/21/25.
//

#pragma once
#include <sstream>
#include <cstdint>
#include <array>
#include <format>
#include <list>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

// #include "Printable.h"
// #include "Utils.h"
// #include "Colors.h"

namespace Utils
{

struct Fragment
{
    uint32_t start = 0U;
    uint32_t size = 0U;
    uint16_t id = 0U;

    Fragment() = default;
    Fragment(const Fragment& other) {
        size = other.size;
        start = other.start;
        id = other.id;
    }
    virtual ~Fragment() = default;
    explicit Fragment(uint32_t size) : size(size) {}
    explicit Fragment(uint32_t start, uint32_t size) : start(start), size(size) {}

    virtual void setStart(uint32_t st) { start = st; };
    // virtual void setBuffer(uint8_t* buf) {buffer = buf;}

    [[nodiscard]] std::string describe() const
    {
        std::stringstream ss;
        ss << "FragID: " << id << " [" << size << " bytes]";
        return ss.str();
        // return std::format(R"({})", name);
    }

};

template <typename T, size_t TSize>
requires std::is_base_of_v<Fragment, T>
class FragmentedStorage
{
protected:
    std::list<std::shared_ptr<T>> m_fragments;
    std::array<uint8_t, TSize> m_buffer = { 0U };
    std::array<uint16_t, TSize> m_bytesPatched = { 0U };
    uint16_t m_fragmentsNum = 1U;

    void fillBytesPatched(uint32_t start, uint32_t end)
    {
        std::fill(m_bytesPatched.begin() + start, m_bytesPatched.begin() + end, m_fragmentsNum);
        m_fragmentsNum++;
    }

    bool checkMultiple(uint32_t start, uint32_t n, uint32_t size)
    {
        if (start + n * size > TSize)
        {
            return false;
        }

        for (int i =0; i < n * size; i++)
        {
            if (!isFree(i))
            {
                return false;
            }
        }

        return true;

    }

    uint32_t numFragmentsBefore(uint32_t start) {
        uint32_t counter = 0U;
        uint32_t prev = 0U;

        for (uint32_t i = 0; i < start; i++)
        {
            auto v = m_bytesPatched[i];
            if (v != 0 && prev != v) {
                counter++;
                prev = v;
            }
        }

        return counter;
    }

    std::optional<uint32_t> findFirstEmpty(uint32_t size) {
        for (int i = 0; i < TSize - size; i++) {
            bool isEmpty = isFree(i);
            if (isEmpty) {
                uint32_t counter = 0;
                for (; counter < size; counter++) {
                    isEmpty = isFree(counter + i);
                    if (!isEmpty) {
                        break;
                    }
                }

                if (counter == size) {
                    return i;
                }
            }
        }

        return std::nullopt;
    };

    bool isFree(uint32_t index) {
        return m_bytesPatched[index] == 0;
    }
public:

    void add(T newFragment, uint32_t start)
    {
        bool isFilled = !isFree(start);
        for (uint32_t i = start + 1; (i < start + newFragment.size) && !isFilled; i++)
        {
            isFilled = !isFree(i);
        }

        if (isFilled)
        {
            throw std::runtime_error("Fragment overlaps other segment");
        }

        std::shared_ptr<T> frag = std::make_shared<T>(newFragment);
        frag->setStart(start);
        frag->id = m_fragmentsNum;
        fillBytesPatched(start, frag->start + frag->size);

        auto n = numFragmentsBefore(start);
        auto it = std::next(m_fragments.begin(), n);
        m_fragments.insert(it, std::move(frag));
    }

    void add(T newFragment)
    {
        auto v = findFirstEmpty(newFragment.size);
        if (!v.has_value())
        {
            throw std::runtime_error("Cant find valid position");
        }

        auto start = v.value();
        std::shared_ptr<T> frag = std::make_shared<T>(newFragment);
        frag->setStart(start);
        frag->id = m_fragmentsNum;
        fillBytesPatched(start, frag->start + frag->size);

        auto n = numFragmentsBefore(start);
        auto it = std::next(m_fragments.begin(), n);
        m_fragments.insert(it, std::move(frag));
    }

    void addMultiple(T newFragment, int n, int start)
    {
        // auto v = findFirstEmpty(newFragment.size);
        // if (!v.has_value())
        // {
        //     throw std::runtime_error("Cant find valid position");
        // }

        if (!checkMultiple(start, n, newFragment.size)) {
            return;
        }

        uint32_t curr = start;
        for (int i = 0; i < n; i++) {
            std::shared_ptr<T> frag = std::make_shared<T>(newFragment);
            frag->setStart(curr);
            frag->id = m_fragmentsNum;
            fillBytesPatched(curr, frag->start + frag->size);

            auto n = numFragmentsBefore(curr);
            auto it = std::next(m_fragments.begin(), n);
            curr+= frag->size;

            m_fragments.insert(it, std::move(frag));
        }

    }

    void append(T newFragment)
    {
        const uint32_t offset = m_fragments.empty() ? 0 : m_fragments.back()->start + m_fragments.back()->size;
        if (offset > TSize)
        {
            throw std::out_of_range("Out of range");
        }

        std::shared_ptr<T> frag = std::make_shared<T>(newFragment);
        frag->setStart(offset);
        frag->id = m_fragmentsNum;
        fillBytesPatched(offset, offset + frag->size);
        m_fragments.emplace_back(std::move(frag));
    }

    void defragment()
    {
        m_bytesPatched.fill(0);
        uint32_t curr = 0U;
        for (const auto& fragment : m_fragments) {
            uint32_t& start = fragment->start;
            const uint32_t& size = fragment->size;
            start = curr;
            curr += size;

            for (uint16_t i = start; i < start + size; i++) {
                m_bytesPatched[i] = fragment->id;
            }
        }
    }

    [[nodiscard]] size_t getNumFragments() const
    {
        return m_fragments.size();
    }

    [[nodiscard]] uint8_t* getBytes()
    {
        return m_buffer.data();
    }

    [[nodiscard]] std::string fragmentsToString() const
    {
        std::stringstream ss;
        // std::string col = nextColor(0);
        uint32_t curr = 0U;
        for (const auto& fragment : m_fragments)
        {
            const uint32_t& start = fragment->start;
            // col = nextColor(start);
            if (curr != start)
            {
                // ss << std::format("{}[{:3}, {:3}]{} Unpatched{}\n", Colors::colorDim, 1, start, Colors::colorItalic, Colors::colorReset);
                ss << std::format("[{:3}, {:3}] Free space [{} bytes]\n",  curr, start - 1, (start - curr));
            }
            // ss << std::format("{}[{:3}, {:3}]{} \"{}\" FID: {} ({} bytes){}\n", col, start + 1, start + fragment->size, Colors::colorItalic, fragment->name, fragment->id, fragment->size, Colors::colorReset);
            ss << std::format("[{:3}, {:3}] {}\n", start, start + fragment->size - 1, fragment->describe());
            curr = start + fragment->size;
        }

        if (curr != TSize - 1)
        {
            ss << std::format("[{:3}, {:3}] Free space [{} bytes]\n", curr, TSize - 1, (TSize - curr));
        }
        return ss.str();
    }

    /*[[nodiscard]] std::string fragmentsToString() const
    {
        std::stringstream ss;
        // std::string col = nextColor(0);
        uint32_t curr = 0U;
        for (const auto& fragment : m_fragments)
        {
            const uint32_t& start = fragment->start;
            // col = nextColor(start);
            if (curr != start)
            {
                // ss << std::format("{}[{:3}, {:3}]{} Unpatched{}\n", Colors::colorDim, 1, start, Colors::colorItalic, Colors::colorReset);
                ss << std::format("{}[{:3}, {:3}] Free space [{} bytes]{}\n", Colors::colorDim,  curr, start - 1, (start - curr), Colors::colorReset);
            }
            // ss << std::format("{}[{:3}, {:3}]{} \"{}\" FID: {} ({} bytes){}\n", col, start + 1, start + fragment->size, Colors::colorItalic, fragment->name, fragment->id, fragment->size, Colors::colorReset);
            ss << std::format("[{:3}, {:3}] {}\n", start, start + fragment->size - 1, fragment->describe());
            curr = start + fragment->size;
        }

        if (curr != TSize - 1)
        {
            ss << std::format("{}[{:3}, {:3}] Free space [{} bytes]{}\n", Colors::colorDim, curr, TSize - 1, (TSize - curr), Colors::colorReset);
        }
        return ss.str();
    }*/



    // [[nodiscard]] std::string bytesToString() const
    // {
    //     std::stringstream ss;
    //     std::vector<float> colorVecHsv = {110.0f, 0.5f, 0.8f};
    //     const auto& bytes = m_buffer;
    //
    //     std::string col = nextColor(0);
    //
    //     const int32_t cond = std::min(static_cast<int32_t>(TSize), 16);
    //     auto printSeperator = [&]()
    //     {
    //         for (int i = 0; i < cond; i++)
    //         {
    //             ss << "----";
    //             if (i < cond - 1)
    //             {
    //                 ss << "-";
    //             }
    //         }
    //         ss << std::endl;
    //     };
    //
    //     ss << Colors::colorItalic;
    //     for (int i = 0; i < cond; i++)
    //     {
    //         static const std::string hex = "0123456789ABCDEF";
    //         ss << "0x" << hex[i] << "  ";
    //     }
    //     ss << Colors::colorReset << std::endl;
    //
    //     printSeperator();
    //
    //     for (int i = 0; i < bytes.size(); i++)
    //     {
    //         if (i % 16 == 0 && i != 0)
    //         {
    //             ss << std::endl;
    //         }
    //
    //         if (m_bytesPatched[i] != 0)
    //         {
    //             col = nextColor(i);
    //         }
    //         else
    //         {
    //             col = Colors::colorReset + Colors::colorDim + Colors::colorReset;
    //         }
    //
    //         ss << col << Utils::padByte(bytes[i], 3) << Colors::colorReset << "  ";
    //     }
    //     ss << std::endl;
    //
    //     printSeperator();
    //
    //     return ss.str();
    // }
};

}

