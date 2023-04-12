class Arith{
   add():Int {
      let a:Int in a <- 1
   };
};

class Main inherits IO {
   main(): Object{
      let x:Int <- (new Arith).add() in out_int(x)
   };
};
