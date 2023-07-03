#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void zFunctionWithPattern(size_t* zFunction, size_t n, const char* pattern, size_t& left, size_t& right) {
  zFunction[0] = 0;
  for (size_t i = 1; i < n; i++) {
    if (left <= i && i <= right) {
      zFunction[i] = std::min(right - i + 1, static_cast<size_t>(zFunction[i - left]));
    }
    while (i + zFunction[i] < n && pattern[i + zFunction[i]] == pattern[zFunction[i]]) {
      zFunction[i]++;
    }
    if (i + zFunction[i] - 1 > right) {
      right = i + zFunction[i] - 1;
      left = i;
    }
  }
  if (n - 1 > right) {
    right = n - 1;
    left = n;
  }
}

void exit(FILE* file, size_t* zFunction, char* text) {
  fclose(file);
  free(text);
  free(zFunction);
}

int main(int argc, char* argv[]) {
  fprintf(stderr, "First parameter in args is the name of the file, where your text situated\n");
  fprintf(stderr, "The second one is the pattern, which ypu want to find in this text\n");
  if (argc != 3) {
    fprintf(stderr, "Incorrect number of arguments");
    return EXIT_FAILURE;
  }
  FILE* file = fopen(argv[1], "rb");
  if (!file) {
    perror("Something went wrong with your file");
    return EXIT_FAILURE;
  }

  const size_t patternSize = strlen(argv[2]);
  size_t* zFunction = static_cast<size_t*>(malloc(patternSize * sizeof(size_t)));
  if (zFunction == nullptr) {
    fclose(file);
    fprintf(stderr, "Something went wrong while allocating memory for z-function");
    return EXIT_FAILURE;
  }
  std::memset(zFunction, 0, sizeof(size_t) * patternSize);
  size_t left = 0, right = 0;
  zFunctionWithPattern(zFunction, patternSize, argv[2], left, right);

  char* text = static_cast<char*>(malloc(patternSize * sizeof(char)));
  if (text == nullptr) {
    fclose(file);
    free(zFunction);
    fprintf(stderr, "Something went wrong while allocating memory for text");
    return EXIT_FAILURE;
  }
  size_t numberOfElementsWhereRead = fread(text, sizeof(char), patternSize - 1, file);
  if (numberOfElementsWhereRead != patternSize - 1) {
    exit(file, zFunction, text);
    fprintf(stderr, "Something went wrong");
    return EXIT_FAILURE;
  }
  char sym = ' ';
  int numberOfAdded = 0;
  for (size_t i = patternSize + 1;; i++) {
    if (numberOfAdded == patternSize) {
      break;
    }
    if (sym != EOF) {
      sym = fgetc(file);
    } else {
      numberOfAdded++;
    }
    text[(i - 2 + patternSize) % patternSize] = sym;
    zFunction[i % patternSize] = 0;
    if (left <= i && i <= right) {
      zFunction[i % patternSize] = std::min(right - i + 1, static_cast<size_t>(zFunction[(i - left) % patternSize]));
      if (zFunction[i % patternSize] >= patternSize) {
        exit(file, zFunction, text);
        fprintf(stdout, "Yes");
        return EXIT_SUCCESS;
      }
    }
    while (text[(i - 1 + patternSize + zFunction[i % patternSize]) % patternSize] ==
           argv[2][zFunction[i % patternSize] % patternSize]) {
      zFunction[i % patternSize]++;
      if (zFunction[i % patternSize] >= patternSize) {
        exit(file, zFunction, text);
        fprintf(stdout, "Yes");
        return EXIT_SUCCESS;
      }
    }
    if (i + zFunction[i % patternSize] - 1 > right) {
      right = i + zFunction[i % patternSize] - 1;
      left = i;
    }
  }
  exit(file, zFunction, text);
  if (errno != 0) {
    perror("Something went wrong");
    return EXIT_FAILURE;
  }
  fprintf(stdout, "No");
  return EXIT_SUCCESS;
}