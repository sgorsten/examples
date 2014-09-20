#pragma once

#include <tuple>

template<class T, int N> struct vec;

template<class T> struct vec<T,2>
{ 
    T x,y;
    
    std::tuple<T,T> tuple() const { return std::make_tuple(x,y); }
    template<class F> vec zip(T r, F f) const { return {f(x,r), f(y,r)}; }
    template<class F> vec zip(const vec & r, F f) const { return {f(x,r.x), f(y,r.y)}; }
};

template<class T> struct vec<T,3>
{ 
    T x,y,z;

    std::tuple<T,T,T> tuple() const { return std::make_tuple(x,y,z); }
    template<class F> vec zip(T r, F f) const { return {f(x,r), f(y,r), f(z,r)}; }
    template<class F> vec zip(const vec & r, F f) const { return {f(x,r.x), f(y,r.y), f(z,r.z)}; }
};

template<class T> struct vec<T,4>
{
    T x,y,z,w;

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

typedef vec<int,2> int2; typedef vec<float,2> float2;
typedef vec<int,3> int3; typedef vec<float,3> float3;
typedef vec<int,4> int4; typedef vec<float,4> float4;
