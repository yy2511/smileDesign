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


SmileDesigenWidget::SmileDesigenWidget(QWidget* parent) : QWidget(parent)
{   // 设置窗口默认全屏

	initUI();
	initVTK();
	initStyle();
	connect(buttonAutoCut, SIGNAL(clicked()), this, SLOT(autoCut()));
	connect(this, SIGNAL(addItemWhereRenderAddMember(const QString&, std::vector<vtkSmartPointer<vtkActor>>)), widget3, SLOT(addItem(const QString&, std::vector<vtkSmartPointer<vtkActor>>)));
	connect(this, SIGNAL(removeItemWhereRenderRemoveMember(const QString&)), widget3, SLOT(removeItem(const QString&)));
	connect(widget3, &ActorWidget::refresh, this, [this]() {
		renderer_main->Render();
		renderer_coronal->Render();
		renderer_sagittal->Render();
		renderer_top->Render();
		});

	connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(onCheckBoxStateChanged(int)));
	connect(checkBox2, SIGNAL(clicked()), this, SLOT(onCheckBox2Clicked()));
	connect(buttonFlip, SIGNAL(clicked()), this, SLOT(flipView()));
	connect(buttonPolyLine, &QPushButton::clicked, this, [this]() {
		if (guidelinesAdded) {
			removeLines();
			buttonPolyLine->setText(QStringLiteral("添加辅助线"));
		}
		else {
			drawLines();
			buttonPolyLine->setText(QStringLiteral("移除辅助线"));
		}
		guidelinesAdded = !guidelinesAdded;

		});
	connect(this, &SmileDesigenWidget::pointSelected, this, &SmileDesigenWidget::onPointSelected);
	//connect(this, &SmileDesigenWidget::changeStyle, this, &SmileDesigenWidget::onChangeStyle);

	connect(buttonImport, &QPushButton::clicked, this, &SmileDesigenWidget::importAll);


	connect(buttonMatch, &QPushButton::clicked, matchWindow, &AlignWindow::show);
	connect(buttonMatch, &QPushButton::clicked, this, [=]() {
		matchWindow->loadModelPair(m_facePath, m_mouthPath);
		});
	connect(matchWindow, &AlignWindow::applyTransform, this, &SmileDesigenWidget::mouthFirstAdjust);




	connect(buttonCalibrate, &QPushButton::clicked, this, [this]() {


		if (renderer_main) {
			pickStyle->SetDefaultRenderer(renderer_main);
		}
		else {
			qDebug() << "renderer_main is null!";
		}


		// Set the point selection style
		if (w3D_main) {
			w3D_main->renderWindow()->GetInteractor()->SetInteractorStyle(pickStyle);
            pickStyle->startNewSelection();
		}

		else {
			qDebug() << "Interactor is null!";
		}
		});
	connect(buttonCalibrate, &QPushButton::clicked, buttonFree, &QPushButton::show);
	connect(buttonCalibrate, &QPushButton::clicked, this, [this]() {
        if(faceActor){
            QPixmap pixmap(":/resources/pic/lefteye.png"); // 替换为你的图片路径

            tip->updateTip(QStringLiteral("请点击左侧眼睛中心"), pixmap);
            //layoutW3->addWidget(tip);
            tip->show();
            //tip->showAtWidgetBottomLeft();
        }
        }
        );
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



			buttonFree->setText(QStringLiteral("自由视角"));
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
			buttonFree->setText(QStringLiteral("锁定视角"));
		}
		isFreeView = !isFreeView;


		});

	connect(buttonLine, &QPushButton::clicked, this, [this]() {

        if(!smileStyle->isHasLine){
            w3D_main->renderWindow()->GetInteractor()->SetInteractorStyle(smileStyle);
            smileStyle->autoAddPoint();
            buttonLine->setText(QStringLiteral("移除微笑线"));
        }else{
            smileStyle->removePoint();
            smileStyle->isHasLine = false;
            buttonLine->setText(QStringLiteral("添加微笑线"));
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


}
void SmileDesigenWidget::initUI() {
	matchWindow = new AlignWindow();
	Qt::WindowFlags flags = this->windowFlags();
	flags |= Qt::WindowStaysOnTopHint;  // 添加置顶标志
	matchWindow->setWindowFlags(flags);


	std::cout << "ssssss" << endl;
	//matchWindow->show();
	w3D_main = new  QVTKOpenGLNativeWidget(this);
	w3D_coronal = new  QVTKOpenGLNativeWidget(this);
	w3D_sagittal = new  QVTKOpenGLNativeWidget(this);
	w3D_top = new  QVTKOpenGLNativeWidget(this);
	widget1 = new QWidget(this);
	widget2 = new QWidget(this);
	widget3 = new ActorWidget(this);
    widget4 = new QWidget(this);

	buttonImport = new QPushButton(this);
	buttonImport->setText(QStringLiteral("导入模型"));

	buttonMatch = new QPushButton(this);
	buttonMatch->setText(QStringLiteral("牙齿匹配"));
	buttonCalibrate = new QPushButton(this);
	buttonCalibrate->setText(QStringLiteral("头位矫正"));
	buttonAutoCut = new QPushButton(this);
	buttonAutoCut->setText(QStringLiteral("自动裁剪"));
	buttonPolyLine = new QPushButton(this);
	buttonPolyLine->setText(QStringLiteral("添加辅助线"));
	// 禁用按钮
	buttonPolyLine->setEnabled(false);

	buttonFree = new QPushButton(QStringLiteral("自由视角"), w3D_main);
	// 计算按钮的位置，使其在右下角
	int buttonWidth = 90;
	int buttonHeight = 30;
	int xPos = w3D_main->width() - buttonWidth;
	int yPos = w3D_main->height() - buttonHeight;

	// 设置按钮的几何位置
	buttonFree->setGeometry(xPos, yPos, buttonWidth, buttonHeight);
	buttonFree->hide();

	buttonLine = new QPushButton(this);
	buttonLine->setText(QStringLiteral("添加微笑线"));

	checkBox = new QCheckBox(this);
	checkBox->setText(QStringLiteral("调整整体"));
	checkBox2 = new QCheckBox(this);
	checkBox2->setText(QStringLiteral("裁剪牙齿"));
	// 添加按钮，点击时显示/隐藏悬浮窗口
	QPushButton* showDialogButton = new QPushButton(QStringLiteral("调整颜色"), this);

	buttonImport->setFixedSize(120, 30);
	buttonMatch->setFixedSize(120, 30);
	buttonCalibrate->setFixedSize(120, 30);
	buttonAutoCut->setFixedSize(120, 30);
	buttonPolyLine->setFixedSize(120, 30);
	showDialogButton->setFixedSize(120, 30);




	// 创建滑块和标签
	QSlider* specularSlider = new QSlider(Qt::Horizontal, this);
	specularSlider->setRange(0, 100);  // 0 to 100 对应 0.0 到 1.0
	specularSlider->setValue(80);  // 设置初始值为中间值

	QSlider* specularPowerSlider = new QSlider(Qt::Horizontal, this);
	specularPowerSlider->setRange(1, 100);  // Specular Power 通常在 1 到 100 之间
	specularPowerSlider->setValue(80);  // 设置初始值为中间值

	QSlider* ambientSlider = new QSlider(Qt::Horizontal, this);
	ambientSlider->setRange(0, 100);  // 0 to 100 对应 0.0 到 1.0
	ambientSlider->setValue(20);  // 设置初始值为 0.3

	QSlider* diffuseSlider = new QSlider(Qt::Horizontal, this);
	diffuseSlider->setRange(0, 100);  // 0 to 100 对应 0.0 到 1.0
	diffuseSlider->setValue(90);  // 设置初始值为 0.8




	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	QVBoxLayout* layout1 = new QVBoxLayout();
	QVBoxLayout* layout2 = new QVBoxLayout();
    layout3 = new QVBoxLayout();
    QVBoxLayout* layoutW1 = new QVBoxLayout();
	QVBoxLayout* layoutW2 = new QVBoxLayout();
    layoutW3 = new QVBoxLayout();
	layout1->setContentsMargins(0, 0, 0, 0);
	layout2->setContentsMargins(0, 0, 0, 0);
	layout3->setContentsMargins(0, 0, 0, 0);
	layoutW1->setContentsMargins(0, 0, 0, 0);
	layoutW2->setContentsMargins(0, 0, 0, 0);
    layoutW3->setContentsMargins(0, 0, 0, 0);

	layoutW1->addWidget(buttonImport);
	layoutW1->addWidget(buttonMatch);
	layoutW1->addWidget(buttonCalibrate);
	layoutW1->addWidget(buttonAutoCut);
	layoutW1->addWidget(buttonPolyLine);
	layoutW1->addWidget(showDialogButton);
	layoutW1->addWidget(buttonLine);


	layoutW2->addWidget(checkBox);
	layoutW2->addWidget(checkBox2);
	layoutW1->setAlignment(Qt::AlignHCenter);  // 设置整个 layoutW1 布局中的控件水平居中
	layoutW1->setSpacing(10);  // 设置按钮之间的垂直间距
	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);  // 设置为水平线
	line->setFrameShadow(QFrame::Sunken); // 设置下沉样式的阴影效果
	layoutW1->addWidget(line); // 将分隔线添加到布局

	layoutW2->setAlignment(Qt::AlignHCenter);
	widget1->setLayout(layoutW1);
	widget2->setLayout(layoutW2);
	layout->addLayout(layout1);
	layout->addLayout(layout2);
	layout->addLayout(layout3);
	layout->setStretch(0, 5);
	layout->setStretch(1, 3);
	layout->setStretch(2, 1);

	layout1->addWidget(w3D_main);
	layout2->addWidget(w3D_coronal);
	layout2->addWidget(w3D_top);
	layout2->addWidget(w3D_sagittal);
	layout3->addWidget(widget1);
	layout3->addWidget(widget2);
	layout3->addWidget(widget3);
    QPixmap pixmap(":/resources/pic/lefteye.png"); // 替换为你的图片路径
    tip = new TipWidget(QStringLiteral("请点击左侧眼睛中心"), pixmap);
    layoutW3->addWidget(tip);
    widget4->setLayout(layoutW3);
    layout3->addWidget(widget4);
    tip->hide();



	buttonFlip = new QPushButton(QStringLiteral("旋转视角"), w3D_sagittal);
	buttonFlip->setGeometry(0, 0, 90, 30);
	this->setLayout(layout);
	std::vector<std::vector<vtkSmartPointer<vtkActor>>> allTeeth = { tooth1, tooth2, tooth3, tooth4 };
	std::vector<vtkSmartPointer<vtkActor>> allAssembly = { assembly1,assembly2,assembly3,assembly4 };
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

	connect(specularPowerSlider, &QSlider::valueChanged, this, [=](int value) {



		assembly1->GetProperty()->SetSpecularPower(value);
		assembly2->GetProperty()->SetSpecularPower(value);
		assembly3->GetProperty()->SetSpecularPower(value);
		assembly4->GetProperty()->SetSpecularPower(value);
		w3D_main->renderWindow()->Render();
		w3D_coronal->renderWindow()->Render();
		w3D_sagittal->renderWindow()->Render();
		w3D_top->renderWindow()->Render();
		qDebug() << "Specular Power value:" << value;  // 输出滑块的值到调试控制台
		});

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

    connect(diffuseSlider, &QSlider::valueChanged, this, [=](int value) {

        double diffuse = value / 100.0;  // 转换为 0.0 到 1.0 范围
        // for (auto& teeth : allTeeth) {
        //     for (auto& tooth : teeth) {
        //         tooth->GetProperty()->SetDiffuse(diffuse);
        //     }
        // }


        assembly1->GetProperty()->SetDiffuse(diffuse);
        assembly2->GetProperty()->SetDiffuse(diffuse);
        assembly3->GetProperty()->SetDiffuse(diffuse);
        assembly4->GetProperty()->SetDiffuse(diffuse);

        w3D_main->renderWindow()->Render();
        w3D_coronal->renderWindow()->Render();
        w3D_sagittal->renderWindow()->Render();
        w3D_top->renderWindow()->Render();
        qDebug() << "Diffuse value:" << diffuse;  // 输出滑块的值到调试控制台
    });

    // 创建颜色选择器
    QWidget* colorPickerWidget = new QWidget(this);
    QGridLayout* colorPickerLayout = new QGridLayout(colorPickerWidget);

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
        colorButton->setFixedSize(40, 40);
        colorButton->setCheckable(true);  // 设置按钮为可选中状态
        colorButton->setStyleSheet(QString("background-color: %1;").arg(colors[i].name()));

        // 添加标签
        QLabel* colorLabel = new QLabel(labels[i], colorButton);
        colorLabel->setAlignment(Qt::AlignCenter);
        colorLabel->setStyleSheet("color: black; background-color: rgba(255, 255, 255, 0);");  // 透明背景

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
    colorPickerWidget->setStyleSheet(
        "QPushButton:checked {"
        "  border: 2px solid blue;"
        "}"
        );

    // 创建悬浮窗口，并设置为隐藏状态
    floatingDialog = new QWidget(this);
    floatingDialog->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    floatingDialog->setWindowModality(Qt::NonModal);
    floatingDialog->setAttribute(Qt::WA_DeleteOnClose, false);
    //floatingDialog->setFixedSize(250, 300);  // 设置悬浮区域的大小
    QVBoxLayout* floatingLayout = new QVBoxLayout(floatingDialog);
    // 将你的滑块和颜色选择器添加到悬浮窗口中
    floatingLayout->addWidget(new QLabel(QStringLiteral("反光强度"), floatingDialog));
    floatingLayout->addWidget(specularSlider);
    floatingLayout->addWidget(new QLabel(QStringLiteral("光泽度"), floatingDialog));
    floatingLayout->addWidget(specularPowerSlider);
    floatingLayout->addWidget(new QLabel(QStringLiteral("环境光比例"), floatingDialog));
    floatingLayout->addWidget(ambientSlider);
    floatingLayout->addWidget(new QLabel(QStringLiteral("漫反射比例"), floatingDialog));
    floatingLayout->addWidget(diffuseSlider);
    floatingLayout->addWidget(colorPickerWidget);




    connect(showDialogButton, &QPushButton::clicked, this, [=]() {
        if (floatingDialog->isVisible()) {
            floatingDialog->hide();
        }
        else if (assembly1) {
            checkBox->setCheckState(Qt::Checked);
            floatingDialog->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            floatingDialog->adjustSize();  // 确保对话框适应内容大小

            // // // 获取主窗口左下角的位置
            // int x = this->geometry().x() + 10;  // 左边距，稍微留一点边距
            // int y = this->geometry().y() + this->height() - floatingDialog->height() - 10;  // 底边距，稍微留一点边距
            // floatingDialog->move(x, y);
            layoutW3->addWidget(floatingDialog);
            widget4->setLayout(layoutW3);
            layout3->addWidget(widget4);
            floatingDialog->show();
        }
    });

    // 创建标签
    QLabel *label = new QLabel(QStringLiteral("请选择牙齿辅助线比例："), this);
    layoutW3->addWidget(label);

    // 创建下拉框
    QComboBox *comboBox = new QComboBox(this);

    // 添加选项
    comboBox->addItem(QStringLiteral("0.8"));
    comboBox->addItem(QStringLiteral("0.7"));
    comboBox->addItem(QStringLiteral("0.618"));
    layoutW3->addWidget(comboBox);
    connect(comboBox, &QComboBox::currentTextChanged, this, [=](const QString &text) {

        smileStyle->factor = text.toDouble();  // 将选中的文本转换为 double 并赋值给变量
        smileStyle->gennerateToothLine();
    });

}

void SmileDesigenWidget::initVTK() {

	vtkSmartPointer<vtkGenericOpenGLRenderWindow>   win1 = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	vtkSmartPointer<vtkGenericOpenGLRenderWindow>   win2 = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	vtkSmartPointer<vtkGenericOpenGLRenderWindow>   win3 = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	vtkSmartPointer<vtkGenericOpenGLRenderWindow>   win4 = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	renderer_main = vtkSmartPointer<vtkRenderer>::New();
	//renderer_main->SetBackground(0.1, 0.2, 0.4);
	renderer_coronal = vtkSmartPointer<vtkRenderer>::New();
	vtkInformation* info_coronal = renderer_coronal->GetInformation();
	info_coronal->Set(RendererIDKey, "Renderer_1");
	cout << "renderer_coronal" << renderer_coronal << endl;

	renderer_sagittal = vtkSmartPointer<vtkRenderer>::New();
	vtkInformation* info_sagittal = renderer_sagittal->GetInformation();
	info_sagittal->Set(RendererIDKey, "Renderer_2");
	cout << "renderer_sagittal" << renderer_sagittal << endl;

	renderer_top = vtkSmartPointer<vtkRenderer>::New();
	vtkInformation* info_top = renderer_top->GetInformation();
	info_top->Set(RendererIDKey, "Renderer_3");
	cout << "renderer_top" << renderer_top << endl;

	//renderer_main->SetBackground(255,255,255);
	//renderer_coronal->SetBackground(255,255,255);
	//renderer_sagittal->SetBackground(255,255,255);
	//renderer_top->SetBackground(255,255,255);
	vtkCamera* camera_main = renderer_main->GetActiveCamera();
	camera_main->SetViewUp(0, 1, 0);
	camera_main->SetPosition(0, 0, 300);
	camera_main->SetFocalPoint(0, 0, 0);

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
	//vtk8.2
	// w3D_main->SetRenderWindow(win1);
	// w3D_coronal->SetRenderWindow(win2);
	// w3D_sagittal->SetRenderWindow(win3);
	// w3D_top->SetRenderWindow(win4);
	// w3D_main->GetRenderWindow()->AddRenderer(renderer_main);
	// w3D_coronal->GetRenderWindow()->AddRenderer(renderer_coronal);
	// w3D_sagittal->GetRenderWindow()->AddRenderer(renderer_sagittal);
	// w3D_top->GetRenderWindow()->AddRenderer(renderer_top);

	HandleInteractor* style1 = new HandleInteractor();
	HandleInteractor* style2 = new HandleInteractor();
	HandleInteractor* style3 = new HandleInteractor();
	w3D_coronal->renderWindow()->GetInteractor()->SetInteractorStyle(style1);
	w3D_sagittal->renderWindow()->GetInteractor()->SetInteractorStyle(style2);
	w3D_top->renderWindow()->GetInteractor()->SetInteractorStyle(style3);

	// //vtk8.2
	// w3D_coronal->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style1);
	// w3D_sagittal->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style2);
	// w3D_top->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style3);
	MyQtObject* myqtObject = new MyQtObject();
	style1->setqtObject(myqtObject);
	style2->setqtObject(myqtObject);
	style3->setqtObject(myqtObject);

	style1->setRender(renderer_coronal);
	style2->setRender(renderer_sagittal);
	style3->setRender(renderer_top);

	connect(myqtObject, SIGNAL(refresh()), this, SLOT(syncRefresh()));
	connect(widget3, &ActorWidget::refresh, this, &SmileDesigenWidget::syncRefresh);

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
}
void SmileDesigenWidget::importTooth() {
    cout<<"import tooth"<<endl;
	namespace fs = std::filesystem;
	Eigen::Matrix4d mat;
	mat << 0.979232, 0.0132982, 0.202309, -4.20313,
		0.0606796, 0.932885, -0.355027, -11.5458,
		-0.193452, 0.35993, 0.912703, -49.5687,
		0, 0, 0, 1;
	vtkMatrix4x4* transM = vtkMatrix4x4::New();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			transM->SetElement(i, j, mat(i, j));
		}
	}

	if (checkBox->isChecked()) {
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
	}
	else {

		if (tooth1.size() != 0) {
			for (int i = 0; i < tooth1.size(); i++) {
				renderer_main->RemoveActor(tooth1[i]);
				renderer_coronal->RemoveActor(tooth2[i]);
				renderer_sagittal->RemoveActor(tooth3[i]);
				renderer_top->RemoveActor(tooth4[i]);
			}
		}
	}

	tooth1.clear();
	tooth2.clear();
	tooth3.clear();
	tooth4.clear();
	fs::path dirPath = "./resources/teeth";

	try {

		if (fs::exists(dirPath) && fs::is_directory(dirPath)) {

			for (const auto& entry : fs::directory_iterator(dirPath)) {


				std::string file = entry.path().string();

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
		else {
			std::cerr << "目录不存在或不是一个目录 " << std::endl;
		}
	}
	catch (const fs::filesystem_error& e) {

		std::cerr << e.what() << std::endl;
	}
    cout<<"unchecked"<<endl;
    checkBox->setCheckState(Qt::Unchecked);
    checkBox->setCheckState(Qt::Checked);
    cout<<"checked"<<endl;
	renderer_main->ResetCamera();
	renderer_coronal->ResetCamera();
	renderer_sagittal->ResetCamera();
	renderer_top->ResetCamera();
	syncRefresh();

}


void SmileDesigenWidget::importFace() {
	// 假设 filePath 是类的成员变量，直接使用它
	std::cout << "import face" << std::endl;

	// 定义有效的文件扩展名
	QStringList validExtensions = { "stl", "ply" };

	// 检查面扫文件
	QFileInfo faceFileInfo(m_facePath);
	if (!faceFileInfo.exists() || !validExtensions.contains(faceFileInfo.suffix().toLower())) {
		QMessageBox::warning(this, "Invalid File", "The face file is either non-existent or not in .stl or .ply format.");
		return;
	}

	// 检查口扫文件
	QFileInfo mouthFileInfo(m_mouthPath);
	if (!mouthFileInfo.exists() || !validExtensions.contains(mouthFileInfo.suffix().toLower())) {
		QMessageBox::warning(this, "Invalid File", "The mouth file is either non-existent or not in .stl or .ply format.");
		return;
	}
	if (!m_facePath.isEmpty())
	{
		// 移除旧的面扫模型
		renderer_main->RemoveActor(faceActor);
		checkBox2->setCheckState(Qt::Unchecked);

		// 设置交互样式
		vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
		renderer_main->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);

		// 发出信号移除面扫模型
		emit removeItemWhereRenderRemoveMember(QStringLiteral("面扫模型"));

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
			return;
		}

		// 添加新的面扫模型
		renderer_main->AddActor(faceActor);

		std::vector<vtkSmartPointer<vtkActor>> actors;
		actors.push_back(faceActor);
		addItemWhereRenderAddMember(QStringLiteral("面扫模型"), actors);

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
	}
}

