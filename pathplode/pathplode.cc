/*! \file pathplode.cc
 * @author    Ulf Klaperski
 * $Id:  $
 */

#include<iostream>
using namespace std;

#include <getopt.h>
#include "ixpath.h"



void process_options (int argc, char* argv[]) {
}

/*!  \fn int main(int argc, char* argv[], char *env[]) 
 * \brief Anzeige der Aufrufparameter des Programms und der Umgebungsvariablen.

 * Die folgenden Parameter werden vom Betriebsystem übergeben:
  \param argc  Anzahl der Argumente
  \param argv[] C-String-Array der weiteren Argumente
  \param env[] C-String-Array der Umgebungsvariablen
*/
int main(int argc, char* argv[], char *env[]) {
    cout << "Aufruf des Programms = "
         << argv[0] << endl;

    cout << (argc-1)
         << " weitere Argumente wurden main() übergeben:\n";

    int i = 1;
    while(argv[i])
          cout << argv[i++] << endl;

    cout << "\n*** Umgebungs-Variablen: ***\n";
    i = 0;
    while(env[i])
          cout << env[i++] << endl;
}
