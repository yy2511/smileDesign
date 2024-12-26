#include "smilelinestyle.h"

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
#include<vtkLineSource.h>
#include<vtkTransform.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkSmartPointer.h>
#include<vtkCamera.h>
vtkStandardNewMacro(SmileLineStyle);

SmileLineStyle::SmileLineStyle() {
    this->ScreenOfSmilePoints = vtkSmartPointer<vtkPoints>::New();
    ScreenOfStraightPoints = vtkSmartPointer<vtkPoints>::New();
    ScreenOfToothPoints = vtkSmartPointer<vtkPoints>::New();
    resultActor = vtkSmartPointer<vtkActor>::New();
    PointSmileActors = {};
    selectedHandle = vtkSmartPointer<vtkActor>::New();
    selectedHandle = nullptr;

}
SmileLineStyle::~SmileLineStyle(){

}
void SmileLineStyle::SetRenderer(vtkRenderer* renderer) {
    this->Renderer = renderer;
}

void SmileLineStyle::OnLeftButtonDown()
{
    isLeftDown = true;
    int* clickPos = this->GetInteractor()->GetEventPosition();



        cout<<"clickPos:"<<clickPos[0]<<" "<<clickPos[1]<<endl;

        if (!PointSmileActors.empty()) {
            for (size_t i = 0; i < PointSmileActors.size(); ++i) {
                // 获取 Actor2D 的位置
                double worldPosition[3];
                PointSmileActors[i]->GetPosition(worldPosition);
                // 将世界坐标转换为屏幕坐标
                vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
                coordinate->SetCoordinateSystemToWorld();
                coordinate->SetValue(worldPosition);
                double* displayPosition = coordinate->GetComputedDoubleDisplayValue(Renderer);  // 屏幕坐标

                std::cout << "Screen pos: " << displayPosition[0] << " " << displayPosition[1] << std::endl;

                // 计算鼠标点击位置与当前 PointActor 的屏幕坐标的距离
                double distance = sqrt(pow(clickPos[0] - displayPosition[0], 2) + pow(clickPos[1] - displayPosition[1], 2));
                if (distance < 15) {
                    selectedHandle = PointSmileActors[i];
                    handleID = i;
                    cout<<"handleIDq"<<handleID<<endl;
                    vtkSmartPointer<vtkRegularPolygonSource> polygonSource = vtkRegularPolygonSource::SafeDownCast(selectedHandle->GetMapper()->GetInputConnection(0, 0)->GetProducer());
                    if (polygonSource) {
                        //polygonSource->SetRadius(15.0);  // 放大手柄的半径
                        selectedHandle->GetMapper()->Update();
                    }
                    selectedHandle->GetProperty()->SetColor(0.235, 0.7, 0.443);

                    this->Renderer->GetRenderWindow()->Render();
                    isTeethHandle = false;
                    return;
                }
            }
            for(int i =0 ; i<teethHandle.size();i++){
                double worldPosition[3];
                teethHandle[i]->GetPosition(worldPosition);
                // 将世界坐标转换为屏幕坐标
                vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();
                coordinate->SetCoordinateSystemToWorld();
                coordinate->SetValue(worldPosition);
                double* displayPosition = coordinate->GetComputedDoubleDisplayValue(Renderer);  // 屏幕坐标

                std::cout << "Screen pos: " << displayPosition[0] << " " << displayPosition[1] << std::endl;

                // 计算鼠标点击位置与当前 PointActor 的屏幕坐标的距离
                double distance = sqrt(pow(clickPos[0] - displayPosition[0], 2) + pow(clickPos[1] - displayPosition[1], 2));
                if (distance < 15) {
                    selectedHandle = teethHandle[i];
                    handleID = i;
                    cout<<"handleIDq"<<handleID<<endl;
                    vtkSmartPointer<vtkRegularPolygonSource> polygonSource = vtkRegularPolygonSource::SafeDownCast(selectedHandle->GetMapper()->GetInputConnection(0, 0)->GetProducer());
                    if (polygonSource) {
                        //polygonSource->SetRadius(15.0);  // 放大手柄的半径
                        selectedHandle->GetMapper()->Update();
                    }
                    selectedHandle->GetProperty()->SetColor(0.235, 0.7, 0.443);

                    this->Renderer->GetRenderWindow()->Render();
                    isTeethHandle = true;
                    return;
                }
            }
        }
    // 继续父类的处理逻辑
    //vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

}