void SmileDesigenWidget::importMouth() {


	// 定义有效的文件扩展名
	QStringList validExtensions = { "stl", "ply" };

	// 检查面扫文件
	QFileInfo faceFileInfo(m_facePath);
	if (!faceFileInfo.exists() || !validExtensions.contains(faceFileInfo.suffix().toLower())) {
		QMessageBox::warning(this, "Invalid File", "The face file is either non-existent or not in .stl or .ply format.");
		return;
	}

	// 检查口扫文件
	QFileInfo mouthFileInfo(m_mouthPath);
	if (!mouthFileInfo.exists() || !validExtensions.contains(mouthFileInfo.suffix().toLower())) {
		QMessageBox::warning(this, "Invalid File", "The mouth file is either non-existent or not in .stl or .ply format.");
		return;
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

	if (!m_mouthPath.isEmpty())
	{
		if (mouths.size() != 0) {
			renderer_main->RemoveActor(mouths[0]);
			renderer_coronal->RemoveActor(mouths[1]);
			renderer_sagittal->RemoveActor(mouths[2]);
			renderer_top->RemoveActor(mouths[3]);
		}
		mouths.clear();
		emit removeItemWhereRenderRemoveMember(QStringLiteral("口扫上颌模型"));
		QFileInfo fileInfo(m_mouthPath);
		QString fileExtension = fileInfo.suffix().toLower();
		if (fileExtension == "stl") {
			vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
			reader->SetFileName(m_mouthPath.toUtf8().data());
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
			reader->SetFileName(m_mouthPath.toUtf8().data());
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

				mouthActor->GetProperty()->SetAmbient(0.3);
				mouthActor->GetProperty()->SetDiffuse(0.7);
				mouthActor->GetProperty()->SetSpecular(1.0);
				mouthActor->GetProperty()->SetSpecularPower(30);
				mouthActor->SetPickable(0);
				mouths.push_back(mouthActor);
				cout << "111111111" << endl;
			}

		}


		addItemWhereRenderAddMember(QStringLiteral("口扫上颌模型"), mouths);

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

}

void SmileDesigenWidget::SplitPolyDataByIdentifier(vtkSmartPointer<vtkPolyData> inputPolyData,
	std::map<int, vtkSmartPointer<vtkPolyData>>& outputPolyDataMap)
{
	vtkSmartPointer<vtkPoints> inputPoints = inputPolyData->GetPoints();
	vtkSmartPointer<vtkCellArray> inputCells = inputPolyData->GetPolys();
	vtkSmartPointer<vtkIntArray> inputIdentifiers = vtkIntArray::SafeDownCast(inputPolyData->GetPointData()->GetArray("Identifier"));

	vtkIdType numPoints = inputPoints->GetNumberOfPoints();
	vtkIdType numCells = inputCells->GetNumberOfCells();

	// 初始化每个标识符对应的 PolyData,
	for (int identifier : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}) {
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

	// 将点和细胞根据标识符分配到不同的 PolyData
	std::map<int, vtkSmartPointer<vtkPoints>> pointsMap;
	std::map<int, vtkSmartPointer<vtkCellArray>> cellsMap;

	for (auto& pair : outputPolyDataMap) {
		pointsMap[pair.first] = pair.second->GetPoints();
		cellsMap[pair.first] = pair.second->GetPolys();
	}

	// 遍历每个点，并将其分配到正确的 PolyData
	vtkIdType pointId;
	vtkIdType npts;
	const vtkIdType* pts;
	//vtkIdType* pts;
	vtkSmartPointer<vtkCellArray> currentCells;
	vtkSmartPointer<vtkIntArray> currentIdentifiers;
	vtkSmartPointer<vtkPoints> currentPoints;
	vtkSmartPointer<vtkCellArray> currentCellArray;

	inputCells->InitTraversal();

	while (inputCells->GetNextCell(npts, pts)) {
		std::map<int, vtkIdType> newPointIds;

		for (vtkIdType i = 0; i < npts; ++i) {
			int identifier = inputIdentifiers->GetValue(pts[i]);
			vtkSmartPointer<vtkPoints> points = pointsMap[identifier];
			vtkIdType newPointId = points->InsertNextPoint(inputPoints->GetPoint(pts[i]));
			newPointIds[pts[i]] = newPointId;
		}

		int currentIdentifier = inputIdentifiers->GetValue(pts[0]);
		currentCellArray = cellsMap[currentIdentifier];
		vtkIdType newCellId = currentCellArray->InsertNextCell(npts);
		for (vtkIdType i = 0; i < npts; ++i) {
			currentCellArray->InsertCellPoint(newPointIds[pts[i]]);
		}
	}

	for (auto& pair : outputPolyDataMap) {
		pair.second->Modified();
	}
}
void SmileDesigenWidget::onCheckBoxStateChanged(int state) {
	cout << 11112 << endl;

	if (!assembly1 && tooth1.size() == 0) {
		cout << 222 << endl;
		checkBox->setCheckState(Qt::Unchecked);
		QMessageBox m_box(QMessageBox::Information, QStringLiteral("提示信息 "), QStringLiteral("请先加载牙齿模型！"));
		QTimer::singleShot(500, &m_box, SLOT(accept()));
		m_box.exec();
		return;
    }
	cout << 1234 << endl;
    cout<<"value="<<state<<endl;
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

		assembly1 = vtkSmartPointer<vtkActor>::New();
		assembly2 = vtkSmartPointer<vtkActor>::New();
		assembly3 = vtkSmartPointer<vtkActor>::New();
		assembly4 = vtkSmartPointer<vtkActor>::New();

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
			floatingDialog->hide();
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
		checkBox2->setCheckState(Qt::Unchecked);
		QMessageBox m_box(QMessageBox::Information, QStringLiteral("提示信息 "), QStringLiteral("请先加载面扫模型！"));
		QTimer::singleShot(500, &m_box, SLOT(accept()));
		m_box.exec();
		return;
	}
	if (checkBox2->isChecked()) {
		//PolygonSelectInteractorStyle *style  = new PolygonSelectInteractorStyle();
		selectStyle->SetPolyData(vtkPolyDataMapper::SafeDownCast(faceActor->GetMapper())->GetInput());
		selectStyle->SetRenderer(renderer_main);
		renderer_main->GetRenderWindow()->GetInteractor()->SetInteractorStyle(selectStyle);

		//style->autoCut();

	}
	else {
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
		renderer_main->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
	}

}

