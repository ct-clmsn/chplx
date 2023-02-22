class Node {
  var data;
  var next: unmanaged Node(data.type)? = nil;
}
var list = new unmanaged Node(1);
list.next = new unmanaged Node(2);

   
writeln(list.data);
writeln(list.next!.data);
delete list.next;
delete list;

   
