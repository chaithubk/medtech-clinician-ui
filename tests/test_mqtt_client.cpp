#include "../src/models/vital_reading.h"
#include "../src/ui/vital_renderer.h"

#include <cstdlib>
#include <iostream>

int main() {
  VitalReading reading;
  reading.hr = 92.0f;
  reading.o2_sat = 98.0f;
  if (!reading.isValid()) {
    std::cerr << "Expected default sample reading to be valid\n";
    return EXIT_FAILURE;
  }

  if (VitalRenderer::formatHR(92.0f) != "92 bpm") {
    std::cerr << "Unexpected HR formatting\n";
    return EXIT_FAILURE;
  }

  std::cout << "Skeleton tests passed\n";
  return EXIT_SUCCESS;
}
