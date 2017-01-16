#include <stdlib.h>
#include <stdio.h>
#include <math.h>
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
#include <vector>

#include "wfn_class.h"
#include "mo_class.h"
#include "convenience.h"

using namespace std;


bool debug_wfn=false;
bool debug_wfn_deep=false;


WFN::WFN(){
	ncen=0;
	nfunc=0;
	nmo=0;
	nex=0;
	charge=0;
	multi=0;
	origin=0;
	d_f_switch=false;
	modified=false;
};

WFN::WFN(int given_origin){
	ncen=0;
	nfunc=0;
	nmo=0;
	nex=0;
	charge=0;
	multi=0;
	origin=given_origin;
	d_f_switch=false;
	modified=false;
};

bool WFN::push_back_atom(string label, double x, double y, double z, int charge){
	ncen++;
	if(charge>=1) atoms.push_back(atom(label, ncen, x, y, z, charge));
	else atoms.push_back(atom());
	return true;
};


bool WFN::erase_atom(int nr){
	if (ncen<1) return false;
	ncen--;
	atoms.erase(atoms.begin()+(nr-1));
	return true;
};

bool WFN::push_back_MO(int nr, float occ, double ener){
	nmo++;
	//hier fehlen noch sinnhaftigkeitsfragen
	MOs.push_back(MO(nr,occ,ener));
	return true;
};

bool WFN::push_back_center(int cent){
	if(cent<=ncen&&cent>0) centers.push_back(cent);
	else return false;
	return true;
};

bool WFN::erase_center(int nr){
	centers.erase(centers.begin()+(nr-1));
	return true;
};


string WFN::get_centers(bool bohr){
	string temp;	
	for (int i=0; i<ncen;i++) {
		temp.append(atoms[i].label);
		temp.append(" "); 
		if(!bohr) temp.append(to_string(atoms[i].x));
		else temp.append(to_string(atoms[i].x/0.52917721067));
		temp.append(" ");
		if(!bohr) temp.append(to_string(atoms[i].y));
		else temp.append(to_string(atoms[i].y/0.52917721067));
		temp.append(" "); 
		if(!bohr) temp.append(to_string(atoms[i].z));
		else temp.append(to_string(atoms[i].z/0.52917721067));
		temp.append("\n");
	}
	return temp;
};
/*
vector<string> WFN::get_centers_vector(){
	vector<string> temp;
	for (int i=0; i<ncen;i++) {
		temp.push_back((""));
		temp[i].append(atoms[i].label);
		temp[i].append(" "); 
		temp[i].append(to_string(atoms[i].x));
		temp[i].append(" ");
		temp[i].append(to_string(atoms[i].y));
		temp[i].append(" "); 
		temp[i].append(to_string(atoms[i].z));
	}
	return temp;
};*/

void WFN::list_centers(){
	for (int i=0; i<ncen;i++) {
		cout << atoms[i].nr << " " << atoms[i].label << " " 
		<< atoms[i].x << " " << atoms[i].y << " " 
		<< atoms[i].z << " " << atoms[i].charge << endl;
	}
};

bool WFN::push_back_type(int type){
	types.push_back(type);
	return true;
};

bool WFN::erase_type(int nr){
	if (nr<1) return false;
	types.erase(types.begin()+(nr-1));
	return true;
};

bool WFN::push_back_exponent(double e){
	exponents.push_back(e);
	return true;

};

bool WFN::erase_exponent(int nr){
	if (nr<1) return false;
	exponents.erase(exponents.begin()+(nr-1));
	return true;
};

bool WFN::remove_primitive(int nr){
	nex--;
	if(erase_center (nr)&&erase_exponent (nr)&&erase_type (nr)){
		for (int n=0;n<nmo; n++) MOs[n].erase_coef (nr,nex);
		return true;
	}
	else return false;
};

bool WFN::add_primitive(int cent, int type, double e, double * values){
	nex++;
	if(push_back_center (cent)&&push_back_type (type)&&push_back_exponent (e))
		for(int n=0; n<nmo; n++) MOs[n].push_back_coef (values[n],nex);
	else return false;
	return true;
};

void WFN::change_type(int nr){
	bool end=false;
	while (!end) {
		cout << "Please enter the new type you want to assign: ";
		int new_type=0;
		cin >> new_type;
		if(new_type>ncen||new_type<0){
			cout << "Sorry, wrong input, try again!\n";
			continue;
		}
		types[nr-1]=new_type;
		end=true;
		set_modified();
	}
};

void WFN::change_exponent(int nr){
	bool end=false;
	while (!end) {
		cout << "Please enter the new exponent you want to assign: ";
		int new_exp=0;
		cin >> new_exp;
		if(new_exp>ncen||new_exp<0){
			cout << "Sorry, wrong input, try again!\n";
			continue;
		}
		exponents[nr-1]=new_exp;
		end=true;
		set_modified();
	}
};

void WFN::change_center(int nr){
	bool end=false;
	while (!end){
		cout << "Please enter the new center you want to assign: ";
		int new_center=0;
		cin >> new_center;
		if(new_center>ncen||new_center<0){
			cout << "Sorry, wrong input, try again!\n";
			continue;
		}
		centers[nr-1]=new_center;
		end=true;
		set_modified();
	}
};

void WFN::change_MO_coef(int nr){
	cout << "Which coefficient out of "<< nex << " do you want to change?\n";
	int sel=0;
	bool end=false;
	while (!end){
		cin >> sel;
		if(sel>nex||sel<=0){
			cout << "sorry this is not inside the possible range\n";
			continue;
		}
		MOs[nr-1].change_coefficient (sel);
		end=true;
	}
	set_modified();
};

bool WFN::change_MO_coef(int nr_mo, int nr_primitive, double value, bool debug){
	if(nr_mo>=MOs.size()){
		cout << "MO doesn't exist!" << endl;
		return false;
	}
	return MOs[nr_mo].change_coefficient (nr_primitive,value,debug);
};

void WFN::list_primitives(){
	for (int i=0; i<nex;i++) cout << i << " center: " << centers[i] << " type: " << types[i] << " exponent: " << exponents[i] << endl;
};

bool WFN::remove_center(int nr){
	erase_center(nr);
	try{
		for (int i=0; i<nex; i++) if(centers[i]==nr) remove_primitive (i);
	}
	catch(...){
		return false;
	}
	set_modified();
	return true;
}

