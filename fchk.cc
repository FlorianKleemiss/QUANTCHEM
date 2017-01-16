#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <math.h>
#include <sstream>

#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <sys/types.h>
#include <vector>

#include "fchk.h"
#include "convenience.h"
#include "basis_set.h"
using namespace std;
bool debug_fchk=false;
//----------------------------FCHK Preparation and Gaussian--------------------------------------

bool chk2fchk(const string &outputname, const string &chk_path, const string &gaussian_path, const int chk_number){
	bool success=false;
	string fchk_path=gaussian_path;
	size_t gaussian_directory=fchk_path.find("g09/g09");
	fchk_path.erase(fchk_path.find("g09/g09"),7);
	fchk_path.append("g09/formchk");
	string chk_temp;
	chk_temp=chk_path;
	if(chk_number>0){
		chk_temp.append("gaussian");
		chk_temp.append(to_string(chk_number));
		chk_temp.append(".chk");
	}
	else chk_temp.append("gaussian.chk");
	pid_t pid = fork();
	FILE *dscf=fopen("formchk.log","wt");
	fprintf(dscf," ");
	fclose(dscf);
	int fd = open("./formchk.log", O_WRONLY);
	int defout=dup(1);
    dup2(fd, 1);  // redirect stdout
	switch (pid) {
	    	case -1: // Error 
       	 		cerr << "Uh-Oh! fork() failed.\n";
        		success=false;
    		case 0: // Child process 
				execl(fchk_path.c_str(),"formchk",chk_temp.c_str(),outputname.c_str(),NULL); // Execute the program 
				cerr << "Uh-Oh! execl() failed in chk2fchk!"; // execl doesn't return unless there's an error 
        		success=false;
    		default: // Parent process 
        		cout << "Process created with pid " << pid << "\n";
        		int status;
        		do {
            			waitpid(pid, &status, 0); // Wait for the process to complete 
        		} while (!WIFEXITED(status)); 
        		cout << "Process exited with " << WEXITSTATUS(status) << "\n";
				success=true;
        }
	dup2(defout, 1);  // redirect stdout back to terminal
	close(fd);
	close(defout);
	return success;
};

bool gaussian(const string &programPath, bool debug){
	if(debug){
		debug_fchk=true;
	}
	bool success=false;
	string envbsd=programPath;
	envbsd.erase(envbsd.find("/g09/g09")+4,4);
	envbsd.append("/bsd");
	string envlocal=programPath;
	envlocal.erase(envlocal.find("/g09/g09")+4,4);
	envlocal.append("/local");
	string envextras=programPath;
	envextras.erase(envextras.find("/g09/g09")+4,4);
	envextras.append("/extras");
	string envshort=programPath;
	envshort.erase(envshort.find("/g09/g09")+4,4);
	if(debug_fchk){
		cout << envbsd << endl;
		cout << envlocal << endl;
		cout << envextras << endl;
		cout << envshort << endl;
	} 
	string EXEDIR=("GAUSS_EXEDIR=");
	EXEDIR.append(envbsd);
	EXEDIR.append(":");
	EXEDIR.append(envlocal);
	EXEDIR.append(":");
	EXEDIR.append(envextras);
	EXEDIR.append(":");
	EXEDIR.append(envshort);
	if(debug_fchk) cout << EXEDIR << endl;
	string BSDDIR=("GAUSS_BSDDIR=");
	BSDDIR.append(envbsd);
	if(debug_fchk) cout << BSDDIR << endl;
	char *exedir = &EXEDIR[0u];
	char *bsddir = &bsddir[0u];
	pid_t pid = fork();
	FILE *dscf=fopen("gaussian_run.log","wt");
	fprintf(dscf," ");
	fclose(dscf);
	int fd = open("./gaussian_run.log", O_WRONLY);
	int defout=dup(1);
	//Enter();
	cls();
	cout << "Running gaussian... Please wait... " << endl;
    dup2(fd, 1);  // redirect stdout
	switch (pid) {
	    	case -1: // Error 
       	 		cerr << "Uh-Oh! fork() failed.\n";
        		success=false;
    		case 0: /* Child process*/
					putenv(exedir);
					putenv(bsddir);
					execl(programPath.c_str(),"g09","gaussian",NULL); // Execute the program 
					cerr << "Uh-Oh! execl() failed in gaussian(programPath)!"; // execl doesn't return unless there's an error 
        			success=false;
    		default: // Parent process 
        		cout << "Process created with pid " << pid << "\n";
        		int status;
        		do {
            			waitpid(pid, &status, 0); // Wait for the process to complete 
        		} while (!WIFEXITED(status)); 
        		cout << "Process exited with " << WEXITSTATUS(status) << "\n";
				success=true;
        }
	dup2(defout, 1);  // redirect stdout back to terminal
	if(!debug) cout << "Finished gaussian!!" << endl; 
	close(fd);
	close(defout);
	ifstream ifile("gaussian.log",ios::in);
	string line;
	string preline;
	while (!ifile.eof()){
		preline=line;
		getline(ifile,line);
	}
	if(line.find("Normal termination")==-1&&preline.find("Normal termination")==-1) success=false;
	return success;
};

