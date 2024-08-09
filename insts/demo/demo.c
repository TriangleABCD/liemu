int main() {
  int a = 1, b = 1;
  while (b < 20) {
    int c = a + b;
    b = a;
    a = c;
  }
  return b;
}
