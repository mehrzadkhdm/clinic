/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PHYSICIAN_DETAILS_MODEL_H_
#define PHYSICIAN_DETAILS_MODEL_H_

#include <Wt/WFormModel>
#include <Wt/WString>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp> 
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>

class PracticeSession;

class PhysicianDetailsModel : public Wt::WFormModel
{
public:

  Wt::WString pracId_;

  PhysicianDetailsModel(const char *conninfo, Wt::WString prac, PracticeSession *session, Wt::WObject *parent = 0);

  Wt::Dbo::Session dbsession;
  Wt::Dbo::backend::Postgres pg_;

  void save(const Wt::Auth::User& practice, Wt::WString email, Wt::WString pcc);

private:
  PracticeSession *session_;
  
};

#endif // PHYSICIAN_DETAILS_MODEL