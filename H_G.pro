QT       += core gui

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
    annovar.cpp \
    assoctool.cpp \
    emmax.cpp \
    emmaxparamwidget.cpp \
    filereader.cpp \
    gcta.cpp \
    gemma.cpp \
    gemmaparamwidget.cpp \
    graphviewer.cpp \
    main.cpp \
    mainwindow.cpp \
    mtoolbutton.cpp \
    phenoselector.cpp \
    plink.cpp \
    poplddecay.cpp \
    runningmsgwidget.cpp \
    tool.cpp \
    toolselector.cpp \
    useros.cpp \
    workdirectory.cpp

HEADERS += \
    annovar.h \
    assoctool.h \
    emmax.h \
    emmaxparamwidget.h \
    filereader.h \
    gcta.h \
    gemma.h \
    gemmaparamwidget.h \
    graphviewer.h \
    mainwindow.h \
    mtoolbutton.h \
    phenoselector.h \
    plink.h \
    poplddecay.h \
    runningmsgwidget.h \
    tool.h \
    toolselector.h \
    useros.h \
    workdirectory.h

FORMS += \
    emmaxparamwidget.ui \
    gemmaparamwidget.ui \
    graphviewer.ui \
    mainwindow.ui \
    runningmsgwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

QMAKE_LFLAGS += -no-pie

#### Config R environment.
## comment this out if you need a different version of R,
## and set set R_HOME accordingly as an environment variable
R_HOME = 		$$system(R RHOME)
#message("R_HOME is" $$R_HOME)

## include headers and libraries for R
RCPPFLAGS = 		$$system($$R_HOME/bin/R CMD config --cppflags)
RLDFLAGS = 		$$system($$R_HOME/bin/R CMD config --ldflags)
RBLAS = 		$$system($$R_HOME/bin/R CMD config BLAS_LIBS)
RLAPACK = 		$$system($$R_HOME/bin/R CMD config LAPACK_LIBS)

## if you need to set an rpath to R itself, also uncomment
RRPATH =		-Wl,-rpath,$$R_HOME/lib

## include headers and libraries for Rcpp interface classes
## note that RCPPLIBS will be empty with Rcpp (>= 0.11.0) and can be omitted
RCPPINCL = 		$$system($$R_HOME/bin/Rscript -e \"Rcpp:::CxxFlags\(\)\")
RCPPLIBS = 		$$system($$R_HOME/bin/Rscript -e \"Rcpp:::LdFlags\(\)\")

## for some reason when building with Qt we get this each time
##   /usr/local/lib/R/site-library/Rcpp/include/Rcpp/module/Module_generated_ctor_signature.h:25: warning: unused parameter ‘classname
## so we turn unused parameter warnings off
## no longer needed with Rcpp 0.9.3 or later
#RCPPWARNING =		-Wno-unused-parameter

## include headers and libraries for RInside embedding classes
RINSIDEINCL = 		$$system($$R_HOME/bin/Rscript -e \"RInside:::CxxFlags\(\)\")
RINSIDELIBS = 		$$system($$R_HOME/bin/Rscript -e \"RInside:::LdFlags\(\)\")

## compiler etc settings used in default make rules
QMAKE_CXXFLAGS +=	$$RCPPWARNING $$RCPPFLAGS $$RCPPINCL $$RINSIDEINCL
QMAKE_LIBS +=           $$RLDFLAGS $$RBLAS $$RLAPACK $$RINSIDELIBS $$RCPPLIBS
