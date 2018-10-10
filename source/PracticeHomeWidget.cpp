/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/

#include <boost/lexical_cast.hpp>

#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WTable>
#include <Wt/Dbo/Dbo>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WText>
#include <Wt/WImage>
#include <Wt/WLink>
#include <Wt/WBreak>
#include "PracticeHomeWidget.h"
#include "InsuranceFormResource.cpp"
#include "EpworthFormResource.cpp"
#include "ReferralFormResource.cpp"

PracticeHomeWidget::PracticeHomeWidget(WContainerWidget *parent) :
WContainerWidget(parent)
{
    Wt::log("notice") << "Practice user switched routes to /home.";
    Wt::WContainerWidget *container = new Wt::WContainerWidget();
    Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
    container->setStyleClass("practice-home-container");
    container->setLayout(hbox);

    Wt::WVBoxLayout *vbox2 = new Wt::WVBoxLayout();
    hbox->addLayout(vbox2);

    vbox2->addWidget(new Wt::WText("<h2>Sleep Study Resources</h2>"));

    Wt::WResource *insurancepdf = new InsuranceFormResource();
    Wt::WPushButton *insurancebutton = new Wt::WPushButton("Insurance Codes 2016 PDF");
    insurancebutton->setLink(insurancepdf);
    insurancebutton->setStyleClass("btn-insurance");
    vbox2->addWidget(new Wt::WText("Insurance codes for sleep, reference information for sleep disorder treatment, and insurance plans we accept."));
    vbox2->addWidget(new Wt::WBreak());
    vbox2->addWidget(insurancebutton);
    vbox2->addWidget(new Wt::WBreak());
    vbox2->addWidget(new Wt::WBreak());

    Wt::WResource *epworthpdf = new EpworthFormResource();
    Wt::WPushButton *epworthbutton = new Wt::WPushButton("Epworth Sleep Test for Patients PDF");
    epworthbutton->setLink(epworthpdf);
    epworthbutton->setStyleClass("btn-epworth");
    vbox2->addWidget(new Wt::WText("The Epworth Sleepiness Test is a great way to assess risk, and engage your patient about their sleep health."));
    vbox2->addWidget(new Wt::WBreak());
    vbox2->addWidget(epworthbutton);
    vbox2->addWidget(new Wt::WBreak());
    vbox2->addWidget(new Wt::WBreak());

    Wt::WResource *referralpdf = new ReferralFormResource();
    Wt::WPushButton *referralbutton = new Wt::WPushButton("Referral Form 2016 PDF");
    referralbutton->setLink(referralpdf);
    referralbutton->setStyleClass("btn-referral");
    vbox2->addWidget(new Wt::WText("If you would prefer to fill out the referral form by hand, and fax it to us - you can download it here."));
    vbox2->addWidget(new Wt::WBreak());
    vbox2->addWidget(referralbutton);
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    container->addWidget(new Wt::WBreak());
    addWidget(container);

}

void PracticeHomeWidget::hello()
{

    //new WText("Welcome to the Staff Home Widget", this);
}
