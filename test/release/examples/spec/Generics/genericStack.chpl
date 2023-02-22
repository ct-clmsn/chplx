class MyNode {
  type itemType;              // type of item
  var item: itemType;         // item in node
  var next: unmanaged MyNode(itemType)?; // reference to next node (same type)
}

record Stack {
  type itemType;             // type of items
  var top: unmanaged MyNode(itemType)?; // top node on stack linked list

  proc push(item: itemType) {
    top = new unmanaged MyNode(itemType, item, top);
  }

  proc pop() {
    if isEmpty then
      halt("attempt to pop an item off an empty stack");
    var oldTop = top;
    var oldItem = top!.item;
    top = top!.next;
    delete oldTop;
    return oldItem;
  }

  proc isEmpty return top == nil;
}

   
var s: Stack(int);
s.push(1);
s.push(2);
s.push(3);
while !s.isEmpty do
  writeln(s.pop());

   
