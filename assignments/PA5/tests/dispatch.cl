class Main {
   o:A <- new A;
   main(): Object {
      o.add(1,2)
   };
};

class A {
   add(a:Int,b:Int):Int {
      a + b
   };
};