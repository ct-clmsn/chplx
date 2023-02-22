module libsci {
  writeln("Initializing libsci");
  module blas {
    writeln("\tInitializing blas");
  }
}
module testmain { // used to avoid warnings
  proc main() {
   
use libsci.blas;
} }
   
