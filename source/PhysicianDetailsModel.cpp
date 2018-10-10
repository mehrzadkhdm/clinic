/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include <Wt/WText>
#include <Wt/WApplication>
#include <Wt/WString>
#include "PhysicianDetailsModel.h"

#include "Practice.h"
#include "PracticeSession.h"

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>

namespace dbo = Wt::Dbo;

PhysicianDetailsModel::PhysicianDetailsModel(const char *conninfo, Wt::WString prac, PracticeSession *session, Wt::WObject *parent)
	: Wt::WFormModel(parent),
	pg_(conninfo),
	pracId_(prac),
	session_(session)
{
	
	//need pg, connection, dbsession and class mapping to practice
	dbsession.setConnection(pg_);

	pg_.setProperty("show-queries", "true");
	
	dbsession.mapClass<Practice>("practice");

	dbo::Transaction transaction(dbsession);
	try {
		dbsession.createTables();
		log("info") << "Database created";
	}
	catch (...) {
		log("info") << "Using existing database";
	}
	transaction.commit();


}


void PhysicianDetailsModel::save(const Wt::Auth::User& practice, Wt::WString email, Wt::WString pcc)
{

	//get practice information by prac uuid
	dbo::Transaction copytrans(dbsession);
	dbo::ptr<Practice> existpractice = dbsession.find<Practice>().where("prac_id = ?").bind(pracId_.toUTF8());
	//save to local vars
	Wt::WString pracName = existpractice->refPracticeName_;
	Wt::WString pracZip = existpractice->refPracticeZip_;
	Wt::WString pracDrFirst = existpractice->refPracticeDrFirst_;
	Wt::WString pracDrLast = existpractice->refPracticeDrLast_;
	Wt::WString pracNpi = existpractice->refPracticeNpi_;
	Wt::WString pracEmail = email;
	Wt::WString pracSpecialty = existpractice->refSpecialty_;
	Wt::WString isGroup = existpractice->isGroup_;
	Wt::WString groupId = existpractice->groupId_;
	Wt::WDateTime dateAdded = existpractice->dateAdded_;
	Wt::WString emailOpt = "Yes";
	Wt::WString clinicore = "Yes";
	Wt::WString pccEmail = pcc;
	Wt::WString exisitingPracId = existpractice->practiceId_;

	existpractice.remove();
	//delete practice with that id
	copytrans.commit();
	//persist new practice with local vars as data
	Wt::Dbo::ptr<Practice> newprac = session_->practice(practice);
	newprac.modify()->refPracticeName_ = pracName.toUTF8();
	newprac.modify()->refPracticeZip_ = pracZip.toUTF8();
	newprac.modify()->refPracticeDrFirst_ = pracDrFirst.toUTF8();
	newprac.modify()->refPracticeDrLast_ = pracDrLast.toUTF8();
	newprac.modify()->refPracticeNpi_ = pracNpi.toUTF8();
	newprac.modify()->refPracticeEmail_ = pracEmail.toUTF8();
	newprac.modify()->refSpecialty_ = pracSpecialty.toUTF8();
	newprac.modify()->pracEmailOpt_ = emailOpt.toUTF8();
	newprac.modify()->clinicoreAccount_ = clinicore.toUTF8();
	newprac.modify()->pccEmail_ = pccEmail.toUTF8();
	newprac.modify()->isGroup_ = isGroup.toUTF8();
	newprac.modify()->groupId_ = groupId.toUTF8();
	newprac.modify()->dateAdded_ = dateAdded;
	newprac.modify()->practiceId_ = exisitingPracId.toUTF8();

}