bool WFN::add_exp(int cent, int type, double e){
	nex++;
	if(!push_back_center (cent)||!push_back_type (type)||!push_back_exponent (e)) return false;
	else return true;
};

double WFN::get_MO_coef(int nr_mo, int nr_primitive, bool debug){
	if(nr_mo < MOs.size() && nr_mo>=0) return MOs[nr_mo].get_coefficient(nr_primitive, debug); 
	else {
		if(debug) cout << "WRONG INPUT TO get_MO_coef!" << endl;
		return -1;
	}
};

int WFN::get_MO_primitive_count(int nr_mo){
	if(nr_mo <= MOs.size() && nr_mo>=0) return MOs[nr_mo].get_primitive_count();
	else return -1;
};

string WFN::hdr(){
	string temp = "GAUSSIAN            ";
	if (nmo>100) temp.append(to_string(nmo));
	else if(nmo<100&nmo>10){
		temp.append( " " );
		temp.append(to_string(nmo));
	}
	else if(nmo<10&&nmo>0){
		temp.append("  ");
		temp.append(to_string(nmo));
	}
	else if(nmo<0) return "PROBLEM";
	temp.append(" MOL ORBITALS    ");
	if (nex>100) temp.append(to_string(nex));
        else if(nex<100&nex>10){
                temp.append(" ");
                temp.append(to_string(nex));
        }
        else if(nex<10&&nex>0){
                temp.append("  ");
                temp.append(to_string(nex));
        }
	else if(nex<0) return "PROBLEM";
	temp.append(" PRIMITIVES      ");
	if (ncen>100) temp.append(to_string(ncen));
        else if(ncen<100&ncen>10){
                temp.append(" ");
                temp.append(to_string(ncen));
        }
        else if(ncen<10&&ncen>0){
                temp.append("  ");
                temp.append(to_string(ncen));
        }
        else if(ncen<0) return "PROBLEM";
	temp.append(" NUCLEI\n");
	return temp;
};

