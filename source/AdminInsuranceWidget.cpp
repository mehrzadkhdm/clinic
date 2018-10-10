/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

//std io stuff
#include <sstream>
#include <fstream>
#include <iostream>
//boost stuff
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

//wt stuff
#include <Wt/WBootstrapTheme>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WTable>
#include <Wt/WTableCell>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WTemplate>
#include <Wt/WStringListModel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WDialog>
#include <Wt/WText>
#include <Wt/WCheckBox>
#include <Wt/WGroupBox>
#include <Wt/WJavaScript>
#include <Wt/WResource>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Render/WPdfRenderer>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WText>
#include <Wt/WBootstrapTheme>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WTable>
#include <Wt/WLabel>
#include <Wt/WTableCell>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WStringListModel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WCheckBox>
#include <Wt/WGroupBox>
#include <Wt/WJavaScript>
#include <Wt/WDialog>
#include <Wt/WTemplate>
#include <Wt/WCalendar>
#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/WDate>
#include <Wt/WDateEdit>
#include <Wt/WStackedWidget>
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

//other header files
#include "Composer.h"

#include "InboundContainerWidget.h"
#include "AdminInsuranceWidget.h"
#include "InsuranceResultWidget.h"
#include "Referral.h"
#include "Practice.h"
#include "Attachment.h"
#include "SleepStudy.h"
#include "Patient.h"
#include "ReferralResource.cpp"
#include "AttachmentResource.cpp"
#include "InsuranceItem.h"
#include "Insurance.h"
#include "UserSession.h"
using namespace Wt;
namespace dbo = Wt::Dbo;

//namespace for stuct of the displayable referral data for view
namespace InboundRefApp{
	struct InboundReferral {
		Wt::WString pracName;
		Wt::WString pracDrName;
		Wt::WString ptFirstName;
		Wt::WString ptLastName;
		Wt::WString ptCity;

		InboundReferral(const Wt::WString& aPracName,
			const Wt::WString& aPracDrName,
			const Wt::WString& aPtFirstName,
			const Wt::WString& aPtLastName,
			const Wt::WString& aPtCity)
			: pracName(aPracName),
			pracDrName(aPracDrName),
			ptFirstName(aPtFirstName),
			ptLastName(aPtLastName),
			ptCity(aPtCity) {}
	};
}

namespace InboundRefAttachments{
	struct InboundAttachment {
		Wt::WString fileName;
		Wt::WString fileType;

		InboundAttachment(const Wt::WString& aFileName,
			const Wt::WString& aFileType)
			: fileName(aFileName),
			fileType(aFileType){}
	};
}

AdminInsuranceWidget::AdminInsuranceWidget(const char *conninfo, UserSession *session, WContainerWidget *parent) :
	WContainerWidget(parent),
	pg_(conninfo),
	session_(session),
	conninfo_(conninfo)
{
	dbsession.setConnection(pg_);

	pg_.setProperty("show-queries", "true");

	dbsession.mapClass<Referral>("referral");
	dbsession.mapClass<CompositeEntity>("composite");
	dbsession.mapClass<AttachmentDb>("attachment");
	dbsession.mapClass<Patient>("patient");	
	dbsession.mapClass<SleepStudy>("sleepstudy");
	dbsession.mapClass<Practice>("practice");	
	dbsession.mapClass<Insurance>("insurance");

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

void AdminInsuranceWidget::searchList()
{
	
	Wt::WContainerWidget *container = new Wt::WContainerWidget();
	container->setStyleClass("print-referrals-container");	

	Wt::WContainerWidget *container2 = new Wt::WContainerWidget();
	container2->setStyleClass("print-referrals-container");

	Wt::WTemplate *form = new Wt::WTemplate(Wt::WString::tr("dateEdit-template"));
	form->addFunction("id", &Wt::WTemplate::Functions::id);

	Wt::WText *delabel = new Wt::WText("Please select the date range for the PT insurance report.");
	container->addWidget(delabel);
	container->addWidget(new Wt::WBreak());
	container->addWidget(new Wt::WBreak());
	
	Wt::WDateEdit *de1 = new Wt::WDateEdit();
	form->bindWidget("from", de1);
	de1->setFormat("MM/dd/yyyy"); // Apply a different date format.
	de1->setPlaceholderText("Start Date");
	de1->setStyleClass("print-de1");
	container->addWidget(de1);

	Wt::WDateEdit *de2 = new Wt::WDateEdit();
	form->bindWidget("to", de2);
	de2->setFormat("MM/dd/yyyy"); // Apply a different date format.
	de2->setPlaceholderText("End Date");
	de2->setStyleClass("print-de2");
	container->addWidget(de2);
	
	Wt::WPushButton *button = new Wt::WPushButton("Get Referrals");
	form->bindWidget("save", button);
	button->setStyleClass("print-button");

	
	Wt::WText *out = new Wt::WText();
	form->bindWidget("out", out);

	container->addWidget(button);
	container->addWidget(out);	
	container->addWidget(new Wt::WBreak());

	resultStack_ = new Wt::WStackedWidget(container);

	button->clicked().connect(std::bind([=] () {	
		Wt::WDate start;
		Wt::WDate end;

		start = de1->date();
		end = de2->date();

		AdminInsuranceWidget::showResult(start, end);
	}));

	addWidget(container);

}

void AdminInsuranceWidget::showResult(Wt::WDate& startDate, Wt::WDate& endDate)
{

	resultStack_->clear();

	referralResult = new InsuranceResultWidget(conninfo_, session_, resultStack_);
	referralResult->showInsuranceLog(startDate, endDate);
	resultStack_->setCurrentWidget(referralResult);

}