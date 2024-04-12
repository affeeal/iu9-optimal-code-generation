// Completely pointless programm to parse.

struct Baz {
  double a;
  int b[3];
};

int Foo(const double i, const int j) {
  Baz x = {i, {j, 2, 3}};
  int* y = &x.b[1];
  return *y;
}

int main() {
  const char* text = "some text";

  int x = Foo(1.5, 1);
  if (!text) {
    x *= 3; 
  }

  double y = 4 + 0.5 * x;
}
