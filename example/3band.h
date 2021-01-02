#define PLUGIN_ID 1
#define PLUGIN_LABEL "crossover"
#define PLUGIN_NAME "FIR Crossover"
#define PLUGIN_MAKER "Danil Kichay"
#define PLUGIN_COPYRIGHT "None"

typedef struct {
  unsigned long AdditionalDelay;
  unsigned long FIRCoefficientsCount;
  float * FIRCoefficients;
  char * PortName;
} Band;

typedef struct {
  char * InputPortName;
  unsigned long BandsCount;
  Band ** BandsPointer;
} Crossover;

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

Band LowpassBand = {0, 10, LowpassFIRCoefficients, "Lowpass"};
Band BandpassBand = {0, 5, BandpassFIRCoefficients, "Bandpass"};
Band HighpassBand = {0, 1, HighpassFIRCoefficients, "Highpass"};
Band * Bands[] = {&LowpassBand, &BandpassBand, &HighpassBand};
Crossover CrossoverData = {"Input", 3, Bands};
