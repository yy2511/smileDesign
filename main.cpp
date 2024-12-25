
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
#include <QApplication>
#include "SmileDesigenWidget.h"
#include <AlignWindow.h>
#include "QHBoxLayout"
#include <QTranslator>
#include <QLocale>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    if (translator.load("D:\\qtProject\\sm\\zh_CN.qm")) {
        a.installTranslator(&translator);
    } else {
        qDebug() << "Failed to load translation file.";
    }



    SmileDesigenWidget sw;


    sw.setDataPath("D:\\WeChat\\files\\xwechat_files\\wxid_94tvczmvxyed22_b25b\\msg\\file\\2024-11\\ID20241022-160409\\ID20241022-160409\\FScan\\20240604112700\\20240604112700.ply",
                   "D:\\WeChat\\files\\xwechat_files\\wxid_94tvczmvxyed22_b25b\\msg\\file\\2024-11\\ID20241022-160409\\ID20241022-160409\\IOS\\upper.ply",
                   "D:\\WeChat\\files\\xwechat_files\\wxid_94tvczmvxyed22_b25b\\msg\\file\\2024-11\\ID20241022-160409\\ID20241022-160409\\IOS\\lower.ply");
    sw.showMaximized();



    return a.exec();
}
