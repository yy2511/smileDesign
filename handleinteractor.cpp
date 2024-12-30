#include "handleinteractor.h"
#include<vtkAlgorithmOutput.h>
#include <QObject>
#include <vtkPNGReader.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkTexture.h>
#include <vtkPolyDataMapper.h>
#include<vtkPropPicker.h>
#include<vtkImageMapper3D.h>
#include<vtkImageMapper.h>
#include<vtkArrowSource.h>
#include<vtkImageThreshold.h>
#include<vtkLineSource.h>
#include<vtkAppendPolyData.h>
#include<vtkPoints.h>
#include<vtkParametricSpline.h>
#include<vtkParametricFunctionSource.h>
#include<vtkTransformFilter.h>
#include<vtkTransformPolyDataFilter.h>
#include "vtkWindowedSincPolyDataFilter.h"
vtkStandardNewMacro(HandleInteractor);
std::vector<vtkSmartPointer<vtkActor>> mouthActors = {};
std::vector<vtkSmartPointer<vtkActor>> mouthActors_lower = {};
vtkInformationStringKey* RendererIDKey = vtkInformationStringKey::MakeKey("RendererID", "vtkRenderer");
int sagittalView = 0;
std::vector<HandleInteractor*> HandleInteractor::styles;

HandleInteractor::HandleInteractor()
{
    styles.push_back(this);
    isLeftDown = false;
    isMiddleDown = false;
    isInRec = false;
    selectedRenderer = nullptr;
    selectedActor = nullptr;
    box = nullptr;
    selectedHandle = nullptr;
    interactor = nullptr;
    myqtObject = nullptr;


}

HandleInteractor::~HandleInteractor()
{
    removeBox();
    auto it = std::find(styles.begin(), styles.end(), this);
    if(it != styles.end()){
        styles.erase(it);
    }
}

void HandleInteractor::OnLeftButtonDown()
{


    isLeftDown = true;
    interactor = this->GetInteractor();
    if (!interactor) return;
    interactor->GetRenderWindow()->Render();

    vtkSmartPointer<vtkRenderer> renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    if (!renderer) return;

    //this->selectedRenderer = renderer;

    //cout<<rendererID<<endl;
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.02);
    int* clickPos = interactor->GetLastEventPosition();

    picker->Pick(clickPos[0], clickPos[1], 0, renderer);
    vtkActor* actor = picker->GetActor();

    if (!handles.empty()) {
        for (size_t i = 0; i < handles.size(); ++i) {
            double* pos = handles[i]->GetPosition();
            double distance = sqrt(pow(clickPos[0] - pos[0], 2) + pow(clickPos[1] - pos[1], 2));
            if (distance < 10) {
                selectedHandle = handles[i];
                handleID = i;
                vtkSmartPointer<vtkRegularPolygonSource> polygonSource = vtkRegularPolygonSource::SafeDownCast(selectedHandle->GetMapper()->GetInputConnection(0, 0)->GetProducer());
                if (polygonSource) {
                    polygonSource->SetRadius(10.0);  // 放大手柄的半径
                    selectedHandle->GetMapper()->Update();
                }
                double *test;
                test = selectedHandle->GetPosition();
                generateArrowHandle(test,handleID);
                selectedHandle->GetProperty()->SetColor(0.235, 0.7, 0.443);
                lastPos[0] = clickPos[0];
                lastPos[1] = clickPos[1];
                interactor->GetRenderWindow()->Render();
                return;
            }
        }
    }

    if(box!=nullptr){
        double* p1 = handles[0]->GetPosition();
        double* p2 = handles[1]->GetPosition();
        double* p3 = handles[2]->GetPosition();
        cout<<"p1:"<<p1[0]<<" "<<p1[1]<<endl;
        cout<<"p2:"<<p2[0]<<" "<<p2[1]<<endl;
        cout<<"p3:"<<p3[0]<<" "<<p3[1]<<endl;
        cout<<"clickPos:"<<clickPos[0]<<" "<<clickPos[1]<<endl;

            if((clickPos[0]>p1[0]&&clickPos[0]<p2[0])&&(clickPos[1]>p1[1]&&clickPos[1]<p3[1])){
                cout<<"in rec"<<endl;
                //box->GetProperty()->SetColor(0.235, 0.7, 0.443);
                isInRec = true;

                box->GetProperty()->SetColor(0.235, 0.7, 0.443);
                interactor->GetRenderWindow()->Render();

                vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
                picker->SetTolerance(0.02);
                int* clickPos = interactor->GetLastEventPosition();
                lastPos[0] = clickPos[0];
                lastPos[1] = clickPos[1];
            }

    }



    if (actor) {
        for(auto a: mouthActors){
            if(a == actor){
                //std::cout<<a<<std::endl;
                return;
            }
        }

        this->selectedActor = actor;
        cout<<"selectedActor:"<<selectedActor<<endl;
        cout<<"toothList:"<<toothList.size()<<endl;
        int num = -1;
        for(int i = 0; i < toothList.size(); i++){
            if(toothList[i]==selectedActor){
                cout<<"this is no."<<i<<"tooth"<<endl;
                num = i;
            }
        }


        for(auto style : styles){
            num == -1?style->isMergedActor = true:style->isMergedActor = false;
            if(style!=this){

                style->setActor(num);
                style->generateBox2D();
                //cout<<style->selectedRenderer<<endl;
            }
        }
        emit myqtObject->refresh();
        generateBox2D();
        interactor->GetRenderWindow()->Render();
    }





}

void HandleInteractor::OnRightButtonDown() {

    if(selectedActor==nullptr){
        return;
    }
    selectedRenderer->ResetCamera();
    //selectedRenderer->Render();
    generateBox2D();
    selectedRenderer->GetRenderWindow()->Render();
}
void HandleInteractor::OnMiddleButtonDown()
{
    // isMiddleDown = true;
    // if (box != nullptr) {
    //     box->GetProperty()->SetColor(0.235, 0.7, 0.443);
    //     interactor->GetRenderWindow()->Render();

    //     vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    //     picker->SetTolerance(0.02);
    //     int* clickPos = interactor->GetLastEventPosition();
    //     lastPos[0] = clickPos[0];
    //     lastPos[1] = clickPos[1];
    // }
    vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
}
void HandleInteractor::OnMiddleButtonUp()
{
    // isMiddleDown = false;
    // if (box != nullptr) {
    //     box->GetProperty()->SetColor(0.39, 0.584, 0.93);
    //     interactor->GetRenderWindow()->Render();
    // }
    vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();
}

void HandleInteractor::OnMouseWheelForward()
{
    Dolly(1.1);
    //vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
    if (box != nullptr) {
        generateBox2D();
    }
    this->selectedRenderer->GetRenderWindow()->Render();
}

void HandleInteractor::OnMouseWheelBackward()
{
    Dolly(0.9);
    //vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
    if (box != nullptr) {
        generateBox2D();
    }
    this->selectedRenderer->GetRenderWindow()->Render();
}

