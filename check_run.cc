//программа проверки отключения счетчика (из-за шумов и т.д.) во время захода - проверяю пропадание напряжения
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

const int MAX_CNTS = 160;
const float DB_MULTI = 1000.0;
const u_int DB_ID = 1312;
const u_int DB_NP = 480;

struct cnt{
public:
    float *voltage;
};
cnt cnts[MAX_CNTS];

string progname;
extern int atc_read_runsmonitor(int i);
extern int get_run_data(int run);

static float sumintplum1;
static float sumintelum1;
static float sumlum_p;
static float sumlum_e;
int sumnev;

int Usage(string status)
{
    cout<<"Usage: "<<progname<<" [options] \n"
	<<"Number RUN to read from a file number_run.dat\n"
	<<"Options:\n"
	<<"  -d draw last Run from a file number_run.dat \n"
	<<"  -u setting dU  (else dU=5 V)\n"
	<<"  -h help  \n"
	<<endl;
    exit(1);
}

inline char* timestamp(time_t t)
{
    static const char* time_fmt="%b %d %Y %H:%M:%S";
    static char strtime[50];

    struct tm* brtm=localtime(&t);
    strftime(strtime,50,time_fmt,brtm);

    return strtime;
}

int main(int argc, char* argv[])
{
    progname=argv[0];
    bool draw=false;
    int u=5;
    //----------------- Process options -----------------//
    int opt;
    while( (opt=getopt(argc,argv,"bm:c:dn:u:hn:v:lF"))!= EOF ) {
	switch( opt ) {
	case '?': Usage(progname); break;
	case 'd': draw=true; break;
	case 'u': u=atoi(optarg); break;
	case 'h': Usage(progname); break;
	}
    }

    const string F_RUN = "/spool/users/ovtin/cosmruns/2019allruns_SC2";

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
	cout<<"1 - Check database...."<<endl;
	cout<<"Begin time of Run"<< *it<<": "<<timestamp(runTime_begin)<<endl;
	cout<<"End time of Run"<< *it<<": "<<timestamp(runTime_end)<<endl;
	kdb_close(connection);

	int n_rows;

	int array[DB_NP];
	for( int i=0; i<DB_NP; i++ )
	    array[i] = 0;

	time_t *array_time;

	KDBconn* data_base;

	data_base = kdb_open();
	if( !data_base ){
	    std::cerr << "Error: DateBase cannot connect !" << std::endl;
	    return 1;
	}

	n_rows = kdb_col_count(data_base, DB_ID, runTime_begin, runTime_end);
	array_time = new time_t[n_rows];

	std::cout << "n_rows: " << n_rows <<"  in Run"<< *it<<std::endl;

	if( kdb_col_read(data_base, DB_ID, runTime_begin, runTime_end, -3, array_time, n_rows)<0 ){
	    std::cerr << "Error: reading DataBase (time) !" << std::endl;
	    return 1;
	}

	for( int _cnt=0; _cnt<MAX_CNTS; _cnt++ ){
	    cnts[_cnt].voltage = new float[n_rows];
	}

	float *curr_time;
	curr_time = new float[n_rows];
	Float_t y[n_rows];

	for( int _cnt=0; _cnt<MAX_CNTS; _cnt++ ){
	    int jump = _cnt*3;
	    std::vector< int > ch_number;
	    std::vector< float > voltage_ref;

	    //const string F_CNT_VOLTAGE = "/home/atc/SettingHV/cnt_voltage_field-6_0kHs.dat";
	    const string F_CNT_VOLTAGE = "/home/ovtin/development/check_run/cnt_voltage_field-6_0kHs.dat";
	    //const string F_CNT_VOLTAGE = "/home/ovtin/development/check_run/cnt_voltage_field-6_0kHs_2015_2016.dat";
	    string nf_cnt_volt  = F_CNT_VOLTAGE;

	    std::ifstream f_in(nf_cnt_volt.c_str());

	    if( f_in.fail() ){
		std::cerr << progname << ": can't open input text file " << nf_cnt_volt.c_str() << "!" << std::endl;
		return 1;
	    }

	    u_int x1;
	    float x2;
	    std::string x3;

	    while( !f_in.eof() ){
		std::string s;
		while( getline(f_in, s) ){
		    if( s.size()>=1 && s[0]!='#' ){
			std::istringstream i_str(s);
			i_str >> x1 >> x2 >> x3;
			ch_number.push_back(x1);
			voltage_ref.push_back(x2);
		    }
		}
	    }
	    f_in.close();

	    int bad_cnt=0;

	    for( int i=0; i<n_rows; i++ ){
		int _ind = n_rows-(i+1);
		curr_time[i] = array_time[_ind];
		if( !kdb_read(data_base, DB_ID, array_time[i], array, DB_NP) ){
		    std::cerr << "Error: reading DataBase (mass) !" << std::endl;
		    return 1;
		}
		cnts[_cnt].voltage[_ind] = array[jump+1]/DB_MULTI;
		y[i]=cnts[_cnt].voltage[_ind];
		//std::cout << _cnt+1 << "\t" << timestamp(curr_time[i]) << "\t" <<cnts[_cnt].voltage[_ind] << "\t" << voltage_ref.at(_cnt) <<std::endl; //вывод номера сч-ка и напряжения

		float delta_U;
		if (cnts[_cnt].voltage[_ind]>4000){ cnts[_cnt].voltage[_ind]=voltage_ref.at(_cnt); } //bit 6000 V - 5/04/2016
		delta_U=fabs(cnts[_cnt].voltage[_ind]-voltage_ref.at(_cnt));

		if ( delta_U>u )
		{
		    bad_cnt++;
		    if(bad_cnt>1)
		    {
			//if( (_cnt+1)!=61 && (_cnt+1)!=156 )  //!!!!!!!23/10/2017!!!!!!!!!!!!!!!!!
			//{
			std::cout << "Bad counter " << _cnt+1 << " in Run" <<  *it <<"\t  Delta_U="<< delta_U <<std::endl;
			//}
		    }
		}
	    }
	    bad_cnt=0;

	    if(draw==true) {
		TCanvas *c1 = new TCanvas("c1"," ");
		c1->GetCanvas();

		TGraph *g = new TGraph(n_rows,curr_time,y);
		g->SetLineColor(2);
		g->SetMarkerColor(4);
		g->SetMarkerStyle(20);
		g->SetMarkerSize(0.7);

		char title[30];
		snprintf( title, 30, "Channel: %d", _cnt );
		g->SetTitle(title);

		g->GetXaxis()->SetTimeOffset(0, "gmt");
		g->GetXaxis()->SetTimeDisplay(1);
		g->GetXaxis()->SetTimeFormat("#splitline{%H:%M}{%d/%m}");
		g->GetYaxis()->SetTitle("Voltage, U");
		g->GetYaxis()->CenterTitle();
		g->Draw("AP");

		c1->SaveAs(TString::Format("cnt_%d.root", _cnt).Data());
	    }
	}
	delete [] array_time;

	if( !kdb_close(data_base) ){
	    std::cerr << "Error: cannot close DateBase !" << std::endl;
	    return 1;
	}

	cout<<"\n2 - Check RunsMonitor...."<<endl;
	atc_read_runsmonitor(*it);
	get_run_data(*it);
    }

    return 0;
}

