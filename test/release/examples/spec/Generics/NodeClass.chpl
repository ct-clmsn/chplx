class Node {
  type eltType;
  var data: eltType;
  var next: unmanaged Node(eltType)?;
}

   
var n: unmanaged Node(real) = new unmanaged Node(real, 3.14);
writeln(n.data);
writeln(n.next);
writeln(n.next.type:string);
delete n;

   
