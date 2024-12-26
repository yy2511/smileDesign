#include<cgalbool.h>

#include<QFileDialog>
#include<QTimer>
#include "MessageBox.h"

STLBooleanProcessor::STLBooleanProcessor(QWidget* parent){

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    resize(800, 600);  // 设置窗口的初始大小为 800x600

    m_titleBar = new CustomDlgTitle(this);
    m_titleBar->move(0, 0);
    m_titleBar->setBackgroundColor(15,15,15);
    m_titleBar->setTitleContent(tr("Veneer Preview"));
    m_titleBar->setTitleIcon(":/icon/icon.png");


    connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
    connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
    connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
    connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));


    setStyleSheet("QFrame{background: #0F0F0F;}");
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,40,0,0);
    //mainLayout->setSpacing(0);
    buttonLayout = new QHBoxLayout();
    vtkWidget = new QVTKOpenGLNativeWidget(this);

    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->SetBackground(44 / 255.0, 44 / 255.0, 44 / 255.0);
    vtkWidget->renderWindow()->AddRenderer(m_renderer);
    saveButton = new QPushButton(tr("Save Model"), this);
    saveButton->setFixedSize(180, 40);
    exportButton = new QPushButton(tr("Export Model"), this);
    exportButton->setFixedSize(180, 40);
    QList<QPushButton*> buttons = {saveButton,exportButton};
    for (QPushButton *btn : buttons) {
        btn->setStyleSheet(R"(
            QPushButton {
                background-color: #5D5D5D;
                color: white;
                border-radius: 8px;
                padding: 8px;
                font: 18px 'Microsoft Yahei';
            }
            QPushButton:hover {
                background: #1296db;
                color: white;
            }
            QPushButton:pressed {
                background: #1296db;
                color: white;
            }
        )");
    }
    //mainLayout->addWidget(m_titleBar);
    mainLayout->addWidget(vtkWidget);              // 添加 VTK 渲染部件
    mainLayout->addLayout(buttonLayout);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exportButton);


    setLayout(mainLayout);  // 设置主布局
    //show();

    connect(saveButton, &QPushButton::clicked, [=]() {
        try{

            // 保存变换后的模型为 STL 文件
            vtkSmartPointer<vtkSTLWriter> writer = vtkSmartPointer<vtkSTLWriter>::New();
            writer->SetFileName((parentDir + "\\dentalVeneers.stl").toStdString().c_str());
            writer->SetInputData(m_filter->GetOutput());
            writer->Write();

            FYMessageBox m_box(FYMessageBox::Information, tr("Prompt Information"), tr("Save Successful!"),FYMessageBox::Ok);
            QTimer::singleShot(1000, &m_box, SLOT(accept()));
            m_box.exec();

        }
        catch(const std::exception &e)
        {
            // 如果发生错误，提示用户
            FYMessageBox::warning(this, tr("Prompt Information"), tr("Failed to export file:")+QString("%1").arg(e.what()),FYMessageBox::Ok);
        }
    }
            );

    connect(exportButton, &QPushButton::clicked, this,[=](){
        QString directory = QFileDialog::getExistingDirectory(this, tr("Select Export Directory"));

        if (!directory.isEmpty())
        {
            // 定义要导出的文件名
            QString fileName = "dentalVeneers.stl";
            QString filePath = directory + "/" + fileName;

            // 假设 transformFilter2 是一个有效的 vtkTransformPolyDataFilter 实例
            //vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
            vtkSmartPointer<vtkPolyData> polyData = m_filter->GetOutput();

            try
            {
                // 使用 vtkSTLWriter 写入 STL 文件
                vtkSmartPointer<vtkSTLWriter> writer = vtkSmartPointer<vtkSTLWriter>::New();
                writer->SetFileName(filePath.toStdString().c_str());
                writer->SetInputData(polyData);
                writer->Write();

                // 提示用户导出成功
                FYMessageBox::information(this, tr("Prompt Information"), tr("The file was successfully exported").arg(filePath),FYMessageBox::Ok);
            }
            catch (const std::exception &e)
            {
                // 如果发生错误，提示用户
                FYMessageBox::warning(this, tr("Prompt Information"), tr("Failed to export file").arg(e.what()),FYMessageBox::Ok);
            }
        }
        else
        {
            // 用户未选择目录
            FYMessageBox::warning(this, tr("Prompt Information"), tr("Export directory not selected"),FYMessageBox::Ok);
        }
    });
}
void STLBooleanProcessor::onButtonMinClicked()
{
    showMinimized();  // Minimize the window
}

void STLBooleanProcessor::onButtonRestoreClicked()
{
    // Handle window restore logic (maximize -> restore)
    if (isMaximized()) {
        showNormal();  // Restore the window to its normal state
    } else {
        showMaximized();  // Maximize the window
    }
}

void STLBooleanProcessor::onButtonMaxClicked()
{
    showMaximized();  // Maximize the window
}

void STLBooleanProcessor::onButtonCloseClicked()
{
    close();  // Close the window
}
