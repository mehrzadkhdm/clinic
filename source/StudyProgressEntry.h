/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef STUDY_PROGRESS_ENTRY_H_
#define STUDY_PROGRESS_ENTRY_H_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

#include <Wt/WDate>
#include <Wt/WString>
#include <string>
#include <array>

using namespace std;
namespace dbo = Wt::Dbo;

class StudyProgressEntry;

class StudyProgressEntry {
public:
	
    Wt::WString patientFirstName;
    Wt::WString patientLastName;
    Wt::WString physicianName;
    Wt::WString patientDob;
    Wt::WDate studyDate;
    Wt::WString status;
 	Wt::WString studyId;

};

#endif // STUDY_PROGRESS_ENTRY_H_
