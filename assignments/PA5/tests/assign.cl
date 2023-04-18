class Main {
   c:Int <- 3;
   d:Int <- 4;

   main(): Object {
    1
   };

   param_param(a:Int,b:Int):Int {
    a <- b
   };

   param_attr(a:Int):Int {
    a <- c
   };

   attr_attr():Int {
    c <- d
   };

   attr_param(a:Int):Int {
    c <- a
   };

   nested(a:Int) : Int {
    d <- (c <- (c <- a))
   };

   arith(a:Int) : Int {
    a <- a + 3 * (5 + a)
   };

   (* need tests for local variable *)
};
