int atoi(const char *str) {
  int i = 0;
  int res = 0;

  while (str[i] != '\0') {
    if (str[i] < '0' || str[i] > '9')
      return -1;
    res = res * 10 + (str[i] - '0');
    i++;
  }
  return res;
}
