#ifndef PDPREVIEW_H
#define PDPREVIEW_H

#include <qt5/QtWidgets/QDialog>

namespace Ui {
    class PdPreview;
}

class PdPreview : public QDialog
{
    Q_OBJECT

public:
    explicit PdPreview(QWidget *parent = 0, const QString &inFile = "");
    ~PdPreview();

private:
    Ui::PdPreview *ui;

private slots:
    void doClose();
};

#endif // PDPREVIEW_H
