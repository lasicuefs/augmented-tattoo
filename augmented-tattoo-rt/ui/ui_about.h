/********************************************************************************
** Form generated from reading UI file 'about.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QGroupBox *groupBox_2;
    QLabel *label_2;
    QGroupBox *groupBox;
    QLabel *label;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->setWindowModality(Qt::ApplicationModal);
        Dialog->resize(402, 274);
        Dialog->setSizeGripEnabled(true);
        Dialog->setModal(true);
        groupBox_2 = new QGroupBox(Dialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 140, 381, 121));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 20, 351, 91));
        label_2->setTextFormat(Qt::RichText);
        label_2->setOpenExternalLinks(true);
        groupBox = new QGroupBox(Dialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(9, 9, 381, 121));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 23, 271, 91));
        label->setTextFormat(Qt::RichText);
        label->setOpenExternalLinks(true);

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "About", nullptr));
        groupBox_2->setTitle(QApplication::translate("Dialog", "\303\223rg\303\243os de Apoio e Colabora\303\247\303\243o", nullptr));
        label_2->setText(QApplication::translate("Dialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">FAPESB - Funda\303\247\303\243o de Amparo \303\240 Pesquisa do Estado da Bahia</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">UEFS - Universidade Estadual de Feira de Santana</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">UFBA - Universidade Federal da Bahia</span></p>\n"
"<p "
                        "style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">LASIC -  </span><a href=\"http://sites.ecomp.uefs.br/lasic\"><span style=\" text-decoration: underline; color:#0000ff;\">sites.ecomp.uefs.br/lasic</span></a></p></body></html>", nullptr));
        groupBox->setTitle(QApplication::translate("Dialog", "Pesquisadores", nullptr));
        label->setText(QApplication::translate("Dialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Jairo Calmon (UEFS/UFBA) <a href=\"mailto:jairocalmon@ecomp.uefs.br?Subject=Augmented%20Tattoo\"><span style=\" text-decoration: underline; color:#0000ff;\">jairocalmon@ecomp.uefs.br</span></a></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Angelo Loula (UEFS) <a href=\"mailto:angelocl@ecomp.uefs.br?Subject=Augmented%20Tattoo\"><span style=\" text-decoration: underline; color:#0000ff;\">angelocl@ecomp.uefs.br</span></a></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12"
                        "px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Claudio Eduardo Goes (UEFS) <a href=\"mailto:cegoes@gmail.com?Subject=Augmented%20Tattoo\"><span style=\" text-decoration: underline; color:#0000ff;\">cegoes@gmail.com</span></a></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Jo\303\243o Queiroz (UFJF) <a href=\"mailto:queirozj@gmail.com?Subject=Augmented%20Tattoo\"><span style=\" text-decoration: underline; color:#0000ff;\">queirozj@gmail.com</span></a></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_H
