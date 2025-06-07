/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayoutMain;
    QStackedWidget *stackWidget;
    QWidget *loginPage;
    QVBoxLayout *verticalLayoutLogin;
    QLineEdit *editUser;
    QLineEdit *editPass;
    QHBoxLayout *horizontalLayoutLoginBtns;
    QPushButton *btnRegister;
    QPushButton *btnLogin;
    QWidget *mainPage;
    QVBoxLayout *verticalLayoutMainPage;
    QHBoxLayout *horizontalLayoutTopBtns;
    QPushButton *btnAddFriend;
    QPushButton *btnRefreshFriend;
    QPushButton *btnCheckRequests;
    QPushButton *btnSendMessage;
    QPushButton *btnCreateGroup;
    QPushButton *btnJoinGroup;
    QPushButton *btnRefreshGroupList;
    QPushButton *btnGroupSendMsg;
    QPushButton *btnFetchGroupMsg;
    QPushButton *btnLogout;
    QHBoxLayout *horizontalLayoutLists;
    QGroupBox *groupBoxFriends;
    QVBoxLayout *verticalLayoutFriendList;
    QListWidget *friendListWidget;
    QGroupBox *groupBoxGroups;
    QVBoxLayout *verticalLayoutGroupList;
    QListWidget *groupListWidget;
    QPushButton *btnAuditLog;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(640, 480);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayoutMain = new QVBoxLayout(centralwidget);
        verticalLayoutMain->setObjectName(QString::fromUtf8("verticalLayoutMain"));
        stackWidget = new QStackedWidget(centralwidget);
        stackWidget->setObjectName(QString::fromUtf8("stackWidget"));
        loginPage = new QWidget();
        loginPage->setObjectName(QString::fromUtf8("loginPage"));
        verticalLayoutLogin = new QVBoxLayout(loginPage);
        verticalLayoutLogin->setObjectName(QString::fromUtf8("verticalLayoutLogin"));
        editUser = new QLineEdit(loginPage);
        editUser->setObjectName(QString::fromUtf8("editUser"));

        verticalLayoutLogin->addWidget(editUser);

        editPass = new QLineEdit(loginPage);
        editPass->setObjectName(QString::fromUtf8("editPass"));
        editPass->setEchoMode(QLineEdit::Password);

        verticalLayoutLogin->addWidget(editPass);

        horizontalLayoutLoginBtns = new QHBoxLayout();
        horizontalLayoutLoginBtns->setObjectName(QString::fromUtf8("horizontalLayoutLoginBtns"));
        btnRegister = new QPushButton(loginPage);
        btnRegister->setObjectName(QString::fromUtf8("btnRegister"));

        horizontalLayoutLoginBtns->addWidget(btnRegister);

        btnLogin = new QPushButton(loginPage);
        btnLogin->setObjectName(QString::fromUtf8("btnLogin"));

        horizontalLayoutLoginBtns->addWidget(btnLogin);


        verticalLayoutLogin->addLayout(horizontalLayoutLoginBtns);

        stackWidget->addWidget(loginPage);
        mainPage = new QWidget();
        mainPage->setObjectName(QString::fromUtf8("mainPage"));
        verticalLayoutMainPage = new QVBoxLayout(mainPage);
        verticalLayoutMainPage->setObjectName(QString::fromUtf8("verticalLayoutMainPage"));
        horizontalLayoutTopBtns = new QHBoxLayout();
        horizontalLayoutTopBtns->setObjectName(QString::fromUtf8("horizontalLayoutTopBtns"));
        btnAddFriend = new QPushButton(mainPage);
        btnAddFriend->setObjectName(QString::fromUtf8("btnAddFriend"));

        horizontalLayoutTopBtns->addWidget(btnAddFriend);

        btnRefreshFriend = new QPushButton(mainPage);
        btnRefreshFriend->setObjectName(QString::fromUtf8("btnRefreshFriend"));

        horizontalLayoutTopBtns->addWidget(btnRefreshFriend);

        btnCheckRequests = new QPushButton(mainPage);
        btnCheckRequests->setObjectName(QString::fromUtf8("btnCheckRequests"));

        horizontalLayoutTopBtns->addWidget(btnCheckRequests);

        btnSendMessage = new QPushButton(mainPage);
        btnSendMessage->setObjectName(QString::fromUtf8("btnSendMessage"));

        horizontalLayoutTopBtns->addWidget(btnSendMessage);

        btnCreateGroup = new QPushButton(mainPage);
        btnCreateGroup->setObjectName(QString::fromUtf8("btnCreateGroup"));

        horizontalLayoutTopBtns->addWidget(btnCreateGroup);

        btnJoinGroup = new QPushButton(mainPage);
        btnJoinGroup->setObjectName(QString::fromUtf8("btnJoinGroup"));

        horizontalLayoutTopBtns->addWidget(btnJoinGroup);

        btnRefreshGroupList = new QPushButton(mainPage);
        btnRefreshGroupList->setObjectName(QString::fromUtf8("btnRefreshGroupList"));

        horizontalLayoutTopBtns->addWidget(btnRefreshGroupList);

        btnGroupSendMsg = new QPushButton(mainPage);
        btnGroupSendMsg->setObjectName(QString::fromUtf8("btnGroupSendMsg"));

        horizontalLayoutTopBtns->addWidget(btnGroupSendMsg);

        btnFetchGroupMsg = new QPushButton(mainPage);
        btnFetchGroupMsg->setObjectName(QString::fromUtf8("btnFetchGroupMsg"));

        horizontalLayoutTopBtns->addWidget(btnFetchGroupMsg);

        btnLogout = new QPushButton(mainPage);
        btnLogout->setObjectName(QString::fromUtf8("btnLogout"));

        horizontalLayoutTopBtns->addWidget(btnLogout);


        verticalLayoutMainPage->addLayout(horizontalLayoutTopBtns);

        horizontalLayoutLists = new QHBoxLayout();
        horizontalLayoutLists->setObjectName(QString::fromUtf8("horizontalLayoutLists"));
        groupBoxFriends = new QGroupBox(mainPage);
        groupBoxFriends->setObjectName(QString::fromUtf8("groupBoxFriends"));
        verticalLayoutFriendList = new QVBoxLayout(groupBoxFriends);
        verticalLayoutFriendList->setObjectName(QString::fromUtf8("verticalLayoutFriendList"));
        friendListWidget = new QListWidget(groupBoxFriends);
        friendListWidget->setObjectName(QString::fromUtf8("friendListWidget"));
        friendListWidget->setAlternatingRowColors(true);

        verticalLayoutFriendList->addWidget(friendListWidget);


        horizontalLayoutLists->addWidget(groupBoxFriends);

        groupBoxGroups = new QGroupBox(mainPage);
        groupBoxGroups->setObjectName(QString::fromUtf8("groupBoxGroups"));
        verticalLayoutGroupList = new QVBoxLayout(groupBoxGroups);
        verticalLayoutGroupList->setObjectName(QString::fromUtf8("verticalLayoutGroupList"));
        groupListWidget = new QListWidget(groupBoxGroups);
        groupListWidget->setObjectName(QString::fromUtf8("groupListWidget"));
        groupListWidget->setAlternatingRowColors(true);

        verticalLayoutGroupList->addWidget(groupListWidget);

        btnAuditLog = new QPushButton(groupBoxGroups);
        btnAuditLog->setObjectName(QString::fromUtf8("btnAuditLog"));

        verticalLayoutGroupList->addWidget(btnAuditLog);


        horizontalLayoutLists->addWidget(groupBoxGroups);


        verticalLayoutMainPage->addLayout(horizontalLayoutLists);

        stackWidget->addWidget(mainPage);

        verticalLayoutMain->addWidget(stackWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\345\256\211\345\205\250\350\201\212\345\244\251\347\263\273\347\273\237", nullptr));
        editUser->setPlaceholderText(QCoreApplication::translate("MainWindow", "\347\224\250\346\210\267\345\220\215", nullptr));
        editPass->setPlaceholderText(QCoreApplication::translate("MainWindow", "\345\257\206\347\240\201", nullptr));
        btnRegister->setText(QCoreApplication::translate("MainWindow", "\346\263\250\345\206\214", nullptr));
        btnLogin->setText(QCoreApplication::translate("MainWindow", "\347\231\273\345\275\225", nullptr));
        btnAddFriend->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\345\245\275\345\217\213", nullptr));
        btnRefreshFriend->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260\345\245\275\345\217\213\345\210\227\350\241\250", nullptr));
        btnCheckRequests->setText(QCoreApplication::translate("MainWindow", "\345\245\275\345\217\213\350\257\267\346\261\202", nullptr));
        btnSendMessage->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201\346\266\210\346\201\257", nullptr));
        btnCreateGroup->setText(QCoreApplication::translate("MainWindow", "\346\226\260\345\273\272\347\276\244\350\201\212", nullptr));
        btnJoinGroup->setText(QCoreApplication::translate("MainWindow", "\345\212\240\345\205\245\347\276\244\350\201\212", nullptr));
        btnRefreshGroupList->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260\347\276\244\345\210\227\350\241\250", nullptr));
        btnGroupSendMsg->setText(QCoreApplication::translate("MainWindow", "\347\276\244\350\201\212\345\217\221\346\266\210\346\201\257", nullptr));
        btnFetchGroupMsg->setText(QCoreApplication::translate("MainWindow", "\347\276\244\350\201\212\345\216\206\345\217\262", nullptr));
        btnLogout->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272\347\231\273\345\275\225", nullptr));
        groupBoxFriends->setTitle(QCoreApplication::translate("MainWindow", "\345\245\275\345\217\213\345\210\227\350\241\250", nullptr));
        groupBoxGroups->setTitle(QCoreApplication::translate("MainWindow", "\347\276\244\350\201\212\345\210\227\350\241\250", nullptr));
        btnAuditLog->setText(QCoreApplication::translate("MainWindow", "\346\237\245\347\234\213\345\256\241\350\256\241\346\227\245\345\277\227", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
