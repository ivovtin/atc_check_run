//скрипт для получения даты по заходам из БД
#include <unistd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <Riostream.h>
#include <vector>

#include "TROOT.h"
#include "TRint.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TMath.h"
#include "TBrowser.h"

#include <getopt.h>
#include <math.h>
#include <stdlib.h>
#include "Common/PlotDate.h"
#include "Common/SetStyle.h"
#include "KDB/kdb.h"

using namespace std;

const float DB_MULTI = 1000.0;
const u_int DB_ID = 1312;
const u_int DB_NP = 480;

string progname;

inline char* timestamp(time_t t)
{
    static const char* time_fmt="%m %d %Y %H %M %S";
    static char strtime[50];

    struct tm* brtm=localtime(&t);
    strftime(strtime,50,time_fmt,brtm);

    return strtime;
}

int main(int argc, char* argv[])
{
    progname=argv[0];
    const string F_RUN = "list_of_run.dat";
    string nf_run  = F_RUN;

    std::ifstream f_in(nf_run.c_str());

    if( f_in.fail() ){
	std::cerr << progname << ": can't open input text file " << nf_run.c_str() << "!" << std::endl;
	return 1;
    }

    int run_N;
    std::vector< int > run;

    while( !f_in.eof() ){
	std::string o;
	while( getline(f_in, o) ){
	    if( o.size()>=1 && o[0]!='#' ){
		std::istringstream i_str(o);
		i_str >> run_N;
		run.push_back(run_N);
		cout<<"Run "<< run_N <<endl;
	    }
	}
    }
    f_in.close();

    ofstream of("list_of_run_time.dat");
    for (vector<int>::iterator it = run.begin(); it != run.end(); ++it )
    {
	time_t runTime_begin; //start time of the current run
	time_t runTime_end; //end time of the current run

	KDBconn* connection=kdb_open();
	if( !connection ) {
	    cerr<<"Can not establish connection to database"<<endl;
	    return 1;
	}

	runTime_begin=kdb_run_get_begin_time(connection, *it);
	runTime_end=kdb_run_get_end_time(connection,  *it);
	//  cout<<"Begin time of "<<run[k]<<": "<<ctime(&runTime)<<endl;
	cout<<"\n========================================================================="<<endl;
	cout<<"Check database...."<<endl;
	cout<<"Begin time of Run"<< *it<<": "<<timestamp(runTime_begin)<<endl;
	cout<<"End time of Run"<< *it<<": "<<timestamp(runTime_end)<<endl;
	of<<*it<<"\t"<<timestamp(runTime_begin)<<endl;
	kdb_close(connection);
    }
    of.close();

    return 0;
}
