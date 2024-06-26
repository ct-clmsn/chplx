proc randindex() {
    //inlinecxx("#include <stdlib.h>");
   //return rand()%5;
   var ret = 0;
   inlinecxx("{} = rand()%5;", ret);
   return ret;
}

config var arraySize = 134217728; //1000000;

var A:[1..arraySize] real;
var C:[1..arraySize] real;

// Initialize arrays
forall i in 1..arraySize {
   A[i] = randindex();
   C[i] = randindex();
}

proc run(A : [] real, C : [] real) {
   // Benchmark Copy
   inlinecxx("hpx::chrono::high_resolution_timer cpy;");
   forall i in 1..arraySize do {
      C[i] = A[i];
   }
   inlinecxx("auto elapsed = cpy.elapsed();");
   inlinecxx("std::cout << hpx::resource::get_num_threads() << \",\" << elapsed << \"\\n\";");
}

run(A, C);
