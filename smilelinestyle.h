#ifndef SMILELINESTYLE_H
#define SMILELINESTYLE_H
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkPoints.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include<vtkVertexGlyphFilter.h>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<vtkResizingWindowToImageFilter.h>
#include<QDebug>
#include<vtkWindowToImageFilter.h>
#include<vtkPNGWriter.h>
class SmileLineStyle: public vtkInteractorStyleTrackballCamera
{
public:
    static SmileLineStyle* New();
    vtkTypeMacro(SmileLineStyle, vtkInteractorStyleTrackballCamera);

    SmileLineStyle();
    ~SmileLineStyle() override;
    // 设置渲染器
    void SetRenderer(vtkRenderer* renderer);
    double *CoordTrans(double* pos);
    // 捕获鼠标左键点击事件
    virtual void OnLeftButtonDown() override;
    virtual void OnLeftButtonUp() override;
    virtual void OnRightButtonDown() override;
    virtual void OnMouseWheelForward() override;
    virtual void OnMouseWheelBackward() override;
    virtual void OnMouseMove() override;

    void removePoint();
    bool isvisible = true;
    //vtkSmartPointer<vtkActor2D> Add2DPointActor(int* pos);
    vtkSmartPointer<vtkActor> CreateCircleActor(double *center);
    void gennerateSmileLine();
    void gennerateStraightLine();
    void gennerateToothLine();
    void gennerateFromPoints();
    void updateToothLine(double* translation);
    double *ClickToPick(int* pos);
    int *WorldToDis(double *world);
    void autoAddPoint();
    void setStraightPoints(int* pos);
    vtkSmartPointer<vtkActor> makeStraightLine(double*p1, double*p2);
    bool getHasLine();

    bool isHasLine = false;
    bool isLeftDown = false;
    int handleID;
    double factor = 0.8;//默认比例
private:
    vtkSmartPointer<vtkPoints> ScreenOfSmilePoints;
    vtkSmartPointer<vtkPoints> ScreenOfStraightPoints;
    vtkSmartPointer<vtkPoints> ScreenOfToothPoints;
    vtkRenderer* Renderer;
    vtkSmartPointer<vtkActor> resultActor;
    vtkSmartPointer<vtkPolyData> PolyData;
    std::vector<vtkSmartPointer<vtkActor>> PointSmileActors;
    vtkSmartPointer<vtkActor> selectedHandle;
    vtkSmartPointer<vtkActor> lineMoveHandle;
    std::vector<vtkSmartPointer<vtkActor>> lineActors;
    std::vector<vtkSmartPointer<vtkActor>> toothLines;
    std::vector<vtkSmartPointer<vtkActor>> teethHandle;
    double pickedPosition[3];
    double temp[3];
    double* test;
    double zmax;
    int centralPosition[2];
    double cpos [3];

    double width;
    double initWidth;
    bool isTeethHandle = false;

    std::unordered_map<int,double> percentOfPoint;
};




#endif // SMILELINESTYLE_H
