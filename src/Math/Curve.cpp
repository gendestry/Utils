#include "Math/Curve.h"
#include <cmath>
#include <iostream>

#include "Math/Maths.h"
#include <stdexcept>

namespace Utils::Maths {
    Curve::Curve(Type type, uint16_t length, uint16_t peaks)
    : m_type(type), m_length(length), m_peaks(peaks)
    {
        // update();
    //     init(type, length, peaks);
    }

    Curve::Curve(Type type, float min, float max, uint16_t length, uint16_t peaks)
    : m_type(type), m_min(min / 100.f), m_max(max / 100.f), m_length(length), m_peaks(peaks)
    {
        // update();
        //     init(type, length, peaks);
    }

    void Curve::init() {
        fill();
    }

    // void Curve::update()
    // {
    //     m_values.resize(m_length);
    //     auto temp = Maths::linearInterpolation(0, 1, m_length);
    //
    //     for (auto i = 0; i < m_length; i++)
    //     {
    //         double lin = temp[i];
    //         double phase = m_peaks * lin;
    //         switch (m_type)
    //         {
    //         case Type::SINUSOID:
    //         {
    //             constexpr double TWO_PI = M_PI;
    //             m_values[i] = std::fabs(std::sin(lin * m_peaks * TWO_PI));
    //             break;
    //         }
    //         case Type::TRIANGLE:
    //         {
    //             double t = std::fmod(phase * 2.0, 2.0);
    //             m_values[i] = (t <= 1.0) ? t : 2.0 - t;
    //             break;
    //         }
    //         case Type::SQUARE:
    //         {
    //             m_values[i] = std::fmod(phase, 1.0) < 0.5 ? 1.0 : 0.0;
    //             break;
    //         }
    //         case Type::LINE:
    //         {
    //             break;
    //         }
    //
    //         default:
    //         {
    //             throw std::invalid_argument("Invalid curve type");
    //         }
    //         }
    //     }
    // }

    float Curve::operator[] (std::size_t index) const
    {
        return m_values[index % m_length];
    }

    void Sinusoid::fill() {
        m_values.clear();
        m_values.resize(m_length);

        const auto lin = Maths::linearInterpolation(0, 1, m_length);
        for (auto i = 0; i < m_length; i++)
        {
            const double phase = m_peaks * lin[i];
            const double val = std::fabs(std::sin(phase * M_PI));
            m_values[i] = m_min + val * (m_max - m_min);
        }
    }

    void SinusoidHalf::fill() {
        m_values.clear();
        m_values.resize(m_length);

        const auto lin = Maths::linearInterpolation(0, 1, m_length);
        constexpr double d90 = M_PI / 2.0;
        for (auto i = 0; i < m_length; i++)
        {
            const double phase = m_peaks * lin[i];
            const double val = std::fabs(std::sin(phase * d90));
            m_values[i] = m_min + val * (m_max - m_min);
        }
    }

    void Triangle::fill() {
        m_values.clear();
        m_values.resize(m_length);

        const auto lin = Maths::linearInterpolationMirrored(m_min, m_max, m_length/m_peaks);
        const auto linlen = lin.size();

        for (int p = 0; p < m_peaks; p++)
        {
            for (auto i = 0; i < linlen; i++)
            {
                m_values[p * linlen + i] = lin[i];
            }
        }
    }

    void Square::fill() {
        m_values.clear();
        m_values.resize(m_length);
        const auto lin = Maths::linearInterpolation(0, 1, m_length);

        for (auto i = 0; i < m_length; i++)
        {
            double phase = m_peaks * lin[i];
            double m = std::fmod(phase, 1.f);
            m_values[i] = (m < 0.5) ? m_max : m_min;
        }
    }

    void Line::fill() {
        m_values.clear();
        m_values.resize(m_length);

        const auto lin = Maths::linearInterpolation(m_min, m_max, m_length/m_peaks);
        const auto linlen = lin.size();

        for (int p = 0; p < m_peaks; p++)
        {
            for (auto i = 0; i < linlen; i++)
            {
                m_values[p * linlen + i] = lin[i];
            }
        }
    }

    // CurveVariant getCurveByType(Type type) {
    //     switch (type) {
    //         case SINUSOID:
    //             return Sinusoid(1, 1);
    //         case TRIANGLE:
    //             return Triangle(1, 1);
    //         case SQUARE:
    //             return Square(1, 1);
    //     }
    // }

    std::unique_ptr<Curve> getCurveByType(Type type, uint16_t length) {
        switch (type) {
            case SINUSOID:
                return std::make_unique<Sinusoid>(length, 1);
            case TRIANGLE:
                return std::make_unique<Triangle>(length, 1);
            case SQUARE:
                return std::make_unique<Square>(length, 1);
            case LINE:
                return std::make_unique<Line>(length, 1);
            default:
                throw std::invalid_argument("Invalid curve type");
        }
    }
}