void HandleInteractor::OnLeftButtonUp()
{
    isLeftDown = false;
    if (selectedHandle != nullptr) {
        selectedHandle->GetProperty()->SetColor(0.39, 0.584, 0.93);
        generateBox2D();
        interactor->GetRenderWindow()->Render();
        selectedHandle = nullptr;
    }

    isInRec = false;
    if (box != nullptr) {
        box->GetProperty()->SetColor(0.39, 0.584, 0.93);
        interactor->GetRenderWindow()->Render();
    }

    removeArrowHandle();
}

void HandleInteractor::OnRightButtonUp() {}
void HandleInteractor::OnMouseMove()
{
    interactor = this->GetInteractor();
    if (!interactor) {
        return;
    }
    interactor->GetRenderWindow()->Render();
    vtkRenderer* renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
    if (!renderer) {

        return;
    }
    this->selectedRenderer = renderer;
    int* clickPos = interactor->GetInteractorStyle()->GetInteractor()->GetLastEventPosition();
    if(!isLeftDown){
        if (!handles.empty()) {
            for (size_t i = 0; i < handles.size(); ++i) {
                double* pos = handles[i]->GetPosition();
                double distance = sqrt(pow(clickPos[0] - pos[0], 2) + pow(clickPos[1] - pos[1], 2));
                if (distance < 10) {
                    //selectedHandle = handles[i];
                    //handleID = i;
                    // vtkSmartPointer<vtkRegularPolygonSource> polygonSource = vtkRegularPolygonSource::SafeDownCast(selectedHandle->GetMapper()->GetInputConnection(0, 0)->GetProducer());
                    // if (polygonSource) {
                    //     polygonSource->SetRadius(10.0);  // 放大手柄的半径
                    //     selectedHandle->GetMapper()->Update();
                    // }
                    double *test;
                    test = handles[i]->GetPosition();
                    generateArrowHandle(test,i);
                    //selectedHandle->GetProperty()->SetColor(0.235, 0.7, 0.443);

                    interactor->GetRenderWindow()->Render();
                    return;
                }else{
                    //selectedHandle = nullptr;
                    removeArrowHandle();
                }
            }
        }
    }


    if (isLeftDown && selectedHandle != nullptr) {
        // Handle movement logic (you can implement it in a similar way as in the original code)

        vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
        picker->SetTolerance(0.01);
        int* clickPos = interactor->GetInteractorStyle()->GetInteractor()->GetLastEventPosition();

        switch (handleID) {
        case 0:{
            std::cout << "00000" << std::endl;
            double* pos4 = handles[4]->GetPosition();
            double* pos6 = handles[6]->GetPosition();
            double midPoint[2] = {(pos4[0] + pos6[0]) / 2.0, (pos4[1] + pos6[1]) / 2.0};

            // 修正currentVector的计算，使用clickPos的y坐标
            double currentVector[2] = {clickPos[0] - midPoint[0], clickPos[1] - midPoint[1]};
            double lastVector[2] = {lastPos[0] - midPoint[0], lastPos[1] - midPoint[1]};

            // 直接计算currentVector和lastVector的角度
            double currentAngle = atan2(currentVector[1], currentVector[0]);
            double lastAngle = atan2(lastVector[1], lastVector[0]);

            // 计算旋转角度
            double rotationAngle = currentAngle - lastAngle;

            vtkPolyData* polydata = vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->GetInput();
            vtkPoints* points = polydata->GetPoints();
            vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
            double center[3];
            polydata->GetCenter(center);
            transform->Translate(center);

            // 根据rendererID选择旋转轴
            if(rendererID == "Renderer_1"){
                transform->RotateZ(vtkMath::DegreesFromRadians(rotationAngle));
            } else if (rendererID == "Renderer_2") {
                if(sagittalView==0){
                    transform->RotateX(vtkMath::DegreesFromRadians(-rotationAngle));
                }else{
                    transform->RotateX(vtkMath::DegreesFromRadians(rotationAngle));
                }
            } else {
                transform->RotateY(vtkMath::DegreesFromRadians(-rotationAngle));
            }
            transform->Translate(-center[0], -center[1], -center[2]);

            for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
                double point[3];
                points->GetPoint(i, point);
                double transformedPoint[3];
                transform->TransformPoint(point, transformedPoint);
                points->SetPoint(i, transformedPoint);
            }

            points->Modified();
            selectedActor->GetMapper()->Update();
            this->lastPos[0] = clickPos[0];
            this->lastPos[1] = clickPos[1];
            break;
        }

        case 1:{
            std::cout<<"1111"<<std::endl;
            double* pos4 = handles[4]->GetPosition();
            double* pos6 = handles[6]->GetPosition();
            double midPoint[2] = {(pos4[0] + pos6[0]) / 2.0, (pos4[1] + pos6[1]) / 2.0};
            double currentVector[2] = {clickPos[0] - midPoint[0], clickPos[1] - midPoint[1]};
            double lastVector[2] = {lastPos[0] - midPoint[0], lastPos[1] - midPoint[1]};

            double currentAngle = atan2(currentVector[1], currentVector[0]);
            double lastAngle = atan2(lastVector[1], lastVector[0]);
            double rotationAngle = currentAngle - lastAngle;
            vtkPolyData* polydata = vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->GetInput();
            vtkPoints* points = polydata->GetPoints();
            vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
            double center[3];
            polydata->GetCenter(center);
            transform->Translate(center);
            if(rendererID=="Renderer_1"){
                transform->RotateZ(vtkMath::DegreesFromRadians(rotationAngle));
            }else if (rendererID=="Renderer_2") {
                if(sagittalView==0){
                    transform->RotateX(vtkMath::DegreesFromRadians(-rotationAngle));
                }else{
                    transform->RotateX(vtkMath::DegreesFromRadians(rotationAngle));
                }

            }else{

                transform->RotateY(vtkMath::DegreesFromRadians(-rotationAngle));
            }

            transform->Translate(-center[0], -center[1], -center[2]);

            for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
                double point[3];
                points->GetPoint(i, point);

                // 将点应用旋转变换
                double transformedPoint[3];
                transform->TransformPoint(point, transformedPoint);

                // 更新点的位置
                points->SetPoint(i, transformedPoint);
            }

            // 更新points和actor
            points->Modified();
            selectedActor->GetMapper()->Update();
            this->lastPos[0] = clickPos[0];
            this->lastPos[1] = clickPos[1];

            break;
        }
        case 2: {
            std::cout << "Handel 2 scaling..." << std::endl;

            // 获取当前鼠标位置
            int* clickPos = interactor->GetInteractorStyle()->GetInteractor()->GetLastEventPosition();
            double deltaX = clickPos[0] - lastPos[0];
            double deltaY = clickPos[1] - lastPos[1];

            // double* pos0 = handles[0]->GetPosition();
            // double xthreshold = pos0[0];
            // double ythreshold = pos0[1];
            // if((clickPos[0]-xthreshold>30)&&(clickPos[1]-ythreshold>30)){
            //     handles[2]->SetPosition()
            // }

            // 等比例缩放因子，以Y轴的变化为参考
            double scaleFactor = 1.0 + deltaY / 200.0;  // 控制缩放速度

            if (scaleFactor <= 0.0) {
                break;  // 避免缩放因子为负数或零
            }

            vtkPolyData* polydata = vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->GetInput();
            vtkPoints* points = polydata->GetPoints();

            // 获取当前物体的中心点
            double center[3];
            polydata->GetCenter(center);

            // 缩放所有的顶点
            for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
                double point[3];
                points->GetPoint(i, point);

                // 计算从中心点到当前点的向量
                point[0] = center[0] + (point[0] - center[0]) * scaleFactor;
                point[1] = center[1] + (point[1] - center[1]) * scaleFactor;
                point[2] = center[2] + (point[2] - center[2]) * scaleFactor;

                // 更新点的位置
                points->SetPoint(i, point);
            }

            points->Modified();
            //updataHandles(2);  // 调用更新手柄的位置
            //updataBox();       // 调用更新边框的形状
            selectedActor->GetMapper()->Update();
            this->lastPos[0] = clickPos[0];
            this->lastPos[1] = clickPos[1];
            break;
        }
        case 3: {
            std::cout << "Handel 3 scaling..." << std::endl;

            // 获取当前鼠标位置
            int* clickPos = interactor->GetInteractorStyle()->GetInteractor()->GetLastEventPosition();
            double deltaX = clickPos[0] - lastPos[0];
            double deltaY = clickPos[1] - lastPos[1];

            // 等比例缩放因子，以Y轴的变化为参考
            double scaleFactor = 1.0 + deltaY / 200.0;  // 控制缩放速度

            if (scaleFactor <= 0.0) {
                break;  // 避免缩放因子为负数或零
            }

            vtkPolyData* polydata = vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->GetInput();
            vtkPoints* points = polydata->GetPoints();

            // 获取当前物体的中心点
            double center[3];
            polydata->GetCenter(center);

            // 缩放所有的顶点
            for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
                double point[3];
                points->GetPoint(i, point);

                // 计算从中心点到当前点的向量
                point[0] = center[0] + (point[0] - center[0]) * scaleFactor;
                point[1] = center[1] + (point[1] - center[1]) * scaleFactor;
                point[2] = center[2] + (point[2] - center[2]) * scaleFactor;

                // 更新点的位置
                points->SetPoint(i, point);
            }

            points->Modified();
            selectedActor->GetMapper()->Update();
            this->lastPos[0] = clickPos[0];
            this->lastPos[1] = clickPos[1];
            break;
        }
        case 4:{
            std::cout<<"4444"<<std::endl;
            int pan = clickPos[1] ;
            double* pos4 = handles[4]->GetPosition();
            double* pos6 = handles[6]->GetPosition();
            double threshold = pos6[1];
            double x4 = pos4[0];
            if(threshold-pan>30){
                handles[4]->SetPosition(x4,pan);
                double *pos = handles[4]->GetPosition();
                generateArrowHandle(pos,4);
            }
            break;

        }
        case 5:{
            std::cout<<"55555"<<std::endl;
            int pan = clickPos[0] ;
            double* pos5 = handles[5]->GetPosition();
            double* pos7 = handles[7]->GetPosition();
            double threshold = pos7[0];
            double y5 = pos5[1];

            if(pan-threshold>30){
                handles[5]->SetPosition(pan,y5);
                double *pos = handles[5]->GetPosition();
                generateArrowHandle(pos,5);
            }
            break;
        }
        case 6:{
            std::cout<<"6666"<<std::endl;
            int pan = clickPos[1];
            double* pos4 = handles[4]->GetPosition();
            double* pos6 = handles[6]->GetPosition();
            double threshold = pos4[1];
            double x6 = pos6[0];
            if(pan - threshold>30){
                handles[6]->SetPosition(x6,pan);
                double *pos = handles[6]->GetPosition();
                generateArrowHandle(pos,6);
            }
            break;
        }
        case 7:{
            std::cout<<"7777"<<std::endl;
            int pan = clickPos[0] ;
            double* pos7 = handles[7]->GetPosition();
            double* pos5 = handles[5]->GetPosition();
            double threshold = pos5[0];
            double y7 = pos7[1];


            if(threshold-pan>30){
                handles[7]->SetPosition(pan,y7);
                double *pos = handles[7]->GetPosition();
                generateArrowHandle(pos,7);
            }

            break;
        }

        }
        updataHandles(handleID);
        updataBox();
        updataActor(handleID);
        int num = -1;
        for(int i = 0; i < toothList.size(); i++){
            if(toothList[i]==selectedActor){
                cout<<"this is no."<<i<<"tooth"<<endl;
                num = i;
            }
        }

        for(auto style : styles){
            if(style!=this){
                style->setActor(num);
                style->generateBox2D();
                //cout<<style->selectedRenderer<<endl;
            }
        }
        emit myqtObject->refresh();
    }

    if (isInRec && box != nullptr) {
        // Handle panning logic (you can implement it in a similar way as in the original code)
        interactor = this->GetInteractor();
        if (!interactor) {
            return;
        }
        interactor->GetRenderWindow()->Render();
        vtkRenderer* renderer = interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
        if (!renderer) {

            return;
        }
        this->selectedRenderer = renderer;
        vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
        picker->SetTolerance(0.01);
        int* clickPos = interactor->GetInteractorStyle()->GetInteractor()->GetLastEventPosition();
        double offsetX = clickPos[0] -lastPos[0];
        double offsetY = clickPos[1] -lastPos[1];
        for(auto handel :handles){
            double *pos = handel->GetPosition();
            pos[0] = pos[0]+offsetX;
            pos[1] = pos[1]+offsetY;
            handel->SetPosition(pos);

        }
        updataBox();

        double worldOffsetStart[4], worldOffsetEnd[4];

        // 获取当前鼠标位置对应的世界坐标
        selectedRenderer->SetDisplayPoint(clickPos[0], clickPos[1], 0);
        selectedRenderer->DisplayToWorld();
        selectedRenderer->GetWorldPoint(worldOffsetStart);

        // 获取上一帧鼠标位置对应的世界坐标
        selectedRenderer->SetDisplayPoint(lastPos[0], lastPos[1], 0);
        selectedRenderer->DisplayToWorld();
        selectedRenderer->GetWorldPoint(worldOffsetEnd);

        // 计算世界坐标系中的偏移量
        double worldOffset[3];
        worldOffset[0] = worldOffsetStart[0] - worldOffsetEnd[0];
        worldOffset[1] = worldOffsetStart[1] - worldOffsetEnd[1];
        worldOffset[2] = worldOffsetStart[2] - worldOffsetEnd[2];

        vtkPolyData* polydata = vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->GetInput();
        vtkPoints* points = polydata->GetPoints();

        // 应用平移到每个点
        for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
            double point[3];
            points->GetPoint(i, point);

            // 平移点
            point[0] += worldOffset[0];
            point[1] += worldOffset[1];
            point[2] += worldOffset[2];

            // 更新点的位置
            points->SetPoint(i, point);
        }

        points->Modified();
        selectedActor->GetMapper()->Update();

        lastPos[0] = clickPos[0];
        lastPos[1] = clickPos[1];

        int num = -1;
        for(int i = 0; i < toothList.size(); i++){
            if(toothList[i]==selectedActor){
                cout<<"this is no."<<i<<"tooth"<<endl;
                num = i;
            }
        }

        for(auto style : styles){
            if(style!=this){
                style->setActor(num);
                style->generateBox2D();
                //cout<<style->selectedRenderer<<endl;
            }
        }
        emit myqtObject->refresh();


    }
}

