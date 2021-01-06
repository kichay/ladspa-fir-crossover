#define PLUGIN_UNIQUE_ID 1
#define PLUGIN_LABEL "crossover"
#define PLUGIN_NAME "FIR Crossover"
#define PLUGIN_MAKER "Danil Kichay"
#define PLUGIN_COPYRIGHT "None"
#define PLUGIN_PORT_COUNT 4

#define CROSSOVER_BAND_COUNT 3
#define CROSSOVER_BUFFER_SIZE 10

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
  0.2,
  0.2,
  0.2,
  0.2,
  0.2,
};

float Highpass[] = {
  1,
};

struct {
  unsigned long Delay;
  unsigned long CoefficientCount;
  float * CoefficientVector;
} Crossover[] = {
  {0, 10, Lowpass},
  {3, 5, Bandpass},
  {5, 1, Highpass},
};

const char * PortNames[] = {
  "Input",
  "Lowpass",
  "Bandpass",
  "Highpass",
};
