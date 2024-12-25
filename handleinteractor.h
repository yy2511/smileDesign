#ifndef HANDLEINTERACTOR_H
#define HANDLEINTERACTOR_H

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCellPicker.h>
#include <vtkCoordinate.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRegularPolygonSource.h>
#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkInformation.h>
#include <vtkObjectFactory.h>
#include <vector>
#include <cmath>
#include <iostream>
#include<QObject>
#include <vtkRendererCollection.h>
#include<vtkInformationStringKey.h>
#include <vtkProperty2D.h>
#include <vtkImageData.h>
#include <vtkPNGReader.h>
#include <vtkImageActor.h>

class MyQtObject : public QObject {
    Q_OBJECT
public:
signals:
    void refresh();
    void generateSync();
};
extern std::vector<vtkSmartPointer<vtkActor>> mouthActors;
extern std::vector<vtkSmartPointer<vtkActor>> mouthActors_lower;
extern vtkInformationStringKey* RendererIDKey ;
extern int sagittalView ;
class HandleInteractor : public vtkInteractorStyleTrackballCamera
{
public:
    static HandleInteractor* New();
    vtkTypeMacro(HandleInteractor, vtkInteractorStyleTrackballCamera)

        HandleInteractor();
    ~HandleInteractor() override;

    void OnLeftButtonDown() override;
    void OnRightButtonDown() override;
    void OnMiddleButtonDown() override;
    void OnMiddleButtonUp() override;
    void OnMouseWheelForward() override;
    void OnMouseWheelBackward() override;
    void OnLeftButtonUp() override;
    void OnRightButtonUp() override;
    void OnMouseMove() override;
    void flipsync();
    void Dolly(double factor)override;
    void DollyToPosition(double factor, int* position, vtkRenderer* renderer);
    void TranslateCamera(vtkRenderer* renderer, int fromX, int fromY, int toX, int toY);
    void ComputeWorldToDisplay(vtkRenderer* renderer, double x, double y, double z, double displayCoords[4]);
    void ComputeDisplayToWorld(vtkRenderer* renderer, double x, double y, double z, double worldCoords[4]);

    void setqtObject(MyQtObject* object);
    void setRender(vtkSmartPointer<vtkRenderer> render);
    void getActorNum(int id){
        cout<<"id="<<id<<endl;
    }
    //void Zoom(double factor) ;

    void generateArrowHandle(double* selectPos, int num);
    void setActor(int num);
    void removeBox();
    void removeArrowHandle() ;
    std::vector<vtkSmartPointer<vtkActor>> toothList;

    vtkSmartPointer<vtkActor> mergedActor;


private:
    void generateBox2D();
    vtkSmartPointer<vtkActor2D> generateCircleHandle(const std::vector<double>& screenPoint);
    void updataBox();
    void updataHandles(int id);
    void updataActor(int id);



    vtkSmartPointer<vtkRenderer> selectedRenderer;
    vtkSmartPointer<vtkActor> selectedActor;
    vtkSmartPointer<vtkActor2D> box;
    std::vector<vtkSmartPointer<vtkActor2D>> handles;
    vtkSmartPointer<vtkActor2D> selectedHandle;
    vtkRenderWindowInteractor* interactor;

    vtkSmartPointer<vtkActor2D> splineActor;
    vtkSmartPointer<vtkActor2D> arrowActorStart;
    vtkSmartPointer<vtkActor2D> arrowActorEnd;


    vtkSmartPointer<vtkImageActor> imageActor;

    bool isLeftDown;
    bool isMiddleDown;
    bool isInRec;
    int lastPos[2];
    int handleID;
    std::string rendererID;
    std::vector<std::vector<double>> handlesPoints;
    double lastWidth;
    double lastHeight;
    MyQtObject* myqtObject;
    bool isMergedActor = true;

    static std::vector<HandleInteractor*> styles;
};

#endif // HANDLEINTERACTOR_H
