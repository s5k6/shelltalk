
> FIXME references to lecture slides SQ and PK3

> FIXME Running `make` should compile all C programs used in the
  demonstration.

> FIXME explain the shell's current directory


Notation
========

It is a well established convention in the documentation of Linux/Unix
shell commands, to write `#` for the root user's prompt, and `$` for
the prompt of an unprivileged user, followed by the command to type
in.

    $ su
    Password:
    # yast2

Note, that typing `#` on the command line (or in a shell script)
introduces a comment until the end of the line.  The actual situation
should be clear from the context.

In this text, we exlusively work as unprivileged user.


Documentation
=============

The definitive source of information should be the manual pages
installed on your system.  You should find them in sync with installed
software, compensating for the diffrent flavours, distributions and
implementations of existing tools.

⇒ Rather use locally installed man-pages than the internet.

Manual Pages
------------

  * Manual pages (aka. man-pages) are organized in sections.

  * Different pages with the same name may exist in different
    sections.

  * Common notation to reference a named manual page in a particular
    section: name(section)

  * A list of sections is available in man(1).

  * Many sections have an intro page, e.g. intro(1).

Man-pages are organised in sections, and sometimes subsections.  A
manual is referenced by the name of the too/function, followed by the
section in parenthesis

    bash(1)

Manuals can be read using the man(1) utility.  If no section is
specified, the first matching manual will be shown.

    $ man man        # shows man(1) which is found before man(7)

    $ man 3 intro

    $ man printf

    $ man 3 printf


The Bourne-Again Shell
----------------------

This text is about the “GNU Bourne-Again Shell” `bash`.

Thus, use bash(1) for reference.  It actually is a good read.

To verify that you're using the `bash`, inspect the `$SHELL`
environment variable:

    $ echo $SHELL
    /bin/bash


Simple Commands
===============

The shell needs to interpret your input to find out what to do.  For
simple commands, the procedure is:

 1. The shell splits your input into words (⇒ what is a word?).

 2. The first word specifies the command to be executed (⇒ what is a command?).

 3. The remaining words are passed as arguments to the invoked
    command (⇒ how is it invoked?).

Example:

    $ echo hello world

The splitting into words can be quite tricky.  Avoid whitespace in
file names!

> In a beginner's course, this would be the place to introduce
> commands like `ls`, `pwd`, `cd`, `mkdir`, `rmdir`, `nano`, `rm`,
> `cp`, `mv`, `file`, `cat`, `less`.

FIXME: `type`, `help`, `man`


Looking at the C side of it
---------------------------

The program [arg.c](arg.c) simply prints the **argument vector**, one
command-line argument per line:

    $ ./arg hello world
    argv[0] = ./arg
    argv[1] = hello
    argv[2] = world

And using strace(1), we can observe how the program `arg` is actually
being executed:

    $ strace -e trace=%process ./arg hello world
    execve("./arg", ["./arg", "hello", "world"], 0x7ffc1c652f50 /* 45 vars */) = 0
    …

The execve(2) system call is called somehow (more on that later), and
the command line arguments are passed as its second argument:

    int execve(const char *pathname,
               char *const argv[],
               char *const envp[]
              );

By convention, `argv[0]` should contain the filename of the program,
but the binary executed is actually found using the first argument
`pathname`.


Globbing
--------

The shell expands wildcard characters when they appear on the command
line. This is called **globbing** in Unix jargon, and a pattern is
referred to as **glob** (in contrast to a regular expression, which is
another kind of pattern).

The character `*` matches any sequence of characters, `?` Matches any single
character, and `[`list`]` matches any single character in the list.

    $ ls *.c
    arg.c         closedup.c  dup2.c  fork.c  pid.c       sharedfd.c
    background.c  conflict.c  exec.c  open.c  redirect.c  write.c

**Note**: The expansion is done *by the shell*, not by the program
called:

    $ ./arg *.c
    argv[0] = ./arg
    argv[1] = arg.c
    argv[2] = background.c
    argv[3] = closedup.c
    …


