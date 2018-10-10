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
class AttachmentResource : public Wt::WResource
{
public:
    AttachmentResource(Wt::WString attId, Wt::WString refId, Wt::WString fileName, Wt::WString fileType, Wt::WObject *parent = 0)
        : Wt::WResource(parent),
        attid(attId),
        refid(refId),
        fn(fileName),
        ft(fileType)
    {
        suggestFileName(fileName);
    }

    ~AttachmentResource() {
        beingDeleted();
    }
    virtual void handleRequest(const Wt::Http::Request &request,
        Wt::Http::Response &response)
    {
        const std::string strFileType(ft.toUTF8());
        response.setMimeType(strFileType);
        std::string attpath = "/srv/clinicore/referraluploads/" + refid.toUTF8() + "/attachments/" + attid.toUTF8() + "/" + fn.toUTF8();
        std::ifstream  dst;
        dst.open(attpath);

        response.out() << dst.rdbuf();
    }
private:
    WString attid;
    WString refid;
    WString fn;
    WString ft;
};
