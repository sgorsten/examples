#pragma once

#include <tuple>

template<class T, int N> struct vec;

template<class T> struct vec<T,2>
{ 
    T x,y;

    vec() : x(), y() {}
    vec(T x, T y) : x(x), y(y) {}
    template<class U> explicit vec(const vec<U,2> & r) : x(T(r.x)), y(T(r.y)) {}
    
    std::tuple<T,T> tuple() const { return std::make_tuple(x,y); }
    template<class F> vec zip(T r, F f) const { return {f(x,r), f(y,r)}; }
    template<class F> vec zip(const vec & r, F f) const { return {f(x,r.x), f(y,r.y)}; }
};

template<class T> struct vec<T,3>
{ 
    T x,y,z;

    vec() : x(), y(), z() {}
    vec(T x, T y, T z) : x(x), y(y), z(z) {}
    vec(vec<T,2> xy, T z) : x(xy.x), y(xy.y), z(z) {}
    template<class U> explicit vec(const vec<U,3> & r) : x(T(r.x)), y(T(r.y)), z(T(r.z)) {}

    std::tuple<T,T,T> tuple() const { return std::make_tuple(x,y,z); }
    template<class F> vec zip(T r, F f) const { return {f(x,r), f(y,r), f(z,r)}; }
    template<class F> vec zip(const vec & r, F f) const { return {f(x,r.x), f(y,r.y), f(z,r.z)}; }
};

template<class T> struct vec<T,4>
{
    T x,y,z,w;

    vec() : x(), y(), z(), w() {}
    vec(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    vec(vec<T,3> xyz, T w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
    template<class U> explicit vec(const vec<U,4> & r) : x(T(r.x)), y(T(r.y)), z(T(r.z)), w(T(r.w)) {}

    std::tuple<T,T,T,T> tuple() const { return std::make_tuple(x,y,z,w); }
    template<class F> vec zip(T r, F f) const { return {f(x,r), f(y,r), f(z,r)}; }
    template<class F> vec zip(const vec & r, F f) const { return {f(x,r.x), f(y,r.y), f(z,r.z), f(w,r.w)}; }
};

template<class T, int N> bool operator == (const vec<T,N> & a, const vec<T,N> & b) { return a.tuple() == b.tuple(); }
template<class T, int N> bool operator != (const vec<T,N> & a, const vec<T,N> & b) { return a.tuple() != b.tuple(); }
template<class T, int N> bool operator <  (const vec<T,N> & a, const vec<T,N> & b) { return a.tuple() <  b.tuple(); }
template<class T, int N> bool operator <= (const vec<T,N> & a, const vec<T,N> & b) { return a.tuple() <= b.tuple(); }
template<class T, int N> bool operator >  (const vec<T,N> & a, const vec<T,N> & b) { return a.tuple() >  b.tuple(); }
template<class T, int N> bool operator >= (const vec<T,N> & a, const vec<T,N> & b) { return a.tuple() >= b.tuple(); }

template<class T, int N> vec<T,N> operator - (const vec<T,N> & a) { return a.zip(T(), [](T a, T) { return -a; }); }

template<class T, int N> vec<T,N> operator + (const vec<T,N> & a, const vec<T,N> & b) { return a.zip(b, [](T a, T b) { return a+b; }); }
template<class T, int N> vec<T,N> operator - (const vec<T,N> & a, const vec<T,N> & b) { return a.zip(b, [](T a, T b) { return a-b; }); }
template<class T, int N> vec<T,N> operator * (const vec<T,N> & a, const vec<T,N> & b) { return a.zip(b, [](T a, T b) { return a*b; }); }
template<class T, int N> vec<T,N> operator / (const vec<T,N> & a, const vec<T,N> & b) { return a.zip(b, [](T a, T b) { return a/b; }); }
template<class T, int N> vec<T,N> operator % (const vec<T,N> & a, const vec<T,N> & b) { return a.zip(b, [](T a, T b) { return a%b; }); }

template<class T, int N> vec<T,N> operator + (const vec<T,N> & a, T b) { return a.zip(b, [](T a, T b) { return a+b; }); }
template<class T, int N> vec<T,N> operator - (const vec<T,N> & a, T b) { return a.zip(b, [](T a, T b) { return a-b; }); }
template<class T, int N> vec<T,N> operator * (const vec<T,N> & a, T b) { return a.zip(b, [](T a, T b) { return a*b; }); }
template<class T, int N> vec<T,N> operator / (const vec<T,N> & a, T b) { return a.zip(b, [](T a, T b) { return a/b; }); }
template<class T, int N> vec<T,N> operator % (const vec<T,N> & a, T b) { return a.zip(b, [](T a, T b) { return a%b; }); }

template<class T, int N, class U> vec<T,N> & operator += (vec<T,N> & a, const U & b) { return a = a+b; }
template<class T, int N, class U> vec<T,N> & operator -= (vec<T,N> & a, const U & b) { return a = a-b; }
template<class T, int N, class U> vec<T,N> & operator *= (vec<T,N> & a, const U & b) { return a = a*b; }
template<class T, int N, class U> vec<T,N> & operator /= (vec<T,N> & a, const U & b) { return a = a/b; }
template<class T, int N, class U> vec<T,N> & operator %= (vec<T,N> & a, const U & b) { return a = a%b; }

template<class T> T dot(const vec<T,2> & a, const vec<T,2> & b) { return a.x*b.x + a.y*b.y; }
template<class T> T dot(const vec<T,3> & a, const vec<T,3> & b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
template<class T> T dot(const vec<T,4> & a, const vec<T,4> & b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
template<class T, int N> T mag2(const vec<T,N> & a) { return dot(a,a); }
template<class T, int N> T mag(const vec<T,N> & a) { return std::sqrt(mag2(a)); }
template<class T, int N> vec<T,N> norm(const vec<T,N> & a) { return a/mag(a); }

typedef vec<int,2> int2; typedef vec<float,2> float2; typedef vec<double,2> double2;
typedef vec<int,3> int3; typedef vec<float,3> float3; typedef vec<double,3> double3;
typedef vec<int,4> int4; typedef vec<float,4> float4; typedef vec<double,4> double4;
