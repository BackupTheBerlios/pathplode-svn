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
#include <queue>
#include "pathp_list.h"


#define VERSION "0.6"

#define USAGE "\n\
Usage: pathplode [options] <command> [options] <command> ... PATH\n\
Modify the incoming path as specified by the commands and options and print it to standard output.\n\
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
Local options:  \n\
  --reset-options(-r): reset options to their default values for the following commands.\n\
  --after(-A) <anchor>: modify append/prepend to insert new element after\n\
        the given element. Note that prepend inserts\n\
        after the first matching element, append after the last\n\
        matching element.\n\
  --before(-B) <anchor>: modify append/prepend to insert new element before\n\
         the given element. Note that prepend inserts\n\
         before the first matching element, append before the last\n\
         matching element.\n\
  --allow-empty(-E): Allow empty entries. By default they are removed.\n\
  --colour(-C): colorize output - currently only affects the list command.\n\
  --regex(-R): Use regular expressions. All hooks and elements to remove are treated\n\
         as regular expressions (GNU regex syntax).\n\
Global options:  \n\
  --preserve-trailing-slash(-P): by default a trailing slash is removed. This changes\n\
           the behaviour to keep those slashes.\n\
  --separator(-S): The separator between path elements. Default: ':'\n\
  --unique(-U): calls a uniquify at the beginning and at the end, just a shortcut for\n\
           putting -u twice. This ensures matching at the right place and leaving a\n\
           uniquified path.\n\
NOTE:\n\
  The order of options matters! Every option affects only subsequent commands, until\n\
  it is changed again. To remove all option use the --reset-options option.\n\
Examples:\n\
  pathplode --uniquify ~/gnu/bin:/usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games:/usr/local/bin\n\
      -> ~/gnu/bin:/usr/local/bin:/usr/bin:/bin:/usr/bin/X11:/usr/games\n\
  pathplode --unique --prepend ~/gnu/bin /usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games\n\
      -> ~/gnu/bin:/usr/local/bin:/usr/bin:/bin:/usr/bin/X11:/usr/games\n\
  pathplode --append ~/gnu/bin /usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games\n\
      -> /usr/local/bin:/usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games:~/gnu/bin\n\
  pathplode --after ~/gnu/bin --prepend ~/bin /usr/bin:/bin:~/gnu/bin:/usr/bin/X11:/usr/games:~/gnu/bin\n\
      -> /usr/bin:/bin:~/gnu/bin:~/bin:/usr/bin/X11:/usr/games:~/gnu/bin\n\
  pathplode --regex --after 'X1[12]' --prepend ~/new /usr/bin:/bin:/usr/bin/X12:~/gnu/bin:/usr/bin/X11:/usr/games\n\
      -> /usr/bin:/bin:/usr/bin/X12:~/new:~/gnu/bin:/usr/bin/X11:/usr/games\n";


#define COPYING "\n\
Copyright (C) 2004, 2005 Ulf Klaperski.\n\
Pathplode comes with ABSOLUTELY NO WARRANTY.\n\
You may redistribute copies of Pathplode\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the file named COPYING.";

static bool unique_opt = false;
static bool list_paths_occured = false;

enum pp_cmd {
  undefined,
  uniquify,
  remove_first,
  remove_last,
  remove_all,
  append,
  prepend,
  list_paths,
  before_elm,
  after_elm,
  colour,
  reset_options,
  use_regex
};

string version_string = string("Pathplode, version ") + VERSION;
const char usage_string[] = USAGE;
const char copying_string[] = COPYING;

// just a helper class to store command and argument in a list
class cmd_with_arg {
public:
  cmd_with_arg(pp_cmd command, string argument) {
    cmd=command;
    arg=argument;
  }
  pp_cmd cmd;
  string arg;
};

static queue<cmd_with_arg> commands;

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
    {"reset-options", 0, 0, 'r'},
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
    {"regex", 0, 0, 'R'},
    {0, 0, 0, 0}
  };

  while (1)
  {
    c = getopt_long (*argc, *argv, "hvtup:a:f:l:d:A:B:CrRES:UP",
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
        commands.push(cmd_with_arg(uniquify, ""));
        break;
      case 'p':
        commands.push(cmd_with_arg(prepend, optarg));
        command = prepend;
        command_arg = optarg;
        break;
      case 'a':
        commands.push(cmd_with_arg(append, optarg));
        break;
      case 'f':
        commands.push(cmd_with_arg(remove_first, optarg));
        break;
      case 'l':
        commands.push(cmd_with_arg(remove_last, optarg));
        break;
      case 'd':
        commands.push(cmd_with_arg(remove_all, optarg));
        break;
      case 't':
        commands.push(cmd_with_arg(list_paths, optarg));
        break;
      case 'A':
        commands.push(cmd_with_arg(after_elm, optarg));
        break;
      case 'E':
        pplist.set_allow_empty(true);
        break;
      case 'U':
        unique_opt = true;
        break;
      case 'B':
        commands.push(cmd_with_arg(before_elm, optarg));
        break;
      case 'R':
        commands.push(cmd_with_arg(use_regex, ""));
        break;
      case 'r':
        commands.push(cmd_with_arg(reset_options, ""));
        break;
      case 'C':
        commands.push(cmd_with_arg(colour, optarg));
        break;
      case 'P':
        pplist.set_purge_trailing_slash(false);
        break;
      case 'S':
        pplist.set_separator(optarg[0]);
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
  
  string path_list_in; 
  pathp_list all_paths("");
  int error_occured = 0;
  process_options(&argc, &argv, command, command_arg, all_paths);
  if (argc>0) path_list_in = argv[argc-1];
  all_paths.set_list_from_string(path_list_in);

  if (unique_opt) error_occured |= all_paths.uniquify();

  while(!commands.empty()) {
    cmd_with_arg &next_command = commands.front();
    switch (next_command.cmd) {
    case uniquify:
      error_occured |= all_paths.uniquify();
      break;
    case list_paths:
      error_occured |= all_paths.list_elements();
      list_paths_occured = true;
      break;
    case append:
      error_occured |= all_paths.append(next_command.arg);
      break;
    case prepend:
      error_occured |= all_paths.prepend(next_command.arg);
      break;
    case remove_first:
      error_occured |= all_paths.remove_first(next_command.arg);
      break;
    case remove_last:
      error_occured |= all_paths.remove_last(next_command.arg);
      break;
    case remove_all:
      error_occured |= all_paths.remove_all(next_command.arg);
      break;
    case before_elm:
        all_paths.set_hook(next_command.arg, before);
      break;
    case after_elm:
      all_paths.set_hook(next_command.arg, after);
      break;
    case use_regex:
      all_paths.set_use_regex(true);
      break;
    case reset_options:
      all_paths.clear_hook();
      all_paths.set_colour(false);
      all_paths.set_use_regex(false);
      break;
    case colour:
        all_paths.set_colour(true);
      break;
    default:
      cout << "Error: Illegal or no command!" << endl;
    }
    commands.pop();
  }

  if (unique_opt) error_occured |= all_paths.uniquify();

  if (!list_paths_occured) {
    cout << all_paths << endl;
  }
  
  return error_occured;
}
