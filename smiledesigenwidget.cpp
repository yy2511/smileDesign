#include "SmileDesigenWidget.h"
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkSTLReader.h"
#include "vtkPolyDataMapper.h"
#include <filesystem>
#include "QFileDialog"
#include "vtkPLYReader.h"
#include "vtkProperty.h"
#include "qmessagebox.h"
#include "qtimer.h"
#include "vtkInteractorStyleTrackballActor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkCellPicker.h"
#include "vtkRendererCollection.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkBoxWidget2.h"
#include "vtkBoxRepresentation.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkMapper.h"
#include "vtkAppendPolyData.h"
#include "vtkCoordinate.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkRegularPolygonSource.h"
#include "vtkInformation.h"
#include "vtkStringArray.h"
#include "vtkInformationStringKey.h"
#include "vtkMath.h"
#include "vtkSelectionNode.h"
#include "vtkSelection.h"
#include "vtkExtractSelection.h"
#include "vtkIdFilter.h"
#include "vtkThreshold.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkLineSource.h"
#include "vtkSphereSource.h"
#include "vtkSelectPolyData.h"
#include<vtkGlyph2D.h>
#include<vtkGlyphSource2D.h>
#include<vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyLine.h>
#include <vtkProperty2D.h>
#include "vtkCamera.h"
#include "vtkMath.h"
#include<vtkMatrix4x4.h>
#include <vtkPolygon.h>
#include <set>
#include<vtkUnsignedCharArray.h>
#include<vtkPointData.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include<QDebug>
#include<iostream>
#include <direct.h>
#include<vtkSmoothPolyDataFilter.h>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QSignalMapper>
#include <QColor>
#include<qbuttongroup.h>
#include<vtkImplicitSelectionLoop.h>
#include<vtkClipPolyData.h>
#include<vtkCleanPolyData.h>
#include<vtkExtractPolyDataGeometry.h>
#include<vtkPolyDataConnectivityFilter.h>
#include<vtkPolyDataEdgeConnectivityFilter.h>
#include<vtkPassThrough.h>
#include<vtkNamedColors.h>
#include<vtkLODActor.h>
#include"pickpointselectionstyle.h"
#include<vtkTransformPolyDataFilter.h>
#include <QFile>
#include <QFileInfo>
#include"handleinteractor.h"
#include"smilelinestyle.h"
#include<vtkBooleanOperationPolyDataFilter.h>
#include<vtkPassThroughFilter.h>
#include<vtkTriangleFilter.h>
#include<vtkPolyDataNormals.h>
#include<vtkFeatureEdges.h>
#include<vtkCallbackCommand.h>
#include<vtkFillHolesFilter.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkPLYReader.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkThreshold.h>
#include <vtkGeometryFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <QMainWindow>
// CGAL 相关头文件
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/Euler_operations.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/config.h>

#include <QMenu>
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include <CGAL/Polygon_mesh_processing/self_intersections.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/orientation.h>

#include <CGAL/Polygon_mesh_processing/border.h>
#include <CGAL/Polygon_mesh_processing/connected_components.h>

#include <QFont>
#include <QFontDatabase>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/repair.h>

#include<vtkLight.h>

