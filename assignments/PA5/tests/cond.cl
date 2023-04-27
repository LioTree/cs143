class Main inherits IO {

  main():Object {
    {
      out_int(cond());
      out_string("\n");
      out_int(cond_nested());
    }
  };

  cond():Int {
    if true then 1 else 2 fi
  };

  cond_nested():Int {
    if (if true then true else false fi) then 1 else 2 fi
  };
};

