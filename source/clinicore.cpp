/*
* Copyright (C) 2015 Cody Scherer <cody@pes7.com> PES7, Inc | Long Beach, CA.
*
* See the LICENSE file for terms of use.
*
* This may look like C code but its really C++
*/
#include <stdlib.h>
#include <Wt/WServer>
#include "UserSession.h"
#include "PracticeSession.h"
#include "StaffApp.h"
#include "PracticeApp.h"
#include "TechApp.h"
#include "LongBeachTechApp.h"
#include "DowneyTechApp.h"
#include "AdminApp.h"
#include "MarketingApp.h"
#include "MarketingAdminApp.h"

int main(int argc, char **argv)
{


    try{
        //starts wserver instance, adds entry points for application
        WServer server(argv[0]);
        server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

        std::cout << "Adding staff auth entry point /staff";
        server.addEntryPoint(Application,
            boost::bind(&createStaffApplication, _1),
            "/staff");
        UserSession::configureAuth();

        std::cout << "Adding tech auth entry point /techs";
        server.addEntryPoint(Application,
            boost::bind(&createTechApplication, _1),
            "/techs");

        std::cout << "Adding lb tech auth entry point /lbtechs";
        server.addEntryPoint(Application,
            boost::bind(&createLongBeachTechApplication, _1),
            "/lbtechs");

        std::cout << "Adding dny tech auth entry point /dnytechs";
        server.addEntryPoint(Application,
            boost::bind(&createDowneyTechApplication, _1),
            "/dnytechs");

        std::cout << "Adding Practice portal entry point ";
        server.addEntryPoint(Application,
            boost::bind(&createPracticeApplication, _1),
            "/providers");

        std::cout << "Adding Admin portal entry point ";
        server.addEntryPoint(Application,
            boost::bind(&createAdminApplication, _1),
            "/admin");

        std::cout << "Adding Marketing portal entry point ";
        server.addEntryPoint(Application,
            boost::bind(&createMarketingApplication, _1),
            "/pcc");

        std::cout << "Adding Marketing admin portal entry point ";
        server.addEntryPoint(Application,
            boost::bind(&createMarketingAdminApplication, _1),
            "/pcc-admin");

        UserSession::configureAuth();

        //this too
        PracticeSession::configureAuth();

        std::cout << "Starting Server";
        if (server.start()) {
            int sig = WServer::waitForShutdown(argv[0]);
            std::cerr << "Shutdown (signal = " << sig << ")" << std::endl;
            server.stop();
        }


    }
    catch (Wt::WServer::Exception& e) {
        std::cerr << e.what() << std::endl;
    }
    catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
}
