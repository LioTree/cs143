class Main {
   o:A <- new B;
   main(): Object {
    {
    1 + 2;
    ~3;
    3-2;
    o.add(1,2);
    o@A.add(1,2);
    }
   };
};

class A {
   add(a:Int,b:Int):Int {
      a + b
   };
};

class B inherits A {
    add(a:Int,b:Int):Int {
      a + b + 1
   };
};