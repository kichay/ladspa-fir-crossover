#include <stdlib.h>
#include <ladspa.h>

#define CROSSOVER_BAND_COUNT 3
#define CROSSOVER_BUFFER_SIZE 10

struct {
  unsigned long Delay;
  unsigned long CoefficientCount;
  float * CoefficientVector;
} Crossover[] = {
  {
    0, 10, (float[]){
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
    }
  },
  {
    3, 5, (float[]){
      0.2,
      0.2,
      0.2,
      0.2,
      0.2,
    }
  },
  {
    5, 1, (float[]){
      1.0,
    }
  },
};

typedef struct BufferElement {
  LADSPA_Data Value;
  struct BufferElement * Previous;
  struct BufferElement * Next;
  struct BufferElement * BandDelay[CROSSOVER_BAND_COUNT];
} BufferElement;

typedef struct {
  LADSPA_Data * m_pfInputBuffer;
  LADSPA_Data * m_pfOutputBuffers[CROSSOVER_BAND_COUNT];
  struct BufferElement * m_psEntrypoint;
  struct BufferElement m_psCircleBuffer[CROSSOVER_BUFFER_SIZE];
} CrossoverInstance;

LADSPA_Handle instantiate (
  const LADSPA_Descriptor * Descriptor,
  unsigned long SampleRate
) {
  CrossoverInstance * psCrossoverInstance;
  struct BufferElement * psBufferCurrent;
  struct BufferElement * psBufferBarrier;
  unsigned long BandIndex;

  psCrossoverInstance = (CrossoverInstance *)malloc(sizeof(CrossoverInstance));
  for (
    psBufferCurrent = psCrossoverInstance->m_psCircleBuffer,
      psBufferBarrier = psCrossoverInstance->m_psCircleBuffer + CROSSOVER_BUFFER_SIZE;
    psBufferCurrent < psBufferBarrier;
    psBufferCurrent++
  ) {
    for (
      BandIndex = 0;
      BandIndex < CROSSOVER_BAND_COUNT;
      BandIndex++
    ) {
      psBufferCurrent->BandDelay[BandIndex] = psBufferCurrent -
        Crossover[BandIndex].Delay;
      if (
        psBufferCurrent->BandDelay[BandIndex] <
          psCrossoverInstance->m_psCircleBuffer
      ) {
        psBufferCurrent->BandDelay[BandIndex] +=
          CROSSOVER_BUFFER_SIZE;
      }
    }
    if (psBufferCurrent == psCrossoverInstance->m_psCircleBuffer) {
      psBufferCurrent->Previous = psBufferBarrier - 1;
      psBufferCurrent->Next = psBufferCurrent + 1;
      continue;
    }
    if (psBufferCurrent == psBufferBarrier - 1) {
      psBufferCurrent->Next = psCrossoverInstance->m_psCircleBuffer;
      psBufferCurrent->Previous = psBufferCurrent - 1;
      continue;
    }
    psBufferCurrent->Next = psBufferCurrent + 1;
    psBufferCurrent->Previous = psBufferCurrent - 1;
  }
  psCrossoverInstance->m_psEntrypoint = psCrossoverInstance->m_psCircleBuffer;
  return (LADSPA_Handle)psCrossoverInstance;
}

void activate (
  LADSPA_Handle Instance
) {
  CrossoverInstance * psCrossoverInstance;
  struct BufferElement * psBufferCurrent;
  struct BufferElement * psBufferBarrier;

  psCrossoverInstance = (CrossoverInstance *)Instance;
  for (
    psBufferCurrent = psCrossoverInstance->m_psCircleBuffer,
      psBufferBarrier = psCrossoverInstance->m_psCircleBuffer + CROSSOVER_BUFFER_SIZE;
    psBufferCurrent < psBufferBarrier;
    psBufferCurrent++
  ) {
    psBufferCurrent->Value = 0;
  }
}

void connect_port (
  LADSPA_Handle Instance,
  unsigned long Port,
  LADSPA_Data * DataLocation
) {
  CrossoverInstance * psCrossoverInstance;

  psCrossoverInstance = (CrossoverInstance *)Instance;
  if (Port == 0) {
    psCrossoverInstance->m_pfInputBuffer = DataLocation;
  } else if (0 < Port && Port <= CROSSOVER_BAND_COUNT) {
    psCrossoverInstance->m_pfOutputBuffers[Port - 1] = DataLocation;
  }
}

void run (
  LADSPA_Handle Instance,
  unsigned long SampleCount
) {
  CrossoverInstance * psCrossoverInstance;
  unsigned long lSampleIndex;
  unsigned long lBandIndex;
  unsigned long lCoefficientIndex;
  LADSPA_Data Accumulator;
  struct BufferElement * psBufferCurrent;

  psCrossoverInstance = (CrossoverInstance *)Instance;
  for (
    lSampleIndex = 0;
    lSampleIndex < SampleCount;
    lSampleIndex++
  ) {
    psCrossoverInstance->m_psEntrypoint->Value =
      psCrossoverInstance->m_pfInputBuffer[lSampleIndex];
    for (
      lBandIndex = 0;
      lBandIndex < CROSSOVER_BAND_COUNT;
      lBandIndex++
    ) {
      Accumulator = 0;
      psBufferCurrent =
        psCrossoverInstance->m_psEntrypoint->BandDelay[lBandIndex];
      for (
        lCoefficientIndex = 0;
        lCoefficientIndex < Crossover[lBandIndex].CoefficientCount;
        lCoefficientIndex++
      ) {
        Accumulator += psBufferCurrent->Value *
          Crossover[lBandIndex].CoefficientVector[lCoefficientIndex];
        psBufferCurrent = psBufferCurrent->Previous;
      }
      psCrossoverInstance->m_pfOutputBuffers[lBandIndex][lSampleIndex] =
        Accumulator;
    }
    psCrossoverInstance->m_psEntrypoint = psCrossoverInstance->m_psEntrypoint->Next;
  }
}

void cleanup(
  LADSPA_Handle Instance
) {
  free(Instance);
}

LADSPA_PortDescriptor PortDescriptors[] = {
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
};

const char * PortNames[] = {
  "Input",
  "Lowpass",
  "Bandpass",
  "Highpass",  
};

LADSPA_PortRangeHint PortRangeHints[] = {
  {0},
  {0},
  {0},
  {0},
};

LADSPA_Descriptor CrossoverDescriptor = { 
  1,
  "crossover",
  LADSPA_PROPERTY_HARD_RT_CAPABLE,
  "FIR Crossover",
  "Danil Kichay",
  "None",
  4,
  PortDescriptors,
  PortNames,
  PortRangeHints,
  NULL,
  &instantiate,
  &connect_port,
  &activate,
  &run,
  NULL,
  NULL,
  NULL,
  &cleanup,
};

const LADSPA_Descriptor * ladspa_descriptor(unsigned long Index) {
  if (Index == 0)
    return &CrossoverDescriptor;
  return NULL;
}