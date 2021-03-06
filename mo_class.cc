#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "mo_class.h"
#include "convenience.h"

using namespace std;

MO::MO(){
	nr=0;
	occ=0.0;
	ener=0.0;
};

MO::MO(int number, float occupation, double energy){
	nr=number;
	occ=occupation;
	ener=energy;
};

bool debug_mo=true;

bool MO::push_back_coef(double val, const int nex){
    coefficients.push_back(val);

    // consistencycheck with WFN
	
    if( nex < coefficients.size()||coefficients.size()>nex ){
		if(debug_mo) cout << nex << " vs. " << coefficients.size() << endl; 
		return false;
	}
	return true;
};

bool MO::erase_coef(int nr, const int nex){
        // Check, if Element (nr) is a valid Vector-Entry
	if (nr-1 <0) return false;
	if (nr-1 >= coefficients.size() ) return false;
        

        // consistencycheck with WFN
        if( nex != coefficients.size()-1 ) return false;
        

        // delete Vector-Entry and rearrange
        coefficients.erase(coefficients.begin()+(nr-1));
        
	return true;
};

void MO::change_coefficient(int nr){


        // Check, if Element (nr) is a valid Vector-Entry
	if (nr-1 <0) return ;
	if (nr-1 >= coefficients.size() ) return ;


	bool end=false;
	while (!end){
		cout << "What is the value for the new exponent?\n";
		double temp_coef=0.0;
		cin >> temp_coef;
		if(temp_coef>1000||temp_coef<-1000){
			cout << "That's suspiciusly big for a MO-coefficient, please select something smaler...\n";
			continue;
		}
		coefficients[nr-1]=temp_coef;
		end=true;
	}
	cls();
};

bool MO::change_coefficient(int nr, double value, bool debug){
        // Check, if Element (nr) is a valid Vector-Entry
	if (nr <0){
		cout << "nr below 0! nr:" << nr << endl;
		return false;
	}
	if (nr >= coefficients.size() ) {
		cout << "nr above size of MO! nr:" << nr << " size: " << coefficients.size() << endl;
		return false;
	}
	coefficients[nr]=value;
	if(debug) cout <<  "coefficients[nr]: " << coefficients[nr] << "[" << nr << "]" << endl;
	return true;
};

string MO::hdr(){
	string temp;
	temp = "MO";
	if(nr<10&&nr>0) temp.append("    ");
	else if(nr<100) temp.append("   ");
	else temp.append("  ");
	temp.append(to_string(nr));
	temp.append("    MO 0.0        OCC NO =");
	if(occ==2.0) temp.append("    2.0000000  ORB. ENERGY =");
	else if(occ<2&&occ>0){
		temp.append("    ");
		temp.append(to_string(occ));
		temp.append("  ORB. ENERGY =");
		if(debug_mo) printf("MO occ lower than 2!");
	}
	if(ener<0&&ener>-10){
		temp.append("   ");
		temp.append(to_string(ener));
		temp.append("\n");
	}
	else if(ener<-10&&ener>-100){
		temp.append("  ");
		temp.append(to_string(ener));
		temp.append("\n");
	}
	else if(ener<-100){
		temp.append(" ");
		temp.append(to_string(ener));
		temp.append("\n");
	}
	else if(ener>0){
		temp.append("    ");
		temp.append(to_string(ener));
		temp.append("\n");
	}
	return temp;	
}

double MO::get_coefficient (int nr, bool debug){ 
	if(nr>=coefficients.size()){
		std::cout << "Requested element outside of range!" << endl;
		return -1;
	}
	if(nr<0){
		cout << "Number below zero!" << endl;
		return -1;
	}
	if(debug) cout << "[nr] will return: " << "[" << nr << "] " << coefficients[nr] << endl;
	return coefficients[nr]; 
};