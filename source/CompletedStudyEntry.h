/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef COMPLETED_STUDY_ENTRY_H_
#define COMPLETED_STUDY_ENTRY_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include <Wt/WDate>
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;
namespace dbo = Wt::Dbo;

class CompletedStudyEntry;

class CompletedStudyEntry {
public:
	
    Wt::WString patientFirstName;
    Wt::WString patientLastName;
    Wt::WString physicianName;
    Wt::WString patientDob;
    Wt::WDate studyDate;
    Wt::WString status;
 	Wt::WString studyId;

};

#endif // COMPLETED_STUDY_ENTRY_H_
