class Animal inherits IO {
   name:Int <- 3;
   name2:Bool <- true;
};

class Dog inherits Animal {
   name3:String <- "aaaa";
   name4:Test;
};

class Test{

};

class Main inherits IO {
   dog:Animal <- new Dog;
   main(): Int {
    {1+2;}
   };
};