void HandleInteractor::generateBox2D()
{
    // The logic of generating the 2D box remains the same as in the original code
    // for (auto& style : styles) {
    //     style->removeBox();
    // }
    this->removeBox();
    double bounds[6];
    this->selectedActor->GetBounds(bounds);
    double xMin = bounds[0];
    double xMax = bounds[1];
    double yMin = bounds[2];
    double yMax = bounds[3];
    double zMin = bounds[4];
    double zMax = bounds[5];
    std::vector<std::vector<double>> points(8, std::vector<double>(3));
    points[0] = {xMin, yMin, zMin};
    points[1] = {xMax, yMin, zMin};
    points[2] = {xMax, yMax, zMin};
    points[3] = {xMin, yMax, zMin};
    points[4] = {xMin, yMin, zMax};
    points[5] = {xMax, yMin, zMax};
    points[6] = {xMax, yMax, zMax};
    points[7] = {xMin, yMax, zMax};

    std::vector<std::vector<double>> vectorZMax = {points[4],points[5],points[6],points[7]} ;
    std::vector<std::vector<double>> vectorXMin = {points[4],points[0],points[3],points[7]} ;
    std::vector<std::vector<double>> vectorXMax = {points[1],points[5],points[6],points[2]} ;
    std::vector<std::vector<double>> vectorYMax = {points[2],points[3],points[7],points[6]} ;
    std::vector<std::vector<double>> vectorYMin = {points[0],points[1],points[5],points[4]} ;

    double scaleFactor = 1.05;
    vtkSmartPointer<vtkPoints> screenPoints = vtkSmartPointer<vtkPoints>::New();
    std::vector<std::vector<double>> displayPoints;

    for (int i = 0; i < 4; ++i) {
        vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
        coordinate->SetCoordinateSystemToWorld();

        if(rendererID=="Renderer_1"){

            coordinate->SetValue(vectorZMax[i][0],vectorZMax[i][1], vectorZMax[i][2]);
        }else if (rendererID=="Renderer_2") {
            if(sagittalView==0){
                coordinate->SetValue(vectorXMax[i][0],vectorXMax[i][1], vectorXMax[i][2]);
            }else{
                coordinate->SetValue(vectorXMin[i][0],vectorXMin[i][1], vectorXMin[i][2]);
            }



        }else if (rendererID=="Renderer_3"){

            coordinate->SetValue(vectorYMin[i][0],vectorYMin[i][1], vectorYMin[i][2]);

        }
        // 获取屏幕坐标
        int* displayCoord = coordinate->GetComputedDisplayValue(selectedRenderer);

        // 存储屏幕坐标
        std::vector<double> screenPoint = {static_cast<double>(displayCoord[0]), static_cast<double>(displayCoord[1])};

        displayPoints.push_back(screenPoint);
        screenPoints->InsertNextPoint(screenPoint[0], screenPoint[1], 0.0);

    }


    double centerX = 0.0, centerY = 0.0;
    for (const auto& point : displayPoints) {
        centerX += point[0];
        centerY += point[1];
    }
    centerX /= 4.0;
    centerY /= 4.0;


    vtkSmartPointer<vtkPoints> scaledPoints = vtkSmartPointer<vtkPoints>::New();
    for (const auto& point : displayPoints) {
        double newX = centerX + (point[0] - centerX) * scaleFactor;
        double newY = centerY + (point[1] - centerY) * scaleFactor;
        scaledPoints->InsertNextPoint(newX, newY, 0.0);
    }
    vtkSmartPointer<vtkCellArray> rectangle = vtkSmartPointer<vtkCellArray>::New();
    rectangle->InsertNextCell(5);
    rectangle->InsertCellPoint(0);
    rectangle->InsertCellPoint(1);
    rectangle->InsertCellPoint(2);
    rectangle->InsertCellPoint(3);
    rectangle->InsertCellPoint(0);
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(scaledPoints);
    polyData->SetLines(rectangle);
    vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    mapper->SetInputData(polyData);
    box = vtkSmartPointer<vtkActor2D>::New();
    box->SetMapper(mapper);
    box->GetProperty()->SetColor(0.39, 0.584, 0.93);
    box->GetProperty()->SetLineWidth(2);
    selectedRenderer->AddActor(box);


    std::vector<std::vector<double>> vertexPoints(4, std::vector<double>(2));
    std::vector<std::vector<double>> midPoints(4, std::vector<double>(2));

    // 获取放大后的矩形顶点
    for (int i = 0; i < 4; ++i) {
        double* point = scaledPoints->GetPoint(i);
        vertexPoints[i] = {point[0], point[1]};
    }

    // 获取边中点
    midPoints[0] = {(vertexPoints[0][0] + vertexPoints[1][0]) / 2,
                    (vertexPoints[0][1] + vertexPoints[1][1]) / 2}; // Bottom center
    midPoints[1] = {(vertexPoints[1][0] + vertexPoints[2][0]) / 2,
                    (vertexPoints[1][1] + vertexPoints[2][1]) / 2}; // Right center
    midPoints[2] = {(vertexPoints[2][0] + vertexPoints[3][0]) / 2,
                    (vertexPoints[2][1] + vertexPoints[3][1]) / 2}; // Top center
    midPoints[3] = {(vertexPoints[3][0] + vertexPoints[0][0]) / 2,
                    (vertexPoints[3][1] + vertexPoints[0][1]) / 2}; // Left center
    lastWidth = midPoints[1][0]-midPoints[3][0];
    lastHeight = midPoints[2][1] - midPoints[0][1];
    handles.clear();

    //生成顶点句柄
    for (const auto& vertexP : vertexPoints) {

        vtkSmartPointer<vtkActor2D> handle =   generateCircleHandle(vertexP);
        handles.push_back(handle);
        selectedRenderer->AddActor(handle);
    }

    // 生成边中点句柄
    for (const auto& midP : midPoints) {
        vtkSmartPointer<vtkActor2D> handle =   generateCircleHandle(midP);
        handles.push_back(handle);
        selectedRenderer->AddActor(handle);
        selectedRenderer->Render();
    }
    handlesPoints.clear();
    handlesPoints.insert(handlesPoints.end(), vertexPoints.begin(), vertexPoints.end());

    handlesPoints.insert(handlesPoints.end(), midPoints.begin(), midPoints.end());






    //generateArrowHandle(handlePos);
    //renderer->SetBackground(1, 1, 1);  // 设置白色背景


}

