class Arith{
   add():Int {
      1 + (2 + 3)
   };
};

class Main inherits IO {
   main(): Object{
      (new Arith).add()
   };
};
