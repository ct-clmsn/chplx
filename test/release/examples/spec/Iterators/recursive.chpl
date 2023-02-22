class Tree {
  var data: string;
  var left, right: unmanaged Tree?;
  proc deinit() {
    if left then delete left;
    if right then delete right;
  }
}

var tree = new unmanaged Tree("a", new unmanaged Tree("b"), new unmanaged Tree("c", new unmanaged Tree("d"), new unmanaged Tree("e")));

   
iter postorder(tree: Tree?): string {
  if tree != nil {
    for child in postorder(tree!.left) do
      yield child;
    for child in postorder(tree!.right) do
      yield child;
    yield tree!.data;
  }
}

   
proc Tree.writeThis(x)
{
  var first = true;
  for node in postorder(this) {
    if first then first = false;
      else x.write(" ");
    write(node);
  }
}
writeln("Tree Data");
writeln(tree);
delete tree;
