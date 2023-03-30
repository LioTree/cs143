class Main {
    test(x: Bool): A {
        if x then
            new C
        else
            new D
        fi
    };

   test2(x: Bool): A {
        if x then
            new C
        else
            new E
        fi
    }; 
};

class A {

};

class B inherits A{

};

class C inherits B{

};

class D inherits A {

};

class E{

};