int atc_read_runsmonitor(int i)
{
    TFile *f1 = TFile::Open(TString::Format("/space/users/todyshev/RunsMonitor/R%02d.root", i).Data());
    if (f1 == 0) {
	cout<<"Cannot open root file"<<TString::Format("/space/users/todyshev/RunsMonitor/R%02d.root", i).Data()<<" - file does not exist"<<endl;
	return 0;
    }
    TDirectory* D1=(TDirectory*)f1->Get("ATC");
    if (D1 == 0) {
	cout<<"Cannot open TDirectory ATC - "<<TString::Format("/space/users/todyshev/RunsMonitor/R%02d.root", i).Data()<<" - data in file does not exist"<<endl;
	return 0;
    }

    TH1F *h1 = (TH1F*)D1->Get("OATCCountH");
    //h1->Draw();

    //TFile *f2=new TFile ("/space/users/todyshev/RunsMonitor/R22681.root");
    //TFile *f2=new TFile ("/space/users/todyshev/RunsMonitor/R23008.root");
    //TFile *f2=new TFile ("/space/users/todyshev/RunsMonitor/R27510.root");
    TFile *f2=new TFile ("/space/users/todyshev/RunsMonitor/R28606.root");
    //TFile *f2=new TFile ("/space/users/todyshev/RunsMonitor/R24649.root");
    TDirectory* D2=(TDirectory*)f2->Get("ATC");
    TH1F *h2 = (TH1F*)D2->Get("OATCCountH");
    h2->SetLineColor(2);
    //h2->Draw("same");

    int Nxbin1=h1->GetNbinsX();

    cout<<"Run="<<i<<"\t"<<"Ref. run=28606"<<endl;
    cout<<"Nxbin1="<<Nxbin1<<endl;

    int k1;
    float bin1;
    float bin2;
    float ratio;

    TCanvas c;
    c.cd();
    TH1F *h3 = new TH1F("h3","ATC:Normalized occupancy",160,0.5,160.5);

    for(int k1=1; k1<=Nxbin1; k1++)
    {
	bin1=h1->GetBinContent(k1);
	bin2=h2->GetBinContent(k1);
	if(bin2==0) { cout<<"Bad channel:"<<k1<<"\t"<<bin1<<"\t"<<bin2<<endl;
	    ratio=0;
	}
	else{
	    ratio=bin1/bin2;
	    if(ratio<=0.3){ cout<<"Bad channel:"<<k1<<"\t"<<bin1<<"\t"<<bin2<<"\t"<<ratio<<endl; }
	    //	else { cout<<k1<<"\t"<<bin1<<"\t"<<bin2<<"\t"<<ratio<<endl; }
	}
	h3->Fill(k1,ratio);
    }
    h3->Draw();
    //c.SaveAs(TString::Format("his_%d.root",i).Data());
    //return 0;
}


