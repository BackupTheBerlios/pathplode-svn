//
// $Id:$


// file
//#include <config.h>
#include "pathp_list.h"
#include<iostream>
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

void pathp_list::list_elements(void) {
  list<string>::iterator element = paths.begin();
  do {
    cout << *element << endl;
    element++;
  } while (element!=paths.end());
}

// pathp_list.cc