void SmileLineStyle::OnLeftButtonUp() {
    isLeftDown = false;
    if (selectedHandle != nullptr) {
        selectedHandle->GetProperty()->SetColor(1,0,0);
        Renderer->GetRenderWindow()->Render();
        selectedHandle = nullptr;
    }
}

void SmileLineStyle::OnMouseMove(){

    if (isLeftDown && selectedHandle != nullptr&&isHasLine) {


        int* mousePos = this->GetInteractor()->GetEventPosition();

        double* pickedPos = ClickToPick(mousePos);

        pickedPos[2] = zmax;



        if(isTeethHandle){

            switch(handleID){
            case 0:
            {   double currpos[3];
                teethHandle[handleID]->GetPosition(currpos);


                pickedPos[1] = currpos[1];
                teethHandle[handleID]->SetPosition(pickedPos);

                double central[3];
                ScreenOfToothPoints->GetPoint(0,central);

                double currwidth = pickedPos[0]-central[0];
                cout<<"currwidth:"<<currwidth<<endl;
                double p1[3];
                double p2[3];



                for(int i =0 ; i<=12;i+=2){
                    ScreenOfToothPoints->GetPoint(i,p1);
                    ScreenOfToothPoints->GetPoint(i+1,p2);
                    p1[0] = central[0]+currwidth*percentOfPoint[i];
                    p2[0] = p1[0];
                    ScreenOfToothPoints->SetPoint(i,p1);
                    ScreenOfToothPoints->SetPoint(i+1,p2);

                }
                ScreenOfToothPoints->GetPoint(14,p1);
                ScreenOfToothPoints->GetPoint(15,p2);
                p1[0] = central[0]+currwidth*percentOfPoint[14];
                p2[0] = central[0]-currwidth*percentOfPoint[14];
                ScreenOfToothPoints->SetPoint(14,p1);
                ScreenOfToothPoints->SetPoint(15,p2);
                gennerateFromPoints();

                break;
            }
            case 1:
            {
                teethHandle[handleID]->SetPosition(pickedPos);
                int * temp = WorldToDis(pickedPos);
                centralPosition[0] = temp[0];
                centralPosition[1] = temp[1];

                double translation[3];
                translation[0] = pickedPos[0] - cpos[0];
                translation[1] = pickedPos[1] - cpos[1];
                translation[2] = pickedPos[2] - cpos[2];

                cpos[0] = pickedPos[0];
                cpos[1] = pickedPos[1];
                cpos[2] = pickedPos[2];

                updateToothLine(translation);

                break;
                }
            }


        }else{
            PointSmileActors[handleID]->SetPosition(pickedPos);

            ScreenOfSmilePoints->SetPoint(handleID, pickedPos);

            ScreenOfSmilePoints->Modified();

            gennerateSmileLine();
        }

        Renderer->GetRenderWindow()->Render();
    }

    vtkInteractorStyleTrackballCamera::OnMouseMove();

}

void SmileLineStyle::OnRightButtonDown(){
    //removePoint();
    isvisible = !isvisible;
    int visible = isvisible;
    for(auto& point : PointSmileActors){
        point->SetVisibility(visible);
    }

    if(resultActor){
        resultActor->SetVisibility(visible);
    }
    for(auto& i : teethHandle){
        i->SetVisibility(visible);
    }

    for(auto& i : lineActors){
        i->SetVisibility(visible);
    }

    Renderer->GetRenderWindow()->Render();

}

void SmileLineStyle::OnMouseWheelForward(){
    vtkInteractorStyleTrackballCamera::OnMouseWheelForward();

}
void SmileLineStyle::OnMouseWheelBackward(){
    vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();

}

