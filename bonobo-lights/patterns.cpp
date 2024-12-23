#include "patterns.h"

Pattern test_6_32 = {
  6, 32, 3, false, (uint8_t *) &test_6_32_Data
};

Pattern cool = {
  240, 32, 3, false, (uint8_t *) &coolData
};

Pattern dudemabaike = {
  60, 16, 3, false, (uint8_t *) &dudemabaikeData
};

extern const uint8_t* all240patterns[N_PATTERNS] = { bonoboData, coolData, rainbowGradientData, trianglesData, dragonData, artichokeData, easterEggData};
