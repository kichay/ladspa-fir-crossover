typedef struct {
  unsigned long AdditionalDelay;
  unsigned long FIRCoefficientsCount;
  float * FIRCoefficients;
  char * PortName;
} Band;

typedef struct {
  char * InputPortName;
  unsigned long BandsCount;
  Band ** Bands;
} Crossover;
