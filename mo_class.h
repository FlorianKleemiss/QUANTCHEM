#pragma once
#ifndef __MO_CLASS_H__
#define __MO_CLASS_H__

#include <vector>
#include <string>


class MO {
	private:
		int nr;
		float occ;
		double ener;
		std::vector<double> coefficients;
	public:
		MO();
		MO (int number,float occupation,double energy);
		bool push_back_coef(double val, const int nex);
		bool erase_coef(int nr, const int nex);
		double get_coefficient(int nr, bool debug);
		void change_coefficient(int nr);
		bool change_coefficient(int nr, double value, bool debug);
		void set_nr(int inr) { nr=inr;};
		void set_occ(float iocc) {occ=iocc;};
		void set_ener(double iener) {ener=iener;};
		int get_primitive_count() {return coefficients.size();};
		std::string hdr();
		
};

#endif