bool WFN::read_wfn(string fileName, bool debug){
	if(debug){
		debug_wfn=true;
		debug_wfn_deep=true;
	}
	if(exists(fileName)){
		if(debug_wfn) printf("File is valid, continuing...\n");
	}
	else{
		cout << "couldn't open or find " << fileName <<", leaving" << endl;
		return false;
	}
	ifstream rf(fileName.c_str());
	string line;
	rf.seekg(0);
	getline(rf,line);
	comment = line;
	getline(rf,line);
	stringstream stream(line);
	string header_tmp;
	int e_nmo, e_nex, e_nuc=0; //number of expected MOs, Exponents and nuclei
	stream >> header_tmp >> e_nmo >> header_tmp >> header_tmp >> e_nex >> header_tmp >> e_nuc;
	if(debug){
		printf("e_nmo: %d, e_nex: %d, e_nuc: %d\n",e_nmo,e_nex,e_nuc);
		Enter();
	}
	//----------------------------- Read Atoms ------------------------------------------------------------
	int dum_nr[e_nuc],dum_ch[e_nuc];
	vector<string> dum_label;
	double dum_x[e_nuc], dum_y[e_nuc], dum_z[e_nuc];
	char tempchar[200];
	size_t length;
	for(int i=0; i<e_nuc; i++){
		getline(rf,line);
		if(debug) cout << i << ".run, line:" << line << endl;
		length = line.copy(tempchar,2,2);
		tempchar[length]='\0';
		if(debug) cout << "First line-copy succesfull" << endl;
		string temp;
		temp=tempchar;
		dum_label.push_back(temp);
		length = line.copy(tempchar,2,6);
		tempchar[length]='\0';
		sscanf(tempchar,"%d",&dum_nr[i]);
		length = line.copy(tempchar,12,24);
		tempchar[length]='\0';
		sscanf(tempchar,"%lf",&dum_x[i]);
		length = line.copy(tempchar,12,36);
		tempchar[length]='\0';
		sscanf(tempchar,"%lf",&dum_y[i]);
		length = line.copy(tempchar,12,48);
		tempchar[length]='\0';
		sscanf(tempchar,"%lf",&dum_z[i]);
		length = line.copy(tempchar,1,72);
		tempchar[length]='\0';
		sscanf(tempchar,"%d",&dum_ch[i]);
		if(debug){
			cout << "label:" << dum_label[i] 
			<< " nr: " << dum_nr[i] 
			<< " x: " << dum_x[i] 
			<< " y: " << dum_y[i] 
			<< " z: " << dum_z[i] 
			<< " charge: " << dum_ch[i] << endl ;
		}
	}
	if(debug) Enter();
	//------------------------------------ Read center assignements -------------------------------------------
	int dum_center[e_nex];
	if(debug) for( int i=0; i< e_nex; i++) dum_center[i]=99;
	int run=0;
	int exnum=0;
	getline(rf,line);
	while (line.compare(0,6,"CENTRE")==0&&!rf.eof()) {
		if(debug) cout << "run: " << run << " exnum: " << exnum << " line: " << line << endl;
		if(exnum+20<=e_nex){
			if(debug) cout << "dum_center: ";
			for(int i=0; i<20; i++){
				length = line.copy(tempchar,2,21+3*i);
				tempchar[length]='\0';
				if(debug) cout << "tempchar: " << tempchar << endl; 
				sscanf(tempchar,"%d",&dum_center[exnum]);
				if(debug) cout << dum_center[exnum];
				if (dum_center[exnum]>e_nuc){ 
					printf("this center doesn't exist.. some weird problem!\n");
					return false;
				}
				exnum++;
			}
			if (debug) cout << endl << "run: " << run*20 << "   center: " << dum_center[exnum] << endl;
		}
		else{
			if (debug) cout << "exnum+20>e_nex...\n";
			if (exnum<e_nex){
				for(int i=0; i<e_nex%20;i++){
					length = line.copy(tempchar,2,21+3*i);
					tempchar[length]='\0';
					if(debug) cout << "tempchar: " << tempchar << endl; 
					sscanf(tempchar,"%d",&dum_center[exnum]);
					if(debug) cout << dum_center[exnum] << endl;
					if (dum_center[exnum]>e_nuc){ 
						printf("this center doesn't exist.. some weird problem!\n");
						return false;
					}
					exnum++;
				}
			}
			else{
				getline(rf,line);
				continue;
			}
		}
		getline(rf,line);
		if (exnum>e_nex){
			printf("run went higher than expected values in center reading, thats suspicius, lets stop here...\n");
			return false;
		}
		run++;
	}
	if(debug){
		cout << exnum << endl;
		Enter();
	}
	if (exnum < e_nex){
		printf("We have a problem adding center assignements!\n");
		return false;
	}
	if (debug_wfn) printf("finished with centers, moving to types...\n");
	//------------------------------------ Read Types ---------------------------------------------------------
	int dum_type[e_nex];
	run=0;
	exnum=0;
	while (line.compare(0,4,"TYPE")==0&&!rf.eof()) {
		if(debug_wfn_deep) cout << "run: " 
			<< run << " exnum: " 
			<< exnum << " line: " 
			<< line << endl;
		if(exnum+20<=e_nex){
			if(debug_wfn_deep) cout << "dum_type: ";
			for(int i=0; i<20; i++){
				length = line.copy(tempchar,2,21+3*i);
				tempchar[length]='\0';
				if(debug_wfn_deep) cout << "tempchar: " << tempchar << endl; 
				sscanf(tempchar,"%d",&dum_type[exnum]);
				if(debug_wfn_deep) cout << dum_type[exnum];
				exnum++;
			}
			if (debug_wfn_deep) cout << endl;
			if (debug_wfn_deep) cout << "run: " << run*20 << "   type: " << dum_type[exnum] << endl;
		}
		else if (exnum<e_nex){
			for(int i=0; i<e_nex%20;i++){
				length = line.copy(tempchar,2,21+3*i);
				tempchar[length]='\0';
				sscanf(tempchar,"%d",&dum_type[exnum]);
				exnum++;
			}
		}
		else{
			getline(rf,line);
			continue;
		}
		getline(rf,line);
		if (exnum>e_nex){
			printf("exnum went higher than expected values in type reading, thats suspicius, lets stop here...\n");
			return false;
		}
		run++;
	}
	if (exnum < e_nex){
		printf("We have a problem adding type assignements!\n");
		return false;
	}
	if(debug_wfn) printf("finished with types, reading exponents now...\n");
	//----------------------------- Read exponents -------------------------------
	double dum_exp[e_nex];
	run=0;
	exnum=0;
	string replace ="E";
	while (line.compare(0,9,"EXPONENTS")==0&&!rf.eof()){
		if(debug_wfn_deep) cout << "run: " 
			<< run << " exnum: " 
			<< exnum << " line: " 
			<< line << endl;
		if(exnum+5<=e_nex){
			if(debug_wfn_deep) cout << "dum_exp: ";
			for(int i=0; i<5; i++){
				line.replace(20+i*14,1,replace);
				length = line.copy(tempchar,13,11+14*i);
				tempchar[length]='\0';
				if(debug_wfn_deep) cout << "	tempchar: " << tempchar << endl; 
				sscanf(tempchar,"%lG",&dum_exp[exnum]);
				if(debug_wfn_deep) cout << dum_exp[exnum];
				exnum++;
			}
			if (debug_wfn_deep) cout << endl;
			if (debug_wfn_deep) cout << "run: " << run*5 << "   exponent: " << dum_exp[exnum-1] << endl;
		}
		else if (exnum<e_nex){
			for(int i=0; i<e_nex%5;i++){
				line.replace(20+i*14,1,replace);
				length = line.copy(tempchar,13,11+14*i);
				tempchar[length]='\0';
				sscanf(tempchar,"%lG",&dum_exp[exnum]);
				exnum++;
			}
		}
		else{
			getline(rf,line);
			continue;
		}
		getline(rf,line);
		if (exnum>e_nex){
			printf("exnum went higher than expected values in exponent reading, thats suspicius, lets stop here...\n");
			return false;
		}
		run++;
	}
	if (exnum < e_nex){
		printf("We have a problem adding exponents!\n");
		return false;
	}
	if(debug_wfn){
		printf("finished with exponents, reading MOs now...\n");
		cout << "line: " << line << endl;
	}
	int linecount=0;
	exnum=0;
	int monum=0;
	double temp_val[e_nmo][e_nex];
	//-------------------------------- Read MOs --------------------------------------
	while (!line.compare(0,3,"END")==0&&!rf.eof()) {
		bool b=0;
		if(monum==e_nmo){
			if(debug_wfn) cout << "read all MOs i expected, finishing read...." << endl;
			b=true;
			break;
		}
		stringstream stream(line);
		string tmp;
		int temp_nr=0;
		float temp_occ=0.0;
		double temp_ener=0.0;
		stream >> tmp >> temp_nr >> tmp >> tmp >> tmp >> tmp >> tmp >> temp_occ >> tmp >> tmp >> tmp >> temp_ener;
		push_back_MO(temp_nr,temp_occ,temp_ener);
		if(debug_wfn_deep){
			cout << "This is the header for the new MO: " << MOs[monum].hdr() << endl;
		}
		//---------------------------Start reading MO coefficients----------------------- 
		getline(rf,line);
		linecount=0;
		exnum=0;
		while (!line.compare(0,2,"MO")==0&&!rf.eof()) {
			if(debug_wfn_deep) cout << "linecount: " 
				<< linecount << " exnum: " 
				<< exnum  << " monum: " 
				<< monum << " line: " 
				<< line << endl;
			if(exnum+5<e_nex){
				for(int i=0; i<5; i++){
					line.replace(12+i*16,1,replace);
					length = line.copy(tempchar,15,1+16*i);
					tempchar[length]='\0';
					if(debug_wfn_deep) cout << "	tempchar: " << temp_val[monum][exnum] << endl; 
					sscanf(tempchar,"%lG",&temp_val[monum][exnum]);
					if(debug_wfn_deep){
						cout << "temp_val: ";
						cout << scientific << setprecision(8) << temp_val[monum][exnum] << endl;
						cout << "Making coef now...\n";
					}
					if(debug_wfn_deep) cout << temp_val[monum][exnum] << endl;
					exnum++;
				}
				if (debug_wfn_deep){
					cout << endl;
					cout << "linecount: " << linecount*5 << "   exponent: " << temp_val[monum][exnum] << endl;
				}
			}
			else if (exnum<e_nex){
				if(debug_wfn) cout << "This should be the last line! e_nex: " << e_nex << " exnum: " << exnum << endl;
				for(int i=0; i<(e_nex%5);i++){
					line.replace(12+i*16,1,replace);
					length = line.copy(tempchar,15,1+16*i);
					tempchar[length]='\0';
					if(debug_wfn_deep) cout << "	tempchar: " << tempchar << endl;
					sscanf(tempchar,"%lG",&temp_val[monum][exnum]);
					if(debug_wfn_deep){
						cout << "temp_val: ";
						cout << scientific << setprecision(8) << temp_val[monum][exnum] << endl;
						cout << "Making coef now...\n";
					}
					if(debug_wfn_deep) cout << temp_val[monum][exnum] << endl;
					exnum++;
				}
			}
			else{
				getline(rf,line);
				continue;
			}
			getline(rf,line);
			if (linecount*5>e_nex+1){
				printf("linecount went higher than expected values in exponent reading, thats suspicius, lets stop here...\n");
				return false;
			}
			run++;
		}
		monum++;
		if(debug_wfn_deep) cout << "monum after: " << monum << endl;
		if(b==true) break;
	}
	if(monum+1 < e_nmo){
		printf("less MOs than expected, quitting...\n");
		if(debug_wfn_deep) cout << "monum: " << monum << " e_nmo: " << e_nmo << endl;
		return false;
	}
	//---------------------Start writing everything from the temp arrays into wave ---------------------
	if(debug_wfn) printf("finished with reading the file, now i'm going to make everything permantent in the wavefunction...\n");

	for(int i=0; i<e_nuc; i++) if(!push_back_atom(dum_label[i],dum_x[i],dum_y[i],dum_z[i],dum_ch[i])) cout << "Error while making atoms!!\n";
	if(debug){
		cout << "Starting to check whether it is bohr or Angstrom input!" << endl;
		Enter();
	}
	if(check_bohr(*this, debug)){
		for(int i=0; i<e_nuc; i++){
			atoms[i].x=atoms[i].x*0.529177;
			atoms[i].y=atoms[i].y*0.529177;
			atoms[i].z=atoms[i].z*0.529177;
		}
	}
	for(int j=0; j<e_nex; j++){
		if(debug_wfn_deep) cout << j;
		if(!add_exp (dum_center[j],dum_type[j],dum_exp[j])){
			cout << "Error while making primitive!!\n";
			Enter();
		}
	}
	for(int i=0; i<e_nmo; i++){
		for(int j=0; j<e_nex; j++){
			if(debug_wfn_deep) cout << "temp value: " << temp_val[i][j] << "  ";
			if(!MOs[i].push_back_coef (temp_val[i][j],nex)){
				cout << "Error while writing MO coefficients...\n";
				Enter();
			}
		}
		cout << endl << endl;
	}
	if(debug) Enter();
};

