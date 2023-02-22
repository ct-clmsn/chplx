use defineActor;
var anActor = new unmanaged Actor(name="Tommy", age=26);

   
var s: string = anActor.name;
anActor.age = 27;

   
writeln((s, anActor));
if (cleanUp) then delete anActor;

   
