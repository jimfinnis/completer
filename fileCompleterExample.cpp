#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <histedit.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "completer.h"

#include <vector>
#include <string>

using namespace std;

/// example of a filename completer.


// This is an undocumented function in libedit. It takes a
// filename and expands any ~ (user-relative) names, allocating
// a new string for the result. Because it's an undocumented function
// it might change!

extern "C" {
char *fn_tilde_expand(const char *txt);
}

// Editline uses this to get the prompt.

const char *getPrompt(){
    return ">> ";
}


// This is the completion iterator. 

class FileAutocompleteIterator : public completer::Iterator {
    // list of completion candidates.
    vector<string> data;
    // our current position in that list.
    vector<string>::iterator it;
    
    // add the contents of a directory to the list of completion
    // candidates.
    
    void addDirContents(string prefix,const char *dir,
                        const char *match,int matchlen){
        DIR *d = opendir(dir);
        if(d){
            while(dirent *e = readdir(d)){
                if(!match || !strncmp(match,e->d_name,matchlen)){
//                    printf("added %s\n",e->d_name);
                    if(e->d_type == DT_DIR){
                        // add with trailing slash if a dir
                        data.push_back(prefix+string(e->d_name)+"/");
                    } else {
                        // otherwise just add
                        data.push_back(prefix+string(e->d_name));
                    }
                }
            }
        } 
    }
public:
    
    // construct the list of possible matches
    virtual void first(const char *stringstart,int length){
        data.clear(); // discard existing data
        const char *ptr;
        if(length){
            // find the last slash
            for(ptr=stringstart+length-1;ptr>stringstart;--ptr){
                if(*ptr=='/')break;
            }
            int len = ptr-stringstart;
            if(!len) {
                // there is no slash, so no directory element, match
                // against files in the current working directory
                addDirContents("",".",stringstart,length);
            } else {
                // there is a directory element, add the matching
                // contents - get the file element to match against
                string dir = string(stringstart,len);
                addDirContents(dir+"/",
                               dir.c_str(),
                               ptr+1,length-len-1);
            }
        } else {
            // a zero length argument, just add everything in the current
            // directory
            addDirContents("",".",NULL,0);
        }
        
        // if an unambiguous match, add a space if it doesn't
        // end with a slash.
        
        if(data.size()==1){
            string& s = data[0];
            if(s.length() && s[s.length()-1]!='/')
                s+=" ";
        }
        it = data.begin();
    }
    
    // iterate the list of possible matches
    virtual const char *next(){
        if(it == data.end())return NULL;
        return (*it++).c_str();
    }
    
    // This gets called before anything else happens and may replace
    // the entire word. This returns a string which must be freed, or NULL.
    // Here it is used to perform tilde expansion on filenames.
    
    virtual const char *modString(const char *stringstart,int len){
        if(len && *stringstart=='~'){
            // make a null-terminated copy of the word
            char *tmp = (char *)malloc(len+1);
            memcpy(tmp,stringstart,len);
            tmp[len]=0;
            // pass it to tilde expansion, free the buffer, return result
            // which the caller must free.
            const char *ret = fn_tilde_expand(tmp);
            free(tmp);
            return ret;
        } else
            return NULL;
    }
    // this iterator does its own padding (adding space or slash)
    // on unambiguous matches, so we turn off automatic space
    // addition at the end of those.
    virtual bool doSpacePadding(){return false;}
};


int main(int argc,char *argv[]){
    
    // basic EditLine and history initialisation
    
    EditLine *el = el_init(argv[0],stdin,stdout,stderr);
    el_set(el,EL_PROMPT,&getPrompt);
    el_set(el,EL_EDITOR,"emacs");
    
    History *hist = history_init();
    HistEvent ev;
    history(hist,&ev,H_SETSIZE,800);
    el_set(el,EL_HIST,history,hist);
    
    // create our file iterator, set up the completer with it
    // and tell the completer which characters are word separators.
    
    FileAutocompleteIterator fileiter;
    completer::setup(el,&fileiter,"\t\n ");
    
    // the main loop
    
    for(;;){
        // standard editline read
        int count;
        const char *line = el_gets(el,&count);
        
        // if we got a result, print it out (note that editline
        // returns string with a newline at the end)
        if(line)
            printf("FILE: %s",line);
        else break;
    }
    completer::shutdown(el);
    return 0;
}
