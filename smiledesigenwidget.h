#ifndef SMILEDESIGENWIDGET_H
#define SMILEDESIGENWIDGET_H

#include <QWidget>
#include "QVTKOpenGLNativeWidget.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "qpushbutton.h"
#include "qcheckbox.h"

#include "vtkMatrix4x4.h"
#include "ActorWidget.h"
#include "vtkAssembly.h"
#include "vtkBoxWidget2.h"
#include <vtkEventQtSlotConnect.h>
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "qcombobox.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include "stack"
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include"pickpointselectionstyle.h"
#include <AlignWindow.h>
#include<polygonselectinteractorstyle.h>
#include"smilelinestyle.h"
#include"handleinteractor.h"
#include"zoomonlyinteractorstyle.h"
//#include "SilhouetteRenderer.h"
#include"cgalbool.h"
#include <vtkSTLWriter.h>
#include<unordered_map>
#include<QMainWindow>
#include<QStackedWidget>
#include<LocalAdjustmentInterStyle.h>
#include<teethpreview.h>
enum class State {
    Init,
    Import,
    Rectify
};

class SmileDesigenWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SmileDesigenWidget(QWidget* parent = nullptr);

signals:
	void addItemWhereRenderAddMember(const QString& name, std::vector<vtkSmartPointer<vtkActor>> actors);
	void removeItemWhereRenderRemoveMember(const QString& name);
	void pointSelected(int step, double x, double y, double z);
    void importtoothFinish();
public slots:
    void setDataPath(QString facePath, QString mouthUpperPath,QString mouthLowerPath);

    void importTooth(const QString& teethpath );
    bool importFace();
    bool importMouth();
	void onCheckBoxStateChanged(int state);

	void onCheckBox2Clicked();
	void syncRefresh();
	void flipView();
	void autoCut();
	void drawLines();
	void addLine(double* p1, double* p2);
	void removeLines();
	//void updateLines(double *p1, double *p2);
	void onPointSelected(int pointNum, double x, double y, double z);
	void mouthFirstAdjust();
	void importAll();

    void boolteeth1();
private:
	void initUI();
	void initVTK();
	void initStyle();
	void SplitPolyDataByIdentifier(vtkSmartPointer<vtkPolyData> inputPolyData,
		std::map<int, vtkSmartPointer<vtkPolyData>>& outputPolyDataMap);

    void gensilhouette();
	// 添加辅助线的开关变量
    bool isFisrstImport = false;

	bool isFreeView = false;
	double point1[3];
	double point2[3];
    std::unordered_map<QString, Eigen::Matrix4d> pathToMatrixMap ;
public:
	AlignWindow* matchWindow;
    QWidget* finishWindow;
    TeethPreview* teethPreview;
private:
	QVTKOpenGLNativeWidget* w3D_main;
	QVTKOpenGLNativeWidget* w3D_coronal;
	QVTKOpenGLNativeWidget* w3D_sagittal;
	QVTKOpenGLNativeWidget* w3D_top;
	vtkSmartPointer<vtkRenderer> renderer_main;
	vtkSmartPointer<vtkRenderer> renderer_coronal;
	vtkSmartPointer<vtkRenderer> renderer_sagittal;
	vtkSmartPointer<vtkRenderer> renderer_top;
	std::vector<vtkSmartPointer<vtkActor>> tooth1;
	std::vector<vtkSmartPointer<vtkActor>> tooth2;
	std::vector<vtkSmartPointer<vtkActor>> tooth3;
	std::vector<vtkSmartPointer<vtkActor>> tooth4;
	std::vector<vtkSmartPointer<vtkActor>> mouths;
    std::vector<vtkSmartPointer<vtkActor>> mouths_lower;
	vtkSmartPointer<vtkActor> faceActor;
	vtkSmartPointer<vtkActor> mouthActor1;
	vtkSmartPointer<vtkActor> mouthActor2;
	vtkSmartPointer<vtkActor> mouthActor3;
	vtkSmartPointer<vtkActor> mouthActor4;
	vtkSmartPointer<vtkActor> selectedActor;
	vtkSmartPointer<vtkRenderWindowInteractor> selectedInteractor;


	vtkSmartPointer<vtkEventQtSlotConnect> Connections;
	vtkMatrix4x4* mouthTransM = vtkMatrix4x4::New();
	QString m_facePath;
    QString m_mouth_upper_Path;
    QString m_mouth_lower_Path;
	QString m_transPath;
    QString m_teethpath;
	dlib::frontal_face_detector detector;
	dlib::shape_predictor predictor;
    //SilhouetteRenderer* silhouetteRenderer;

	//QPushButton *buttonImportTooth;
    QLabel* teethlabel;
    QComboBox* teethComboBox;
	QPushButton* buttonImportFace;
	QPushButton* buttonImportMouth;
	QPushButton* buttonFlip;
	QPushButton* buttonPolyLine;
	QPushButton* buttonAutoCut;
	QPushButton* buttonCalibrate;
	QPushButton* buttonMatch;
	QPushButton* buttonImport;
	QPushButton* buttonFree;
	QPushButton* buttonLine;
    QPushButton* buttonclearLine;
	TipWidget* tip;
    QStackedWidget *stackedWidget;

	QCheckBox* checkBox;

    int lastcheckBoxState = 0;

    QLabel* polylinelabel;
	QComboBox* polylinecomboBox;
    QWidget* widgetButtton;
    QWidget* widgetFixed;
	ActorWidget* widgetActor;
    QWidget* widget4;
    QWidget* floatingDialog;
	QLabel* floatingLabel;
    QVBoxLayout* layoutfixed;
    QVBoxLayout* layoutW3;
    QVBoxLayout* layoutWidget;

    QAction *autoAction;
    QAction *manualAction;

    std::vector<vtkSmartPointer<vtkActor>>assembleactor;
	vtkSmartPointer<vtkActor> assembly1;
	vtkSmartPointer<vtkActor> assembly2;
	vtkSmartPointer<vtkActor> assembly3;
	vtkSmartPointer<vtkActor> assembly4;
	vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

	std::vector<vtkSmartPointer<vtkActor2D>> lineList;
	vtkSmartPointer<vtkBoxWidget2> box;

	//styles
	vtkSmartPointer<ZoomOnlyInteractorStyle> zoomStyle;
	vtkSmartPointer<PickPointSelectionStyle> pickStyle;
	vtkSmartPointer<PolygonSelectInteractorStyle> selectStyle;
	vtkSmartPointer<SmileLineStyle> smileStyle;
    vtkSmartPointer<HandleInteractor> handleStyle1;
    vtkSmartPointer<HandleInteractor> handleStyle2;
    vtkSmartPointer<HandleInteractor> handleStyle3;
    vtkSmartPointer<LocalAdjustmentInterStyle> localadjustStyle;


	int currentStep;
	double rightEyePoint[3];
	double leftEyePoint[3];
	double noseOnPoint[3];
	double noseUnderPoint[3];

	bool isMatched = false;
    bool firstimport = true;
    State currentState;
	vtkSmartPointer<vtkTransform> m_transform;
	vtkSmartPointer<vtkTransform> m_transform2;
    vtkSmartPointer<vtkTransform> calibrate_transform ;
    bool isCalibrated = false;
    vtkSmartPointer<vtkTransform> lastTransform;
    void test();
    STLBooleanProcessor* processor;
};

#endif // SMILEDESIGENWIDGET_H
