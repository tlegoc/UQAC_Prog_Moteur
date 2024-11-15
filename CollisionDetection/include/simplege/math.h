#pragma once

#include <simplege/pch/precomp.h>

namespace SimpleGE
{
  template<std::size_t W>
  struct Vector;

  template<std::size_t W>
  struct Point
  {
    static Point<W> Origin() { return {}; }

    Point() = default;

    Point(const std::array<float, W>& val) : point(val) {}

    template<std::size_t W2>
    explicit Point(const Point<W2>& other)
    {
      for (int i = 0; i < std::min(W, W2); i++)
      {
        operator[](i) = other[i];
      }
    }

    explicit operator const float*() const { return point.data(); }

    float& operator[](int idx) { return gsl::at(point, idx); }

    float operator[](int idx) const { return gsl::at(point, idx); }

    inline Point<W>& operator+=(const Vector<W>& rhs);

    friend Point<W> operator+(const Vector<W>& lhs, const Point<W>& rhs) { return rhs + lhs; }

    std::array<float, W> point{};
  };

  template<std::size_t W>
  struct Vector
  {
    Vector() = default;

    Vector(const std::array<float, W>& val) : vector(val) {}

    template<std::size_t W2>
    explicit Vector(const Vector<W2>& other)
    {
      for (int i = 0; i < std::min(W, W2); i++)
      {
        operator[](i) = other[i];
      }
    }

    explicit operator const float*() const { return vector.data(); }

    float& operator[](int idx) { return gsl::at(vector, idx); }

    float operator[](int idx) const { return gsl::at(vector, idx); }

    Vector<W>& operator+=(const Vector<W>& rhs)
    {
      for (int i = 0; i < W; i++)
      {
        operator[](i) += rhs[i];
      }
      return *this;
    }

    friend Vector<W> operator+(Vector<W> lhs, const Vector<W>& rhs)
    {
      lhs += rhs;
      return lhs;
    }

    Vector<W>& operator*=(float scale)
    {
      for (int i = 0; i < W; i++)
      {
        operator[](i) *= scale;
      }
      return *this;
    }

    friend Vector<W> operator*(Vector<W> lhs, float scale)
    {
      lhs *= scale;
      return lhs;
    }

    [[nodiscard]] float MagnitudeSq() const
    {
      float sum{};
      for (int i = 0; i < W; i++)
      {
        sum += operator[](i) * operator[](i);
      }
      return sum;
    }

    [[nodiscard]] float Magnitude() const { return std::sqrt(MagnitudeSq()); }

    void Normalize()
    {
      float magnitude = Magnitude();
      Ensures(magnitude != 0.F);

      for (int i = 0; i < W; i++)
      {
        operator[](i) /= magnitude;
      }
    }

    std::array<float, W> vector{};
  };

  template<std::size_t W>
  inline Point<W>& Point<W>::operator+=(const Vector<W>& rhs)
  {
    for (int i = 0; i < W; i++)
    {
      operator[](i) += rhs[i];
    }
    return *this;
  }

  template<std::size_t W>
  static inline Point<W> operator+(Point<W> lhs, const Vector<W>& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  template<std::size_t W>
  static inline Vector<W> operator-(const Point<W>& lhs, const Point<W>& rhs)
  {
    Vector<W> vec;
    for (int i = 0; i < W; i++)
    {
      vec[i] = lhs[i] - rhs[i];
    }
    return vec;
  }

  template<typename T>
  struct Size
  {
    T width;
    T height;
  };

  struct Area
  {
    Area() = default;

    Area(float x, float y, float width, float height) : position({x, y}), size({width, height}) {}

    [[nodiscard]] float x() const { return position[0]; }
    [[nodiscard]] float y() const { return position[1]; }
    [[nodiscard]] float width() const { return size.width; }
    [[nodiscard]] float height() const { return size.height; }

    [[nodiscard]] float xMin() const { return x() - width() / 2.F; }
    [[nodiscard]] float xMax() const { return x() + width() / 2.F; }
    [[nodiscard]] float yMin() const { return y() - height() / 2.F; }
    [[nodiscard]] float yMax() const { return y() + height() / 2.F; }

    [[nodiscard]] bool Intersects(const Area& other) const
    {
      return !((xMin() >= other.xMax()) || (xMax() <= other.xMin()) || (yMin() >= other.yMax()) ||
               (yMax() <= other.yMin()));
    }

    [[nodiscard]] bool Contains(const Area& other) const
    {
      return (xMin() <= other.xMin()) && (xMax() >= other.xMax()) && (yMin() <= other.yMin()) &&
             (yMax() >= other.yMax());
    }

    [[nodiscard]] Area Union(const Area& other) const
    {
      float minX = std::min(xMin(), other.xMin());
      float minY = std::min(yMin(), other.yMin());
      float maxX = std::max(xMax(), other.xMax());
      float maxY = std::max(yMax(), other.yMax());
      return {minX + (maxX - minX) / 2.F, minY + (maxY - minY) / 2.F, maxX - minX, maxY - minY};
    }

    /**
    * @returns An array of 4 areas, each representing a quadrant of the current area. bl, br, tl, tr
    */
    [[nodiscard]] auto Split()
    {
      return std::array<Area, 4>{
        Area{xMin() + width() / 4.F, yMin() + height() / 4.F, width() / 2.F, height() / 2.F},
        Area{xMin() + 3.F * width() / 4.F, yMin() + height() / 4.F, width() / 2.F, height() / 2.F},
        Area{xMin() + width() / 4.F, yMin() + 3.F * height() / 4.F, width() / 2.F, height() / 2.F},
        Area{xMin() + 3.F * width() / 4.F, yMin() + 3.F * height() / 4.F, width() / 2.F, height() / 2.F}
      };
    }

    Point<2> position{};
    Size<float> size{};
  };
} // namespace SimpleGE