#ifndef PICKPOINTSELECTIONSTYLE_H
#define PICKPOINTSELECTIONSTYLE_H

#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkCellPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vector>
#include <functional>
#include <QObject>


class PickPointSelectionStyle : public vtkInteractorStyleTrackballCamera
{


public:
    static PickPointSelectionStyle* New();
    vtkTypeMacro(PickPointSelectionStyle, vtkInteractorStyleTrackballCamera);

    void SetCallback(std::function<void(int, double, double, double)> callback);

    void startNewSelection();

    void stopSelection();

    PickPointSelectionStyle();

    virtual void OnLeftButtonDown() override;

private:
    void clearMarks();
public:
    std::vector<vtkSmartPointer<vtkActor>> SelectedActors;
private:
    std::function<void(int, double, double, double)> PointSelectedCallback;
    int SelectedPointCount;
    //std::vector<vtkSmartPointer<vtkActor>> SelectedActors;
    bool pickMode;
    int test;
};


#endif // PICKPOINTSELECTIONSTYLE_H