int get_run_data(int run)
{
    static const int lum_table_id=2007, lum_table_length=7;
    static const int e_table_id=2119, e_table_length=16;

    static int last_event=0;
    static float lum_e=0, lum_p=0;
    static float dbbeam_energy=0;
    static float beam_energy=0;

    static float intplum1=0;
    static float intelum1=0;

    bool rc=false;
    last_event=0;
    beam_energy=0;

    int buflum[lum_table_length];
    int bufenergy[e_table_length];

    KDBconn *conn = kdb_open();

    if (!conn) {
	cerr<<"Can not connect to database"<<endl;
	return false;
    }
    if( kdb_read_for_run(conn,lum_table_id,run,buflum,lum_table_length) ) {
	lum_p=buflum[0]*1E-3;
	lum_e=buflum[1]*1E-3;
	last_event=buflum[2];

	sumlum_p+=lum_p;
	sumlum_e+=lum_e;

	//cout<<"1-->"<<buflum[0]<<"  2-->"<<buflum[1]<<" 3-->"<<buflum[2]<<" 4-->"<<buflum[3]<<" 5-->"<<buflum[4]<<" 6-->"<<buflum[5]<<" 7-->"<<buflum[6]<<endl;
	//if(run==19654)
	//{ last_event=307251; }
	rc=true;
    }

    kdb_setver(conn,0);
    if( kdb_read_for_run(conn,e_table_id,run,bufenergy,e_table_length) ) {
	dbbeam_energy=bufenergy[1]*1E-6;
	rc=true;
    }

    //===========================================================
    int RunStatus=1;                                       //0-information in the start run; 1-information in the end run; 2 - information in the pause run
    KDBruninfo runinfo;
    if( kdb_run_get_info(conn,run,RunStatus,&runinfo) ) {
	last_event=runinfo.nread;
	beam_energy=runinfo.E_setup*1E-3;
	intplum1=runinfo.intplum*1E-3;
	intelum1=runinfo.intelum*1E-3;
	beam_energy=runinfo.E_setup*1E-3;
	sumintplum1+=intplum1;
	sumintelum1+=intelum1;

	sumnev+=runinfo.nread;

	cout<<" kdb_run_get_info   ===>>   NREAD="<<runinfo.nread<<"\t"<<"sumNev="<<sumnev<<endl;
	cout<<" kdb_run_get_info   ===>>   E_setup="<<runinfo.E_setup*1E-3<<"\t"<<"intplum="<<intplum1<<"\t"<<"intelum="<<intelum1<<endl;
	cout<<" kdb_run_get_info   ===>>   "<<"sumintplum="<<sumintplum1<<"\t"<<"sumintelum="<<sumintelum1<<endl;
	rc=true;
    }

    //=======================================================================

    kdb_close(conn);

    if( rc ) {
	cout<<" RUN DB INFO: LastEvent="<<last_event<<", LumP="<<lum_p<<", LumE="<<lum_e<<", Energy="<<dbbeam_energy<<endl;
	cout<<" RUN DB INFO: "<<", sumLumP="<<sumlum_p<<", sumLumE="<<sumlum_e<<endl;
    }
    else{
	cerr<<" No run info in lmrunscan for this run!"<<endl;
    }

    return rc;
}
