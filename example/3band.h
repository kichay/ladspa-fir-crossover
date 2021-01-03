#define PLUGIN_ID 1
#define PLUGIN_LABEL "crossover"
#define PLUGIN_NAME "FIR Crossover"
#define PLUGIN_MAKER "Danil Kichay"
#define PLUGIN_COPYRIGHT "None"
#define PLUGIN_INPUT_PORT_NAME "Input"
#define PLUGIN_PORT_COUNT 4 // (Input, Lowpass, Bandpass, Highpass)

typedef struct {
  unsigned long AdditionalDelay;
  unsigned long CoefficientsCount;
  float * Coefficients;
  char * OutputPortName;
} Band;

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

Band Crossover[] = {
  {0, 10, Lowpass, "Lowpass"},
  {3, 5, Bandpass, "Bandpass"},
  {5, 1, Highpass, "Highpass"},
};
