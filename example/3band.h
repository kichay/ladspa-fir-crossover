#define PLUGIN_ID 1
#define PLUGIN_LABEL "crossover"
#define PLUGIN_NAME "FIR Crossover"
#define PLUGIN_MAKER "Danil Kichay"
#define PLUGIN_COPYRIGHT "None"

float LowpassCoefficients[] = {
  0.1,
  0.1,
  0.1,
  0.1,
  0.1,
  0.1,
  0.1,
  0.1,
  0.1,
  0.1,
};

float BandpassCoefficients[] = {
  0.2,
  0.2,
  0.2,
  0.2,
  0.2,
};

float HighpassCoefficients[] = {
  1,
};

Band Bands[] = {
  {0, 10, LowpassCoefficients, "Lowpass"},
  {0, 5, BandpassCoefficients, "Bandpass"},
  {0, 1, HighpassCoefficients, "Highpass"},
};

Crossover CrossoverData = {"Input", 3, Bands};
