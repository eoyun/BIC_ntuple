#include "test.h"
#include "getFile.h"
#include <iostream>
#include <vector>
#include "functions.h"
#include "getMap.h"
#include "TFile.h"
#include "TTree.h"

int main( int argc, char * argv[]) {
    int runnum = atoi(argv[1]);
    Converter converter;
    converter.printMessage();
    std::cout<<"run number is "<<runnum<<std::endl;
    std::vector MIDs = getMID(runnum);
    std::vector<Mapping> mapping_info;
    mapping_info = getMap("../map/TB202503.txt");
    std::vector<getFile*> files;
    std::vector<std::vector<const char*>> address_vector;
    TFile *f_root = new TFile("test.root","recreate");
    TTree *t = new TTree("event_build","event_build");
    std::vector<int> MID, ch, trigger_number, data_length;
    std::vector<long long> trigger_time;

    std::vector<std::string> name;
    std::vector<short> waveform_total;
    std::vector<int> waveform_idx;
    t->Branch("MID",&MID);
    t->Branch("ch",&ch);
    t->Branch("trigger_number",&trigger_number);
    t->Branch("trigger_time",&trigger_time);
    t->Branch("data_length",&data_length);
    t->Branch("name",&name);
    t->Branch("waveform_total",&waveform_total);
    t->Branch("waveform_idx",&waveform_idx);
    for (auto i : mapping_info) {
    	std::vector<const char*> tmp;
	address_vector.push_back(tmp);
    }
    for (auto i : MIDs){
    	//if (i!= 41){
	getFile* tmp_file = new getFile(runnum,i);
	files.push_back(tmp_file);
	//}
    }
    int all_file = 0;
    int iloop = 0;
    PacketGroup tmp_group;
    while (all_file != files.size()){
	all_file = 0;
    	for (auto f : files){
	    if (!f->isEnd()) {
		tmp_group = f->getNextHeader();
		for (int i=0;i< (int) mapping_info.size();i++) {
		    if (!tmp_group.is_multi){
		        if (f->MID() == mapping_info.at(i).MID && tmp_group.single_header.channel == mapping_info.at(i).ch){
		    	    address_vector.at(i).push_back(f->cursor());
		        }
		    } else {
		        if (f->MID() == mapping_info.at(i).MID){
		    	    address_vector.at(i).push_back(f->cursor());
		        }
		       
		    }
		}
	    }
	    else all_file ++;
	}
    }
    for (int i = 0;i<1000;i++){
    //while (true){
        if (i%10 == 0) std::cout<<i<<" taken"<<std::endl;
    	for (int j=0;j<(int)address_vector.size();j++){
            auto idx = find(MIDs.begin(),MIDs.end(),mapping_info.at(j).MID);
	    int file_idx = (int) (idx - MIDs.begin());	    
	    PacketGroup p = files.at(file_idx)->getHeader(address_vector.at(j).at(i));
	    if (!p.is_multi){
		waveform_idx.push_back(waveform_total.size());	
		std::vector<short> tmp_waveform  =files.at(file_idx)->getData(address_vector.at(j).at(i),p);
	    	waveform_total.insert(waveform_total.end(),tmp_waveform.begin(),tmp_waveform.end());
		trigger_number.push_back(p.single_header.tcb_trigger_number);
		trigger_time.push_back(p.single_header.tcb_trigger_time);
		data_length.push_back(p.single_header.data_length - 32);
	    }else  {
		int tmp_ch = mapping_info.at(j).ch;
		waveform_idx.push_back(waveform_total.size());	
		std::vector<std::vector<short>> tmp_waveform = splitVector(files.at(file_idx)->getData(address_vector.at(j).at(i),p),p.multi_headers.at(tmp_ch-1).data_length/2 - 16);
	    	waveform_total.insert(waveform_total.end(),tmp_waveform.at(tmp_ch-1).begin(),tmp_waveform.at(tmp_ch - 1).end());
		trigger_number.push_back(p.multi_headers.at(tmp_ch - 1).tcb_trigger_number);
		trigger_time.push_back(p.multi_headers.at(tmp_ch - 1).tcb_trigger_time);
		data_length.push_back(p.multi_headers.at(tmp_ch - 1).data_length/2 - 16);
	    	
	    }
	    MID.push_back(mapping_info.at(j).MID);
	    ch.push_back(mapping_info.at(j).ch);
	    name.push_back(mapping_info.at(j).name);
	    
	}
	t->Fill();
	waveform_idx.clear();
	waveform_total.clear();
	trigger_time.clear();
	trigger_number.clear();
	MID.clear();
	ch.clear();
	name.clear();
	data_length.clear();
    }
    t->Write();
    f_root->Close();
    for (auto f : files) delete f;
    return 0;
}