SmileDesigenWidget::SmileDesigenWidget(QWidget* parent) : QWidget(parent)
{   // 设置窗口默认全屏
    this->setStyleSheet("SmileDesigenWidget{background-color: #0F0F0F}");
    initUI();
    initVTK();
    initStyle();
    processor = new STLBooleanProcessor();

    // 使用 findChildren 查找并设置按钮字体
    QFont font;
    font.setFamily("Microsoft Yahei");   // 设置字体类型，例如 "Arial"
    QList<QPushButton*> buttons = this->findChildren<QPushButton*>();
    for (QPushButton *btn : buttons) {
        btn->setFont(font);
    }



    // 使用 findChildren 查找并设置标签字体
    QList<QLabel*> labels = this->findChildren<QLabel*>();
    for (QLabel *lbl : labels) {
        lbl->setFont(font);
    }
    QList<QComboBox*> comboboxs = this->findChildren<QComboBox*>();
    for (QComboBox *cbb : comboboxs) {
        cbb->setFont(font);
    }
    QList<QCheckBox*> checkboxs = this->findChildren<QCheckBox*>();
    for (QCheckBox *ckb : checkboxs) {
        ckb->setFont(font);
    }


    pathToMatrixMap = {
        { "./resources/teeth", (Eigen::Matrix4d() <<
                                      0.979232, 0.0132982, 0.202309, -4.20313,
                                  0.0606796, 0.932885, -0.355027, -11.5458,
                                  -0.193452, 0.35993, 0.912703, -49.5687,
                                  0, 0, 0, 1).finished()
        },
        { "./resources/teeth/style", (Eigen::Matrix4d() <<
                                            -0.978579, 0.202602, 0.0365592, 4.47705,
                                        -0.0497586, -0.405074, 0.912929, 168.496,
                                        0.19977, 0.891554, 0.406478, 37.3091,
                                        0, 0, 0, 1).finished()
        },
        { "./resources/teeth/style6.4", (Eigen::Matrix4d() <<
                                            -0.976389, 0.2148, 0.0229266, -2.31085,
                                        0.0537914, 0.344547, -0.937226, -162.182,
                                        -0.209216, -0.913864, -0.347967, -57.1186,
                                        -0, 0, 0, 1).finished()
        }
        // 可以添加更多路径和对应矩阵
    };
    currentState = State::Init;


    connect(buttonAutoCut, SIGNAL(clicked()), this, SLOT(autoCut()));




    connect(this, SIGNAL(addItemWhereRenderAddMember(const QString&, std::vector<vtkSmartPointer<vtkActor>>)), widgetActor, SLOT(addItem(const QString&, std::vector<vtkSmartPointer<vtkActor>>)));
    connect(this, SIGNAL(removeItemWhereRenderRemoveMember(const QString&)), widgetActor, SLOT(removeItem(const QString&)));
    // connect(widget3, &ActorWidget::refresh, this, [this]() {
    // 	renderer_main->Render();
    // 	renderer_coronal->Render();
    // 	renderer_sagittal->Render();
    // 	renderer_top->Render();
    // 	});

    connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged(int)));
    connect(checkBox, &QCheckBox::stateChanged, this, [=](){
        cout<<"checkBox->checkState"<<checkBox->checkState()<<endl;


    });

    connect(buttonFlip, &QPushButton::clicked, this, [=](){
        flipView();
        handleStyle2->flipsync();
    });
    connect(this, &SmileDesigenWidget::pointSelected, this, &SmileDesigenWidget::onPointSelected);
    //connect(this, &SmileDesigenWidget::changeStyle, this, &SmileDesigenWidget::onChangeStyle);

    connect(buttonImport, &QPushButton::clicked, this, &SmileDesigenWidget::importAll);


    connect(buttonMatch, &QPushButton::clicked, this, [=](){
        if(currentState==State::Import){
            matchWindow->show();
            matchWindow->loadModelPair(m_facePath, m_mouth_upper_Path);
        }else if(currentState==State::Import||currentState==State::Rectify){

        }else{
            QMessageBox m_box(QMessageBox::Information, tr("Prompt Information"), tr("Please import the model first!"));
            QTimer::singleShot(500, &m_box, SLOT(accept()));
            m_box.exec();
        }
    });

    connect(matchWindow, &AlignWindow::applyTransform, this, &SmileDesigenWidget::mouthFirstAdjust);


    // connect(buttonCalibrate, &QPushButton::clicked, this, [this]() {
    //
    // });

    connect(buttonCalibrate, &QPushButton::clicked, this, [this]() {

        if(currentState == State::Import ||currentState == State::Rectify ){
            cout<<"have imported"<<endl;
            currentState = State::Rectify;

            pickStyle->SetDefaultRenderer(renderer_main);
            w3D_main->renderWindow()->GetInteractor()->SetInteractorStyle(pickStyle);
            pickStyle->startNewSelection();
            buttonFree->show();
            teethlabel->show();
            teethComboBox->show();
            if(faceActor){
                QPixmap pixmap("./resources/pic/lefteye.png"); // 替换为你的图片路径

                tip->updateTip(tr("Click on the left eye"), pixmap);
                //layoutW3->addWidget(tip);
                tip->show();
                //tip->showAtWidgetBottomLeft();
            }
            isCalibrated = true;
        }else{
            QMessageBox m_box(QMessageBox::Information, tr("Prompt Information"), tr("Please import the model first!"));
            QTimer::singleShot(500, &m_box, SLOT(accept()));
            m_box.exec();

        }});
    connect(buttonCalibrate, &QPushButton::clicked, this, [this]() {

        if(currentState == State::Import ||currentState == State::Rectify ){
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    std::cout << calibrate_transform->GetMatrix()->GetElement(i, j) << " ";
                }
                std::cout << std::endl;
            }
            calibrate_transform->Inverse();
            if(!isCalibrated){
                vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
                transformFilter->SetTransform(calibrate_transform);
                transformFilter->SetInputData(faceActor->GetMapper()->GetInput());
                transformFilter->Update();

                // 获取变换后的数据
                vtkSmartPointer<vtkPolyData> transformedPolyData = transformFilter->GetOutput();

                //faceActor->SetUserTransform(m_transform);

                vtkPolyDataMapper* polyDataMapper = dynamic_cast<vtkPolyDataMapper*>(faceActor->GetMapper());
                if (polyDataMapper) {
                    polyDataMapper->SetInputData(transformedPolyData);
                }
                else {
                    std::cerr << "The mapper is not a vtkPolyDataMapper!" << std::endl;
                }

                vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
                transformFilter1->SetTransform(calibrate_transform);

                for (int i = 0; i < 4; i++) {
                    // 获取当前 Actor 的 Mapper 并检查其类型
                    vtkSmartPointer<vtkPolyDataMapper> polyDataMapper = vtkPolyDataMapper::SafeDownCast(mouthActors[i]->GetMapper());

                    // 获取当前 Actor 的输入数据并检查其有效性
                    vtkSmartPointer<vtkPolyData> polyData = polyDataMapper->GetInput();
                    // 设置 TransformPolyDataFilter 的输入数据
                    transformFilter1->SetInputData(polyData);
                    transformFilter1->Update();

                    // 将变换后的数据设置回 Mapper
                    vtkSmartPointer<vtkPolyData> transformedPolyData = transformFilter1->GetOutput();
                    polyDataMapper->SetInputData(transformedPolyData);
                }

                vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
                transformFilter2->SetTransform(calibrate_transform);
                vtkSmartPointer<vtkPolyDataMapper> polyDataMapper2 = vtkPolyDataMapper::SafeDownCast(mouthActors_lower[0]->GetMapper());

                // 获取当前 Actor 的输入数据并检查其有效性
                vtkSmartPointer<vtkPolyData> polyData2= polyDataMapper2->GetInput();
                // 设置 TransformPolyDataFilter 的输入数据
                transformFilter2->SetInputData(polyData2);
                transformFilter2->Update();

                // 将变换后的数据设置回 Mapper
                vtkSmartPointer<vtkPolyData> transformedPolyData2 = transformFilter2->GetOutput();
                polyDataMapper2->SetInputData(transformedPolyData2);

                //m_mesh_view->setCameraProperties(campos,focalpoint,viewup,true,60);

                vtkCamera* camera = renderer_main->GetActiveCamera();

                // 设置相机的新位置
                camera->SetPosition(0, 0, -300);

                // 设置相机焦点为模型中心
                camera->SetFocalPoint(0, 0, 0);

                // 可选: 设置相机的上方向，防止图像上下颠倒
                camera->SetViewUp(0, -1, 0); // 根据实际情况调整
                renderer_main->ResetCamera();
                renderer_main->GetRenderWindow()->Render();
                isCalibrated = false;
            }

        }

    });


    connect(buttonFree, &QPushButton::clicked, this, [this]() {
        if (isFreeView) {
            double campos[3] = { 0,0,300 };
            double focalpoint[3] = { 0,0,0 };
            double viewup[3] = { 0,1,0 };
            //m_mesh_view->setCameraProperties(campos,focalpoint,viewup,true,60);

            vtkCamera* camera = renderer_main->GetActiveCamera();

            camera->SetPosition(campos);

            camera->SetFocalPoint(focalpoint);

            camera->SetViewUp(viewup);

            camera->SetViewAngle(60);

            camera->ParallelProjectionOn();
            renderer_main->ResetCamera();
            renderer_main->GetRenderWindow()->Render();



            buttonFree->setText(tr("Free View"));
            if(smileStyle->getHasLine()){
                w3D_main->renderWindow()->GetInteractor()->SetInteractorStyle(smileStyle);
            }else{
                w3D_main->renderWindow()->GetInteractor()->SetInteractorStyle(zoomStyle);
            }

        }
        else {
            // 设置交互样式
            vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
            w3D_main->renderWindow()->GetInteractor()->SetInteractorStyle(style);
            buttonFree->setText(tr("Locked View"));
        }
        isFreeView = !isFreeView;


    });

    connect(buttonLine, &QPushButton::clicked, this, [this]() {
        if(currentState == State::Rectify){

        }else{
            QMessageBox m_box(QMessageBox::Information, tr("Prompt Information"), tr("Please perform head position correction first!"));
            QTimer::singleShot(500, &m_box, SLOT(accept()));
            m_box.exec();
        }


    });
    connect(buttonclearLine,&QPushButton::clicked,this,[this](){
        if(!smileStyle->isHasLine){

            // 设置相机的新位置
            //camera->SetPosition(0, 0, -300);

            // 设置相机焦点为模型中心

            // 可选: 设置相机的上方向，防止图像上下颠倒
            //camera->SetViewUp(0, -1, 0); // 根据实际情况调整
            renderer_main->ResetCamera();
            renderer_main->GetRenderWindow()->Render();
            w3D_main->renderWindow()->GetInteractor()->SetInteractorStyle(smileStyle);
            smileStyle->autoAddPoint();
            if(firstimport){
                if (!assembly1) {
                    cout<<"没有整体牙"<<endl;
                    importTooth("./resources/teeth");

                }
                firstimport = false;
            }
            buttonclearLine->setText(tr("Remove Auxiliary Line"));
            polylinelabel->show();
            polylinecomboBox->show();
        }else{
            smileStyle->removePoint();
            smileStyle->isHasLine = false;
            buttonclearLine->setText(tr("Add Auxiliary Line"));
            polylinelabel->hide();
            polylinecomboBox->hide();
        }

    });
    detector = dlib::get_frontal_face_detector();

    // 加载面部关键点检测模型
    try {
        dlib::deserialize("./Config/shape_predictor_68_face_landmarks.dat") >> predictor;
        cout << "model loaded" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading shape predictor: " << e.what() << std::endl;
        return;
    }
    // assembly1 = vtkSmartPointer<vtkActor>::New();
    // assembly2 = vtkSmartPointer<vtkActor>::New();
    // assembly3 = vtkSmartPointer<vtkActor>::New();
    // assembly4 = vtkSmartPointer<vtkActor>::New();

    finishWindow = nullptr;
}
void SmileDesigenWidget::initUI() {
    matchWindow = new AlignWindow();
    Qt::WindowFlags flags = this->windowFlags();
    flags |= Qt::WindowStaysOnTopHint;  // 添加置顶标志
    //matchWindow->setWindowFlags(Qt::FramelessWindowHint);
    teethPreview = new TeethPreview();
    //teethPreview->setWindowFlags(flags);
    stackedWidget = new QStackedWidget;
    stackedWidget->setStyleSheet("QWidget{background-color: #2F2F2F;}");
    std::cout << "ssssss" << endl;
    //matchWindow->show();
    w3D_main = new  QVTKOpenGLNativeWidget(this);
    w3D_coronal = new  QVTKOpenGLNativeWidget(this);
    w3D_sagittal = new  QVTKOpenGLNativeWidget(this);
    w3D_top = new  QVTKOpenGLNativeWidget(this);
    widgetButtton = new QWidget(this);
    widgetButtton->setStyleSheet("QWidget{background-color: #2F2F2F}");


    widgetFixed = new QWidget(this);
    widgetFixed->setStyleSheet("QWidget{background-color: #2F2F2F;}");
    widgetActor = new ActorWidget(this);
    widgetActor->setStyleSheet("QWidget{background-color: #2F2F2F;}");
    //widgetActor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //widget4 = new QWidget(this);
    //widget4->setObjectName("widget4");
    //widget4->setStyleSheet("QWidget{background-color: rgb(255,255,255);}");

    //widget4->setMaximumWidth(300);  // 限制右侧控件栏的最大宽度为 300 像素

    buttonImport = new QPushButton(this);
    buttonImport->setText(tr("Import Model"));

    buttonMatch = new QPushButton(this);
    buttonMatch->setText(tr("Model Matching"));
    buttonCalibrate = new QPushButton(this);
    buttonCalibrate->setText(tr("Head Alignment"));
    buttonAutoCut = new QPushButton(this);
    buttonAutoCut->setText(tr("Remove Teeth"));



    buttonFree = new QPushButton(tr("Free View"), w3D_main);
    // 计算按钮的位置，使其在右下角
    int buttonWidth = 100;
    int buttonHeight = 30;
    int xPos = w3D_main->width() - buttonWidth;
    int yPos = w3D_main->height() - buttonHeight;

    // 设置按钮的几何位置
    buttonFree->setGeometry(xPos, yPos, buttonWidth, buttonHeight);
    buttonFree->hide();

    buttonLine = new QPushButton(this);
    buttonLine->setText(tr("Auxiliary Line"));
    buttonclearLine = new QPushButton(this);
    buttonclearLine->setText(tr("Add Auxiliary Line"));

    checkBox = new QCheckBox(this);
    checkBox->setText(tr("Adjust Overall"));
    // 设置 QCheckBox 样式
    checkBox->setStyleSheet(R"(
            QCheckBox {
                font-size: 16px; /* 设置字体大小 */
                color: white;    /* 设置字体颜色 */
            }
            QCheckBox::indicator {
                width: 50px; /* 根据图标大小调整 */
                height: 50px;
            }
            QCheckBox::indicator:unchecked {
                image: url(./resources/pic/toggle-off.png); /* 关闭状态图标 */
            }
            QCheckBox::indicator:checked {
                image: url(./resources/pic/toggle-on.png); /* 开启状态图标 */
            }
        )");





    teethlabel = new QLabel(tr("Select Teeth Model"), this);
    teethlabel->setStyleSheet("font-size: 16px; color: white;");

    teethComboBox = new QComboBox(this);

    teethComboBox->setItemDelegate(new CustomComboBoxDelegate(teethComboBox));
    teethComboBox->setStyleSheet(R"(
    QComboBox {
        background-color: #5D5D5D;    /* 使用背景颜色，与按钮一致 */
        color: white;                 /* 字体颜色 */
        border-radius: 8px;           /* 圆角 */
        padding: 8px;                 /* 内边距 */
        font-size: 18px;              /* 字体大小 */
    }

    QComboBox:hover {
        background-color:  #1296db; /* 悬停时的背景渐变 */
    }

    QComboBox QAbstractItemView {
        background-color: #2C2C2C;  /* 下拉选项背景颜色 */
        color: white;               /* 下拉选项字体颜色 */
        selection-background-color: #1296db; /* 选中项背景渐变 */
        border: 1px solid #2C2C2C;  /* 加边框确保圆角效果 */
        border-radius: 4px;         /* 圆角 */
    }

    /* 下拉按钮样式 */
    QComboBox::drop-down {
        subcontrol-origin: padding;
        subcontrol-position: top right;
        width: 15px;                   /* 按钮宽度 */
        border-left: 1px solid gray;   /* 左边线 */
        border-top-right-radius: 8px;  /* 右上圆角 */
        border-bottom-right-radius: 8px; /* 右下圆角 */
        background-color: transparent; /* 设置为透明，让 QComboBox:hover 控制颜色 */
    }

    QComboBox::item {
        border-radius: 4px;   /* 每个选项的圆角 */
        padding: 8px;         /* 内边距 */
    }

    /* 自定义下拉箭头 */
    QComboBox::down-arrow {
        image: url("./resources/pic/down_arrow.png");
        width: 10px;/*设置该图标的宽高*/
            height: 10px;
    }

    QComboBox::item:selected {
        background-color:  #1296db; /* 选中项背景渐变 */
        color: white;         /* 选中项字体颜色 */
    }
)");


    //teethComboBox->setIconSize(QSize(50, 50)); // 使得当前项的图标大小和下拉列表中保持一致


    teethComboBox->addItem(QIcon("./resources/pic/teeth.png"), tr("Model 1"));
    teethComboBox->addItem(QIcon("./resources/pic/teeth1.png"), tr("Model 2"));
    teethComboBox->addItem(QIcon("./resources/pic/teeth2.png"), tr("Model 3"));
    teethComboBox->addItem(QIcon("./resources/pic/teeth3.png"), tr("Model 4"));
    //layoutWidget->insertWidget(2, teethComboBox);  // 将 teethComboBox 插入到第 1 个位置
    //layoutW2->addWidget(teethComboBox);
    connect(teethComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index){
        // 处理选择更改事件
        qDebug() << "Selected Index:" << index << ", Text:" << teethComboBox->currentText();

        switch(index){
        case 0:{
            cout<<"teethComboBox index==0"<<endl;
            importTooth("./resources/teeth");
            break;
        }
        case 1:{
            cout<<"teethComboBox index==1"<<endl;
            importTooth("./resources/teeth/style");
            break;
        }
        case 2:{
            importTooth("./resources/teeth/style6.4");
            break;
        }
        case 3:{
            importTooth("./resources/teeth/style6.4");
            break;
        }
        }
    });
    // 添加按钮，点击时显示/隐藏悬浮窗口
    QPushButton* showDialogButton = new QPushButton(tr("Adjust Color"), this);

    QHBoxLayout* layoutSmile = new QHBoxLayout();
    layoutSmile->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* layoutmain = new QVBoxLayout();
    QVBoxLayout* layoutotherview = new QVBoxLayout();
    layoutWidget = new QVBoxLayout();
    //layoutWidget->addStretch(0);
    //layoutWidget->setSpacing(3);
    QVBoxLayout* layoutButton = new QVBoxLayout();
    layoutfixed = new QVBoxLayout();
    //layoutW3 = new QVBoxLayout();
    layoutmain->setContentsMargins(0, 0, 0, 0);
    layoutotherview->setContentsMargins(0, 0, 0, 0);
    layoutWidget->setContentsMargins(0, 0, 0, 0);
    layoutButton->setContentsMargins(0, 15, 0, 15);
    layoutfixed->setContentsMargins(4,3,4,3);
    //layoutW3->setContentsMargins(0, 0, 0, 0);

    layoutButton->addWidget(buttonImport);
    layoutButton->addWidget(buttonMatch);
    layoutButton->addWidget(buttonCalibrate);
    layoutButton->addWidget(buttonAutoCut);
    layoutButton->addWidget(buttonLine);
    layoutButton->addWidget(showDialogButton);





    layoutButton->setAlignment(Qt::AlignHCenter);  // 设置整个 layoutW1 布局中的控件水平居中
    layoutButton->setSpacing(15);  // 设置按钮之间的垂直间距


    //layoutW2->setAlignment(Qt::AlignHCenter);
    widgetButtton->setLayout(layoutButton);
    widgetFixed->setLayout(layoutfixed);
    layoutSmile->addLayout(layoutotherview);
    layoutSmile->addLayout(layoutmain);

    layoutSmile->addLayout(layoutWidget);
    layoutSmile->setStretch(0, 3);
    layoutSmile->setStretch(1, 5);
    layoutSmile->setStretch(2, 1);

    layoutmain->addWidget(w3D_main);
    layoutotherview->addWidget(w3D_coronal);
    layoutotherview->addWidget(w3D_top);
    layoutotherview->addWidget(w3D_sagittal);
    widgetButtton->setMinimumWidth(250);
    layoutWidget->addWidget(widgetButtton);

    //widgetBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    layoutfixed->setAlignment(Qt::AlignHCenter);

    layoutWidget->addWidget(stackedWidget);
    layoutWidget->addWidget(widgetFixed);
    //layoutfixed->addStretch(0); //加弹簧
    layoutfixed->setSpacing(0); // 设置间距

    widgetActor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    widgetActor->setFixedWidth(180);
    layoutfixed->insertWidget(0,widgetActor);

    teethComboBox->setFixedSize(180,40);
    //layoutfixed->insertWidget(0,teethComboBox);

    //teethlabel->setFixedWidth(180);
    //layoutfixed->insertWidget(0,teethlabel);

    checkBox->setFixedWidth(180);

    layoutfixed->insertWidget(0,checkBox);




    //layoutfixed->addWidget(buttonclearLine);





    QPixmap pixmap("./resources/pic/lefteye.png"); // 替换为你的图片路径
    tip = new TipWidget(tr("Click on the left eye"), pixmap);
    tip->setFixedWidth(180);
    //layoutfixed->insertWidget(layoutfixed->count(), tip);
    //widget4->setLayout(layoutW3);


    buttonFlip = new QPushButton(tr("Flip View"), w3D_sagittal);
    buttonFlip->setGeometry(0, 0, 90, 30);

    QIcon icon("./resources/pic/test2.png"); // 资源文件路径或本地文件路径
    buttonFlip->setIcon(icon); // 设置图标
    buttonFlip->setIconSize(QSize(20, 20)); // 设置图标大小
    this->setLayout(layoutSmile);
    std::vector<std::vector<vtkSmartPointer<vtkActor>>> allTeeth = { tooth1, tooth2, tooth3, tooth4 };
    std::vector<vtkSmartPointer<vtkActor>> allAssembly = { assembly1,assembly2,assembly3,assembly4 };

    // 创建滑块和标签
    QSlider* specularSlider = new QSlider(Qt::Horizontal, this);
    specularSlider->setRange(0, 100);  // 0 to 100 对应 0.0 到 1.0
    specularSlider->setValue(80);  // 设置初始值为中间值
    specularSlider->setStyleSheet("border: none; ");
    //QSlider* specularPowerSlider = new QSlider(Qt::Horizontal, this);
    //specularPowerSlider->setRange(1, 100);  // Specular Power 通常在 1 到 100 之间
    //specularPowerSlider->setValue(80);  // 设置初始值为中间值

    QSlider* ambientSlider = new QSlider(Qt::Horizontal, this);
    ambientSlider->setRange(0, 100);  // 0 to 100 对应 0.0 到 1.0
    ambientSlider->setValue(20);  // 设置初始值为 0.3



    QString sliderStyle =
        "QSlider::groove:horizontal {"
        "background: #d3d3d3;"                   // 滑轨背景颜色
        "height: 10px;"
        "border-radius: 5px;"
        "border: none; "
        "}"

        "QSlider::handle:horizontal {"
        "background: #73C4F6;" // 滑块渐变颜色
        "width: 10px;"                          // 滑块宽度
        "height: 10px;"                         // 滑块高度
        "margin: -5px 0;"                       // 调整滑块的位置
        "border-radius: 10px;"                  // 滑块圆角
        "border: none; "
        "}"

        "QSlider::add-page:horizontal {"
        "background: #d3d3d3;"                  // 未滑动部分颜色
        "border-radius: 5px;"
        "border: none; "
        "}"

        "QSlider::sub-page:horizontal {"
        "background:  #1296db;" // 滑动部分渐变颜色
        "border-radius: 5px;"
        "border: none; "
        "}";
    QList<QSlider*>sliders = {specularSlider,ambientSlider};

    for(auto slider:sliders){
        slider->setStyleSheet(sliderStyle);
    }
    // 连接滑块信号到槽函数
    connect(specularSlider, &QSlider::valueChanged, this, [=](int value) {
        double specular = value / 100.0;


        assembly1->GetProperty()->SetSpecular(specular);
        assembly2->GetProperty()->SetSpecular(specular);
        assembly3->GetProperty()->SetSpecular(specular);
        assembly4->GetProperty()->SetSpecular(specular);
        w3D_main->renderWindow()->Render();
        w3D_coronal->renderWindow()->Render();
        w3D_sagittal->renderWindow()->Render();
        w3D_top->renderWindow()->Render();
        qDebug() << "Specular value:" << specular;  // 输出滑块的值到调试控制台
    });

    // connect(specularPowerSlider, &QSlider::valueChanged, this, [=](int value) {



    // 	assembly1->GetProperty()->SetSpecularPower(value);
    // 	assembly2->GetProperty()->SetSpecularPower(value);
    // 	assembly3->GetProperty()->SetSpecularPower(value);
    // 	assembly4->GetProperty()->SetSpecularPower(value);
    // 	w3D_main->renderWindow()->Render();
    // 	w3D_coronal->renderWindow()->Render();
    // 	w3D_sagittal->renderWindow()->Render();
    // 	w3D_top->renderWindow()->Render();
    // 	qDebug() << "Specular Power value:" << value;  // 输出滑块的值到调试控制台
    // 	});

    connect(ambientSlider, &QSlider::valueChanged, this, [=](int value) {
        double ambient = value / 100.0;  // 转换为 0.0 到 1.0 范围




        assembly1->GetProperty()->SetAmbient(ambient);
        assembly2->GetProperty()->SetAmbient(ambient);
        assembly3->GetProperty()->SetAmbient(ambient);
        assembly4->GetProperty()->SetAmbient(ambient);

        w3D_main->renderWindow()->Render();
        w3D_coronal->renderWindow()->Render();
        w3D_sagittal->renderWindow()->Render();
        w3D_top->renderWindow()->Render();
        qDebug() << "Ambient value:" << ambient;  // 输出滑块的值到调试控制台
    });



    // 创建颜色选择器
    QWidget* colorPickerWidget = new QWidget(this);

    QGridLayout* colorPickerLayout = new QGridLayout(colorPickerWidget);
    colorPickerLayout->setContentsMargins(0,0,0,0);
    QList<QColor> colors = {
        QColor(235,225,223), QColor(225,217,211), QColor(238,225,203), QColor(242,225,203),QColor(237,221,194),
        QColor(231,223,219), QColor(223,219,217), QColor(229,222,215), QColor(238,223,200),QColor(221,216,206),
        QColor(229,221,206), QColor(229,213,200), QColor(236,227,208), QColor(224,216,207),QColor(224,216,202),
        QColor(233,216,197), QColor(231,231,234), QColor(231,231,228), QColor(235,234,229),QColor(250,250,250)
    };

    QStringList labels = {
        "A1", "A2", "A3", "A3.5","A4",
        "B1", "B2", "B3", "B4","C1",
        "C2", "C3", "C4", "D2","D3",
        "D4", "OM1", "OM2", "OM3","OM4"
    };

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);  // 设置为独占模式
    int cols = 5;  // 定义 cols 为列数

    for (int i = 0; i < colors.size(); ++i) {
        QPushButton* colorButton = new QPushButton(this);
        colorButton->setFixedSize(30, 30);
        colorButton->setCheckable(true);  // 设置按钮为可选中状态
        colorButton->setStyleSheet(QString("background-color: %1;").arg(colors[i].name()));

        // 添加标签
        QLabel* colorLabel = new QLabel(labels[i], colorButton);
        colorLabel->setAlignment(Qt::AlignCenter);
        colorLabel->setStyleSheet("color: black; background-color: rgba(255, 255, 255, 0); font-size: 12px;");  // 设置字体大小为 12px


        QVBoxLayout* buttonLayout = new QVBoxLayout(colorButton);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->addWidget(colorLabel);

        buttonGroup->addButton(colorButton, i);
        colorPickerLayout->addWidget(colorButton, i / cols, i % cols);

        connect(colorButton, &QPushButton::clicked, this, [this, colorButton, color = colors[i]]()
                {
                    assembly1->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
                    assembly2->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
                    assembly3->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
                    assembly4->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
                    w3D_main->renderWindow()->Render();
                    w3D_coronal->renderWindow()->Render();
                    w3D_sagittal->renderWindow()->Render();
                    w3D_top->renderWindow()->Render();
                    qDebug() << "Selected color:" << color.name();
                });
    }

    // 可以通过样式表在选中时添加边框
    colorPickerWidget->setStyleSheet(R"(
    QPushButton {
        border: 1px solid gray;
        border-radius: 4px;
    }
    QPushButton:checked {
        border: 2px solid blue;
    }
)");


    // 创建悬浮窗口，并设置为隐藏状态
    floatingDialog = new QWidget(this);
    floatingDialog->setStyleSheet("border: none; ");
    floatingDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    floatingDialog->setWindowModality(Qt::NonModal);
    floatingDialog->setAttribute(Qt::WA_DeleteOnClose, false);
    //floatingDialog->setFixedSize(250, 300);  // 设置悬浮区域的大小
    QVBoxLayout* floatingLayout = new QVBoxLayout(floatingDialog);
    // 例如：设置floatingLayout的内边距为0
    floatingLayout->setContentsMargins(0, 2, 0, 2);

    // 将你的滑块和颜色选择器添加到悬浮窗口中
    QLabel* specularLabel = new QLabel(tr("Reflectivity"), floatingDialog);
    specularLabel->setStyleSheet("color: white;");
    floatingLayout->addWidget(specularLabel);
    floatingLayout->addWidget(specularSlider);

    //QLabel* specularPowerLabel = new QLabel(tr("Glossiness"), floatingDialog);
    //specularPowerLabel->setStyleSheet("color: white;");
    // floatingLayout->addWidget(specularPowerLabel);
    // floatingLayout->addWidget(specularPowerSlider);

    QLabel* ambientLabel = new QLabel(tr("Ambient Light"), floatingDialog);
    ambientLabel->setStyleSheet("color: white;");
    floatingLayout->addWidget(ambientLabel);
    floatingLayout->addWidget(ambientSlider);



    floatingLayout->addWidget(colorPickerWidget);

    QPushButton* buttonBool = new QPushButton(this);
    buttonBool->setText(tr("Calculate Veneer"));

    connect(buttonBool,&QPushButton::clicked,this,[=](){
        if(currentState==State::Rectify && assembly1){
            checkBox->setCheckState(Qt::Checked);
            boolteeth1();
            //test();
        }else{
            cout<<"dont bool"<<endl;
        }
    });

    layoutButton->addWidget(buttonBool);

    QList<QPushButton*> buttons = { buttonFree,buttonFlip,buttonImport, buttonMatch,
                                    buttonCalibrate, buttonAutoCut, showDialogButton, buttonLine, buttonBool,
                                    buttonclearLine };

    // 批量设置按钮样式
    for (QPushButton* button : buttons) {
        button->setFixedSize(180,40);
        button->setStyleSheet(R"(
        QPushButton {
            background-color: #5D5D5D; /* 默认状态下背景颜色为 #5D5D5D */
            color: white;               /* 默认字体颜色为白色 */
            border-radius: 8px;
            padding: 8px;
            font-size: 18px;
        }
        QPushButton:hover {
            background:  #1296db; /* 悬停时渐变背景 */
            color: white;               /* 悬停时字体颜色为白色 */
        }
        QPushButton:pressed {
            background: #1296db; /* 点击时渐变背景 */
            color: white;               /* 点击时字体颜色为白色 */
        }
    )");
    }


    connect(showDialogButton, &QPushButton::clicked, this, [=]() {
        cout<<"showDialogButton950"<<endl;
        if (assembly1) {
            cout<<"showDialogButton950if"<<endl;
            handleStyle1->removeBox();
            handleStyle2->removeBox();
            handleStyle3->removeBox();
            checkBox->setCheckState(Qt::Checked);

            // floatingDialog->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            // floatingDialog->adjustSize();  // 确保对话框适应内容大小
            floatingDialog->setFixedWidth(180);
        }
    });

    // 创建标签
    polylinelabel = new QLabel(tr("Please select the teeth auxiliary line ratio:"), this);
    polylinelabel->setStyleSheet("background-color: #2C2C2C; font-size: 16px; color: white;"); // 设置字体大小为18px，颜色为白色
    polylinelabel->setFixedWidth(180);
    //layoutfixed->insertWidget(layoutfixed->count(),polylinelabel);

    // 创建下拉框
    polylinecomboBox = new QComboBox(this);
    polylinecomboBox->setFixedSize(180,40);
    polylinecomboBox->setStyleSheet(R"(
    QComboBox {
        background-color: #5D5D5D;    /* 使用背景颜色，与按钮一致 */
        color: white;                 /* 字体颜色 */
        border-radius: 8px;           /* 圆角 */
        padding: 8px;                 /* 内边距 */
        font-size: 18px;              /* 字体大小 */
    }

    QComboBox:hover {
        background-color: #1296db; /* 悬停时的背景渐变 */
    }

    QComboBox QAbstractItemView {
        background-color: #2C2C2C;  /* 下拉选项背景颜色 */
        color: white;               /* 下拉选项字体颜色 */
        selection-background-color: #1296db; /* 选中项背景渐变 */
        border: 1px solid #2C2C2C;  /* 加边框确保圆角效果 */
        border-radius: 4px;         /* 圆角 */
    }

    /* 下拉按钮样式 */
    QComboBox::drop-down {
        subcontrol-origin: padding;
        subcontrol-position: top right;
        width: 15px;                   /* 按钮宽度 */
        border-left: 1px solid gray;   /* 左边线 */
        border-top-right-radius: 8px;  /* 右上圆角 */
        border-bottom-right-radius: 8px; /* 右下圆角 */
        background-color: transparent; /* 设置为透明，让 QComboBox:hover 控制颜色 */
    }

    QComboBox::item {
        border-radius: 4px;   /* 每个选项的圆角 */
        padding: 8px;         /* 内边距 */
    }

    /* 自定义下拉箭头 */
    QComboBox::down-arrow {
        image: url("./resources/pic/down_arrow.png");
        width: 10px;/*设置该图标的宽高*/
            height: 10px;
    }

    QComboBox::item:selected {
        background-color: #1296db; /* 选中项背景渐变 */
        color: white;         /* 选中项字体颜色 */
    }
)");

    // 添加选项
    polylinecomboBox->addItem(("0.8"));
    polylinecomboBox->addItem(("0.7"));
    polylinecomboBox->addItem(("0.618"));
    //layoutfixed->insertWidget(layoutfixed->count(),polylinecomboBox);

    connect(polylinecomboBox, &QComboBox::currentTextChanged, this, [=](const QString &text) {

        smileStyle->factor = text.toDouble();  // 将选中的文本转换为 double 并赋值给变量
        smileStyle->gennerateToothLine();
    });
    //polylinelabel->hide();
    //polylinecomboBox->hide();

    QWidget *page0 = new QWidget(stackedWidget);
    //QVBoxLayout *layout1 = new QVBoxLayout(page1);

    QWidget *page1 = new QWidget(stackedWidget);
    QVBoxLayout *layout1 = new QVBoxLayout(page1);
    layout1->addStretch(0);
    layout1->setSpacing(0);
    layout1->setAlignment(Qt::AlignHCenter);

    layout1->insertWidget(0,tip);

    tip->hide();

    QWidget *page2 = new QWidget(stackedWidget);
    QVBoxLayout *layout2 = new QVBoxLayout(page2);
    layout2->addStretch(0);
    //layout2->setSpacing(20);
    layout2->setAlignment(Qt::AlignHCenter);



    layout2->insertWidget(0,teethComboBox);
    layout2->insertWidget(0,teethlabel);
    // 插入间距（在 polylinecomboBox 后添加间距）
    layout2->insertSpacing(0, 25);

    layout2->insertWidget(0,polylinecomboBox);
    layout2->insertWidget(0,polylinelabel);


    layout2->insertWidget(0,buttonclearLine);
    teethlabel->hide();
    teethComboBox->hide();
    polylinecomboBox->hide();
    polylinelabel->hide();

    QWidget *page3 = new QWidget(stackedWidget);
    QVBoxLayout *layout3 = new QVBoxLayout(page3);
    layout3->addStretch(0);
    layout3->setSpacing(0);
    layout3->setAlignment(Qt::AlignHCenter);
    layout3->insertWidget(0,floatingDialog);

    stackedWidget->addWidget(page0);
    stackedWidget->addWidget(page1);
    stackedWidget->addWidget(page2);
    stackedWidget->addWidget(page3);
    connect(buttonCalibrate, &QPushButton::clicked, this, [=](){

        stackedWidget->setCurrentIndex(1); // 显示第一个页面
    });

    connect(buttonLine, &QPushButton::clicked, this, [=](){

        if(currentState == State::Rectify){
            stackedWidget->setCurrentIndex(2); // 显示第一个页面
        }

    });

    connect(showDialogButton, &QPushButton::clicked, this, [=](){
        cout<<"showDialogButton1096"<<endl;
        if(assembly1){
            stackedWidget->setCurrentIndex(3); // 显示第一个页面
            cout<<"showDialogButton1096if"<<endl;
        }

    });

}

