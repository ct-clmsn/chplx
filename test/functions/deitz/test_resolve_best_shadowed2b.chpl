module OuterModule {
  proc main() {
    proc f() { }
    {
      proc f() { } // shadows other f based on call to f
      f();
    }

    public use M3;
    public use M4;
    h();
  }

  module M3 {
    public use super.M5;
    proc h() { }
  }

  module M4 {
    public use super.M5;
  }

  module M5 {
    proc h() { }
  }
}
