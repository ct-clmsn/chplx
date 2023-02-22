module LibraryA {
  proc callWorkers(arg) {
    worker1();
    worker2();
  }
}

module LibraryB {
  use LibraryA;
  proc worker1() { writeln("in LibraryB"); }
  proc libFun(arg) {
    callWorkers(arg);
  }
}

module Application {
  use LibraryB;
  proc worker1() { writeln("in Application"); }
  proc worker2() { writeln("in Application"); }
  proc main() {
    libFun(1);
  }
}
