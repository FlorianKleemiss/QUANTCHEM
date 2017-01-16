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

#include "crystal.h"
#include "wfn_class.h"
#include "convenience.h"

using namespace std;

bool read_crystal_geometry(const string &filename, WFN &wave, bool &debug){
	if(exists(filename)){
		if(debug) printf("File is valid, continuing...\n");
	}
	else{
		cout << "couldn't open or find " << filename <<", leaving" << endl;
		return false;
	}
	ifstream rf(filename.c_str());
	string line;
	rf.seekg(0);
	bool found=false;
	int counter=0;
	while(!found&&!rf.eof()){
		getline(rf,line);
		counter++;
		if(line.find("GEOMETRY FOR WAVE FUNCTION")!=-1) found=true; 
	}
	if(rf.eof()){
		cout << "Sorry, file ended before i found the geometry, plase make sure it's in there!" << endl;
		return false;
	}
	found=false;
	counter=0;
	if(debug) cout << "found GEOMETRY here: " << line << " " << counter << endl;
	for(int i=0; i<6; i++) getline(rf,line);
	char tempchar[200];
	size_t length;
	//----------------------------- Read Atoms ------------------------------------------------------------
	vector<string> dum_label;
	vector<double> x;
	vector<double> y;
	vector<double> z;
	while(line.find("T")!=-1&&!rf.eof()){
		if(debug) cout << counter << ".run, line:" << line << endl;
		length = line.copy(tempchar,3,11);
		tempchar[length]='\0';
		string temp(tempchar);
		while(temp.find(" ")!=-1) temp.pop_back();
		dum_label.push_back(temp);
		if(debug) cout << "char: " << tempchar << " string: " << temp << " dum_label: " << dum_label[counter] << endl;
		length = line.copy(tempchar,19,16);
		tempchar[length]='\0';
		double dum=0.0;
		sscanf(tempchar,"%lf",&dum);
		x.push_back(dum);
		length = line.copy(tempchar,19,36);
		tempchar[length]='\0';
		sscanf(tempchar,"%lf",&dum);
		y.push_back(dum);
		length = line.copy(tempchar,19,56);
		tempchar[length]='\0';
		sscanf(tempchar,"%lf",&dum);
		z.push_back(dum);
		if(debug){
			cout << "label:" << dum_label[counter]  
			<< " x: " << x[counter] 
			<< " y: " << y[counter] 
			<< " z: " << z[counter] << endl;
			Enter();
		}
		getline(rf,line);
		counter++;
	}
	if(debug) Enter();
	//---------------------writing geometry to wave---------------------
	if(debug) cout << "finished with reading the file, now i'm going to make everything permantent in the wavefunction..." << endl;
	for(int i=0; i<counter; i++){
		if(!wave.push_back_atom(dum_label[i],x[i],y[i],z[i],1)) cout << "Error while making atoms!!\n";
		if(debug) cout << "atom: " << i << endl;
	}
	if(debug){
		Enter();
		cout << "Returning to main program" << endl;
	}
	rf.close();
	return true;
};

