#define PLUGIN_ID 1
#define PLUGIN_LABEL "crossover"
#define PLUGIN_NAME "FIR Crossover"
#define PLUGIN_MAKER "Danil Kichay"
#define PLUGIN_COPYRIGHT "None"

typedef struct {
  const unsigned long BandAdditionalDelay;
  const unsigned long BandFIRCoefficientsCount;
  const float * BandFIRCoefficients;
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

CrossoverBand Crossover[] = {
  {0, 10, Lowpass},
  {0, 10, Bandpass},
  {0, 10, Highpass},
};
