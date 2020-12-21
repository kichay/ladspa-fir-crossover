#include <stdlib.h>
#include <string.h>
#include <ladspa.h>

#define STATIC_QUOTE(x) #x
#define QUOTE(x) STATIC_QUOTE(x)

#ifndef FIR_HEADER
#define FIR_HEADER example/3band.h
#endif

#include QUOTE(FIR_HEADER)

LADSPA_Descriptor * g_psFIRDescriptor = NULL;

typedef struct {
  LADSPA_Data * m_pfInputBuffer;
  unsigned int m_lOutputBandCount;
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
  unsigned long lBandIndex;
  unsigned long lHistoryBufferLength = 0;

  psCrossoverInstance = (CrossoverInstance *)malloc(sizeof(CrossoverInstance));
  psCrossoverInstance->m_pfHistoryBuffer = (LADSPA_Data *)malloc(
    sizeof(LADSPA_Data) * (lHistoryBufferLength)
  );
  psCrossoverInstance->m_pfHistoryBufferCurrentElement = psCrossoverInstance->m_pfHistoryBuffer;
  psCrossoverInstance->m_pfHistoryBufferLidElement =
    psCrossoverInstance->m_pfInputBuffer + lHistoryBufferLength;
  for (
    lBandIndex = 0;
    lBandIndex < psCrossoverInstance->m_lOutputBandCount;
    lBandIndex++
  ) {
    if (
      lHistoryBufferLength <
      Crossover[lBandIndex].BandAdditionalDelay +
      Crossover[lBandIndex].BandFIRCoefficientsLength
    ) {
      lHistoryBufferLength = Crossover[lBandIndex].BandAdditionalDelay +
      Crossover[lBandIndex].BandFIRCoefficientsLength;
    }
  }
  psCrossoverInstance->m_lOutputBandCount = sizeof(Crossover) / sizeof(Crossover[0]);
  psCrossoverInstance->m_p2pfOutputBuffer = (LADSPA_Data **)malloc(
    sizeof(LADSPA_Data *) * psCrossoverInstance->m_lOutputBandCount
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
  } else if (0 < Port && Port <= psCrossoverInstance->m_lOutputBandCount) {
    *(psCrossoverInstance->m_p2pfOutputBuffer + Port - 1) = DataLocation;
  }
}

void run (
  LADSPA_Handle Instance,
  unsigned long SampleCount
) {
/*
  LADSPA_Data * pfInput;
  LADSPA_Data * pfOutput;
  FIRInstance * psFIRInstance;
  unsigned long lCoefficientIndex;

  psFIRInstance = (FIRInstance *)Instance;
  pfInput = psFIRInstance->m_pfInputBuffer;
  for (
    pfOutput = psFIRInstance->m_pfOutputBuffer;
    pfOutput < psFIRInstance->m_pfOutputBuffer + SampleCount;
    pfOutput++
  ) {
    *(pfOutput) = 0;
    *(psFIRInstance->m_pfFIRBufferCurrentElement) = *(pfInput++);
    if (++psFIRInstance->m_pfFIRBufferCurrentElement == psFIRInstance->m_pfFIRBufferLidElement)
      psFIRInstance->m_pfFIRBufferCurrentElement = psFIRInstance->m_pfFIRBuffer;
    for(
      lCoefficientIndex = g_lFIRLastCoefficientIndex;
      lCoefficientIndex + 1 > 0;
      lCoefficientIndex--
    ) {
      *(pfOutput) += *(psFIRInstance->m_pfFIRBufferCurrentElement) * FIRCoefficients[lCoefficientIndex];
      if (++psFIRInstance->m_pfFIRBufferCurrentElement == psFIRInstance->m_pfFIRBufferLidElement)
        psFIRInstance->m_pfFIRBufferCurrentElement = psFIRInstance->m_pfFIRBuffer;
    }
  }
*/
}

