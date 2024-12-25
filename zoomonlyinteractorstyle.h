#ifndef ZOOMONLYINTERACTORSTYLE_H
#define ZOOMONLYINTERACTORSTYLE_H
#include<vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>

class ZoomOnlyInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static ZoomOnlyInteractorStyle* New();
    vtkTypeMacro(ZoomOnlyInteractorStyle, vtkInteractorStyleTrackballCamera);

    // 禁止左键点击事件
    virtual void OnLeftButtonDown() override;
    
    // 禁止右键点击事件
    virtual void OnRightButtonDown() override;

    // 滚轮事件照常处理
    virtual void OnMouseWheelForward() override;
    virtual void OnMouseWheelBackward() override;
};



#endif // ZOOMONLYINTERACTORSTYLE_H
