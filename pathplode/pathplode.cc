/*! \file pathplode.cc
 * @author    Ulf Klaperski
 * $Id$
 */

#include<iostream>
using namespace std;

#include <getopt.h>
#include "pathp_list.h"



void process_options (int argc, char* argv[]) {
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
    {"separator", 0, 0, 'S'},
    {"unique", 0, 0, 'U'},
    {"preserve-trailing-slash", 0, 0, 'P'},
    {0, 0, 0, 0}
  };

  while (1)
  {
    c = getopt_long (argc, argv, "htup:a:f:l:d:A:B:CESUP",
                     pathplode_options, &option_index);
    if (c<0) break;

    switch (c)
    {
      case 'h':
        cout << "HELP!" << endl;
        break;
      case 'u':
      case 'S':
      case 'A':
      case 'E':
      case 'U':
      case 'B':
      case 'C':
      case 'P':
        cout << c << " UNSUPPORTED!" << endl;
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
  process_options(argc, argv);
  string path_list_in = argv[1]; 
  pathp_list all_paths(path_list_in);
  all_paths.list_elements();
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
}