vtkSmartPointer<vtkActor2D> HandleInteractor::generateCircleHandle(const std::vector<double>& screenPoint)
{
    // The logic for generating circular handles remains the same
    vtkSmartPointer<vtkRegularPolygonSource> polygonSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource->SetCenter(0.0,0.0, 0.0);
    polygonSource->SetRadius(5.0);
    polygonSource->SetNumberOfSides(200);

    vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    mapper->SetInputConnection(polygonSource->GetOutputPort());

    vtkSmartPointer<vtkActor2D> circleActor = vtkSmartPointer<vtkActor2D>::New();
    circleActor->SetMapper(mapper);
    circleActor->GetProperty()->SetColor(0.39, 0.584, 0.93);
    circleActor->SetPosition(screenPoint[0], screenPoint[1]);
    // cout<<screenPoint[0]<<" "<<screenPoint[1]<<endl;
    return circleActor;
}

void HandleInteractor::updataBox()
{
    // Update the box based on new positions
    vtkSmartPointer<vtkPoints> scaledPoints = vtkSmartPointer<vtkPoints>::New();
    double* pos0 = handles[0]->GetPosition();
    double* pos1 = handles[1]->GetPosition();
    double* pos2 = handles[2]->GetPosition();
    double* pos3 = handles[3]->GetPosition();
    scaledPoints->InsertNextPoint(pos0[0], pos0[1], 0.0);
    scaledPoints->InsertNextPoint(pos1[0], pos1[1], 0.0);
    scaledPoints->InsertNextPoint(pos2[0], pos2[1], 0.0);
    scaledPoints->InsertNextPoint(pos3[0], pos3[1], 0.0);
    vtkSmartPointer<vtkCellArray> rectangle = vtkSmartPointer<vtkCellArray>::New();
    rectangle->InsertNextCell(5);
    rectangle->InsertCellPoint(0);
    rectangle->InsertCellPoint(1);
    rectangle->InsertCellPoint(2);
    rectangle->InsertCellPoint(3);
    rectangle->InsertCellPoint(0);
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(scaledPoints);
    polyData->SetLines(rectangle);
    vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    mapper->SetInputData(polyData);
    box->SetMapper(mapper);
}

