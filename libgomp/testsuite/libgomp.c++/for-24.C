// { dg-do run }
// { dg-additional-options "-std=c++17" }

typedef __PTRDIFF_TYPE__ ptrdiff_t;
extern "C" void abort ();

namespace std {
  template<typename T> struct tuple_size;
  template<int, typename> struct tuple_element;
}

#pragma omp declare target
template <typename T>
class I
{
public:
  typedef ptrdiff_t difference_type;
  I ();
  ~I ();
  I (T *);
  I (const I &);
  T &operator * ();
  T *operator -> ();
  T &operator [] (const difference_type &) const;
  I &operator = (const I &);
  I &operator ++ ();
  I operator ++ (int);
  I &operator -- ();
  I operator -- (int);
  I &operator += (const difference_type &);
  I &operator -= (const difference_type &);
  I operator + (const difference_type &) const;
  I operator - (const difference_type &) const;
  template <typename S> friend bool operator == (I<S> &, I<S> &);
  template <typename S> friend bool operator == (const I<S> &, const I<S> &);
  template <typename S> friend bool operator < (I<S> &, I<S> &);
  template <typename S> friend bool operator < (const I<S> &, const I<S> &);
  template <typename S> friend bool operator <= (I<S> &, I<S> &);
  template <typename S> friend bool operator <= (const I<S> &, const I<S> &);
  template <typename S> friend bool operator > (I<S> &, I<S> &);
  template <typename S> friend bool operator > (const I<S> &, const I<S> &);
  template <typename S> friend bool operator >= (I<S> &, I<S> &);
  template <typename S> friend bool operator >= (const I<S> &, const I<S> &);
  template <typename S> friend typename I<S>::difference_type operator - (I<S> &, I<S> &);
  template <typename S> friend typename I<S>::difference_type operator - (const I<S> &, const I<S> &);
  template <typename S> friend I<S> operator + (typename I<S>::difference_type , const I<S> &);
private:
  T *p;
};
template <typename T> I<T>::I () : p (0) {}
template <typename T> I<T>::~I () {}
template <typename T> I<T>::I (T *x) : p (x) {}
template <typename T> I<T>::I (const I &x) : p (x.p) {}
template <typename T> T &I<T>::operator * () { return *p; }
template <typename T> T *I<T>::operator -> () { return p; }
template <typename T> T &I<T>::operator [] (const difference_type &x) const { return p[x]; }
template <typename T> I<T> &I<T>::operator = (const I &x) { p = x.p; return *this; }
template <typename T> I<T> &I<T>::operator ++ () { ++p; return *this; }
template <typename T> I<T> I<T>::operator ++ (int) { return I (p++); }
template <typename T> I<T> &I<T>::operator -- () { --p; return *this; }
template <typename T> I<T> I<T>::operator -- (int) { return I (p--); }
template <typename T> I<T> &I<T>::operator += (const difference_type &x) { p += x; return *this; }
template <typename T> I<T> &I<T>::operator -= (const difference_type &x) { p -= x; return *this; }
template <typename T> I<T> I<T>::operator + (const difference_type &x) const { return I (p + x); }
template <typename T> I<T> I<T>::operator - (const difference_type &x) const { return I (p - x); }
template <typename T> bool operator == (I<T> &x, I<T> &y) { return x.p == y.p; }
template <typename T> bool operator == (const I<T> &x, const I<T> &y) { return x.p == y.p; }
template <typename T> bool operator != (I<T> &x, I<T> &y) { return !(x == y); }
template <typename T> bool operator != (const I<T> &x, const I<T> &y) { return !(x == y); }
template <typename T> bool operator < (I<T> &x, I<T> &y) { return x.p < y.p; }
template <typename T> bool operator < (const I<T> &x, const I<T> &y) { return x.p < y.p; }
template <typename T> bool operator <= (I<T> &x, I<T> &y) { return x.p <= y.p; }
template <typename T> bool operator <= (const I<T> &x, const I<T> &y) { return x.p <= y.p; }
template <typename T> bool operator > (I<T> &x, I<T> &y) { return x.p > y.p; }
template <typename T> bool operator > (const I<T> &x, const I<T> &y) { return x.p > y.p; }
template <typename T> bool operator >= (I<T> &x, I<T> &y) { return x.p >= y.p; }
template <typename T> bool operator >= (const I<T> &x, const I<T> &y) { return x.p >= y.p; }
template <typename T> typename I<T>::difference_type operator - (I<T> &x, I<T> &y) { return x.p - y.p; }
template <typename T> typename I<T>::difference_type operator - (const I<T> &x, const I<T> &y) { return x.p - y.p; }
template <typename T> I<T> operator + (typename I<T>::difference_type x, const I<T> &y) { return I<T> (x + y.p); }

