//
// $Id$


// file
//#include <config.h>
#include "pathp_list.h"
#include <iostream>
#include <set>
using namespace std;

char pathp_list::separator_default = ':';
bool pathp_list::purge_trailing_slash = true;

#define REVERSE_ITERATOR_DOES_NOT_WORK

void bail_out (string message) {
  cerr << message << "!" << endl;
  exit(1);
}

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
    if (purge_trailing_slash) {
      path_unslash_r(path_element);
    }    
    paths.push_back(path_element);
    path_string.erase(0, sep_pos+1);
    pattern_hook = none;
    use_colour = false;
    // this is set to false for speed... if we have purge_trailing_slash (default)
    // it is useless anyway
    ignore_trailing_slash = false; 
  } while (sep_pos!=-1);
}

pathp_list::pathp_list(const pathp_list& src_pplst) {
  separator = src_pplst.separator;
  paths = src_pplst.paths;
}

void pathp_list::prepend(string new_elm) {
  list<string>::iterator elm;
  bool found = false;
  if (purge_trailing_slash) {
    path_unslash_r(new_elm);
  }
  elm = paths.begin();
  if (pattern_hook != none) {
    while (elm != paths.end() && !found) { 
      if (*elm == this->hook_pattern) { 
        found = true;
        if (pattern_hook==after) elm++;
      } else elm++;
    }
  } else found=true;
  if (found==false) bail_out("Anchor not found during append");
  insert_iterator< list<string> > ins_elm(paths, elm);
  *ins_elm = new_elm;
}

void pathp_list::append(string new_elm) {
  list<string>::iterator elm;
  bool found = false;
  if (purge_trailing_slash) {
    path_unslash_r(new_elm);
  }
  elm = paths.end();
  if (pattern_hook != none) {
    while (!found && --elm != paths.begin()) {
      if (this->hook_pattern == *elm) { 
        found = true;
        if (pattern_hook==after) elm++;
      };
    }
  } else found=true;
  if (found==false) bail_out("Anchor not found during append");
  insert_iterator< list<string> > ins_elm(paths, elm);
  *ins_elm = new_elm;
}

void pathp_list::remove_first(string elm_to_remove) {
  list<string>::iterator elm;
  list<string>::iterator elm_last = paths.end();
  list<string>::iterator elm_next;
  bool found = false;
  if (purge_trailing_slash) {
    path_unslash_r(elm_to_remove);
  }
  elm = paths.begin();
  while (elm != paths.end() && !found) {
    elm_next=elm; elm_next++;
    if ((*elm == elm_to_remove) &&
        (pattern_hook == none
         || (pattern_hook==after && elm_last!=paths.end() && *elm_last==hook_pattern)
         || (pattern_hook==before && (elm_next)!= paths.end() && *elm_next==hook_pattern))) {
      found = true;
    } else {
      elm_last = elm;
      elm++;
    }
  } 
  if (found) paths.erase(elm);
}

#ifdef REVERSE_ITERATOR_DOES_NOT_WORK
void pathp_list::remove_last(string elm_to_remove) {
  list<string>::iterator elm, elm_found;
  list<string>::iterator elm_last = paths.end();
  list<string>::iterator elm_next;
  elm_found = paths.end();
  if (purge_trailing_slash) {
    path_unslash_r(elm_to_remove);
  }
  elm = paths.begin();
  while (elm != paths.end()) {
    elm_next=elm; elm_next++;
    if ((*elm == elm_to_remove) &&
        (pattern_hook == none
         || (pattern_hook==after && elm_last!=paths.end() && *elm_last==hook_pattern)
         || (pattern_hook==before && (elm_next)!= paths.end() && *elm_next==hook_pattern))) {
      elm_found = elm;
    } 
    elm_last = elm;
    elm++;
  } 
  if (elm_found!=paths.end()) paths.erase(elm_found);
}
#else
void pathp_list::remove_last(string elm_to_remove) {
  list<string>::iterator elm;
  list<string>::reverse_iterator elm_last = paths.rend();
  list<string>::reverse_iterator elm_next;
  reverse_iterator<list<string>::iterator> elm_r(elm);
  bool found = false;
  if (purge_trailing_slash) {
    path_unslash_r(elm_to_remove);
  }
  elm_r = paths.rbegin();
  while (elm_r != paths.rend() && !found) {
    elm_next=elm_r; elm_next++;
    cout << *elm_r << endl;
     if ((*elm_r == elm_to_remove) &&
        (pattern_hook == none
         || (pattern_hook==after && elm_next!=paths.rend() && *elm_next==hook_pattern)
         || (pattern_hook==before && (elm_last)!= paths.rbegin() && *elm_last==hook_pattern))) {
      found = true;
    } else {
      elm_last = elm_r;
      elm_r++;
    }
  }
  //elm = static_cast<list<string>::iterator>(elm_r);
  cout << *elm_r << ":" << *elm_r.base()<< endl;
  if (found) paths.erase(elm);
}
#endif

void pathp_list::uniquify(void) {
  list<string> tmp_lst = this->paths;
  set<string> elms_found;
  this->paths.clear();
  list<string>::iterator elm = tmp_lst.begin();
  string elm_maybe_no_slash;

  do {
    elm_maybe_no_slash = path_maybe_unslashed(*elm);
    
    if (elms_found.find(elm_maybe_no_slash) == elms_found.end()) {
      elms_found.insert(elm_maybe_no_slash);
      this->paths.push_back(*elm);
    }
    elm++;
  } while (elm!=tmp_lst.end());
}


void pathp_list::list_elements(void) {
  list<string>::iterator element = paths.begin();
  set<string> elms_found;
  bool elm_repeated;
  do {
    elm_repeated = (elms_found.find(*element)!=elms_found.end()) && use_colour;
    cout << (elm_repeated?"\e[1;31m":"") << *element << (elm_repeated?"\e[0m":"") << endl;
    elms_found.insert(*element);
    element++;
  } while (element!=paths.end());
}

string pathp_list::to_string(void) {
  string retval = "";
  int again = 0;
  list<string>::iterator element = paths.begin();
  while (element!=paths.end()) {
    if (again>0) {
      retval += this->separator;
    } else {
      again++;
    }
    retval += *element ;
    element++;
  }
  return retval;
}

ostream& operator<<(ostream &os, const pathp_list &pplst) {
  pathp_list tmp_pplst = pplst;
  string to_out = tmp_pplst.to_string();
  os << to_out;
  return os;
}

// pathp_list.cc


