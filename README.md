---
title: Why the Shell…?
author: Stefan Klinger <https://stefan-klinger.de/>
---

This text is feeding on some Shell stuff taken from a [beginners
lecture](https://stefan-klinger.de/files/sq_15w.pdf), and Linux
Systems Programming stuff taken from a third semester [C
course](https://stefan-klinger.de/files/pk3_15w.pdf), both of which
I've had the joy to deliver repeatedly at U'KN.  (Unfortunately, the C
course degraded over the years, you'll find better ones).

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

Note, that `$` and `#` may be legitimate output of a previously run
command, and that typing `#` on the command line (or in a shell
script) introduces a *comment* until the end of the line.  The actual
situation, however, should be clear from the context.

In this text, we exlusively work as unprivileged user.

> IMHO: One should stick to that convention when writing
> documentation.  Many distributions and users define their own chic
> prompt, leading to much variation without benefit for the reader.
> In contrast, `$` and `#` form an established, simple code.


Finding Documentation
=====================

The definitive source of information should be the manual pages
installed on your system.  You should find them in sync with installed
software, compensating for the diffrent versions, flavours,
distributions and implementations of existing tools.

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
manual is referenced by the name of the tool / function, followed by
the section in parenthesis

    bash(1)

Manuals can be read using the man(1) utility.  If no section is
specified, the first matching manual will be shown.

    $ man man        # shows man(1) which is found before man(7)

    $ man 3 intro

    $ man printf

    $ man 3 printf


The Info system
---------------

Info is a hypertext system, with roots predating the WWW.

    $ info

Use the cursor keys to move around, (Shift)+Tab to jump to the next
(previous) hyperlink, Return to follow link, `p`/`n`/`u` to move to
the previous/next/parent node in the document tree, `/` to search for
a string, `q` to quit.

Try

    $ info info
    $ info bash

Compare these two:

    $ man 1 ls
    $ info '(coreutils) ls invocation'

you'll find a reference to the latter in section SEE ALSO at the very
end of the former.

> IMHO: Expect the man-page to provide all necessary information.  If
> Info documentation exists, then it is probably more extensive and
> worth visiting if you need more in-depth knowledge.  Expect all core
> utilities to have very good Info documentation, try `info
> coreutils`.


The Bourne-Again Shell
----------------------

This text is about the “GNU Bourne-Again Shell” `bash`, which claims
to be compatible to the POSIX standard shell sh(1).

Thus, use bash(1) for reference.  It actually is a good read.

To verify that you're using the `bash`, inspect the `$SHELL`
environment variable:

    $ echo $SHELL
    /bin/bash

At the time of writing, this is

    $ bash --version
    GNU bash, version 5.1.8(1)-release (x86_64-pc-linux-gnu)



Simple Commands
===============

A simple command is a sequence of blank-separated words and
redirections.  The first word specifies the command, the remaining
words are passed as arguments to the invoked command.

> Above is paraphrased from bash(1).  Search bash(1) for “Simple
> Commands” to get the complete picture.

 1. The shell splits your input into words (⇒ what is a word?).

 2. The first word specifies the command to be executed (⇒ what is a
    command?).

 3. The remaining words are passed as arguments to the invoked command
    (⇒ how is it invoked?).

Example:

    $ echo hello world

The splitting into words can be quite tricky.  Avoid whitespace in
file names!

> In a beginner's course, this would be the place to introduce
> commands like
>
>  * Looking and navigating around with `ls`, `pwd`, `cd`, `mkdir`,
>    `rmdir`
>
>  * Investigating and editing files with `file`, `cat`, `less`,
>    `nano`.  Note that `less` is `man`'s pager.
>
>  * Managing files with `cp`, `mv`, `rm`, `touch`


Finding the command
-------------------

The shell **searches** for the command to execute:

  * It the first word contains a **slash**, then it is assumed a path
    to an executable file,

  * otherwise, the shell looks into the following dictionaries, in
    this order, trying to resolve the command name:

     1. an **alias** is a simple abbreviation for a single command

     2. a **function** may contain multiple commands, and be recursive,

     3. a **builtin** exposes a feature of the shell to the command line.

  * if that's still not successful, search the directories in the
    `PATH` environment variable for an executable file of that name.

    > Note: The shell may cache the result of a path search.  So if
    > you create a script to shadow an existing binary, be sure to
    > make it found first, and use the `hash` builtin to enforce a new
    > path search.

For even more details, see bash(1), “command execution”.  Also, shell
builtins, and defining aliases and functions, have their own sections
in the manual.

In some sense (will become clear later on), functions, builtins, and
“real programs” do behave the same way.  We'll discuss some
differences in this text.  The first is this one:


### Finding documentation, again

Use the `type` builtin to figure out how the shell interprets the
command.  Examples:

    $ type ls
    ls is aliased to `ls $LS_OPTIONS'

    $ type cd
    cd is a shell builtin

    $ type vi
    vi is /usr/bin/vi

Shell builtins are part of the shell, and documented in bash(1).  The
`help` builtin provides a short overview:

    $ help pwd
    pwd: pwd [-LP]
        Print the current working directory.  With the -P option, pwd prints
        the physical directory, without any symbolic links; the -L option
        makes pwd follow symbolic links.

The available aliases and functions depend on the distribution and
administrator used.  Try `declare -a` and `declare -f` to list them.
Do not expect documentation.

* * *

Back to running simple commands: Assuming the Input has been split
into words, i.e., into command and arguments, the command has been
identified as an executable file.  How is it run?


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

The third argument is “the environment”, more on that later (FIXME I
hope).


Globbing
--------

The shell expands wildcard characters when they appear on the command
line. This is called **globbing** in Unix jargon (but bash(1) says
**pathname expansion** instead), and a pattern is referred to as
**glob** (in contrast to a regular expression, which is another kind
of pattern).

The character `*` matches any sequence of characters, `?` Matches any single
character, and `[`*list*`]` matches any single character in the *list*.

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
shell.

  * In interactive shalls, I prefer to have `failglob` enabled by
    default: If the glob fails, I'd rather think about what to really
    pass to a program.  Modify your `~/.bashrc` to taste.

  * In a shell script, it may very well be adequate to deviate from
    failing.  It really **depends on the program invoked**.  You can
    change these options multiple times in a script, if req'd.


Quoting
-------

If spaces separate words, and globs are expanded to file names, then
how can one pass these **meta characters** as part of a command line
argument?

Now consider this:

    $ ls -l
    total 8
    -rw------- 1 klinger klinger  0 Jun 14 16:54 *
    -rw------- 1 klinger klinger 33 Jun 14 16:52 hello
    -rw------- 1 klinger klinger  0 Jun 14 16:53 hello world
    -rw------- 1 klinger klinger 33 Jun 14 16:52 world

How can one delete the empty files?

    $ rm *                 # nope
    $ rm hello world       # nope

**Quoting** (aka. **escaping**) is a mechanism to give some text a
different meaning.  In the shell context, it means reducing special
characters to their **literal value**.

The shell's special **metacharacters** are space, tab, and

    " # $ & ' ( ) * ; < > ? [ \ ] ` { | } ~

> Note: If *history expansion* is enabled, then `!` is special as
> well, but escaping it is inconsistent inside double quotes.  If this
> bugs you, disable history expansion with `set +H` in your
> `~/.bashrc`.


### The shell knows different quoting mechanisms

  * An unquoted backslash `\` serves as the **escape character**,
    i.e., it preserves the following character (unless it is a
    newline, which is removed entirely):

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

    Exceptions are `"`, `$`, `‘`, and the *now quoted* `\`.

        $ ./arg "home is ${HOME}"
        argv[0] = ./arg
        argv[1] = home is /home/klinger

    Above, `$` is special and expands the environment variable
    `HOME`, but the spaces are literal and do not split the argument.

    The *quoted* backslash only quotes characters which are still
    special in a weakly quoted context (plus, it removes a newline)

        $ ./arg   \' \\   '\'   "\"" "\$x" "\\"   "\'"
        argv[0] = ./arg
        argv[1] = '        # escaped single quote
        argv[2] = \        # escaped backslash
        argv[3] = \        # strongly quoted backslash
        argv[4] = "        # weakly quoted escaped double quote
        argv[5] = $x       # weakly quoted escaped dollar and an x
        argv[6] = \        # weakly quoted escaped backslash
        argv[7] = \'       # weakly quoted backslash and a single quote

  * More exotic variations: Text of the form `$'…'` replaces
    `\`-escaped characters as known from ANSI C, e.g., `\n` and `\t`
    become newline and tab.  And `$"…"` is supposed to invoke
    translation according to the current locale.

        $ ./arg $'foo\tbar\nqux\'tux'
        argv[0] = ./arg
        argv[1] = foo   bar
        qux'tux

Confused?  There's a simple mental model to quoting on the shell:


### Not “string literals” but “quoting toggles”

Think of backslash `\`, single quote `'` and double quote `"` as
**quoting toggles** that switch different quoting modes on and off,
with `\` affecting only the following character.

In languages like Java, quotes delimit string literals, which are
somewhat atomic entities for the compiler.  Not so in the shell.

Think of the shell as parsing your input left-to-right, starinig in
unquoted mode.  Then:

  * An unquoted…

      - …backslash `\` escapes the next character, or deletes a
        directly following newline,

      - …single quote `'` switches on strong quoting,

      - …double quote `"` switches on weak quoting.

  * A strongly quoted single quote `'` ends strong quoting.

  * A weakly quoted…

      - …double quote `"` ends weak quoting.

      - …backslash `\` removes any special meaning from a following
        `"`, `$`, `‘`, or `\`.

This simplifies writing a parser to a pretty straight forward state
machine, and also allows to **switch quoting style** on the fly, as
needed:

  * Unusual, but proves the point: The first `'` switches strong
    quoting on (protecting the space), the second one switches it off.

        $ ./arg hel'lo wor'ld
        argv[0] = ./arg
        argv[1] = hello world

  * A single quote with strongly quoted text:

        $ ./arg 'it'\''s time'
        argv[0] = ./arg
        argv[1] = it's time

  * An even less contrived example:

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


The seven kinds of expansion
----------------------------

*After* splitting the input into words (at blanks, i.e., space and
tab), the shell performs **seven kinds of expansion**, in this order:

 1. Brace expansion.

 2. Performed left-to right:

      * Tilde expansion — e.g., `~` → your home directory.

      * Parameter and variable expansion — e.g., `$HOME` → your home directory.

      * Arithmetic expansion.

      * Command substitution.

      * Process substitution.

 4. Word splitting — again!

 5. Pathname expansion — this is globbing.

Then, unquoted quotes are removed.


Shell scripts
=============

A shell script is an executable plain text file (cf. chmod(1)),
starting with the shebang (aka. **hashbang**) indicating where to find
the interpreter.

    #!/bin/bash

The exact mechanics and limitations are explained in execve(2).


Environment variables
---------------------

FIXME move this to past execve

Every Linux processes can access a per-process list of environment
variables, cf. environ(7).

  * By convention, these are key/value pairs, both entries being
    0-terminated strings.

  * Can be investigated in `/proc/$$/environ` (use a hexdump tool).

  * Typically (but *not* necessarily), the environment is handed down
    form a process to all programs it calls (cf. below).


### Shell variables

> Read `info '(bash)Environment'` and `info '(bash)Shell Parameters'`
> and for full details.

Shell variables are not environment variables!

  * When the shell starts, it creates shell variables from all
    environment variables.  Thus, we can read the “environment
    variable” `HOME` using

        $ echo $HOME
        /home/sk

  * Modifying shell variables **does not modify the environment** of
    the shell process (although the manual says otherwise), but only
    the shell's variables.

  * Only shell variables that have been **marked for export** (e.g.,
    using the `export` or `declare` builtin) will be part of the
    environment of a new process.

  * All variables imported from the environment at startup are
    implicitly marked for export.

  * The export property can be removed with `export -n`.


Advice
------

  * Use available tools.  The shell is a **tool-combiner**.

    Learn sed(1), find(1), ssh(1), tar(1), ….

  * Shell scripts do have **no suffix**.

    Using a suffix like `.sh` would be the surfacing of an
    implementation detail.  User experience degrades.  On replacing
    the script by another implementation, users (human and mechanic)
    have to be informed and adapt.  The apt user can find out whether
    a file is a script (and what kind thereof), and the rest should
    not care.

  * Use **lowercase variable names** in your scripts.

    This is a poor man's namespace segregation: Though there's no
    technical need, *environment variables* tend to be uppercase-only.
    So lowercase variables decrease your chance to overwrite one
    ([example](https://pubs.opengroup.org/onlinepubs/9699919799/xrat/V4_xbd_chap08.html)).

  * The **shebang** is `#!/bin/bash`, unless your OS *dictates* otherwise.

    Because the [Linux Filesystem Hierarchy Standard
    (FHS)](https://refspecs.linuxfoundation.org/fhs.shtml) puts
    “Essential user command binaries” in `/bin`, amongst which are
    `/bin/sh` and `/bin/csh`.

    The use of `#!/usr/bin/env bash` is a silly custom, originating in
    the early days of Python, the community of which was plagued by
    different ideas about where to put the interpreter.  Also, it
    brings in `$PATH`, probably against your aim for portability.

  * Always aiming for **portability is a misguided objective**.

    Shell scripts typically utilise avaliable system tools,
    e.g. sed(1), which already differs between unixoid systems (`-E`
    versus `-r` for extended regular expressions).

    If you do need portability, decide on the standards you commit to
    (SUS, POSIX, FHS, …?), limit your use of system tools to the
    standardised, and rethink whether the effort is warranted.  Then
    the shebang is `#!/bin/sh`.

  * Rather **fail** catastrophically and fast, than subtly and late.

  * A **template** sufficient for 99% of your bash scripts:

        #!/bin/bash
        set -u -e -C
        shopt -s failglob

    Then, deviate as needed, e.g., adjusting `failglob` and `nullglob`
    mid-script for a tool that needs this.

  * A **stub** for your `~/.bashrc`:

        set -u +H -C
        shopt -s failglob globstar
        shopt -u progcomp    # progcomp is buggy with failglob


Running a program
=================

What's happening when the shell runs a program?

I'll need some basics for that, and these apply to all program
launches on Linux.  ⇒ Well worth the time!


Processes and their ID
----------------------

Each process has a **Process ID**, and every process except `init`
(PID=1) has one parent process.

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
    PID 10566, PPID 10561
    $ echo "PID $$, PPID $PPID"
    PID 10566, PPID 10561          # same result, `echo` is a shell builtin

A program can inspect its own and its parent's PID using the system
calls getpid(2) and getppid(2), see [pid.c](pid.c).

    $ ./pid
    pid: PID 11788, PPID 10566
    $ ./pid
    pid: PID 11790, PPID 10566     # new invocation, new process, new PID

The two invocations of `pid` return different PIDs, coming from
separately created processes running (instances of) the same program.
They have the shell's PID as PPID.


Execute: Replace one program with another
-----------------------------------------

If you execute a program, then **by replacing the running program** —
which may sound odd.

The execve(2) system call (also observed above) **does not return** on
success.  It replaces the calling program by a new one.  Some of the
execution context will be modified during that process, some will be
left unchanged.  Details in the manual.

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

> Also, `exec` can modify the shell's own redirections and file
> descriptors — we'll come back to that…


Fork a child process
--------------------

If running a program replaces an existing one, then how can there be
multiple programs?

The fork(2) system call duplicates a process, the new one being the
child of the caller.

In contrast to exeve(2), **fork(2) returns twice** on success, once in
the parent (returning the child's PID), once in the child (returning
0).

Example [fork.c](fork.c):

    $ echo $$
    10566

    $ ./fork
    fork: before, PID 12133, PPID 10566
    fork: after,  PID 12133, PPID 10566, fork() returned 12134
    fork: after,  PID 12134, PPID 12133, fork() returned 0

The first two lines are produced by the instance of `fork` that we've
called (PID 12133).  The last line is printed by the new child (PID
12134).

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

If `background` terminates before `pid`, then `pid` becomes an
**orphan**, and is adopted by (i.e., reparented to) the `init`
process, i.e., the one with PID 1.


### What the shell does

Well precisely that, if you run a command terminated by the
ampersand `&` **control operator** instead of a semicolon `;` — the
latter of which may frequently be omitted.

Without the ampersand `&`, the shell immediately issues one of the
wait(2) system calls to wait for, and collect a child process' status
change, see [wait.c](wait.c).

    $ ./wait ./pid wait
    wait: PID 2939
    wait: My child process has ID 2940
    pid: PID 2940, PPID 2939
    wait: Child 2940 has returned 0

> It is instructive to investigate the `log` file created by
>
>     $ strace -e trace=%process -olog -f bash -c 'sleep 3& date'
>
> Note: The shell will not terminat right away, because it waits for
> its children before termination.  But you'll see the date(1) printed
> immediately, proving the sleep(1) to run in the background of the
> shell.

> Note: wait(2) may actually be a frontend to wait4(2), but I’ll stick
> to writing “wait” for simplicity.


If a child process dies, it becomes a **zombie** until…

  * …its exit status is collected by the parent via wait(2) & Co., or

  * …its parent dies and the zombie is adopted by `init` which
    implicitly performs the wait, a process called “reaping”, which is
    why `init` is also referred to as **reaper**.  There may be
    **sub-reapers**, see pid_namespaces(7).

The OS must maintain the exit status of a child for retrieval by its
parent (while the reaper don't care).  Thus, under a long-living
parent, zombies may amass and pollute the process table.

⇒ Wait for your children lest they become zombies.

It was an established technique to fight zombies with a double fork.
Alternatively, explicitly ignore SIGCHLD.  The shell reacts to SIGCHLD
to reap zombies as soon as possible, and reports before printing the
next prompt:

    $ sleep 1 &
    $                 # wait more than 1 second, then hit Return
    [1]+  Done                    sleep 1
    $



FIXME talk about subshells introduced by `(…)`

FIXME talk about `jobs` and `wait` shell builtins


Standard streams
================

Each Unix process initially is connected to three data streams:

  * Standard input (*stdin*) to read data.  This is used to get your
    input, just like the shell does.

  * Standard output (*stdout*) to write data.  This is where a program
    prints its output, like ls(1) does.

  * Standard error (*stderr*) to write error messages.  Also shows up
    in your terminal, interleaved with *stdout*.

E.g., cat(1) copies *stdin* to *stdout* if no arguments are given.
Pressing C-d (Ctrl+D, ^D) signals end of input.

    $ cat
    dog
    dog

> The fact that I/O appears line by line is a result of buffering in
> the *line discipline* of terminal driver, just *slightly* out of
> scope here… ;-)



Redirection
-----------

As example, the following command should print the listing of one
file to *stdout*, and an error message to *stderr*:

    $ ls README.md noSuchFile
    ls: cannot access 'noSuchFile': No such file or directory
    README.md

Use `1>` (or the shorthand `>`) to write *stdout* to a file:

    $ ls README.md noSuchFile 1> file1
    ls: cannot access 'noSuchFile': No such file or directory

    $ cat file1
    README.md

**Note:** Redirection syntax does not appear as argument to the
invoked command:

    $ ./arg hello 1> file1 world
    $ cat file1
    argv[0] = ./arg
    argv[1] = hello
    argv[2] = world

Use `2>` to write *stderr* to a file:

    $ ls README.md noSuchFile 2> file2
    README.md

    $ cat file2
    ls: cannot access 'noSuchFile': No such file or directory

Similarly, use `0<` (or the shorthand `<`) to read *stdin* from a
file:

    $ cat 0< lorem.txt
    Lorem ipsum dolor sit amet, consectetur adipiscing elit

What about using `42>`?

    $ ls README.md noSuchFile 42> file42
    $ ls -l file42

What do the numbers (0 for *stdin*, 1 for *stdout*, 2 for *stderr*, 42
for whatever) mean?


### Open a file by name

> open(2) may actually use openat(2), but I'll stick to writing “open”
> for simplicity.

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

    $ strace -e trace=%file,read cat lorem.txt
    …
    openat(AT_FDCWD, "lorem.txt", O_RDONLY) = 3
    read(3, "Lorem ipsum dolor sit amet, cons"..., 131072) = 56
    read(3, "", 131072)                     = 0
    …


### Access files without opening?

Without an argument, cat(1) will not open a file, but instead read
from file descriptor 0.  We redirect this from a file:

    $ strace -e trace=%file,read cat 0< lorem.txt
    …
    read(0, "Lorem ipsum dolor sit amet, cons"..., 131072) = 56
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

How does the shell do that?  We need to

  * control the file descriptor (0, 1, 2) returned by open(2), and

  * communicate an open file descriptor to a child process.


### Open file descriptors are shared on fork(2)

Example [sharedfd.c](sharedfd.c)

    $ ./sharedfd
    sharedfd: Writing to file: output
    $ cat output
    I am the parent
    I am the child

Note: Both processes share the write offset in the file, which is part
of the open file description.

Optional: Demonstrate a race condition by repeating each write 100
times.


> Historical note: This was not the case in Linux before 3.14, see the
> “Bugs” sections in read(2) and write(2).


### Make sure a file is opened with file descriptor 2

We cannot do this, but we *can* assign an open file descript**ion**
(referenced by its file descript**or**) to another file
descript**or**, using dup2(2), [dup2.c](dup2.c):

    $ ./dup2
    knock knock
    dup2: open("output", ...) = FD 3
    $ cat output
    who's there


### Summary redirection to/from files works

So redirection from/to files simply makes the shell open these files
in read/write mode, and assigns the expected file descriptors.

Use cat(1) redirecting *stdin* and *stdout* to copy a file:

    $ cat < lorem.txt > foo
    $ cat foo
    Lorem ipsum dolor sit amet, consectetur adipiscing elit

Let's investigate:

    $ strace -olog -f -etrace=%process,openat,read,write,dup2,close \
      bash -c 'cat < lorem.txt > foo'

An excerpt from the `log` file, 35958 and 35959 are the PIDs of the
shell and its child:

    …
    35958 clone(…) = 35959
    35958 wait4(-1,  <unfinished ...>
    …

The shell (PID 35958) has created a child (PID 35959) and waits for a
status change with wait4(2).  The child is still running `bash`'s code
and is now setting file descriptors:

    …
    35959 openat(AT_FDCWD, "lorem.txt", O_RDONLY) = 3
    35959 dup2(3, 0)                        = 0
    35959 close(3)                          = 0
    35959 openat(AT_FDCWD, "foo", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
    35959 dup2(3, 1)                        = 1
    35959 close(3)                          = 0
    …

Then the child replaces itself with cat(1), which does the copying:

    35959 execve("/usr/bin/cat", ["cat"], 0x558ca29a6220 /* 45 vars */) = 0
    …
    35959 read(0, "Lorem ipsum dolor sit amet, cons"..., 131072) = 56
    35959 write(1, "Lorem ipsum dolor sit amet, cons"..., 56) = 56
    35959 read(0, "", 131072)               = 0
    …

The shell collects the child's exit code.

    …
    35958 <... wait4 resumed>…, 0, NULL) = 35959

* * *

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
descript**ions**, not sharing a common file offset for writing:

    $ strace -olog -f bash -c 'ls -l README.md /noSuchFile 1>file12 2>file12'
    $ grep file12 log
    8571  openat(AT_FDCWD, "file12", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
    8571  openat(AT_FDCWD, "file12", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3

Instead, we'd like to reuse the file opened once for *stdout*, and
duplicate its file descriptor to be used for *stderr*.

This calls for dup2(2), and the shell provides Syntax for that:

    $ ls -l README.md /noSuchFile 1>file12 2>&1

where `2>&1` simply translates to `dup2(1, 2)`, or in words: “Redirect
FD 2 to where FD 1 is pointing now.”


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


### Swapping *stdout* and *stderr*

    $ foo 3>&1 1>&2 2>&3


Pipelines
=========

A **pipeline** is a sequence of one or more *commands* separated by
the pipe `|` control operator.  (paraphrased from bash(1))

> Above is paraphrased from bash(1).  Search bash(1) for “SHELL
> GRAMMAR” to get the complete picture.

______________________________________________________________________
TODO

  * <> >> >| shorthands

  * closing file descriptors, or using exec for the shell's redirections

  * `$@`, `$*`, `( "$@" )`

  * to $HOME or ${HOME}

  * bad:  path='/foo/bar/'; file="${path}qux"

  * `namei`, `tee`, `xargs`

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
