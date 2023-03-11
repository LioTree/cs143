class A {

ana(): Book {
    case xcar.print() of
                dummy : Book => out_string("- dynamic type was Book -\n");
                dummy : Article => out_string("- dynamic type was Article -\n");
            esac;
};
};