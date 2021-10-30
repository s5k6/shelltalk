What's wrong with

    $ find ./logs -name *.log -mtime +30 -delete

to delete old log files?  After all, it worked in testing, and find(1)
says:

    -name pattern
           Base of file name (the path with the leading directories
           removed) matches shell pattern pattern.

Obviously, the unquoted `*.log` is the problem.  But also, the
developers's shell is set up dangerously (I'd say *wrong*, although it
seems to be a default with many distributions), which is why he did
not notice.

To play with this, create some test data

    $ pushd "$(mktemp -d)"   # create and goto temp dir for test
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

Ok, the desired output, which the developer has seen during his test.  It
would work on any shell set up with `failglob` and `nullglob`
disabled, although another precondition is required (see below).

Let's enable only `nullglob`:

    $ shopt -s nullglob; shopt -u failglob
    $ find ./logs -name *.log -mtime +30
    find: paths must precede expression: `+30'

Above, the shell found no match for `*.log` in the current directory,
so expanded the glob to nothing, and actually called

    find ./logs -name -mtime +30

omitting the argument to `-name`.

Moving on, enable `failglob`, which also makes the setting of
`nullglob` irrelevant:

    $ shopt -s failglob
    $ find ./logs -name *.log -mtime +30
    bash: no match: *.log

That last example really rubs that other problem in one's face, does
it?  And if you have not seen any output for the commands above, then
this may be the reason why:

    $ touch foo.log
    $ find ./logs -name *.log -mtime +30   # no output

The presence of any file matching `*.log` in the *current* working
directory changes how `find` is called: Now the glob expands to
`foo.log`, no such file exists in `./logs`, nothing is printed (and
nothing would be deleted).  The setting of `nullglob` and `failglob`
is not relevant, because the glob does not fail.

Add one more:

    $ touch 1.log
    $ find ./logs -name *.log -mtime +30
    find: paths must precede expression: `foo.log'
    find: possible unquoted pattern after predicate `-name'?

That's not a subtle hint anymore, find(1) knows its users too well.
Here, we've executed

    find ./logs -name 1.log foo.log -mtime +30

because two log files exist in the current directory.

    $ rm foo.log
    $ find ./logs -name *.log -mtime +30
    ./logs/1.log

Since the glob is evaluated against the current directory, only the
file with the same name below `./logs` is printed (and would be
deleted).

**Summary:** The correct command invocation is with a quoted glob

    $ find ./logs -name '*.log' -mtime +30 -delete

and the developer would have been more likely to notice this mistake
by having `shopt -s failglob` as a default in `~/.bashrc`.

Bonus question: Give a scenario in which the command with unquoted
glob would still do exactly the expected thing, although `failglob` is
set.
