---
title: Why the Shell…?
subtitle: Why the shell does what it does, and how so.
author: Stefan Klinger <https://stefan-klinger.de/>
---

Linux Shell Programming may feel revolting.  If something like
“ordinary programming” existed at all, then Shell Programming would
not be it.  But why is it so awkward?  After some basics, I'll try to
show how direct an interface to the Linux OS the shell is, and why
some of its awkwardness is the result of a quite elegant design.

You should have a little shell and C experience to enjoy this.

This is not a shell scripting course.  There are better and more
extensive texts, and my scripting is far from perfect.  **Do have a
look** at these:

  * [The Grymoire][1]

  * [Bash Pitfalls][2]

> This text is feeding on some Shell stuff taken from a [beginners
> lecture][4a], and some Linux Systems Programming stuff taken from a
> third semester [C course][4b], both of which I've had the joy to
> deliver repeatedly at U'KN.  (Unfortunately, for…  *administrative*
> reasons, the C course degraded over the years, and I haven't found
> the time to unearth the better versions.  You'll find better ones).


### Warning — Do not use the examples

The provided example code files are intended as **minimal working
examples**, and constitute **no best practices** (e.g, not using
`O_CLOEXEC` where they should, leaving unused file desctiptors open,
not checking for errors properly, *etc.* …).

⇒ Do not use the examples as basis for your own projects, but for
experiments and to foster your understanding.  Do read the documentation!

> I claim no originality in the examples.  They are trivial and
> obvious applications of what everyone can find in the relevant
> documents.


### Notation

It is a well established convention in the documentation of Linux/Unix
shell commands, to write `#` for the root user's prompt, and `$` for
the prompt of an unprivileged user, followed by the command to type
in.

Note, that `$` and `#` may as well be legitimate output of a
previously run command, and that typing `#` on the command line (or in
a shell script) introduces a *comment* until the end of the line.  The
actual situation, however, should be clear from the context.

    $ su
    Password:      # type your password here
    # yast2

In this text, we exlusively work as unprivileged user.

> IMHO: One should stick to that convention when writing
> documentation.  Many distributions and users define their own chic
> prompt, but in documentation this leads to much variation without
> benefit for the reader.  In contrast, `$` and `#` form a simple and
> ubiquitous code.



> FIXME Running `make` should compile all C programs used in the
  demonstration.

> FIXME explain the shell's current directory


Finding Documentation
=====================

The definitive source of information should be the manual pages
installed on your system.  You should find them in sync with installed
software, compensating for the diffrent versions, flavours,
distributions and implementations of existing tools.

Use the internet for ideas, but verify them against locally installed
docs.


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
> utilities to have *very good* Info documentation, try `info
> coreutils`.


The Bourne-Again Shell
----------------------

This text uses the “GNU Bourne-Again Shell” `bash`, which claims to be
compatible to the POSIX standard shell sh(1).

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

  * If the first word contains a **slash**, then it is assumed a path
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

Back to running simple commands: Assuming the input has been split
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
    arg.c         closedup.c  dup2.c  fork.c  pid.c       fdfork.c
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

Print your default settings, mine are:

    $ shopt failglob nullglob
    failglob        off
    nullglob        off

If `failglob` is off (unset), then `nullglob` decides whether a
non-matching glob is passed to the program, or not:

    $ shopt -u failglob nullglob   # switch both off
    $ ./arg *.x
    argv[0] = ./arg
    argv[1] = *.x          # unexpanded glob passed to program

    $ shopt -s nullglob    # enable nullglob
    $ ./arg *.x
    argv[0] = ./arg        # no arguments are created

Hence, with `nullglob`, a failed match creates no arguments.

However, if `failglob` is set, then the program will not be called
with a failed match.  The setting of nullglob becomes irrelevant:

    $ shopt -s failglob    # set failglob
    $ ./arg *.x
    bash: no match: *.x    # `arg` is not even called

Settings by `shopt` persist until changed again, or termination of the
shell.  Modify your `~/.bashrc` to taste.

In interactive shells, I prefer to have `failglob` enabled by default:
If the glob fails, I'd rather think about what to really pass to a
program.

In a shell script, it really depends on the situation.  It may very
well be legitimate to have a failing glob create an empty list, or to
yield an error.  You can change these options multiple times in a
script, I tend to start with `failglob` enabled, and adapt mid-script
if req'd.


Quoting
-------

