#include <stdlib.h>
#include <string.h>
#include <ladspa.h>

#define SIMPLE_QUOTE(x) #x
#define QUOTE(x) SIMPLE_QUOTE(x)

#ifndef FIR_HEADER
#define FIR_HEADER example.h
#endif

#include QUOTE(FIR_HEADER)

#define FIR_INPUT 0
#define FIR_OUTPUT 1
#define FIR_LENGTH (sizeof(FIRArray) / sizeof(FIRArray[0]))

typedef struct {
  LADSPA_Data * m_pfInputBuffer;
  LADSPA_Data * m_pfOutputBuffer;
  LADSPA_Data * m_pfFIRBuffer;
  unsigned long lFIRBufferOffset;
} FIRInstance;

LADSPA_Handle instantiate (
  const LADSPA_Descriptor * Descriptor,
  unsigned long SampleRate
) {
  FIRInstance * psFIRInstance;
  psFIRInstance = (FIRInstance *)malloc(sizeof(FIRInstance));
  psFIRInstance->m_pfFIRBuffer = (LADSPA_Data *)malloc(FIR_LENGTH * sizeof(LADSPA_Data));
  return (LADSPA_Handle)psFIRInstance;
}

void activate (
  LADSPA_Handle Instance
) {
  FIRInstance * psFIRInstance;
  LADSPA_Data * pfFIRBuffer;
  unsigned long lIndex;

  psFIRInstance = (FIRInstance *)Instance;
  psFIRInstance->lFIRBufferOffset = 0;
  pfFIRBuffer = psFIRInstance->m_pfFIRBuffer;
  for (lIndex = 0; lIndex < FIR_LENGTH; lIndex++) {
    *(pfFIRBuffer++) = 0;
  }
}

void connect_port (
  LADSPA_Handle Instance,
  unsigned long Port,
  LADSPA_Data * DataLocation
) {
  FIRInstance * psFIRInstance;
  psFIRInstance = (FIRInstance *)Instance;

  switch (Port) {
  case FIR_INPUT:
    psFIRInstance->m_pfInputBuffer = DataLocation;
    break;
  case FIR_OUTPUT:
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
  LADSPA_Data * pfHistory;
  FIRInstance * psFIRInstance; 
  unsigned long lHistoryIndex;

  psFIRInstance = (FIRInstance *)Instance;
  pfInput = psFIRInstance->m_pfInputBuffer;
  for (
    pfOutput = psFIRInstance->m_pfOutputBuffer;
    pfOutput < psFIRInstance->m_pfOutputBuffer + SampleCount;
    pfOutput++
  ) {
    pfHistory = psFIRInstance->m_pfFIRBuffer + psFIRInstance->lFIRBufferOffset;
    *(pfHistory) = *(pfInput++);
    *(pfOutput) = 0;
    for(
      lHistoryIndex = 0;
      lHistoryIndex < psFIRInstance->lFIRBufferOffset + 1;
      lHistoryIndex++
    ) {
      *(pfOutput) += *(pfHistory--) * FIRArray[lHistoryIndex];
    }
    pfHistory = psFIRInstance->m_pfFIRBuffer + FIR_LENGTH - 1;
    for(
      lHistoryIndex = psFIRInstance->lFIRBufferOffset + 1;
      lHistoryIndex < FIR_LENGTH;
      lHistoryIndex++
    ) {
      *(pfOutput) += *(pfHistory--) * FIRArray[lHistoryIndex];
    }
    psFIRInstance->lFIRBufferOffset++;
    psFIRInstance->lFIRBufferOffset %= FIR_LENGTH;
  }
}

void cleanup(LADSPA_Handle Instance) {
  FIRInstance * psFIRInstance;

  psFIRInstance = (FIRInstance *)Instance;
  free((LADSPA_Data *)psFIRInstance->m_pfFIRBuffer);
  free(Instance);
}

LADSPA_Descriptor * g_psFIRDescriptor = NULL;

void _init() {
  char ** pcPortNames;
  LADSPA_PortDescriptor * piPortDescriptors;
  LADSPA_PortRangeHint * psPortRangeHints;
  g_psFIRDescriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
  g_psFIRDescriptor->UniqueID = PLUGIN_ID;
  g_psFIRDescriptor->Label = strdup(PLUGIN_LABEL);
  g_psFIRDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
  g_psFIRDescriptor->Name  = strdup(PLUGIN_NAME);
  g_psFIRDescriptor->Maker = strdup(PLUGIN_MAKER);
  g_psFIRDescriptor->Copyright = strdup(PLUGIN_COPYRIGHT);
  g_psFIRDescriptor->PortCount = 2;
  piPortDescriptors = (LADSPA_PortDescriptor *)calloc(2, sizeof(LADSPA_PortDescriptor));
  g_psFIRDescriptor->PortDescriptors = (const LADSPA_PortDescriptor *)piPortDescriptors;
  piPortDescriptors[FIR_INPUT] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
  piPortDescriptors[FIR_OUTPUT] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
  pcPortNames = (char **)calloc(2, sizeof(char *));
  g_psFIRDescriptor->PortNames = (const char **)pcPortNames;
  pcPortNames[FIR_INPUT] = strdup("Input");
  pcPortNames[FIR_OUTPUT] = strdup("Output");
  psPortRangeHints = ((LADSPA_PortRangeHint *)calloc(2, sizeof(LADSPA_PortRangeHint)));
  g_psFIRDescriptor->PortRangeHints = (const LADSPA_PortRangeHint *)psPortRangeHints;
  psPortRangeHints[FIR_INPUT].HintDescriptor = 0;
  psPortRangeHints[FIR_OUTPUT].HintDescriptor = 0;
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
