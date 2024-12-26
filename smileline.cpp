#include "smileline.h"

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkRendererCollection.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include<vtkCoordinate.h>
#include<vtkGlyphSource2D.h>
#include<vtkGlyph2D.h>
#include<vtkPolyDataMapper2D.h>
#include<vtkProperty2D.h>
#include<vtkActor2D.h>
#include<vtkNew.h>
#include<vtkParametricSpline.h>
#include<vtkParametricFunctionSource.h>
#include<vtkCellPicker.h>
#include<vtkRegularPolygonSource.h>
#include<vtkAlgorithmOutput.h>
#include<vtkPropPicker.h>
#include<vtkPointPicker.h>
vtkStandardNewMacro(SmileLine);

SmileLine::SmileLine() {
    this->ScreenPoints = vtkSmartPointer<vtkPoints>::New();

    resultActor = vtkSmartPointer<vtkActor>::New();
    PointActors = {};
    selectedHandle = vtkSmartPointer<vtkActor>::New();

}
SmileLine::~SmileLine(){

}
void SmileLine::SetRenderer(vtkRenderer* renderer) {
    this->Renderer = renderer;
}

void SmileLine::OnLeftButtonDown()
{
    isLeftDown = true;
    int* clickPos = this->GetInteractor()->GetEventPosition();
    // cout<<"clickPos:"<<clickPos[0]<<" "<<clickPos[1]<<endl;

    // 使用 vtkCellPicker 来检测点击的actor
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.0005);  // 设置拾取容差
    picker->Pick(clickPos[0], clickPos[1], 0, Renderer);

    vtkActor* pickedActor = picker->GetActor();  // 获取被选中的actor

    if (pickedActor != nullptr)
    {
        // std::cout << "Picked actor!" << std::endl;
        double bounds[6];
        pickedActor->GetBounds(bounds);
        // std::cout << "Bounding box: "
        //           << "xmin: " << bounds[0] << ", xmax: " << bounds[1] << ", "
        //           << "ymin: " << bounds[2] << ", ymax: " << bounds[3] << ", "
        //           << "zmin: " << bounds[4] << ", zmax: " << bounds[5] << std::endl;
        zmax = bounds[5];  // 获取 zmax
    }

    if(!isHasLine){
        // 转换屏幕坐标到世界坐标
        double* worldCoordinates = ClickToPick(clickPos);
        worldCoordinates[2] = zmax;
        vtkSmartPointer<vtkActor> circleActor = CreateCircleActor(worldCoordinates);
        Renderer->AddActor(circleActor);
        PointActors.push_back(circleActor);
        ScreenPoints->InsertNextPoint(worldCoordinates);
        Renderer->GetRenderWindow()->Render();

        if (this->ScreenPoints->GetNumberOfPoints()%3==0 )
        {
            double point[3];
            ScreenPoints->GetPoint(1, point); // 获取点的坐标
            // std::cout << "Point " << 2 << ": (" << point[0] << ", " << point[1] << ", " << point[2] << ")" << std::endl;

            point[1]+=5.0;

            lineMoveHandle = CreateCircleActor(point);
            PointActors.push_back(lineMoveHandle);
            Renderer->AddActor(lineMoveHandle);
            gennerateLine();
        }
    }
    else{
        // cout<<"clickPos:"<<clickPos[0]<<" "<<clickPos[1]<<endl;

        if (!PointActors.empty()) {
            for (size_t i = 0; i < PointActors.size(); ++i) {
                // 获取 Actor2D 的位置
                double worldPosition[3];
                PointActors[i]->GetPosition(worldPosition);
                // 将世界坐标转换为屏幕坐标
                vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
                coordinate->SetCoordinateSystemToWorld();
                coordinate->SetValue(worldPosition);
                double* displayPosition = coordinate->GetComputedDoubleDisplayValue(Renderer);  // 屏幕坐标

                // std::cout << "Screen pos: " << displayPosition[0] << " " << displayPosition[1] << std::endl;

                // 计算鼠标点击位置与当前 PointActor 的屏幕坐标的距离
                double distance = sqrt(pow(clickPos[0] - displayPosition[0], 2) + pow(clickPos[1] - displayPosition[1], 2));
                if (distance < 10) {
                    selectedHandle = PointActors[i];
                    handleID = i;
                    // cout<<"handleID"<<handleID<<endl;
                    vtkSmartPointer<vtkRegularPolygonSource> polygonSource = vtkRegularPolygonSource::SafeDownCast(selectedHandle->GetMapper()->GetInputConnection(0, 0)->GetProducer());
                    if (polygonSource) {
                        //polygonSource->SetRadius(15.0);  // 放大手柄的半径
                        selectedHandle->GetMapper()->Update();
                    }
                    selectedHandle->GetProperty()->SetColor(0.235, 0.7, 0.443);

                    this->Renderer->GetRenderWindow()->Render();
                    return;
                }
            }
        }
    // 继续父类的处理逻辑
    //vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }
}

