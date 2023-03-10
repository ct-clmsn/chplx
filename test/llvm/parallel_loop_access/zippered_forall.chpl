config const n = 10000;
proc main() {
  var A:[1..n] real = 0;
  loop(A);
  writeln(A[1], " ", A[n]);
}

proc loop(A) {
  forall (i,j) in zip(1..n, 2..) {
    A[i] = 17.5 * j;
  }
  // check follower loop body in the coforall
  // CHECK: @coforall
  // CHECK: fmul
  // CHECK: !llvm.access.group ![[GROUP2:[0-9]+]]
  // CHECK: br i1
  // CHECK-SAME: !llvm.loop ![[LOOP2:[0-9]+]]
}

// CHECK: ![[LOOP2]] = distinct !{![[LOOP2]], ![[PA2:[0-9]+]]
// CHECK: ![[PA2]] = !{!"llvm.loop.parallel_accesses",
// CHECK-SAME: ![[GROUP2]]
// CHECK-SAME: }
