use IO;

class mything {
  var x:int;

  proc readThis(r) throws {
    r.read(x);
  }

  proc writeThis(w) throws {
    w.write(x);
  }
}

class subthing : mything {
  var y:int;

  override proc readThis(r) throws {
    x = r.read(int);
    r.readLiteral(",");
    y = r.read(int);
  }

  override proc writeThis(w) throws {
    w.write(x);
    w.writeLiteral(",");
    w.write(y);
  }
}


{
  var a = (new owned mything(1)).borrow();

  writeln("Writing ", a);

  var f = openmem();
  var w = f.writer();

  w.write(a);
  w.close();

  var r = f.reader();

  var b = (new owned mything(2)).borrow();

  r.read(b);
  r.close();

  writeln("Read ", b);

  assert(a.x == b.x);
}

{
  var a = (new owned subthing(3,4)).borrow();

  writeln("Writing ", a);

  var f = openmem();
  var w = f.writer();

  w.write(a);
  w.close();

  var r = f.reader();

  var b = (new owned subthing(5,6)).borrow();

  r.read(b);
  r.close();

  writeln("Read ", b);

  assert(a.x == b.x);
  assert(a.y == b.y);
}

{
  var a = (new owned subthing(3,4)).borrow();

  writeln("Writing ", a);

  var f = openmem();
  var w = f.writer();

  w.write(a);
  w.close();

  var r = f.reader();

  var b = (new owned subthing(5,6)).borrow();
  var c:borrowed mything = b;

  r.read(c);
  r.close();

  writeln("Read ", b);

  assert(a.x == b.x);
  assert(a.y == b.y);
}


