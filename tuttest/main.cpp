#include "input_options.hpp"

int main(int argc, const char *argv[]) {
  InputOptions options;
  const bool dolog = true;
  if (!options.Parse("testing command line options", argc, argv, dolog)) {
    return 1;
  }
  printf("modelPath=\"%s\" skyboxPath=\"%s\"\n", options.modelPath.c_str(),
         options.skyboxPath.c_str());

  return 0;
}
