class Main inherits IO {
  a:Int <- 0;
  main():Object {
    while a < 5 loop {
      out_int(a);
      out_string("\n");
      a <- a + 1;
    }
    pool
  };
};