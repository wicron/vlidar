#include <iostream>
#include <vlidar/v_lidar_window.h>
#include <QApplication>

int main(int argc, char *argv[] )
{
    std::cout << "Welcome to vlidar program" <<std::endl;
    QApplication a(argc, argv);
    a.setApplicationName( "vlidar" );

    VLidarWindow window;
    window.show();
    return a.exec();
    return 0;
}