void SmileLineStyle::gennerateSmileLine(){
    if(resultActor){
        Renderer->RemoveActor(resultActor);
    }
    vtkNew<vtkParametricSpline> spline;
    spline->SetPoints(ScreenOfSmilePoints);
    cout<<ScreenOfSmilePoints->GetNumberOfPoints()<<endl;
    vtkParametricFunctionSource* functionSource =
        vtkParametricFunctionSource::New();
    functionSource->SetParametricFunction(spline);
    functionSource->SetUResolution(50 * ScreenOfSmilePoints->GetNumberOfPoints());
    functionSource->Update();
    vtkPolyData* out = functionSource->GetOutput();
    std::cout << "Number of points in the output: " << out->GetNumberOfPoints() << std::endl;
    vtkNew<vtkPolyDataMapper> resultMapper;
    resultMapper->SetInputConnection(functionSource->GetOutputPort());


    resultActor->SetMapper(resultMapper);
    resultActor->GetProperty()->SetLineWidth(2.0);  // 设置线宽为2
    resultActor->GetProperty()->SetColor(1.0, 1.0, 0.0);  // 将曲线设置为黄色

    this->Renderer->AddActor(resultActor);
    this->Renderer->GetRenderWindow()->Render();
    isHasLine = true;


}


double* SmileLineStyle::ClickToPick(int* pos) {

    vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();

    // 设置输入的屏幕坐标系
    coordinate->SetCoordinateSystemToDisplay();
    coordinate->SetValue(pos[0], pos[1], 0);  // 设置屏幕坐标，Z设置为0

    // 将屏幕坐标转换为世界坐标
    double* worldPos = coordinate->GetComputedWorldValue(Renderer);
    worldPos[2] = zmax;

    // 返回世界坐标
    return worldPos;
}

int* SmileLineStyle::WorldToDis(double* world){
    vtkSmartPointer<vtkCoordinate> coordinate = vtkSmartPointer<vtkCoordinate>::New();

    // 设置为世界坐标
    coordinate->SetCoordinateSystemToWorld();
    coordinate->SetValue(world);

    // 将世界坐标转换为屏幕坐标
    int* screenPos = coordinate->GetComputedDisplayValue(Renderer);
    return screenPos;
}

