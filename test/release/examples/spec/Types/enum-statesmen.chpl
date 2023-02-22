enum statesman { Aristotle, Roosevelt, Churchill, Kissinger }
proc quote(s: statesman) {
  select s {
    when statesman.Aristotle do
       writeln("All paid jobs absorb and degrade the mind.");
    when statesman.Roosevelt do
       writeln("Every reform movement has a lunatic fringe.");
    when statesman.Churchill do
       writeln("A joke is a very serious thing.");
    when statesman.Kissinger do
       { write("No one will ever win the battle of the sexes; ");
         writeln("there's too much fraternizing with the enemy."); }
  }
}
for s in statesman do
  quote(s:statesman);
