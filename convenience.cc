#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <math.h>

#include "convenience.h"

using namespace std;

//--------------------------General convenience terminal functions---------------------------------

bool yesno(){
	bool end=false;
	while (!end) {
		char dum ='?';
		cout << "(Y/N)?";
		cin >> dum;
		if(dum == 'y'||dum == 'Y'){
			cout << "Okay..." << endl;
			end=true;
		}
		else if(dum == 'N'||dum == 'n') return false;
		else cout << "Sorry, i did not understand that!" << endl;
	}
	return true;
};

void Enter(){
	cout << "press ENTER to continue... " << flush;
	cin.ignore();
	//cin.get();
};

void cls(){
    cout << string( 100, '\n' );
};

string atnr2letter(const int nr){
	switch(nr){
			case 1:
				return ("H");
			case 2:
				return ("He");
			case 3:
				return ("Li");
			case 4:
				return ("Be");
			case 5:
				return ("B");
			case 6:
				return ("C");
			case 7:
				return ("N");
			case 8:
				return ("O");
			case 9:
				return ("F");
			case 10:
				return ("Ne");
			case 11:
				return ("Na");
			case 12:
				return ("Mg");
			case 13:
				return ("Al");
			case 14:
				return ("Si");
			case 15:
				return ("P");
			case 16:
				return ("S");
			case 17:
				return ("Cl");
			case 18:
				return ("Ar");
			case 19:
				return ("K");
			case 20:
				return ("Ca");
			case 21:
				return ("Sc");
			case 22:
				return ("Ti");
			case 23:
				return ("V");
			case 24:
				return ("Cr");
			case 25:
				return ("Mn");
			case 26:
				return ("Fe");
			case 27:
				return ("Co");
			case 28:
				return ("Ni");
			case 29:
				return ("Cu");
			case 30:
				return ("Zn");
			case 31:
				return ("Ga");
			case 32:
				return ("Ge");
			case 33:
				return ("As");
			case 34:
				return ("Se");
			case 35:
				return ("Br");
			case 36:
				return ("Kr");
			default:
				cout << "Only yet implemented from H-Kr, ask Florian for improvements or give a reasonable number between 1-36!" << endl;
				Enter();
				return ("PROBLEM");
	}
};

void copy_file(string from, string to){
	ifstream source(from, ios::binary);
    ofstream dest(to, ios::binary);

    dest << source.rdbuf();

    source.close();
    dest.close();
};

//----------------------------WFN Handling-------------------------------------------------------------

bool writewfn(WFN &wavefunction, bool debug){
	string path;
	cout << "Path to the .wfn file to be written: ";
	cin >> path;
	int tries=0;
	if(path.find(".wfn")==-1){
		do {
			cout << "This doesn't look like a .wfn file, try again: " << endl;
			tries++;
			cin >> path;
		} while (path.find(".wfn")==-1&&tries<3);
		if(tries == 3) throw (int)2;
	}
	try{
		if(wavefunction.write_wfn (path,debug)){
			wavefunction.make_path(path);
			return true;
		}
		else{
			cout << "Sorry, something went wrong, try to look above what i might be..\n";
			throw (int)1;
		}
	}
	catch(exception& e)
	{
		cerr << "Sorry, something went wrong, try to look above what i might be.. the error code is " << e.what() << endl;
		throw (int) 3;
	}
}; 

bool readwfn(WFN &wavefunction, bool debug){
	string path;
	cout << "Path to the .wfn file: ";
	cin >> path;
	int tries=0;
	if(path.find(".wfn")==-1){
		do {
			cout << "This doesn't look like a .wfn file, try again: " << endl;
			tries++;
			cin >> path;
		} while (path.find(".wfn")==-1&&tries<3);
		if(tries == 3) throw (int)2;
	}
	try{
		if(wavefunction.read_wfn (path,debug)){
			wavefunction.make_path(path);
			return true;
		}
		else{
			cout << "Sorry, something went wrong, try to look above what i might be..\n";
			throw (int)1;
		}
	}
	catch(exception& e)
	{
		cerr << "Sorry, something went wrong, try to look above what i might be.. the error code is " << e.what() << endl;
		throw (int) 3;
	}
};

//---------------------------Configuration files ---------------------------------------------------

void write_template_confi(){
	ofstream conf;
	string line;
	if(exists("programs.conf")){
		cout << "File already exist, do you want to overwrite it?" << endl;
		if(!yesno()) return;
	}
	conf.open("programs.conf",ios::out);
	conf << "gaussian=\"/usr/local/g09/g09\"" << endl;
	conf << "turbomole=\"/usr/local/bin/dscf7.1\"" << endl;
	conf << "chkfiles=\"/scratch/florian/\"" << endl;
	conf << "basis=\"/home/florian/tonto-3.5.0/basis_sets/\"" << endl;
	conf.flush();
	conf.close();
	return;
};

bool program_confi(string &gaussian_path, string &turbomole_path, 
                   string &checkpoint_path, string &basis, bool debug){
	ifstream conf("programs.conf");
	string line;
	if(conf.good()){
		if(debug) cout << "File is valid, continuing..." << endl;
	}
	else{
		cout << "couldn't open or find programs.conf,do you want me to write a template for you?" << endl;
		if(yesno()){
			write_template_confi();
			cout << "Wrote a template for you, please restart the program!" << endl;
			exit (-1);
		}
		else return false;
	}
	conf.seekg(0);
	getline(conf,line);
	size_t length;
	char tempchar[200];
	int run=0;
	while(!conf.eof()){
		switch(run){
			case 0:
				length = line.copy(tempchar,line.size()-11,10);
				tempchar[length]='\0';
				gaussian_path=tempchar;
				break;
			case 1:
				length = line.copy(tempchar,line.size()-12,11);
				tempchar[length]='\0';
				turbomole_path=tempchar;
				break;
			case 2:
				length = line.copy(tempchar,line.size()-11,10);
				tempchar[length]='\0';
				checkpoint_path=tempchar;
				break;
			case 3:
				length = line.copy(tempchar,line.size()-8,7);
				tempchar[length]='\0';
				basis=tempchar;
				break;
			default:
				if(debug) cout << "This should be the end, if you miss something check the switch" << endl;
				break;
		}
		if(debug) cout << run << ". line: " << tempchar << endl;
		run++;
		getline(conf,line);
	}
	return true;
};

bool check_bohr(WFN &wave, bool debug){
	double min_length=300.0;
	for(int i=0; i<wave.get_ncen(); i++){
		double atom1[3];
		for(int x=0; x<3; x++) atom1[x]=wave.get_atom_coordinate (i, x, debug);
		for(int j=i+1; j<wave.get_ncen(); j++){
			double atom2[3];
			for(int x=0; x<3; x++) atom2[x]=wave.get_atom_coordinate (j,x,debug);
			double d[3];
			d[0]=atom1[0]-atom2[0];
			d[1]=atom1[1]-atom2[1];
			d[2]=atom1[2]-atom2[2];
			double length=sqrt(pow(d[0],2)+pow(d[1],2)+pow(d[2],2));
			if(debug) cout << "Length for: " << i << ";" << j << ": " << length << ", min_length: " << min_length << endl;
			if(length<min_length) min_length=length;
		}
	}
	if(min_length<2){
		if(debug) cout << "Decided it's written in Angstrom" << endl;
		Enter();
		return false;
	}
	else{
		if(debug) cout << "Decided it's written in Bohr" << endl;
		Enter();
		return true;
	}
};
