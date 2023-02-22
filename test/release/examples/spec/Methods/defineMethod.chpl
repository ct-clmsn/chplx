class Actor {
  var name: string;
  var age: uint;
}
var anActor = new Actor(name="Tommy", age=27);
writeln(anActor);

   
proc Actor.print() {
  writeln("Actor ", name, " is ", age, " years old");
}

   
anActor.print();

   
