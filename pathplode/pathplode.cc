/*! \file pathplode.cc
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
 *
 * $Id$
 */

#include<iostream>
using namespace std;

extern char *optarg;
#include <getopt.h>
#include "pathp_list.h"

static bool uniquify_before = false;
static bool uniquify_after = false;

enum pp_cmd {
  undefined,
  uniquify,
  remove_first,
  remove_last,
  remove_all,
  append,
  prepend,
  list_paths
};

#define VERSION "0.1"

#define USAGE "\n\
Usage: pathplode [options] <command> PATH\n\
Modify the incoming path as specified by command and options and give it to standard output.\n\
Information:\n\
  --help (-h)\n\
  --version (-v)\n\
Commands:\n\
  --append(-a) <element>: put element at end of list\n\
  --prepend(-p) <element>: put element at beginning of list\n\
  --remove-first(-f) <element>: remove the first matching element \n\
  --remove-last(-l) <element>: remove the last matching element \n\
  --remove-all(-d) <element>: remove all occurrences of element\n\
  --uniquify(-u): remove all multiple entries, just leaving the first one \n\
  --list(-t): List all path entries, each on its own line\n\
Options:  \n\
  --unique(-U): remove multiple entries. Only the entry with the\n\
          highest priority is left over.\n\
  --after(-A) <anchor>: modify append/prepend to insert new element after\n\
        the given element. Note that prepend inserts\n\
        after the first matching element, append after the last\n\
        matching element.\n\
  --before(-B) <anchor>: modify append/prepend to insert new element before\n\
         the given element. Note that prepend inserts\n\
         before the first matching element, append before the last\n\
         matching element.\n\
  --separator(-S): The separator between path elements. Default: ':'\n\
  --preserve-trailing-slash(-P): by default a trailing slash is removed. This changes\n\
           the behaviour to keep those slashes.\n\
  --allow-empty(-E): Allow empty entries. By default they are removed.\n\
  --colour(-C): colorize output - currently only affects the list command.\n\
Examples:\n\
  pathplode --uniquify ~/gnu/bin:/usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games:/usr/local/bin\n\
      -> ~/gnu/bin:/usr/local/bin:/usr/bin:/bin:/usr/bin/X11:/usr/games\n\
  pathplode --unique --prepend ~/gnu/bin /usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games\n\
      -> ~/gnu/bin:/usr/local/bin:/usr/bin:/bin:/usr/bin/X11:/usr/games\n\
  pathplode --append ~/gnu/bin /usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games\n\
      -> /usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games:~/gnu/bin\n\
  pathplode --prepend ~/bin --after ~/gnu/bin /usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games:~/gnu/bin\n\
      -> /usr/bin:/bin:~/gnu/bin:~/bin:/usr/bin/X11:/usr/games:~/gnu/bin";

#define COPYING "\n\
Copyright (C) 2004 Ulf Klaperski.\n\
Pathplode comes with ABSOLUTELY NO WARRANTY.\n\
You may redistribute copies of Pathplode\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the file named COPYING.";

string version_string = string("Pathplode, version ") + VERSION;
const char usage_string[] = USAGE;
const char copying_string[] = COPYING;

void process_options (int* argc, char** argv[], pp_cmd &command, string &command_arg,
                      pathp_list &pplist) {
  int c;
  int option_index = 0;

  static struct option pathplode_options[] =
  {
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {"list", 0, 0, 't'},
    {"uniquify", 0, 0, 'u'},
    {"prepend", 1, 0, 'p'},
    {"append", 1, 0, 'a'},
    {"remove-first", 1, 0, 'f'},
    {"remove-last", 1, 0, 'l'},
    {"remove-all", 1, 0, 'd'},
    {"after", 1, 0, 'A'},
    {"before", 1, 0, 'B'},
    {"colour", 0, 0, 'C'},
    {"allow-empty", 0, 0, 'E'},
    {"separator", 1, 0, 'S'},
    {"unique", 0, 0, 'U'},
    {"preserve-trailing-slash", 0, 0, 'P'},
    {0, 0, 0, 0}
  };

  while (1)
  {
    c = getopt_long (*argc, *argv, "hvtup:a:f:l:d:A:B:CES:UP",
                     pathplode_options, &option_index);
    if (c<0) break;

    switch (c)
    {
      case 'h':
        cout << version_string << endl;
        cout << usage_string << endl;
        cout << copying_string << endl;
        exit(0);
      case 'v':
        cout << version_string << endl;
        cout << copying_string << endl;
        exit(0);
      case 'u':
        command = uniquify;
        break;
      case 'p':
        command = prepend;
        command_arg = optarg;
        break;
      case 'a':
        command = append;
        command_arg = optarg;
        break;
      case 'f':
        command = remove_first;
        command_arg = optarg;
        break;
      case 'l':
        command = remove_last;
        command_arg = optarg;
        break;
      case 'd':
        command = remove_all;
        command_arg = optarg;
        break;
      case 't':
        command = list_paths;
        break;
      case 'A':
        pplist.set_hook(optarg, after);
        break;
      case 'E':
        break;
      case 'U':
        uniquify_before = true;
        uniquify_after = true;
        break;
      case 'B':
        pplist.set_hook(optarg, before);
        break;
      case 'C':
        pplist.set_colour(true);
        break;
      case 'P':
        cout << c << " UNSUPPORTED!" << endl;
        break;
      case 'S':
        pathp_list::set_separator(optarg[0]);
        break;
        
      default:
        cout << c << " HHHHMMM..." << endl;
    }
  }
}

/*!  \fn int main(int argc, char* argv[], char *env[]) 
 * \brief Anzeige der Aufrufparameter des Programms und der Umgebungsvariablen.

 * Die folgenden Parameter werden vom Betriebsystem übergeben:
  \param argc  Anzahl der Argumente
  \param argv[] C-String-Array der weiteren Argumente
  \param env[] C-String-Array der Umgebungsvariablen
*/
int main(int argc, char* argv[], char *env[]) {
  pp_cmd command = undefined;
  string command_arg = "";
  
  string path_list_in(argv[argc-1]); 
  pathp_list all_paths(path_list_in);
  int error_occured = 0;
  process_options(&argc, &argv, command, command_arg, all_paths);

  if (uniquify_before) error_occured |= all_paths.uniquify();
  switch (command) {
    case uniquify:
      error_occured |= all_paths.uniquify();
      break;
    case list_paths:
      error_occured |= all_paths.list_elements();
      break;
    case append:
      error_occured |= all_paths.append(command_arg);
      break;
    case prepend:
      error_occured |= all_paths.prepend(command_arg);
      break;
    case remove_first:
      error_occured |= all_paths.remove_first(command_arg);
      break;
    case remove_last:
      error_occured |= all_paths.remove_last(command_arg);
      break;
    case remove_all:
      error_occured |= all_paths.remove_all(command_arg);
      break;
    default:
      cout << "Error: Illegal or no command!" << endl;
  }
  if (uniquify_after) error_occured |= all_paths.uniquify();

  if (command != list_paths) {
    cout << all_paths << endl;
  }
  
  return error_occured;
}
