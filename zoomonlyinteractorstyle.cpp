#include "zoomonlyinteractorstyle.h"

#include <vtkObjectFactory.h>

vtkStandardNewMacro(ZoomOnlyInteractorStyle);

void ZoomOnlyInteractorStyle::OnLeftButtonDown() {
    // 不处理左键点击事件
}

void ZoomOnlyInteractorStyle::OnRightButtonDown() {
    // 不处理右键点击事件
}

void ZoomOnlyInteractorStyle::OnMouseWheelForward() {
    // 保留父类的滚轮前进行为
    vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
}

void ZoomOnlyInteractorStyle::OnMouseWheelBackward() {
    // 保留父类的滚轮后退行为
    vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
}