### Handling failed matches

The shell's options `failglob` and `nullglob` control the shell's
behaviour in case pattern matching fails.  This may be an important
design choice when writing shell scripts.

Print your default settings:

    $ shopt failglob nullglob
    failglob        off
    nullglob        off

If `failglob` is set, the program will not be called if the match
fails:

    $ shopt -s failglob    # set failglob
    $ ./arg *.x
    bash: no match: *.x    # `arg` is not even called

Otherwise, `nullglob` decides whether a non-matching glob is passed to
the program, or not:

    $ shopt -u failglob nullglob
    $ ./arg *.x
    argv[0] = ./arg
    argv[1] = *.x          # unexpanded glob passed to program

    $ shopt -s nullglob
    $ ./arg *.x
    argv[0] = ./arg        # no arguments are created

Settings by `shopt` persist until changed again, or termination of the
shell.  Modify your `~/.bashrc` for the defaults you like.

  * In interactive shalls, I prefer to have `failglob` enabled by
    default: If the glob fails, I'd rather think about what to really
    pass to a program.

  * In a shell script, it may very well be adequate to deviate from
    failing.  It really **depends on the program invoked**.  You can
    change these options multiple times in a script, if req'd.


Quoting
-------

If spaces separate words, and globs are expanded to file names, then
how can one pass these **meta characters** as part of a command line
argument?

**Quoting** (in other contexts known as **escaping**) is a mechanism
to give some text a different meaning.  In the shell context, it means
reducing special characters to their **literal value**.

The shell's special **metacharacters** are space, tab, and

    ! " # $ & ' ( ) * ; < > ? [ \ ] ` { | } ~

The shell knows different quoting mechanisms:

  * An unquoted backslash `\` serves as the escape character, i.e., it
    **preserves the following character** (unless it is a newline, which
    is removed entirely):

        $ ./arg hello\ world
        argv[0] = ./arg
        argv[1] = hello world

        $ ./arg hello\
        world
        argv[0] = ./arg
        argv[1] = helloworld

        $ ./arg \t \n \\
        argv[0] = ./arg
        argv[1] = t
        argv[2] = n
        argv[3] = \           # the first `\` makes the second one literal

    Note, that an unquoted backslash does not introduce ANSI C
    escape sequences.

  * Single quotes `'·'` preserve the literal values of *all enclosed
    characters*, which is known as **strong quoting**.

        $ ./arg 'hello world'    # a quoted space does not separate words
        argv[0] = ./arg
        argv[1] = hello world

    Quoted characters loose all their special meaning.  ⇒ The
    backslash does not work here, so you cannot put a `'` inside
    single quotes!

        $ ./arg 'nothing \\ special'
        argv[0] = ./arg
        argv[1] = nothing \\ special

  * Double quotes `"·"` preserve the literal values of *most* of the
    enclosed characters.  That's why it's called **weak quoting**.

    Exceptions are `"`, `$`, `‘`, and the now quoted `\`.  If *history
    expansion* is enabled, `!` is special as well.

        $ ./arg "home is ${HOME}"
        argv[0] = ./arg
        argv[1] = home is /home/klinger

    Above, `$` is special and expands the environment variable
    `HOME`, but the spaces are literal and do not split the argument.

    The *quoted* backslash only quotes characters which are still
    special (and removes a newline)

        $ ./arg \' "\'" "\""
        argv[0] = ./arg
        argv[1] = '
        argv[2] = \'
        argv[3] = "

  * More exotic variations: Text of the form `$'…'` replaces
    `\`-escaped characters as known from ANSI C, e.g., `\n` and `\t`
    become newline and tab.  And `$"…"` is supposed to invoke
    translation according to the current locale.

        $ ./arg $'foo\tbar\nqux'
        argv[0] = ./arg
        argv[1] = foo   bar
        qux

Confused?  There's a simple mental model to quoting on the shell:

**
Think "quoting toggles", not "string literal delimiters"
**

In languages like Java, quotes delimit string literals, which are
somewhat atomic entities for the compiler.  Not so in the shell.

Think of `\`, `'` and `"` as **quoting toggles** that switch different
quoting modes on and off, with `\` affecting only the following
character.

With that model in mind, the following, completey valid examples
become easily understandable:

    $ ./arg hel'lo wor'ld
    argv[0] = ./arg
    argv[1] = hello world

Above, the first `'` switches strong quoting on (protecting the
space), the second one switches it off.

