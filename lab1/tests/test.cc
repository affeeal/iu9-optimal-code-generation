void Bar() {}

int Foo(const int x, const int x_doubled) {
  return 3 * x + x_doubled;
}

int main() {
  Bar();

  auto x = 0;
  auto y = Foo(x, x * 2);

  return y % 2;
}
