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
private:
  list<string> paths;
};


#endif /* _PATHP_LIST_H */