void SmileLine::OnLeftButtonUp() {
    isLeftDown = false;
    if (selectedHandle != nullptr) {
        selectedHandle->GetProperty()->SetColor(1,0,0);
        Renderer->GetRenderWindow()->Render();
        selectedHandle = nullptr;
    }
}

void SmileLine::OnMouseMove(){

    if (isLeftDown && selectedHandle != nullptr&&isHasLine) {



        // cout<<"qqqqqq"<<endl;
        int* mousePos = this->GetInteractor()->GetEventPosition();
        // if(handleID == 3){
        //     double worldPosition[3];
        //     PointActors[3]->GetPosition(worldPosition) ;//将世界坐标转换为屏幕坐标
        //     vtkSmartPointer<vtkCoordinate>coordinate= vtkSmartPointer<vtkCoordinate>::New();
        //     coordinate->SetCoordinateSystemToWorld();
        //     coordinate->SetValue(worldPosition);
        //     double* displayPosition =coordinate->GetComputedDoubleDisplayValue(Renderer) ;//屏幕坐标
        // }
        double* pickedPos = ClickToPick(mousePos);

        pickedPos[2] = zmax;

        // cout<<"handleID"<<handleID<<endl;
        PointActors[handleID]->SetPosition(pickedPos);

        ScreenPoints->SetPoint(handleID, pickedPos);
        ScreenPoints->Modified();

        gennerateLine();
        Renderer->GetRenderWindow()->Render();
    }

    vtkInteractorStyleTrackballCamera::OnMouseMove();

}

void SmileLine::OnRightButtonDown(){
    //removePoint();
}

void SmileLine::OnMouseWheelForward(){
    vtkInteractorStyleTrackballCamera::OnMouseWheelForward();

}
void SmileLine::OnMouseWheelBackward(){
    vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();

}

void SmileLine::gennerateLine(){
    if(resultActor){
        Renderer->RemoveActor(resultActor);
    }
    vtkNew<vtkParametricSpline> spline;
    spline->SetPoints(ScreenPoints);
    // cout<<ScreenPoints->GetNumberOfPoints()<<endl;
    vtkParametricFunctionSource* functionSource =
        vtkParametricFunctionSource::New();
    functionSource->SetParametricFunction(spline);
    functionSource->SetUResolution(50 * ScreenPoints->GetNumberOfPoints());
    functionSource->Update();
    vtkPolyData* out = functionSource->GetOutput();
    // std::cout << "Number of points in the output: " << out->GetNumberOfPoints() << std::endl;
    vtkNew<vtkPolyDataMapper> resultMapper;
    resultMapper->SetInputConnection(functionSource->GetOutputPort());


    resultActor->SetMapper(resultMapper);
    resultActor->GetProperty()->SetLineWidth(2.0);  // 设置线宽为2
    resultActor->GetProperty()->SetColor(1.0, 1.0, 0.0);  // 将曲线设置为黄色

    this->Renderer->AddActor(resultActor);
    this->Renderer->GetRenderWindow()->Render();
    isHasLine = true;
}


double* SmileLine::ClickToPick(int* pos) {
    vtkSmartPointer<vtkPropPicker> picker = vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(pos[0], pos[1], 0, Renderer);
    picker->GetPickPosition(pickedPosition);
    // cout<<"pickedPosition：："<<pickedPosition[0]<<" "<<pickedPosition[1]<<" "<<pickedPosition[2]<<endl;
    if (pickedPosition[0]!=0) {
        // std::cout << "Picked an actor!" << std::endl;
        temp[0] = pickedPosition[0];
        temp[1] = pickedPosition[1];
        temp[2] = pickedPosition[2];
    } else {
        std::cout << "No actor picked." << std::endl;

        return temp;
    }
    return pickedPosition;
}

vtkSmartPointer<vtkActor> SmileLine:: CreateCircleActor(double *center)
{
    // 创建一个近似圆形的多边形源
    vtkSmartPointer<vtkRegularPolygonSource> circleSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
    circleSource->SetCenter(0,0,0);       // 设置圆心位置
    circleSource->SetRadius(2.0);       // 设置圆的半径
    circleSource->SetNumberOfSides(200);  // 设置圆的边数（越多越接近真正的圆）

    // 创建映射器
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(circleSource->GetOutputPort());

    // 创建actor
    vtkSmartPointer<vtkActor> circleActor = vtkSmartPointer<vtkActor>::New();
    circleActor->SetMapper(mapper);
    circleActor->GetProperty()->SetColor(1.0, 0.0, 0.0);  // 设置颜色为绿色
    circleActor->SetPosition(center);
    return circleActor;
}
void SmileLine::removePoint(){
    for(auto& point : PointActors){
        Renderer->RemoveActor(point);
    }
    PointActors.clear();
    if(resultActor){
        Renderer->RemoveActor(resultActor);
    }
    Renderer->GetRenderWindow()->Render();
}