bool gaussian(bool debug){
	if(debug) debug_fchk=true;
	bool success=false;
	const char* programPath="/usr/local/g09/g09";
	string envbsd_def=programPath;
	envbsd_def.erase(envbsd_def.find("/g09/g09")+4,4);
	envbsd_def.append("/bsd");
	string envlocal=programPath;
	envlocal.erase(envlocal.find("/g09/g09")+4,4);
	envlocal.append("/local");
	string envextras=programPath;
	envextras.erase(envextras.find("/g09/g09")+4,4);
	envextras.append("/extras");
	string envshort=programPath;
	envshort.erase(envshort.find("/g09/g09")+4,4);
	if(debug_fchk){
		cout << envbsd_def << endl;
		cout << envlocal << endl;
		cout << envextras << endl;
		cout << envshort << endl;
	} 
	string EXEDIR=("GAUSS_EXEDIR=");
	EXEDIR.append(envbsd_def);
	EXEDIR.append(":");
	EXEDIR.append(envlocal);
	EXEDIR.append(":");
	EXEDIR.append(envextras);
	EXEDIR.append(":");
	EXEDIR.append(envshort);
	if(debug_fchk) cout << EXEDIR << endl;
	string BSDDIR=("GAUSS_BSDDIR=");
	BSDDIR.append(envbsd_def);
	if(debug_fchk) cout << BSDDIR << endl;
	char *exedir = &EXEDIR[0u];
	char *bsddir = &bsddir[0u];
	pid_t pid = fork();
	FILE *dscf=fopen("gaussian_run.log","wt");
	fprintf(dscf," ");
	fclose(dscf);
	int fd = open("./gaussian_run.log", O_WRONLY);
	int defout=dup(1);
	string envbsd=programPath;
	envbsd.erase(envbsd.find("/g09/g09")+4,4);
	envbsd.append("/bsd");
	if(debug_fchk) cout << envbsd << endl;
	//Enter();
	cls();
	cout << "Running gaussian... Please wait... " << endl;
    dup2(fd, 1);  // redirect stdout
	switch (pid) {
	    	case -1: // Error 
       	 		cerr << "Uh-Oh! fork() failed.\n";
        		success=false;
    		case 0: /* Child process*/
				putenv(exedir);
				putenv(bsddir);
				execl(programPath,"g09","gaussian",NULL); // Execute the program 
				cerr << "Uh-Oh! execl() failed in gaussian()!"; // execl doesn't return unless there's an error 
        		success=false;
    		default: // Parent process 
        		cout << "Process created with pid " << pid << "\n";
        		int status;
        		do {
            			waitpid(pid, &status, 0); // Wait for the process to complete 
        		} while (!WIFEXITED(status)); 
        		cout << "Process exited with " << WEXITSTATUS(status) << "\n";
				success=true;
        }
	dup2(defout, 1);  // redirect stdout back to terminal
	if(!debug) cout << "Finished gaussian!!" <<endl;
	close(fd);
	close(defout);
	return success;
};