If spaces separate words, and globs are expanded to file names, then
how can one pass these **metacharacters** as part of a command line
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

  * Single quotes `'…'` preserve the literal values of *all enclosed
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

  * Double quotes `"…"` preserve the literal values of *most* of the
    enclosed characters.  That's why it's called **weak quoting**.

    Exceptions are `"`, `$`, `` ` ``, and the *now quoted* `\`.

        $ ./arg "home is ${HOME}"
        argv[0] = ./arg
        argv[1] = home is /home/klinger

    Above, `$` is special and expands the environment variable
    `HOME`, but the spaces are literal and do not split the argument.

    The *quoted backslash* behaves different from the *unquoted* one:
    It only quotes characters which are still special in the weakly
    quoted context (plus, it removes a newline), and is otherwise not
    removed:

        $ ./arg   \n   "\n"   \'   "\'"   \\   "\\"   \$x   '$x'   "\$x"
        argv[0] = ./arg
        argv[1] = n       # Non-special character preserved (kinda pointless).
        argv[2] = \n      # Quoted backslash remains because n is not special.
        argv[3] = '       # Made single quote literal.
        argv[4] = \'      # Single quote not special in weakly quoted context.
        argv[5] = \       # First backslash escapes second one…
        argv[6] = \       #   …in unquoted and weakly quoted context.
        argv[7] = $x      # Quote dollar sign in unquoted, …
        argv[8] = $x      #   …strongly quoted, …
        argv[9] = $x      #   …and weakly quoted context.

  * More exotic variations:

    In an unquoted context, `$'…'` introduces a strong quoting style
    which interprets `\`-escaped characters as known from ANSI C,
    e.g., `\n` and `\t` become newline and tab.

        $ ./arg $'foo\tbar\n\'\n${HOME}'
        argv[0] = ./arg
        argv[1] = foo   bar
        '
        ${HOME}

    And `$"…"` is supposed to invoke translation according to the
    current locale, but I've never seen this working.

Confused?  There's a simple mental model to escaping, weak and strong
quoting on the shell:


### Not “string literals” but “quoting toggles”

Think of backslash `\`, single quote `'` and double quote `"` as
**quoting toggles** that switch different quoting modes on and off,
with `\` affecting only the following character.

Parsing your input left-to-right, the shell **starts in unquoted
mode**, then:

  * In unquoted mode,

      - a backslash `\` only **escapes** the next character, or skips
        a directly following newline,

      - a single quote `'` switches to **strong quoting** mode,

      - a double quote `"` switches to **weak quoting** mode.

  * In strongly quoted mode,

       - a single quote `'` ends strong quoting, switching back to
         **unquoted** mode,

  * In weakly quoted mode,

      - a double quote `"` ends weak quoting, switching back to
        **unquoted** mode,

      - a backslash `\` removes any special meaning from a following
        metacharacter (`"`, `$`, `` ` ``, `\`), or is used literally
        if an ordinary character follows.

This simplifies writing a parser to a pretty straight forward state
machine, and also allows to **switch the quoting style** on the fly,
as needed:

  * Unusual, but proves the point: The first `'` switches strong
    quoting on (protecting the space), the second one switches it off.

        $ ./arg hel'lo wor'ld
        argv[0] = ./arg
        argv[1] = hello world

  * A single quote with strongly quoted text:

        $ ./arg 'now it'\''s time'
        argv[0] = ./arg
        argv[1] = now it's time

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


### Better default configuration to encourage proper quoting

Assume a lon-running task writing log files into a directory `./logs`.
How to delete the old log files?  The approach of

    $ find ./logs -name *.log -mtime +30 -delete

seems reasonable, as find(1) says:

    -name pattern
           Base of file name (the path with the leading directories
           removed) matches shell pattern pattern.

This may even work on a test system, but still fail on a production
system.

Obviously, the unquoted `*.log` is the bug.  If the shell is set up
wrong (which seems to be the default with many distributions), it may
be difficult to notice the issue by testing.

To play with this, create some test data

    $ mkdir -p logs
    $ touch -d "@$(date -d'32 days ago' +%s)" logs/{{1..3}.log,data1.dat}
    $ touch -d "@$(date -d'22 days ago' +%s)" logs/{{4..6}.log,data2.dat}
    $ ls -l logs
    …

Instead of deleting the old log files, let's just print them instead,
so we can evaluate different scenarios:

    $ find ./logs -name *.log -mtime +30
    ./logs/3.log
    ./logs/2.log
    ./logs/1.log

This is the expected output.  Do you see it too?  It depends on the
settings of `failglob` and `nullglob`, ond (even worse!) on one more
thing.  Care to take a guess?

    $ shopt -u failglob nullglob
    $ find ./logs -name *.log -mtime +30
    ./logs/1.log
    ./logs/2.log
    ./logs/3.log

Ok, the desired output, which the developer has seen during his test.
It would work on any shell set up with `failglob` and `nullglob`
disabled, if one more precondition is met.

Let's enable `failglob`:

    $ shopt -s failglob
    $ find ./logs -name *.log -mtime +30
    bash: no match: *.log

