use IO;

var line:string;
var i = 0;
while stdin.readLine(line) {
  if i & 1 == 0 {
    stdout.write(line);
  } else {
    stderr.write(line);
  }
  i += 1;
}