void SmileDesigenWidget::initVTK() {

    vtkSmartPointer<vtkGenericOpenGLRenderWindow>   win1 = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    vtkSmartPointer<vtkGenericOpenGLRenderWindow>   win2 = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    vtkSmartPointer<vtkGenericOpenGLRenderWindow>   win3 = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    vtkSmartPointer<vtkGenericOpenGLRenderWindow>   win4 = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renderer_main = vtkSmartPointer<vtkRenderer>::New();
    renderer_main->SetBackground(44/255.0, 44/255.0, 44/255.0);
    // 设置背景的渐变颜色
    // renderer_main->SetBackground(0.1, 0.2, 0.3);  // 设置顶部颜色 (R, G, B) 范围在 [0, 1]
    // renderer_main->SetBackground2(0.5, 0.7, 0.9); // 设置底部颜色 (R, G, B) 范围在 [0, 1]

    // // 启用渐变背景
    // renderer_main->GradientBackgroundOn();
    renderer_coronal = vtkSmartPointer<vtkRenderer>::New();
    renderer_coronal->SetBackground(44/255.0, 44/255.0, 44/255.0);
    // renderer_coronal->SetBackground2(0.5, 0.7, 0.9);
    // renderer_coronal->GradientBackgroundOn();
    vtkInformation* info_coronal = renderer_coronal->GetInformation();
    info_coronal->Set(RendererIDKey, "Renderer_1");
    cout << "renderer_coronal" << renderer_coronal << endl;

    renderer_sagittal = vtkSmartPointer<vtkRenderer>::New();
    renderer_sagittal->SetBackground(44/255.0, 44/255.0, 44/255.0);
    vtkInformation* info_sagittal = renderer_sagittal->GetInformation();
    info_sagittal->Set(RendererIDKey, "Renderer_2");
    cout << "renderer_sagittal" << renderer_sagittal << endl;

    renderer_top = vtkSmartPointer<vtkRenderer>::New();
    renderer_top->SetBackground(44/255.0, 44/255.0, 44/255.0);
    vtkInformation* info_top = renderer_top->GetInformation();
    info_top->Set(RendererIDKey, "Renderer_3");
    cout << "renderer_top" << renderer_top << endl;

    double campos[3] = {0, 0, 300};
    double focalpoint[3] = {0, 0, 0};
    double viewup[3] = {0, 1, 0};
    vtkCamera* camera_main = renderer_main->GetActiveCamera();
    camera_main->SetViewUp(viewup);
    camera_main->SetPosition(campos);
    camera_main->SetFocalPoint(focalpoint);
    // 主光源（Key Light） - 从右上方照射
    // vtkSmartPointer<vtkLight> light1 = vtkSmartPointer<vtkLight>::New();
    // light1->SetPosition(100, 100, 300);
    // light1->SetFocalPoint(0, 0, 0);
    // light1->SetColor(1.0, 1.0, 1.0); // 白色光
    // light1->SetIntensity(1.0);
    // renderer_main->AddLight(light1);

    // // 创建补光源
    // vtkSmartPointer<vtkLight> light2 = vtkSmartPointer<vtkLight>::New();
    // light2->SetPosition(-100, -100, 300);
    // light2->SetFocalPoint(0, 0, 0);
    // light2->SetColor(0.8, 0.8, 0.8); // 略微偏暗的光
    // light2->SetIntensity(0.5);
    // renderer_main->AddLight(light2);

    // 设置环境光
    renderer_main->SetAmbient(0.2, 0.2, 0.2);


    vtkCamera* camera_coronal = renderer_coronal->GetActiveCamera();
    camera_coronal->SetViewUp(0, 1, 0);
    camera_coronal->SetPosition(0, 0, 500);
    camera_coronal->SetFocalPoint(0, 0, 0);
    camera_coronal->SetParallelScale(20);
    camera_coronal->ParallelProjectionOn();
    vtkCamera* camera_sagittal = renderer_sagittal->GetActiveCamera();
    camera_sagittal->SetViewUp(0, 1, 0);
    camera_sagittal->SetPosition(-250, 0, 25);
    camera_sagittal->SetFocalPoint(0, 0, 25);
    camera_sagittal->SetParallelScale(25);
    camera_sagittal->ParallelProjectionOn();
    vtkCamera* camera_top = renderer_top->GetActiveCamera();
    camera_top->SetViewUp(0, 0, 1);
    camera_top->SetFocalPoint(0, 0, 30);
    camera_top->SetPosition(0, -300, 30);
    camera_top->SetParallelScale(25);
    camera_top->ParallelProjectionOn();
    w3D_main->setRenderWindow(win1);
    w3D_coronal->setRenderWindow(win2);
    w3D_sagittal->setRenderWindow(win3);
    w3D_top->setRenderWindow(win4);
    w3D_main->renderWindow()->AddRenderer(renderer_main);
    w3D_coronal->renderWindow()->AddRenderer(renderer_coronal);
    w3D_sagittal->renderWindow()->AddRenderer(renderer_sagittal);
    w3D_top->renderWindow()->AddRenderer(renderer_top);


    connect(widgetActor, &ActorWidget::refresh, this, &SmileDesigenWidget::syncRefresh);

    connect(this,&SmileDesigenWidget::importtoothFinish,this,[this](){this->isFisrstImport = false;});
}

