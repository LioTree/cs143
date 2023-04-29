class Main inherits IO {
  a:Bool;
  main():Object {
    {
      out_int(cond());
      out_string("\n");
      out_int(cond_nested());
      out_string("\n");
      cond_bool();
      out_string("\n");
    }
  };

  cond():Int {
    if true then 1 else 2 fi
  };

  cond_nested():Int {
    if (if true then true else false fi) then 1 else 2 fi
  };

  cond_bool():Object {
    if a then
      out_string("true")
    else
      out_string("false")
    fi 
  };
};

