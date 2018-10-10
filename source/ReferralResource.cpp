#include <Wt/WPushButton>
#include <Wt/WResource>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Render/WPdfRenderer>
#include <Wt/WApplication>

#include <hpdf.h>
using namespace Wt;
namespace {
    void HPDF_STDCALL error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no,
        void *user_data) {
        fprintf(stderr, "libharu error: error_no=%04X, detail_no=%d\n",
            (unsigned int)error_no, (int)detail_no);
    }
}

class ReferralResource : public Wt::WResource
{
public:
    ReferralResource(Wt::WString t, Wt::WObject *parent = 0)
        : Wt::WResource(parent),
        tem(t)
    {

        suggestFileName("referral.pdf");

    }

    virtual void handleRequest(const Wt::Http::Request& request,
        Wt::Http::Response& response)
    {
        response.setMimeType("application/pdf");

        HPDF_Doc pdf = HPDF_New(error_handler, 0);

        // Note: UTF-8 encoding (for TrueType fonts) is only available since libharu 2.3.0 !
        //HPDF_UseUTFEncodings(pdf);

        renderReport(tem, pdf);

        HPDF_SaveToStream(pdf);
        unsigned int size = HPDF_GetStreamSize(pdf);
        HPDF_BYTE *buf = new HPDF_BYTE[size];
        HPDF_ReadFromStream(pdf, buf, &size);
        HPDF_Free(pdf);
        response.out().write((char*)buf, size);
        delete[] buf;
    }
    void renderReport(WString templ, HPDF_Doc pdf) {
        renderPdf(templ, pdf);
    }

    void renderPdf(const Wt::WString& html, HPDF_Doc pdf)
    {
        HPDF_Page page = HPDF_AddPage(pdf);
        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);

        Wt::Render::WPdfRenderer renderer(pdf, page);
        renderer.setMargin(2.54);
        renderer.setDpi(96);
        renderer.render(html);
    }

private:
    WString tem;

};
