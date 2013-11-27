#ifndef PdOptions_H
#define PdOptions_H

#include <QDialog>

namespace Ui {
    class PdOptions;
}

class PdOptions : public QDialog
{
    Q_OBJECT
    
public:
    explicit PdOptions(QWidget *parent = 0);
    ~PdOptions();
    void setFocusOnEmpty(bool &);
    void setDelConf(bool &);
    void setDeselConf(bool &);
    void setAutoMove(bool &);
    bool getFocusEmpty();
    bool getDelConf();
    bool getDeselConf();
    bool getAutoMove();
    bool getGoodExit();
    
private:
    Ui::PdOptions *ui;
    bool useData;

private slots:
    void doSave();
    void doQuit();
};

#endif // PdOptions_H
