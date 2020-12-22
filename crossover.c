#include <stdlib.h>
#include <string.h>
#include <ladspa.h>

#define STATIC_QUOTE(x) #x
#define QUOTE(x) STATIC_QUOTE(x)

#ifndef FIR_HEADER
#define FIR_HEADER example/3band.h
#endif

#include QUOTE(FIR_HEADER)

LADSPA_Descriptor * g_psCrossoverDescriptor = NULL;
unsigned int g_lOutputBandCount = 0;
unsigned long g_lHistoryBufferLength = 0;

typedef struct {
  LADSPA_Data * m_pfInputBuffer;
  LADSPA_Data ** m_p2pfOutputBuffer;
  LADSPA_Data * m_pfHistoryBuffer;
  LADSPA_Data * m_pfHistoryBufferCurrentElement;
  LADSPA_Data * m_pfHistoryBufferLidElement;
} CrossoverInstance;

LADSPA_Handle instantiate (
  const LADSPA_Descriptor * Descriptor,
  unsigned long SampleRate
) {
  CrossoverInstance * psCrossoverInstance;

  psCrossoverInstance = (CrossoverInstance *)malloc(sizeof(CrossoverInstance));
  psCrossoverInstance->m_pfHistoryBuffer = (LADSPA_Data *)malloc(
    sizeof(LADSPA_Data) * (g_lHistoryBufferLength)
  );
  psCrossoverInstance->m_pfHistoryBufferCurrentElement =
    psCrossoverInstance->m_pfHistoryBuffer;
  psCrossoverInstance->m_pfHistoryBufferLidElement =
    psCrossoverInstance->m_pfHistoryBuffer + g_lHistoryBufferLength;
  psCrossoverInstance->m_p2pfOutputBuffer = (LADSPA_Data **)malloc(
    sizeof(LADSPA_Data *) * g_lOutputBandCount
  );
  return (LADSPA_Handle)psCrossoverInstance;
}

void activate (
  LADSPA_Handle Instance
) {
  CrossoverInstance * psCrossoverInstance;
  LADSPA_Data * pfHistoryBufferResetElement;

  psCrossoverInstance = (CrossoverInstance *)Instance;
  for (
    pfHistoryBufferResetElement = psCrossoverInstance->m_pfHistoryBuffer;
    pfHistoryBufferResetElement < psCrossoverInstance->m_pfHistoryBufferLidElement;
    *(pfHistoryBufferResetElement++) = 0
  );
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
  } else if (0 < Port && Port <= g_lOutputBandCount) {
    *(psCrossoverInstance->m_p2pfOutputBuffer + Port - 1) = DataLocation;
  }
}

void run (
  LADSPA_Handle Instance,
  unsigned long SampleCount
) {
  LADSPA_Data * pfInput;
  LADSPA_Data * pfOutput;
  LADSPA_Data * pfHistoryBufferMultiplyElement;
  LADSPA_Data fOutputSummary;
  unsigned long lSampleIndex;
  unsigned long lBandIndex;
  unsigned long lFIRCoefficientIndex;
  CrossoverInstance * psCrossoverInstance;

  psCrossoverInstance = (CrossoverInstance *)Instance;
  for (
    lSampleIndex = 0;
    lSampleIndex < SampleCount;
    lSampleIndex++
  ) {
    pfInput = psCrossoverInstance->m_pfInputBuffer + lSampleIndex;
    *(psCrossoverInstance->m_pfHistoryBufferCurrentElement) = *(pfInput);
    if(
      ++psCrossoverInstance->m_pfHistoryBufferCurrentElement ==
        psCrossoverInstance->m_pfHistoryBufferLidElement
    ) {
      psCrossoverInstance->m_pfHistoryBufferCurrentElement =
        psCrossoverInstance->m_pfHistoryBuffer;
    }
    for (
      lBandIndex = 0;
      lBandIndex < g_lOutputBandCount;
      lBandIndex++
    ) {
      pfOutput = *(psCrossoverInstance->m_p2pfOutputBuffer + lBandIndex) + lSampleIndex;
      pfHistoryBufferMultiplyElement = psCrossoverInstance->m_pfHistoryBufferCurrentElement;
      fOutputSummary = 0;
      for (
          lFIRCoefficientIndex = Crossover[lBandIndex].BandFIRCoefficientsCount - 1;
          lFIRCoefficientIndex + 1 > 0;
          lFIRCoefficientIndex--
      ) {
          fOutputSummary += Crossover[lBandIndex].BandFIRCoefficients[lFIRCoefficientIndex] *
            *(pfHistoryBufferMultiplyElement);
          if (
            ++pfHistoryBufferMultiplyElement ==
              psCrossoverInstance->m_pfHistoryBufferLidElement
          ) {
            pfHistoryBufferMultiplyElement = psCrossoverInstance->m_pfHistoryBuffer;
          }
      }
      *(pfOutput) = fOutputSummary;
    }
  }
}

