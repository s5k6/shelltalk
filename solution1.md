Redirection order

    $ ls README.md /noSuchFile 1>file12 2>&1

yields

    fd = open("file12", …); dup2(fd, 1); close(fd);   // 1>file12

    dup2(1, 2);    // 2>&1

producing 1,2 → `file12`.  And redirection order

    $ ls README.md /noSuchFile 2>&1 1>file12

yields exactly the same two lines, in reverse order

    dup2(1, 2);    // 2>&1

    fd = open("file12", …); dup2(fd, 1); close(fd);   // 1>file12

producing 2 → *stdout*, 1 → `file12`.
