proc randindex() {
    //inlinecxx("#include <stdlib.h>");
   //return rand()%5;
   var ret = 0;
   inlinecxx("{} = rand()%5;", ret);
   return ret;
}


  const iterations = 10;
config var arraySize = 1000000;

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
    forall a in 1..iterations do {
      C[a] = A[a];
    }
inlinecxx("auto elapsed = cpy.elapsed();");
inlinecxx("std::cout << \"Copy: \" << hpx::resource::get_num_threads() << \",\" << elapsed << \"\\n\";");

  // Benchmark Scale
inlinecxx("hpx::chrono::high_resolution_timer scale;");
    forall b in 1..iterations do{
      C[b] = 3.0 * A[b];
    }
inlinecxx("elapsed = scale.elapsed();");
inlinecxx("std::cout << \"Scale: \" << hpx::resource::get_num_threads() << \",\" << elapsed << \"\\n\";");

  // Benchmark Add
inlinecxx("hpx::chrono::high_resolution_timer add;");
    forall c in 1..iterations do{
      C[c] = A[c] + B[c];
    }
inlinecxx("elapsed = add.elapsed();");
inlinecxx("std::cout << \"Add: \" << hpx::resource::get_num_threads() << \",\" << elapsed << \"\\n\";");

  // Benchmark Triad
inlinecxx("hpx::chrono::high_resolution_timer triad;");
    forall d in 1..iterations do{
      D[d] = A[d] + 3.0 * B[d];
    }
inlinecxx("elapsed = cpy.elapsed();");
inlinecxx("std::cout << \"Triad: \" << hpx::resource::get_num_threads() << \",\" << elapsed << \"\\n\";");

// Example usage with an array size of 1000000
//streamBenchmark();
