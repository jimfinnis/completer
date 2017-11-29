# completer
Simple string autocompleter for libedit / editline in C++. Allows tab-completion and double-tab to list possible completions.
Permits simple context-sensitive completion by argument number (i.e. different "words" in the input can be completed in different ways).

## Usage
Include `completer.h` and `completer.cpp` in your codebase. In files
which use the completer, include `completer.h`.

Create a subclass of `completer::Iterator`. This class will
iterate through the possible strings, comparing them against the partial input string and returning them
if they match.
It must implement the following virtual methods:
* `virtual void first(const char *stringstart,int len)` : reset the iterator. The parameters are the characters typed so far and how many there are.
* `virtual const char *next()`: get the next matching string and advance the iterator, return the match or NULL if out of matches.

Create an instance of this class.

Then call `completer::setup()` passing three values:
* The `EditLine` instance which will use this completer
* A pointer to the `Iterator` instance
* A string of characters considered to be word breaks: the completer
will run back through the input string until it finds either the start
of the string or one of these characters, to indicate where the match
should start.

This function will connect the `EditLine` object with the completer
through the `EL_CLIENTDATA` parameter - this means you cannot use
this parameter for other purposes. It will also set `EL_GETCFN` to
replace the standard character input function, in order to count tabs
and act accordingly.

Once this is done, use `EditLine` as normal. Do not use the `readline`
compatibility layer.

When finished, call `completer::shutdown()` passing in the `EditLine`
instance to disconnect the completer.

## Context-sensitive completion with alternative iterators
It is possible to autocomplete each item in the line differently.
By default, the iterator passed into `setup()` is used to get
completions for all items (where items are separated by
the word break characters passed into `setup()`).
However, calling `completer::setArgIterator()`
will set a different iterator to use for a given item. The parameters are
* the index of the item in the line (0 for the first item)
* a pointer to the `Iterator` to use.

## Example
A full example of a simple command completer can be found in
`commandCompleterExample.cpp`. This allows each word in the input
to be completed from a list of permitted words, given as an array 
of strings terminated by a NULL. This sets up two different iterators
for the first item and the others, demonstrating context-sensitive
completion.

