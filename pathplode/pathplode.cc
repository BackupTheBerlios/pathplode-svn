/*! \file pathplode.cc
 * @author    Ulf Klaperski
 *
 * C++ implementation of pathplode
 *
 * Copyright 2003 by Ulf Klaperski
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

enum pp_cmd {
  undefined,
  uniquify,
  remove_first,
  remove_last,
  remove_all,
  append,
  prepend,
  list_path
};


void process_options (int* argc, char** argv[], pp_cmd &command, string &command_arg,
                      pathp_list &pplist) {
  int c;
  int option_index = 0;

  static struct option pathplode_options[] =
  {
    {"help", 0, 0, 'h'},
    {"list", 0, 0, 't'},
    {"uniquify", 0, 0, 'u'},
    {"prepend", 1, 0, 'p'},
    {"append", 1, 0, 'a'},
    {"remove-first", 1, 0, 'f'},
    {"remove-last", 1, 0, 'l'},
    {"remove-all", 1, 0, 'd'},
    {"after", 1, 0, 'A'},
    {"before", 1, 0, 'B'},
    {"color", 0, 0, 'C'},
    {"allow-empty", 0, 0, 'E'},
    {"separator", 1, 0, 'S'},
    {"unique", 0, 0, 'U'},
    {"preserve-trailing-slash", 0, 0, 'P'},
    {0, 0, 0, 0}
  };

  while (1)
  {
    c = getopt_long (*argc, *argv, "htup:a:f:l:d:A:B:CES:UP",
                     pathplode_options, &option_index);
    if (c<0) break;

    switch (c)
    {
      case 'h':
        cout << "HELP!" << endl;
        break;
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
      case 'A':
        pplist.set_hook(optarg, after);
        break;
      case 'E':
        break;
      case 'U':
        break;
      case 'B':
        pplist.set_hook(optarg, before);
        break;
      case 'C':
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
  process_options(&argc, &argv, command, command_arg, all_paths);

  switch (command) {
    case uniquify:
      all_paths.uniquify();
      break;
    case list_path:
      all_paths.list_elements();
      break;
    case append:
      all_paths.append(command_arg);
      break;
    case prepend:
      all_paths.prepend(command_arg);
      break;
    default:
      cout << "Error: Illegal or no command!" << endl;
  }

  if (command != list_path) {
    cout << all_paths << endl;
  }
  

//     cout << "Aufruf des Programms = "
//          << argv[0] << endl;

//     cout << (argc-1)
//          << " weitere Argumente wurden main() übergeben:\n";

//     int i = 1;
//     while(argv[i])
//           cout << argv[i++] << endl;

//     cout << "\n*** Umgebungs-Variablen: ***\n";
//     i = 0;
//     while(env[i])
//           cout << env[i++] << endl;
  return 0;
}
