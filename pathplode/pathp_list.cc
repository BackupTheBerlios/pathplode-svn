//
// $Id$


// file
//#include <config.h>
#include "pathp_list.h"
#include <iostream>
#include <set>
using namespace std;

char pathp_list::separator_default = ':';

void pathp_list::set_hook(string new_hook_pattern, location_position new_pattern_hook) {
  if (pattern_hook!=none) {
    cerr << "Error: only one before/after regex must be defined!" << endl;
    exit(1);
  }
  hook_pattern = new_hook_pattern;
  pattern_hook = new_pattern_hook;
}


// constructor from path string
pathp_list::pathp_list(string path_string) {
  int sep_pos;
  separator = separator_default;
  string path_element;
  do {
    sep_pos = path_string.find(separator);
    path_element = path_string.substr(0, sep_pos);
    paths.push_back(path_element);
    path_string.erase(0, sep_pos+1);
    pattern_hook = none;
  } while (sep_pos!=-1);
           
}

pathp_list::pathp_list(const pathp_list& src_pplst) {
  separator = src_pplst.separator;
  paths = src_pplst.paths;
}

void pathp_list::prepend(string new_elm) {
  list<string>::iterator elm;
  bool found = false;
  elm = paths.begin();
  if (pattern_hook != none) {
    while (elm != paths.end() && !found) { 
      if (*elm == this->hook_pattern) { // TODO: should be a regexp match
        found = true;
        if (pattern_hook==after) elm++;
      } else elm++;
    }
  } else found=true;
  if (found==false) exit(1); // TODO: add better error handling
  insert_iterator< list<string> > ins_elm(paths, elm);
  *ins_elm = new_elm;
}

void pathp_list::append(string new_elm) {
  list<string>::iterator elm;
  bool found = false;
  elm = paths.end();
  if (pattern_hook != none) {
    while (!found && --elm != paths.begin()) {
      if (*elm == this->hook_pattern) { // TODO: should be a regexp match
        found = true;
        if (pattern_hook==after) elm++;
      } else elm--;
    }
  } else found=true;
  insert_iterator< list<string> > ins_elm(paths, elm);
  *ins_elm = new_elm;
}


void pathp_list::uniquify(void) {
  list<string> tmp_lst = this->paths;
  set<string> elms_found;
  this->paths.clear();
  list<string>::iterator elm = tmp_lst.begin();

  do {
    if (elms_found.find(*elm) == elms_found.end()) {
      elms_found.insert(*elm);
      this->paths.push_back(*elm);
    }
    elm++;
  } while (elm!=tmp_lst.end());
}


void pathp_list::list_elements(void) {
  list<string>::iterator element = paths.begin();
  do {
    cout << *element << endl;
    element++;
  } while (element!=paths.end());
}

string pathp_list::to_string(void) {
  string retval = "";
  int again = 0;
  list<string>::iterator element = paths.begin();
  do {
    if (again>0) {
      retval += this->separator;
    } else {
      again++;
    }
    retval += *element ;
    element++;
  } while (element!=paths.end());
    return retval;
}

ostream& operator<<(ostream &os, const pathp_list &pplst) {
  pathp_list tmp_pplst = pplst;
  string to_out = tmp_pplst.to_string();
  os << to_out;
    return os;
}

// pathp_list.cc