int prepare_gaussian(const string &basis_set_path, const string &fchkname, const string &checkpoint_path, WFN &wave, bool debug){
	if(exists("gaussian.com")){
		cout << "gaussian.com already exists, do you want me to overwrite it?";
		if(!yesno()) return -1;
	}
	if(wave.get_modified ()){
		cout << "The wavefunction has been modified after reading. Please make sure that you know what you are doing!" << endl;
		cout << "Do you want to continue?" << flush;
		if(!yesno()) return -1;
	}
	//-----------------READING BASIS SET--------------------------------
	if(wave.get_nr_basis_set_loaded ()==0){
		cout << "No basis set loaded, will load a complete basis set now!" << endl;
		if(debug) Enter();
		if(!read_basis_set_vanilla (basis_set_path, wave, debug)) {
			cout << "Problem during reading of the basis set!" << endl;
			return -1;
		}
	}
	else if(wave.get_nr_basis_set_loaded ()<wave.get_ncen ()){
		cout << "Not all atoms have a basis set loaded!" << endl
			<< "Do you want to laod the missing atoms?" << flush;
		if(!yesno()){
			cout << "Do you want to load a new basis set?" <<flush;
			if(!yesno()){
				cout << "Okay, aborting then!!!" << endl;
				if(debug) Enter();
				return -1;
			}
			cout << "deleting old one..." << endl;
			if(debug) Enter();
			if(!delete_basis_set_vanilla (basis_set_path, wave, debug)){
				cout << "ERROR while deleting a basis set!" << endl;
				return -1;
			}
			else if(!read_basis_set_vanilla (basis_set_path, wave, debug)){
				cout << "ERROR during reading of the new basis set!" << endl;
				return -1;
			}
		}
		cout << "okay, loading the missing atoms.." << endl;
		if(debug) Enter();
		if(!read_basis_set_missing(basis_set_path, wave, debug)){
			cout << "ERROR during reading of missing basis set!" << endl;
			return -1;
		}
	}
	else if(wave.get_nr_basis_set_loaded ()==wave.get_ncen ()){
		cout << "There already is a basis set loaded!" << endl
			<< "Do you want me to delete the old one and load a new one?" << flush;
		if(debug) Enter();
		if(!yesno()){
			cout << "Okay, continuing with the old one..." << endl;
			if(debug) Enter();
		}
		else{
			cout << "Deleting the old basis set!" << endl;
			if(debug) Enter();
			if(!delete_basis_set_vanilla (basis_set_path, wave, debug)){
				cout << "ERROR during deleting of the basis set!";
				Enter();
				return -1;
			}
			cout << "Going to load a new one now!" << endl;
			if(!read_basis_set_vanilla (basis_set_path, wave, debug)) {
				cout << "Problem during reading of the basis set!" << endl;
				return -1;
			}
		}
	}
	else{
		cout << "# of loaded > # atoms" << endl
			<< "Sorry, this should not happen... aborting!!!" << endl;
		return -1;
	}
	//-----------------------check ordering and order accordingly----------------------
	if(!wave.sort_wfn(wave.check_order(debug),debug)){
		cout << "Could not order the wavefunction, aborting!" << endl;
		if(debug) Enter();
		return -1;
	}
	//------------setting up the .com file with basis set---------------
	ofstream com;
	com.open("gaussian.com",ios::out);
	string temp;
	temp=checkpoint_path;
	temp.append("gaussian.chk");
	int run=0;
	if(exists(temp)){
		bool found=true;
		while (found){
			run++;
			temp=checkpoint_path;
			temp.append("gaussian");
			temp.append(to_string(run));
			temp.append(".chk");
			if(!exists(temp)) found=false;
		}
		if(debug) cout << "writing chk to "<< temp;
		if(run>20) cout << "Maybe you should tidy up your scratch folder!" << endl;
	}
	com << "%chk="<< temp << endl;
	com << "%mem=500MW" << endl;
	com << "%nproc=4" << endl;
	//com << "# SCF b3lyp/gen 6D 10F" << endl << endl;
	if(wave.get_d_f_switch()==true) com << "# SCF=(MaxCycle=3000,Conver=-1) b3lyp/gen 5D 7F nosymm" << endl << endl;
	if(wave.get_d_f_switch()==false) com << "# SCF=(MaxCycle=3000,Conver=-1) b3lyp/gen 6D 10F nosymm" << endl << endl;
	com << "TITLE" << endl << endl;
	com << wave.get_charge() << " " << wave.get_multi() << endl;
	com << wave.get_centers (false);
	com << endl;
	//com << "****" << endl;
	vector<string> elements_list;
	if(debug) cout << "elements_list.size()= " << elements_list.size() << endl;
	for(int a=0; a<wave.get_ncen(); a++){
		string label_temp;
		label_temp=wave.get_atom_label(a);
		bool found=false;
		if(elements_list.size()!=0){
			for (int i=0; i< elements_list.size(); i++) {
				if(elements_list[i].find(label_temp)!=-1){
					found=true;
					if(debug) cout << "Found " << label_temp << " in the elements_list!" << endl;
				}
			}
		}
		if(!found){
			elements_list.push_back(label_temp);
			if(debug) cout << "elements_list.size()= " << elements_list.size() << endl;
			com << label_temp << " 0" << endl;
			int working_shell=-1;
			for (int p=0; p< wave.get_atom_primitive_count (a);p++){
				int temp_shell=wave.get_basis_set_shell (a,p);
				if(temp_shell!=working_shell){
					working_shell=temp_shell;
					switch (wave.get_shell_type (a,temp_shell)){
						case 1:
							//if(working_shell==1) com << "S " << wave.get_atom_shell_primitives (a,temp_shell)+1 << " 1.00" << endl;
							//else 
							com << "S " << wave.get_atom_shell_primitives (a,temp_shell) << " 1.00" << endl;
							break;
						case 2:
							com << "P " << wave.get_atom_shell_primitives (a,temp_shell) << " 1.00" << endl;
							break;
						case 3:
							com << "D " << wave.get_atom_shell_primitives (a,temp_shell) << " 1.00" << endl;
							break;
						case 4:
							com << "F " << wave.get_atom_shell_primitives (a,temp_shell) << " 1.00" << endl;
							break;
					}
				}
				com << scientific << setw(17) << setprecision(10) << wave.get_atom_basis_set_exponent (a,p);
				com << " ";
				com << scientific << setw(17) << setprecision(10) << wave.get_atom_basis_set_coefficient (a,p);
				com << endl;
		    }
			com << "****" << endl;
		}
	}
	com << endl;
	com.flush();
	com.close();
	if(debug){
		cout << "Wrote the gaussian Input! Please check it before i continue..." << endl;
		Enter();
	}
	/*if(!delete_basis_set_vanilla (basis_set_path, wave, debug)){
		cout << "ERROR during deletion of the basis set";
		Enter();
	}*/
	return run;
};

