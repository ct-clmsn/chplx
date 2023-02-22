proc handleFromBegin() {
  try! {
    sync {
      begin canThrow(0);
      begin canThrow(1);
    }
  } catch e: TaskErrors {
    writeln("caught from Locale 0");
  }
}