void HandleInteractor::updataHandles(int id)
{
    // Update handles based on their IDs
    switch (id) {

    case 0:{



        break;
    }


    case 4:{

        double* pos4 = handles[4]->GetPosition();
        double y = pos4[1];
        double* pos0 = handles[0]->GetPosition();
        double* pos1 = handles[1]->GetPosition();
        handles[0]->SetPosition(pos0[0],y);
        handles[1]->SetPosition(pos1[0],y);
        double* pos2 = handles[2]->GetPosition();
        double* pos3 = handles[3]->GetPosition();

        handles[5]->SetPosition(pos1[0],(pos1[1]+pos2[1])/2);
        handles[7]->SetPosition(pos0[0],(pos0[1]+pos3[1])/2);


        break;
    }
    case 5:{
        double* pos5 = handles[5]->GetPosition();
        double x = pos5[0];
        double* pos1 = handles[1]->GetPosition();
        double* pos2 = handles[2]->GetPosition();
        handles[1]->SetPosition(x,pos1[1]);
        handles[2]->SetPosition(x,pos2[1]);

        double* pos0 = handles[0]->GetPosition();
        double* pos3 = handles[3]->GetPosition();
        handles[4]->SetPosition((pos0[0]+pos1[0])/2,pos0[1]);
        handles[6]->SetPosition((pos3[0]+pos2[0])/2,pos3[1]);

        break;
    }
    case 6:{
        double* pos6 = handles[6]->GetPosition();
        double y = pos6[1];
        double* pos2 = handles[2]->GetPosition();
        double* pos3 = handles[3]->GetPosition();
        handles[2]->SetPosition(pos2[0],y);
        handles[3]->SetPosition(pos3[0],y);
        double* pos0 = handles[0]->GetPosition();
        double* pos1 = handles[1]->GetPosition();
        handles[5]->SetPosition(pos1[0],(pos1[1]+pos2[1])/2);
        handles[7]->SetPosition(pos0[0],(pos0[1]+pos3[1])/2);

        break;
    }
    case 7:{
        double* pos7 = handles[7]->GetPosition();
        double x = pos7[0];
        double* pos0 = handles[0]->GetPosition();
        double* pos3 = handles[3]->GetPosition();
        handles[0]->SetPosition(x,pos0[1]);
        handles[3]->SetPosition(x,pos3[1]);

        double* pos1 = handles[1]->GetPosition();
        double* pos2 = handles[2]->GetPosition();
        handles[4]->SetPosition((pos0[0]+pos1[0])/2,pos0[1]);
        handles[6]->SetPosition((pos3[0]+pos2[0])/2,pos3[1]);

        break;
    }
    }
}