void SmileDesigenWidget::initStyle() {
    zoomStyle = vtkSmartPointer<ZoomOnlyInteractorStyle>::New();

    selectStyle = vtkSmartPointer<PolygonSelectInteractorStyle>::New();
    pickStyle = vtkSmartPointer<PickPointSelectionStyle> ::New();
    pickStyle->SetCallback([this](int step, double x, double y, double z) {
        emit pointSelected(step, x, y, z);
    });
    smileStyle = vtkSmartPointer<SmileLineStyle>::New();
    smileStyle->SetRenderer(renderer_main);

    handleStyle1 = vtkSmartPointer<HandleInteractor>::New();
    handleStyle2 = vtkSmartPointer<HandleInteractor>::New();
    handleStyle3 = vtkSmartPointer<HandleInteractor>::New();
    MyQtObject* myqtObject = new MyQtObject();
    handleStyle1->setqtObject(myqtObject);
    handleStyle2->setqtObject(myqtObject);
    handleStyle3->setqtObject(myqtObject);


    handleStyle1->setRender(renderer_coronal);
    handleStyle2->setRender(renderer_sagittal);
    handleStyle3->setRender(renderer_top);

    connect(myqtObject, SIGNAL(refresh()), this, SLOT(syncRefresh()));
    w3D_coronal->renderWindow()->GetInteractor()->SetInteractorStyle(handleStyle1);
    w3D_sagittal->renderWindow()->GetInteractor()->SetInteractorStyle(handleStyle2);
    w3D_top->renderWindow()->GetInteractor()->SetInteractorStyle(handleStyle3);

}
void SmileDesigenWidget::importTooth(const QString& teethpath) {
    cout<<"import tooth"<<endl;
    m_teethpath = teethpath;
    namespace fs = std::filesystem;
    Eigen::Matrix4d mat;
    if (pathToMatrixMap.find(teethpath) != pathToMatrixMap.end()) {
        mat = pathToMatrixMap[teethpath];
        cout<<mat<<endl;
    } else {
        std::cerr << "未找到对应路径的矩阵，使用默认矩阵" << std::endl;
        mat << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1; // 默认单位矩阵
    }

    vtkMatrix4x4* transM = vtkMatrix4x4::New();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            transM->SetElement(i, j, mat(i, j));
        }
    }


    if (assembly1 != nullptr) {
        cout<<"clear assmble"<<endl;
        renderer_main->RemoveActor(assembly1);
        renderer_coronal->RemoveActor(assembly2);
        renderer_sagittal->RemoveActor(assembly3);
        renderer_top->RemoveActor(assembly4);
        assembly1 = nullptr;
        assembly2 = nullptr;
        assembly3 = nullptr;
        assembly4 = nullptr;
    }


    if (tooth1.size() != 0) {
        for (int i = 0; i < tooth1.size(); i++) {
            renderer_main->RemoveActor(tooth1[i]);
            renderer_coronal->RemoveActor(tooth2[i]);
            renderer_sagittal->RemoveActor(tooth3[i]);
            renderer_top->RemoveActor(tooth4[i]);
        }
    }


    tooth1.clear();
    tooth2.clear();
    tooth3.clear();
    tooth4.clear();
    fs::path dirPath = fs::path(teethpath.toStdString());

    try {

        if (fs::exists(dirPath) && fs::is_directory(dirPath)) {

            for (const auto& entry : fs::directory_iterator(dirPath)) {
                // 判断当前条目是否是文件夹，如果是则跳过
                if (entry.is_directory()) {
                    continue; // 跳过文件夹
                }

                std::string file = entry.path().string();
                std::string fileName = entry.path().string();
                cout<<"file="<<file<<endl;
                cout<<"fileName="<<fileName<<endl;
                if (fileName.size() > 4 && fileName.substr(file.size() - 4) == ".stl") {
                    // 提取文件名中的编号部分

                    std::string numberStr = entry.path().stem().string();  // 使用 stem() 获取不带扩展名的部分
                    //std::string numberStr = file.substr(0, file.size() - 4);
                    int toothNumber = std::stoi(numberStr);
                    cout<<"toothNumber="<<toothNumber<<endl;
                    // 检查编号是否在 m_selected_teeth 中
                    if (std::find(teethPreview->m_selected_teeth.begin(), teethPreview->m_selected_teeth.end(), toothNumber) != teethPreview->m_selected_teeth.end()) {

                        vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
                        reader->SetFileName(file.c_str());
                        reader->Update();
                        vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
                        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
                        transform->SetMatrix(transM);

                        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
                        transformFilter->SetTransform(transform);
                        transformFilter->SetInputData(polyData);
                        transformFilter->Update();
                        vtkSmartPointer<vtkPolyData> transPolyData = transformFilter->GetOutput();

                        for (int i = 1; i < 5; i++) {
                            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                            mapper->SetInputData(transPolyData);
                            vtkSmartPointer<vtkActor> teeth = vtkSmartPointer<vtkActor>::New();
                            teeth->SetMapper(mapper);
                            //                           renderer_main->AddActor(teeth);
                            teeth->GetProperty()->SetSpecular(0.8);
                            teeth->GetProperty()->SetSpecularPower(80);
                            teeth->GetProperty()->SetAmbient(0.2);
                            teeth->GetProperty()->SetDiffuse(0.9);
                            switch (i) {
                            case 1: {

                                tooth1.push_back(teeth);
                                break;
                            }
                            case 2: {
                                // cout<<"push::"<<teeth<<endl;
                                tooth2.push_back(teeth);
                                HandleInteractor* currentStyle1 = dynamic_cast<HandleInteractor*>
                                    (w3D_coronal->renderWindow()->GetInteractor()->GetInteractorStyle());
                                currentStyle1->toothList.push_back(teeth);
                                break;
                            }
                            case 3: {
                                cout << "push::" << teeth << endl;
                                tooth3.push_back(teeth);
                                HandleInteractor* currentStyle2 = dynamic_cast<HandleInteractor*>
                                    (w3D_sagittal->renderWindow()->GetInteractor()->GetInteractorStyle());
                                currentStyle2->toothList.push_back(teeth);
                                break;
                            }

                            case 4: {

                                tooth4.push_back(teeth);
                                HandleInteractor* currentStyle3 = dynamic_cast<HandleInteractor*>
                                    (w3D_top->renderWindow()->GetInteractor()->GetInteractorStyle());
                                currentStyle3->toothList.push_back(teeth);
                                break;
                            }

                            }
                        }
                    }

                }
            }
        }
        else {
            std::cerr << "目录不存在或不是一个目录 " << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {

        std::cerr << e.what() << std::endl;
    }
    isFisrstImport = true;

    checkBox->setCheckState(Qt::Checked);
    //cout << "test" << endl;
    double separateColor[3];
    tooth1[0]->GetProperty()->GetColor(separateColor);
    double separateSpecular = tooth1[0]->GetProperty()->GetSpecular();
    double separateSpecularPower = tooth1[0]->GetProperty()->GetSpecularPower();
    double separateAmbient = tooth1[0]->GetProperty()->GetAmbient();
    double separateDiffuse = tooth1[0]->GetProperty()->GetDiffuse();

    for (int i = 0; i < tooth1.size(); i++) {

        renderer_main->RemoveActor(tooth1[i]);
        renderer_coronal->RemoveActor(tooth2[i]);
        renderer_sagittal->RemoveActor(tooth3[i]);
        renderer_top->RemoveActor(tooth4[i]);
    }
    appendFilter->RemoveAllInputs();

    // assembly1 = vtkSmartPointer<vtkActor>::New();
    // assembly2 = vtkSmartPointer<vtkActor>::New();
    // assembly3 = vtkSmartPointer<vtkActor>::New();
    // assembly4 = vtkSmartPointer<vtkActor>::New();
    if (!assembly1) {
        assembly1 = vtkSmartPointer<vtkActor>::New();
    }

    if (!assembly2) {
        assembly2 = vtkSmartPointer<vtkActor>::New();
    }

    if (!assembly3) {
        assembly3 = vtkSmartPointer<vtkActor>::New();
    }

    if (!assembly4) {
        assembly4 = vtkSmartPointer<vtkActor>::New();
    }

    for (int i = 0; i < tooth1.size(); i++) {
        vtkSmartPointer<vtkPolyData> polyData = vtkPolyDataMapper::SafeDownCast(tooth1[i]->GetMapper())->GetInput();
        vtkSmartPointer<vtkIntArray> Identifiers = vtkSmartPointer<vtkIntArray>::New();
        Identifiers->SetName("Identifier");
        Identifiers->SetNumberOfComponents(1);
        Identifiers->SetNumberOfTuples(polyData->GetNumberOfPoints());
        Identifiers->FillComponent(0, i);
        polyData->GetPointData()->AddArray(Identifiers);
        appendFilter->AddInputData(polyData);
    }

    appendFilter->Update();


    vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper1->SetInputData(appendFilter->GetOutput());
    vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputData(appendFilter->GetOutput());
    vtkSmartPointer<vtkPolyDataMapper> mapper3 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper3->SetInputData(appendFilter->GetOutput());
    vtkSmartPointer<vtkPolyDataMapper> mapper4 = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper4->SetInputData(appendFilter->GetOutput());
    assembly1->SetMapper(mapper1);
    assembly2->SetMapper(mapper2);
    assembly3->SetMapper(mapper3);
    assembly4->SetMapper(mapper4);


    assembly1->GetProperty()->SetColor(separateColor);
    assembly1->GetProperty()->SetSpecular(separateSpecular);
    assembly1->GetProperty()->SetSpecularPower(separateSpecularPower);
    assembly1->GetProperty()->SetAmbient(separateAmbient);
    assembly1->GetProperty()->SetDiffuse(separateDiffuse);

    assembly2->GetProperty()->SetColor(separateColor);
    assembly2->GetProperty()->SetSpecular(separateSpecular);
    assembly2->GetProperty()->SetSpecularPower(separateSpecularPower);
    assembly2->GetProperty()->SetAmbient(separateAmbient);
    assembly2->GetProperty()->SetDiffuse(separateDiffuse);

    assembly3->GetProperty()->SetColor(separateColor);
    assembly3->GetProperty()->SetSpecular(separateSpecular);
    assembly3->GetProperty()->SetSpecularPower(separateSpecularPower);
    assembly3->GetProperty()->SetAmbient(separateAmbient);
    assembly3->GetProperty()->SetDiffuse(separateDiffuse);

    assembly4->GetProperty()->SetColor(separateColor);
    assembly4->GetProperty()->SetSpecular(separateSpecular);
    assembly4->GetProperty()->SetSpecularPower(separateSpecularPower);
    assembly4->GetProperty()->SetAmbient(separateAmbient);
    assembly4->GetProperty()->SetDiffuse(separateDiffuse);

    renderer_main->AddActor(assembly1);
    assembleactor.push_back(assembly1);
    renderer_coronal->AddActor(assembly2);
    HandleInteractor* currentstyle1 = dynamic_cast<HandleInteractor*>(w3D_coronal->renderWindow()->GetInteractor()->GetInteractorStyle());
    currentstyle1->mergedActor = assembly2;

    renderer_sagittal->AddActor(assembly3);
    HandleInteractor* currentstyle2 = dynamic_cast<HandleInteractor*>(w3D_sagittal->renderWindow()->GetInteractor()->GetInteractorStyle());
    currentstyle2->mergedActor = assembly3;

    renderer_top->AddActor(assembly4);
    HandleInteractor* currentstyle3 = dynamic_cast<HandleInteractor*>(w3D_top->renderWindow()->GetInteractor()->GetInteractorStyle());
    currentstyle3->mergedActor = assembly4;

    renderer_main->ResetCamera();
    renderer_coronal->ResetCamera();
    renderer_sagittal->ResetCamera();
    renderer_top->ResetCamera();

    syncRefresh();


}


bool SmileDesigenWidget::importFace() {
    // 假设 filePath 是类的成员变量，直接使用它
    std::cout << "import face" << std::endl;

    // 定义有效的文件扩展名
    QStringList validExtensions = { "stl", "ply" };

    // 检查面扫文件
    QFileInfo faceFileInfo(m_facePath);
    if (!faceFileInfo.exists() || !validExtensions.contains(faceFileInfo.suffix().toLower())) {
        QMessageBox::warning(this, tr("Invalid File"), tr("The face file is either non-existent or not in .stl or .ply format."));
        return false;
    }

    // 检查口扫文件
    QFileInfo mouthFileInfo(m_mouth_upper_Path);
    if (!mouthFileInfo.exists() || !validExtensions.contains(mouthFileInfo.suffix().toLower())) {
        QMessageBox::warning(this, tr("Invalid File"), tr("The mouth file is either non-existent or not in .stl or .ply format."));
        return false;
    }
    if (!m_facePath.isEmpty())
    {
        // 移除旧的面扫模型
        renderer_main->RemoveActor(faceActor);
        //checkBox2->setCheckState(Qt::Unchecked);

        // 设置交互样式
        vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        renderer_main->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);

        // 发出信号移除面扫模型
        emit removeItemWhereRenderRemoveMember(tr("Face Scan Model"));

        // 获取文件后缀并转换为小写
        QFileInfo fileInfo(m_facePath);
        QString fileExtension = fileInfo.suffix().toLower();

        if (fileExtension == "stl") {
            // 使用 vtkSTLReader 读取 STL 文件
            vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
            reader->SetFileName(m_facePath.toUtf8().data());
            reader->Update();

            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(reader->GetOutputPort());

            faceActor = vtkSmartPointer<vtkActor>::New();
            faceActor->SetMapper(mapper);
        }
        else if (fileExtension == "ply") {
            // 使用 vtkPLYReader 读取 PLY 文件
            vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
            reader->SetFileName(m_facePath.toUtf8().data());
            reader->Update();
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(reader->GetOutputPort());
            mapper->SetColorModeToDefault();
            mapper->ScalarVisibilityOn();

            faceActor = vtkSmartPointer<vtkActor>::New();
            faceActor->SetMapper(mapper);
            faceActor->GetProperty()->SetAmbient(1.0);
            faceActor->GetProperty()->SetDiffuse(0.0);
            faceActor->GetProperty()->SetSpecular(0.0);
        }
        else {
            qWarning() << "Unsupported file format: " << fileExtension;
            return false;
        }

        // 添加新的面扫模型
        renderer_main->AddActor(faceActor);

        std::vector<vtkSmartPointer<vtkActor>> actors;
        actors.push_back(faceActor);
        addItemWhereRenderAddMember(tr("Face Scan Model"), actors);

        // 重新渲染窗口
        // 获取相机
        vtkSmartPointer<vtkCamera> camera = renderer_main->GetActiveCamera();

        // 获取模型的边界框


        // 设置相机的新位置
        camera->SetPosition(0, 0, -300);

        // 设置相机焦点为模型中心
        camera->SetFocalPoint(0, 0, 0);

        // 可选: 设置相机的上方向，防止图像上下颠倒
        camera->SetViewUp(0, -1, 0); // 根据实际情况调整
        renderer_main->GetRenderWindow()->Render();
        return true;
    }
}

