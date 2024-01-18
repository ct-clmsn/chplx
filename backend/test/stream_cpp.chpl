proc randindex() {
    //inlinecxx("#include <stdlib.h>");
   //return rand()%5;
   var ret = 0;
   inlinecxx("{} = rand()%5;", ret);
   return ret;
}


proc streamBenchmark() {
  const iterations = 10;

const arraySize = 1000000;
  //writeln("STREAM Benchmark (Chapel - Embarrassingly Parallel)");
  //writeln("===================================================");

  var A:[1..arraySize] real;
  var B:[1..arraySize] real;
  var C:[1..arraySize] real;
  var D:[1..arraySize] real;

  // Initialize arrays
  forall i in 1..arraySize {
    A[i] = randindex();
    B[i] = randindex();
    C[i] = randindex();
    D[i] = 0.0;
  }


  // Benchmark Copy
inlinecxx("hpx::chrono::high_resolution_timer cpy;");
    forall it in 1..iterations do {
      C[it] = A[it];
    }
inlinecxx("auto elapsed = cpy.elapsed();");
inlinecxx("std::cout << \"Copy: \" << hpx::resource::get_num_threads() << \",\" << elapsed << \"\\n\";");

  // Benchmark Scale
inlinecxx("hpx::chrono::high_resolution_timer scale;");
    forall it in 1..iterations do{
      C[it] = 3.0 * A[it];
    }
inlinecxx("elapsed = scale.elapsed();");
inlinecxx("std::cout << \"Scale: \" << hpx::resource::get_num_threads() << \",\" << elapsed << \"\\n\";");

  // Benchmark Add
inlinecxx("hpx::chrono::high_resolution_timer add;");
    forall it in 1..iterations do{
      C[it] = A[it] + B[it];
    }
inlinecxx("elapsed = add.elapsed();");
inlinecxx("std::cout << \"Add: \" << hpx::resource::get_num_threads() << \",\" << elapsed << \"\\n\";");

  // Benchmark Triad
inlinecxx("hpx::chrono::high_resolution_timer triad;");
    forall it in 1..iterations do{
      D[it] = A[it] + 3.0 * B[it];
    }
inlinecxx("elapsed = cpy.elapsed();");
inlinecxx("std::cout << \"Triad: \" << hpx::resource::get_num_threads() << \",\" << elapsed << \"\\n\";");
}

// Example usage with an array size of 1000000
streamBenchmark();