void HandleInteractor::updataActor(int id)
{
    // Update the actor based on the handle interactions
    switch (id) {
    case 0:{


        break;
    }
    case 1:{

        break;
    }
    case 2:{
        break;
    }
    case 3:{
        break;
    }
    case 4:{
        double* pos6 = handles[6]->GetPosition();
        double* pos4 = handles[4]->GetPosition();

        double newHeight = pos6[1] - pos4[1];
        double lastHeight = handlesPoints[6][1] - handlesPoints[4][1];

        vtkPolyData* polydata = vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->GetInput();
        vtkPoints* points = polydata->GetPoints();
        vtkSmartPointer<vtkMatrix4x4> worldToProjectionMatrix = selectedRenderer->GetActiveCamera()->GetCompositeProjectionTransformMatrix(selectedRenderer->GetTiledAspectRatio(), 0, 1);
        vtkSmartPointer<vtkMatrix4x4> projectionToWorldMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        vtkMatrix4x4::Invert(worldToProjectionMatrix, projectionToWorldMatrix);

        int windowWidth = selectedRenderer->GetRenderWindow()->GetSize()[0];
        int windowHeight = selectedRenderer->GetRenderWindow()->GetSize()[1];

        for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
            double point[4];
            points->GetPoint(i, point);
            point[3] = 1.0;
            double projectedPoint[4];
            worldToProjectionMatrix->MultiplyPoint(point, projectedPoint);

            if (projectedPoint[3] != 0.0) {
                projectedPoint[0] /= projectedPoint[3];
                projectedPoint[1] /= projectedPoint[3];
                projectedPoint[2] /= projectedPoint[3];
            }

            projectedPoint[0] = ((projectedPoint[0] + 1.0) * windowWidth / 2.0);
            projectedPoint[1] = ((projectedPoint[1] + 1.0) * windowHeight / 2.0);
            projectedPoint[2] = (projectedPoint[2] + 1.0) * 0.5;




            projectedPoint[1] = pos6[1] - ((pos6[1] - projectedPoint[1]) / lastHeight) * newHeight;

            double ndcX = (2.0 * projectedPoint[0] / windowWidth) - 1.0;
            double ndcY = (2.0 * projectedPoint[1] / windowHeight) - 1.0;
            double ndcZ = (projectedPoint[2] / 0.5) - 1.0;

            double homogenousPoint[4] = { ndcX, ndcY, ndcZ, 1.0 };
            double worldPoint[4];
            projectionToWorldMatrix->MultiplyPoint(homogenousPoint, worldPoint);

            if (worldPoint[3] != 0.0) {
                worldPoint[0] /= worldPoint[3];
                worldPoint[1] /= worldPoint[3];
                worldPoint[2] /= worldPoint[3];
            }

            points->SetPoint(i, worldPoint[0], worldPoint[1], worldPoint[2]);
        }
        points->Modified();
        selectedActor->GetMapper()->Update();
        handlesPoints[6][1] = pos6[1];
        handlesPoints[4][1] = pos4[1];
        //            lastHeight = newHeight;


        break;
    }
    case 5:{

        double* pos5 = handles[5]->GetPosition();
        double* pos7 = handles[7]->GetPosition();
        double newWidth = pos5[0] - pos7[0];
        double lastWidth = handlesPoints[5][0]-handlesPoints[7][0];
        vtkPolyData* polydata = vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->GetInput();
        vtkPoints* points = polydata->GetPoints();

        vtkSmartPointer<vtkMatrix4x4> worldToProjectionMatrix = selectedRenderer->GetActiveCamera()->GetCompositeProjectionTransformMatrix(selectedRenderer->GetTiledAspectRatio(), 0, 1);
        vtkSmartPointer<vtkMatrix4x4> projectionToWorldMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        vtkMatrix4x4::Invert(worldToProjectionMatrix, projectionToWorldMatrix);

        int windowWidth = selectedRenderer->GetRenderWindow()->GetSize()[0];
        int windowHeight = selectedRenderer->GetRenderWindow()->GetSize()[1];
        for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
            double point[4];
            points->GetPoint(i, point);
            point[3] = 1.0;
            double projectedPoint[4];
            worldToProjectionMatrix->MultiplyPoint(point, projectedPoint);
            if (projectedPoint[3] != 0.0) {
                projectedPoint[0] /= projectedPoint[3];
                projectedPoint[1] /= projectedPoint[3];
                projectedPoint[2] /= projectedPoint[3];
            }

            projectedPoint[0] = ((projectedPoint[0] + 1.0) * windowWidth / 2.0);
            projectedPoint[1] = ((projectedPoint[1] + 1.0) * windowHeight / 2.0);
            projectedPoint[2] = (projectedPoint[2] + 1.0) * 0.5;

            projectedPoint[0] = ((projectedPoint[0] - pos7[0]) / lastWidth) * newWidth + pos7[0];
            double ndcX = (2.0 * projectedPoint[0] / windowWidth) - 1.0;
            double ndcY = (2.0 * projectedPoint[1] / windowHeight) - 1.0;
            double ndcZ = (projectedPoint[2] / 0.5) - 1.0;

            double homogenousPoint[4] = { ndcX, ndcY, ndcZ, 1.0 };
            double worldPoint[4];
            projectionToWorldMatrix->MultiplyPoint(homogenousPoint, worldPoint);

            if (worldPoint[3] != 0.0) {
                worldPoint[0] /= worldPoint[3];
                worldPoint[1] /= worldPoint[3];
                worldPoint[2] /= worldPoint[3];
            }

            points->SetPoint(i, worldPoint[0], worldPoint[1], worldPoint[2]);
        }
        points->Modified();
        selectedActor->GetMapper()->Update();
        handlesPoints[5][0] = pos5[0];
        handlesPoints[7][0] = pos7[0];
        //            lastWidth = newWidth;
        break;
    }
    case 6:{
        double* pos6 = handles[6]->GetPosition();
        double* pos4 = handles[4]->GetPosition();
        double newHeight = pos6[1] - pos4[1];
        double lastHeight = handlesPoints[6][1] -handlesPoints[4][1];
        vtkPolyData* polydata = vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->GetInput();
        vtkPoints* points = polydata->GetPoints();

        vtkSmartPointer<vtkMatrix4x4> worldToProjectionMatrix = selectedRenderer->GetActiveCamera()->GetCompositeProjectionTransformMatrix(selectedRenderer->GetTiledAspectRatio(), 0, 1);
        vtkSmartPointer<vtkMatrix4x4> projectionToWorldMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        vtkMatrix4x4::Invert(worldToProjectionMatrix, projectionToWorldMatrix);

        int windowWidth = selectedRenderer->GetRenderWindow()->GetSize()[0];
        int windowHeight = selectedRenderer->GetRenderWindow()->GetSize()[1];
        for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
            double point[4];
            points->GetPoint(i, point);
            point[3] = 1.0;
            double projectedPoint[4];
            worldToProjectionMatrix->MultiplyPoint(point, projectedPoint);
            if (projectedPoint[3] != 0.0) {
                projectedPoint[0] /= projectedPoint[3];
                projectedPoint[1] /= projectedPoint[3];
                projectedPoint[2] /= projectedPoint[3];
            }

            projectedPoint[0] = ((projectedPoint[0] + 1.0) * windowWidth / 2.0);
            projectedPoint[1] = ((projectedPoint[1] + 1.0) * windowHeight / 2.0);
            projectedPoint[2] = (projectedPoint[2] + 1.0) * 0.5;

            projectedPoint[1] = pos4[1] + ((projectedPoint[1] - pos4[1]) / lastHeight) * newHeight;
            double ndcX = (2.0 * projectedPoint[0] / windowWidth) - 1.0;
            double ndcY = (2.0 * projectedPoint[1] / windowHeight) - 1.0;
            double ndcZ = (projectedPoint[2] / 0.5) - 1.0;

            double homogenousPoint[4] = { ndcX, ndcY, ndcZ, 1.0 };
            double worldPoint[4];
            projectionToWorldMatrix->MultiplyPoint(homogenousPoint, worldPoint);

            if (worldPoint[3] != 0.0) {
                worldPoint[0] /= worldPoint[3];
                worldPoint[1] /= worldPoint[3];
                worldPoint[2] /= worldPoint[3];
            }

            points->SetPoint(i, worldPoint[0], worldPoint[1], worldPoint[2]);}
        points->Modified();
        selectedActor->GetMapper()->Update();
        handlesPoints[6][1] = pos6[1];
        handlesPoints[4][1] = pos4[1];
        //            lastHeight = newHeight;
        break;
    }
    case 7:{


        double* pos5 = handles[5]->GetPosition();
        double* pos7 = handles[7]->GetPosition();

        double newWidth = pos5[0] - pos7[0];
        double lastWidth = handlesPoints[5][0] - handlesPoints[7][0];
        vtkPolyData* polydata = vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->GetInput();
        vtkPoints* points = polydata->GetPoints();

        vtkSmartPointer<vtkMatrix4x4> worldToProjectionMatrix = selectedRenderer->GetActiveCamera()->GetCompositeProjectionTransformMatrix(selectedRenderer->GetTiledAspectRatio(), 0, 1);
        vtkSmartPointer<vtkMatrix4x4> projectionToWorldMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        vtkMatrix4x4::Invert(worldToProjectionMatrix, projectionToWorldMatrix);

        int windowWidth = selectedRenderer->GetRenderWindow()->GetSize()[0];
        int windowHeight = selectedRenderer->GetRenderWindow()->GetSize()[1];
        for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
            double point[4];
            points->GetPoint(i, point);
            point[3] = 1.0;
            double projectedPoint[4];
            worldToProjectionMatrix->MultiplyPoint(point, projectedPoint);
            if (projectedPoint[3] != 0.0) {
                projectedPoint[0] /= projectedPoint[3];
                projectedPoint[1] /= projectedPoint[3];
                projectedPoint[2] /= projectedPoint[3];
            }

            projectedPoint[0] = ((projectedPoint[0] + 1.0) * windowWidth / 2.0);
            projectedPoint[1] = ((projectedPoint[1] + 1.0) * windowHeight / 2.0);
            projectedPoint[2] = (projectedPoint[2] + 1.0) * 0.5;

            projectedPoint[0] = pos5[0] - ((pos5[0] - projectedPoint[0]) / lastWidth) * newWidth;
            double ndcX = (2.0 * projectedPoint[0] / windowWidth) - 1.0;
            double ndcY = (2.0 * projectedPoint[1] / windowHeight) - 1.0;
            double ndcZ = (projectedPoint[2] / 0.5) - 1.0;

            double homogenousPoint[4] = { ndcX, ndcY, ndcZ, 1.0 };
            double worldPoint[4];
            projectionToWorldMatrix->MultiplyPoint(homogenousPoint, worldPoint);

            if (worldPoint[3] != 0.0) {
                worldPoint[0] /= worldPoint[3];
                worldPoint[1] /= worldPoint[3];
                worldPoint[2] /= worldPoint[3];
            }

            points->SetPoint(i, worldPoint[0], worldPoint[1], worldPoint[2]);}


        points->Modified();
        vtkPolyDataMapper::SafeDownCast(selectedActor->GetMapper())->Update();
        handlesPoints[5][0] = pos5[0];
        handlesPoints[7][0] = pos7[0];
        //            lastWidth = newWidth;
        break;
    }
    }
}

