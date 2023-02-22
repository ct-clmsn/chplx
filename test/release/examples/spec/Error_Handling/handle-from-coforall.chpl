class DemoError : Error { }
proc handleFromCoforall() {
  try! {
    writeln("before coforall block");
    coforall i in 1..2 {
      coforall j in 1..2 {
        throw new owned DemoError();
      }
    }
    writeln("after coforall block");
  } catch errors: TaskErrors { // not nested
    // all of e will be of runtime type DemoError in this example
    for e in errors {
      writeln("Caught task error e ", e!.message());
    }
  }
}
