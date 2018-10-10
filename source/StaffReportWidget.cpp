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
#include <Wt/WText>
#include <Wt/WCheckBox>
#include <Wt/WGroupBox>
#include <Wt/WJavaScript>
#include <Wt/WResource>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Render/WPdfRenderer>
//dbo stuff
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Postgres>
#include <string>
//hpdf stuff
#include <hpdf.h>

//other header files
#include "StaffReportWidget.h"
#include "Report.h"
using namespace Wt;
namespace dbo = Wt::Dbo;

//namespace for stuct of the displayable Report data for view
namespace InboundRepApp{
    struct InboundReport {
        Wt::WString pracName;
        Wt::WString pracDrFirstName;
        Wt::WString pracDrLastName;
        Wt::WString ptFirstName;
        Wt::WString ptLastName;
        Wt::WString ptDob;

        InboundReport(const Wt::WString& aPracName,
            const Wt::WString& aPracDrFirstName,
            const Wt::WString& aPracDrLastName,
            const Wt::WString& aPtFirstName,
            const Wt::WString& aPtLastName,
            const Wt::WString& aPtDob)
            : pracName(aPracName),
            pracDrFirstName(aPracDrFirstName),
            pracDrLastName(aPracDrLastName),
            ptFirstName(aPtFirstName),
            ptLastName(aPtLastName),
            ptDob(aPtDob) {}
    };
}

StaffReportWidget::StaffReportWidget(const char *conninfo, WContainerWidget *parent) :
WContainerWidget(parent),
pg_(conninfo)
{
    dbsession.setConnection(pg_);
    pg_.setProperty("show-queries", "true");

    dbsession.mapClass<Report>("report");
    dbsession.mapClass<CompositeEntity>("composite");

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

void StaffReportWidget::reportList()
{

    dbo::Transaction transaction(dbsession);

    typedef dbo::collection< dbo::ptr<Report> > Reports;
    Reports reports = dbsession.find<Report>();
    std::cerr << reports.size() << std::endl;



    Wt::WTable *table = new Wt::WTable();
    table->setHeaderCount(1);

    table->elementAt(0, 0)->addWidget(new Wt::WText("#"));
    table->elementAt(0, 1)->addWidget(new Wt::WText("Practice Name"));
    table->elementAt(0, 2)->addWidget(new Wt::WText("Patient First Name"));
    table->elementAt(0, 3)->addWidget(new Wt::WText("Patient Last Name"));
    table->elementAt(0, 4)->addWidget(new Wt::WText("Patient Date of Birth"));
    table->elementAt(0, 5)->addWidget(new Wt::WText("Completed By"));
    table->elementAt(0, 6)->addWidget(new Wt::WText("Remove"));




    int row = 1;
    for (Reports::const_iterator i = reports.begin(); i != reports.end(); ++i, ++row)
    {

        new Wt::WText(Wt::WString::fromUTF8("{1}").arg(row),
            table->elementAt(row, 0)),
            new Wt::WText((*i)->repPracticeName_, table->elementAt(row, 1)),
            new Wt::WText((*i)->patientFirstName_, table->elementAt(row, 2)),
            new Wt::WText((*i)->patientLastName_, table->elementAt(row, 3)),
            new Wt::WText((*i)->patientDob_, table->elementAt(row, 4));
            if ((*i)->repStaffName_ == ""){
                Wt::WString reportid = Wt::WString((*i)->reportId_);
                btnStaffComplete = new Wt::WPushButton("Complete Now", table->elementAt(row, 5));
                btnStaffComplete->clicked().connect(std::bind([=]() {
                    btnStaffComplete->hide();
                    btnStaffComplete1 = new Wt::WPushButton("Done", table->elementAt(row, 5));
                    Wt::WLineEdit *repStaffNameEdit_ = new Wt::WLineEdit(table->elementAt(row, 5));
                    repStaffNameEdit_->show();
                    btnStaffComplete1->clicked().connect(std::bind([=](){
                        btnStaffComplete->hide();
                        btnStaffComplete1->hide();
                        repStaffNameEdit_->hide();
                        staffDone = new  Wt::WText(table->elementAt(row, 5));
                        staffDone->setText(repStaffNameEdit_->text());
                        staffDone->show();
                        dbo::Transaction transaction(dbsession);

                        dbo::ptr<Report> report = dbsession.find<Report>().where("report_id = ?").bind(reportid);
                        report.modify()->repStaffName_ = staffDone->text().toUTF8();
                        transaction.commit();

                    }));

                }));
            }
            else {
                new Wt::WText((*i)->repStaffName_, table->elementAt(row, 5));
            }
            btnRemoveReport = new Wt::WPushButton("Remove", table->elementAt(row, 6));
            Wt::WString repid = Wt::WString((*i)->reportId_);
            btnRemoveReport->clicked().connect(std::bind([=]() {

                dbo::Transaction rmtrans(dbsession);
                dbo::ptr<Report> report = dbsession.find<Report>().where("report_id = ?").bind(repid);
                report.remove();
                rmtrans.commit();
                Wt::WText *showRemoved = new Wt::WText("Marked for Removal.", table->elementAt(row, 6));
                btnRemoveReport->hide();
                showRemoved->show();
            }));



    }

    transaction.commit();

    table->addStyleClass("table form-inline");
    table->addStyleClass("table table-striped");
    table->addStyleClass("table table-hover");

    Wt::WContainerWidget *result = new Wt::WContainerWidget();
    result->addWidget(table);
    addWidget(new Wt::WText("<h3>Requests for Reports</h3>"));
    addWidget(result);

}
