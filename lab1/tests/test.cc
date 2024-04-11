int x;

void Foo() {
}

int main() {
  x = 1;
  auto y = x < 2;
  auto z = !y;
  
  if (x > 3) {
    x += 4;
  }

  auto w = 5 * x + 6;
}
