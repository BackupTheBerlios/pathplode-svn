// hey emacs, this is a -*- c++ -*- file!
//
// $Id$

#ifndef _PATHP_LIST_H
#define _PATHP_LIST_H 1

#include <string>
#include <list>
using namespace std;

enum location_position {
  none,
  before,
  after
};

class pathp_list {
public:


  // constructor
  pathp_list(string path_string);
  // copy constructor
  pathp_list(const pathp_list& src_pplst);
  //

  //
  void prepend(string new_elm);
  void append(string new_elm);
  void uniquify(void);
  void list_elements(void);
  string to_string(void);
  static void set_separator(char new_sep) { pathp_list::separator_default=new_sep;}
  void set_hook(string new_hook_pattern, location_position new_pattern_hook);

private:
  list<string> paths;
  char separator;
  string hook_pattern;
  location_position pattern_hook;
  static char separator_default;
};

ostream& operator<<(ostream &os, const pathp_list &pplst);


#endif /* _PATHP_LIST_H */

