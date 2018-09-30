# TODO to improve TritonSizer code quality

## For better readability 
- Factor out the codes. There are many duplicated codes. Try to keep APIs to have
< 30 lines (so that it can be visible in one page)

- Align the column. Keep 80 column line-break rule.

   e.g.)

      s       = 1;
      xxxxxxx = 2;

      functionA(xxxxxxxxxxxxxxxx, yyyyyyyyyyyyyy, zzzzzzzzzzzzzzzzzzzzzzzzz,
               tt, uuuuuuuuuuuuu, vvvvvvvvvvvvvvvvvvvvvvvv, 
               wwwwwwwwwwwwwwwwwwww, kkkk);
             

- You can easily change your code-format by typing a below command

      $ clang-format -i XXX.cpp
      
- Make sure that use the setting in [src/.clang-format](src/.clang-format)

## Functionality 
- It would be better to support user-defined signoff tools. Currently PT, Tempus 
commands are hard-coded in TBC. 
- It would be better to improve UI / log messages. Current log messages might be too
verbose.

## Code structure / ETC 
- Encapsulate codes. Don't make everything public. Alway use static if possible.
- Try to avoid global variables
- Using assert is sometimes useful to debug.