void cleanup(LADSPA_Handle Instance) {
  CrossoverInstance * psCrossoverInstance;

  psCrossoverInstance = (CrossoverInstance *)Instance;
  free((LADSPA_Data *)psCrossoverInstance->m_pfHistoryBuffer);
  free((LADSPA_Data **)psCrossoverInstance->m_p2pfOutputBuffer);
  free(Instance);
}

void _init() {
  unsigned long lPortIndex;
  char ** pcPortNames;
  LADSPA_PortDescriptor * piPortDescriptors;
  LADSPA_PortRangeHint * psPortRangeHints;

  g_lOutputBandCount = sizeof(Crossover) / sizeof(Crossover[0]);
  g_psCrossoverDescriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
  g_psCrossoverDescriptor->UniqueID = PLUGIN_ID;
  g_psCrossoverDescriptor->Label = strdup(PLUGIN_LABEL);
  g_psCrossoverDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
  g_psCrossoverDescriptor->Name  = strdup(PLUGIN_NAME);
  g_psCrossoverDescriptor->Maker = strdup(PLUGIN_MAKER);
  g_psCrossoverDescriptor->Copyright = strdup(PLUGIN_COPYRIGHT);
  g_psCrossoverDescriptor->PortCount = g_lOutputBandCount + 1;
  piPortDescriptors = (LADSPA_PortDescriptor *)calloc(
    g_psCrossoverDescriptor->PortCount,
    sizeof(LADSPA_PortDescriptor)
  );
  g_psCrossoverDescriptor->PortDescriptors = (const LADSPA_PortDescriptor *)piPortDescriptors;
  pcPortNames = (char **)calloc(g_psCrossoverDescriptor->PortCount, sizeof(char *));
  g_psCrossoverDescriptor->PortNames = (const char **)pcPortNames;
  psPortRangeHints = (LADSPA_PortRangeHint *)calloc(
    g_psCrossoverDescriptor->PortCount,
    sizeof(LADSPA_PortRangeHint)
  );
  g_psCrossoverDescriptor->PortRangeHints = (const LADSPA_PortRangeHint *)psPortRangeHints;
  for(
    lPortIndex = 0;
    lPortIndex < g_psCrossoverDescriptor->PortCount;
    lPortIndex++
  ) {
    if (lPortIndex == 0) {
      piPortDescriptors[lPortIndex] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
      pcPortNames[lPortIndex] = CrossoverInputPortName;
      psPortRangeHints[lPortIndex].HintDescriptor = 0;
    } else {
      piPortDescriptors[lPortIndex] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
      pcPortNames[lPortIndex] = Crossover[lPortIndex - 1].PortName;
      psPortRangeHints[lPortIndex].HintDescriptor = 0;
      if (
        g_lHistoryBufferLength <
        Crossover[lPortIndex - 1].BandAdditionalDelay +
          Crossover[lPortIndex - 1].BandFIRCoefficientsCount
      ) {
        g_lHistoryBufferLength = Crossover[lPortIndex - 1].BandAdditionalDelay +
          Crossover[lPortIndex - 1].BandFIRCoefficientsCount;
      }
    }
  }
  g_psCrossoverDescriptor->instantiate = instantiate;
  g_psCrossoverDescriptor->connect_port = connect_port;
  g_psCrossoverDescriptor->activate = activate;
  g_psCrossoverDescriptor->run = run;
  g_psCrossoverDescriptor->run_adding = NULL;
  g_psCrossoverDescriptor->set_run_adding_gain = NULL;
  g_psCrossoverDescriptor->deactivate = NULL;
  g_psCrossoverDescriptor->cleanup = cleanup;
}

void _fini() {
  if (g_psCrossoverDescriptor) {
    free((char *)g_psCrossoverDescriptor->Label);
    free((char *)g_psCrossoverDescriptor->Name);
    free((char *)g_psCrossoverDescriptor->Maker);
    free((char *)g_psCrossoverDescriptor->Copyright);
    free((LADSPA_PortDescriptor *)g_psCrossoverDescriptor->PortDescriptors);
    free((char **)g_psCrossoverDescriptor->PortNames);
    free((LADSPA_PortRangeHint *)g_psCrossoverDescriptor->PortRangeHints);
    free(g_psCrossoverDescriptor);
  }
}

const LADSPA_Descriptor * ladspa_descriptor(unsigned long Index) {
  switch (Index) {
    case 0:
      return g_psCrossoverDescriptor;
    default:
      return NULL;
  }
}