bool SmileDesigenWidget::importMouth() {


    // 定义有效的文件扩展名
    QStringList validExtensions = { "stl", "ply" };

    // 检查面扫文件
    QFileInfo faceFileInfo(m_facePath);
    if (!faceFileInfo.exists() || !validExtensions.contains(faceFileInfo.suffix().toLower())) {
        QMessageBox::warning(this, "Invalid File", "The face file is either non-existent or not in .stl or .ply format.");
        return false;
    }

    // 检查口扫文件
    QFileInfo mouthFileInfo(m_mouth_upper_Path);
    if (!mouthFileInfo.exists() || !validExtensions.contains(mouthFileInfo.suffix().toLower())) {
        QMessageBox::warning(this, tr("Invalid File"), tr("The mouth file is either non-existent or not in .stl or .ply format."));
        return false;
    }




    if (isMatched) {
        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

        QFile file(m_transPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    in >> matrix->Element[i][j];
                }
            }
            file.close();
        }
        else {
            qWarning() << "Failed to open file for reading" << endl;
        }

        m_transform2 = vtkSmartPointer<vtkTransform>::New();
        m_transform2->SetMatrix(matrix);
    }
    else {

        m_transform2 = vtkSmartPointer<vtkTransform>::New();
    }

    if (!m_mouth_upper_Path.isEmpty())
    {
        if (mouths.size() != 0) {
            renderer_main->RemoveActor(mouths[0]);
            renderer_coronal->RemoveActor(mouths[1]);
            renderer_sagittal->RemoveActor(mouths[2]);
            renderer_top->RemoveActor(mouths[3]);
        }
        mouths.clear();
        emit removeItemWhereRenderRemoveMember(tr("Upper Jaw Scan"));
        QFileInfo fileInfo(m_mouth_upper_Path);
        QString fileExtension = fileInfo.suffix().toLower();
        if (fileExtension == "stl") {
            vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
            reader->SetFileName(m_mouth_upper_Path.toUtf8().data());
            reader->Update();
            vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

            vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
            transformFilter1->SetTransform(m_transform2);
            transformFilter1->SetInputData(polyData);
            transformFilter1->Update();
            vtkSmartPointer<vtkPolyData> transPolyData = transformFilter1->GetOutput();

            for (int i = 0; i < 4; i++) {

                vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputData(transPolyData);
                vtkSmartPointer<vtkActor> mouthActor = vtkSmartPointer<vtkActor>::New();
                mouthActor->SetMapper(mapper);
                mouthActor->GetProperty()->SetColor(1, 1, 0);
                mouths.push_back(mouthActor);
            }


        }
        else {
            vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
            reader->SetFileName(m_mouth_upper_Path.toUtf8().data());
            reader->Update();
            vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

            vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
            transformFilter1->SetTransform(m_transform2);
            transformFilter1->SetInputData(polyData);
            transformFilter1->Update();
            vtkSmartPointer<vtkPolyData> transPolyData = transformFilter1->GetOutput();

            for (int i = 0; i < 4; i++) {
                vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputData(transPolyData);
                mapper->SetColorModeToDefault();
                mapper->ScalarVisibilityOn();
                vtkSmartPointer<vtkActor> mouthActor = vtkSmartPointer<vtkActor>::New();
                mouthActor->SetMapper(mapper);

                mouthActor->GetProperty()->SetAmbient(1.0);
                mouthActor->GetProperty()->SetDiffuse(0.0);
                mouthActor->GetProperty()->SetSpecular(0.0);
                mouthActor->GetProperty()->SetSpecularPower(30);
                // mouthActor->SetPickable(0);
                mouths.push_back(mouthActor);
                cout << "111111111" << endl;
            }

        }


        addItemWhereRenderAddMember(tr("Upper Jaw Scan"), mouths);

        renderer_main->AddActor(mouths[0]);
        renderer_coronal->AddActor(mouths[1]);
        renderer_sagittal->AddActor(mouths[2]);
        renderer_top->AddActor(mouths[3]);
        mouthActors.clear();
        mouthActors = mouths;
        renderer_main->ResetCamera();
        renderer_coronal->ResetCamera();
        renderer_sagittal->ResetCamera();
        renderer_top->ResetCamera();
        syncRefresh();
    }
    if (!m_mouth_lower_Path.isEmpty())
    {
        if (mouths_lower.size() != 0) {
            renderer_main->RemoveActor(mouths_lower[0]);
            //renderer_coronal->RemoveActor(mouths[1]);
            //renderer_sagittal->RemoveActor(mouths[2]);
            //renderer_top->RemoveActor(mouths[3]);
        }
        mouths_lower.clear();
        emit removeItemWhereRenderRemoveMember(tr("Lower Jaw Scan"));
        QFileInfo fileInfo(m_mouth_lower_Path);
        QString fileExtension = fileInfo.suffix().toLower();
        if (fileExtension == "stl") {
            vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
            reader->SetFileName(m_mouth_lower_Path.toUtf8().data());
            reader->Update();
            vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

            vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
            transformFilter1->SetTransform(m_transform2);
            transformFilter1->SetInputData(polyData);
            transformFilter1->Update();
            vtkSmartPointer<vtkPolyData> transPolyData = transformFilter1->GetOutput();

            for (int i = 0; i < 4; i++) {

                vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputData(transPolyData);
                vtkSmartPointer<vtkActor> mouthActor = vtkSmartPointer<vtkActor>::New();
                mouthActor->SetMapper(mapper);
                mouthActor->GetProperty()->SetColor(1, 1, 0);
                mouths_lower.push_back(mouthActor);
            }


        }
        else {
            vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
            reader->SetFileName(m_mouth_lower_Path.toUtf8().data());
            reader->Update();
            vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

            vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
            transformFilter1->SetTransform(m_transform2);
            transformFilter1->SetInputData(polyData);
            transformFilter1->Update();
            vtkSmartPointer<vtkPolyData> transPolyData = transformFilter1->GetOutput();

            for (int i = 0; i < 1; i++) {
                vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                mapper->SetInputData(transPolyData);
                mapper->SetColorModeToDefault();
                mapper->ScalarVisibilityOn();
                vtkSmartPointer<vtkActor> mouthActor = vtkSmartPointer<vtkActor>::New();
                mouthActor->SetMapper(mapper);

                mouthActor->GetProperty()->SetAmbient(1.0);
                mouthActor->GetProperty()->SetDiffuse(0.0);
                mouthActor->GetProperty()->SetSpecular(0.0);
                mouthActor->GetProperty()->SetSpecularPower(30);


                // mouthActor->SetPickable(0);
                mouths_lower.push_back(mouthActor);
                cout << "111111111" << endl;
            }

        }


        addItemWhereRenderAddMember(tr("Lower Jaw Scan"), mouths_lower);

        renderer_main->AddActor(mouths_lower[0]);
        //renderer_coronal->AddActor(mouths[1]);
        //renderer_sagittal->AddActor(mouths[2]);
        //renderer_top->AddActor(mouths[3]);
        mouthActors_lower.clear();
        mouthActors_lower = mouths_lower;
        renderer_main->ResetCamera();
        renderer_coronal->ResetCamera();
        renderer_sagittal->ResetCamera();
        renderer_top->ResetCamera();
        syncRefresh();
    }
    return true;
}