What, no match?  So the successful execution is the result of bash
“being smart” and not failing, but passing a glob it cannot resolve to
the program.  In other words, it perversely **hinges on the occurrence
and handling of an expected error**.

    $ touch foo.log
    $ find ./logs -name *.log -mtime +30   # no output

The presence of any file matching `*.log` in the *current* working
directory changes how `find` is called: Now the glob expands to
`foo.log`, no such file exists in `./logs`, nothing is printed (and
nothing would be deleted).  The setting of `nullglob` and `failglob`
is not relevant, because the glob does not fail.  And if you have not
seen any files listed for the commands above, then the non-occurrence
of this error may be the reason why.

By the way, find(1) knows its users too well =)

    $ touch bar.log
    $ find ./logs -name *.log -mtime +30
    find: paths must precede expression: `foo.log'
    find: possible unquoted pattern after predicate `-name'?

Here, we have executed (after filename expansion)

    find ./logs -name bar.log foo.log -mtime +30

and find(1) is smart enought to smell the rat.

**Summary:** The correct command invocation is with a quoted glob

    $ find ./logs -name '*.log' -mtime +30 -delete

and the mistake of forgetting to quote is more likely to be noticed
when `shopt -s failglob` is set as default in `~/.bashrc`, because
you'll get an error if the glob cannot be resolved, or the results
printed are unlikely to be the expected ones.



Seven kinds of expansion
------------------------

*After* initially splitting the input into words (at blanks, i.e.,
space and tab), the shell performs **seven kinds of expansion**.

Amongst these, you'll find

  * parameter and variable expansion, command substitution

  * a second round of word splitting, but this time using the
    **Internal Field Separator** `$IFS` for delimiting words, and

  * pathname expansion, aka. globbing

to be performed in this order.

> Note: I don't want to repeat the manual here.  These are all
> outlined in bash(1), and **much more detailed** in `info
> '(bash)Shell Expansions'`.  Look them up as you need them.

The details are many and subtle, but *one* major point, simplified, is
this: The result of expanding an **unquoted variable** is itself split
into words.  And all the resulting words are used for globbing.

    $ x='a* b*'

    $ ./arg "$x"       # weakly quoted, result not expanded
    argv[0] = ./arg
    argv[1] = a* b*

    $ ./arg $x         # unquoted, result expanded and used as globs
    argv[0] = ./arg
    argv[1] = arg
    argv[2] = arg.c
    argv[3] = background
    argv[4] = background.c

Note, that the **initial word splitting** (finding the command) uses
**blanks** (i.e., space and tab) to separate words.  The word
splitting of expansion results uses the characters in the shell's `$IFS`
variable (defaulting to `$' \t\n'`):

    $ x='a*,b*'       # note the comma

    $ ./arg $x
    argv[0] = ./arg
    argv[1] = a*,b*   # no file name contains a comma

    $ IFS=,           # set IFS to split words at `,` after expansion

    $ ./arg $x foo
    argv[0] = ./arg
    argv[1] = arg
    argv[2] = arg.c
    argv[3] = background
    argv[4] = background.c


Advice: Always quote your variables
-----------------------------------

