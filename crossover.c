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
unsigned long g_lCircleBufferLength = 0;

typedef struct BufferElement {
  LADSPA_Data Value;
  struct BufferElement * Previous;
  struct BufferElement * Next;
  struct BufferElement ** BandsAdditionalDelay;
} BufferElement;

typedef struct {
  LADSPA_Data * m_pfInputBuffer;
  LADSPA_Data ** m_p2pfOutputBuffer;
  struct BufferElement * m_psCircleBuffer;
} CrossoverInstance;

LADSPA_Handle instantiate (
  const LADSPA_Descriptor * Descriptor,
  unsigned long SampleRate
) {
  CrossoverInstance * psCrossoverInstance;
  struct BufferElement * psBufferCurrent;
  struct BufferElement * psBufferBarrier;

  psCrossoverInstance = (CrossoverInstance *)malloc(sizeof(CrossoverInstance));
  psCrossoverInstance->m_p2pfOutputBuffer = (LADSPA_Data **)malloc(
    sizeof(LADSPA_Data *) * CrossoverData.BandsCount
  );
  psCrossoverInstance->m_psCircleBuffer = (struct BufferElement *)malloc(
    sizeof(struct BufferElement) * g_lCircleBufferLength
  );
  for(
    psBufferCurrent = psCrossoverInstance->m_psCircleBuffer,
      psBufferBarrier = psCrossoverInstance->m_psCircleBuffer + g_lCircleBufferLength;
    psBufferCurrent < psBufferBarrier;
    psBufferCurrent++
  ) {
    psBufferCurrent->BandsAdditionalDelay = (struct BufferElement **)malloc(
      sizeof(struct BufferElement) * CrossoverData.BandsCount
    );
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
  return (LADSPA_Handle)psCrossoverInstance;
}

void activate (
  LADSPA_Handle Instance
) {
  CrossoverInstance * psCrossoverInstance;
  struct BufferElement * psBufferCurrent;
  struct BufferElement * psBufferBarrier;

  psCrossoverInstance = (CrossoverInstance *)Instance;
  for(
    psBufferCurrent = psCrossoverInstance->m_psCircleBuffer,
      psBufferBarrier = psCrossoverInstance->m_psCircleBuffer + g_lCircleBufferLength;
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
  } else if (0 < Port && Port <= CrossoverData.BandsCount) {
    *(psCrossoverInstance->m_p2pfOutputBuffer + Port - 1) = DataLocation;
  }
}

void run (
  LADSPA_Handle Instance,
  unsigned long SampleCount
) {
}

void cleanup(LADSPA_Handle Instance) {
  CrossoverInstance * psCrossoverInstance;
  struct BufferElement * psBufferCurrent;
  struct BufferElement * psBufferBarrier;

  psCrossoverInstance = (CrossoverInstance *)Instance;
  for(
    psBufferCurrent = psCrossoverInstance->m_psCircleBuffer,
      psBufferBarrier = psCrossoverInstance->m_psCircleBuffer + g_lCircleBufferLength;
    psBufferCurrent < psBufferBarrier;
    psBufferCurrent++
  ) {
    free((struct BufferElement **)psBufferCurrent->BandsAdditionalDelay);
  }
  free((struct BufferElement *)psCrossoverInstance->m_psCircleBuffer);
  free((LADSPA_Data **)psCrossoverInstance->m_p2pfOutputBuffer);
  free(Instance);
}

void _init() {
  unsigned long lPortIndex;
  unsigned long lBandDelay;
  char ** pcPortNames;
  LADSPA_PortDescriptor * piPortDescriptors;
  LADSPA_PortRangeHint * psPortRangeHints;

  g_psCrossoverDescriptor = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
  g_psCrossoverDescriptor->UniqueID = PLUGIN_ID;
  g_psCrossoverDescriptor->Label = strdup(PLUGIN_LABEL);
  g_psCrossoverDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
  g_psCrossoverDescriptor->Name  = strdup(PLUGIN_NAME);
  g_psCrossoverDescriptor->Maker = strdup(PLUGIN_MAKER);
  g_psCrossoverDescriptor->Copyright = strdup(PLUGIN_COPYRIGHT);
  g_psCrossoverDescriptor->PortCount = CrossoverData.BandsCount + 1;
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
  g_lCircleBufferLength = 0;
  for(
    lPortIndex = 0;
    lPortIndex < g_psCrossoverDescriptor->PortCount;
    lPortIndex++
  ) {
    if (lPortIndex == 0) {
      piPortDescriptors[lPortIndex] = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
      pcPortNames[lPortIndex] = CrossoverData.InputPortName;
      psPortRangeHints[lPortIndex].HintDescriptor = 0;
    } else {
      piPortDescriptors[lPortIndex] = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
      pcPortNames[lPortIndex] = CrossoverData.BandsPointer[lPortIndex - 1]->PortName;
      psPortRangeHints[lPortIndex].HintDescriptor = 0;
      lBandDelay = CrossoverData.BandsPointer[lPortIndex - 1]->AdditionalDelay +
          CrossoverData.BandsPointer[lPortIndex - 1]->FIRCoefficientsCount;
      if (lBandDelay > g_lCircleBufferLength)
        g_lCircleBufferLength = lBandDelay;
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
