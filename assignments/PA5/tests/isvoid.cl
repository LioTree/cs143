class Main inherits IO {
  a:A;
  b:A <- new A;
  main():Object {
    {
      out_int(if isvoid_test() then 1 else 2 fi);
      out_string("\n");
      out_int(if isvoid_test2() then 1 else 2 fi);
      out_string("\n");
    }
  };

  isvoid_test():Bool {
    isvoid a
  };

  isvoid_test2():Bool {
    isvoid b
  };
};

class A {

};