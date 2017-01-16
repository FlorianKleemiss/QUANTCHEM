
#ifndef FCHK_H__
#define FCHK_H__

class WFN;

bool chk2fchk(const std::string &outputname, const std::string &chk_path, 
              const std::string &gaussian_path, const int chk_number);

bool gaussian(const std::string &programPath);

bool gaussian();

int prepare_gaussian(const std::string &basis_set_path, const std::string &fchkname, 
                     const std::string &checkpoint_path, WFN &wave, bool debug);

bool new_fchk(const std::string &basis_set_path, const std::string &fchkname, 
              const std::string &checkpoint_path, const std::string &gaussian_path, 
              WFN &wave, bool debug);

bool modify_fchk(const std::string &fchk_name, const std::string &basis_set_path, WFN &wave, bool debug, const bool &read);

#include "wfn_class.h"

#endif