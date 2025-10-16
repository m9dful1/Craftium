#include <QtWidgets/QApplication>
#include "../include/controllerapp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application information
    QApplication::setApplicationName("Craftium");
    QApplication::setApplicationDisplayName("Craftium - Auto Crafter");
    QApplication::setOrganizationName("SpiritWise Studios LLC");
    
    ControllerApp window;
    window.show();

    return app.exec();
} 