bool read_crystal_DM(const string &filename, WFN &wave, bool &debug){
	if(exists(filename)){
		if(debug) printf("File is valid, continuing...\n");
	}
	else{
		cout << "couldn't open or find " << filename <<", leaving" << endl;
		return false;
	}
	ifstream rf(filename.c_str());
	string line;
	rf.seekg(0);
	bool found=false;
	int counter=0;
	while(!found&&!rf.eof()){
		getline(rf,line);
		counter++;
		if(line.find("P(G) +++ G=   1 (  0  0  0 ) +++")!=-1) found=true; 
	}
	if(rf.eof()){
		cout << "Sorry, file ended before i found the DM, plase make sure it's in there!" << endl;
		return false;
	}
	found=false;
	int shellcount=0;
	int primitivecount=0;
	for(int i=0; i<wave.get_ncen (); i++) {
		shellcount += wave.get_atom_shell_count (i);
		for(int j=0; j< wave.get_atom_shell_count(i); j++){
			if(wave.get_atom_shell_primitives (i,j)!=-1) primitivecount += wave.get_atom_shell_primitives (i,j);
			else{
				cout << "negative number of primitives returned! for atom: "<<i << " shell: " << j << endl;
				Enter();
			}
		}
	}
	if(debug) cout << "There are " << shellcount << " shells in this DM and " << primitivecount << " primitives!" << endl;
	Enter();
	for(int i=0; i<4; i++) getline (rf,line);
	int line_count=1;
	int nr_in_line=0;
	char tempchar[200];
	size_t length;
	while(line.size()>2&&!rf.eof()){
		getline(rf,line);
		line_count++;
	}
	line_count--;
	if(debug){
		cout << "line_count: " << line_count << endl;
		Enter();
	}
	rf.seekg(0);
	while(!found&&!rf.eof()){
		getline(rf,line);
		counter++;
		if(line.find("P(G) +++ G=   1 (  0  0  0 ) +++")!=-1) found=true; 
	}
	if(rf.eof()){
		cout << "Sorry, file ended before i found the DM, plase make sure it's in there!" << endl;
		return false;
	}
	found=false;
	for(int i=0; i<4; i++) getline (rf,line);
	for(int i=0; i<line_count; i++){
		if(debug) cout << "line: " << i << endl << "j: ";
		int buffer=0;
		nr_in_line=0;
		for(int j=0; j<=i; j++){
			if(debug) cout << j << " ";
			if(nr_in_line>9){
				while(line.size()>2&&!rf.eof()) getline (rf,line);
				for(int k=0; k<3; k++) getline (rf,line);
				if(debug) cout << "This is the first line of the next block: " << line << endl;
				while(buffer<i+1&&!rf.eof()){
					getline(rf,line);
					length = line.copy(tempchar,3,2);
					tempchar[length]='\0';
					sscanf(tempchar,"%d",&buffer);
					//if(debug) cout << "buffer: " << buffer << endl;
				}
				if(debug) cout << "This is the line i'm going to read from now: " << line << endl;
				nr_in_line=0;
				buffer=0;
			}
			length = line.copy(tempchar,11,8+(nr_in_line*12));
			tempchar[length]='\0';
			double dum=0.0;
			sscanf(tempchar,"%lf",&dum);
			if(debug) cout << "tempchar: " << tempchar << " dum: " << dum << endl;
			wave.push_back_DM (dum);
			nr_in_line++;
		}
		buffer=0;
		if(i>=9){
			//go back up to start reading from where we left in the first block
			rf.seekg(0);
			while(!found&&!rf.eof()){
				getline(rf,line);
				counter++;
				if(line.find("P(G) +++ G=   1 (  0  0  0 ) +++")!=-1) found=true; 
			}
			if(rf.eof()){
				cout << "Sorry, file ended before i found the DM, plase make sure it's in there!" << endl;
				return false;
			}
			found=false;
			for(int k=0; k<4; k++) getline (rf,line);
			if(debug) cout << "This should be the first line of the first block... " << line << endl;
			while(buffer<i+1&&!rf.eof()){
				getline(rf,line);
				length = line.copy(tempchar,3,2);
				tempchar[length]='\0';
				sscanf(tempchar,"%d",&buffer);
			}
			if(debug) cout << "This is where i ended up after going up again: " << line << endl;
		}
		getline (rf,line);
	}
	if(debug){
		cout << "Number of elements in DM: " << wave.get_DM_size () << endl;
		Enter();
		cout << "Do you want me to print the DM for you now? ";
		if(yesno()){
			for(int i=0; i<wave.get_DM_size(); i++){
			string temp=" ";
			for(int j=0; j<5; j++){
				if(i+j<wave.get_DM_size ()){
						stringstream stream;
						stream << scientific << setw(15) << setprecision(8) << wave.get_DM(i+j) << " ";
						temp += stream.str();
						temp.replace(12+j*16,1,"E");
					}
				}
				i+=4;
				temp+='\n';
				cout << temp;
			}
			Enter();
		}
		Enter();
	}
	rf.close();
	return true;
};