void SmileDesigenWidget::SplitPolyDataByIdentifier(vtkSmartPointer<vtkPolyData> inputPolyData,
                                                   std::map<int, vtkSmartPointer<vtkPolyData>>& outputPolyDataMap)
{
    // 获取输入数据的点、单元和标识符数组
    vtkSmartPointer<vtkPoints> inputPoints = inputPolyData->GetPoints();
    vtkSmartPointer<vtkCellArray> inputCells = inputPolyData->GetPolys();
    vtkSmartPointer<vtkIntArray> inputIdentifiers = vtkIntArray::SafeDownCast(inputPolyData->GetPointData()->GetArray("Identifier"));

    // 检查标识符数组是否存在
    if (!inputIdentifiers) {
        std::cerr << "Identifier array not found!" << std::endl;
        return;
    }

    // 获取输入的点和单元数量
    vtkIdType numPoints = inputPoints->GetNumberOfPoints();
    vtkIdType numCells = inputCells->GetNumberOfCells();
    std::cout << "merge points: " << numPoints << std::endl;
    std::cout << "merge cells: " << numCells << std::endl;

    // 获取所有不同的标识符
    std::set<int> uniqueIdentifiers;
    for (vtkIdType i = 0; i < inputIdentifiers->GetNumberOfTuples(); ++i) {
        uniqueIdentifiers.insert(inputIdentifiers->GetValue(i));
    }

    // 初始化每个标识符对应的 PolyData
    for (int identifier : uniqueIdentifiers) {
        if (outputPolyDataMap.find(identifier) == outputPolyDataMap.end()) {
            vtkSmartPointer<vtkPolyData> newPolyData = vtkSmartPointer<vtkPolyData>::New();
            newPolyData->SetPoints(vtkSmartPointer<vtkPoints>::New());
            newPolyData->SetPolys(vtkSmartPointer<vtkCellArray>::New());
            vtkSmartPointer<vtkIntArray> newIdentifiers = vtkSmartPointer<vtkIntArray>::New();
            newIdentifiers->SetName("Identifier");
            newIdentifiers->SetNumberOfComponents(1);
            newPolyData->GetPointData()->AddArray(newIdentifiers);
            outputPolyDataMap[identifier] = newPolyData;
        }
    }

    // 创建用于存储点和单元的映射
    std::map<int, vtkSmartPointer<vtkPoints>> pointsMap;
    std::map<int, vtkSmartPointer<vtkCellArray>> cellsMap;

    for (auto& pair : outputPolyDataMap) {
        pointsMap[pair.first] = pair.second->GetPoints();
        cellsMap[pair.first] = pair.second->GetPolys();
    }

    // 存储全局到局部点 ID 的映射
    std::map<vtkIdType, vtkIdType> newPointIds;

    // 遍历输入的每个单元并将其分配到正确的 PolyData
    vtkIdType npts;
    const vtkIdType* pts;

    inputCells->InitTraversal();
    while (inputCells->GetNextCell(npts, pts)) {
        // 获取当前单元的标识符
        int currentIdentifier = inputIdentifiers->GetValue(pts[0]);
        vtkSmartPointer<vtkPoints> currentPoints = pointsMap[currentIdentifier];
        vtkSmartPointer<vtkCellArray> currentCellArray = cellsMap[currentIdentifier];

        // 为单元中的每个点创建新的局部 ID（如果尚未创建）
        for (vtkIdType i = 0; i < npts; ++i) {
            if (newPointIds.find(pts[i]) == newPointIds.end()) {
                vtkIdType newPointId = currentPoints->InsertNextPoint(inputPoints->GetPoint(pts[i]));
                newPointIds[pts[i]] = newPointId;
            }
        }

        // 将单元添加到对应标识符的 CellArray 中
        currentCellArray->InsertNextCell(npts);
        for (vtkIdType i = 0; i < npts; ++i) {
            currentCellArray->InsertCellPoint(newPointIds[pts[i]]);
        }
    }

    // 标记所有输出 PolyData 为已修改
    for (auto& pair : outputPolyDataMap) {
        pair.second->Modified();
    }
}
void SmileDesigenWidget::onCheckBoxStateChanged(int state) {
    w3D_coronal->renderWindow()->GetInteractor()->SetInteractorStyle(handleStyle1);
    cout << 11112 << endl;
    cout<<"state"<<state<<endl;
    if (!assembly1 && tooth1.size() == 0) {
        cout << 222 << endl;
        if(state == Qt::Checked){
            checkBox->setCheckState(Qt::Unchecked);
            QMessageBox m_box(QMessageBox::Information, tr("Prompt Information"), tr("Please load the teeth model first!"));
            QTimer::singleShot(500, &m_box, SLOT(accept()));
            m_box.exec();
            return;
        }else{
            return;
        }

    }
    cout << 1234 << endl;

    if (state == Qt::Checked) {
        cout << "test" << endl;
        double separateColor[3];
        tooth1[0]->GetProperty()->GetColor(separateColor);
        double separateSpecular = tooth1[0]->GetProperty()->GetSpecular();
        double separateSpecularPower = tooth1[0]->GetProperty()->GetSpecularPower();
        double separateAmbient = tooth1[0]->GetProperty()->GetAmbient();
        double separateDiffuse = tooth1[0]->GetProperty()->GetDiffuse();

        for (int i = 0; i < tooth1.size(); i++) {

            renderer_main->RemoveActor(tooth1[i]);
            renderer_coronal->RemoveActor(tooth2[i]);
            renderer_sagittal->RemoveActor(tooth3[i]);
            renderer_top->RemoveActor(tooth4[i]);
        }
        appendFilter->RemoveAllInputs();

        // assembly1 = vtkSmartPointer<vtkActor>::New();
        // assembly2 = vtkSmartPointer<vtkActor>::New();
        // assembly3 = vtkSmartPointer<vtkActor>::New();
        // assembly4 = vtkSmartPointer<vtkActor>::New();
        if (!assembly1) {
            assembly1 = vtkSmartPointer<vtkActor>::New();
        }

        if (!assembly2) {
            assembly2 = vtkSmartPointer<vtkActor>::New();
        }

        if (!assembly3) {
            assembly3 = vtkSmartPointer<vtkActor>::New();
        }

        if (!assembly4) {
            assembly4 = vtkSmartPointer<vtkActor>::New();
        }

        for (int i = 0; i < tooth1.size(); i++) {
            vtkSmartPointer<vtkPolyData> polyData = vtkPolyDataMapper::SafeDownCast(tooth1[i]->GetMapper())->GetInput();
            vtkSmartPointer<vtkIntArray> Identifiers = vtkSmartPointer<vtkIntArray>::New();
            Identifiers->SetName("Identifier");
            Identifiers->SetNumberOfComponents(1);
            Identifiers->SetNumberOfTuples(polyData->GetNumberOfPoints());
            Identifiers->FillComponent(0, i);
            polyData->GetPointData()->AddArray(Identifiers);
            appendFilter->AddInputData(polyData);
        }

        appendFilter->Update();


        vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper1->SetInputData(appendFilter->GetOutput());
        vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper2->SetInputData(appendFilter->GetOutput());
        vtkSmartPointer<vtkPolyDataMapper> mapper3 = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper3->SetInputData(appendFilter->GetOutput());
        vtkSmartPointer<vtkPolyDataMapper> mapper4 = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper4->SetInputData(appendFilter->GetOutput());
        assembly1->SetMapper(mapper1);
        assembly2->SetMapper(mapper2);
        assembly3->SetMapper(mapper3);
        assembly4->SetMapper(mapper4);


        assembly1->GetProperty()->SetColor(separateColor);
        assembly1->GetProperty()->SetSpecular(separateSpecular);
        assembly1->GetProperty()->SetSpecularPower(separateSpecularPower);
        assembly1->GetProperty()->SetAmbient(separateAmbient);
        assembly1->GetProperty()->SetDiffuse(separateDiffuse);

        assembly2->GetProperty()->SetColor(separateColor);
        assembly2->GetProperty()->SetSpecular(separateSpecular);
        assembly2->GetProperty()->SetSpecularPower(separateSpecularPower);
        assembly2->GetProperty()->SetAmbient(separateAmbient);
        assembly2->GetProperty()->SetDiffuse(separateDiffuse);

        assembly3->GetProperty()->SetColor(separateColor);
        assembly3->GetProperty()->SetSpecular(separateSpecular);
        assembly3->GetProperty()->SetSpecularPower(separateSpecularPower);
        assembly3->GetProperty()->SetAmbient(separateAmbient);
        assembly3->GetProperty()->SetDiffuse(separateDiffuse);

        assembly4->GetProperty()->SetColor(separateColor);
        assembly4->GetProperty()->SetSpecular(separateSpecular);
        assembly4->GetProperty()->SetSpecularPower(separateSpecularPower);
        assembly4->GetProperty()->SetAmbient(separateAmbient);
        assembly4->GetProperty()->SetDiffuse(separateDiffuse);

        renderer_main->AddActor(assembly1);

        renderer_coronal->AddActor(assembly2);
        HandleInteractor* currentstyle1 = dynamic_cast<HandleInteractor*>(w3D_coronal->renderWindow()->GetInteractor()->GetInteractorStyle());
        currentstyle1->mergedActor = assembly2;

        renderer_sagittal->AddActor(assembly3);
        HandleInteractor* currentstyle2 = dynamic_cast<HandleInteractor*>(w3D_sagittal->renderWindow()->GetInteractor()->GetInteractorStyle());
        currentstyle2->mergedActor = assembly3;

        renderer_top->AddActor(assembly4);
        HandleInteractor* currentstyle3 = dynamic_cast<HandleInteractor*>(w3D_top->renderWindow()->GetInteractor()->GetInteractorStyle());
        currentstyle3->mergedActor = assembly4;

    }
    else {
        cout << 5678 << endl;
        if (floatingDialog->isVisible()) {
            cout<<"floatingDialog->isVisible"<<endl;
            stackedWidget->setCurrentIndex(0);
        }
        cout << "qqqqqq" << endl;
        double assemblyColor[3];
        assembly1->GetProperty()->GetColor(assemblyColor);
        double assemblySpecular = assembly1->GetProperty()->GetSpecular();
        double assemblySpecularPower = assembly1->GetProperty()->GetSpecularPower();
        double assemblyAmbient = assembly1->GetProperty()->GetAmbient();
        double assemblyDiffuse = assembly1->GetProperty()->GetDiffuse();
        cout << "wwwwww" << endl;
        renderer_main->RemoveActor(assembly1);
        renderer_coronal->RemoveActor(assembly2);
        renderer_sagittal->RemoveActor(assembly3);
        renderer_top->RemoveActor(assembly4);
        vtkSmartPointer<vtkPolyData> mergedPolyData = appendFilter->GetOutput();
        std::map<int, vtkSmartPointer<vtkPolyData>> splitPolyDataMap;
        SplitPolyDataByIdentifier(mergedPolyData, splitPolyDataMap);
        cout << "eeeeee" << endl;
        for (int i = 0; i < tooth1.size(); i++) {

            // Mapper 1
            vtkSmartPointer<vtkPolyDataMapper> mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper1->SetInputData(splitPolyDataMap[i]);// 使用平滑后的数据
            //tooth1[i] = vtkSmartPointer<vtkActor>::New();
            tooth1[i]->SetMapper(mapper1);
            tooth1[i]->GetProperty()->SetColor(assemblyColor); // 设置颜色
            // 设置反光和光泽度属性
            tooth1[i]->GetProperty()->SetSpecular(assemblySpecular);         // 设置高光反射强度
            tooth1[i]->GetProperty()->SetSpecularPower(assemblySpecularPower);   // 设置高光反射的集中度
            tooth1[i]->GetProperty()->SetAmbient(assemblyAmbient);          // 设置环境光比例
            tooth1[i]->GetProperty()->SetDiffuse(assemblyDiffuse);          // 设置漫反射比例

            // Mapper 2
            vtkSmartPointer<vtkPolyDataMapper> mapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper2->SetInputData(splitPolyDataMap[i]);
            //tooth2[i] = vtkSmartPointer<vtkActor>::New();
            tooth2[i]->SetMapper(mapper2);
            tooth2[i]->GetProperty()->SetColor(assemblyColor); // 设置颜色
            // 设置反光和光泽度属性
            tooth2[i]->GetProperty()->SetSpecular(assemblySpecular);         // 设置高光反射强度
            tooth2[i]->GetProperty()->SetSpecularPower(assemblySpecularPower);   // 设置高光反射的集中度
            tooth2[i]->GetProperty()->SetAmbient(assemblyAmbient);          // 设置环境光比例
            tooth2[i]->GetProperty()->SetDiffuse(assemblyDiffuse);          // 设置漫反射比例


            // Mapper 3
            vtkSmartPointer<vtkPolyDataMapper> mapper3 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper3->SetInputData(splitPolyDataMap[i]);// 使用平滑后的数据
            //tooth3[i] = vtkSmartPointer<vtkActor>::New();
            tooth3[i]->SetMapper(mapper3);
            tooth3[i]->GetProperty()->SetColor(assemblyColor); // 设置颜色

            tooth3[i]->GetProperty()->SetSpecular(assemblySpecular);         // 设置高光反射强度
            tooth3[i]->GetProperty()->SetSpecularPower(assemblySpecularPower);   // 设置高光反射的集中度
            tooth3[i]->GetProperty()->SetAmbient(assemblyAmbient);          // 设置环境光比例
            tooth3[i]->GetProperty()->SetDiffuse(assemblyDiffuse);          // 设置漫反射比例
            cout << "check::" << tooth3[i] << endl;
            // Mapper 4
            vtkSmartPointer<vtkPolyDataMapper> mapper4 = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper4->SetInputData(splitPolyDataMap[i]);// 使用平滑后的数据
            //tooth4[i] = vtkSmartPointer<vtkActor>::New();
            tooth4[i]->SetMapper(mapper4);
            tooth4[i]->GetProperty()->SetColor(assemblyColor); // 设置颜色
            // 设置反光和光泽度属性
            tooth4[i]->GetProperty()->SetSpecular(assemblySpecular);         // 设置高光反射强度
            tooth4[i]->GetProperty()->SetSpecularPower(assemblySpecularPower);   // 设置高光反射的集中度
            tooth4[i]->GetProperty()->SetAmbient(assemblyAmbient);          // 设置环境光比例
            tooth4[i]->GetProperty()->SetDiffuse(assemblyDiffuse);          // 设置漫反射比例
            // 将每个 Actor 添加到相应的渲染器中
            renderer_main->AddActor(tooth1[i]);
            renderer_coronal->AddActor(tooth2[i]);
            renderer_sagittal->AddActor(tooth3[i]);
            renderer_top->AddActor(tooth4[i]);

        }


    }


    syncRefresh();
}


void SmileDesigenWidget::syncRefresh() {
    // w3D_main->renderWindow()->GetInteractor()->Render();
    // w3D_coronal->renderWindow()->GetInteractor()->Render();
    // w3D_sagittal->renderWindow()->GetInteractor()->Render();
    // w3D_top->renderWindow()->GetInteractor()->Render();
    //cout<<"sync"<<endl;
    w3D_main->renderWindow()->Render();
    w3D_coronal->renderWindow()->Render();
    w3D_sagittal->renderWindow()->Render();
    w3D_top->renderWindow()->Render();

    //vtk8.2
    // w3D_main->GetRenderWindow()->GetInteractor()->Render();
    // w3D_coronal->GetRenderWindow()->GetInteractor()->Render();
    // w3D_sagittal->GetRenderWindow()->GetInteractor()->Render();
    // w3D_top->GetRenderWindow()->GetInteractor()->Render();
}

void SmileDesigenWidget::flipView() {
    std::cout << "dddddd" << std::endl;
    if (sagittalView == 0) {
        vtkCamera* camera_sagittal = renderer_sagittal->GetActiveCamera();
        camera_sagittal->SetViewUp(0, 1, 0);
        camera_sagittal->SetPosition(250, 0, 25);
        camera_sagittal->SetFocalPoint(0, 0, 25);
        camera_sagittal->SetParallelScale(25);
        camera_sagittal->ParallelProjectionOn();
        sagittalView = 1;
    }
    else {
        vtkCamera* camera_sagittal = renderer_sagittal->GetActiveCamera();
        camera_sagittal->SetViewUp(0, 1, 0);
        camera_sagittal->SetPosition(-250, 0, 25);
        camera_sagittal->SetFocalPoint(0, 0, 25);
        camera_sagittal->SetParallelScale(25);
        camera_sagittal->ParallelProjectionOn();
        sagittalView = 0;
    }
    renderer_sagittal->ResetCamera();
    w3D_sagittal->renderWindow()->Render();
}
void SmileDesigenWidget::onCheckBox2Clicked() {
    if (faceActor == nullptr) {
        //checkBox2->setCheckState(Qt::Unchecked);
        QMessageBox m_box(QMessageBox::Information, tr("Prompt Information"), tr("Please load the face scan model first!"));
        QTimer::singleShot(1000, &m_box, SLOT(accept()));
        m_box.exec();
        return;
    }

    if(currentState == State::Rectify){
        //PolygonSelectInteractorStyle *style  = new PolygonSelectInteractorStyle();
        selectStyle->SetPolyData(vtkPolyDataMapper::SafeDownCast(faceActor->GetMapper())->GetInput());
        selectStyle->SetRenderer(renderer_main);
        renderer_main->GetRenderWindow()->GetInteractor()->SetInteractorStyle(selectStyle);
    }else{

        QMessageBox m_box(QMessageBox::Information, tr("Prompt Information"), tr("Please perform head alignment first!"));
        QTimer::singleShot(500, &m_box, SLOT(accept()));
        m_box.exec();
    }


}

