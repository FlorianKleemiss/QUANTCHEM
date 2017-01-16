#pragma once
#ifndef __CRYSTAL_H__
#define __CRYSTAL_H__


class MO;
class WFN;

#include <vector>
#include <string>

bool read_crystal_geometry(const std::string &filename, WFN &wave, bool &debug);
bool read_crystal_DM(const std::string &filename, WFN &wave, bool &debug);


#endif