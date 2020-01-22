///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2018, The Regents of the University of California
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

/**************************************************************************
 * Copyright(c) 2018 Regents of the University of California
 *              Kwangsoo Han, Andrew B. Kahng and Sriram Venkatesh
 * Contact      kwhan@ucsd.edu, abk@cs.ucsd.edu, srvenkat@ucsd.edu
 * Affiliation: Computer Science and Engineering Department, UC San Diego,
 *              La Jolla, CA 92093-0404, USA
 *
 *************************************************************************/

#include "mymeasure.h"
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <iomanip>
using namespace std;

// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
// VmSize:
// Virtual memory size
// rss:
// Resident Set Size: number of pages the process has in real
// memory.  This is just the pages which count toward text,
// data, or stack space.  This does not include pages which
// have not been demand-loaded in, or which are swapped out.

//void CMeasure::process_mem_usage(double& vm_usage, double& resident_set)
void CMeasure::process_mem_usage(){
	//vm_usage	  = 0.0;
	//resident_set = 0.0;
	double vm_usage	  = 0.0;
	double resident_set = 0.0;

	// 'file' stat seems to give the most reliable results
	//
	ifstream stat_stream("/proc/self/stat",ios_base::in);

	// dummy vars for leading entries in stat that we don't care about
	//
	string pid, comm, state, ppid, pgrp, session, tty_nr;
	string tpgid, flags, minflt, cminflt, majflt, cmajflt;
	string utime, stime, cutime, cstime, priority, nice;
	string O, itrealvalue, starttime;

	// the two fields we want
	//
	unsigned long vsize;
	long rss;

	stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
					>> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
					>> utime >> stime >> cutime >> cstime >> priority >> nice
					>> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

	stat_stream.close();

	long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
	vm_usage	  = vsize / 1024.0;
	resident_set = rss * page_size_kb;
	
	//cout<<comm<<endl;
	cout << "virtual memory size : " << vm_usage << endl;
	cout << "resident Set Size   : " << resident_set << endl;
    cout << "----------------------------------------" << endl;
}

void CMeasure::start_clock(){
	begin = clock();
}

void CMeasure::stop_clock(string task){
	tasks.push_back(task);
	elapsed_secs.push_back(double(clock() - begin)/CLOCKS_PER_SEC);
}

void CMeasure::print_clock(){
	cout<<"Tasks"<<setw(25)<<"Time"<<endl;
	for (unsigned i=0; i < tasks.size(); i++)
		cout<<tasks[i]<<right<<setw(30-tasks[i].size())<<elapsed_secs[i]<<endl;
}

void CMeasure::printMemoryUsage(void)
{
        ifstream status("/proc/self/status");
        string data;

        if(status.good())
        {
                for (int i=0 ; i<2 ; ++i)
                        getline(status, data);

                // account for login and loginlinux versions
                getline(status, data);
                if(data.find("SleeAVG") == string::npos)
                        for(int i=0 ; i<7 ; ++i)
                                getline(status,data);
                else
                        for(int i=0 ; i<6;  ++i)
                                getline(status,data);

                // vmPeak
                getline(status, data);
                cout << endl << "----------------------------------------";
                cout << endl << "### VmPeak\t\t\t: "<< data << endl;
        }
        status.close();

        status.open("/proc/self/stat");
        if(status.good())
        {
                double vmsize;
                for(unsigned i=0 ; i< 22; ++i)
                {
                        status >> data;
                }
                status >> vmsize;

                cout << "### Memory Usage\t: " << vmsize/1048576. << " MB" <<endl;
        }
        status.close();
}

