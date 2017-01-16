/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.cc
 * Copyright (C) 2016 Florian Kleemiss <florian.kleemiss@uni-bremen.de>
 * 
 * wfn-cpp is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * wfn-cpp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <limits>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#include <iomanip>
#include <vector>


#include "convenience.h"
#include "fchk.h"
#include "basis_set.h"
#include "crystal.h"

using namespace std;
bool debug_main=false;

int main(int argc, char **argv)
{
	cls();
	if(debug_main) cout << "argc:"<< argc << endl;
	bool end=false;
	vector<WFN> wavy;
	int activewave=0;
	string wavename;
	bool programs=false;
	string gaussian_path;
	string turbomole_path;
	string checkpoint_path;
	string basis_set_path;
	if(!program_confi(gaussian_path, turbomole_path, checkpoint_path, basis_set_path, debug_main)){
		cout << "No programs.confi found, do you want to continue without the employment of external programs?" << endl;
		if(!yesno()){
			cout << "Then please start again and write a config file or use the template choice." << endl;
			return -1;
		}
	}
	else programs=true;
	while(!end){
		char sel;
		cout << "What do you want to do?\n";
		if(wavy.size() > 0) {
			cout << "Active Wavefunction: " << wavy[activewave].get_path();
			if(wavy[activewave].get_modified()) cout << "*";
			cout << endl;
		}
		cout << "W) Read in a wavefunction in .wfn format"<<endl 
			<< "C) Read in CRYSTAL14 outputfile"<<endl;
		if(wavy.size() > 0){
			cout << "M) Modify an already loaded wavefunction"<<endl 
				<< "S) Save the active wavefunction" << endl
				<< "O) Order the exponents in the wavefunction" <<endl
				<< "B) Read a basis set" << endl;
			if(wavy.size() > 1) cout << "A) Activate another wavefunction (Change which one to work with)" <<endl;
		}
		cout << "E) Exit the program"<<endl;
		cin >> sel;
		cls();
		switch(sel){
			case 'W':
			case 'w':{
				while(!end){
					if(wavy.size()!=0) activewave++;
					if(activewave<=wavy.size()&&wavy.size()>0){
						cout << "This will delete the previously loaded wavefunction "<<wavy[activewave].get_path() <<"! Are you sure?";
						if(yesno()) end=true;
						else break;
					}
					else{
						bool b_success=false;
						try {
							wavy.push_back(WFN(2));
							b_success=readwfn(wavy[activewave],debug_main);
						}
						catch (int e){
								cout << "Sorry, an error occured.\nError number: "<< e << "\nLet's try this again if you wish..."<< endl;
						}
						if(!b_success){
							Enter();
							cls();
						}
						else {
							if(debug_main) Enter();
							cls();
						}	   
						if(debug_main) cout << wavy[activewave].get_ncen () << endl;
						break;
					}
				}
				cls();
				end=false;
				break;
			}
			case 'C':
			case 'c':
				cout << "!!!!!!!!!!PLEASE ONLY TRY READING A FILE WITH MOLECULE KEYWORD CONTAINING ONLY THE FIRST ONE DENSITY MATRIX!!!!!!!!!" << endl;
				while(!end){
					if(wavy.size()!=0) activewave++;
					if(activewave<=wavy.size()&&wavy.size()>0){
						cout << "This will delete the previously loaded wavefunction "<<wavy[activewave].get_path() <<"! Are you sure?";
						if(yesno()) end=true;
						else break;
					}
					else{
						wavy.push_back(WFN(1));
						string path;
						cout << "Path to the .out file: ";
						cin >> path;
						int tries=0;
						if(path.find(".out")==-1){
							do {
								cout << "This doesn't look like a .out file, try again: " << endl;
								tries++;
								cin >> path;
							} while (path.find(".out")==-1&&tries<3);
							if(tries == 3) break;
						}
						//-----read the geometry from the crystal file
						if(!read_crystal_geometry (path, wavy[activewave],debug_main)){
							cout << "ERROR in reading geometry from CRYSTAL file!" << endl;
							Enter();
							cls();
							break;
						}
						else {
							wavy[activewave].make_path(path);
							if(debug_main) Enter();
							cls();
							//------------------load the basis set used 
							if(!read_basis_set_vanilla (basis_set_path,wavy[activewave],debug_main)){
								cout << "Something went wrong during the reading of the basis set!!" << endl;
								Enter();
								cls();
								break;
							}
							else{
								//-------------read the DM
								if(!read_crystal_DM(path, wavy[activewave],debug_main)){
									cout << "Something went wrong while reading the DM from the CRYSTAL file!" << endl;
									Enter();
									cls();
									break;
								}
							}
							end=true;
						}	   
						if(debug_main) cout << wavy[activewave].get_ncen () << endl;
					}
				}
				cls();
				end=false;
				break;
			case 'M':
			case 'm':{
				if(wavy.size()<1){
					cout << "First you need to read a wavefunction!" << endl;
					break;
				}
				int msel=0;
				string label="?";
				float x,y,z=0.0;
				int charge=0;
				//cout << "Sorry, not fully implemented yet, see how far you come :P" << endl;
				cout << "What do you want to do?"<<endl
					<< "D) Delete a center or set of values for a certain function"<<endl
					<< "A) Add a center" << endl
					<< "C) Change a value" << endl
					<< "E) Exit to Main Menu" << endl;
				cin >> sel;
				cls();
				switch(sel){
					case 'D':
					case 'd':
						cout << "A) Atom"<< endl
							<< "E) Exponent" << endl
							<< "B) Back to main menu"<<endl;
						cin >> sel;
						switch(sel){
							end=false;
							case 'A':
							case 'a':
								cout << "The list of centers:\n";
								wavy[activewave].list_centers();
								cout << "Which center do you want to delete?\n";
								cin >> msel;
								if (msel>=0){
									if(wavy[activewave].remove_center(msel)){
										cout << "Deleted center nr " << msel << " succesfully! Going back to Main Menu.\n";
										wavy[activewave].set_modified ();
									}
									else cout << "Something went wrong... Sorry, let's start again...\n";
								}
								else cout << "Wrong selection, start again!\n";
								break;
							case 'E':
							case 'e':
								while(!end){
									wavy[activewave].list_primitives ();
									cout << "Which exponent out of "<< wavy[activewave].get_nex() << " do you want to delete?\n";
									cin >> msel;
									if(msel<0||msel>wavy[activewave].get_nex()){
										cout << "Sorry, wrong input";
										continue;
									}
									cout << "This is the set of information you requested:\n";
									wavy[activewave].print_primitive(msel);
									end=true;
								}
								break;
							case 'B':
							case 'b':
								cls();
								break;
						}
						break;
					case 'A':
					case 'a':
						cout << "A) Atom\nE) Exponent\nB) Back to main menu" << endl;
						cin >> sel;
						switch(sel){
							end=false;
							case 'E':
							case 'e':
								while(!end){
									cout << "Please remember that this exponent will be appended to the data structure, "
										<< "it will not be sorted in any way!\n Centre Assignement: ";
									int temp_cen=0;
									cin >> temp_cen;
									if (temp_cen>wavy[activewave].get_ncen ()||temp_cen<0){
										cout << "Wrong input, start over!";
										continue;
									}
									cout << "Type: ";
									int temp_type=0;
									cin >> temp_type;
									cout << "Exponent: ";
									double temp_exp=0.0;
									cin >> temp_exp;
									double temp_val[wavy[activewave].get_nmo()];
									for (int i=0; i<wavy[activewave].get_nmo();i++){
										cout << "Enter coefficient for MO " << i << ":";
										cin >> temp_val[i];
										if(temp_val[i]<-1000||temp_val[i]>1000){
											cout << "Wrong input, please try again...\n";
											i--;
											continue;
										}
									}   
									cout << "Let me recapitulate: Center " << temp_cen << " type: " << temp_type << " exp: " << temp_exp
										<< " and the MO coefficients:\n";
									for(int i=0; i<wavy[activewave].get_nmo(); i++){
										cout << temp_val[i] <<"   ";
										if( i%5==0) cout << endl;
									}
									bool end2=false;
									while(!end2) {
										cout << "is this correct?";
										if(yesno()) end2=true;
									}
								}
								break;
							case 'A':
							case 'a':
								while(!end){
									cout << "The list of centers:\n";
									wavy[activewave].list_centers();
									cout << "Which center do you want to add?\nlabel:";
									cin >> label;
									cout << "x: ";
									cin >> x;
									if(x < -99.999 || x > 99.999){
										cout << "Sorry, number too large\n";
										continue;
									}   
									cout << "y: ";
									cin >> y;
									if(y < -99.999 || y > 99.999){
										cout << "Sorry, number too large\n";
										continue;
									}
									cout << "z: ";
									cin >> z;
									if(z < -99.999 || z > 99.999){
										cout << "Sorry, number too large\n";
										continue;
									}
									cout << "charge: ";
									cin >> charge;
									if(charge <= 0 || charge > 118){
										cout << "Sorry, that atom is not yet disovered\n";
										continue;
									}
									wavy[activewave].push_back_atom(label, x, y, z, charge);
								}
								break;
							case 'B':
							case 'b':
								cls();
								break;
						}
						break;
					case 'C':
					case 'c':
						if(wavy.size() <1) continue;
						cout << "The center/type/exponent status until now is:\n";
						wavy[activewave].list_primitives ();
						cout << "what do you want to change?\nC) Center Assignement\nT) Type assignement\nE) Exponent\nM) MO coefficient\nQ) Quit\n";
						cin >> sel;
						switch (sel){
							case 'Q':
							case 'q':
								cls();
								break;
							case 'C':
							case 'c':
								cout << "Which one do you want to change? (0=return to menu)";
								cin >> msel;
								if(msel>wavy[activewave].get_nex()||msel<0){ 
									cout << "Wrong input, start again!\n";
									break;
								}
								else if(msel==0) break;
								wavy[activewave].change_center (msel);
								break;
							case 'E':
							case 'e':
								cout << "What is the nr. of the exponent you want to change? (0=return to menu)" << endl;
								cin >> msel;
								if(msel>wavy[activewave].get_nex()||msel<0){ 
									cout << "Wrong input, start again!\n";
									break;
								}
								else if(msel==0) break;
								wavy[activewave].change_exponent (msel);
								break;
							case 'T':
							case 't':
								cout << "What is the nr. of the type you want to change? (0=return to menu)" << endl;
								cin >> msel;
								if(msel>wavy[activewave].get_nex()||msel<0){ 
									cout << "Wrong input, start again!\n";
									break;
								}
								else if(msel==0) break;
								wavy[activewave].change_type (msel);
								break;
							case 'M':
							case 'm':
								bool end=false;
								while(!end){
									cout << "Which MO out of " << wavy[activewave].get_nmo() << " MOs? (0=return to menu)\n";
									int MOsel;
									cin >> MOsel;
									if(MOsel>wavy[activewave].get_nmo()||MOsel<0){
										cout << "This is not a valid choice...\n";
										continue;
									}
									else if(MOsel==0) break;
									else{
										wavy[activewave].change_MO_coef(MOsel);
										Enter();
										end=true;
									}
								}
							break;
						}
					case 'E':
					case 'e':
						break;
				}
				break;
			}
			case 'S':
			case 's':{
				if(wavy.size()<1){
					cls();
					continue;
				}
				cout << "Which format do you want to save the wavefunction in?" << endl
					<< "W) WFN format" << endl
					<< "F) Fchk format (this might take a few minutes)" << endl
					<< "C) CRYSTAL 14 format (not implemented yet)" << endl;
				cin >> sel;
				switch(sel){
					case 'W':
					case 'w':{
						bool b_success=false;
						try {
							b_success=writewfn(wavy[activewave],debug_main);
						}
						catch (int e){
							cout << "Sorry, an error occured.\nError number: "<< e << "\nLet's try this again if you wish..."<< endl;
						}
						if(!b_success){
							Enter();
							cls();
						}
						else {
							if(debug_main) Enter();
							cls();
							cout << "Wrote Wavefunction!\n";
						}
						break;
					}
					case 'F':
					case 'f':{
						//debug_main=true;
						string outputname=wavy[activewave].get_path();
						size_t where;
						if(wavy[activewave].get_origin()==2) where = outputname.find(".wfn");
						else if(wavy[activewave].get_origin()==1) where = outputname.find(".out");
						if(where==outputname.length()){
							cout << "Could not make an output file name for your wavefunction path, please enter it manually:" << endl;
							cin >> outputname;
						}
						else{
							outputname.erase(where,4);
						}
						string basis_temp = wavy[activewave].get_basis_set_name ();
						if(basis_temp.length()<3){
							int tries=0;
							bool end=false;
							string temp;
							while(!end&&tries!=3){
								cout << "Please give the name of the basis set you want to use: ";
								cin >> temp;
								string basis_set_file(basis_set_path);
								basis_set_file.append(temp);
								if(debug_main) cout << "looking for: " << basis_set_file << endl;
								if(exists(basis_set_file)) end=true;
								else tries++;
							}
							if(tries==3){
								cout << "Sorry, this takes too long... please make sure you know what you want and try again!" << endl;
								Enter();
								break;
							}
							wavy[activewave].change_basis_set_name(temp);
						}
						string temp_check_exist;
						temp_check_exist=outputname;
						temp_check_exist.append(".fchk");
						bool read=false;
						if(!exists (temp_check_exist)){
							if(wavy[activewave].get_charge ()==0){
								cout << "What is the charge of your molecule?" << endl;
								int temp=0;
								cin >> temp;
								wavy[activewave].assign_charge(temp);
							}
							if(wavy[activewave].get_multi()==0){
								cout << "What is the multiplicity of your molecule?" << endl;
								int temp=0;
								cin	 >> temp;
								wavy[activewave].assign_multi(temp);
							}
							if(!new_fchk(basis_set_path,outputname,checkpoint_path,gaussian_path, wavy[activewave],debug_main)){
								cout << "Error during fchk writing!" << endl;
								break;
							}
							else{
								read=false;
								if(debug_main){
									cout << "Seems as if this worked, but check all the reesult file"
										<< " (" << outputname << ".fck) to see if that really worked" 
										<< endl;
									Enter();
								}
								cls();
								cout << "Wrote FCHK!\n";
							}
						}
						else read=true;
						if(debug_main){
							cls();
							cout << "Fchk file from gaussian is there, now for the changes..." << endl;
							Enter();
						}
						//Just for now only for WFN and CRYSTAL origin files
						if(!modify_fchk(outputname,basis_set_path, wavy[activewave], debug_main, read)){
							cout << "Sorry, something went wrong..!" << endl;
							Enter();
						}
						break;
					}
					case 'C':
					case 'c':
						cls();
						cout << "Sorry, not yet implemented\n\n\n";
						break;
					default:{
						cout << "Sorry, i didn't get that, could you try it again?\n";
						Enter();
						cls();
					}
				}
				}
				break;
			case 'O':
			case 'o':
				if(wavy.size()<1){
					cout << "First you need to read a wavefunction!" << endl;
					break;
				}
				if(wavy[activewave].get_origin ()==2){
					cout << "Ordering wavefunction!" << endl;
					wavy[activewave].sort_wfn (wavy[activewave].check_order (debug_main),debug_main);
					if(debug_main) Enter();
					cls();
				}
				else{
					cout << "I can only order .wfn files!" << endl;
					Enter();
				}
				break;
			case 'B':
			case 'b':
				if(wavy.size()<1){
					cout << "First you need to read a wavefunction!" << endl;
					break;
				}
				if(!read_basis_set_vanilla (basis_set_path,wavy[activewave],debug_main)) cout << "Problem during reading of the basis set!" << endl;
				if(debug_main) Enter();
				cls();
				break;
			case 'a':
			case 'A':
				if(wavy.size() <2) {
					cls();
					continue;
				}
				while(!end){
					cout << "Which should be the new active wavefunction?"<<endl;
					for(int i=0;i<wavy.size();i++) cout << wavy[i].get_path() << endl;
					int new_active=0;
					cin >> new_active;
					if(new_active<0||new_active>wavy.size()){
						cout << "invalid selection, try again..."<<endl;
						continue;
					}
					else{
						end=true;
						activewave=new_active;
					}
				}
				break;
			case 'e':
			case 'E':
				end=true;
				break;
			case 'd':
			case 'D':
				debug_main=true;
				cls();
				break;
			default:
				cout << "Sorry, i didn't get that, could you try it again?\n";
				Enter();
				cls();
				cout << "Sorry, i didn't get that, could you try it again?\n";
				break;
		}
	}
	cls();
	return 0;
}

