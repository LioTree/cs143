class Main inherits IO {
   c:Int <- 3;
   d:Int <- 4;

   main(): Object {
      {
         out_int(param_param(1,2));
         out_string("\n");
         out_int(param_attr(3));
         out_string("\n");
         out_int(attr_attr());
         out_string("\n");
         out_int(attr_param(4));
         out_string("\n");
         out_int(nested(5));
         out_string("\n");
         out_int(arith(6));
         out_string("\n");
      }
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
