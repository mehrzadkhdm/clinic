/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_STUDY_PROGRESS_WIDGET_H_
#define PRACTICE_STUDY_PROGRESS_WIDGET_H_
#include <fstream>
#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WString>
#include <Wt/WStackedWidget>
#include <Wt/WTimer>
#include "PracticeRecentWidget.h"
#include "PracticeSession.h"

using namespace Wt;

class PracticeSession;
class PracticeStudyProgressWidget : public Wt::WContainerWidget
{
public:
    PracticeStudyProgressWidget(const char *conninfo, Wt::WString *prac, PracticeSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;

    Wt::Dbo::backend::Postgres pg_;


    void referralList();
    void hello();
    void searchList();
    Wt::WString *prac_;

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }

    PracticeRecentWidget *recentContainer;
    Wt::WStackedWidget *resultStack_;
    
    PracticeSession *session_;

    const char *conninfo_;
    
    Wt::WTimer *refreshTimer; 

    ~PracticeStudyProgressWidget()
    {
        refreshTimer->stop();
    }

private:


    void stopRefreshTimer();
    void startRefreshTimer();
    
    void search(Wt::WString searchString);
    void preSearch(Wt::WString search);

};

#endif //PRACTICE_STUDY_PROGRESS_WIDGET_H
