// hey emacs, this is a -*- c++ -*- file!
/*! \file pathp_list.cc
 * @author    Ulf Klaperski
 *
 * C++ implementation of pathplode
 *
 * Copyright 2004 by Ulf Klaperski
 *
 * This file is part of pathplode.
 * 
 *##############################################################################
 *
 * Pathplode is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * Pathplode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with the pathplode sources; see the file COPYING.  
 *
 *##############################################################################
 */
// $Id$

#ifndef _PATHP_LIST_H
#define _PATHP_LIST_H 1

#include <string>
#include <list>
#include <iostream>
using namespace std;

enum location_position {
  none,
  before,
  after
};

void bail_out (string message);

class pathp_list {
public:

  // constructor
  pathp_list(string path_string);
  // copy constructor
  pathp_list(const pathp_list& src_pplst);

  void prepend(string new_elm);
  void append(string new_elm);
  void remove_first(string elm_to_remove);
  void remove_last(string elm_to_remove);
  void uniquify(void);
  void list_elements(void);
  string to_string(void);
  static void set_separator(char new_sep) { pathp_list::separator_default=new_sep;}
  void set_hook(string new_hook_pattern, location_position new_pattern_hook);

  inline void set_colour(bool new_val) { use_colour = new_val; }
  
private:

  
  inline void path_unslash_r(string &path) {
    int path_len = path.length();
    if (path_len>1 && path.substr(path_len-1)=="/") path.resize(path_len-1);
  }

  inline string path_maybe_unslashed(string path) {
    if (ignore_trailing_slash) {
      path_unslash_r(path);
    }
    return path;
  }

  list<string> paths;
  char separator;
  string hook_pattern;
  location_position pattern_hook;
  bool use_colour;
  bool ignore_trailing_slash; // in comparisons.. so /bin and /bin/ is the same
  static char separator_default;
  static bool purge_trailing_slash;
};

ostream& operator<<(ostream &os, const pathp_list &pplst);


#endif /* _PATHP_LIST_H */

