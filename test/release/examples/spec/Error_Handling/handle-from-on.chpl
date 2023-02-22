proc handleFromOn() {
  try {
    on Locales[0] {
      canThrow(1);
    }
  } catch {
    writeln("caught from Locale 0");
  }
}
