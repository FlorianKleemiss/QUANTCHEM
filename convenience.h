#ifndef __CONVENIENCE_H__
#define __CONVENIENCE_H__

#include <iostream>
#include <string>
#include <sys/stat.h>

class WFN;

//------------------general functions for easy use of terminal input--------------------
bool yesno();
void Enter();
void cls();
inline bool exists(const std::string &name){
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
};
std::string atnr2letter(const int nr);
void copy_file(std::string from, std::string to);
//------------------Functions to handle .wfn files--------------------------------------
bool writewfn(WFN &wavefunction, bool debug);
bool readwfn(WFN &wavefunction, bool debug);
//------------------Functions to work with configuration files--------------------------
void write_template_confi();
bool program_confi(std::string &gaussian_path, std::string &turbomole_path, 
                   std::string &checkpoint_path, std::string &basis, bool debug);
bool check_bohr(WFN &wave, bool debug);

#include "wfn_class.h"

#endif