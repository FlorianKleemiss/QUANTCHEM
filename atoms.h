#pragma once
#ifndef __ATOMS_H__
#define __ATOMS_H__

#include <iostream>
#include <vector>
#include <string>

//-----------------Definition of atoms and basis sets--------------------

struct basis_set_entry{
	double coefficient;
	double exponent;
	unsigned int type;
	unsigned int shell;
	basis_set_entry operator=(const basis_set_entry &rhs);
	basis_set_entry();
	basis_set_entry(double g_coefficient, double g_exponent, unsigned int g_type, unsigned int g_shell);
};

inline basis_set_entry::basis_set_entry(){
	coefficient=0.0;
	exponent=0.0;
	type=0;
	shell=0;
};

inline basis_set_entry::basis_set_entry(double g_coefficient, double g_exponent, unsigned int g_type, unsigned int g_shell){
	coefficient=g_coefficient;
	exponent=g_exponent;
	type=g_type;
	shell=g_shell;
};

inline basis_set_entry basis_set_entry::operator=(const basis_set_entry &rhs){
	coefficient = rhs.coefficient;
	exponent = rhs.exponent;
	type = rhs.type;
};

struct atom {
	std::string label;
	int nr;
	double x;
	double y;
	double z;
	int charge;
	atom();
	atom(std::string l, int n, double c1, double c2, double c3, int ch);
	atom operator=(const atom &rhs);
	void print_values();
	bool push_back_basis_set(double coefficient, double exponent, int type, int shell, bool debug);
	void print_values_long();
	bool get_basis_set_loaded();
	std::vector<basis_set_entry> basis_set;
	std::vector<int> shellcount;
};

inline atom::atom(){
    label='?';
	nr=0;
	x=0.0;
	y=0.0;
	z=0.0;
	charge=0;
};

inline atom::atom (std::string l, int n, double c1, double c2, double c3, int ch){
	nr=n;
	label=l;
	x=c1;
	y=c2;
	z=c3;
	charge=ch;
};

inline atom atom::operator= (const atom &rhs){
	label=rhs.label;
	nr=rhs.nr;
	x=rhs.x;
	y=rhs.y;
	z=rhs.z;
	charge=rhs.charge;
	basis_set=rhs.basis_set;
	shellcount=rhs.shellcount;
};

inline void atom::print_values(){
	std::cout << "nr: " << nr << " label: " << label << " x: " << x << " y: " << y << " z: " << z << " charge: " << charge << std::endl;
};

inline void atom::print_values_long(){
	std::cout << "nr: " << nr << " label: " << label << " x: " << x << " y: " << y << " z: " << z << " charge: " << charge << std::endl;
	if(basis_set.size()>0){
		for (int i=0; i<basis_set.size(); i++){
			std::cout << "basis set entry " << i << ": expo: " << basis_set[i].exponent << " coef: " << basis_set[i].coefficient << " type: " << basis_set[i].type << " shell: " << basis_set[i].shell << std::endl;
		}
	}
	if(shellcount.size()>0){
		for (int i=0; i<shellcount.size(); i++){
			std::cout << "shellcount " << i << ": value: " << shellcount[i] << std::endl;
		}
	}
};

inline bool atom::push_back_basis_set(double exponent, double coefficient, int type, int shell, bool debug){
	if(shell == shellcount.size()){
		shellcount.push_back(1);
		if(debug) std::cout << "added a new shell"<< std::endl;
	}
	else{
		shellcount[shell]++;
		if(debug) std::cout << "increased the size of the shell" << std::endl << "The size is now: " << shellcount[shell] << std::endl;
	}
	if( type >= 0 && type < 5 && shell>=0){
		basis_set.push_back(basis_set_entry(coefficient, exponent, type, shell));
		return true;
	}
	else{
		std::cout << "This is not a valid basis set entry!" << std::endl;
		std::cout << "Exponent: " << exponent << " coefficient: "<< coefficient << " type: " << type << " shell: " << shell << std::endl;
		return false;
	}
};

inline bool atom::get_basis_set_loaded(){
	if(basis_set.size()>0) return true;
	else return false;
};

#endif
