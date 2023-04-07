class Animal inherits IO {
   name:Int;
   name2:Bool;
};

class Dog inherits Animal {
   name3:String;
   name4:Test;
};

class Test{

};

class Main inherits IO {
   dog:Dog <- new Dog;
   main(): Int {
    {1+2;}
   };
};
