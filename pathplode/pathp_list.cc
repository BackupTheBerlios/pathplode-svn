//
// $Id:$


// file
//#include <config.h>
#include "pathp_list.h"
#include <iostream>
#include <set>
using namespace std;

char pathp_list::separator_default = ':';

pathp_list::pathp_list(string path_string) {
  int sep_pos;
  separator = separator_default;
  string path_element;
  do {
    sep_pos = path_string.find(separator);
    path_element = path_string.substr(0, sep_pos);
    paths.push_back(path_element);
    path_string.erase(0, sep_pos+1);
  } while (sep_pos!=-1);
           
}

pathp_list::pathp_list(const pathp_list& src_pplst) {
  separator = src_pplst.separator;
  paths = src_pplst.paths;
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

// pathp_list.cc