double WFN::get_atom_coordinate(int nr, int axis, bool debug){
	if(nr<0||nr>=ncen||axis<0||axis>2){
		cout << "This input is incalid for get_atom_coordinate!" << endl;
		return -1;
	}
	else{
		switch(axis){
			case 0: return atoms[nr].x; break;
			case 1: return atoms[nr].y; break;
			case 2: return atoms[nr].z; break;
		}
	}
};

bool WFN::write_wfn(string fileName, bool debug){
	if(debug){
		debug_wfn=true;
		debug_wfn_deep=true;
	}
	if(exists(fileName)){
		printf("File already existed, do you want to overwrite it?");
		if(!yesno()) return false;
	}
	else {
		if(debug_wfn) cout << "File didn't exist before, writing comment to it now." << endl;
	}
	ofstream rf(fileName.c_str(), ios::out);
	string line;
	char choice;
	if(!rf.is_open()){
		cout << "Sorry, can't open the file...\n";
		return false;
	}
	rf << comment << endl;
	if(debug_wfn) cout << "comment written, now for the header..\n";
	rf << hdr();
	if(debug_wfn){
		cout << "header written, now for the centers..\n";
		cout << "this is the header: \n" << hdr();
	}
	rf.flush();
	for (int i=0; i<ncen; i++){
		rf << "  " << atoms[i].label;
		if(i<9) rf << "   ";
		else rf << "  ";
		rf << i+1 << "    (CENTRE ";
		if(i<9) rf << ' ';
		rf << i+1 << ") ";
		rf << fixed << showpoint << setprecision(8);
		rf << setw(12) << atoms[i].x;
		rf << setw(12) << atoms[i].y;
		rf << setw(12) << atoms[i].z;
		rf << "  CHARGE = ";
		rf << fixed << showpoint << setprecision(1) << setw(2) << atoms[i].charge;
		rf << ".0";
		rf << '\n';
	}
	if(debug_wfn) cout << "centers written, now for the center_assignement..\n";
	if(debug_wfn) cout << "ncen: " << ncen << " nex: "<< nex << " nmo: " << nmo << endl; 
	int run=0;
	int exnum=0;
	for (int i=0; i<nex/20; i++){
		rf << "CENTRE ASSIGNMENTS  ";
		for (int j=0; j<20; j++){
			rf << setw(3) << centers[exnum];
			if(exnum>nex){
				printf("run is too big in center writing");
				if(debug_wfn) cout << "in 20er-lines...\n";
				return false;
			}
			exnum++;
		}
		run++;
		rf << '\n';
	}
	if(debug_wfn) cout << "this should be the last line... \n";
	if(exnum<nex){
		rf << "CENTRE ASSIGNMENTS  ";
		for (int j=0; j<nex%20; j++){
			rf << setw(3) << centers[exnum];
			if(exnum>nex){
				printf("run is too big in center writing");
				if(debug_wfn) cout << " in last line... trying to access # "<< exnum << "\n";
				return false;
			}
			exnum++;
		}
		rf << '\n';
	}
	if(run*20<nex/20-1){
		printf("Problem during writing of Centre assignments... stopping...\n");
		return false;
	}
	if(debug_wfn) cout << "center assignements written, now for the types..\n";
	run=0;
	exnum=0;
	for (int i=0; i<nex/20; i++){
		rf << "TYPE ASSIGNMENTS    ";
		for (int j=0; j<20; j++){
			rf << setw(3) << types[exnum];
			if(exnum>nex){
				printf("run is too big in types writing\n");
				return false;
			}
			exnum++;
		}
		run++;
		rf << '\n';
	}
	if(exnum<nex){
		rf << "TYPE ASSIGNMENTS    ";
		int final_j=0;
		for (int j=0; j<nex%20; j++){
			rf << setw(3) << types[exnum];
			if(exnum>nex){
				printf("run is too big in types writing");
				return false;
			}
			final_j=j;
			exnum++;
		}
		if(debug_wfn) cout << "final_j: " << final_j << endl;
		rf << '\n';
	}
	if(run*20<nex/20-1){
		printf("Problem during writing of Type assignments... stopping...");
		return false;
	}
	if(debug_wfn) cout << "types assignements written, now for the exponents..\n";
	run = 0;
	exnum=0;
	string replace =("D");
	for (int i=0; i<nex/5; i++){
		rf << "EXPONENTS ";
		for (int j=0; j<5; j++){
			stringstream stream;
			string temp;
			stream << scientific << setw(14) << setprecision(7) << exponents[exnum];
			temp = stream.str();
			temp.replace(10,1,replace);
			rf << temp;
			if(exnum>nex){
				printf("run is too big in exponents writing");
				return false;
			}
			exnum++;
		}
		run++;
		rf << '\n';
	}
	if(exnum<nex){
		rf << "EXPONENTS ";
		for (int j=0; j<nex%5; j++){
			stringstream stream;
			string temp;
			stream << scientific << setw(14) << setprecision(7) << exponents[exnum];
			temp = stream.str();
			temp.replace(10,1,replace);
			rf << temp;
			if(run>nex){
				printf("run is too big in exponents writing");
				return false;
			}
			exnum++;
		}
		rf << '\n';
	}
	if(run*5<nex/5-1){
		printf("Problem during writing of Exponents... stopping...");
		return false;
	}
	if(debug_wfn) cout << "exponents assignements written, now for the MOs.." << endl << "For informational purposes: ncen " 
		<< ncen << " nmo " << nmo << " nex " << nex << endl;
	for(int mo_counter=0;mo_counter<nmo;mo_counter++){
		rf << MOs[mo_counter].hdr();
		run=0;
		for (int i=0; i<nex/5; i++){
			for (int j=0; j<5; j++){
				stringstream stream;
				string temp;
				stream << scientific << showpoint << setprecision(8) << setw(16)<<  MOs[mo_counter].get_coefficient(run, debug);
				temp = stream.str();
				temp.replace(12,1,replace);
				rf << temp;
				if(run>nex){
					cout << "run (" << run <<") is too big in MO ceofficients writing" << endl;
					return false;
				}
				run++;
			}
			rf << '\n';
		}
		if(run<nex){
			if(debug_wfn_deep) cout << "Still some left to write... going in % for loop...." << endl;
			for (int j=0; j<nex%5; j++){
				stringstream stream;
				string temp;
				stream << scientific << showpoint << setprecision(8) << setw(16)<<  MOs[mo_counter].get_coefficient(run, debug);
				temp = stream.str();
				temp.replace(12,1,replace);
				rf << temp;
				if(run>nex){
					cout << "run (" << run <<") is too big in MO ceofficients writing" << endl;
					return false;
				}
				run++;
			}
			rf << '\n';
		}
	}
	if(run!=nex){
		printf("Problem during writing of MOs... stopping...");
		if(debug_wfn_deep) cout << "run: " << run << endl;
		return false;
	}
	rf << "END DATA" << endl;
	rf.flush();
	rf.close();
	return true;
};