void SmileDesigenWidget::autoCut() {
	cout << "smile::autocut used" << endl;
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

	if (!faceActor) {
		QMessageBox m_box(QMessageBox::Information, QStringLiteral("提示信息 "), QStringLiteral("请先加载面扫模型！"));
		QTimer::singleShot(500, &m_box, SLOT(accept()));
		m_box.exec();
		return;
	}
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
	for (auto& face : faces)
	{
		dlib::full_object_detection shape = predictor(cimg, face);
		std::cout << "Number of parts: " << shape.num_parts() << std::endl;
		double test1[2] = { static_cast<double>(shape.part(27).x()), static_cast<double>(img_height - shape.part(27).y()) };
		double test2[2] = { static_cast<double>(shape.part(33).x()), static_cast<double>(img_height - shape.part(33).y()) };

		// 在图像上绘制关键点并打印点位信息
		for (int i = 60; i < 68; ++i)
		{
			cv::Point point(shape.part(i).x(), shape.part(i).y());
			cv::circle(img, point, 2, cv::Scalar(0, 255, 0), -1);

			// Adjust y-coordinate for VTK
			int vtk_y = img_height - point.y;

			// Print adjusted coordinates
			std::cout << "Point " << i + 1 << ": (" << point.x << ", " << vtk_y << ")" << std::endl;

			ScreenPoints->InsertNextPoint(point.x, vtk_y, 0.0);
		}
        double p1[3];
        ScreenPoints->GetPoint(0,p1);
        p1[0]-=2;

        //ScreenPoints->InsertNextPoint(p1);

	}



    vtkSmartPointer<vtkPolyData> facePolyData = vtkSmartPointer<vtkPolyData>::New();
    facePolyData = vtkPolyDataMapper::SafeDownCast(faceActor->GetMapper())->GetInput();

    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.005);
    for (vtkIdType i = 0; i < ScreenPoints->GetNumberOfPoints(); ++i)
    {
        double point[3];
        ScreenPoints->GetPoint(i, point);
        picker->Pick(point[0], point[1], 0, renderer_main);

        double pickedPosition[3];
        picker->GetPickPosition(pickedPosition);
        WorldPoints->InsertNextPoint(pickedPosition[0], pickedPosition[1], pickedPosition[2]);
    }

    vtkNew<vtkActor> resultActor;
    vtkNew<vtkParametricSpline> spline;
    spline->SetPoints(WorldPoints);
    //spline->ClosedOn();
    cout << "Number of input points: " << WorldPoints->GetNumberOfPoints() << endl;

    vtkNew<vtkParametricFunctionSource> functionSource;
    functionSource->SetParametricFunction(spline);
    functionSource->SetUResolution(60 * (WorldPoints->GetNumberOfPoints()));
    functionSource->Update();

    vtkPolyData* out = functionSource->GetOutput();
    std::cout << "Number of points in the output: " << out->GetNumberOfPoints() << std::endl;

    // 获取插值曲线上的点
    vtkPoints* curvePoints = out->GetPoints();


    for (vtkIdType i = 0; i < out->GetNumberOfPoints(); i++) {
        double point[3];
        curvePoints->GetPoint(i, point);  // 获取每个点的坐标
        std::cout << "Point " << i << ": (" << point[0] << ", " << point[1] << ", " << point[2] << ")" << std::endl;
    }
    vtkNew<vtkPolyDataMapper> resultMapper;
    resultMapper->SetInputConnection(functionSource->GetOutputPort());

    resultActor->SetMapper(resultMapper);
    resultActor->GetProperty()->SetLineWidth(2.0);  // 设置线宽为2
    resultActor->GetProperty()->SetColor(1.0, 1.0, 0.0);  // 将曲线设置为黄色
    vtkPolyDataMapper* resultMapper1 = vtkPolyDataMapper::SafeDownCast(resultActor->GetMapper());

    // 获取 resultMapper 的输入数据
    vtkPolyData* polyData = resultMapper1->GetInput();

    if (polyData)
    {
        vtkPoints* points = polyData->GetPoints();  // 获取所有点的指针

        if (points)
        {
            // 遍历所有点并打印它们的坐标
            for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
            {
                double point[3];  // 存储单个点的坐标 (x, y, z)
                points->GetPoint(i, point);  // 获取第 i 个点的坐标

                // 打印点的坐标
                std::cout << "Point " << i << ": ("
                          << point[0] << ", "
                          << point[1] << ", "
                          << point[2] << ")" << std::endl;
            }
        }
    }
    //renderer_main->AddActor(resultActor);
    renderer_main->GetRenderWindow()->Render();

    facePolyData = vtkPolyDataMapper::SafeDownCast(faceActor->GetMapper())->GetInput();


    vtkSmartPointer<vtkImplicitSelectionLoop> loop = vtkSmartPointer<vtkImplicitSelectionLoop>::New();
    loop->SetLoop(out->GetPoints());
    loop->SetAutomaticNormalGeneration(false);

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

