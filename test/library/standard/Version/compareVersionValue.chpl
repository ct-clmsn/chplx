use Version;

const v2_1_0 = new versionValue(2,1);
const v2_1_1 = new versionValue(2,1,1);
const v2_2_0 = new versionValue(2,2,0);
const v2_2_1 = new versionValue(2,2,1);
const v3_1_0 = new versionValue(3,1);
const v3_1_1 = new versionValue(3,1,1);

const v2_1_0_p = new versionValue(2,1,0,"aaa");
const v2_1_1_p = new versionValue(2,1,1,"bbb");
const v2_2_0_p = new versionValue(2,2,commit="ccc");
const v2_2_1_p = new versionValue(2,2,1,"ddd");
const v3_1_0_p = new versionValue(3,1,0,"eee");
const v3_1_1_p = new versionValue(3,1,1,"fff");

compareVersions(v3_1_1, v3_1_1);
compareVersions(v3_1_1_p, v3_1_1_p);

compareVersions(v2_1_0, v2_1_0);
compareVersions(v2_1_0_p, v2_1_0_p);
compareLTVersions(v2_1_0, v2_1_0_p);
compareGTVersions(v2_1_0_p, v2_1_0);

compareBothVersions(v2_1_0, v2_1_1);
compareBothVersions(v2_1_0, v2_2_0);
compareBothVersions(v2_1_0, v2_2_1);
compareBothVersions(v2_1_0, v3_1_0);
compareBothVersions(v2_1_0, v3_1_1);

compareBothVersions(v2_1_0, v2_1_1_p);
compareBothVersions(v2_1_0, v2_2_0_p);
compareBothVersions(v2_1_0, v2_2_1_p);
compareBothVersions(v2_1_0, v3_1_0_p);
compareBothVersions(v2_1_0, v3_1_1_p);

compareBothVersions(v2_1_1, v2_2_0);
compareBothVersions(v2_1_1, v2_2_1);
compareBothVersions(v2_1_1, v3_1_0);
compareBothVersions(v2_1_1, v3_1_1);

compareBothVersions(v2_1_1, v2_2_0_p);
compareBothVersions(v2_1_1, v2_2_1_p);
compareBothVersions(v2_1_1, v3_1_0_p);
compareBothVersions(v2_1_1, v3_1_1_p);

compareBothVersions(v2_2_0, v2_2_1);
compareBothVersions(v2_2_0, v3_1_0);
compareBothVersions(v2_2_0, v3_1_1);

compareBothVersions(v2_2_0, v2_2_1_p);
compareBothVersions(v2_2_0, v3_1_0_p);
compareBothVersions(v2_2_0, v3_1_1_p);

compareBothVersions(v2_2_1, v3_1_0);
compareBothVersions(v2_2_1, v3_1_1);

compareBothVersions(v2_2_1, v3_1_0_p);
compareBothVersions(v2_2_1, v3_1_1_p);

compareBothVersions(v3_1_0, v3_1_1);

compareBothVersions(v3_1_0, v3_1_1_p);

proc compareBothVersions(v1, v2) {
  compareVersions(v1,v2);
  compareVersions(v2,v1);
}

proc compareVersions(v1, v2) {
  writeln("Comparing versions:");
  writeln(v1);
  writeln(v2);
  writeln("------------------");
  writeln("== : ", v1 == v2);
  writeln("!= : ", v1 != v2);
  writeln("<  : ", v1 < v2);
  writeln("<= : ", v1 <= v2);
  writeln(">  : ", v1 > v2);
  writeln(">= : ", v1 >= v2);
  writeln();
}

proc compareLTVersions(v1, v2) {
  writeln("Comparing versions:");
  writeln(v1);
  writeln(v2);
  writeln("------------------");
  writeln("== : ", v1 == v2);
  writeln("!= : ", v1 != v2);
  writeln("<  : ", v1 < v2);
  writeln("<= : ", v1 <= v2);
  writeln();
}

proc compareGTVersions(v1, v2) {
  writeln("Comparing versions:");
  writeln(v1);
  writeln(v2);
  writeln("------------------");
  writeln("== : ", v1 == v2);
  writeln("!= : ", v1 != v2);
  writeln(">  : ", v1 > v2);
  writeln(">= : ", v1 >= v2);
  writeln();
}

