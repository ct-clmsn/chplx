class DemoError : Error { }
proc handleFromForall() {
  try! {
    writeln("before forall block");
    forall i in 1..2 {
      throw new owned DemoError();
    }
    writeln("after forall block");
  } catch errors: TaskErrors {
    for e in errors {
      writeln("Caught task error e ", e!.message());
    }
  }
}
