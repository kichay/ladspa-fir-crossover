#define PLUGIN_ID 1
#define PLUGIN_LABEL "crossover"
#define PLUGIN_NAME "FIR Crossover"
#define PLUGIN_MAKER "Danil Kichay"
#define PLUGIN_COPYRIGHT "None"

char * CrossoverInputPortName = "Input";

typedef struct {
  unsigned long BandAdditionalDelay;
  unsigned long BandFIRCoefficientsLength;
  float * BandFIRCoefficients;
  char * PortName;
} CrossoverBand;

float Lowpass[] = {
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

float Bandpass[] = {
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

float Highpass[] = {
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
  {0, 10, Lowpass, "Lowpass"},
  {0, 10, Bandpass, "Bandpass"},
  {0, 10, Highpass, "Highpass"},
};
