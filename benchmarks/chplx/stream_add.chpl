proc randindex() {
    //inlinecxx("#include <stdlib.h>");
   //return rand()%5;
   var ret = 0;
   inlinecxx("{} = rand()%5;", ret);
   return ret;
}

config var nx = 134217728; //1000000;

var A:[1..nx] real;
var B:[1..nx] real;
var C:[1..nx] real;

// Initialize arrays
forall i in 1..nx {
   A[i] = randindex();
   B[i] = randindex();
   C[i] = randindex();
}

proc run(A : [] real, B : [] real, C : [] real) {
   // Benchmark Copy
   inlinecxx("hpx::chrono::high_resolution_timer cpy;");
   forall i in 1..nx do {
      C[i] = A[i] + B[i];
   }
   inlinecxx("auto elapsed = cpy.elapsed();");
   inlinecxx("std::cout << \"chapelng,\" << {} << \",\" << {} << \",\" << hpx::resource::get_num_threads() << \",\" << {} << \",\" << {} << \",\" << elapsed << \",0\";", nx, 0, 0, 0);
}

run(A, B, C);