Unquoted use of shell variables (except for legitimate reasons) tend
to cause trouble.  The web knows many examples, in the extreme
[amounting to](https://unix.stackexchange.com/questions/171346)
security implications like arbitrary code execution.


### Quoting alone may not suffice

Quoting alone may not be enough to protect a command invocation
against unintended effects.  E.g., [filenames with leading
dashes][2a] might be interpreted as flags by a program.

Some programs use the `--` marker to denote “end of options”, i.e.,
everything that follows is used as a filename (`info
'(coreutils)Common options'`).  For other programs, resort to
prefixing the file with `./`, or construct an absolute path.  For
non-file arguments, other means may be necessary:

    $ mv -- "${untrustedFilename}"
    $ sometool "./${untrustedFilename}"    # if `--` is not understood
    $ printf '%s\n' "${untrustedText}"     # instead of `echo`


Shell scripts
=============

A shell script is an executable plain text file (cf. chmod(1)),
starting with the **shebang** (aka. **hashbang**) indicating where to
find the interpreter.

    #!/bin/bash

The exact mechanics and limitations are explained in execve(2).


Environment variables
---------------------

FIXME move this to past execve

Every Linux processes can access a per-process list of environment
variables, cf. environ(7).

  * By convention, these are key/value pairs, entries being
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

  * A shell variable `X` can be exempt from being exported with
    `export -n X`.

> So can one modify the *environment* of a running process?
>
> Having sufficient privileges, of course one can.  But AFAIK there's
> no Linux API to do that, so I'd expect it to be bad practice: A
> running program is unlikely to expect changes to the environment
> variables.


Advice on scripting
-------------------

  * Use available tools.  The shell is a **tool-combiner**.

    Learn sed(1), find(1), ssh(1), tar(1), ….

  * Shell scripts do have **no suffix** in their filename.

    Using a suffix like `.sh` would be the surfacing of an
    implementation detail.  User experience degrades.  On replacing
    the script by another implementation, users (human and mechanic)
    have to be informed and adapt.  The apt user can find out whether
    a file is a script (and what kind of), and the rest should not
    care.

  * Use **lowercase variable names** in your scripts.

    This is a poor man's namespace segregation: Though there's no
    technical need, *environment variables* tend to be uppercase-only.
    So lowercase variables decrease your chance to overwrite one
    (which is described in [The Open Group Base Specifications][3]).

  * The **shebang** is `#!/bin/bash`.

    Because the [Linux Filesystem Hierarchy Standard
    (FHS)](https://refspecs.linuxfoundation.org/fhs.shtml) puts
    “Essential user command binaries” in `/bin`, amongst which are
    `/bin/sh` and `/bin/csh`.

    Okay, *maybe* it's `#!/usr/bin/bash`.

    The use of `#!/usr/bin/env bash` is a silly custom, originating in
    the early days of Python, the community of which was plagued by
    different ideas about where to put the interpreter.  Also, it now
    depends on the location of env(1), and the value of `$PATH`, both
    of which counteract portability.

  * Always **aiming for portability is a misguided objective**.

    Shell scripts typically utilise avaliable system tools,
    e.g. sed(1), which already differs between unixoid systems (`-E`
    versus `-r` for extended regular expressions).

    If you do need portability, decide on the standards you commit to
    (SUS, POSIX, FHS, …?), limit your use of system tools to the
    standardised, and rethink whether the effort is warranted.  Then
    the shebang is likely `#!/bin/sh`.

    If you still insist on *portability* ∩ *bash*, read [Bash empty
    array expansion with `set
    -u`](https://stackoverflow.com/questions/7577052/) and [the
    associated
    experiment](https://gist.github.com/dimo414/2fb052d230654cc0c25e9e41a9651ebe),
    leaving you with a wart like `"${array[@]+"${array[@]}"}"` to
    safely expand an array.

  * Rather **fail fast** and catastrophically, than late and subtly.

  * A **template** sufficient for 99% of your bash scripts:

        #!/bin/bash
        set -u -e -C
        shopt -s failglob nullglob

    Then, deviate as needed, e.g., adjusting `failglob` and `nullglob`
    mid-script as required.

  * A **stub** for your `~/.bashrc`:

        set -u +H -C
        shopt -s failglob globstar
        shopt -u progcomp    # progcomp is buggy with failglob

        # See: info '(coreutils)Formatting the file names'
        export QUOTING_STYLE=shell     # or `shell-escape`


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
PID (`$PPID`).  But be careful, `$$` **may give unexpected results**,
see below.

    $ echo "PID $$, PPID $PPID"
    PID 10566, PPID 10561

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

On Linux, when you execute a program, then **by replacing the program
running in the current process** — which may seem odd at first.

The execve(2) system call (also observed above) **does not return** on
success.  It replaces the calling program by a new one.  Some of the
execution context will be modified during that transition, some will
be left unchanged.  Details in the manual.

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

Note: `pid` sports the same PID as `exec`, so it is the same process
executing the program `exec` first, and the program `pid` second.  The
program executed in the process has been replaced!

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


The fork/exec pattern
---------------------

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
> Note 1: The shell will not terminate right away, because it waits
> for its children before termination.  But you'll see the date(1)
> printed immediately, proving the sleep(1) to run in the background
> of the shell.
>
> Note 2: wait(2) may actually be a frontend to wait4(2).  I’ll stick
> to writing “wait” for simplicity, but expect strace(1) to report
> “waitfor” .


### Orphans, zombies, reapers, and double forks…

If a child process dies, it becomes a **zombie** until…

  * …its exit status is collected by the parent via wait(2) & Co., or

  * …its parent dies and the zombie is adopted by `init` which
    implicitly performs the wait(2) — a process called “reaping”,
    which is why `init` is also referred to as **reaper**.  There may
    be **sub-reapers**, see pid_namespaces(7).

The OS must maintain the exit status of a child for retrieval by its
parent (while the reaper don't care).  Thus, under a long-living
parent, zombies may amass and fill the process table.

⇒ Wait for your children lest they become zombies.

It was an established technique to fight zombies with a **double
fork**.  Alternatively, explicitly ignore SIGCHLD.  The shell reacts
to SIGCHLD to reap zombies as soon as possible, and only reports
before printing the next prompt:

    $ sleep 1 &
    $                 # wait more than 1 second, then hit Return
    [1]+  Done                    sleep 1
    $



FIXME talk about subshells introduced by `$(…)`.

FIXME talk about the `jobs` and `wait` shell builtins.


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
Pressing C-d (aka. Ctrl+D, ^D) signals end of input.

    $ cat
    dog
    dog

> The fact that I/O appears line by line is a result of buffering in
> the *line discipline* of the terminal driver, a topic just
> *slightly* out of scope here… ;-)



Redirection
-----------

As example, the following command should print the listing of one
file to *stdout*, and an error message to *stderr*:

    $ ls README.md /noSuchFile
    ls: cannot access '/noSuchFile': No such file or directory
    README.md

Use `1>` (or the shorthand `>`) to write *stdout* to a file:

    $ ls README.md /noSuchFile 1> file1
    ls: cannot access '/noSuchFile': No such file or directory

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

    $ ls README.md /noSuchFile 2> file2
    README.md

    $ cat file2
    ls: cannot access '/noSuchFile': No such file or directory

Similarly, use `0<` (or the shorthand `<`) to read *stdin* from a
file:

    $ cat 0< lorem.txt
    Lorem ipsum dolor sit amet, consectetur adipiscing elit

What about using `42>`?

    $ ls README.md /noSuchFile 42> file42
    $ ls -l file42

What do the numbers (0 for *stdin*, 1 for *stdout*, 2 for *stderr*, 42
for whatever) mean?


### Open a file by name

> open(2) may actually use openat(2), but I'll stick to writing “open”
> for simplicity.  But look for `openat` in the output of strace(1).

Usually, one would open(2) a file, before reading or writing, see
[open.c](open.c)

    $ strace -e trace=%file,write ./open
    …
    openat(AT_FDCWD, "open.out", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
    write(3, "Hello named file\n", 17)      = 17
    …

    $ cat open.out
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
[write.c](write.c) writes to a file descriptor passed as argument,
without opening any files:

    $ ./write 5
    write: Using file descriptor 5.
    write: Write to file descriptor 5 failed: Bad file descriptor

    $ ./write 5 5> file5
    write: Using file descriptor 5.

    $ cat file5
    play the kazoo

So somehow the file `file5` was opened for writing, and the file
descriptor 5 was made available to the newly run program.

How does the shell do that?  We already know how to get a file
descriptor from open(2).  But here, the shell needs to

 1. control the file descriptor returned by open(2), because *we* want
    it to be 5, and

 2. communicate an open file descriptor to a new program, `./write` in
    this case.


### Make sure a file is opened with given file descriptor

We cannot control the file descriptor returned by open, but we *can*
assign an open file descript**ion** (referenced by its file
descript**or**) to another file descriptor, using dup2(2) —
[dup2.c](dup2.c):

    $ ./dup2
    knock knock
    dup2: open("dup2.out") returned 3

    $ cat dup2.out
    who's there


### Open file descriptors survive execve(2)

Open files are a **property of the process**, not of the program.  By
default (cf. `FD_CLOEXEC` in fcntl(2)), they remain open across a call
to execve(2).

Example [fdexec.c](fdexec.c):

    $ ./fdexec
    fdexec: open("fdexec.out") returned 3
    write: Using file descriptor 3.
    $ cat fdexec.out
    play the kazoo

`fdexec` opens the file, then replaces itself with [write](write.c),
passing the file descriptor as command line argument.


### Summary: redirection with files

So redirection from/to files simply makes the shell open these files
in read/write mode, and assigns them to the expected file descriptors.

Use cat(1) and redirecting *stdin* and *stdout* to copy a file:

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
and is now setting up the file descriptors:

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

The newly loaded program cat(1) started its live in an environment
with correctly set up file descriptors.  Hence, the shell did not
communicate file contents to/from cat(1), but rather handed the open
file descriptors to it.

After copying, the shell collects the child's exit code.

    …
    35958 <... wait4 resumed>…, 0, NULL) = 35959


Notes and conclusions
---------------------

### Open file descriptors are also shared on fork(2)

Example [fdfork.c](fdfork.c):

    $ ./fdfork
    fdfork: Writing to file: fdfork.out

    $ cat fdfork.out
    I am the parent
    I am the child

Note: Both processes share the write offset in the file, which is part
of the open file description.

Optional: Demonstrate correct interleaved writing by repeating each
write 100 times.

> Historical note: This was not handled correctly by Linux before
> 3.14, see the “Bugs” sections in read(2) and write(2).


### The flags passed to open(2)

You can easily observe the effects of the following redirection
operators on the open system call:

    $ for i in '<' '>' '<>' '>>' '>|'; do
          c="true ${i}foo"   # the command to run
          echo "$c"
          strace -f bash -C -c "$c" 2>&1 | grep -E '^open.*foo'
          rm -f foo
      done

If the `-C` shell option is used (you should do this), `>` implies
`O_EXCL` protecting you from unwanted damage.


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
descript**ions**, *not sharing* a common file offset for writing:

    $ strace -olog -f bash -c 'ls -l README.md /noSuchFile 1>file12 2>file12'
    $ grep -A2 file12 log
    12403 openat(AT_FDCWD, "file12", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
    12403 dup2(3, 1)                        = 1
    12403 close(3)                          = 0
    12403 openat(AT_FDCWD, "file12", O_WRONLY|O_CREAT|O_TRUNC, 0666) = 3
    12403 dup2(3, 2)                        = 2
    12403 close(3)                          = 0

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

![[Key Competence in Computer Science, Winter 2015][4a], page
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
`dup2` involved.  (See [solution1](solution1.md))


### The shell's current open files

FIXME maybe move this into shell script section

If the `exec` builtin is used without giving a command, then its
redirections take effect in the current shell.

    $ exec 1>log
    $ date           # note, no output!
    $ cat log >&2
    Sat Jun 26 04:54:24 PM CEST 2021

FIXME talk about closing file descriptors with `exec 23>&-`.

> Bash 5.1 (FIXME: maybe earlier) allows to use the file descriptor
> returned by open(2), and assign it to a shell variable, which gives
> a pretty direct interface to open(2) and close(2).  The syntax is
> `{fd}>filename`, binding `$fd` to the file descriptor returned by
> `open("filename", …)`.  Search bash(1) for `>&-` for more
> information.  **Note**, that this is not available in older
> versions.


Connecting processes
====================

Getting data from one process to another is generally termed [inter
process communication][5] (IPC).  The needs are diverse, and an
operating system typically offers different means to different ends:
From two processes alternatingly using the same file, to signals,
semaphores, shared memory, pipelines, and network sockets.

Here, we only talk about pipelines.  Any good operating system lecture
should present the others as well.


Pipelines
---------

A **pipeline** is a sequence of one or more *commands* separated by
the pipe `|` control operator.

> Above is paraphrased from bash(1).  Search bash(1) for “SHELL
> GRAMMAR” to get the complete picture.

The idea is to **pipe** *stdout* of one command into *stdin* of another.

    $ date | rev

Remember, that “a simple command is a sequence of words *and
redirections*”, i.e., in a pipeline, **each command has its own
redirections**:

    $ ls *.c /noSuchFile  |  wc -l  2>errors
    ls: cannot access '/noSuchFile': No such file or directory
    17
    $ cat errors   # no content

    $ ls *.c /noSuchFile 2>errors  |  wc -l
    17
    $ cat errors
    ls: cannot access '/noSuchFile': No such file or directory

So *after* setting up redirections for each command, the resulting
file descriptor 1 of the former command is connected to file descriptor
0 of the latter.


### Swapping *stdout* and *stderr*

Only *stdout* of a command is forwarded in a pipe (or used in a
command substitution or process substitution, see below), but *stderr*
is not.

What if you want to pipe a commands *stderr* into another command,
e.g., to count its error messages?

    $ foo 3>&1 1>&2 2>&3 3>&- | bar

The `3>&-` simply calls close(2) on file descriptor 3, so that the
program `foo` does not see (and hand down to its children) an open
file descriptor 3, keeping alive an unneeded open file description.

> This makes a great interview question.  Also a nice question:
> Assuming there's an open file descriptor 5, what's the difference
> between the redirections `2>&5` and `2<&5`?



Pipes are an OS feature
-----------------------

See pipe(7) for an overview.

The pipe(2) system call creates a **unidirectional data channel**.
The reading and writing end are bound to open file descriptors.

The plain system call allows sending data between related (fork(2))
processes, see [pipe.c](pipe.c):

    $ ./pipe
    Hello child process

Combined with our knowledge of exec(3) and dup2(2), this extends to
building pipelines.  We create a separate child for reading and
writing, [pipeline.c](pipeline.c)…

    $ ./pipeline date rev
    1202 TSEC MP 34:63:10 72 nuJ nuS

…as the shell does:

    $ strace -olog -etrace=pipe2,clone,execve,dup2,read,write -f bash -c 'date | rev'
    4202 TSEC MP 43:01:11 52 yaM taS

    $ cat log     # I've simplified the output
    …
    51877 pipe2([3, 4], 0) = 0
    51877 clone(...) = 51878
    51877 clone(...) = 51879
    51878 dup2(4, 1) = 1
    51878 execve("/usr/bin/date", ["date"], ...) = 0
    51879 dup2(3, 0) = 0
    51879 execve("/usr/bin/rev", ["rev"], ...) = 0
    …
    51878 write(1, "Sat May 25 11:10:34 PM CEST 2024"..., 33) = 33
    51878 +++ exited with 0 +++
    51879 read(0, "Sat May 25 11:10:34 PM CEST 2024"..., 4096) = 33
    51879 write(1, "4202 TSEC MP 43:01:11 52 yaM taS"..., 33) = 33
    51879 read(0, "", 4096)                 = 0
    51879 +++ exited with 0 +++
    …

> Note, that on older systems strace(1) may report `pipe` rather than
> `pipe2` (and requires the filter set up accordingly).  As usual, I
> keep talking about `pipe` for simplicity.


### FIFOs

FIFOs (aka. **named pipes**) are a very similar concept, except that
the pipe is not bound to open file descriptors within a common parent
process, but to a name in the file system instead.  Cooperating
processes may then use convention to **find the pipe by name**,
resolving the need for a **common ancester** process to create the
pipe, and to inherit the open file descriptors from.

See fifo(7) for overview, mkfifo(1) and mkfifo(3) for use.


Redirections involving pipes
----------------------------

Note the subtle difference between “pipeline” (a shell grammar
construct), and “pipe” (an OS
[IPC](https://en.wikipedia.org/wiki/Inter-process_communication)
concept).

**Note**: Unless documented (search bash(1) for “subshell”), the
precise arrangement of processes, whether and when a child is created,
and of which parent, is an *implementation detail* of the shell.  Do
not assume the structure revealed by strace(1) to be applicable to
other situations.  We'll discuss some relevant artefacts of subshells
in the next section.


### Literal text to *stdin*

Pipelines are the mechanism behind bash(1) **here strings** and **here
documents**:

    $ rev <<<'Amore, Roma.'
    .amoR ,eromA

    $ strace -olog -etrace=pipe2,clone,execve,dup2,read,write -f bash -c "rev <<<'Amore, Roma.'"

Have a look at the `log` file: The child forked for rev(1) creates a
pipe, and writes to it before replacing itself with the rev(1)
program.  The process talks to itself, replacing the program it runs
in between.


### *stdout* to text

bash(1) **command substitution** replaces `$(command)` with the
output written by `command` to its *stdout*.

    $ ./arg "$(date)"
    argv[0] = ./arg
    argv[1] = Sun Jun 27 03:54:05 PM CEST 2021

    $ strace -olog -etrace=pipe2,clone,execve,dup2,read,write -f bash -c './arg "$(date)"'

*Between the parenthesis* of `$(…)`, write a command as usual.  The
inside quoting starts out in **unquoted mode** (cf. above) wrt. the
command, which is exactly what you'd want to have.

But note, that the output of the command undergoes word splitting, so
**proper quoting** of the command substitution is required.

**Do not use** the historical syntax, `` `…` ``.  Some consider it
deprecated, it does not nest as nicely, is harder to read, and treats
some characters differently.  But remember to **quote** all
occurrences of backticks you want to use, e.g., in printed messages,
lest they invoke a command.


### *stdout* to named file

Also, bash(1) **process substitution** uses pipes, supported on
systems where a process has its open file descriptors listed under
`/proc/self/fd/` (or similar, use realpath(1) or namei(1) to find
out):

    $ rev <(date)
    1202 TSEC MP 12:40:30 72 nuJ nuS

    $ echo <(date)
    /dev/fd/63

So `<(…)` is actually replaced by a **file name** that is internally
connected to a pipe providing *stdout* of the enclosed command.

Again,

    $ strace -olog -etrace=openat,pipe2,clone,execve,dup2,read,write -f bash -c 'rev <(date)'

reveals how a pipe's read end becomes the argument of rev(1):

    52454 execve("/usr/bin/bash", ["bash", "-c", "rev <(date)"], 0x7fffa4f7ac68 /* 70 vars */) = 0
    …
    52454 pipe2([3, 4], 0)                  = 0
    52454 dup2(3, 63)                       = 63
    52454 clone(…)                          = 52455
    52455 dup2(4, 1)                        = 1
    52454 execve("/usr/bin/rev", ["rev", "/dev/fd/63"], 0x584a51aec850 /* 70 vars */) = 0
    …
    52455 execve("/usr/bin/date", ["date"], 0x584a51aec850 /* 70 vars */)             = 0
    …
    52454 openat(AT_FDCWD, "/dev/fd/63", O_RDONLY) = 3
    52454 read(3,  <unfinished ...>
    …
    52455 write(1, "Sat May 25 11:41:42 PM CEST 2024"..., 33) = 33
    52454 <... read resumed>"Sat May 25 11:41:42 PM CEST 2024"..., 4096) = 33
    52454 write(1, "4202 TSEC MP 24:14:11 52 yaM taS"..., 33) = 33
    …

Note, that the “file” `/dev/fd/63` resides in a virtual file system,
used by the kernel to expose information to a process.  In this case,
a list of its file descriptors.


Subshells
=========

“Each command in a pipeline is executed as a separate process”, says
bash(1).  This can be observerd

    $ echo $$
    8185

    $ ./pid | ./pid
    pid: PID 8238, PPID 8185
    pid: PID 8237, PPID 8185

And this is also true, when a command in the pipeline is not an
executable program, but a shell builtin or compound command instead:

    $ date | if true; then echo "$$" "${BASHPID}"; fi | cat
    8185 8382

`$BASHPID` expands to the PID of the bash process doing the expansion,
while `$$` expands to a less well-defined parent shell from within
subshells, usually the shell in your terminal, or the one running a
script.

> Actually, documentation is a tad unclear.  According to bash(1),
> `$$` refers to the “current shell”, and `$BASHPID` to the “current
> bash process” without clarifying the difference, nor “current”.
>
> According to `info '(bash)Special Parameters'`, in a subshell `$$`
> should expand to the PID of the “invoking shell” — which is slightly
> more correct, except for *nested* subshells as in
>
>     $ echo "$$"; ( echo "$$"; ( echo "$$" ))
>     4488
>     4488
>     4488
>
> where the last number —according to the text— should be different,
> as an strace(1) reveals:
>
>     4488  clone(…) = 4489
>     4489  clone(…) = 4490
>
> Thinking about this reveals the presence of a not precisely defined
> concept of a *main shell* that `$$` refers to.  It's probably “the
> shell thou typeth into”…

Enough nitpicking!


Consequences of subshells
-------------------------

FIXME bash execution context.  Builtins may modify this, read →
variable, cd → pwd, …

### Reading from a command

FIXME `read` builtin.

If a subshell modifies its variables, then this information is **not
propagated back** to the parent shell:

    $ date | read x
    $ echo $x
    bash: x: unbound variable        # if `set -u` is used

or, more sublte:

    $ count=0
    $ ls *.c | while read name; do echo "$((++count))"; done
    1
    2
    3
    …
    17
    $ echo "$count"
    0                  # dafuq?

The correct way is to have the counting **variable in the current
shell** (to be clear: the shell you want to have the modified variable
in).

    $ count=0
    $ while read name; do echo "$((++count))"; done < <(ls *.c)
    1
    2
    3
    …
    17
    $ echo "$count"
    17

Above, the semantics is slightly different: We do not build a
*pipeline*, but use a *pipe* to read from a subshell running ls(1)
into the *current shell* evaluating the loop.


### Robust reading of directory contents

    $ while IFS= read -r -d '' x; do
          echo "${x}"
      done < <( find /tmp -maxdepth 1 -type f -print0 )

Explain

  * Why `IFS=`

  * Why -r

  * Why a space between `-d ''`

  * Why `-print0`

To protect against file names with leading dashes, find(1) prefixes
its results with the search path even if it's just `./`.

Much better and extensive examples can be found in the
[BashPitfalls][2] wiki.

____________________

TODO
====

  * bash's coprocesses

  * builtin `mapfile` (aka. `readarray`)

  * `xargs`

  * closing file descriptors, or using exec for the shell's own redirections

  * `$@`, `$*`, `arr=( "$@" )`

  * to $HOME or ${HOME}

  * bad:  path='/foo/bar/'; file="${path}qux"

  * `namei`, `tee`

  * Subshells created by `… <(…)` or `$(…)`, compared to `{…}`, and
    not to be confused with creation of arrays `…=(…)`.

  * Maybe explain read(2), and why/when C-c or C-d is used to end a
    program.  C-c makes the terminal (FIXME make clear to be vague
    about “terminal”) send a signal SIGINT to its child process, while
    C-d makes it flush stdin.  Write your own `cat`.


Leftovers
---------

  * [closedup.c](closedup.c) demonstrates closing on duplicated file
    descriptor.

  * [conflict.c](conflict.c) demonstrates conflicting write to file
    opened twice.

  * [redirect.c](redirect.c) demonstrates redirection of child' stdout
    to file.


Further Reading
---------------

  * https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_09



[1]: https://www.grymoire.com/Unix/index.html
[2]: https://mywiki.wooledge.org/BashPitfalls
[2a]: https://mywiki.wooledge.org/BashPitfalls#Filenames_with_leading_dashes
[3]: https://pubs.opengroup.org/onlinepubs/9699919799/xrat/V4_xbd_chap08.html
[4a]: https://stefan-klinger.de/files/sq_15w.pdf
[4b]: https://stefan-klinger.de/files/pk3_15w.pdf
[5]: https://en.wikipedia.org/wiki/Inter-process_communication
