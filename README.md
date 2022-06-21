Compare normal search with a hash (Rabin-Karp) search
=====================================================

Usage:

tsearch - Search warpeace.txt for the word " convict "

tsearch the - Search warpeace.txt for the word " the "

tsearch the testfile.txt - Search testfile.txt for the word " the "

Options
=======
-p - Print lines that match

-1 - Only match one per line

-h - Only do hash search

-n - ONly do normal search

Notes
=====
Using -h and -n together does nothing good.

You can only provide (at most) one file name

If you want to provide a file name you must include a search string

Both the search string and lines are processed to be all lowercase and
have any non-letter characters converted to spaces. There is a space put at
both ends. So "happy" matches:

I am not happy

I am happy.

Happy days are here again.

Oh, happy, happy, joy, joy!

But does not match:

He was unhappy.

Build
=====

gcc -g -o tsearch tsearch.c

Output
======
Normal done (34737,4489678,0,4489678)

Hashed done (34737,3325996,6245928,9571924)


This output means that 34,737 matches were found by both algorithms (if this doesn't match, oh oh...)

There were 4,489,678 compares for the normal algorithm and 3,325,996 matches for the hashed algorithm.

The hashed algorightm had an extra 6.2 million math operations (3.1 million adds and 3.1 millon subtracts) and the
total is the last number.

NOTE
====
The default testfiles are too big for github so unzip tests.zip to get the text files.


