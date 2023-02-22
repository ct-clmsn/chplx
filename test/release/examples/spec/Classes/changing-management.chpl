class C { }
type borrowedC = borrowed C;
type ownedC = (borrowedC:owned);

   
writeln(borrowedC:string);
writeln(ownedC:string);

   
