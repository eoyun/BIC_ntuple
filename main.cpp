#include "test.h"
#include "getFile.h"
#include <iostream>
#include <vector>
#include "functions.h"
#include "getMap.h"
#include "TFile.h"
#include "TTree.h"
#include <unordered_map>
#include <chrono>


int main( int argc, char * argv[]) {

    // start measure time
    auto start = std::chrono::high_resolution_clock::now();

    // Parse run number from command line
    int runnum = atoi(argv[1]);

    Converter converter;
    converter.printMessage();
    std::cout<<"run number is "<<runnum<<std::endl;
    
    // Get list of MID values in data files
    std::vector MIDs = getMID(runnum);

    // Get mapping information 
    std::vector<Mapping> mapping_info;
    mapping_info = getMap("../map/TB202503.txt");

    // Prepare the file readers for each datafile
    std::vector<getFile*> files;
    
    // Map MID -> index for faster loop
    std::unordered_map<int, int> mid_to_index;
    for (int i = 0; i < MIDs.size(); ++i) {
        mid_to_index[MIDs[i]] = i;
    }

    // Initialize address vector 
    std::vector<std::vector<const char*>> address_vector;
    
    // Initialize root file
    TFile *f_root = new TFile("test.root","recreate");
    TTree *t = new TTree("event_build","event_build");
    
    // Prepare branches
    std::vector<int> MID, ch, trigger_number, data_length;
    std::vector<unsigned long long> trigger_time;
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
    
    // Initialize address vector structure
    for (auto i : mapping_info) {
    	std::vector<const char*> tmp;
	address_vector.push_back(tmp);
    }

    // Open files for each MID
    for (auto i : MIDs){
	getFile* tmp_file = new getFile(runnum,i);
	files.push_back(tmp_file);
    }

    int all_file = 0;
    int iloop = 0;
    PacketGroup tmp_group;
    
    // Distribute addresses into address vector based on header matching
    while (all_file != files.size()){
        if (iloop%100 == 0) std::cout<<iloop<<" taken"<<std::endl;
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
	iloop ++;
    }

    // Determine the minimum number of events among all channels
    auto it = std::min_element(
        address_vector.begin(), address_vector.end(),
        [](const std::vector<const char*>& a, const std::vector<const char*>& b) {
            return a.size() < b.size();
        });

    iloop = 0;
    int loop_end = it->size();
    std::cout<<"total evt : "<<loop_end<<std::endl;
    
    // Main event building loop 
    while (iloop < (int) loop_end){
        if (iloop%100== 0) std::cout<<iloop<<" / "<<loop_end<<" taken"<<std::endl;

	int file_idx_tmp = mid_to_index.at(mapping_info[0].MID);
	PacketGroup p_tmp = files.at(file_idx_tmp)->getHeader(address_vector.at(0).at(0));
	iloop ++;
        
	unsigned long long trig_time_tmp = p_tmp.multi_headers.at(1).tcb_trigger_time;
	int trig_num_tmp = p_tmp.multi_headers.at(1).tcb_trigger_number;
    	
	for (int j=0;j<(int)address_vector.size();j++){
	    bool flag = false;
	    for (int k = 0; k< (int) address_vector.at(j).size(); k++){
	        int file_idx =  mid_to_index.at(mapping_info[j].MID);
		const char* access_address = address_vector.at(j).at(k);
	        PacketGroup p = files.at(file_idx)->getHeader(access_address);

	        if (!p.is_multi){
		    //if (p.single_header.tcb_trigger_time != trig_time_tmp && p.single_header.tcb_trigger_number != trig_num_tmp) continue;
		    if (p.single_header.tcb_trigger_time != trig_time_tmp || p.single_header.tcb_trigger_number != trig_num_tmp) continue;
	            
		    waveform_idx.push_back(waveform_total.size());	
	            std::vector<short> tmp_waveform  =files.at(file_idx)->getData(access_address,p);
	            waveform_total.insert(waveform_total.end(),tmp_waveform.begin(),tmp_waveform.end());
	            trigger_number.push_back(p.single_header.tcb_trigger_number);
	            trigger_time.push_back(p.single_header.tcb_trigger_time);
	            data_length.push_back(p.single_header.data_length/2 - 16);
		    address_vector.at(j).erase(address_vector.at(j).begin() + k);
		    flag = true;
		    break;
	        }else  {
	            int tmp_ch = mapping_info.at(j).ch;
		    //if (p.multi_headers.at(tmp_ch - 1).tcb_trigger_time != trig_time_tmp && p.multi_headers.at(tmp_ch - 1).tcb_trigger_number != trig_num_tmp) continue;
		    if (p.multi_headers.at(tmp_ch - 1).tcb_trigger_time != trig_time_tmp || p.multi_headers.at(tmp_ch - 1).tcb_trigger_number != trig_num_tmp) continue;
	            
		    waveform_idx.push_back(waveform_total.size());	
	            std::vector<std::vector<short>> tmp_waveform = splitVector(files.at(file_idx)->getData(access_address,p),p.multi_headers.at(tmp_ch-1).data_length/2 - 16);
	            waveform_total.insert(waveform_total.end(),tmp_waveform.at(tmp_ch-1).begin(),tmp_waveform.at(tmp_ch - 1).end());
	            trigger_number.push_back(p.multi_headers.at(tmp_ch - 1).tcb_trigger_number);
	            trigger_time.push_back(p.multi_headers.at(tmp_ch - 1).tcb_trigger_time);
	            data_length.push_back(p.multi_headers.at(tmp_ch - 1).data_length/2 - 16);
		    address_vector.at(j).erase(address_vector.at(j).begin() + k);
		    flag = true;
	            break;
	        }
	    }
	    if (!flag) continue; 
	    // Save the mapping info of each channels
	    MID.push_back(mapping_info.at(j).MID);
	    ch.push_back(mapping_info.at(j).ch);
	    name.push_back(mapping_info.at(j).name);
	    
	}

	// Save event to TTree
	t->Fill();

	// Clear all buffers for next event
	waveform_idx.clear();
	waveform_total.clear();
	trigger_time.clear();
	trigger_number.clear();
	MID.clear();
	ch.clear();
	name.clear();
	data_length.clear();
    }

    // Finalize ROOT outup
    t->Write();
    f_root->Close();

    for (auto f : files) delete f;

    // End time
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds\n";

    return 0;
}