bool new_fchk(const string &basis_set_path, const std::string &fchkname, 
              const std::string &checkpoint_path, const std::string &gaussian_path, 
              WFN &wave, bool debug){
	if(debug) debug_fchk=true;
	if(debug_fchk) cout << "let's run a gaussian calculation to obtain a fchk file" << endl;
	int chk_number;
	if(checkpoint_path.size()<3){
		cout << "Going to make the fchk file with default checkpoint folder!" << endl;
		string temp=("gaussian.chk");
		chk_number=prepare_gaussian (basis_set_path,fchkname,temp,wave,debug);
	}
	else{
		chk_number=prepare_gaussian (basis_set_path, fchkname,checkpoint_path, wave, debug);
	}
	if(chk_number==-1){
		cout << "ERROR druing Preparation of gaussian, aborting!" << endl;
		if(debug) Enter();
		return false;
	}
	bool success=false;
	if(gaussian_path.size()>3) success=gaussian(gaussian_path,debug);
	else success=gaussian(debug);
	if(success){
		success=chk2fchk(fchkname,checkpoint_path,gaussian_path,chk_number);
		if(debug){
			cout << "made the fchk!" << endl;
			Enter();
		}
	}
	else return false;
	if(debug){
		cout << "I will keep the gaussian files which have been created temporarily!!" << endl;
		Enter();
	}
	if(success&&!debug_fchk){
		remove("gaussian.com");
		remove("gaussian.log");
		remove("gaussian_run.log");
		remove("formchk.log");
	}
	return success;
};