void HandleInteractor::removeBox()
{
    if (selectedRenderer != nullptr) {
        if(box){
            selectedRenderer->RemoveActor(box);
            box = nullptr;
            for (auto& handle : handles) {
                selectedRenderer->RemoveActor(handle);
            }
            handles.clear();
        }


        selectedRenderer->GetRenderWindow()->Render();
    }
}

void HandleInteractor::setqtObject(MyQtObject* object)
{
    this->myqtObject = object;
}



void HandleInteractor::setRender(vtkSmartPointer<vtkRenderer> render){
    this->selectedRenderer = render;
    vtkInformation* info = selectedRenderer->GetInformation();
    //cout<<info<<endl;
    this->rendererID = info->Get(RendererIDKey);
}
void HandleInteractor::setActor(int num){


    if(isMergedActor){
        this->selectedActor = mergedActor;
    }else{
        this->selectedActor = this->toothList[num];
    }

}
void HandleInteractor::Dolly(double factor)
{
    DollyToPosition(factor, this->Interactor->GetEventPosition(), selectedRenderer);

    // 更新相机裁剪范围，确保图像正确显示
    selectedRenderer->ResetCameraClippingRange();

    // 重新渲染窗口
    this->selectedRenderer->GetRenderWindow()->Render();
}

void HandleInteractor::DollyToPosition(double factor, int* position, vtkRenderer* renderer)
{
    vtkCamera* cam = renderer->GetActiveCamera();
    if (cam->GetParallelProjection())
    {
        // Compute the world coordinates of the mouse position before zooming
        double mouseWorldCoordsBefore[4];
        ComputeDisplayToWorld(renderer, position[0], position[1], 0.0, mouseWorldCoordsBefore);

        // Zoom by adjusting the parallel scale
        cam->SetParallelScale(cam->GetParallelScale() / factor);

        // Compute the world coordinates of the mouse position after zooming
        double mouseWorldCoordsAfter[4];
        ComputeDisplayToWorld(renderer, position[0], position[1], 0.0, mouseWorldCoordsAfter);

        // Calculate the translation required to keep the mouse position stationary
        double delta[3];
        delta[0] = mouseWorldCoordsBefore[0] - mouseWorldCoordsAfter[0];
        delta[1] = mouseWorldCoordsBefore[1] - mouseWorldCoordsAfter[1];
        delta[2] = mouseWorldCoordsBefore[2] - mouseWorldCoordsAfter[2];

        // Adjust the camera's position and focal point
        cam->SetPosition(
            cam->GetPosition()[0] + delta[0],
            cam->GetPosition()[1] + delta[1],
            cam->GetPosition()[2] + delta[2]);

        cam->SetFocalPoint(
            cam->GetFocalPoint()[0] + delta[0],
            cam->GetFocalPoint()[1] + delta[1],
            cam->GetFocalPoint()[2] + delta[2]);
    }
    else
    {
        // 处理透视投影的情况
        double viewFocus[4], originalViewFocus[3], cameraPos[3], newCameraPos[3];
        double newFocalPoint[4], norm[3];

        // 获取相机位置和焦点
        cam->GetPosition(cameraPos);
        cam->GetFocalPoint(viewFocus);
        cam->GetFocalPoint(originalViewFocus);
        cam->GetViewPlaneNormal(norm);

        // 将焦点转换为屏幕坐标
        ComputeWorldToDisplay(renderer, viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);

        // 将鼠标位置从屏幕坐标转换为世界坐标，得到新的焦点
        ComputeDisplayToWorld(renderer, double(position[0]), double(position[1]), viewFocus[2], newFocalPoint);

        // 设置新的焦点
        cam->SetFocalPoint(newFocalPoint);

        // 缩放相机
        cam->Dolly(factor);

        // 调整相机位置，保持相机与焦点的相对位置
        cam->GetPosition(newCameraPos);

        double newPoint[3];
        newPoint[0] = originalViewFocus[0] + newCameraPos[0] - cameraPos[0];
        newPoint[1] = originalViewFocus[1] + newCameraPos[1] - cameraPos[1];
        newPoint[2] = originalViewFocus[2] ;

        cam->SetFocalPoint(newPoint);
    }
}

