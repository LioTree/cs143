class A {
a:Int <- 123;
b:Int;
ana(c:Int): Int {
    {
        b<-a+123;
        c<-b-111;
    }
};
};

class B inherits A {
};