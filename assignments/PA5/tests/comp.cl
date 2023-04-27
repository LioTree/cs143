class Main inherits IO {

  main():Object {
    {
      out_int(if comp() then 1 else 2 fi);
      out_string("\n");
    }
  };

  comp():Bool {
    not 1 < 2
  };
};