void WFN::print_primitive(int nr){
	cout << "center assignement: " << centers[nr] << " type: " << types[nr] 
		<< " exponent: " << exponents[nr] << endl << "MO coefficients:";
		for(int i=0; i<nmo; i++){
			cout << MOs[nr].get_coefficient(i,false) <<"   ";
			if( i%5==0) cout << endl;
		}
};

double WFN::get_atom_basis_set_exponent(int nr_atom, int nr_prim) {
	if(nr_atom<=ncen && nr_atom>=0 && atoms[nr_atom].basis_set.size()>=nr_prim && nr_prim>=0)
		return atoms[nr_atom].basis_set[nr_prim].exponent;
	else return -1;
};

double WFN::get_atom_basis_set_coefficient(int nr_atom, int nr_prim) {
	if(nr_atom<=ncen && nr_atom>=0 && atoms[nr_atom].basis_set.size()>=nr_prim && nr_prim>=0) 
		return atoms[nr_atom].basis_set[nr_prim].coefficient; 
	else return -1;
};

bool WFN::change_atom_basis_set_exponent (int nr_atom, int nr_prim, double value) { 
	if(nr_atom<=ncen && nr_atom>=0 && atoms[nr_atom].basis_set.size()>=nr_prim && nr_prim>=0) {
		atoms[nr_atom].basis_set[nr_prim].exponent=value;
		set_modified();
		return true;
	}
	else return false;
};

bool WFN::change_atom_basis_set_coefficient (int nr_atom, int nr_prim, double value) { 
	if(nr_atom<=ncen && nr_atom>=0 && atoms[nr_atom].basis_set.size()>=nr_prim && nr_prim>=0) {
		atoms[nr_atom].basis_set[nr_prim].coefficient=value;
		set_modified();
		return true;
	}
	else return false;
};

int WFN::get_atom_primitive_count(int nr){
	if( nr<=ncen && nr>=0) return atoms[nr].basis_set.size();
	else return -1;
};

bool WFN::erase_atom_primitive(int nr, int nr_prim){
	if( nr <=ncen && nr >=0 && nr_prim>=0 && nr_prim < atoms[nr].basis_set.size()){
		atoms[nr].basis_set.erase(atoms[nr].basis_set.begin()+nr_prim);
		return true;
	}
	else return false;
};

int WFN::get_basis_set_shell (int nr_atom, int nr_prim){
	if( nr_atom<=ncen && nr_atom>=0 && atoms[nr_atom].basis_set.size()>=nr_prim && nr_prim>=0 ) {
		return atoms[nr_atom].basis_set[nr_prim].shell;
	}
	else return -1;
};

int WFN::get_atom_shell_count(int nr){
	if( nr<=ncen && nr>=0 ) return atoms[nr].shellcount.size();
	else return -1;
};

