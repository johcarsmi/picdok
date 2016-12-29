/*
 *  Show the generated HTML index page on a new form using the QWebView class.
*/
#include "pdpreview.h"
#include "ui_pdpreview.h"

PdPreview::PdPreview(QWidget *parent, const QString &inFile) :
    QDialog(parent),
    ui(new Ui::PdPreview)
{
    ui->setupUi(this);
    ui->wvPreview->load(QUrl::fromLocalFile(inFile));
}

PdPreview::~PdPreview()
{
    delete ui;
}

void PdPreview::doClose()   // Close the preview.
{
    this->close();
}
