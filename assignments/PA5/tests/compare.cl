class Main inherits IO {

  main():Object {
    {
      if lt() then out_string("true") else out_string("false") fi;
      out_string("\n");
      if le() then out_string("true") else out_string("false") fi;
      out_string("\n");
      if eq() then out_string("true") else out_string("false") fi;
      out_string("\n");
    }
  };

  lt():Bool {
    1 < 2
  };

  le():Bool {
    1 <= 2
  };

  eq():Bool {
    1 = 2
  };

};

