class A {
    test():Int {
        1
    };
};

class B inherits A {
    test():Int {
        2
    };
};

class Main {
    main() : Object {
        (new B).test()
    };
};