A less contrived example:

    $ x='I say "hello"'         # setting a shell variable

    $ ./arg '$x='"'$x'"
    argv[0] = ./arg
    argv[1] = $x='I say "hello"'

Strong quoting prevents the first `$x` from being expanded.  Weak
quoting allows the use of single quotes, and the second occurrence of
`$x` to expand, but does not split the result into words at the
spaces.

> Note: If you want to **print** correctly quoted strings, i.e., have
> the shell introduce quoting for further use, use `printf %q`, see
> bash(1).


Running a program
=================

What's happening when the shell runs a program?  I'll need some basics
for that, and thes apply to all program launches on Linux.


Processes and their ID
----------------------

Each process has a **Process ID**, and every process except `init`
(PID=1) has a parent process.

The tools ps(1) and top(1) can be used to inspect the process table.

    $ ps -e --forest -o pid,ppid,user,cmd
        PID    PPID USER     CMD
    …
      95357       1 klinger  xterm
      95362   95357 klinger   \_ bash
      95372   95362 klinger       \_ ps -e --forest -o pid,ppid,user,cmd
    …


> Note: The visible processes may be limited by the pid_namespaces(7)
> due to containerisation, but this is outside the scope of this text.

The shell provides variables to inspect its PID (`$$`) and its parents
PID (`$PPID`).  But be careful, `$$` may lie to you, see below.

    $ echo "PID $$, PPID $PPID"
    PID 95362, PPID 95357

A program can inspect its own and its parent's PID using the system
calls getpid(2) and getppid(2), see [pid.c](pid.c).

    $ ./pid
    $ ./pid

Both `pid`-processes have individual PIDs, and the shell's PID as PPID.


Execute: Replace one program with another
-----------------------------------------

This may come as a surprise: If you execute a program, then **by
replacing an existing program**.

The execve(2) system call (observed above) does not return on success.
It replaces the calling program by a new one.  Some of the execution
context will be modified during that process, some will be left
unchanged.

> Note: When programming C, there's a bunch of frontends to execve(2),
> summarised in exec(3).

Example [exec.c](exec.c)

    $ ./exec arg hello
    exec: PID 95742, about to run arg
    argv[0] = arg
    argv[1] = hello

    $ echo $$
    95362
    $ ./exec pid
    exec: PID 95889, about to run pid
    pid: PID 95889, PPID 95362

Note: `pid` sports the same PID as `exec`, so it has replaced it!

The shell provides an `exec` builtin:

    $ type exec
    $ help exec         # `help` works for builtin commands

    $ echo $$
    $ exec ./pid wait   # this ends the shell!

Use this to your advantage: When the shell script's last job is to
launch another program, use `exec` to get rid of the shell itself.


Fork a child process
--------------------

If running a program replaces an existing one, then how can there be
multiple programs?

The fork(2) system call duplicates a process, the new one being the
child of the caller.

