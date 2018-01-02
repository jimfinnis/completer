#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <histedit.h>

#include "completer.h"

// editline uses this to get the prompt
const char *getPrompt(){
    return ">> ";
}

/// this is a simple autocomplete iterator - it is given a list of strings which
/// it uses to try to complete the user's input when TAB is pressed.
/// Feel free to copy this class for use in your own code. See the main() function
/// below for how to use it.

class CommandAutocompleteIterator : public completer::Iterator {
    /// our list of "commands" - single word strings which can complete the input
    const char **commands;
    /// the item in the list we're currently iterating
    const char **ptr;
    
    /// the data passed to first(), which we store.
    /// Other implementations might construct a list of candidates
    /// in first().
    
    const char *strstart;
    int len;
          
public:
    
    /// constructor - takes an array of strings (possible commands) which
    /// is terminated by a NULL.
    CommandAutocompleteIterator(const char **c){
        commands = c;
        ptr=NULL;
    }
    
    /// get the first item, given the start of the word entered
    /// so far and its length.
    
    virtual void first(const char *stringstart,int length){
        strstart = stringstart;
        len = length;
        ptr = commands;
    }
    
    /// skip (if necessary) to the next matching item
    /// return the next item and increment the pointer
    virtual const char *next(){
        while(*ptr && strncmp(*ptr,strstart,len))ptr++;
        return *ptr++;
    }
};
    


/// the main program.

int main(int argc,char *argv[]){
    
    // basic EditLine and history initialisation
    
    EditLine *el = el_init(argv[0],stdin,stdout,stderr);
    el_set(el,EL_PROMPT,&getPrompt);
    el_set(el,EL_EDITOR,"emacs");
    
    History *hist = history_init();
    HistEvent ev;
    history(hist,&ev,H_SETSIZE,800);
    el_set(el,EL_HIST,history,hist);
    
    // create an iterator with a list of strings for the first argument
    
    static const char *commandsFirst[] = {
        "foo","bar","baz","fish","abort",NULL
    };
    CommandAutocompleteIterator completerFirst(commandsFirst);
    
    // and another with a list of strings for the other arguments
    
    static const char *commandsDefault[] = {
        "somearg","anotherarg","thirdpossiblearg",NULL
    };
    CommandAutocompleteIterator completerDefault(commandsDefault);
    
    // start up the completer, passing in the default iterator which will
    // be used to complete words. Also specify which characters count as
    // word breaks.
    
    completer::setup(el,&completerDefault,"\t\n ");

    // specify an alternative iterator for the first item
    
    completer::setArgIterator(el,0,&completerFirst);
    
    // a standard EditLine read loop..
    for(;;){
        int count;
        const char *line = el_gets(el,&count);
        // if we got a result, print it out (note that editline
        // returns string with a newline at the end)
        if(line){
            printf("* %s",line);
        }
        else break;
    }
    // disconnect EditLine from the completer and delete
    // internal structures.
    completer::shutdown(el);
    return 0;
}
