#include "pickpointselectionstyle.h"

vtkStandardNewMacro(PickPointSelectionStyle);

PickPointSelectionStyle::PickPointSelectionStyle()
{
    this->SelectedPointCount = 0;
    pickMode=false;
    test = 0;
}

void PickPointSelectionStyle::SetCallback(std::function<void(int, double, double, double)> callback)
{
    this->PointSelectedCallback = callback;
}

void PickPointSelectionStyle::OnLeftButtonDown()
{
    if((!pickMode))
    {
        //vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
        return;
    }
    if(pickMode){
        cout<<"pickMode"<<endl;
        // 获取鼠标点击的屏幕坐标
        int* clickPos = this->GetInteractor()->GetEventPosition();

        // 创建拾取器
        vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
        picker->SetTolerance(0.001);

        // 在场景中拾取
        picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

        double pickedPos[3];
        picker->GetPickPosition(pickedPos);

        if (picker->GetCellId() != -1) // 确保拾取到了有效的点
        {
            std::cout << "Picked point at: "
                      << pickedPos[0] << ", "
                      << pickedPos[1] << ", "
                      << pickedPos[2] << std::endl;

            if (this->PointSelectedCallback)
            {
                this->PointSelectedCallback(SelectedPointCount, pickedPos[0], pickedPos[1], pickedPos[2]);
            }

            // 创建控制点球体
            vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
            sphereSource->SetRadius(3.0);
            sphereSource->SetCenter(pickedPos);

            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper->SetInputConnection(sphereSource->GetOutputPort());

            vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
            actor->SetMapper(mapper);

            if (this->SelectedPointCount == 2 || this->SelectedPointCount == 3)
            {
                actor->GetProperty()->SetColor(0, 0, 1); // 蓝色 - 鼻子
            }
            else if(this->SelectedPointCount == 1 ||this->SelectedPointCount == 0)
            {
                actor->GetProperty()->SetColor(1, 1, 0); // 黄色 - 眼睛
            }

            this->GetDefaultRenderer()->AddActor(actor);
            cout<<"ppppppppppp"<<endl;
            this->SelectedActors.push_back(actor);

            this->SelectedPointCount++;
            if(SelectedActors.size()==4){
                stopSelection();
            }

        }

        // 继续处理鼠标事件（相机控制等）
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

}

void PickPointSelectionStyle::clearMarks()
{
    this->SelectedPointCount = 0;
    for(int i=0;i<SelectedActors.size();i++)
    {
        this->GetDefaultRenderer()->RemoveActor(SelectedActors.at(i));
    }
    SelectedActors.clear();

}

void PickPointSelectionStyle::startNewSelection()
{
    std::cout << "start  selection mode." << std::endl;
    cout<<"test:="<<test<<endl;
    clearMarks();
    cout<<2222222<<endl;
    pickMode=true;
    this->GetDefaultRenderer()->Render();
    cout<<33333<<endl;
}

void PickPointSelectionStyle::stopSelection()
{   std::cout << "stop  selection mode." << std::endl;
    clearMarks();
    pickMode = false;

    test = 9999;
    this->GetDefaultRenderer()->Render();
}
