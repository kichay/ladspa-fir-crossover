#define PLUGIN_ID 1
#define PLUGIN_LABEL "crossover"
#define PLUGIN_NAME "FIR Crossover"
#define PLUGIN_MAKER "Danil Kichay"
#define PLUGIN_COPYRIGHT "None"

float LowpassFIRCoefficients[] = {
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

float BandpassFIRCoefficients[] = {
  0.2,
  0.2,
  0.2,
  0.2,
  0.2,
};

float HighpassFIRCoefficients[] = {
  1,
};

Band Lowpass = {0, 10, LowpassFIRCoefficients, "Lowpass"};
Band Bandpass = {0, 5, BandpassFIRCoefficients, "Bandpass"};
Band Highpass = {0, 1, HighpassFIRCoefficients, "Highpass"};

Band * Bands[] = {&Lowpass, &Bandpass, &Highpass};

Crossover CrossoverData = {"Input", 3, Bands};
