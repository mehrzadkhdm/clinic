/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_ORDER_WIDGET_H_
#define PRACTICE_ORDER_WIDGET_H_

#include <fstream>
#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <Wt/WContainerWidget>
#include <Wt/WString>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include "PracticeSession.h"
#include "Composer.h"

using namespace Wt;
using namespace Wt::Dbo;
class PracticeSession;

class PracticeOrderWidget : public Wt::WContainerWidget
{

public:
    PracticeOrderWidget(const char *conninfo, PracticeSession *session, Wt::WContainerWidget *parent = 0);

    Wt::Dbo::Session dbsession;
    Wt::Dbo::backend::Postgres pg_;

private:
    Wt::WString uuid() {
        boost::uuids::uuid uuidx = boost::uuids::random_generator()();

        return boost::lexical_cast<std::string>(uuidx);
    }

    PracticeSession *session_;
    Composer *composer_;
    const char *conninfo_;

public:
    void start();
    void demographics();
    void complete();
    void sendOrder();
    void saveOrder();

    Wt::WText *staticRef_;

    Wt::WText *outRefId_;

    Wt::WString setRefId();
    Wt::WString strRefId();

    Wt::WText *orderSent_;



};

#endif //PRACTICE_ORDER_WIDGET_H
