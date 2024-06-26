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

// Example usage with an array size of 1000000
config var arraySize = 134217728; //1000000;

var A:[1..arraySize] real;
var C:[1..arraySize] real;

// Initialize arrays
forall i in 1..arraySize {
   A[i] = randindex();
   C[i] = randindex();
}

proc run(A : [] real, C : [] real) {
   var copy_t: stopwatch;
   copy_t.start();

   // Benchmark Copy
   forall it in 1..arraySize do {
      C[it] = 3.0 * A[it];
   }
   copy_t.stop();
   writeln(getenv('CHPL_RT_NUM_THREADS_PER_LOCALE'.c_str()):string, ",",copy_t.elapsed());
}

run(A, C);
