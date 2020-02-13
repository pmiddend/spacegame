#pragma once

#include <cmath>
#include <ostream>

namespace sg {
template <typename T>
class Vector {
public:
  Vector(T const &_x, T const &_y) : _x(_x), _y(_y) {}

  T const &x() const { return _x; }

  T const &y() const { return _y; }

  Vector<T> operator-() const { return Vector(-_x, -_y); }

private:
  T _x;
  T _y;
};

template <typename T, typename F>
auto vmap(Vector<T> const &r, F const &f) -> Vector<decltype(f(r.x()))> {
  return Vector{f(r.x()), f(r.y())};
}

template <typename U, typename T>
Vector<U> structure_cast(Vector<T> const &r) {
  return vmap(r, [](T const &t) { return static_cast<U>(t); });
}

template <typename T>
T dot(Vector<T> const &r, Vector<T> const &s) {
  return r.x() * s.x() + r.y() * s.y();
}

template <typename T>
T length(Vector<T> const &r) {
  return std::sqrt(dot(r, r));
}

template <typename T>
Vector<T> normalize(Vector<T> const &r) {
  auto const l{length(r)};
  if (l < static_cast<T>(0.000001))
    return r;
  return r / l;
}

template <typename T>
Vector<T> operator+(Vector<T> const &a, Vector<T> const &b) {
  return Vector(a.x() + b.x(), a.y() + b.y());
}

template <typename T>
Vector<T> operator*(Vector<T> const &a, Vector<T> const &b) {
  return Vector(a.x() * b.x(), a.y() * b.y());
}

template <typename T>
Vector<T> operator*(T const &a, Vector<T> const &b) {
  return Vector(a * b.x(), a * b.y());
}

template <typename T>
Vector<T> operator+=(Vector<T> const &a, Vector<T> const &b) {
  return a + b;
}

template <typename T>
Vector<T> operator-(Vector<T> const &a, Vector<T> const &b) {
  return a + (-b);
}

template <typename T>
Vector<T> operator/(Vector<T> const &a, T const &b) {
  return Vector(a.x() / b, a.y() / b);
}

template <typename T>
std::ostream &operator<<(std::ostream &os, Vector<T> const &v) {
  return os << v.x() << "x" << v.y();
}

template <typename T>
class Rectangle {
public:
  Rectangle(T const &_left, T const &_right, T const &_top, T const &_bottom)
      : _left(_left), _right(_right), _top(_top), _bottom(_bottom) {}

  static Rectangle<T>
  from_edges(Vector<T> const &left_top, Vector<T> const &right_bottom) {
    return Rectangle{
        left_top.x(), right_bottom.x(), left_top.y(), right_bottom.y()};
  }

  static Rectangle<T>
  from_pos_and_size(Vector<T> const &pos, Vector<T> const &size) {
    return from_edges(pos, pos + size);
  }

  T const &left() const { return _left; }
  T const &right() const { return _right; }
  T const &top() const { return _top; }
  T const &bottom() const { return _bottom; }

  T h() const { return _bottom - _top; }
  T w() const { return _right - _left; }

private:
  T _left;
  T _right;
  T _top;
  T _bottom;
};

template <typename T>
bool rect_intersect(Rectangle<T> const &outer, Rectangle<T> const &inner) {
  auto const X1 = outer.left();
  auto const X2 = inner.left();
  auto const Y1 = outer.top();
  auto const Y2 = inner.top();
  auto const W1 = outer.w();
  auto const W2 = inner.w();
  auto const H1 = outer.h();
  auto const H2 = inner.h();
  return !(X1 + W1 < X2 || X2 + W2 < X1 || Y1 + H1 < Y2 || Y2 + H2 < Y1);
}

template <typename T, typename F>
auto rect_map(Rectangle<T> const &r, F const &f)
    -> Rectangle<decltype(f(r.left()))> {
  return Rectangle(f(r.left()), f(r.right()), f(r.top()), f(r.bottom()));
}

template <typename U, typename T>
Rectangle<U> structure_cast(Rectangle<T> const &r) {
  return rect_map(r, [](T const &t) { return static_cast<U>(t); });
}
} // namespace sg
