#include "error.h"

const int mainHasArgs = 0;
const int mainPreserveDelimiter = 0;
void CreateConfigVarTable(void) {
  initConfigVarTable();
  installConfigVar("printModuleInitOrder", "bool", "Built-in", /* private = */ 0, /* deprecated = */ 0, "printModuleInitOrder is deprecated");
  installConfigVar("dataParTasksPerLocale", "int(64)", "Built-in", /* private = */ 0, /* deprecated = */ 0, "dataParTasksPerLocale is deprecated");
  installConfigVar("dataParIgnoreRunningTasks", "bool", "Built-in", /* private = */ 0, /* deprecated = */ 0, "dataParIgnoreRunningTasks is deprecated");
  installConfigVar("dataParMinGranularity", "int(64)", "Built-in", /* private = */ 0, /* deprecated = */ 0, "dataParMinGranularity is deprecated");
  installConfigVar("memTrack", "bool", "Built-in", /* private = */ 0, /* deprecated = */ 0, "memTrack is deprecated");
  installConfigVar("memStats", "bool", "Built-in", /* private = */ 0, /* deprecated = */ 0, "memStats is deprecated");
  installConfigVar("memLeaksByType", "bool", "Built-in", /* private = */ 0, /* deprecated = */ 0, "memLeaksByType is deprecated");
  installConfigVar("memLeaks", "bool", "Built-in", /* private = */ 0, /* deprecated = */ 0, "memLeaks is deprecated");
  installConfigVar("memMax", "uint(64)", "Built-in", /* private = */ 0, /* deprecated = */ 0, "memMax is deprecated");
  installConfigVar("memThreshold", "uint(64)", "Built-in", /* private = */ 0, /* deprecated = */ 0, "memThreshold is deprecated");
  installConfigVar("memLog", "string", "Built-in", /* private = */ 0, /* deprecated = */ 0, "memLog is deprecated");
  installConfigVar("memLeaksLog", "string", "Built-in", /* private = */ 0, /* deprecated = */ 0, "memLeaksLog is deprecated");
  installConfigVar("memLeaksByDesc", "string", "Built-in", /* private = */ 0, /* deprecated = */ 0, "memLeaksByDesc is deprecated");
  installConfigVar("debugGpu", "bool", "Built-in", /* private = */ 0, /* deprecated = */ 0, "debugGpu is deprecated");
  installConfigVar("gpuNoCpuModeWarning", "bool", "Built-in", /* private = */ 0, /* deprecated = */ 0, "gpuNoCpuModeWarning is deprecated");
  installConfigVar("enableGpuP2P", "bool", "Built-in", /* private = */ 0, /* deprecated = */ 0, "enableGpuP2P is deprecated");
  installConfigVar("defaultHashTableResizeThreshold", "real(64)", "Built-in", /* private = */ 0, /* deprecated = */ 0, "defaultHashTableResizeThreshold is deprecated");
  installConfigVar("numLocales", "int(64)", "Built-in", /* private = */ 0, /* deprecated = */ 0, "numLocales is deprecated");
  installConfigVar("ghosts", "int(64)", "heat", /* private = */ 0, /* deprecated = */ 0, "ghosts is deprecated");
  installConfigVar("k", "real(64)", "heat", /* private = */ 0, /* deprecated = */ 0, "k is deprecated");
  installConfigVar("dt", "real(64)", "heat", /* private = */ 0, /* deprecated = */ 0, "dt is deprecated");
  installConfigVar("dx", "real(64)", "heat", /* private = */ 0, /* deprecated = */ 0, "dx is deprecated");
  installConfigVar("nx", "int(64)", "heat", /* private = */ 0, /* deprecated = */ 0, "nx is deprecated");
  installConfigVar("nt", "int(64)", "heat", /* private = */ 0, /* deprecated = */ 0, "nt is deprecated");
  installConfigVar("threads", "int(64)", "heat", /* private = */ 0, /* deprecated = */ 0, "threads is deprecated");
}


