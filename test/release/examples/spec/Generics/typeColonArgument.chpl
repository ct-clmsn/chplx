proc getOne(type t:numeric) {
  return 1:t;
}
var anInt8 = getOne(int(8));
var aReal = getOne(real);

   
writeln(anInt8.type:string, " ", anInt8);
writeln(aReal.type:string, " ", aReal);

   
