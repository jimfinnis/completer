#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <histedit.h>

#include "completer.h"

/*
 * 
 * UNFINISHED. Unstarted, even.
 *
 */


const char *getPrompt(){
    return ">> ";
}

class FileAutocompleteIterator : public completer::Iterator {
public:
    
    virtual void first(){
        
    }
    virtual const char *next(const char *stringstart,int len){
        return NULL;
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
    
    FileAutocompleteIterator fileiter;
    completer::setup(el,&fileiter,"\t\n ");
    
    for(;;){
        int count;
        const char *line = el_gets(el,&count);
        if(line){
            printf("* %s",line);
        }
        else break;
    }
    completer::shutdown(el);
    return 0;
}
