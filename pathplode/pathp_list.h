// hey emacs, this is a -*- c++ -*- file!
//
// $Id: $

#ifndef _PATHP_LIST_H
#define _PATHP_LIST_H 1

#include <string>
#include <list>
using namespace std;

class pathp_list {
public:
  pathp_list(string path_string);
  void list_elements(void);
  static void set_separator(char new_sep) { pathp_list::separator_default=new_sep;}
private:
  list<string> paths;
  char separator;
  static char separator_default;
};


#endif /* _PATHP_LIST_H */