Opposed to exeve(2), **fork(2) returns twice**, once in the parent
(returning the child's PID), once in the child (returning 0).

Example [fork.c](fork.c):

    $ echo $$
    96291

    $ ./fork
    fork: before, PID 96330, PPID 96291
    fork: after,  PID 96330, PPID 96291, pid 96331
    fork: after,  PID 96331, PPID 96330, pid 0

The first two lines are produced by the instance of `fork` that we've
called (PID 96330).  The last line is printed by the new child (PID
96331).

> Historically, fork(2) was a system call.  Today it's most likely a
> frontend to clone(2) which gives more control over what parts of the
> execution context should be shared between parent and child.  The
> line between child process and thread is blurred on Linux.  Also,
> strace(1) will report `clone`, not `fork`.  See posix_spawn(3) and
> vfork(2) for more alternatives.


Fork a child, then replace it
-----------------------------

Example [background.c](background.c):

    $ ./background /usr/bin/xterm
    background: PID 96503
    background: My child process has ID 96504

    $ ./background pid
    background: PID 96931
    background: My child process has ID 96932
    pid: PID 96932, PPID 1

If `background` terminates before `pid`, then `pid` becomes an orphan,
and is adopted by (reparented to) the `init` process, i.e., the one
with PID 1.


Further remarks
---------------

You can (and a shell typically does) wait(2) for a child process, see
[background2.c](background2.c).  If a child process dies, it becomes a
zombie until

  * its exit status is collected by the parent via wait(2), or

  * its parent dies and the zombie is adopted by `init` which
    implicitly performs the wait, a process called “reaping”, which is
    why `init` is also referred to as “reaper”.  There may be
    “subreapers”, see pid_namespaces(7).

Wait for your children lest they become zombies.

Under a long-living parent, zombies may amass and pollute the process
table (each consuming a PID that cannot be reused).  It was an
established technique to fight zombies with a double fork.

FIXME talk about subshells introduced by `(…)`


Standard streams
================

Each Unix process initially is connected to three data streams:

  * Standard input (*stdin*) to read data.  This is used to get your
    input, just like the shell does.

  * Standard output (*stdout*) to write data.  This is where a program
    prints its output, like ls does.

  * Standard error (*stderr*) to write error messages.  Also shows up
    in your terminal, interleaved with *stdout*.

E.g., cat(1) copies *stdin* to *stdout* if no arguments are given.
Pressing C-d (Ctrl+D, ^D) signals end of input.

    $ cat


Redirection
-----------

As example, the following command should print the listing of one
file to *stdout*, and an error message to *stderr*:

    $ ls -l README.md noSuchFile
    ls: cannot access 'noSuchFile': No such file or directory
    -rw------- 1 klinger klinger 20609 Jun 11 09:34 README.md

Use `1>` (or the shorthand `>`) to write *stdout* to a file:

    $ ls -l README.md noSuchFile 1> file1
    ls: cannot access 'noSuchFile': No such file or directory
    $ cat file1
    -rw------- 1 klinger klinger 22k Jun 11 10:26 README.md

Use `2>` to write *stderr* to a file:

    $ ls -l README.md noSuchFile 2> file2
    -rw------- 1 klinger klinger 22k Jun 11 10:26 README.md
    $ cat file2
    ls: cannot access 'noSuchFile': No such file or directory

Similarly, use `0<` (or the shorthand `<`) to read *stdin* from a
file:

    $ cat 0< arg.c
    …

What about using `42>`?

    $ ls -l README.md noSuchFile 42> file42
    $ ls -l file42

What do the numbers (0 for *stdin*, 1 for *stdout*, 2 for *stderr*, 42
for whatever) mean?


### Open a file by name

> open(2) is just a frontend for openat(2), but I'll stick to writing
> “open” for simplicity.

Usually, one would open(2) a file, before reading or writing, see
[open.c](open.c)

    $ strace -e trace=%file,write ./open
    …
    openat(AT_FDCWD, "output", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
    write(3, "Hello named file\n", 17)      = 17
    …

    $ cat output
    Hello named file

open(2) and similar system calls return a **file descriptor**, which
is an **integer** indexing the *per process* table of *open file
descriptions*, which can be observed in `/proc/${PID}/fd` and
`/proc/${PID}/fdinfo`.

Similar for reading, cat(1) opens the file passed as argument (yields
file descriptor 3), which is then read from:

    $ strace -e trace=%file,read cat arg.c
    …
    openat(AT_FDCWD, "arg.c", O_RDONLY)     = 3
    read(3, "#include <stdio.h>\n\nint main(int"..., 131072) = 154
    …


### Access files without opening?

Without an argument, cat(1) will not open a file, but instead read
from file descriptor 0.  We redirect this from a file:

    $ strace -e trace=%file,read cat 0< arg.c
    …
    read(0, "#include <stdio.h>\n\nint main(int"..., 131072) = 154
    …

This also works with write(2) in the other direction:
[write.c](write.c) writes to a file descriptor without opening it:

    $ ./write
    write: write to file descriptor 5 failed: Bad file descriptor

    $ ./write 5> file5
    $ cat file5
    play the kazoo

So somehow the file `file5` was opened for writing, and the file
descriptor 5 was made available to the newly run program.

How does the shell do that?  We would need to

  * control the file descriptor (0, 1, 2) returned by open(2), and

  * communicate an open file descriptor to a child process.


### Open file descriptors are shared on fork(2)

Example [sharedfd.c](sharedfd.c)

    $ ./sharedfd
    sharedfd: Writing to file: output
    $ cat output
    I am the parent
    I am the child

Note: Both processes share the write position in the file, which is
part of the open file description.

> Optional: Demonstrate a race condition by repeating each write 100
> times.


### Make sure a file is opened with file descriptor 2

We cannot do this, but we *can* reassign a file descriptor to another
one, using dup2(2), [dup2.c](dup2.c):

    $ ./dup2
    knock knock
    dup2: open("output", ...) = FD 3
    $ cat output
    who's there


### Summary redirection to/from files works

So redirection from/to files simply makes the shell open these files
in read/write mode, and assigns the expected file descriptors.

Use cat(1) redirecting *stdin* and *stdout* to copy a file:

    $ cat < arg.c > foo
    $ cmp arg.c foo

Let's investigate:

    $ strace -olog -f bash -c 'cat < arg.c > foo'

An excerpt from the `log` file, 8102 and 8103 are the PIDs of the
shell and its child:

    …
    8102  clone(…) = 8103
    8102  wait4(-1,  <unfinished ...>
    …

The shell (PID 8102) has created a child (PID 8103) and waits for a
status change with wait4(2).  The child is still running `bash`'s code
and is now setting file descriptors:

    …
    8103  openat(AT_FDCWD, "arg.c", O_RDONLY) = 3
    8103  dup2(3, 0)                        = 0
    8103  close(3)                          = 0
    …
    8103  openat(AT_FDCWD, "foo", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
    8103  dup2(3, 1)                        = 1
    8103  close(3)                          = 0
    …

Then the child replaces itself with cat(1), which does the copying:

    …
    8103  execve("/usr/bin/cat", ["cat"], 0x564891b95220 /* 45 vars */) = 0
    …
    8103  read(0, "#include <stdio.h>\n\nint main(int"..., 131072) = 154
    8103  write(1, "#include <stdio.h>\n\nint main(int"..., 154) = 154
    8103  read(0, "", 131072)               = 0
    …
    8103  close(0)                          = 0
    8103  close(1)                          = 0
    8103  exit_group(0)                     = ?
    …

The shell collects the child's exit code.

    …
    8102  <... wait4 resumed> … WEXITSTATUS(s) == 0 …) = 8103


Understanding this yields many more insights:


Bring in the harvest
--------------------

### Do not open files twice

It is generally wrong to redirect stderr and stdout to the same file:

    $ ls -l README.md /noSuchFile
    ls: cannot access '/noSuchFile': No such file or directory
    -rw------- 1 klinger klinger 25k Jun 11 12:46 README.md

    $ ls -l README.md /noSuchFile 1>file12 2>file12
    $ cat file12
    -rw------- 1 klinger klinger 25k Jun 11 12:46 README.md
    ry

And we can explain why: Both redirections, `1>` and `2>` make the
shell call open(2), so we'll have **two different** open file
descriptions, not sharing a common file position for writing:

    $ strace -olog -f bash -c 'ls -l README.md /noSuchFile 1>file12 2>file12'
    $ grep file12 log
    8571  openat(AT_FDCWD, "file12", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
    8571  openat(AT_FDCWD, "file12", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3

Instead, we'd like to reuse the file opened once for *stdout*, and
duplicate its file descriptor to be used for *stderr*.

This calls for dup2(2), and the shell provides Syntax for that:

    $ ls -l README.md /noSuchFile 1>file12 2>&1

where `2>&1` simply translates to `dup2(1, 2)`, or in words: “Redirect
FD 2 to the same thing, where FD 1 i pointing now.”


### Order of redirections

With this, we can understand why the order of redirections is
significant.  The shell just parses and applies the redirections from
left to right, starting from the assignment of *stdin* 0, *stdout* 1,
*stderr* 2.

![[Key Competence in Computer Science, Winter
 2015](https://stefan-klinger.de/files/sq_15w.pdf), page
 113](media/sq_15w_113.png)

Compare

        $ ls README.md /noSuchFile 1>file12 2>&1
        $ cat file12
        ls: cannot access '/noSuchFile': No such file or directory
        README.md

with

        $ ls README.md /noSuchFile 2>&1 1>file12
        ls: cannot access '/noSuchFile': No such file or directory
        $ cat file12
        README.md

and think about the order of the system calls `open`, `close` and
`dup2` involved.




____________________
FIXME CONTINUE




Run program with redirection to file
====================================

    fork a child process
    if child:
        open output file
        make 1 an alias for its file descriptor
        replace yourself with the program to run

Demo:

    $ e redirect.c

    $ ./redirect /usr/bin/ls -l arg.c mdlbrmft
    parent stdout still working
    /usr/bin/ls: cannot access 'mdlbrmft': No such file or directory

    $ cat output
    -rw------- 1 klinger klinger 154 Feb 17 17:29 arg.c


______________________________________________________________________
TODO

  * <> >> >| >&- &>

  * to $HOME or ${HOME}

  * bad:  path='/foo/bar/'; file="${path}qux"

  * explain order of redirections, and 2>&1

  * Subshells created by `… <(…)` or `$(…)`, compared to `{…}`, and
    not to be confused with creation of arrays `…=(…)`.

  * $BASHPID ./. $$

  * Maybe explain read(2), and why/when C-c or C-d is used to end a
    program.  C-c makes the terminal (FIXME make clear to be vague
    about “terminal”) send a signal SIGINT to its child process, while
    C-d makes it flush stdin.  Write your own `cat`.

  * The seven kinds of expansion: bash(1) /^EXPANSION

  * Difference between shell variables and environment variables.


Further Reading:

  * https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_09



Myths

  * Always write portable shell scripts.

    Systems with an incompatible shell are likely to feature
    incompatible tools as well (e.g., SLES 11 features a `sed`
    implementation which does not understand `-E`)

  * Always name scripts `foo.sh`

    It's an implementation detail, that unnecessarily fixes an
    interface.  The apt can find out, the inept should not care.


Types of commands
-----------------

The first word is the **command name**, and used by the shell to [find
the command][comex] to execute.

  * If command name contains at least on slash, then it is used as
    path to find an **executable file**, be it absolute or relative
    to the shell's current directroy.

        $ /bin/uname
        Linux

        $ ./hello
        Hello world

  * If the command name contains no slash, then the shell starts to
    search, in order, these places:

      - [Aliases][aliases] are repalced by their definition, before
        command lookup continues recursively.

      - [Shell functions][funcs] contain a series of commands to be
        executed by the shell (not by a new process, as would be the
        case for a shell script).

      - [Builtin commands][builtins] are part of the shell's
        functionality, and cannot be redefined by the user.

      - The PATH environment variable is searched for an executable
        file with the command name.

[aliases]: https://man7.org/linux/man-pages/man1/bash.1.html#ALIASES
[comex]: https://man7.org/linux/man-pages/man1/bash.1.html#COMMAND_EXECUTION
[funcs]: https://man7.org/linux/man-pages/man1/bash.1.html#FUNCTIONS
[builtins]: https://man7.org/linux/man-pages/man1/bash.1.html#SHELL_BUILTIN_COMMANDS