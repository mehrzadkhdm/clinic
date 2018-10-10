#include <Wt/WContainerWidget>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WObject>
#include <Wt/WResource>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>
using namespace Wt;
using namespace std;
class ReferralFormResource : public Wt::WResource
{
public:
    ReferralFormResource(Wt::WObject *parent = 0)
        : Wt::WResource(parent)
    {
        suggestFileName("referralform2016.pdf");
    }

    ~ReferralFormResource() {
        beingDeleted();
    }
    virtual void handleRequest(const Wt::Http::Request &request,
        Wt::Http::Response &response)
    {
        response.setMimeType("application/pdf");
        std::string attpath = "../docroot/resources/united/referralform.pdf";
        std::ifstream  dst;
        dst.open(attpath);

        response.out() << dst.rdbuf();
    }
};
