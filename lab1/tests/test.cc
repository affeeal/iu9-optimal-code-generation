// Small input used to exercise representative GIMPLE and tree nodes.

struct Baz {
  double a;
  int b[3];
};

int Foo(const double i, const int j) {
  Baz x = {i, {j, 2, 3}};
  int* y = &x.b[1];
  return *y;
}

int Apply(int (*function)(int), int value) { return function(value); }

int Twice(int value) { return value * 2; }

__int128 WideInteger() { return (static_cast<__int128>(1) << 100) + 7; }

int main() {
  const char* text = "some text";

  int x = Foo(1.5, 1);
  if (!text) {
    x *= 3;
  }

  double y = 4 + 0.5 * x;
  return Apply(&Twice, static_cast<int>(y));
}
