class Main inherits IO {
   main(): Object {
      {
         out_int(let_init());
         out_string("\n");
         out_int(let_no_init());
         out_string("\n");
         out_int(let_no_init2());
         out_string("\n");
      }
   };

   let_init() : Int {
    let a:Int<-1,b:Int<-2 in a + b
   };

   let_no_init() : Int {
    let a:Int<-1,b:Int,c:A in a + b
   };

   let_no_init2() : Int {
    let a:Int<-1,b:Int<-2,c:Int<-3,d:Int<-4,e:Int<-5,f:Int<-6,g:Int,h:A in a + g
   };
};

class A{

};