void HandleInteractor::TranslateCamera(vtkRenderer* renderer, int fromX, int fromY, int toX, int toY)
{
    vtkCamera* cam = renderer->GetActiveCamera();
    double viewFocus[4], focalDepth;
    double newPickPoint[4], oldPickPoint[4], motionVector[3];

    // 获取焦点位置
    cam->GetFocalPoint(viewFocus);

    // 将焦点转换为屏幕坐标，获取深度值
    ComputeWorldToDisplay(renderer, viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
    focalDepth = viewFocus[2];

    // 将屏幕坐标转换为世界坐标，得到新的拾取点
    ComputeDisplayToWorld(renderer, double(toX), double(toY), focalDepth, newPickPoint);
    ComputeDisplayToWorld(renderer, double(fromX), double(fromY), focalDepth, oldPickPoint);

    // 计算移动向量
    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
    motionVector[2] = oldPickPoint[2] - newPickPoint[2];

    // 更新相机位置和焦点
    cam->SetFocalPoint(
        cam->GetFocalPoint()[0] + motionVector[0],
        cam->GetFocalPoint()[1] + motionVector[1],
        cam->GetFocalPoint()[2] + motionVector[2]);

    cam->SetPosition(
        cam->GetPosition()[0] + motionVector[0],
        cam->GetPosition()[1] + motionVector[1],
        cam->GetPosition()[2] + motionVector[2]);
}

void HandleInteractor::ComputeWorldToDisplay(vtkRenderer* renderer, double x, double y, double z, double displayCoords[4])
{
    renderer->SetWorldPoint(x, y, z, 1.0);
    renderer->WorldToDisplay();
    renderer->GetDisplayPoint(displayCoords);
}

void HandleInteractor::ComputeDisplayToWorld(vtkRenderer* renderer, double x, double y, double z, double worldCoords[4])
{
    renderer->SetDisplayPoint(x, y, z);
    renderer->DisplayToWorld();
    renderer->GetWorldPoint(worldCoords);
    if (worldCoords[3] != 0.0)
    {
        worldCoords[0] /= worldCoords[3];
        worldCoords[1] /= worldCoords[3];
        worldCoords[2] /= worldCoords[3];
        worldCoords[3] = 1.0;
    }
}

void HandleInteractor::generateArrowHandle(double* selectPos, int num) {

    removeArrowHandle();
    vtkSmartPointer<vtkPoints> points1 = vtkSmartPointer<vtkPoints>::New();

    // Define control points based on the 'num' parameter
    switch (num) {
    case 0: {
        points1->InsertNextPoint(selectPos[0] - 10, selectPos[1] + 20, 0.0);
        points1->InsertNextPoint(selectPos[0], selectPos[1], 0.0);
        points1->InsertNextPoint(selectPos[0] + 20, selectPos[1] - 10, 0.0);
        break;
    }
    case 1: {
        points1->InsertNextPoint(selectPos[0] - 20, selectPos[1] - 10, 0.0);
        points1->InsertNextPoint(selectPos[0], selectPos[1], 0.0);
        points1->InsertNextPoint(selectPos[0] + 10, selectPos[1] + 20, 0.0);
        break;
    }
    case 2: {
        points1->InsertNextPoint(selectPos[0] - 15, selectPos[1] - 15, 0.0);
        points1->InsertNextPoint(selectPos[0], selectPos[1], 0.0);
        points1->InsertNextPoint(selectPos[0] + 15, selectPos[1] + 15, 0.0);
        break;
    }
    case 3: {
        points1->InsertNextPoint(selectPos[0] - 15, selectPos[1] + 15, 0.0);
        points1->InsertNextPoint(selectPos[0], selectPos[1], 0.0);
        points1->InsertNextPoint(selectPos[0] + 15, selectPos[1] - 15, 0.0);
        break;
    }
    case 4: {
        points1->InsertNextPoint(selectPos[0] , selectPos[1] - 20, 0.0);
        points1->InsertNextPoint(selectPos[0], selectPos[1], 0.0);
        points1->InsertNextPoint(selectPos[0] , selectPos[1] + 20, 0.0);
        break;
    }
    case 5: {
        points1->InsertNextPoint(selectPos[0] -20, selectPos[1], 0.0);
        points1->InsertNextPoint(selectPos[0], selectPos[1], 0.0);
        points1->InsertNextPoint(selectPos[0] +20, selectPos[1] , 0.0);
        break;
    }
    case 6: {
        points1->InsertNextPoint(selectPos[0] , selectPos[1] - 20, 0.0);
        points1->InsertNextPoint(selectPos[0], selectPos[1], 0.0);
        points1->InsertNextPoint(selectPos[0] , selectPos[1] + 20, 0.0);
        break;
    }
    case 7: {
        points1->InsertNextPoint(selectPos[0] - 20, selectPos[1] , 0.0);
        points1->InsertNextPoint(selectPos[0], selectPos[1], 0.0);
        points1->InsertNextPoint(selectPos[0] + 20, selectPos[1] , 0.0);
        break;
    }
    }


    // Create a spline curve from the control points
    vtkSmartPointer<vtkParametricSpline> spline = vtkSmartPointer<vtkParametricSpline>::New();
    spline->SetPoints(points1);

    vtkSmartPointer<vtkParametricFunctionSource> splineSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
    splineSource->SetParametricFunction(spline);
    splineSource->Update();

    // Mapper and actor for the spline curve
    vtkSmartPointer<vtkPolyDataMapper2D> splineMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
    splineMapper->SetInputData(splineSource->GetOutput());

    splineActor = vtkSmartPointer<vtkActor2D>::New();
    splineActor->SetMapper(splineMapper);
    splineActor->GetProperty()->SetColor(246/255.0, 198/255.0, 89/255.0);  // Blue color
    splineActor->GetProperty()->SetLineWidth(3.0);

    // Helper function to create arrowheads
    auto CreateArrowHead = [](vtkPoints* points, int index1, int index2, bool reverseDirection) {
        double pArrow[3], pAdj[3], tangent[3];

        points->GetPoint(index1, pArrow);
        points->GetPoint(index2, pAdj);

        // Compute the tangent vector
        vtkMath::Subtract(pAdj, pArrow, tangent);  // tangent = pAdj - pArrow
        vtkMath::Normalize(tangent);
        double angle = vtkMath::DegreesFromRadians(atan2(tangent[1], tangent[0]));

        // Adjust the angle for reverse direction if needed
        if (reverseDirection) {
            angle += 180.0;
        }

        // Adjust the initial orientation of the triangle
        double initialRotation = -90.0;  // Rotate triangle so apex points along positive X-axis

        // Create the arrowhead geometry
        vtkSmartPointer<vtkRegularPolygonSource> arrowHead = vtkSmartPointer<vtkRegularPolygonSource>::New();
        arrowHead->SetNumberOfSides(3);  // Triangle
        arrowHead->SetRadius(8.0);
        arrowHead->Update();

        // Apply transformations: rotate, then translate
        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->PostMultiply();  // Transformations will be applied in the order specified
        transform->RotateZ(initialRotation + angle);  // Rotate to align with tangent
        transform->Translate(pArrow);                 // Move to pArrow

        vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transformFilter->SetTransform(transform);
        transformFilter->SetInputConnection(arrowHead->GetOutputPort());
        transformFilter->Update();

        // Mapper and actor for the arrowhead
        vtkSmartPointer<vtkPolyDataMapper2D> arrowMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
        arrowMapper->SetInputData(transformFilter->GetOutput());

        vtkSmartPointer<vtkActor2D> arrowActor = vtkSmartPointer<vtkActor2D>::New();
        arrowActor->SetMapper(arrowMapper);
        arrowActor->GetProperty()->SetColor(246/255.0, 198/255.0, 89/255.0);  // Blue color

        return arrowActor;
    };

    // Create arrowheads at both ends of the spline
    int numPoints = points1->GetNumberOfPoints();
    if (numPoints >= 2) {
        // Arrowhead at the start of the spline
        // 起始箭头
        arrowActorStart = CreateArrowHead(points1, 0, 1, true);
        selectedRenderer->AddActor(arrowActorStart);

        // 结束箭头
        arrowActorEnd = CreateArrowHead(points1, numPoints - 1, numPoints - 2, true);
        selectedRenderer->AddActor(arrowActorEnd);
    }

    // Add the spline actor to the renderer
    selectedRenderer->AddActor(splineActor);
}
void HandleInteractor::removeArrowHandle() {
    if (selectedRenderer) {
        if (splineActor) {
            selectedRenderer->RemoveActor(splineActor);
            splineActor = nullptr;  // 清空指针
        }
        if (arrowActorStart) {
            selectedRenderer->RemoveActor(arrowActorStart);
            arrowActorStart = nullptr;
        }
        if (arrowActorEnd) {
            selectedRenderer->RemoveActor(arrowActorEnd);
            arrowActorEnd = nullptr;
        }

        // 刷新渲染器，以反映更改
        selectedRenderer->GetRenderWindow()->Render();
    }
}

void HandleInteractor::flipsync(){
    if (box != nullptr) {
        generateBox2D();
    }
    this->selectedRenderer->GetRenderWindow()->Render();
}

bool HandleInteractor::isHaveBox(){
    return box;
}
