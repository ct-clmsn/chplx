var s: [1..10] int = [i in 1..10] i;
var result =

   
[i in 1..s.size] if i % 2 == 1 then s(i)

   
;
writeln(result);
writeln(result.domain);

   
