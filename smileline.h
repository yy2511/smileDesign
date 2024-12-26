#ifndef SMILELINE_H
#define SMILELINE_H
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkPoints.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include<vtkVertexGlyphFilter.h>
class SmileLine: public vtkInteractorStyleTrackballCamera
{
public:
    static SmileLine* New();
    vtkTypeMacro(SmileLine, vtkInteractorStyleTrackballCamera);

    SmileLine();
    ~SmileLine() override;
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

    void updateScreenPoints();

    void pickToScreen();
    void removePoint();
    //vtkSmartPointer<vtkActor2D> Add2DPointActor(int* pos);
    vtkSmartPointer<vtkActor> CreateCircleActor(double *center);
    void gennerateLine();
    double *ClickToPick(int* pos);
    bool isHasLine = false;
    bool isLeftDown = false;
    int handleID;
private:
    vtkSmartPointer<vtkPoints> ScreenPoints;
    vtkRenderer* Renderer;
    vtkSmartPointer<vtkActor> resultActor;
    vtkSmartPointer<vtkPolyData> PolyData;
    std::vector<vtkSmartPointer<vtkActor>> PointActors;
    vtkSmartPointer<vtkActor> selectedHandle;
    vtkSmartPointer<vtkActor> lineMoveHandle;
    double pickedPosition[3];
    double temp[3];
    double* test;
    double zmax;
};

#endif // SMILELINE_H
