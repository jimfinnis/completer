#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <histedit.h>

#include "completer.h"

const char *getPrompt(){
    return ">> ";
}

class CommandAutocompleteIterator : public AutocompleteIterator {
    const char **commands;
    const char **ptr;
public:
    
    CommandAutocompleteIterator(const char **c){
        commands = c;
        ptr=NULL;
    }
    
    virtual void first(){
        ptr = commands;
    }
    virtual const char *next(const char *stringstart,int len){
        while(*ptr && strncmp(*ptr,stringstart,len))ptr++;
        return *ptr++;
    }
};
    

int main(int argc,char *argv[]){
    EditLine *el = el_init(argv[0],stdin,stdout,stderr);
    el_set(el,EL_PROMPT,&getPrompt);
    el_set(el,EL_EDITOR,"emacs");
    
    History *hist = history_init();
    HistEvent ev;
    history(hist,&ev,H_SETSIZE,800);
    el_set(el,EL_HIST,history,hist);
    
    static const char *commands[] = {
        "foo","bar","baz","fish","abort",NULL
    };
    CommandAutocompleteIterator completer(commands);
    setupAutocomplete(el,&completer,"\t\n ");
    
    for(;;){
        int count;
        const char *line = el_gets(el,&count);
        if(line){
            printf("* %s",line);
        }
        else break;
    }
    shutdownAutocomplete(el);
    return 0;
}
