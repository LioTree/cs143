class Main inherits IO {
   b:Int <- 3;
   main(): Object {
    {
      out_int(intconst());
      out_int(parameter(123));
      out_int(attribute());
    }
   };

   intconst():Int {
    ~~~~~1 
   };

   parameter(a:Int):Int {
    ~~~~a
   };

   attribute():Int {
    ~~~b
   };
};
