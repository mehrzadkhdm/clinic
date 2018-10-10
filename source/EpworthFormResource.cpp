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
class EpworthFormResource : public Wt::WResource
{
public:
    EpworthFormResource(Wt::WObject *parent = 0)
        : Wt::WResource(parent)
    {
        suggestFileName("patient-epworth-test.pdf");
    }

    ~EpworthFormResource() {
        beingDeleted();
    }
    virtual void handleRequest(const Wt::Http::Request &request,
        Wt::Http::Response &response)
    {
        response.setMimeType("application/pdf");
        std::string attpath = "../docroot/resources/united/printepworth.pdf";
        std::ifstream  dst;
        dst.open(attpath);

        response.out() << dst.rdbuf();
    }
};