template <typename T>
class J
{
public:
  J(const I<T> &x, const I<T> &y) : b (x), e (y) {}
  const I<T> &begin ();
  const I<T> &end ();
private:
  I<T> b, e;
};

template <typename T> const I<T> &J<T>::begin () { return b; }
template <typename T> const I<T> &J<T>::end () { return e; }

struct K
{
  template <int N> int &get () { if (N == 0) return c; else if (N == 1) return b; return a; }
  int a, b, c;
};

template <> struct std::tuple_size<K> { static constexpr int value = 3; };
template <int N> struct std::tuple_element<N, K> { using type = int; };

struct L
{
  int a, b, c;
};

int a[2000];
long b[40];
short c[50];
int d[1024];
K e[1089];
L f[1093];
#pragma omp end declare target

int results[2000];

#pragma omp declare target
template <typename T>
void
baz (I<T> &i)
{
  if (*i < 0 || *i >= 2000)
    abort ();
  results[*i]++;
}

void
baz (int i)
{
  if (i < 0 || i >= 2000)
    abort ();
  results[i]++;
}

void
f1 ()
{
#pragma omp distribute parallel for default(none) shared(a)
  for (auto i : a)
    baz (i);
}

void
f2 ()
{
#pragma omp distribute parallel for default(none) shared(a)
  for (auto &i : a)
    if (&i != &a[i])
      abort ();
    else
      baz (i);
}

void
f3 ()
{
#pragma omp distribute parallel for collapse(3) default(none) shared(b, c)
  for (auto &i : b)
    for (int j = 9; j < 10; j++)
      for (auto k : c)
	if (&i != &b[i] || i < 0 || i >= 40 || j != 9 || k < 0 || k >= 50)
	  abort ();
	else
	  baz (i * 50 + k);
}

void
f4 (J<int> j)
{
#pragma omp distribute parallel for default(none) shared(j, a)
  for (auto &i : j)
    if (&i != &a[i])
      abort ();
    else
      baz (i);
}

void
f5 ()
{
#pragma omp distribute parallel for simd default(none) shared(d, results)
  for (auto i : d)
    results[i % 1024] += 2 * ((unsigned) i >> 10) + 1;
}

void
f6 (J<K> j)
{
#pragma omp distribute parallel for default(none) shared(j, e)
  for (auto & [k, l, m] : j)
    if (&k != &e[m].c || &l != &e[m].b || &m != &e[m].a || k != m * 3 || l != m * 2)
      abort ();
    else
      baz (m);
}

void
f7 (J<L> j)
{
#pragma omp distribute parallel for default(none) shared(j, f)
  for (auto & [k, l, m] : j)
    if (&k != &f[k].a || &l != &f[k].b || &m != &f[k].c || l != k * 4 || m != k * 5)
      abort ();
    else
      baz (k);
}

void
f8 (J<K> j)
{
#pragma omp distribute parallel for default(none) shared(j)
  for (auto [k, l, m] : j)
    if (k != m * 3 || l != m * 2)
      abort ();
    else
      baz (m);
}

void
f9 (J<L> j)
{
#pragma omp distribute parallel for default(none) shared(j)
  for (auto [k, l, m] : j)
    if (l != k * 4 || m != k * 5)
      abort ();
    else
      baz (k);
}

template <int N>
void
f10 ()
{
#pragma omp distribute parallel for default(none) shared(a)
  for (auto i : a)
    baz (i);
}

template <int N>
void
f11 ()
{
#pragma omp distribute parallel for default(none) shared(a)
  for (auto &i : a)
    if (&i != &a[i])
      abort ();
    else
      baz (i);
}

template <int N>
void
f12 ()
{
#pragma omp distribute parallel for collapse(3) default(none) shared(a, b, c)
  for (auto &i : b)
    for (I<int> j = I<int> (&a[9]); j < I<int> (&a[10]); j++)
      for (auto k : c)
	if (&i != &b[i] || i < 0 || i >= 40 || *j != 9 || k < 0 || k >= 50)
	  abort ();
	else
	  baz (i * 50 + k);
}

template <typename T>
void
f13 (J<T> j)
{
#pragma omp distribute parallel for default(none) shared(j, a)
  for (auto &i : j)
    if (&i != &a[i])
      abort ();
    else
      baz (i);
}

