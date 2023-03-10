use Regex;

config type t = string;

writeln("Compile/Delete");
{
  var r = compile("[a-z]+":t);
}

writeln("Search 1");
{
  var r = compile("[a-z]+":t);

  var str = " test ":t;
  {
    var match = r.search(str);

    writeln(match);

    writeln(str[match]);
  }
  {
    var match = str.find(r);

    writeln(match);

  }
}

writeln("Search 2");
{
  var r = compile("(t)[a-z]+":t);

  var str = " test ":t;
  var cap:t;
  {
    var match = r.search(str, cap);

    writeln(match);
    writeln(cap);

    writeln(str[match]);
  }
  {
    var match = str.find(r);

    writeln(match);
  }
}

writeln("Search 3");
{
  var r = compile("(t)[a-z]+":t);

  var str = " test ":t;
  var cap:regexMatch;
  {
    var match = r.search(str, cap);

    writeln(match);
    writeln(cap);

    writeln(str[match]);
  }
  {
    var match = str.find(r);

    writeln(match);
  }
}

writeln("Search 4");
{
  var r = compile("([a-z]+)":t);

  var str = " test ":t;
  var cap:regexMatch;
  {
    var match = r.search(str, cap);

    writeln(match);
    writeln(cap);

    writeln(str[match]);
  }{
    var match = str.find(r);

    writeln(match);
  }
}

writeln("Search 5");
{
  var r = compile("([0-9]+)":t);

  var str = " 57 ":t;
  var cap:int;
  {
    var match = r.search(str, cap);

    writeln(match);
    writeln(cap);

    writeln(str[match]);
  }
  {
    var match = str.find(r);

    writeln(match);
  }
}

writeln("Match 1");
{
  var r = compile("[a-z]+":t);

  var str = "test ":t;
  {
    var match = r.match(str);

    writeln(match);

    writeln(str[match]);
  }
  {
    var match = str.startsWith(r);

    writeln(match);
  }
}

writeln("Match 2");
{
  var r = compile("(t)[a-z]+":t);

  var str = "test ":t;
  var cap:t;
  {
    var match = r.match(str, cap);

    writeln(match);
    writeln(cap);

    writeln(str[match]);
  }
  {
    var match = str.startsWith(r);

    writeln(match);
  }
}

writeln("Match 3");
{
  var r = compile("(t)[a-z]+":t);

  var str = " test ":t;
  var cap:t;
  {
    var match = r.match(str, cap);

    writeln(match);
    writeln(cap);

    writeln(str[match]);
  }
  {
    var match = str.startsWith(r);

    writeln(match);
  }
}

writeln("Split 1");
{
  var split = compile("\\W+":t);
  var str = "Words, words, words.":t;
  for s in split.split(str) {
    writeln(s);
  }
  for s in str.split(split) {
    writeln(s);
  }
}

writeln("Split 2");
{
  var split = compile("(\\W+)":t);
  var str = "Words, words, words.":t ;
  for s in split.split(str) {
    writeln(s);
  }
  for s in str.split(split) {
    writeln(s);
  }
}

writeln("Split 3");
{
  var split = compile("\\W+":t);
  var str = "...words, words...":t;
  for s in split.split(str) {
    writeln(s);
  }
  for s in str.split(split) {
    writeln(s);
  }
}

writeln("Split 4");
{
  var split = compile("(\\W+)":t);
  var str = "...words, words...":t;
  for s in split.split(str) {
    writeln(s);
  }
  for s in str.split(split) {
    writeln(s);
  }
}

writeln("Split 5");
{
  var split = compile("\\W+":t);
  var str = "Words, words, words.":t;
  for s in split.split(str, 1) {
    writeln(s);
  }
  for s in str.split(split, 1) {
    writeln(s);
  }
}

writeln("Split 6");
{
  var split = compile("\\W+":t);
  var str = "Words, words, words.":t;
  for s in split.split(str, 2) {
    writeln(s);
  }
  for s in str.split(split, 2) {
    writeln(s);
  }
}

writeln("sub 1");
{
  var re = compile("(w)(\\w+)":t);
  var str = "Words, words, word.":t;
  writeln(re.sub("\\1-\\2":t, str, true));
}

writeln("sub 2");
{
  var re = compile("(w)(\\w+)":t);
  var str = "Words, words, word.":t;
  writeln(re.subn("\\1-\\2":t, str, true));
}

writeln("sub 3");
{
  var re = compile("(w)(\\w+)":t);
  var str = "Words, words, word.":t;
  writeln(re.sub("\\1-\\2":t, str, false));
}
