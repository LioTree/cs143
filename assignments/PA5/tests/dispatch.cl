class Main {
   o:A <- new B;
   main(): Object {
    1
   };

   dispatch():Object {
      o.add(1,2)
   };

   static_dispatch():Object {
    o@A.add(1,2)
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