int WFN::get_atom_shell_primitives(int nr_atom, int nr_shell){
	if( nr_atom<=ncen && nr_atom>=0 && nr_shell<atoms[nr_atom].shellcount.size() && nr_shell>=0 ) 
		return atoms[nr_atom].shellcount[nr_shell];
	else return -1;
};

int WFN::get_shell_type(int nr_atom, int nr_shell){
	if( nr_atom<=ncen && nr_atom>=0 && nr_shell<=atoms[nr_atom].shellcount.size() && nr_shell>=0 ) {
		int primitive_counter=0;
		while (atoms[nr_atom].basis_set[primitive_counter].shell!=nr_shell)	primitive_counter++;
		return atoms[nr_atom].basis_set[primitive_counter].type;
	}
	else return -1;
};

int WFN::get_shell_start (int nr_atom, int nr_shell, bool debug){
	if( nr_atom<=ncen && nr_atom>=0 && nr_shell<=atoms[nr_atom].shellcount.size()-1 && nr_shell>=0 ) {
		int primitive_counter=0;
		while (primitive_counter < atoms[nr_atom].basis_set.size() && atoms[nr_atom].basis_set[primitive_counter].shell!=nr_shell){
			primitive_counter++;
			if(debug) cout << primitive_counter << " " << endl;
		}
		return primitive_counter;
	}
	else return -1;
};

int WFN::get_shell_end (int nr_atom, int nr_shell, bool debug){
	if( nr_atom<=ncen && nr_atom>=0 && nr_shell<=atoms[nr_atom].shellcount.size() && nr_shell>=0 ) {
		/*if(nr_shell==0) return atoms[nr_atom].basis_set.size();
		else */ 
		if(nr_shell==atoms[nr_atom].shellcount.size()-1) return atoms[nr_atom].basis_set.size()-1;
		int primitive_counter=0;
		while (atoms[nr_atom].basis_set[primitive_counter].shell!=(nr_shell+1))	primitive_counter++;
		return primitive_counter-1;
	}
	else return -1;
};

string WFN::get_atom_label(int nr){
	string error_return;
	error_return='?';
	if(nr<ncen && nr>=0) return atoms[nr].label;
	else return error_return;
};

int WFN::get_nr_basis_set_loaded(){
	int count=0;
	for(int a =0; a< ncen; a++) if(atoms[a].get_basis_set_loaded ()) count++;
	return count;
};

bool WFN::get_atom_basis_set_loaded(int nr){
	if( nr<=ncen && nr>=0 ) return atoms[nr].get_basis_set_loaded ();
	else {
		cout << "invalid atom choice in atom_basis_set_loaded!" << endl;
		return false;
	}
};

void WFN::push_back_DM(double value){
	DensityMatrix.push_back(value);
};

double WFN::get_DM(int nr){
	if(nr>=0&& nr < DensityMatrix.size() ) return DensityMatrix[nr];
	else{
		cout << "Requested nr out of range! Size: " << DensityMatrix.size() << " nr: " << nr << endl;
		return -1;
	}
};

bool WFN::set_DM(int nr, double value){
	if(nr>=0&& nr < DensityMatrix.size() ) return DensityMatrix[nr]=value;
	else{
		cout << "invalid arguments for set_DM! Input was: " << nr << ";" << value << endl;
		return false;
	}
};

