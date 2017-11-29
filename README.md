# completer
Simple string autocompleter for libedit / editline in C++. Allows tab-completion and double-tab to list possible completions.

## Usage
Include `completer.h` and `completer.cpp` in your codebase. In files
which use the completer, include `completer.h`.

Create a subclass of `AutocompleterIterator`. This class will
iterate through the possible strings, comparing them against the partial input string and returning them
if they match.
It must implement the following virtual methods:
* `virtual void first()` : reset the iterator
* `virtual const char *next(const char *stringstart,int len)`: 
get the next matching string and advance the iterator, return the match
or NULL if out of matches. The parameters are the characters typed so
far and how many there are.

Create an instance of this class.

Then call `setupAutocomplete()` passing three values:
* The `EditLine` instance which will use this completer
* A pointer to the completer instance
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

When finished, call `shutdownAutoComplete()` passing in the `EditLine`
instance to disconnect the completer.

## Example
A full example of a simple command completer can be found in
`commandCompleterExample.cpp`. This allows each word in the input
to be completed from a list of permitted words, given as an array 
of strings terminated by a NULL.