template <int N>
void
f14 ()
{
#pragma omp distribute parallel for simd default(none) shared(d, results)
  for (auto i : d)
    results[i % N] += 2 * ((unsigned) i >> 10) + 1;
}

template <typename T>
void
f15 (J<T> j)
{
#pragma omp distribute parallel for default(none) shared(j, e)
  for (auto & [k, l, m] : j)
    if (&k != &e[m].c || &l != &e[m].b || &m != &e[m].a || k != m * 3 || l != m * 2)
      abort ();
    else
      baz (m);
}

template <typename T>
void
f16 (J<T> j)
{
#pragma omp distribute parallel for default(none) shared(j, f)
  for (auto & [k, l, m] : j)
    if (&k != &f[k].a || &l != &f[k].b || &m != &f[k].c || l != k * 4 || m != k * 5)
      abort ();
    else
      baz (k);
}

template <int N>
void
f17 (J<K> j)
{
#pragma omp distribute parallel for default(none) shared(j)
  for (auto [k, l, m] : j)
    if (k != m * 3 || l != m * 2)
      abort ();
    else
      baz (m);
}

template <int N>
void
f18 (J<L> j)
{
#pragma omp distribute parallel for default(none) shared(j)
  for (auto [k, l, m] : j)
    if (l != k * 4 || m != k * 5)
      abort ();
    else
      baz (k);
}
#pragma omp end declare target

#define check(expr) \
  for (int i = 0; i < 2000; i++)			\
    if (expr)						\
      {							\
	if (results[i] != 1)				\
	  abort ();					\
	results[i] = 0;					\
      }							\
    else if (results[i])				\
      abort ()

int
main ()
{
  for (int i = 0; i < 2000; i++)
    a[i] = i;
  for (int i = 0; i < 40; i++)
    b[i] = i;
  for (int i = 0; i < 50; i++)
    c[i] = i;
  for (int i = 0; i < 1024; i++)
    d[i] = i;
  for (int i = 0; i < 1089; i++)
    {
      e[i].a = i;
      e[i].b = 2 * i;
      e[i].c = 3 * i;
    }
  for (int i = 0; i < 1093; i++)
    {
      f[i].a = i;
      f[i].b = 4 * i;
      f[i].c = 5 * i;
    }
  #pragma omp target update to (a, b, c, d, e, f)
  #pragma omp target teams map (tofrom: results)
  f1 ();
  check (1);
  #pragma omp target teams map (tofrom: results)
  f2 ();
  check (1);
  #pragma omp target teams map (tofrom: results)
  f3 ();
  check (1);
  #pragma omp target teams map (tofrom: results)
  f4 (J<int> (&a[14], &a[1803]));
  check (i >= 14 && i < 1803);
  #pragma omp target teams map (tofrom: results)
  f5 ();
  check (i >= 0 && i < 1024);
  #pragma omp target teams map (tofrom: results)
  f6 (J<K> (&e[19], &e[1029]));
  check (i >= 19 && i < 1029);
  #pragma omp target teams map (tofrom: results)
  f7 (J<L> (&f[15], &f[1091]));
  check (i >= 15 && i < 1091);
  #pragma omp target teams map (tofrom: results)
  f8 (J<K> (&e[27], &e[1037]));
  check (i >= 27 && i < 1037);
  #pragma omp target teams map (tofrom: results)
  f9 (J<L> (&f[1], &f[1012]));
  check (i >= 1 && i < 1012);
  #pragma omp target teams map (tofrom: results)
  f10 <0> ();
  check (1);
  #pragma omp target teams map (tofrom: results)
  f11 <1> ();
  check (1);
  #pragma omp target teams map (tofrom: results)
  f12 <2> ();
  check (1);
  #pragma omp target teams map (tofrom: results)
  f13 (J<int> (&a[24], &a[1703]));
  check (i >= 24 && i < 1703);
  #pragma omp target teams map (tofrom: results)
  f14 <1024> ();
  check (i >= 0 && i < 1024);
  #pragma omp target teams map (tofrom: results)
  f15 (J<K> (&e[39], &e[929]));
  check (i >= 39 && i < 929);
  #pragma omp target teams map (tofrom: results)
  f16 (J<L> (&f[17], &f[1071]));
  check (i >= 17 && i < 1071);
  #pragma omp target teams map (tofrom: results)
  f17 <3> (J<K> (&e[7], &e[1017]));
  check (i >= 7 && i < 1017);
  #pragma omp target teams map (tofrom: results)
  f18 <5> (J<L> (&f[121], &f[1010]));
  check (i >= 121 && i < 1010);
}
