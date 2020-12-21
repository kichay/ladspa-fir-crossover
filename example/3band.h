#define PLUGIN_ID 1
#define PLUGIN_LABEL "crossover"
#define PLUGIN_NAME "FIR Crossover"
#define PLUGIN_MAKER "Danil Kichay"
#define PLUGIN_COPYRIGHT "None"
#define PLUGIN_INPUT "Input"

typedef struct {
  const unsigned long BandAdditionalDelay;
  const unsigned long BandFIRCoefficientsLength;
  const float * BandFIRCoefficients;
  const char * PortName;
} CrossoverBand;

const float Lowpass[] = {
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

const float Bandpass[] = {
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

const float Highpass[] = {
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

const CrossoverBand Crossover[] = {
  {0, 10, Lowpass, "Lowpass"},
  {0, 10, Bandpass, "Bandpass"},
  {0, 10, Highpass, "Highpass"},
};
