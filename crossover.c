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
  LADSPA_Data * m_pfOutputBuffer;
  LADSPA_Data * m_pfFIRBuffer;
  LADSPA_Data * m_pfFIRBufferCurrentElement;
  LADSPA_Data * m_pfFIRBufferLidElement;
} FIRInstance;

/*

LADSPA_Handle instantiate (
  const LADSPA_Descriptor * Descriptor,
  unsigned long SampleRate
) {
  FIRInstance * psFIRInstance;

  psFIRInstance = (FIRInstance *)malloc(sizeof(FIRInstance));
  psFIRInstance->m_pfFIRBuffer = (LADSPA_Data *)malloc(
    sizeof(LADSPA_Data) * (g_lFIRLastCoefficientIndex + 1)
  );
  psFIRInstance->m_pfFIRBufferCurrentElement = psFIRInstance->m_pfFIRBuffer;
  psFIRInstance->m_pfFIRBufferLidElement = psFIRInstance->m_pfFIRBuffer + g_lFIRLastCoefficientIndex + 1;
  return (LADSPA_Handle)psFIRInstance;
}

void activate (
  LADSPA_Handle Instance
) {
  FIRInstance * psFIRInstance;
  LADSPA_Data * pfFIRBufferResetElement;

  psFIRInstance = (FIRInstance *)Instance;
  for (
    pfFIRBufferResetElement = psFIRInstance->m_pfFIRBuffer;
    pfFIRBufferResetElement < psFIRInstance->m_pfFIRBufferLidElement;
    *(pfFIRBufferResetElement++) = 0
  );
}

void connect_port (
  LADSPA_Handle Instance,
  unsigned long Port,
  LADSPA_Data * DataLocation
) {
  FIRInstance * psFIRInstance;
  psFIRInstance = (FIRInstance *)Instance;

  switch (Port) {
  case FIR_INPUT_PORT:
    psFIRInstance->m_pfInputBuffer = DataLocation;
    break;
  case FIR_OUTPUT_PORT:
    psFIRInstance->m_pfOutputBuffer = DataLocation;
    break;
  }
}

void run (
  LADSPA_Handle Instance,
  unsigned long SampleCount
) {
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
}

void cleanup(LADSPA_Handle Instance) {
  FIRInstance * psFIRInstance;

  psFIRInstance = (FIRInstance *)Instance;
  free((LADSPA_Data *)psFIRInstance->m_pfFIRBuffer);
  free(Instance);
}

*/

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
  /*
  g_psFIRDescriptor->instantiate = instantiate;
  g_psFIRDescriptor->connect_port = connect_port;
  g_psFIRDescriptor->activate = activate;
  g_psFIRDescriptor->run = run;
  g_psFIRDescriptor->run_adding = NULL;
  g_psFIRDescriptor->set_run_adding_gain = NULL;
  g_psFIRDescriptor->deactivate = NULL;
  g_psFIRDescriptor->cleanup = cleanup;
  */
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
