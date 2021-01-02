typedef struct {
  unsigned long AdditionalDelay;
  unsigned long CoefficientsCount;
  float * Coefficients;
  char * PortName;
} Band;

typedef struct {
  char * InputPortName;
  unsigned long BandsCount;
  Band * Bands;
} Crossover;