void SmileDesigenWidget::setDataPath(QString facePath, QString mouthPath) {

	m_facePath = facePath;
	m_mouthPath = mouthPath;
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

        QPixmap pixmap(":/resources/pic/righteye.png"); // 替换为你的图片路径

        tip->updateTip(QStringLiteral("请点击右侧眼睛中心"), pixmap);
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
        tip->updateTip(QStringLiteral("请点击鼻上侧"), pixmap);
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

        QPixmap pixmap(":/resources/pic/noseunder.png"); // 替换为你的图片路径
        tip->updateTip(QStringLiteral("请点击鼻下侧"), pixmap);
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

		std::cout << "Transform  inverseMatrix Matrix:" << std::endl;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				// 打印矩阵的每一个元素
				std::cout << inverseMatrix->GetElement(i, j) << " ";
			}
			std::cout << std::endl;
		}
		// 创建 transform
		m_transform = vtkSmartPointer<vtkTransform>::New();
		m_transform->SetMatrix(inverseMatrix);


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
		cout << "pickStyle->SelectedActors.size():" << pickStyle->SelectedActors.size() << endl;


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
		renderer_main->GetRenderWindow()->Render();

		if (!assembly1) {
			importTooth();

		}
		cout << 1111 << endl;
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


	emit removeItemWhereRenderRemoveMember(QStringLiteral("口扫上颌模型"));
	QFileInfo fileInfo(m_mouthPath);
	QString fileExtension = fileInfo.suffix().toLower();
	renderer_main->RemoveActor(mouths[0]);
	renderer_coronal->RemoveActor(mouths[1]);
	renderer_sagittal->RemoveActor(mouths[2]);
	renderer_top->RemoveActor(mouths[3]);
	mouths.clear();

	if (fileExtension == "stl") {
		vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
		reader->SetFileName(m_mouthPath.toUtf8().data());
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
		reader->SetFileName(m_mouthPath.toUtf8().data());
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

			mouthActor->GetProperty()->SetAmbient(0.3);
			mouthActor->GetProperty()->SetDiffuse(0.7);
			mouthActor->GetProperty()->SetSpecular(1.0);
			mouthActor->GetProperty()->SetSpecularPower(30);
			mouths.push_back(mouthActor);
		}
	}

	addItemWhereRenderAddMember(QStringLiteral("口扫上颌模型"), mouths);

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
void SmileDesigenWidget::importAll() {
    initStyle();
    //isFisrstImport = false;
	if (checkBox->isChecked()) {
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
	}
	else {

		if (tooth1.size() != 0) {
			for (int i = 0; i < tooth1.size(); i++) {
				renderer_main->RemoveActor(tooth1[i]);
				renderer_coronal->RemoveActor(tooth2[i]);
				renderer_sagittal->RemoveActor(tooth3[i]);
				renderer_top->RemoveActor(tooth4[i]);
			}
		}
	}

	tooth1.clear();
	tooth2.clear();
	tooth3.clear();
	tooth4.clear();
	if (QFile::exists(m_transPath)) {
		qDebug() << "File exists!";
		isMatched = true;
    }
	importFace();
	importMouth();

}
