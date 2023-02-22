class DemoError : Error { }
proc handleFromCobegin() {
  try! {
    writeln("before cobegin block");
    cobegin {
      throw new owned DemoError();
      throw new owned DemoError();
    }
    writeln("after cobegin block");
  } catch errors: TaskErrors {
    for e in errors {
      writeln("Caught task error e ", e!.message());
    }
  }
}
