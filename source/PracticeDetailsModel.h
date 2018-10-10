/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#ifndef PRACTICE_DETAILS_MODEL_H_
#define PRACTICE_DETAILS_MODEL_H_

#include <Wt/WFormModel>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>

class PracticeSession;

class PracticeDetailsModel : public Wt::WFormModel
{
public:
  static const Field PracticeNameField;
  static const Field PracticeZipField;
  static const Field PracticeDrFirstName;
  static const Field PracticeDrLastName;
  static const Field PracticeDrNpi;
  static const Field PracticeEmailAddress;
  static const Field PracticeSpecialty;
  static const Field PracticeId;

  Wt::WText *staticPrac_;
  Wt::WText *emailOpt_;
  Wt::WString setPracId();
  Wt::WString strPracId();
  Wt::WString strEmailOpt();

  PracticeDetailsModel(const char *conninfo, PracticeSession& session, Wt::WObject *parent = 0);

  Wt::Dbo::Session dbsession;
  Wt::Dbo::backend::Postgres pg_;

  void save(const Wt::Auth::User& practice);

private:
  PracticeSession& session_;

  Wt::WString uuid() {
      boost::uuids::uuid uuidx = boost::uuids::random_generator()();

      return boost::lexical_cast<std::string>(uuidx);
  }
};

#endif // PRACTICE_DETAILS_MODEL