void cleanup(LADSPA_Handle Instance) {
  CrossoverInstance * psCrossoverInstance;

  psCrossoverInstance = (CrossoverInstance *)Instance;
  free((LADSPA_Data *)psCrossoverInstance->m_pfHistoryBuffer);
  free((LADSPA_Data **)psCrossoverInstance->m_p2pfOutputBuffer);
  free(Instance);
}

void _init() {
  char ** pcPortNames;
  unsigned long lPortIndex;
  LADSPA_PortDescriptor * piPortDescriptors;
  LADSPA_PortRangeHint * psPortRangeHints;
  g_psFIRDescriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
  g_psFIRDescriptor->UniqueID = PLUGIN_ID;
  g_psFIRDescriptor->Label = strdup(PLUGIN_LABEL);
  g_psFIRDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
  g_psFIRDescriptor->Name  = strdup(PLUGIN_NAME);
  g_psFIRDescriptor->Maker = strdup(PLUGIN_MAKER);
  g_psFIRDescriptor->Copyright = strdup(PLUGIN_COPYRIGHT);
  g_psFIRDescriptor->PortCount = sizeof(Crossover) / sizeof(Crossover[0]) + 1;
  piPortDescriptors = (LADSPA_PortDescriptor *)calloc(
    g_psFIRDescriptor->PortCount,
    sizeof(LADSPA_PortDescriptor)
  );
  g_psFIRDescriptor->PortDescriptors = (const LADSPA_PortDescriptor *)piPortDescriptors;
  pcPortNames = (char **)calloc(g_psFIRDescriptor->PortCount, sizeof(char *));
  g_psFIRDescriptor->PortNames = (const char **)pcPortNames;
  psPortRangeHints = (LADSPA_PortRangeHint *)calloc(
    g_psFIRDescriptor->PortCount,
    sizeof(LADSPA_PortRangeHint)
  );
  g_psFIRDescriptor->PortRangeHints = (const LADSPA_PortRangeHint *)psPortRangeHints;
  for(
    lPortIndex = 0;
    lPortIndex < g_psFIRDescriptor->PortCount;
    lPortIndex++
  ) {
    if (lPortIndex == 0) {
      piPortDescriptors[lPortIndex] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
      pcPortNames[lPortIndex] = strdup(PLUGIN_INPUT);
      psPortRangeHints[lPortIndex].HintDescriptor = 0;
    } else {
      piPortDescriptors[lPortIndex] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
      pcPortNames[lPortIndex] = strdup(Crossover[lPortIndex].PortName);
      psPortRangeHints[lPortIndex].HintDescriptor = 0;
    }
  }
  g_psFIRDescriptor->instantiate = instantiate;
  g_psFIRDescriptor->connect_port = connect_port;
  g_psFIRDescriptor->activate = activate;
  g_psFIRDescriptor->run = run;
  g_psFIRDescriptor->run_adding = NULL;
  g_psFIRDescriptor->set_run_adding_gain = NULL;
  g_psFIRDescriptor->deactivate = NULL;
  g_psFIRDescriptor->cleanup = cleanup;
}

void _fini() {
  unsigned long lIndex;
  if (g_psFIRDescriptor) {
    free((char *)g_psFIRDescriptor->Label);
    free((char *)g_psFIRDescriptor->Name);
    free((char *)g_psFIRDescriptor->Maker);
    free((char *)g_psFIRDescriptor->Copyright);
    free((LADSPA_PortDescriptor *)g_psFIRDescriptor->PortDescriptors);
    for (lIndex = 0; lIndex < g_psFIRDescriptor->PortCount; lIndex++)
      free((char *)(g_psFIRDescriptor->PortNames[lIndex]));
    free((char **)g_psFIRDescriptor->PortNames);
    free((LADSPA_PortRangeHint *)g_psFIRDescriptor->PortRangeHints);
    free(g_psFIRDescriptor);
  }
}

const LADSPA_Descriptor * ladspa_descriptor(unsigned long Index) {
  switch (Index) {
    case 0:
      return g_psFIRDescriptor;
    default:
      return NULL;
  }
}