vtkSmartPointer<vtkActor> SmileLineStyle:: CreateCircleActor(double *center)
{
    // 创建一个近似圆形的多边形源
    vtkSmartPointer<vtkRegularPolygonSource> circleSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
    circleSource->SetCenter(0,0,0);       // 设置圆心位置
    circleSource->SetRadius(1.0);       // 设置圆的半径
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
void SmileLineStyle::removePoint(){
    cout<<"removePoint"<<endl;
    for(auto& point : PointSmileActors){
        Renderer->RemoveActor(point);
    }
    PointSmileActors.clear();
    if(resultActor){
        Renderer->RemoveActor(resultActor);
    }
    for(auto& i : teethHandle){
        Renderer->RemoveActor(i);
    }
    teethHandle.clear();
    for(auto& i : lineActors){
        Renderer->RemoveActor(i);
    }
    lineActors.clear();
    Renderer->GetRenderWindow()->Render();
}

void SmileLineStyle::autoAddPoint(){

    for(auto& point : PointSmileActors){
        point->SetVisibility(1);
    }

    if(resultActor){
        resultActor->SetVisibility(1);
    }
    for(auto& i : teethHandle){
        i->SetVisibility(1);
    }

    for(auto& i : lineActors){
        i->SetVisibility(1);
    }
    isvisible = true;
    ScreenOfSmilePoints->Reset();


    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor predictor;

    try {
        dlib::deserialize("./Config/shape_predictor_68_face_landmarks.dat") >> predictor;
        cout<<"model loaded"<<std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading shape predictor: " << e.what() << std::endl;
        return ;
    }
    // Capture the window as an image
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(Renderer->GetRenderWindow());
    windowToImageFilter->Update();

    // Write the image to a PNG file
    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName("screenshot_1.png");
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();
    std::ifstream file("screenshot_1.png", std::ios::binary);
    std::vector<uchar> buffer(std::istreambuf_iterator<char>(file), {});

    // 使用 OpenCV 将字节数组解码为 Mat 对象
    cv::Mat img = cv::imdecode(buffer, cv::IMREAD_COLOR);

    if (img.empty())
    {
        std::cerr << "Unable to load image!" << std::endl;
        return ;
    } else {
        qDebug() << "Shape image loaded successfully." << endl;
    }

    dlib::cv_image<dlib::bgr_pixel> cimg(img);

    // 检测人脸
    std::vector<dlib::rectangle> faces = detector(cimg);

    int img_height = img.rows; // Get the height of the image
    // 对每张检测到的人脸提取68个关键点

    for (auto& face : faces)
    {
        dlib::full_object_detection shape = predictor(cimg, face);
        std::cout << "Number of parts: " << shape.num_parts() << std::endl;
        int pos[2];
        //picker获取面部actor的zmax
        cv::Point point(shape.part(28).x(), shape.part(28).y());
        cv::circle(img, point, 2, cv::Scalar(0, 255, 0), -1);
        // Adjust y-coordinate for VTK
        int vtk_y = img_height - point.y;
        pos[0] = point.x;
        pos[1] = vtk_y;
        vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
        picker->SetTolerance(0.0005);  // 设置拾取容差
        picker->Pick(pos[0], pos[1], 0, Renderer);

        vtkActor* pickedActor = picker->GetActor();  // 获取被选中的actor

        if (pickedActor != nullptr)
        {
            std::cout << "Picked actor!" << std::endl;
            double bounds[6];
            pickedActor->GetBounds(bounds);
            std::cout << "Bounding box: "
                      << "xmin: " << bounds[0] << ", xmax: " << bounds[1] << ", "
                      << "ymin: " << bounds[2] << ", ymax: " << bounds[3] << ", "
                      << "zmin: " << bounds[4] << ", zmax: " << bounds[5] << std::endl;
            zmax = bounds[5];  // 获取 zmax
        }
        if(PointSmileActors.size()>0){
            for(int i = 0; i < PointSmileActors.size(); i++){
                Renderer->RemoveActor(PointSmileActors[i]);
            }
        }
        PointSmileActors.clear();

        point.x=shape.part(60).x();
        point.y=shape.part(60).y();

        cv::circle(img, point, 2, cv::Scalar(0, 255, 0), -1);
        // Adjust y-coordinate for VTK
        vtk_y = img_height - point.y;
        pos[0] = point.x;
        pos[1] = vtk_y;
        ScreenOfSmilePoints->InsertNextPoint(ClickToPick(pos));



        vtkSmartPointer<vtkActor> circleActor = CreateCircleActor(ClickToPick(pos));
        Renderer->AddActor(circleActor);
        PointSmileActors.push_back(circleActor);

        point.x=shape.part(66).x();
        point.y=shape.part(66).y();

        cv::circle(img, point, 2, cv::Scalar(0, 255, 0), -1);
        // Adjust y-coordinate for VTK
        vtk_y = img_height - point.y;

        pos[0] = point.x;
        pos[1] = vtk_y;

        centralPosition[0] = pos[0];
        centralPosition[1] = pos[1]-10;
        cout<<"pos"<<pos[0]<<" "<<pos[1]<<endl;

        double* t1 = ClickToPick(pos);

        double x1[3];
        x1[0] = t1[0];
        x1[1] = t1[1];
        x1[2] = t1[0];
        cout<<"t1="<<t1[0]<<" "<<t1[1]<<" "<<t1[2]<<endl;
        point.x=shape.part(65).x();
        point.y=shape.part(65).y();
        vtk_y = img_height - point.y;
        int pos1[2];
        pos1[0] = point.x;
        pos1[1] = vtk_y;
        cout<<"pos1"<<pos1[0]<<" "<<pos1[1]<<endl;
        double* temp1 = ClickToPick(pos1);
        double x2[3];
        x2[0] = temp1[0];
        x2[1] = temp1[1];
        x2[2] = temp1[0];

        cout<<"t1="<<t1[0]<<" "<<t1[1]<<" "<<t1[2]<<endl;
        cout<<"temp1="<<temp1[0]<<" "<<temp1[1]<<" "<<temp1[2]<<endl;
        width = x1[0]-x2[0];
        cout<<"width="<<width<<endl;

        ScreenOfSmilePoints->InsertNextPoint(ClickToPick(pos));

        circleActor = CreateCircleActor(ClickToPick(pos));
        Renderer->AddActor(circleActor);
        PointSmileActors.push_back(circleActor);

        point.x=shape.part(64).x();
        point.y=shape.part(64).y();

        cv::circle(img, point, 2, cv::Scalar(0, 255, 0), -1);
        // Adjust y-coordinate for VTK
        vtk_y = img_height - point.y;

        pos[0] = point.x;
        pos[1] = vtk_y;
        ScreenOfSmilePoints->InsertNextPoint(ClickToPick(pos));
        circleActor = CreateCircleActor(ClickToPick(pos));
        Renderer->AddActor(circleActor);
        PointSmileActors.push_back(circleActor);




        gennerateToothLine();
        gennerateSmileLine();
        gennerateStraightLine();
        this->Renderer->GetRenderWindow()->Render();
    }
}

void SmileLineStyle::gennerateStraightLine(){
    ScreenOfStraightPoints->Reset();
    double p1[3];
    ScreenOfStraightPoints->GetPoint(0,p1);

    p1[2]=zmax;
    cout<<"line P1:"<<p1[0]<<" "<<p1[1]<<" "<<p1[2]<<endl;
    double p2[3];
    ScreenOfStraightPoints->GetPoint(1,p2);
    p2[2]=zmax;
    cout<<"line P2:"<<p2[0]<<" "<<p2[1]<<" "<<p2[2]<<endl;
    makeStraightLine(p1,p2);

    ScreenOfStraightPoints->GetPoint(2,p1);
    cout<<"line P1:"<<p1[0]<<" "<<p1[1]<<" "<<p1[2]<<endl;
    p1[2]=zmax;

    ScreenOfStraightPoints->GetPoint(3,p2);
    p2[2]=zmax;
    makeStraightLine(p1,p2);

}
void SmileLineStyle::setStraightPoints(int* pos_int ){


    cout<<333<<endl;
    cout<<"ScreenOfStraightPoints1"<<endl;
    ScreenOfStraightPoints->InsertNextPoint(ClickToPick(pos_int));
    cout<<"ScreenOfStraightPoints2"<<endl;
}

bool SmileLineStyle::getHasLine(){
    return isHasLine;
}
void SmileLineStyle::gennerateToothLine(){

    cout<<"gennerateToothLine0::"<<"ScreenOfToothPoints"<<ScreenOfToothPoints->GetNumberOfPoints()<<endl;
    if(toothLines.size()>0){
        for(int i =0; i< toothLines.size();i++){
            Renderer->RemoveActor(toothLines[i]);
        }
        toothLines.clear();
    }
    if(teethHandle.size()>0){
        for(int i =0; i< teethHandle.size();i++){
            Renderer->RemoveActor(teethHandle[i]);
        }
        teethHandle.clear();
    }
    this->ScreenOfToothPoints->Reset();

    cout<<"gennerateToothLine::"<<"ScreenOfToothPoints"<<ScreenOfToothPoints->GetNumberOfPoints()<<endl;

    int p1[3];
    int p2[3];
    int p3[2];
    double temp1[3];
    double temp2[3];
    double* centralp = ClickToPick(centralPosition);
    cpos[0] = centralp[0];
    cpos[1] = centralp[1];
    cpos[2] = centralp[2];
    vtkSmartPointer<vtkActor> line = vtkSmartPointer<vtkActor>::New();
    temp1[0] = centralp[0];
    temp2[0] = centralp[0];
    temp1[1] = centralp[1]-28.50;
    temp2[1] = centralp[1]+28.50;
    temp1[2] = centralp[2];
    temp2[2] = centralp[2];
    // p2[0] = centralPosition[0];
    // p2[1] = centralPosition[1]+70;

    line = makeStraightLine(temp1,temp2);
    cout<<"temp1 = "<<temp1[0]<<" "<<temp1[1]<<" "<<temp1[2]<<endl;
    cout<<"temp2 = "<<temp2[0]<<" "<<temp2[1]<<" "<<temp2[2]<<endl;
    ScreenOfToothPoints->InsertNextPoint(temp1);
    ScreenOfToothPoints->InsertNextPoint(temp2);
    cout<<endl<<endl;
    toothLines.push_back(line);



    p1[0] = centralPosition[0]-width;
    p1[1] = centralPosition[1]-60;

    p2[0] = centralPosition[0]-width;
    p2[1] = centralPosition[1]+50;

    temp1[0] = centralp[0]-width;
    temp2[0] = centralp[0]-width;
    temp1[1] = centralp[1]-20.0;
    temp2[1] = centralp[1]+20.0;
    temp1[2] = centralp[2];
    temp2[2] = centralp[2];

    line = makeStraightLine(temp1,temp2);
    cout<<"temp1 = "<<temp1[0]<<" "<<temp1[1]<<" "<<temp1[2]<<endl;
    cout<<"temp2 = "<<temp2[0]<<" "<<temp2[1]<<" "<<temp2[2]<<endl;
    ScreenOfToothPoints->InsertNextPoint(temp1);
    ScreenOfToothPoints->InsertNextPoint(temp2);


    toothLines.push_back(line);//左侧那条线

    p1[0] = centralPosition[0]+width;
    p1[1] = centralPosition[1]-50;

    p2[0] = centralPosition[0]+width;
    p2[1] = centralPosition[1]+50;

    temp1[0] = centralp[0]+width;
    temp2[0] = centralp[0]+width;
    temp1[1] = centralp[1]-20.0;
    temp2[1] = centralp[1]+20.0;
    temp1[2] = centralp[2];
    temp2[2] = centralp[2];

    line = makeStraightLine(temp1,temp2);
    cout<<"temp1 = "<<temp1[0]<<" "<<temp1[1]<<" "<<temp1[2]<<endl;
    cout<<"temp2 = "<<temp2[0]<<" "<<temp2[1]<<" "<<temp2[2]<<endl;
    ScreenOfToothPoints->InsertNextPoint(temp1);
    ScreenOfToothPoints->InsertNextPoint(temp2);


    toothLines.push_back(line);//右侧那条线


    /*------------------------------------------------*/
    double width1 = width+width*factor;
    p1[0] = centralPosition[0]-width1;
    p1[1] = centralPosition[1]-50;

    p2[0] = centralPosition[0]-width1;
    p2[1] = centralPosition[1]+50;

    temp1[0] = centralp[0]-width1;
    temp2[0] = centralp[0]-width1;
    temp1[1] = centralp[1]-20.0;
    temp2[1] = centralp[1]+20.0;
    temp1[2] = centralp[2];
    temp2[2] = centralp[2];

    line = makeStraightLine(temp1,temp2);
    cout<<"temp1 = "<<temp1[0]<<" "<<temp1[1]<<" "<<temp1[2]<<endl;
    cout<<"temp2 = "<<temp2[0]<<" "<<temp2[1]<<" "<<temp2[2]<<endl;
    ScreenOfToothPoints->InsertNextPoint(temp1);
    ScreenOfToothPoints->InsertNextPoint(temp2);

    toothLines.push_back(line);//左侧那条线

    p1[0] = centralPosition[0]+width1;
    p1[1] = centralPosition[1]-50;

    p2[0] = centralPosition[0]+width1;
    p2[1] = centralPosition[1]+50;

    temp1[0] = centralp[0]+width1;
    temp2[0] = centralp[0]+width1;
    temp1[1] = centralp[1]-20.0;
    temp2[1] = centralp[1]+20.0;
    temp1[2] = centralp[2];
    temp2[2] = centralp[2];

    line = makeStraightLine(temp1,temp2);
    cout<<"temp1 = "<<temp1[0]<<" "<<temp1[1]<<" "<<temp1[2]<<endl;
    cout<<"temp2 = "<<temp2[0]<<" "<<temp2[1]<<" "<<temp2[2]<<endl;
    ScreenOfToothPoints->InsertNextPoint(temp1);
    ScreenOfToothPoints->InsertNextPoint(temp2);

    toothLines.push_back(line);//右侧那条线

    /*------------------------------------------------*/

    double width2 = width+width*factor+width*factor*factor;
    p1[0] = centralPosition[0]-width2;
    p1[1] = centralPosition[1]-50;

    p2[0] = centralPosition[0]-width2;
    p2[1] = centralPosition[1]+50;

    temp1[0] = centralp[0]-width2;
    temp2[0] = centralp[0]-width2;
    temp1[1] = centralp[1]-20.0;
    temp2[1] = centralp[1]+20.0;
    temp1[2] = centralp[2];
    temp2[2] = centralp[2];

    line = makeStraightLine(temp1,temp2);
    cout<<"temp1 = "<<temp1[0]<<" "<<temp1[1]<<" "<<temp1[2]<<endl;
    cout<<"temp2 = "<<temp2[0]<<" "<<temp2[1]<<" "<<temp2[2]<<endl;
    ScreenOfToothPoints->InsertNextPoint(temp1);
    ScreenOfToothPoints->InsertNextPoint(temp2);
    toothLines.push_back(line);//左侧那条线

    p1[0] = centralPosition[0]+width2;
    p1[1] = centralPosition[1]-50;

    p2[0] = centralPosition[0]+width2;
    p2[1] = centralPosition[1]+50;

    temp1[0] = centralp[0]+width2;
    temp2[0] = centralp[0]+width2;
    temp1[1] = centralp[1]-20.0;
    temp2[1] = centralp[1]+20.0;
    temp1[2] = centralp[2];
    temp2[2] = centralp[2];

    line = makeStraightLine(temp1,temp2);
    cout<<"temp1 = "<<temp1[0]<<" "<<temp1[1]<<" "<<temp1[2]<<endl;
    cout<<"temp2 = "<<temp2[0]<<" "<<temp2[1]<<" "<<temp2[2]<<endl;
    ScreenOfToothPoints->InsertNextPoint(temp1);
    ScreenOfToothPoints->InsertNextPoint(temp2);

    toothLines.push_back(line);//右侧那条线


    p1[0] = centralPosition[0]+width2 +width;
    p1[1] = centralPosition[1];

    p2[0] = centralPosition[0]-width2 -width;
    p2[1] = centralPosition[1];

    temp1[0] = centralp[0]+width2 +width;
    temp2[0] = centralp[0]-width2 -width;
    temp1[1] = centralp[1];
    temp2[1] = centralp[1];
    temp1[2] = centralp[2];
    temp2[2] = centralp[2];
    cout<<"temp1 = "<<temp1[0]<<" "<<temp1[1]<<" "<<temp1[2]<<endl;
    cout<<"temp2 = "<<temp2[0]<<" "<<temp2[1]<<" "<<temp2[2]<<endl;
    ScreenOfToothPoints->InsertNextPoint(temp1);//左端
    ScreenOfToothPoints->InsertNextPoint(temp2);//右端

    line = makeStraightLine(temp1,temp2);

    cout<<endl<<endl;
    toothLines.push_back(line);//横着的那条线

    int pos[2];
    pos[0] = p2[0];
    pos[1] = p2[1];
    vtkSmartPointer<vtkActor> t = CreateCircleActor(temp1);
    teethHandle.push_back(t);
    Renderer->AddActor(t);

    // pos[0] = centralPosition[0];
    // pos[1] = centralPosition[1];
    double cent[3];
    ScreenOfToothPoints->GetPoint(0,cent);
    double cent1[3];
    ScreenOfToothPoints->GetPoint(1,cent1);
    cent[1] = (cent[1]+cent1[1])/2.0;

    t = CreateCircleActor(centralp);
    teethHandle.push_back(t);
    Renderer->AddActor(t);

    double right[3];
    ScreenOfToothPoints->GetPoint(14,right);
    double mid[3];
    ScreenOfToothPoints->GetPoint(0,mid);
    initWidth = right[0]-mid[0];
    cout<<"initWidth = "<<initWidth<<endl;
    for(int i = 0;i <=14;i+=2){
        double p[3];
        ScreenOfToothPoints->GetPoint(i,p);
        percentOfPoint[i] = (p[0]-mid[0])/initWidth;
    }

    for(auto i : percentOfPoint){
        cout<<"no."<<i.first<<" "<<i.second<<endl;
    }



    for(vtkIdType i = 0;i<ScreenOfToothPoints->GetNumberOfPoints();i++){
        double p[3];
        ScreenOfToothPoints->GetPoint(i,p);
        cout<<"第"<<i<<"个点："<<p[0]<<" "<<p[1]<<" "<<p[2]<<endl;
    }
    this->Renderer->GetRenderWindow()->Render();
}
vtkSmartPointer<vtkActor>SmileLineStyle:: makeStraightLine(double*p1, double*p2){
    vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
    lineSource->SetPoint1(p1);
    lineSource->SetPoint2(p2);
    lineSource->Update();

    // 创建映射器 (mapper)
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(lineSource->GetOutputPort());

    // 创建演员 (actor) 并设置映射器
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1, 1, 1);  // 设置线条颜色（红色）
    actor->GetProperty()->SetOpacity(0.6);
    actor->GetProperty()->SetLineWidth(2.5);  // 设置线条宽度
    lineActors.push_back(actor);
    Renderer->AddActor(actor);
    //Renderer->GetRenderWindow()->Render();
    return actor;


}
void SmileLineStyle::updateToothLine(double* translation){
    cout<<"translation:"<<translation[0]<<" "<<translation[1]<<" "<<translation[2]<<endl;
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(translation);

    cout<<"updateToothLine::"<<"ScreenOfToothPoints"<<ScreenOfToothPoints->GetNumberOfPoints()<<endl;
    for(vtkIdType i = 0; i < ScreenOfToothPoints->GetNumberOfPoints(); i++){
        double currentPosition[3];
        ScreenOfToothPoints->GetPoint(i,currentPosition);
        double newPosition[3];
        newPosition[0] = currentPosition[0] + transform->GetPosition()[0];
        newPosition[1] = currentPosition[1] + transform->GetPosition()[1];
        newPosition[2] = currentPosition[2] + transform->GetPosition()[2];
        ScreenOfToothPoints->SetPoint(i,newPosition);
    }
    gennerateFromPoints();
    if(teethHandle.size()>0){
        for(int i =0; i< teethHandle.size()-1;i++){

                // 获取当前的 actor 位置
                double currentPosition[3];
                teethHandle[i]->GetPosition(currentPosition);

                // 根据变换计算新的位置
                double newPosition[3];
                newPosition[0] = currentPosition[0] + transform->GetPosition()[0];
                newPosition[1] = currentPosition[1] + transform->GetPosition()[1];
                newPosition[2] = currentPosition[2] + transform->GetPosition()[2];

                // 设置新的位置
                teethHandle[i]->SetPosition(newPosition);

        }

    }

}

void SmileLineStyle::gennerateFromPoints(){
    double temp[3];
    double temp1[3];
    if(toothLines.size()>0){
        for(int i =0; i< toothLines.size();i++){
            Renderer->RemoveActor(toothLines[i]);
        }
        toothLines.clear();
    }


    vtkSmartPointer<vtkActor> line = vtkSmartPointer<vtkActor>::New();


    for(vtkIdType i = 0; i < ScreenOfToothPoints->GetNumberOfPoints();i+=2){

        ScreenOfToothPoints->GetPoint(i,temp);
        ScreenOfToothPoints->GetPoint(i+1,temp1);
        line = makeStraightLine(temp,temp1);
        toothLines.push_back(line);
    }

    this->Renderer->GetRenderWindow()->Render();
}
