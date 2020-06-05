QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    EthernetContoller/DataBase/DataBase.cpp \
    EthernetContoller/Server.cpp \
    GameInstance/BattleArea/BattleArea.cpp \
    GameInstance/BattleArea/BattlePair/BattlePair.cpp \
    GameInstance/Card/Card.cpp \
    GameInstance/GameInstance.cpp \
    GameInstance/HandUp/HandUp.cpp \
    GameInstance/PlayerDeck/PlayerDeck.cpp \
    GameInstance/StartDeck/NewDeckGenerator/NewDeckGenerator.cpp \
    GameInstance/StartDeck/StartDeck.cpp \
    Player/Player.cpp \
    main.cpp

HEADERS += \
    EthernetContoller/DataBase/DataBase.h \
    EthernetContoller/Server.h \
    GameInstance/BattleArea/BattleArea.h \
    GameInstance/BattleArea/BattlePair/BattlePair.h \
    GameInstance/Card/Card.h \
    GameInstance/GameInstance.h \
    GameInstance/HandUp/HandUp.h \
    GameInstance/PlayerDeck/PlayerDeck.h \
    GameInstance/StartDeck/NewDeckGenerator/NewDeckGenerator.h \
    GameInstance/StartDeck/StartDeck.h \
    Player/Player.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