int WFN::check_order(bool debug){
	for(int i=0; i< ncen; i++){
		if(!get_atom_basis_set_loaded (i)){
			cout << "Sorry, consisttency check only works if basis set is loaded for all atoms!" << endl;
			Enter();
			return -1;
		}
	}
	//---------------------------check if the wfn is in the right order----------------------
	int order=0; //1= gaussian (P=2222 3333 4444) 2= tonto (234 234 234 234) 3= ORCA (423 423 423 423)
	int f_order=0; //1=gaussian (F=11 12 13 17 14 15 18 19 16 20) 2=tonto=ORCA 3=ORCA (11 12 13 14 15 17 16 18 19 20)
	int primcounter=0;
	bool order_found=false;
	for(int a=0; a< get_ncen(); a++){
		for(int s=0; s< get_atom_shell_count(a); s++){
			int type_temp=get_shell_type(a,s);
			if(type_temp==-1){
				cout << "ERROR in type assignement!!"<<endl;
				Enter();
			}
			switch (type_temp){
				case 1:
					for(int i=0; i< get_atom_shell_primitives (a,s); i++){
						if(types[primcounter]!=1){
							order=-1;
							if(debug){
								cout << "This should not happen, the order of your file is not ok for S-types! Checked #:" << primcounter << endl;
								Enter();
								return -1;
							}
						}
						else primcounter++; 
					}
					break;
				case 2:
					if(order_found){
						if(order==1){
							for(int r=0;r<get_atom_shell_primitives (a,s); r++){
								if(types[primcounter]!=2||types[primcounter+get_atom_shell_primitives (a,s)]!=3
								   ||types[primcounter+2*get_atom_shell_primitives (a,s)]!=4){
									if(debug){
										cout << "The found order does not match all type entries! primcounter: " << primcounter << endl;
										Enter();
									}
									return -1;
								}
								primcounter++;
							}
							primcounter+=2*get_atom_shell_primitives (a,s);
						}
						else if(order==2){
							for(int r=0;r<get_atom_shell_primitives (a,s); r++){
							if(types[primcounter]!=2||types[primcounter+1]!=3||types[primcounter+2]!=4){
									if(debug){
										cout << "The found order does not match all type entries! primcounter: " << primcounter << endl;
										Enter();
									}
									return -1;
								}
								primcounter+=3;
							}
						}
						else if(order==3)
							{
							for(int r=0;r<get_atom_shell_primitives (a,s); r++){
								if(types[primcounter]!=4||types[primcounter+1]!=2||types[primcounter+2]!=3){
									if(debug){
									cout << "The found order does not match all type entries! primcounter: " << primcounter << endl;
										Enter();
									}
									return -1;
								}
								primcounter+=3;
							}
						}
					}
					else{
						if(types[primcounter]==2){
							if(debug&&a==0){
								cout << "Seems to be either tonto or gaussian file..." << endl;
								Enter();
							}
							order=1;
							if(types[primcounter+1]==3){
								order=2;
								order_found=true;
								if(debug){
									cout << "This wfn file is in tonto order!" << endl;
									Enter();
								}
							}
							else if(types[primcounter+1]==2&&get_atom_shell_primitives (a,s)>1){
								order=1;
								order_found=true;
								if(debug){
									cout << "This wfn file is in gaussian order!" << endl;
									Enter();
								}
							}
							else{
								order=1;
								order_found=true;
								if(debug){
									cout << "Either some error or this shell only has 1 p-primitive and "
									<< "i didn't find any order yet... assuming gaussian" << endl;
									Enter();
								}
							}
						}
						else if(types[primcounter]==4){
							if(debug&&a==0){
								cout << "Seems as if this file was ORCA ordered..." << endl;
								Enter();
							}
							order=3;
							if(types[primcounter+1]==2){
								if(debug){
									cout << "Yep, that's right! making it permanent now!" << endl;
									Enter();
								}
								order_found=true;
							}
						}
						else{
							order=-1;
							cout << "I can't recognize this order of the .wfn file..." << endl;
							Enter();
							return -1;
						}
						s--;
					}
					break;
				case 3:
					for(int i=0; i< get_atom_shell_primitives (a,s); i++){
						if(types[primcounter]!=5||types[primcounter+1]!=6||types[primcounter+2]!=7
						   ||types[primcounter+3]!=8||types[primcounter+4]!=9||types[primcounter+5]!=10){
							order=-1;
							if(debug){
								cout << "The checked types are 6 from #" << primcounter << " and are:" << endl;
								cout << types[primcounter] << " " << types[primcounter+1] << " " << types[primcounter+2] << " "
									<< types[primcounter+3] << " " << types[primcounter+4] << " " << types[primcounter+5] << endl;
								cout << "Something seems to be wrong in the order of your D-Types..." << endl;
								Enter();
								return -1;
							}
						}
						else primcounter+=6; 
					}
					break;
				case 4:
					for(int i=0; i< get_atom_shell_primitives (a,s); i++){
						if(types[primcounter]!=11||types[primcounter+1]!=12||types[primcounter+2]!=13
						   ||types[primcounter+3]!=17||types[primcounter+4]!=14||types[primcounter+5]!=15
						   ||types[primcounter+6]!=18||types[primcounter+7]!=19||types[primcounter+8]!=16||types[primcounter+9]!=20){
							if(types[primcounter]!=11||types[primcounter+1]!=12||types[primcounter+2]!=13
								||types[primcounter+3]!=14||types[primcounter+4]!=15||types[primcounter+5]!=17
								||types[primcounter+6]!=16||types[primcounter+7]!=18||types[primcounter+8]!=19||types[primcounter+9]!=20){
									order=-1;
									if(debug){
										cout << "The checked types are 10 from #" << primcounter << " and are:" << endl;
										cout << types[primcounter] << " " << types[primcounter+1] << " " << types[primcounter+2] << " "
											<< types[primcounter+3] << " " << types[primcounter+4] << " " << types[primcounter+5] << " "
											<< types[primcounter+6] << " " << types[primcounter+7] << " " << types[primcounter+8] << " " 
											<< types[primcounter+9] << endl;
										cout << "Something seems to be wrong in the order of your F-Types..." << endl;
										Enter();
										return -1;
									}
							}
							else{
								f_order=3;
								primcounter+=10;
							}
						}
						else {
							f_order=1;
							primcounter+=10;
						} 
					}
					break;
			}
		}
	}
	if(debug){
		cout << "Going to return " << f_order*10+order << endl;
		Enter();
	}
	return (f_order*10+order);
};

