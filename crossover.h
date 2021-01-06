#include <boost/preprocessor/repetition/repeat.hpp>

LADSPA_Handle instantiate (
  const LADSPA_Descriptor * Descriptor,
  unsigned long SampleRate
);

void activate (
  LADSPA_Handle Instance
);

void connect_port (
  LADSPA_Handle Instance,
  unsigned long Port,
  LADSPA_Data * DataLocation
);

void run (
  LADSPA_Handle Instance,
  unsigned long SampleCount
);

void cleanup(
  LADSPA_Handle Instance
);

LADSPA_PortDescriptor PortDescriptors[] = {
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  #define WRITE_PORT_DESCRIPTOR(z, n, _) LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
  BOOST_PP_REPEAT(CROSSOVER_BAND_COUNT, WRITE_PORT_DESCRIPTOR, nil)
};

LADSPA_PortRangeHint PortRangeHints[] = {
  #define WRITE_PORT_RANGE_HINT(z, n, _) {0},
  BOOST_PP_REPEAT(PLUGIN_PORT_COUNT, WRITE_PORT_RANGE_HINT, nil)
};

LADSPA_Descriptor CrossoverDescriptor = { 
  PLUGIN_UNIQUE_ID,
  PLUGIN_LABEL,
  LADSPA_PROPERTY_HARD_RT_CAPABLE,
  PLUGIN_NAME,
  PLUGIN_MAKER,
  PLUGIN_COPYRIGHT,
  PLUGIN_PORT_COUNT,
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