void SmileDesigenWidget::autoCut() {
    cout << "smile::autocut used" << endl;


    if (!faceActor) {
        QMessageBox m_box(QMessageBox::Information, tr("Prompt Information"), tr("Please load the face scan model first!"));
        QTimer::singleShot(1000, &m_box, SLOT(accept()));
        m_box.exec();
        return;
    }
    // 加载图像
    if(currentState == State::Rectify){
        // Capture the window as an image
        double campos[3] = { 0,0,300 };
        double focalpoint[3] = { 0,0,0 };
        double viewup[3] = { 0,1,0 };
        //m_mesh_view->setCameraProperties(campos,focalpoint,viewup,true,60);

        vtkCamera* camera = renderer_main->GetActiveCamera();

        camera->SetPosition(campos);

        camera->SetFocalPoint(focalpoint);

        camera->SetViewUp(viewup);

        camera->SetViewAngle(60);

        camera->ParallelProjectionOn();
        renderer_main->ResetCamera();
        renderer_main->GetRenderWindow()->Render();
        vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
        windowToImageFilter->SetInput(renderer_main->GetRenderWindow());
        windowToImageFilter->Update();

        // Write the image to a PNG file
        vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
        writer->SetFileName("screenshot.png");
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();
        std::ifstream file("screenshot.png", std::ios::binary);
        std::vector<uchar> buffer(std::istreambuf_iterator<char>(file), {});

        // 使用 OpenCV 将字节数组解码为 Mat 对象
        cv::Mat img = cv::imdecode(buffer, cv::IMREAD_COLOR);

        if (img.empty())
        {
            std::cerr << "Unable to load image!" << std::endl;
            return;
        }
        else {
            qDebug() << "Shape image loaded successfully." << endl;
        }

        dlib::cv_image<dlib::bgr_pixel> cimg(img);

        // 检测人脸
        std::vector<dlib::rectangle> faces = detector(cimg);

        double img_height = img.rows; // Get the height of the image
        // 对每张检测到的人脸提取68个关键点
        vtkSmartPointer<vtkPoints> ScreenPoints = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPoints> WorldPoints = vtkSmartPointer<vtkPoints>::New();
        for (auto& face : faces)
        {
            dlib::full_object_detection shape = predictor(cimg, face);
            std::cout << "Number of parts: " << shape.num_parts() << std::endl;


            // 在图像上绘制关键点并打印点位信息
            for (int i = 60; i < 68; ++i)
            {
                cv::Point point(shape.part(i).x(), shape.part(i).y());
                cv::circle(img, point, 2, cv::Scalar(0, 255, 0), -1);

                // Adjust y-coordinate for VTK
                double vtk_y = img_height - point.y;

                // Print adjusted coordinates
                std::cout << "Point " << i + 1 << ": (" << point.x << ", " << vtk_y << ")" << std::endl;

                ScreenPoints->InsertNextPoint(point.x, vtk_y, 0.0);
            }


        }



        vtkSmartPointer<vtkPolyData> facePolyData = vtkSmartPointer<vtkPolyData>::New();
        facePolyData = vtkPolyDataMapper::SafeDownCast(faceActor->GetMapper())->GetInput();

        vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
        double point[3];
        double worldCoords[4];
        picker->SetTolerance(0.005);
        for (vtkIdType i = 0; i < ScreenPoints->GetNumberOfPoints(); ++i)
        {

            ScreenPoints->GetPoint(i, point);

            renderer_main->SetDisplayPoint(point[0], point[1], 0.0);
            renderer_main->DisplayToWorld();
            renderer_main->GetWorldPoint(worldCoords);
            if (worldCoords[3] != 0.0)
            {
                worldCoords[0] /= worldCoords[3];
                worldCoords[1] /= worldCoords[3];
                worldCoords[2] /= worldCoords[3];
                worldCoords[3] = 1.0;
            }
            WorldPoints->InsertNextPoint(worldCoords);
            //WorldPoints->InsertNextPoint(pickedPosition[0], pickedPosition[1], pickedPosition[2]);
        }

        // double first[3];
        // WorldPoints->GetPoint(0,first);
        // WorldPoints->InsertNextPoint(first[0],first[1],first[2]-2);

        vtkNew<vtkParametricSpline> spline;
        spline->SetPoints(WorldPoints);
        spline->ClosedOn();
        cout << "Number of input points: " << WorldPoints->GetNumberOfPoints() << endl;

        vtkNew<vtkParametricFunctionSource> functionSource;
        functionSource->SetParametricFunction(spline);
        functionSource->SetUResolution(10 * (WorldPoints->GetNumberOfPoints()));
        functionSource->Update();

        vtkPolyData* out = functionSource->GetOutput();
        std::cout << "Number of points in the output: " << out->GetNumberOfPoints() << std::endl;

        // 获取插值曲线上的点
        vtkPoints* curvePoints = out->GetPoints();
        vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();

        for (vtkIdType i = 0; i < out->GetNumberOfPoints()-3; i++) {
            double point[3];
            curvePoints->GetPoint(i, point);  // 获取每个点的坐标
            newPoints->InsertNextPoint(point);
            std::cout << "Point " << i << ": (" << point[0] << ", " << point[1] << ", " << point[2] << ")" << std::endl;
        }


        facePolyData = vtkPolyDataMapper::SafeDownCast(faceActor->GetMapper())->GetInput();


        vtkSmartPointer<vtkImplicitSelectionLoop> loop = vtkSmartPointer<vtkImplicitSelectionLoop>::New();




        loop->SetLoop(newPoints);
        loop->SetAutomaticNormalGeneration(true);

        // 获取摄像机的投影方向，作为法向量
        double* normal = renderer_main->GetActiveCamera()->GetDirectionOfProjection();
        loop->SetNormal(normal);

        vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
        clipper->SetInputData(facePolyData);  // 使用原始 PolyData
        clipper->SetClipFunction(loop);       // 使用 vtkImplicitSelectionLoop 来裁剪
        clipper->Update();


        // 获取裁剪后的 PolyData
        vtkSmartPointer<vtkPolyData> clippedPolyData = clipper->GetOutput();


        // 更新原有 PolyData
        facePolyData->DeepCopy(clippedPolyData);

        // 将裁剪后的 actor 添加到渲染器中

        renderer_main->GetRenderWindow()->Render();


        //保存带关键点的图像
        if (cv::imwrite("output_with_landmarks.bmp", img)) {
            std::cout << "Image saved successfully to output_with_landmarks.bmp" << std::endl;
        }
        else {
            std::cerr << "Failed to save the image!" << std::endl;
        }
    }else{
        QMessageBox m_box(QMessageBox::Information, tr("Prompt Information"), tr("Please perform head alignment first!"));
        QTimer::singleShot(500, &m_box, SLOT(accept()));
        m_box.exec();
    }

}
void SmileDesigenWidget::drawLines() {
    // 加载图像
    // Capture the window as an image
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderer_main->GetRenderWindow());
    windowToImageFilter->Update();

    // Write the image to a PNG file
    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName("screenshot.png");
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();
    std::ifstream file("screenshot.png", std::ios::binary);
    std::vector<uchar> buffer(std::istreambuf_iterator<char>(file), {});

    // 使用 OpenCV 将字节数组解码为 Mat 对象
    cv::Mat img = cv::imdecode(buffer, cv::IMREAD_COLOR);

    if (img.empty())
    {
        std::cerr << "Unable to load image!" << std::endl;
        return;
    }
    else {
        qDebug() << "Shape image loaded successfully." << endl;
    }

    dlib::cv_image<dlib::bgr_pixel> cimg(img);

    // 检测人脸
    std::vector<dlib::rectangle> faces = detector(cimg);

    int img_height = img.rows; // Get the height of the image
    // 对每张检测到的人脸提取68个关键点
    vtkSmartPointer<vtkPoints> ScreenPoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPoints> WorldPoints = vtkSmartPointer<vtkPoints>::New();
    dlib::rectangle face = faces[0];

    dlib::full_object_detection shape = predictor(cimg, face);

    double p1[2] = { static_cast<double>(shape.part(27).x()), static_cast<double>(img_height - shape.part(27).y()) };
    double p2[2] = { static_cast<double>(shape.part(33).x()), static_cast<double>(img_height - shape.part(33).y()) };
    //addLine(p1,p2);

    // 更新 p1 和 p2 的值
    p1[0] = static_cast<double>(shape.part(61).x());
    p1[1] = static_cast<double>(img_height - shape.part(61).y());

    p2[0] = static_cast<double>(shape.part(67).x());
    p2[1] = static_cast<double>(img_height - shape.part(67).y());



    addLine(p1, p2);

    p1[0] = static_cast<double>(shape.part(62).x());
    p1[1] = static_cast<double>(img_height - shape.part(62).y());

    p2[0] = static_cast<double>(shape.part(66).x());
    p2[1] = static_cast<double>(img_height - shape.part(66).y());

    addLine(p1, p2);

    p1[0] = static_cast<double>(shape.part(63).x());
    p1[1] = static_cast<double>(img_height - shape.part(63).y());

    p2[0] = static_cast<double>(shape.part(65).x());
    p2[1] = static_cast<double>(img_height - shape.part(65).y());

    addLine(p1, p2);

    renderer_main->GetRenderWindow()->Render();
}

void SmileDesigenWidget::removeLines() {
    cout<<"removeLines"<<endl;
    for (auto& actor : lineList) {
        renderer_main->RemoveActor(actor);
        actor = nullptr; // 释放每个 vtkSmartPointer 持有的 vtkActor2D 对象
    }
    lineList.clear();
    renderer_main->GetRenderWindow()->Render();
}
void SmileDesigenWidget::addLine(double* p1, double* p2) {
    vtkNew<vtkPoints> points;
    points->InsertNextPoint(p1[0], p1[1], 0.0);
    points->InsertNextPoint(p2[0], p2[1], 0.0);

    // Create a vtkCellArray to store the line (as a single polyline)
    vtkNew<vtkCellArray> lines;
    vtkNew<vtkPolyLine> polyLine;
    polyLine->GetPointIds()->SetNumberOfIds(2);  // We have 2 points in the line
    polyLine->GetPointIds()->SetId(0, 0);  // The first point
    polyLine->GetPointIds()->SetId(1, 1);  // The second point
    lines->InsertNextCell(polyLine);

    // Create a vtkPolyData to store the points and lines
    vtkNew<vtkPolyData> linePolyData;
    linePolyData->SetPoints(points);
    linePolyData->SetLines(lines);

    // Create a vtkPolyDataMapper2D to map the line data to graphics primitives
    vtkNew<vtkPolyDataMapper2D> mapper;
    mapper->SetInputData(linePolyData);

    // Create a vtkActor2D to represent the line in the scene
    vtkNew<vtkActor2D> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0.0, 1.0, 0.0);  // Set the line color to green
    actor->GetProperty()->SetLineWidth(2.0);  // Optional: set line width
    actor->GetProperty()->SetLineStipplePattern(0xF0F0); // 设置虚线模式
    actor->GetProperty()->SetLineStippleRepeatFactor(1); // 设置重复因子
    // Add the line actor to the renderer
    renderer_main->AddActor2D(actor);
    lineList.push_back(actor);
}

void SmileDesigenWidget::setDataPath(QString facePath, QString mouthUpperPath,QString mouthLowerPath) {

    m_facePath = facePath;
    m_mouth_upper_Path = mouthUpperPath;
    m_mouth_lower_Path = mouthLowerPath;
    QFileInfo fileInfo(m_facePath);
    m_transPath = fileInfo.absolutePath() + "\\" + "transform.txt";
}
void SmileDesigenWidget::onPointSelected(int pointNum, double x, double y, double z)
{


    cout << "onPointSelected" << endl;
    currentStep = pointNum;
    if (currentStep == 0) {
        // statusLabel->setText("Select the left eye...");
        // m_mesh_view->AddPoint(x, y, z, Qt::yellow);
        qDebug() << "Select the left eye...";
        rightEyePoint[0] = x;
        rightEyePoint[1] = y;
        rightEyePoint[2] = z;
        cout << "pickStyle->SelectedActors.size():" << pickStyle->SelectedActors.size() << endl;

        QPixmap pixmap("./resources/pic/righteye.png"); // 替换为你的图片路径

        tip->updateTip(tr("Click on the right eye"), pixmap);
        tip->show();
        //smileStyle->setStraightPoints(rightEyePoint);
    }
    else if (currentStep == 1) {
        // statusLabel->setText("Select the nose tip...");
        // m_mesh_view->AddPoint(x, y, z, Qt::yellow);
        qDebug() << "Select the Right eye...";
        leftEyePoint[0] = x;
        leftEyePoint[1] = y;
        leftEyePoint[2] = z;
        cout << "pickStyle->SelectedActors.size():" << pickStyle->SelectedActors.size() << endl;

        QPixmap pixmap("./resources/pic/noseup.png"); // 替换为你的图片路径
        tip->updateTip(tr("Click on the upper nose"), pixmap);
        tip->show();
        //smileStyle->setStraightPoints(leftEyePoint);
    }
    else if (currentStep == 2) {
        // statusLabel->setText("Selection complete!");
        // m_mesh_view->AddPoint(x, y, z, Qt::blue);
        qDebug() << "Select the nose on...";
        noseOnPoint[0] = x;
        noseOnPoint[1] = y;
        noseOnPoint[2] = z;
        cout << "pickStyle->SelectedActors.size():" << pickStyle->SelectedActors.size() << endl;

        QPixmap pixmap("./resources/pic/noseunder.png"); // 替换为你的图片路径
        tip->updateTip(tr("Click on the lower nose"), pixmap);
        tip->show();
    }
    else if (currentStep == 3) {

        qDebug() << "Select the nose under ...";
        tip->hide();
        noseUnderPoint[0] = x;
        noseUnderPoint[1] = y;
        noseUnderPoint[2] = z;

        //calculate
        // 计算 X 方向（归一化）
        double xDirection[3];
        vtkMath::Subtract(leftEyePoint, rightEyePoint, xDirection);
        vtkMath::Normalize(xDirection);

        // 计算 Z 方向（归一化）
        double tempVec[3], zDirection[3];
        vtkMath::Subtract(noseOnPoint, noseUnderPoint, tempVec);
        vtkMath::Cross(xDirection, tempVec, zDirection);
        vtkMath::Normalize(zDirection);

        // 计算 Y 方向（归一化）
        double yDirection[3];
        vtkMath::Cross(zDirection, xDirection, yDirection);

        // 构建旋转矩阵
        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        matrix->Identity();
        for (int i = 0; i < 3; ++i) {
            matrix->SetElement(i, 0, xDirection[i]);
            matrix->SetElement(i, 1, yDirection[i]);
            matrix->SetElement(i, 2, zDirection[i]);
            matrix->SetElement(i, 3, noseUnderPoint[i]);
        }
        vtkSmartPointer<vtkMatrix4x4> inverseMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        vtkMatrix4x4::Invert(matrix, inverseMatrix);


        // 创建 transform
        m_transform = vtkSmartPointer<vtkTransform>::New();
        m_transform->SetMatrix(inverseMatrix);

        calibrate_transform = m_transform;


        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter->SetTransform(m_transform);
        transformFilter->SetInputData(faceActor->GetMapper()->GetInput());
        transformFilter->Update();

        // 获取变换后的数据
        vtkSmartPointer<vtkPolyData> transformedPolyData = transformFilter->GetOutput();

        //faceActor->SetUserTransform(m_transform);

        vtkPolyDataMapper* polyDataMapper = dynamic_cast<vtkPolyDataMapper*>(faceActor->GetMapper());
        if (polyDataMapper) {
            polyDataMapper->SetInputData(transformedPolyData);
        }
        else {
            std::cerr << "The mapper is not a vtkPolyDataMapper!" << std::endl;
        }

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter1->SetTransform(m_transform);

        for (int i = 0; i < 4; i++) {
            // 获取当前 Actor 的 Mapper 并检查其类型
            vtkSmartPointer<vtkPolyDataMapper> polyDataMapper = vtkPolyDataMapper::SafeDownCast(mouthActors[i]->GetMapper());

            // 获取当前 Actor 的输入数据并检查其有效性
            vtkSmartPointer<vtkPolyData> polyData = polyDataMapper->GetInput();
            // 设置 TransformPolyDataFilter 的输入数据
            transformFilter1->SetInputData(polyData);
            transformFilter1->Update();

            // 将变换后的数据设置回 Mapper
            vtkSmartPointer<vtkPolyData> transformedPolyData = transformFilter1->GetOutput();
            polyDataMapper->SetInputData(transformedPolyData);
        }

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter2->SetTransform(m_transform);
        vtkSmartPointer<vtkPolyDataMapper> polyDataMapper2 = vtkPolyDataMapper::SafeDownCast(mouthActors_lower[0]->GetMapper());

        // 获取当前 Actor 的输入数据并检查其有效性
        vtkSmartPointer<vtkPolyData> polyData2= polyDataMapper2->GetInput();
        // 设置 TransformPolyDataFilter 的输入数据
        transformFilter2->SetInputData(polyData2);
        transformFilter2->Update();

        // 将变换后的数据设置回 Mapper
        vtkSmartPointer<vtkPolyData> transformedPolyData2 = transformFilter2->GetOutput();
        polyDataMapper2->SetInputData(transformedPolyData2);

        cout << "pickStyle->SelectedActors.size():" << pickStyle->SelectedActors.size() << endl;

        teethlabel->show();
        teethComboBox->show();
        //reset view
        double campos[3] = { 0,0,300 };
        double focalpoint[3] = { 0,0,0 };
        double viewup[3] = { 0,1,0 };
        //m_mesh_view->setCameraProperties(campos,focalpoint,viewup,true,60);

        vtkCamera* camera = renderer_main->GetActiveCamera();

        camera->SetPosition(campos);

        camera->SetFocalPoint(focalpoint);

        camera->SetViewUp(viewup);

        camera->SetViewAngle(60);

        camera->ParallelProjectionOn();
        renderer_main->ResetCamera();

        renderer_coronal->ResetCamera();
        renderer_sagittal->ResetCamera();
        renderer_top->ResetCamera();
        syncRefresh();
        renderer_main->GetRenderWindow()->Render();



        int pos_int[2];
        double* displayPosition;
        // 创建 transform
        m_transform = vtkSmartPointer<vtkTransform>::New();
        m_transform->SetMatrix(inverseMatrix);

        // 获取变换后的 rightEyePoint
        double transformedRightEyePoint[3];
        m_transform->TransformPoint(rightEyePoint, transformedRightEyePoint);

        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();
        coordinate->SetValue(transformedRightEyePoint);
        displayPosition = coordinate->GetComputedDoubleDisplayValue(renderer_main);  // 屏幕坐标

        pos_int[0] = displayPosition[0]-200;
        pos_int[1] = displayPosition[1];

        smileStyle->setStraightPoints(pos_int);
        // 获取变换后的 leftEyePoint
        double transformedLeftEyePoint[3];
        m_transform->TransformPoint(leftEyePoint, transformedLeftEyePoint);

        coordinate->SetCoordinateSystemToWorld();
        coordinate->SetValue(transformedLeftEyePoint);
        displayPosition = coordinate->GetComputedDoubleDisplayValue(renderer_main);  // 屏幕坐标

        pos_int[0] = displayPosition[0]+200;
        //pos_int[1] = displayPosition[1];

        smileStyle->setStraightPoints(pos_int);

        // 获取变换后的 noseOnPoint
        double transformedNoseOnPoint[3];
        m_transform->TransformPoint(noseOnPoint, transformedNoseOnPoint);
        coordinate->SetCoordinateSystemToWorld();
        coordinate->SetValue(transformedNoseOnPoint);
        displayPosition = coordinate->GetComputedDoubleDisplayValue(renderer_main);  // 屏幕坐标

        pos_int[0] = displayPosition[0];
        pos_int[1] = displayPosition[1]+300;

        smileStyle->setStraightPoints(pos_int);



        pos_int[0] = displayPosition[0];
        pos_int[1] -=800;

        smileStyle->setStraightPoints(pos_int);
        //gensilhouette();
        syncRefresh();



    }
    else {
        cout << "ooooooooooooo" << endl;
    }

}

