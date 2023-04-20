class Main {
   main():Int {
      1
   };

   parameter1(a:Int): Int {
     2 + a
   };

   parameter2(a:Int): Int {
     a + 2
   };

   parameter3(a:Int): Int {
     a + a
   };

   nested() :Int {
      1 + (2 + (3 + (4 + (5 + (6 + (7 + (8 + 9)))))))
   };

   nested2(a:Int) :Int {
      1 + (2 + (a + (4 + (5 + (6 + (7 + (8 + 9)))))))
   };

   mix():Int {
      1 + 3 - 1 * 3
   };
};
