#ifndef POLYGONSELECTINTERACTORSTYLE_H
#define POLYGONSELECTINTERACTORSTYLE_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkActor2D.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <stack>
#include <chrono>
#include <vtkGlyphSource2D.h>
#include <vtkGlyph2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <vtkPolygon.h>
#include<vtkParametricSpline.h>
class PolygonSelectInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static PolygonSelectInteractorStyle* New();
    vtkTypeMacro(PolygonSelectInteractorStyle, vtkInteractorStyleTrackballCamera);

    PolygonSelectInteractorStyle();

    virtual void OnKeyPress() override;
    virtual void OnLeftButtonDown() override;
    virtual void OnRightButtonDown() override;

    void SetRenderer(vtkRenderer* renderer);
    void SetPolyData(vtkSmartPointer<vtkPolyData> polyData);

private:
    vtkSmartPointer<vtkPoints> ScreenPoints;
    vtkSmartPointer<vtkPoints> WorldPoints;
    vtkRenderer* Renderer;
    vtkSmartPointer<vtkPolyData> PolyData;
    vtkSmartPointer<vtkActor> resultActor;
    std::stack<vtkSmartPointer<vtkActor2D>> PointActors;
    std::stack<vtkSmartPointer<vtkActor2D>> LineActors;
    //vtkSmartPointer<vtkParametricSpline> spline;
    void DrawPolyline(double* p1, double* p2);
    void PerformPolygonSelection();
    void UndoLastAction();
    void ClearAllPointsAndLines();

    vtkSmartPointer<vtkActor2D> Add2DPointActor(int* pos);
    vtkSmartPointer<vtkActor2D> Add2DLineActor(double* p1, double* p2);
    bool IsPointInPolygon(double testPoint[3], vtkPoints* polygonPoints);
};

#endif // POLYGONSELECTINTERACTORSTYLE_H