bool modify_fchk(const string &fchk_name, const string &basis_set_path, WFN &wave, bool debug, const bool &read){
	wave.set_modified();
	if(debug) debug_fchk=true;
	vector<double> CMO;
	int nao=0;
	int nshell=0;
	int naotr=0;
	if(debug){
		cout << "Origin: " << wave.get_origin() << endl;
		Enter();
	}
	if(wave.get_origin ()==2){
		//-----------------READING BASIS SET--------------------------------
		if(read){
			if(!read_basis_set_vanilla (basis_set_path, wave, debug)) {
				cout << "Problem during reading of the basis set!" << endl;
				return false;
			}
		}
		double pi=3.14159265358979;
		//---------------normalize basis set---------------------------------
		if(debug) cout << "starting to normalize the basis set" << endl;
		vector<double> norm_const;
		//-----------debug output---------------------------------------------------------
		if(debug){
			cout << "exemplary output before norm_const of the first atom with all it's properties: "<< endl;
			wave.print_atom_long(0);
			cout << "ended normalising the basis set, now for the MO_coeffs" << endl;	
			cout << "Status report:" << endl;
			cout << "size of norm_const: " << norm_const.size() << endl;
			cout << "WFN MO counter: " << wave.get_nmo() << endl;
			cout << "Number of atoms: " << wave.get_ncen() << endl;
			cout << "Primtive count of zero MO: " << wave.get_MO_primitive_count(0) << endl;
			cout << "Primtive count of first MO: " << wave.get_MO_primitive_count(1) << endl;
			Enter();
		}
		//-----------------------check ordering and order accordingly----------------------
		wave.sort_wfn(wave.check_order(debug),debug);
		//-------------------normalize the basis set shell wise---------------------
		for(int a=0; a< wave.get_ncen(); a++){
			for(int p=0; p< wave.get_atom_primitive_count(a); p++){
				double temp=wave.get_atom_basis_set_exponent(a,p);
				switch(wave.get_atom_primitive_type(a,p)){
					case 1:
						temp=2*temp/pi;
						temp=pow(temp,0.75);
						temp=temp*wave.get_atom_basis_set_coefficient(a,p);
						wave.change_atom_basis_set_coefficient(a,p,temp);
						break;
					case 2:
						temp=128*pow(temp,5);
						temp=temp/pow(pi,3);
						temp=pow(temp,0.25);
						temp=wave.get_atom_basis_set_coefficient(a,p)*temp;
						wave.change_atom_basis_set_coefficient(a,p,temp);
						break;
					case 3:
						temp=2048*pow(temp,7);
						temp=temp/(9*pow(pi,3));
						temp=pow(temp,0.25);
						temp=wave.get_atom_basis_set_coefficient(a,p)*temp;
						wave.change_atom_basis_set_coefficient(a,p,temp);
						break;
					case 4:
						temp=32768*pow(temp,9);
						temp=temp/(225*pow(pi,3));
						temp=pow(temp,0.25);
						temp=wave.get_atom_basis_set_coefficient(a,p)*temp;
						wave.change_atom_basis_set_coefficient(a,p,temp);
						break;
					case -1:
						cout << "Sorry, the type reading went wrong somwhere, look where it may have gone crazy..." << endl;
						Enter();
						break;
				}
			}
		}
		for(int a=0; a< wave.get_ncen(); a++){
			double aiaj=0.0;
			double factor=0.0;
			for(int s=0; s< wave.get_atom_shell_count(a); s++){
				int type_temp=wave.get_shell_type(a,s);		
				if(type_temp==-1){
					cout << "ERROR in type assignement!!"<<endl;
					Enter();
				}
				if (debug){
					cout << "Shell: " << s << " of atom: " << a << " Shell type: " << type_temp << endl;
					int testcount=0;
					cout << "start: " << wave.get_shell_start(a,s,false) << flush;
					cout <<" stop: " << wave.get_shell_end(a,s,false) << flush << endl;
					cout << "factor: ";
				}
				switch (type_temp){
					case 1:
						factor=0;
						for(int i=wave.get_shell_start(a,s,false); i<= wave.get_shell_end(a,s,false); i++){
							for(int j=wave.get_shell_start(a,s,false); j<= wave.get_shell_end(a,s,false); j++){
								double aiaj=wave.get_atom_basis_set_exponent (a,i)+wave.get_atom_basis_set_exponent (a,j);
								double term=(pi/aiaj);
								term=pow(term,1.5);
								factor+=wave.get_atom_basis_set_coefficient(a,i)*wave.get_atom_basis_set_coefficient(a,j)*term;
							}
						}
						if(factor==0) return false;
						factor=pow(factor,-0.5);
						if(debug) cout << factor << endl;
						for(int i=wave.get_shell_start(a,s,false); i<= wave.get_shell_end(a,s,false); i++){
							if(debug){
								cout << "Contraction coefficient before: " << wave.get_atom_basis_set_coefficient(a,i) << endl;
								cout << "Contraction coefficient after:  " << factor*wave.get_atom_basis_set_coefficient(a,i) << endl;
							}
							wave.change_atom_basis_set_coefficient(a,i,factor*wave.get_atom_basis_set_coefficient(a,i));
							norm_const.push_back(wave.get_atom_basis_set_coefficient(a,i));
						}
						break;
					case 2:
						factor=0;
						for(int i=wave.get_shell_start(a,s,false); i<= wave.get_shell_end(a,s,false); i++){
							for(int j=wave.get_shell_start(a,s,false); j<= wave.get_shell_end(a,s,false); j++){
								double aiaj=wave.get_atom_basis_set_exponent (a,i)+wave.get_atom_basis_set_exponent (a,j);
								double term=4*pow(aiaj,5);
								term=pow(pi,3)/term;
								term=pow(term,0.5);
								factor+=wave.get_atom_basis_set_coefficient(a,i)*wave.get_atom_basis_set_coefficient(a,j)*term;
							}
						}
						if(factor==0) return false;
						factor=pow(factor,-0.5);
						if(debug) cout << factor << endl;
						for(int i=wave.get_shell_start(a,s,false); i<= wave.get_shell_end(a,s,false); i++){
							if(debug){
								cout << "Contraction coefficient before: " << wave.get_atom_basis_set_coefficient(a,i) << endl;
								cout << "Contraction coefficient after:  " << factor*wave.get_atom_basis_set_coefficient(a,i) << endl;
							}
							wave.change_atom_basis_set_coefficient(a,i,factor*wave.get_atom_basis_set_coefficient(a,i));
							for(int k=0; k<3; k++) norm_const.push_back(wave.get_atom_basis_set_coefficient(a,i));
						}
						break;
					case 3:
						factor=0;
						for(int i=wave.get_shell_start(a,s,false); i<= wave.get_shell_end(a,s,false); i++){
							for(int j=wave.get_shell_start(a,s,false); j<= wave.get_shell_end(a,s,false); j++){
								double aiaj=wave.get_atom_basis_set_exponent (a,i)+wave.get_atom_basis_set_exponent (a,j);
								double term=16*pow(aiaj,7);
								term=pow(pi,3)/term;
								term=pow(term,0.5);
								factor+=wave.get_atom_basis_set_coefficient(a,i)*wave.get_atom_basis_set_coefficient(a,j)*term;
							}
						}
						if(factor==0) return false;
						factor=(pow(factor,-0.5))/sqrt(3);
						if(debug) cout << factor << endl;
						for(int i=wave.get_shell_start(a,s,false); i<= wave.get_shell_end(a,s,false); i++){
							if(debug){
								cout << "Contraction coefficient before: " << wave.get_atom_basis_set_coefficient(a,i) << endl;
								cout << "Contraction coefficient after:  " << factor*wave.get_atom_basis_set_coefficient(a,i) << endl;
							}
							wave.change_atom_basis_set_coefficient(a,i,factor*wave.get_atom_basis_set_coefficient(a,i));
							for(int k=0; k<3; k++) norm_const.push_back(wave.get_atom_basis_set_coefficient(a,i));
							for(int k=0; k<3; k++) norm_const.push_back(sqrt(3)*wave.get_atom_basis_set_coefficient(a,i));
						}
						break;
					case 4:
						factor=0;
						for(int i=wave.get_shell_start(a,s,false); i<= wave.get_shell_end(a,s,false); i++){
							for(int j=wave.get_shell_start(a,s,false); j<= wave.get_shell_end(a,s,false); j++){
								double aiaj=wave.get_atom_basis_set_exponent (a,i)+wave.get_atom_basis_set_exponent (a,j);
								double term=64*pow((aiaj),9);
								term=pow(pi,3)/term;
								term=pow(term,0.5);
								factor+=wave.get_atom_basis_set_coefficient(a,i)*wave.get_atom_basis_set_coefficient(a,j)*term;
							}
						}
						if(factor==0) return false;
						factor=pow(factor,-0.5)/sqrt(15);
						if(debug) cout << factor << endl;
						for(int i=wave.get_shell_start(a,s,false); i<= wave.get_shell_end(a,s,false); i++){
							if(debug){
								cout << "Contraction coefficient before: " << wave.get_atom_basis_set_coefficient(a,i) << endl;
								cout << "Contraction coefficient after:  " << factor*wave.get_atom_basis_set_coefficient(a,i) << endl;
							}
							wave.change_atom_basis_set_coefficient(a,i,factor*wave.get_atom_basis_set_coefficient(a,i));
							for(int l=0; l<10;l++) norm_const.push_back(wave.get_atom_basis_set_coefficient(a,i));
						} 
						break;
				}
				if (debug)	cout << "This shell has: " <<  wave.get_shell_end(a,s,false)-wave.get_shell_start(a,s,false)+1 << " primitives" << endl;
			}
		}
		//-----------debug output---------------------------------------------------------
		if(debug){
			cout << "exemplary output of the first atom with all it's properties: "<< endl;
			wave.print_atom_long(0);
			cout << "ended normalising the basis set, now for the MO_coeffs" << endl;	
			cout << "Status report:" << endl;
			cout << "size of norm_const: " << norm_const.size() << endl;
			cout << "WFN MO counter: " << wave.get_nmo() << endl;
			cout << "Number of atoms: " << wave.get_ncen() << endl;
			cout << "Primtive count of zero MO: " << wave.get_MO_primitive_count(0) << endl;
			cout << "Primtive count of first MO: " << wave.get_MO_primitive_count(1) << endl;
			Enter();
		}
		//---------------------To not mix up anything start normailising WFN_matrix now--------------------------
		int run=0;
		ofstream norm_cprim;
		if(debug) norm_cprim.open("norm_prim.debug", ofstream::out);
		for(int m=0; m< wave.get_nmo(); m++){
			if(debug) norm_cprim << m << ". MO: " << endl;
			for(int p=0; p<wave.get_MO_primitive_count(m);p++){
				if(debug) cout << p << ". primitive; " << m << ". MO " << "norm nonst: " << norm_const[p];
				double temp=wave.get_MO_coef(m,p, debug)/norm_const[p];
				if(debug){
					cout << " temp after normalisation: " << temp << endl;
					norm_cprim << " " << temp << endl;
				}
				run++;
				if(!wave.change_MO_coef(m,p,temp, debug)){
					cout << "ERROR in changing the coefficients after normalising!";
					if(debug) cout << "m:" << m << " p: " << p << " temp:" << temp; 
					cout << endl;
					Enter();
					return false;
				}
			}
		}
		if(debug) {
			norm_cprim.flush();
			norm_cprim.close();
			cout << "See norm_cprim.debug for the CPRIM vectors" << endl;
			cout << "Total count in CPRIM: " << run << endl;
			Enter();
		}
		//--------------Build CMO of alessandro from the first elements of each shell-------------
		for (int m =0; m < wave.get_nmo (); m++){
			int offset=0;
			int run=0;
			for (int a =0; a < wave.get_ncen (); a++){
				int s_count=0;
				int p_count=0;
				int d_count=0;
				int f_count=0;
				for (int s=0; s< wave.get_atom_shell_count(a); s++){
					switch (wave.get_shell_type(a,s)){
						case 1:
							CMO.push_back(wave.get_MO_coef(m,wave.get_shell_start(a,s,false)+2*p_count+5*d_count+9*f_count+offset, false));
							if(debug) cout << "shell start: " << wave.get_shell_start(a,s,false) << " pushing back S type: " 
									<< wave.get_MO_coef(m,wave.get_shell_start(a,s,false)+2*p_count+5*d_count+9*f_count+offset, false) << " number: "  
									<< wave.get_shell_start(a,s,false)+2*p_count+5*d_count+9*f_count+offset << endl;
							if(m==0) nao++;
							s_count+=wave.get_atom_shell_primitives(a,s);
							if(debug&&wave.get_atom_shell_primitives(a,s)!=1) 
								cout << "This shell has " << wave.get_atom_shell_primitives(a,s) << " primitives!" << endl;
						break;  
						case 2:
							for (int i=0; i<3 ; i++){
								CMO.push_back(wave.get_MO_coef(m,wave.get_shell_start(a,s,false)+i+2*p_count+5*d_count+9*f_count+offset, false));
								if(debug) cout << "shell start: " << wave.get_shell_start(a,s,false) << " pushing back P type: "
									<< wave.get_MO_coef(m,wave.get_shell_start(a,s,false)+i+2*p_count+5*d_count+9*f_count+offset, false)  << " number: "  
									<< wave.get_shell_start(a,s,false)+i+2*p_count+5*d_count+9*f_count+offset << endl;
							}
							p_count+=wave.get_atom_shell_primitives(a,s);
							if(debug&&wave.get_atom_shell_primitives(a,s)!=1) 
								cout << "This shell has " << wave.get_atom_shell_primitives(a,s) << " primitives!" << endl; 
							if(m==0) nao+=3;
						break;
						case 3:
							for (int i=0; i<6 ; i++){
								CMO.push_back(wave.get_MO_coef(m,wave.get_shell_start(a,s,false)+i+2*p_count+5*d_count+9*f_count+offset,false));
								if(debug) cout << "shell start: " << wave.get_shell_start(a,s,false) << " pushing back D type: " 
									<< wave.get_MO_coef(m,wave.get_shell_start(a,s,false)+i+2*p_count+5*d_count+9*f_count+offset,false) << " number: "  
									<< wave.get_shell_start(a,s,false)+i+2*p_count+5*d_count+9*f_count+offset << endl;
							}
							d_count+=wave.get_atom_shell_primitives(a,s);
							if(debug&&wave.get_atom_shell_primitives(a,s)!=1) 
								cout << "This shell has " << wave.get_atom_shell_primitives(a,s) << " primitives!" << endl;
							if(m==0) nao+=6;
						break;
						case 4:
							for (int i=0; i<10 ; i++){
								CMO.push_back(wave.get_MO_coef(m,wave.get_shell_start(a,s,false)+i+2*p_count+5*d_count+9*f_count+offset,false));
								if(debug) cout << "shell start: " << wave.get_shell_start(a,s,false) << " pushing back F type: " 
									<< wave.get_MO_coef(m,wave.get_shell_start(a,s,false)+i+2*p_count+5*d_count+9*f_count+offset,false) << " number: "  
									<< wave.get_shell_start(a,s,false)+i+2*p_count+5*d_count+9*f_count+offset << endl;
							}
							f_count+=wave.get_atom_shell_primitives(a,s);
							if(debug&&wave.get_atom_shell_primitives(a,s)!=1) 
								cout << "This shell has " << wave.get_atom_shell_primitives(a,s) << " primitives!" << endl;
							if(m==0) nao+=10;
						break;
					}
					run++;
				}
				if(debug) cout << "finished with atom!" << endl;
				offset+=s_count+3*p_count+6*d_count+10*f_count;
			}
			if(debug) cout << "finished with MO!" << endl;
			if(nshell!=run) nshell=run;
		}
		if(debug){
			ofstream cmo ("cmo.debug", ofstream::out);
			for(int p=0; p< CMO.size() ; p++){
				string temp;
				for( int i=0; i< 5; i++){
					stringstream stream;
					stream << scientific << setw(14) << setprecision(7) << CMO[p+i] << " ";
					temp += stream.str();
				}
				p+=4;
				cmo << temp << endl;
			}
			cmo.flush();
			cmo.close();
			cout << CMO.size() << " Elements in CMO" << endl;
			cout << norm_const.size() << " = nprim" << endl;
			cout << nao << " = nao" << endl;
			cout << nshell << " = nshell" << endl;
			Enter();
		}
		//------------------ make the DM -----------------------------
		naotr=nao*(nao+1)/2;
		vector<double> kp;
		for(int i=0; i< naotr; i++) wave.push_back_DM (0.0);
		if(debug) cout << "I made kp!" << endl << nao << " is the maximum for iu" << endl;
		for(int iu=0; iu<nao; iu++){
			for(int iv=0; iv<=iu; iv++){
				int iuv=(iu*(iu+1)/2)+iv;
				if(debug) cout << "iu: " << iu << " iv: " << iv << " iuv: " << iuv << " kp(iu): " << iu*(iu+1)/2<< endl;
				for(int m=0; m<wave.get_nmo(); m++){
					if(!wave.set_DM(iuv,wave.get_DM(iuv)+2*CMO[iu+(m*nao)]*CMO[iv+(m*nao)])){
						cout << "Something went wrong while writing the DM! iuv=" << iuv << endl;
						Enter();
						return false;
					}
				}
			}
		}
		if(debug){
			cout << "DM is:" << endl;
			for(int p=0; p< wave.get_DM_size() ; p++){
				string temp;
				for( int i=0; i< 5; i++){
					stringstream stream;
					stream << scientific << setw(14) << setprecision(7) << wave.get_DM(i+p) << " ";
					temp += stream.str();
				}
				p+=4;
				cout << temp << endl;
			}
			cout << wave.get_DM_size() << " Elements in DM" << endl;
			Enter();
		}
	}
	// open fchk for copying
	string temp_fchk=fchk_name;
	temp_fchk.append(".fchk");
	if(!exists(temp_fchk)){
		cout << "This is worrysome... The fchk should be there.." << endl;
		cout << "fchk_name: " << temp_fchk << endl;
		Enter();
		return false;
	}
	ifstream ifchk(temp_fchk.c_str());
	if(!ifchk.is_open()){
		cout << "ERROR while opening .fchk ifile!" << endl;
		Enter();
		return false;
	}
	ofstream ofchk;
	ofchk.open("temp.fchk",ofstream::out);
	string line;
	int dum_nao=0;
	if(wave.get_origin()==2){
		while(line.find("Alpha Orbital Energies")==-1 &&!ifchk.eof()){
			getline(ifchk,line);
			if(debug) cout << "line: " << line << endl;
			if(line.find("Alpha Orbital Energies")==-1) ofchk << line << endl;
			else{
				char tempchar[100];
				size_t length;
				length = line.copy(tempchar,11,50);
				tempchar[length]='\0';
				sscanf(tempchar,"%d",&dum_nao);
				if(debug){
					cout << "nao read from fchk: " << dum_nao << " and from basis set: " << nao << endl;
					Enter();
				}
				ofchk << line << endl;
			}
		}
		while(line.find("Alpha MO")==-1 && !ifchk.eof()){
			getline(ifchk,line);
			if(debug) cout << "line: " << line << endl;
			if(line.find("Alpha MO")==-1) ofchk << line << endl;
		}
		ofchk.flush();   
		ofchk << "Alpha MO coefficients                      R   N=" << setw(12) << nao*dum_nao << endl;
		//now write the CMO and skip lines in IFCHK
		for(int i=0; i<nao*dum_nao; i++){
			string temp=" ";
			for(int j=0; j<5; j++){
				if(i+j<CMO.size()){
					stringstream stream;
					stream << scientific << setw(15) << setprecision(8) << CMO[i+j] << " ";
					temp += stream.str();
					temp.replace(12+j*16,1,"E");
				}
				else if(i+j<nao*nao){
					stringstream stream;
					stream << scientific << setw(15) << setprecision(8) << 0.0 << " ";
					temp += stream.str();
					temp.replace(12+j*16,1,"E");
				}
			}
			i+=4;
			temp+='\n';
			ofchk << temp;
			getline(ifchk,line);
		}
	}
	if(wave.get_origin()==1){
		while(line.find("Total SCF Density")==-1 && !ifchk.eof()){
			getline(ifchk,line);
			if(debug) cout << "line: " << line << endl;
			if(line.find("Total SCF Density")==-1) ofchk << line << endl;
		}
		ofchk.flush();
	}
	ofchk << "Total SCF Density                          R   N=" << setw(12) << wave.get_DM_size() << endl;
	//now write the DM and skip lines in IFCHK
	for(int i=0; i<wave.get_DM_size(); i++){
		string temp=" ";
		if(debug) cout << "i: " << i << " DM_size= " << wave.get_DM_size() << " Element ";
		for(int j=0; j<5; j++){
			if(i+j<wave.get_DM_size ()){
				stringstream stream;
				if(debug) cout << i+j << " ";
				stream << scientific << setw(15) << setprecision(8) << wave.get_DM(i+j) << " ";
				if(i+j<wave.get_DM_size ()){
					temp += stream.str();
					temp.replace(12+j*16,1,"E");
				}
			}
		}
		i+=4;
		if(debug) cout << endl;
		temp+='\n';
		ofchk << temp;
		getline(ifchk,line);
	}
	if(debug) Enter();
	while(!ifchk.eof()){
		getline(ifchk,line);
		ofchk << line ;
		if(!ifchk.eof()) ofchk << endl ;
	}
	ofchk.flush();
	ofchk.close();
	ifchk.close();
	if(debug){
		cout << "Do you want me to keep the old fchk file and the new temp.fchk?";
		if(!yesno()){
			copy_file("temp.fchk",temp_fchk);
		}
	}
	else copy_file("temp.fchk",temp_fchk);
	cls();
	return true;
};
