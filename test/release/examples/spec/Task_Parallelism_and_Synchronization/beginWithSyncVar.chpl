class Tree {
  var isLeaf: bool;
  var left, right: unmanaged Tree?;
  var value: int;

  proc sum():int {
    if (isLeaf) then
       return value;

    var x$: sync int;
    begin x$.writeEF(left!.sum());
    var y = right!.sum();
    return x$.readFE() + y;
  }
}
var tree: unmanaged Tree = new unmanaged Tree(false, new unmanaged Tree(false, new unmanaged Tree(true, nil, nil, 1),
                                                 new unmanaged Tree(true, nil, nil, 1), 1),
                                 new unmanaged Tree(false, new unmanaged Tree(true, nil, nil, 1),
                                                 new unmanaged Tree(true, nil, nil, 1), 1), 1);
writeln(tree.sum());
proc Tree.deinit() {
  if isLeaf then return;
  delete left;
  delete right;
}
delete tree;