void SmileDesigenWidget::mouthFirstAdjust() {
    cout << "first" << endl;
    glm::mat4 glmMatrix = matchWindow->getTransform();

    // VTK 的矩阵是列主序的数组，因此需要按照列优先顺序设置

    vtkSmartPointer<vtkMatrix4x4> vtkMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            vtkMatrix->SetElement(i, j, glmMatrix[i][j]);
        }
    }
    m_transform2 = vtkSmartPointer<vtkTransform>::New();
    m_transform2->SetMatrix(vtkMatrix);

    QFile file(m_transPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                out << vtkMatrix->GetElement(i, j);
                if (j < 3) {
                    out << " ";  // 在元素之间插入空格
                }
            }
            out << "\n";  // 换行
        }
        file.close();
    }
    else {
        qWarning() << "Failed to open file for writing" << endl;
    }

    // 创建一个 TransformPolyDataFilter 来应用变换到 polyData

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetTransform(m_transform2);


    emit removeItemWhereRenderRemoveMember(tr("Upper Jaw Scan"));
    QFileInfo fileInfo(m_mouth_upper_Path);
    QString fileExtension = fileInfo.suffix().toLower();
    renderer_main->RemoveActor(mouths[0]);
    renderer_coronal->RemoveActor(mouths[1]);
    renderer_sagittal->RemoveActor(mouths[2]);
    renderer_top->RemoveActor(mouths[3]);
    mouths.clear();

    if (fileExtension == "stl") {
        vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
        reader->SetFileName(m_mouth_upper_Path.toUtf8().data());
        reader->Update();
        vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter1->SetTransform(m_transform2);
        transformFilter1->SetInputData(polyData);
        transformFilter1->Update();
        vtkSmartPointer<vtkPolyData> transPolyData = transformFilter1->GetOutput();

        for (int i = 0; i < 4; i++) {

            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(transPolyData);
            vtkSmartPointer<vtkActor> mouthActor = vtkSmartPointer<vtkActor>::New();
            mouthActor->SetMapper(mapper);
            mouthActor->GetProperty()->SetColor(1, 1, 0);

            mouths.push_back(mouthActor);
        }
    }
    else {
        vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
        reader->SetFileName(m_mouth_upper_Path.toUtf8().data());
        reader->Update();
        vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter1->SetTransform(m_transform2);
        transformFilter1->SetInputData(polyData);
        transformFilter1->Update();
        vtkSmartPointer<vtkPolyData> transPolyData = transformFilter1->GetOutput();

        for (int i = 0; i < 4; i++) {
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(transPolyData);
            mapper->SetColorModeToDefault();
            mapper->ScalarVisibilityOn();
            vtkSmartPointer<vtkActor> mouthActor = vtkSmartPointer<vtkActor>::New();
            mouthActor->SetMapper(mapper);

            mouthActor->GetProperty()->SetAmbient(1.0);
            mouthActor->GetProperty()->SetDiffuse(0.0);
            mouthActor->GetProperty()->SetSpecular(0.0);
            mouthActor->GetProperty()->SetSpecularPower(30);
            mouths.push_back(mouthActor);
        }
    }

    addItemWhereRenderAddMember(tr("Upper Jaw Scan"), mouths);

    renderer_main->AddActor(mouths[0]);
    renderer_coronal->AddActor(mouths[1]);
    renderer_sagittal->AddActor(mouths[2]);
    renderer_top->AddActor(mouths[3]);
    mouthActors.clear();
    mouthActors = mouths;
    renderer_main->ResetCamera();
    renderer_coronal->ResetCamera();
    renderer_sagittal->ResetCamera();
    renderer_top->ResetCamera();
    syncRefresh();

    emit removeItemWhereRenderRemoveMember(tr("Lower Jaw Scan"));
    QFileInfo fileInfo1(m_mouth_lower_Path);
    QString fileExtension1 = fileInfo1.suffix().toLower();
    renderer_main->RemoveActor(mouths_lower[0]);
    // renderer_coronal->RemoveActor(mouths[1]);
    // renderer_sagittal->RemoveActor(mouths[2]);
    // renderer_top->RemoveActor(mouths[3]);
    mouths_lower.clear();

    if (fileExtension == "stl") {
        vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
        reader->SetFileName(m_mouth_lower_Path.toUtf8().data());
        reader->Update();
        vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter1->SetTransform(m_transform2);
        transformFilter1->SetInputData(polyData);
        transformFilter1->Update();
        vtkSmartPointer<vtkPolyData> transPolyData = transformFilter1->GetOutput();

        for (int i = 0; i < 4; i++) {

            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(transPolyData);
            vtkSmartPointer<vtkActor> mouthActor = vtkSmartPointer<vtkActor>::New();
            mouthActor->SetMapper(mapper);
            mouthActor->GetProperty()->SetColor(1, 1, 0);

            mouths_lower.push_back(mouthActor);
        }
    }
    else {
        vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
        reader->SetFileName(m_mouth_lower_Path.toUtf8().data());
        reader->Update();
        vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter1->SetTransform(m_transform2);
        transformFilter1->SetInputData(polyData);
        transformFilter1->Update();
        vtkSmartPointer<vtkPolyData> transPolyData = transformFilter1->GetOutput();

        for (int i = 0; i < 4; i++) {
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputData(transPolyData);
            mapper->SetColorModeToDefault();
            mapper->ScalarVisibilityOn();
            vtkSmartPointer<vtkActor> mouthActor = vtkSmartPointer<vtkActor>::New();
            mouthActor->SetMapper(mapper);

            mouthActor->GetProperty()->SetAmbient(1.0);
            mouthActor->GetProperty()->SetDiffuse(0.0);
            mouthActor->GetProperty()->SetSpecular(0.0);
            mouthActor->GetProperty()->SetSpecularPower(30);
            mouths_lower.push_back(mouthActor);
        }
    }

    addItemWhereRenderAddMember(tr("Lower Jaw Scan"), mouths_lower);

    renderer_main->AddActor(mouths_lower[0]);
    // renderer_coronal->AddActor(mouths[1]);
    // renderer_sagittal->AddActor(mouths[2]);
    // renderer_top->AddActor(mouths[3]);
    mouthActors_lower.clear();
    mouthActors_lower = mouths_lower;
    renderer_main->ResetCamera();
    renderer_coronal->ResetCamera();
    renderer_sagittal->ResetCamera();
    renderer_top->ResetCamera();
    syncRefresh();

}
void SmileDesigenWidget::importAll() {

    calibrate_transform = vtkSmartPointer<vtkTransform>::New();
    currentState = State::Import;
    if (floatingDialog->isVisible()) {
        stackedWidget->setCurrentIndex(0);
    }
    initStyle();
    firstimport = true;
    smileStyle->removePoint();
    smileStyle->isHasLine = false;
    buttonclearLine->setText(tr("Add Auxiliary Line"));

    polylinelabel->hide();
    polylinecomboBox->hide();
    teethComboBox->setCurrentIndex(0);

    renderer_main->RemoveAllViewProps();
    renderer_coronal->RemoveAllViewProps();
    renderer_sagittal->RemoveAllViewProps();
    renderer_top->RemoveAllViewProps();
    if (assembly1 != nullptr) {
        renderer_main->RemoveActor(assembly1);
        renderer_coronal->RemoveActor(assembly2);
        renderer_sagittal->RemoveActor(assembly3);
        renderer_top->RemoveActor(assembly4);
        assembly1 = nullptr;
        assembly2 = nullptr;
        assembly3 = nullptr;
        assembly4 = nullptr;
    }

    if (tooth1.size() != 0) {
        for (int i = 0; i < tooth1.size(); i++) {
            renderer_main->RemoveActor(tooth1[i]);
            renderer_coronal->RemoveActor(tooth2[i]);
            renderer_sagittal->RemoveActor(tooth3[i]);
            renderer_top->RemoveActor(tooth4[i]);
        }
    }

    assembleactor.clear();
    tooth1.clear();
    tooth2.clear();
    tooth3.clear();
    tooth4.clear();
    if (QFile::exists(m_transPath)) {
        qDebug() << "File exists!";
        isMatched = true;
    }
    //importFace();
    //importMouth();
    if(importFace()&&importMouth()){
        teethPreview->exec();
    }

}

void SmileDesigenWidget::gensilhouette(){

}


void SmileDesigenWidget::boolteeth1(){
    //cgb->test();
    vtkSmartPointer<vtkPolyData> mouth_poly = vtkPolyDataMapper::SafeDownCast(mouths[0]->GetMapper())->GetInput();
    if(!mouth_poly){
        cout<<"mouth_poly is null"<<endl;
    }
    vtkSmartPointer<vtkPolyData> assembly_poly = vtkPolyDataMapper::SafeDownCast(assembly2->GetMapper())->GetInput();
    vtkSmartPointer<vtkActor> resActor = vtkSmartPointer<vtkActor>::New();

    // 输出点的数量
    std::cout << "Number of points before cleaning: " << assembly_poly->GetNumberOfPoints() << std::endl;
    //std::cout << "Number of points after cleaning: " << cleanedPolyData->GetNumberOfPoints() << std::endl;



    // 从 STL 文件加载第二个模型
    Mesh mesh2;
    if (!processor->loadFromSTL(assembly_poly, mesh2))
    {
        return ;
    }
    processor->repairMesh(mesh2,"mesh2");
    // 从 STL 文件加载第一个模型并修补孔洞
    Mesh mesh1;
    if (!processor->loadFromSTL(mouth_poly, mesh1))
    {
        return ;
    }
    processor->removeSmallComponents(mesh1,100);
    processor->fillHoles(mesh1);
    //processor.repairMesh(mesh1,"mesh1");
    // 执行布尔运算（用第二个模型减去修补后的第一个模型）



    Mesh result_mesh;
    if (!processor->performBooleanDifference(mesh1, mesh2, result_mesh))
    {
        return ;
    }

    // 保存结果
    processor->saveSTL(resActor, result_mesh);

    if (!processor->issucessful) {
        cout << "fail bool" << endl;
        return;
    }




    // 配置模型属性
    double assemblyColor[3];
    assembly1->GetProperty()->GetColor(assemblyColor);
    double assemblySpecular = assembly1->GetProperty()->GetSpecular();
    double assemblySpecularPower = assembly1->GetProperty()->GetSpecularPower();
    double assemblyAmbient = assembly1->GetProperty()->GetAmbient();
    double assemblyDiffuse = assembly1->GetProperty()->GetDiffuse();

    resActor->GetProperty()->SetColor(assemblyColor);
    resActor->GetProperty()->SetSpecular(assemblySpecular);
    resActor->GetProperty()->SetSpecularPower(assemblySpecularPower);
    resActor->GetProperty()->SetAmbient(assemblyAmbient);
    resActor->GetProperty()->SetDiffuse(assemblyDiffuse);



    processor->setActor(resActor);






    QFileInfo fileInfo(m_mouth_upper_Path);
    QDir dir = fileInfo.dir(); // 获取文件所在的目录
    QString parentDir = dir.absolutePath(); // 获取当前文件的目录路径
    //vtkSmartPointer<vtkSTLWriter> writer = vtkSmartPointer<vtkSTLWriter>::New();
    //writer->SetFileName((parentDir + "\\dentalVeneers.stl").toStdString().c_str());
    vtkSmartPointer<vtkPolyData> res_poly = vtkPolyDataMapper::SafeDownCast(resActor->GetMapper())->GetInput();
    processor->setPath(parentDir);

    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    QFile file(m_transPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                in >> matrix->Element[i][j];
            }
        }
        file.close();
    }
    else {
        qWarning() << "Failed to open file for reading" << endl;
    }

    m_transform2 = vtkSmartPointer<vtkTransform>::New();
    m_transform2->SetMatrix(matrix);


    vtkSmartPointer<vtkTransform> inverseTrans2 = vtkSmartPointer<vtkTransform>::New();
    inverseTrans2->DeepCopy(calibrate_transform);
    inverseTrans2->Inverse(); // 求 trans2 的逆

    vtkSmartPointer<vtkTransform> inverseTrans1 = vtkSmartPointer<vtkTransform>::New();
    inverseTrans1->DeepCopy(m_transform2);
    inverseTrans1->Inverse(); // 求 trans1 的逆

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter1 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter1->SetInputData(res_poly);
    transformFilter1->SetTransform(inverseTrans2); // 先应用 trans2 的逆
    transformFilter1->Update();

    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter2->SetInputData(transformFilter1->GetOutput());
    transformFilter2->SetTransform(inverseTrans1); // 再应用 trans1 的逆
    transformFilter2->Update();



    processor->setFilter(transformFilter2);

    processor->m_render();


}