bool WFN::sort_wfn(int order, bool debug){
	set_modified ();
	int primcounter=0;
	int f_order;
	if(order>=10&&order<20){
		f_order=1;
		order-=10;
	}
	else if(order>=20&&order<30){
		f_order=2;
		order-=20;
	}
	else if(order >=30&&order<40){
		f_order=3;
		order-=30;
	}
	switch(order){
		case 1:
			for(int a=0; a< get_ncen(); a++){
				for(int s=0; s< get_atom_shell_count(a); s++){
					int type_temp=get_shell_type(a,s);
					if(type_temp!=2){
						switch (type_temp){
							case 1:
								primcounter+=get_atom_shell_primitives (a,s);
								break;
							case 3:
								primcounter+=get_atom_shell_primitives (a,s)*6;
								break;
							case 4:
								primcounter+=get_atom_shell_primitives (a,s)*10;
								break;
						}
					}
					else{
						int temp_centers[3*get_atom_shell_primitives (a,s)];
						int temp_types[3*get_atom_shell_primitives (a,s)];
						double temp_exponents[get_atom_shell_primitives (a,s)*3];
						double temp_MO_coefficients[get_atom_shell_primitives (a,s)*3][nmo];
						for(int i=0; i<get_atom_shell_primitives (a,s); i++){
							for(int c=0; c<3; c++){
								temp_centers[3*i+c]=centers[primcounter+i+c*get_atom_shell_primitives (a,s)];
								temp_types[3*i+c]=types[primcounter+i+c*get_atom_shell_primitives (a,s)];
								temp_exponents[3*i+c]=exponents[primcounter+i+c*get_atom_shell_primitives (a,s)];
								for(int m=0; m< nmo; m++) 
									temp_MO_coefficients[3*i+c][m]=MOs[m].get_coefficient (primcounter+i+c*get_atom_shell_primitives (a,s),false);
							}
						}
						for(int i=0; i<3*get_atom_shell_primitives (a,s); i++){
							centers[primcounter+i]=temp_centers[i];
							types[primcounter+i]=temp_types[i];
							exponents[primcounter+i]=temp_exponents[i];
							for(int m=0; m< nmo; m++){
								if(!MOs[m].change_coefficient (primcounter+i,temp_MO_coefficients[i][m],false)){
									cout << "Error while assigning new MO coefficient!" << endl;
									Enter();
									return false;
								}
							}
						}
						primcounter+=get_atom_shell_primitives (a,s)*3;
					}
				}
			}
			break;
		case 2:
			if(debug){
				cout << "Nothing to be done here, i like tonto type..." << endl;
				Enter();
			}
			break;
		case 3:		
			for(int a=0; a< get_ncen(); a++){
				for(int s=0; s< get_atom_shell_count(a); s++){
					int type_temp=get_shell_type(a,s);
					if(type_temp!=2){
						switch (type_temp){
							case 1:
								primcounter+=get_atom_shell_primitives (a,s);
								break;
							case 3:
								primcounter+=get_atom_shell_primitives (a,s)*6;
								break;
							case 4:
								primcounter+=get_atom_shell_primitives (a,s)*10;
								break;
						}
					}
					else{
						int temp_center;
						int temp_type;
						double temp_exponent;
						double temp_MO_coefficients[nmo];
						for(int i=0; i<get_atom_shell_primitives (a,s); i++){
							temp_center=centers[primcounter];
							temp_type=types[primcounter];
							temp_exponent=exponents[primcounter];
							for(int m=0; m< nmo; m++) 
									temp_MO_coefficients[m]=MOs[m].get_coefficient (primcounter,false);
							for(int j=0; j<2; j++){
								centers[primcounter+j]=centers[primcounter+1+j];
								types[primcounter+j]=types[primcounter+1+j];
								exponents[primcounter+j]=exponents[primcounter+1+j];
								for(int m=0; m< nmo; m++){
									if(!MOs[m].change_coefficient (primcounter+j,MOs[m].get_coefficient(primcounter+1+j,false),false)){
										cout << "Error while assigning new MO coefficient!" << endl;
										Enter();
										return false;
									}
								}
							}
							centers[primcounter+2]=temp_center;
							types[primcounter+2]=temp_type;
							exponents[primcounter+2]=temp_exponent;
							for(int m=0; m< nmo; m++){
								if(!MOs[m].change_coefficient (primcounter+2,temp_MO_coefficients[m],false)){
									cout << "Error while assigning new MO coefficient!" << endl;
									Enter();
									return false;
								}
							}
							primcounter+=3;
						}
					}
				}
			}
			break;
		default:
			cout << "order type not supported!" << endl;
			Enter();
			return false;
			break;
	}
	primcounter=0;
	switch(f_order){
		case 1:
			for(int a=0; a< get_ncen(); a++){
				for(int s=0; s< get_atom_shell_count(a); s++){
					int type_temp=get_shell_type(a,s);
					if(type_temp!=4){
						switch (type_temp){
							case 1:
								primcounter+=get_atom_shell_primitives (a,s);
								break;
							case 3:
								primcounter+=get_atom_shell_primitives (a,s)*6;
								break;
							case 2:
								primcounter+=get_atom_shell_primitives (a,s)*3;
								break;
						}
					}
					else{
						int temp_center;
						int temp_type;
						double temp_exponent;
						double temp_MO_coefficients[nmo];
						for(int i=0; i<get_atom_shell_primitives (a,s); i++){
							temp_center=centers[primcounter+3];
							temp_type=types[primcounter+3];
							temp_exponent=exponents[primcounter+3];
							for(int m=0; m< nmo; m++) 
									temp_MO_coefficients[m]=MOs[m].get_coefficient (primcounter+3,false);
							for(int j=0; j<2; j++){
								centers[primcounter+3+j]=centers[primcounter+4+j];
								types[primcounter+3+j]=types[primcounter+4+j];
								exponents[primcounter+3+j]=exponents[primcounter+4+j];
								for(int m=0; m< nmo; m++){
									if(!MOs[m].change_coefficient (primcounter+3+j,MOs[m].get_coefficient(primcounter+4+j,false),false)){
										cout << "Error while assigning new MO coefficient!" << endl;
										Enter();
										return false;
									}
								}
							}
							centers[primcounter+5]=temp_center;
							types[primcounter+5]=temp_type;
							exponents[primcounter+5]=temp_exponent;
							for(int m=0; m< nmo; m++){
								if(!MOs[m].change_coefficient (primcounter+5,temp_MO_coefficients[m],false)){
									cout << "Error while assigning new MO coefficient!" << endl;
									Enter();
									return false;
								}
							}
							temp_center=centers[primcounter+8];
							temp_type=types[primcounter+8];
							temp_exponent=exponents[primcounter+8];
							for(int m=0; m< nmo; m++) 
									temp_MO_coefficients[m]=MOs[m].get_coefficient (primcounter+8,false);
							for(int j=0; j<2; j++){
								centers[primcounter+8-j]=centers[primcounter+7-j];
								types[primcounter+8-j]=types[primcounter+7-j];
								exponents[primcounter+8-j]=exponents[primcounter+7-j];
								for(int m=0; m< nmo; m++){
									if(!MOs[m].change_coefficient (primcounter+8-j,MOs[m].get_coefficient(primcounter+7-j,false),false)){
										cout << "Error while assigning new MO coefficient!" << endl;
										Enter();
										return false;
									}
								}
							}
							centers[primcounter+6]=temp_center;
							types[primcounter+6]=temp_type;
							exponents[primcounter+6]=temp_exponent;
							for(int m=0; m< nmo; m++){
								if(!MOs[m].change_coefficient (primcounter+6,temp_MO_coefficients[m],false)){
									cout << "Error while assigning new MO coefficient!" << endl;
									Enter();
									return false;
								}
							}
							primcounter+=10;
						}
					}
				}
			}
			break;
		case 2:
			if(debug){
				cout << "Nothing to be done here, i like tonto f-type..." << endl;
				Enter();
			}
			break;
		case 3:
			if(debug){
				cout << "Nothing to be done here, i like ORCA f-type..." << endl;
				Enter();
			}
			break;
		default:
			cout << "f-order type not supported!" << endl;
			Enter();
			return false;
			break;
	}
	return true;
};
/*
bool WFN::change_center(int nr, int value){
	if(nr>centers.size()||nr<0||value <=0){
		cout << "This is an imposible choice" << endl;
		Enter();
		return false;
	}
	else centers[nr]=value;
};

bool WFN::change_type(int nr, int value){
	if(nr>types.size()||nr<0||value <=0||value>20){
		cout << "This is an imposible choice" << endl;								NOT NEEDED AT THIS POINT
		Enter();
		return false;
	}
	else types[nr]=value;
};

bool WFN::change_exponent(int nr, double value){
	if(nr>exponents.size()||nr<0||value <=0){
		cout << "This is an imposible choice" << endl;
		Enter();
		return false;
	}
	else exponents[nr]=value;
};
*/
