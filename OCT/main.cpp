

#ifdef MAINQT
#include "OCTGUI.h"
#include <QtWidgets/QApplication>
#include"model.h"
#include"controller.h"



/**
 * @brief 主函数，程序入口点
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 程序退出状态码
 */
int main(int argc, char *argv[])
{
    // 创建Qt应用程序对象
    QApplication app(argc, argv);
    OCTGUI window;
    
    // 创建模型和控制器对象，建立MVC架构
    Model model;
    Controller controller(&model, &window);

    // 显示主窗口并启动事件循环
    window.show();
    return app.exec();
}

#endif