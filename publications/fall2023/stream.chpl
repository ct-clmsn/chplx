use CTypes;

use Time;

extern proc getenv(name : c_string) : c_string;
extern proc sizeof(type T): uint(32);
extern const RAND_MAX: c_int;
extern proc rand(): c_int;

extern proc srand(seed: c_uint);

srand(0);

proc randindex() {
   return rand()%5;
}

proc streamBenchmark(n: int) {
  const iterations = 10;

  writeln("STREAM Benchmark (Chapel - Embarrassingly Parallel)");
  writeln("===================================================");

  var A:[1..n] real;
  var B:[1..n] real;
  var C:[1..n] real;
  var D:[1..n] real;

  // Initialize arrays
  forall i in 1..n {
    A[i] = randindex();
    B[i] = randindex();
    C[i] = randindex();
    D[i] = 0.0;
  }

  var copy_t: stopwatch;
  copy_t.start();

  // Benchmark Copy
    forall it in 1..iterations do {
      C[it] = A[it];
    }
    copy_t.stop();
  writeln("Copy: ",getenv('CHPL_RT_NUM_THREADS_PER_LOCALE'.c_str()):string, ",",copy_t.elapsed());

  var scale_t: stopwatch;
  scale_t.start();
  // Benchmark Scale
    forall it in 1..iterations do{
      C[it] = 3.0 * A[it];
    }
  writeln("Scale: ",getenv('CHPL_RT_NUM_THREADS_PER_LOCALE'.c_str()):string, ",", scale_t.elapsed());

  var add_t: stopwatch;
  add_t.start();

  // Benchmark Add
    forall it in 1..iterations do{
      C[it] = A[it] + B[it];
    }
  writeln("Add: ",getenv('CHPL_RT_NUM_THREADS_PER_LOCALE'.c_str()):string, ",", add_t.elapsed());

  var triad_t:stopwatch();
  triad_t.start();

  // Benchmark Triad
    forall it in 1..iterations do{
      D[it] = A[it] + 3.0 * B[it];
    }
  writeln("Triad: ",getenv('CHPL_RT_NUM_THREADS_PER_LOCALE'.c_str()):string, ",", triad_t.elapsed());
}

// Example usage with an array size of 1000000
const arraySize = 1000000;
streamBenchmark(arraySize);
